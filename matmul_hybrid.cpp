#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <mpi.h>
#include <omp.h>

using namespace std;
using namespace std::chrono;

/**
 * Classe pour la multiplication matrice-vecteur hybride (MPI + OpenMP)
 */
class MatrixVectorMultiplierHybrid {
private:
    int rows_;           // Nombre total de lignes
    int cols_;           // Nombre de colonnes
    int local_rows_;     // Nombre de lignes pour ce processus
    int rank_;           // Rang du processus MPI
    int size_;           // Nombre total de processus MPI
    int num_threads_;    // Nombre de threads OpenMP
    
    vector<double> local_matrix_;  // Sous-matrice locale (lignes locales)
    vector<double> vector_;        // Vecteur complet (broadcasté à tous)
    vector<double> local_result_;  // Résultat local
    vector<double> global_result_; // Résultat global (seulement sur rank 0)
    
public:
    /**
     * Constructeur
     */
    MatrixVectorMultiplierHybrid(int rows, int cols, int rank, int size) 
        : rows_(rows), cols_(cols), rank_(rank), size_(size) {
        
        // Calcul du nombre de lignes par processus
        local_rows_ = rows_ / size_;
        if (rank_ < rows_ % size_) {
            local_rows_++;
        }
        
        // Allocation mémoire
        local_matrix_.resize(local_rows_ * cols_);
        vector_.resize(cols_);
        local_result_.resize(local_rows_);
        
        if (rank_ == 0) {
            global_result_.resize(rows_);
        }
        
        // Récupérer le nombre de threads OpenMP
        #pragma omp parallel
        {
            #pragma omp master
            num_threads_ = omp_get_num_threads();
        }
    }
    
