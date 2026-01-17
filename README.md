# Projet : Multiplication Matrice-Vecteur Parallèle
## MPI + OpenMP en C++

---

## 📋 Description du projet

Implémentation parallèle de la multiplication matrice-vecteur avec trois approches :
1. **Séquentielle** : Version de référence
2. **MPI** : Parallélisation entre processus
3. **Hybride MPI + OpenMP** : Parallélisation à deux niveaux

### Objectif
Comparer les performances des différentes stratégies de parallélisation et analyser le speedup et l'efficacité.

---

## 📁 Structure du projet

```
projet/
├── matmul_sequential.cpp    # Version séquentielle
├── matmul_mpi.cpp            # Version MPI
├── matmul_hybrid.cpp         # Version hybride MPI + OpenMP
├── compile_hybrid.bat        # Script de compilation (hybride)
├── compile_mpi.bat           # Script de compilation (MPI)
├── benchmark.bat             # Script de benchmark automatique
├── collect_results.bat       # Script de collecte de résultats
├── analyze_performance.py    # Script Python pour graphiques
└── README.md                 # Ce fichier
```

---

## 🔧 Compilation

### Prérequis
- **Compilateur** : g++ (MinGW sur Windows)
- **MPI** : Microsoft MPI (MS-MPI)
- **OpenMP** : Inclus avec g++

### Compiler les programmes

```bash
# Version séquentielle
g++ -O3 -Wall -Wextra -std=c++17 -o matmul_sequential.exe matmul_sequential.cpp

# Version MPI
./compile_mpi.bat

# Version hybride (MPI + OpenMP)
./compile_hybrid.bat
```

---

## 🚀 Exécution

### Version séquentielle
```bash
./matmul_sequential.exe <rows> <cols>
# Exemple:
./matmul_sequential.exe 1000 1000
```

### Version MPI
```bash
mpiexec -n <num_processes> matmul_mpi.exe <rows> <cols>
# Exemples:
mpiexec -n 2 matmul_mpi.exe 1000 1000
mpiexec -n 4 matmul_mpi.exe 2000 2000
```

### Version hybride (MPI + OpenMP)
```bash
mpiexec -n <num_proc> -env OMP_NUM_THREADS <num_threads> matmul_hybrid.exe <rows> <cols>
# Exemples:
mpiexec -n 2 -env OMP_NUM_THREADS 2 matmul_hybrid.exe 1000 1000
mpiexec -n 2 -env OMP_NUM_THREADS 4 matmul_hybrid.exe 2000 2000
mpiexec -n 4 -env OMP_NUM_THREADS 2 matmul_hybrid.exe 5000 5000
```

---

## 📊 Benchmarks

### Lancer tous les tests automatiquement
```bash
./benchmark.bat
```

Ce script teste 7 configurations différentes avec une matrice 2000×2000.

### Collecter des résultats pour plusieurs tailles
```bash
./collect_results.bat
```

Teste les tailles : 500×500, 1000×1000, 2000×2000, 5000×5000

---

## 📈 Analyse des résultats

### Générer les graphiques
```bash
python analyze_performance.py
```

Génère deux graphiques :
- `performance_analysis.png` : Comparaison des 7 configurations
- `scalability_analysis.png` : Scalabilité selon la taille

---

## 🎯 Résultats obtenus (Matrice 2000×2000)

| Configuration | Temps (ms) | GFLOPS | Speedup | Efficacité |
|--------------|------------|--------|---------|------------|
| Séquentiel   | 4.322      | 1.851  | 1.00x   | 100%       |
| MPI 1 proc   | 4.336      | 1.845  | 1.00x   | 100%       |
| **MPI 2 proc**   | **2.545**      | **3.144**  | **1.70x**   | **85%**        |
| **MPI 4 proc**   | **2.230**      | **3.587**  | **1.94x**   | **48%**        |
| Hybrid 2×2   | 4.102      | 1.950  | 1.05x   | 26%        |
| Hybrid 2×4   | 2.785      | 2.873  | 1.55x   | 19%        |
| Hybrid 4×2   | 3.547      | 2.256  | 1.22x   | 15%        |

### 🏆 Meilleure configuration : **MPI 4 processus**
- Speedup : **1.94x**
- Performance : **3.587 GFLOPS**

---

## 💡 Observations

### Points positifs ✅
1. **MPI pur** donne les meilleures performances jusqu'à 4 processus
2. Speedup quasi-linéaire avec 2 processus (85% d'efficacité)
3. Bon speedup avec 4 processus (48% d'efficacité)
4. Vérification systématique de la correction des résultats

### Limitations ⚠️
1. Version hybride moins performante sur matrices moyennes (overhead OpenMP)
2. Scalabilité limitée au-delà de 4 processus (overhead communication)
3. Efficacité diminue avec le nombre de workers

### Pourquoi MPI > Hybride ici ?
- Matrice 2000×2000 trop petite pour justifier l'overhead OpenMP
- Création/synchronisation de threads coûteuse
- Communication MPI suffit pour cette taille

---

## 🔬 Pistes d'amélioration

1. **Tester avec des matrices plus grandes** (10000×10000, 20000×20000)
   - L'approche hybride devrait mieux performer
   
2. **Optimisations algorithmiques**
   - Cache blocking / tiling
   - Vectorisation SIMD explicite (AVX/AVX2)
   - Loop unrolling
   
3. **Optimisations MPI**
   - Recouvrement calcul/communication (MPI_Isend/Irecv)
   - Réduction du nombre de communications
   
4. **Optimisations OpenMP**
   - Ajuster le scheduling (static, dynamic, guided)
   - Affinity des threads
   - NUMA awareness

---

## 📝 Méthodologie

### Décomposition MPI
- Distribution des **lignes** de la matrice entre processus
- **Broadcast** du vecteur à tous les processus
- Calcul **local** sur chaque processus
- **Gather** des résultats sur le processus 0

### Parallélisation OpenMP
- Parallélisation de la **boucle externe** (lignes)
- Vectorisation de la **boucle interne** (produit scalaire)
- Réduction pour éviter les race conditions

### Vérification
- Recalcul du premier élément du résultat
- Comparaison avec une tolérance de 10⁻⁶

---

## 👥 Auteur
Zeineb LOUATI - M2 IA

## 📅 Date
Janvier 2025

---

## 📚 Références
- Documentation OpenMP : https://www.openmp.org/
- Documentation MPI : https://www.mpich.org/documentation/
- Microsoft MPI : https://docs.microsoft.com/en-us/message-passing-interface/microsoft-mpi
