#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace std::chrono;

/**
 * Classe pour la multiplication matrice-vecteur
 * Gère l'allocation, l'initialisation et le calcul
 */
class MatrixVectorMultiplier {
private:
    int rows_;
    int cols_;
    vector<double> matrix_;  // Stockage row-major
    vector<double> vector_;
    vector<double> result_;
    
public:
    /**
     * Constructeur
     */
    MatrixVectorMultiplier(int rows, int cols) 
        : rows_(rows), cols_(cols) {
        matrix_.resize(rows * cols);
        vector_.resize(cols);
        result_.resize(rows);
    }
    
    /**
     * Initialise la matrice avec des valeurs aléatoires
     */
    void initializeMatrix(unsigned int seed = 42) {
        mt19937 gen(seed);
        uniform_real_distribution<double> dist(0.0, 1.0);
        
        for (int i = 0; i < rows_ * cols_; i++) {
            matrix_[i] = dist(gen);
        }
    }
    
    /**
     * Initialise le vecteur avec des valeurs aléatoires
     */
    void initializeVector(unsigned int seed = 42) {
        mt19937 gen(seed + 1);  // Seed différent pour varier
        uniform_real_distribution<double> dist(0.0, 1.0);
        
        for (int i = 0; i < cols_; i++) {
            vector_[i] = dist(gen);
        }
    }
    
    /**
     * Effectue la multiplication matrice-vecteur
     * result = matrix * vector
     */
    void multiply() {
        for (int i = 0; i < rows_; i++) {
            result_[i] = 0.0;
            for (int j = 0; j < cols_; j++) {
                result_[i] += matrix_[i * cols_ + j] * vector_[j];
            }
        }
    }
    
    /**
     * Vérifie la correction du résultat
     */
    bool verifyResult() {
        // Recalcule le premier élément pour vérification
        double expected = 0.0;
        for (int j = 0; j < cols_; j++) {
            expected += matrix_[j] * vector_[j];
        }
        
        double error = abs(result_[0] - expected);
        
        if (error > 1e-6) {
            cout << "ERREUR: result[0] = " << result_[0] 
                 << ", expected = " << expected 
                 << ", error = " << scientific << error << endl;
            return false;
        }
        
        cout << "✓ Vérification réussie (erreur = " 
             << scientific << setprecision(2) << error << ")" << endl;
        return true;
    }
    
    /**
     * Affiche un vecteur (pour debug)
     */
    void printVector(const vector<double>& vec, const string& name, int maxPrint = 10) {
        cout << name << " = [";
        int limit = min(maxPrint, (int)vec.size());
        for (int i = 0; i < limit; i++) {
            cout << fixed << setprecision(4) << vec[i];
            if (i < limit - 1) cout << ", ";
        }
        if ((int)vec.size() > maxPrint) cout << ", ...";
        cout << "]" << endl;
    }
    
    /**
     * Affiche le vecteur résultat
     */
    void printResult(int maxPrint = 10) {
        printVector(result_, "Résultat", maxPrint);
    }
    
    /**
     * Affiche le vecteur d'entrée
     */
    void printInputVector(int maxPrint = 10) {
        printVector(vector_, "Vecteur", maxPrint);
    }
    
    /**
     * Retourne la taille mémoire utilisée en MB
     */
    double getMemoryUsageMB() {
        size_t total_bytes = (matrix_.size() + vector_.size() + result_.size()) * sizeof(double);
        return total_bytes / (1024.0 * 1024.0);
    }
    
    /**
     * Getters
     */
    int getRows() const { return rows_; }
    int getCols() const { return cols_; }
    const vector<double>& getResult() const { return result_; }
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
    // Paramètres par défaut
    int rows = 1000;
    int cols = 1000;
    
    // Lecture des arguments
    if (argc >= 2) rows = stoi(argv[1]);
    if (argc >= 3) cols = stoi(argv[2]);
    
    cout << "=== Multiplication Matrice-Vecteur Séquentielle (C++) ===" << endl;
    cout << "Taille de la matrice: " << rows << " x " << cols << endl;
    
    // Création de l'objet
    MatrixVectorMultiplier mvmul(rows, cols);
    
    cout << "Mémoire utilisée: " << fixed << setprecision(2) 
         << mvmul.getMemoryUsageMB() << " MB" << endl << endl;
    
    // Initialisation
    cout << "Initialisation des données..." << endl;
    mvmul.initializeMatrix(42);
    mvmul.initializeVector(42);
    
    // Affichage du vecteur si petit
    if (cols <= 20) {
        mvmul.printInputVector(cols);
    }
    
    // Calcul
    cout << "\nDébut du calcul..." << endl;
    Timer timer;
    timer.start();
    
    mvmul.multiply();
    
    double elapsed = timer.elapsed();
    cout << "✓ Calcul terminé!" << endl << endl;
    
    // Affichage du résultat si petit
    if (rows <= 20) {
        mvmul.printResult(rows);
    }
    
    // Vérification
    mvmul.verifyResult();
    
    // Performance
    cout << "\n=== Performance ===" << endl;
    cout << "Temps d'exécution: " << fixed << setprecision(6) 
         << elapsed << " secondes" << endl;
    
    // Calcul des GFLOPS (2*rows*cols opérations)
    double gflops = (2.0 * rows * cols) / elapsed / 1e9;
    cout << "Performance: " << fixed << setprecision(3) 
         << gflops << " GFLOPS" << endl;
    
    return 0;
}