    /**
     * Initialise la matrice locale avec des valeurs aléatoires
     * Parallélisé avec OpenMP
     */
    void initializeLocalMatrix(unsigned int seed = 42) {
        // Calculer l'offset de départ pour ce processus
        int start_row = 0;
        for (int r = 0; r < rank_; r++) {
            start_row += rows_ / size_;
            if (r < rows_ % size_) {
                start_row++;
            }
        }
        
        // Chaque thread initialise sa partie
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            int nthreads = omp_get_num_threads();
            
            // Générateur aléatoire par thread (pour éviter les conflits)
            mt19937 gen(seed + tid);
            uniform_real_distribution<double> dist(0.0, 1.0);
            
            // Sauter les valeurs des processus précédents et des threads précédents
            int skip = start_row * cols_;
            int chunk_size = (local_rows_ * cols_ + nthreads - 1) / nthreads;
            skip += tid * chunk_size;
            
            for (int i = 0; i < skip; i++) {
                dist(gen);
            }
            
            // Générer les valeurs pour ce thread
            int start = tid * chunk_size;
            int end = min(start + chunk_size, local_rows_ * cols_);
            
            for (int i = start; i < end; i++) {
                local_matrix_[i] = dist(gen);
            }
        }
    }
    
    /**
     * Initialise le vecteur (seulement sur rank 0, puis broadcast)
     */
    void initializeVector(unsigned int seed = 42) {
        if (rank_ == 0) {
            mt19937 gen(seed + 1);
            uniform_real_distribution<double> dist(0.0, 1.0);
            
            for (int i = 0; i < cols_; i++) {
                vector_[i] = dist(gen);
            }
        }
        
        // Broadcast du vecteur à tous les processus
        MPI_Bcast(vector_.data(), cols_, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    
    /**
     * Effectue la multiplication locale matrice-vecteur
     * PARALLÉLISÉE avec OpenMP !
     */
    void multiplyLocal() {
        // Parallélisation de la boucle externe avec OpenMP
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < local_rows_; i++) {
            double sum = 0.0;
            
            // Boucle interne : produit scalaire
            // On peut aussi vectoriser cette boucle
            #pragma omp simd reduction(+:sum)
            for (int j = 0; j < cols_; j++) {
                sum += local_matrix_[i * cols_ + j] * vector_[j];
            }
            
            local_result_[i] = sum;
        }
    }
    
    /**
     * Rassemble les résultats de tous les processus sur rank 0
     */
    void gatherResults() {
        // Préparer les tailles et déplacements pour MPI_Gatherv
        vector<int> recvcounts;
        vector<int> displs;
        
        if (rank_ == 0) {
            recvcounts.resize(size_);
            displs.resize(size_);
            
            int offset = 0;
            for (int r = 0; r < size_; r++) {
                int local_rows = rows_ / size_;
                if (r < rows_ % size_) {
                    local_rows++;
                }
                recvcounts[r] = local_rows;
                displs[r] = offset;
                offset += local_rows;
            }
        }
        
        // Utiliser MPI_Gatherv car les processus peuvent avoir des tailles différentes
        MPI_Gatherv(local_result_.data(), local_rows_, MPI_DOUBLE,
                    global_result_.data(), recvcounts.data(), displs.data(), 
                    MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    
    /**
     * Vérifie la correction du résultat (seulement sur rank 0)
     */
    bool verifyResult() {
        if (rank_ != 0) return true;
        
        // Recalcule le premier élément pour vérification
        double expected = 0.0;
        
        // On doit recréer la première ligne de la matrice
        mt19937 gen(42);
        uniform_real_distribution<double> dist(0.0, 1.0);
        
        for (int j = 0; j < cols_; j++) {
            double matrix_val = dist(gen);
            expected += matrix_val * vector_[j];
        }
        
        double error = abs(global_result_[0] - expected);
        
        if (error > 1e-6) {
            cout << "ERREUR: result[0] = " << global_result_[0] 
                 << ", expected = " << expected 
                 << ", error = " << scientific << error << endl;
            return false;
        }
        
        cout << "✓ Vérification réussie (erreur = " 
             << scientific << setprecision(2) << error << ")" << endl;
        return true;
    }
    
    /**
     * Affiche le résultat (seulement sur rank 0)
     */
    void printResult(int maxPrint = 10) {
        if (rank_ != 0) return;
        
        cout << "Résultat = [";
        int limit = min(maxPrint, (int)global_result_.size());
        for (int i = 0; i < limit; i++) {
            cout << fixed << setprecision(4) << global_result_[i];
            if (i < limit - 1) cout << ", ";
        }
        if ((int)global_result_.size() > maxPrint) cout << ", ...";
        cout << "]" << endl;
    }
    
    /**
     * Getters
     */
    int getRows() const { return rows_; }
    int getCols() const { return cols_; }
    int getLocalRows() const { return local_rows_; }
    int getRank() const { return rank_; }
    int getSize() const { return size_; }
    int getNumThreads() const { return num_threads_; }
};

/**
 * Classe pour mesurer le temps d'exécution
 */
class Timer {
private:
    high_resolution_clock::time_point start_time_;
    
public:
    void start() {
        start_time_ = high_resolution_clock::now();
    }
    
    double elapsed() {
        auto end_time = high_resolution_clock::now();
        duration<double> diff = end_time - start_time_;
        return diff.count();
    }
};

/**
 * Fonction principale
 */
int main(int argc, char** argv) {
    // Initialisation MPI avec support thread
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Paramètres par défaut
    int rows = 1000;
    int cols = 1000;
    
    // Lecture des arguments (seulement sur rank 0)
    if (argc >= 2) rows = stoi(argv[1]);
    if (argc >= 3) cols = stoi(argv[2]);
    
    // Broadcast des paramètres à tous les processus
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Création de l'objet
    MatrixVectorMultiplierHybrid mvmul(rows, cols, rank, size);
    
    // Affichage uniquement sur rank 0
    if (rank == 0) {
        cout << "=== Multiplication Matrice-Vecteur HYBRIDE (MPI + OpenMP) ===" << endl;
        cout << "Taille de la matrice: " << rows << " x " << cols << endl;
        cout << "Nombre de processus MPI: " << size << endl;
        cout << "Nombre de threads OpenMP par processus: " << mvmul.getNumThreads() << endl;
        cout << "Parallélisme total: " << size << " x " << mvmul.getNumThreads() 
             << " = " << size * mvmul.getNumThreads() << " workers" << endl;
        cout << "Lignes par processus: ~" << rows / size << endl << endl;
    }
    
    // Initialisation
    mvmul.initializeLocalMatrix(42);
    mvmul.initializeVector(42);
    
    // Synchronisation avant le calcul
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Calcul avec mesure de temps
    Timer timer;
    timer.start();
    
    mvmul.multiplyLocal();
    mvmul.gatherResults();
    
    MPI_Barrier(MPI_COMM_WORLD);
    double elapsed = timer.elapsed();
    
    // Affichage des résultats (seulement sur rank 0)
    if (rank == 0) {
        cout << "✓ Calcul terminé!" << endl << endl;
        
        if (rows <= 20) {
            mvmul.printResult(rows);
        }
        
        // Vérification
        mvmul.verifyResult();
        
        // Performance
        cout << "\n=== Performance ===" << endl;
        cout << "Temps d'exécution: " << fixed << setprecision(6) 
             << elapsed << " secondes" << endl;
        
        // Calcul des GFLOPS
        double gflops = (2.0 * rows * cols) / elapsed / 1e9;
        cout << "Performance: " << fixed << setprecision(3) 
             << gflops << " GFLOPS" << endl;
    }
    
    // Finalisation MPI
    MPI_Finalize();
    
    return 0;
}