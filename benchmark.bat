@echo off
REM Script de benchmark complet

echo ========================================
echo BENCHMARK COMPLET - Multiplication Matrice-Vecteur
echo ========================================
echo.

set SIZE=2000

echo Test 1: Version sequentielle
echo ----------------------------
matmul_sequential.exe %SIZE% %SIZE%
echo.
echo.

echo Test 2: MPI avec 1 processus
echo ----------------------------
mpiexec -n 1 matmul_mpi.exe %SIZE% %SIZE%
echo.
echo.

echo Test 3: MPI avec 2 processus
echo ----------------------------
mpiexec -n 2 matmul_mpi.exe %SIZE% %SIZE%
echo.
echo.

echo Test 4: MPI avec 4 processus
echo ----------------------------
mpiexec -n 4 matmul_mpi.exe %SIZE% %SIZE%
echo.
echo.

echo Test 5: Hybride - 2 proc MPI x 2 threads OpenMP
echo ------------------------------------------------
mpiexec -n 2 -env OMP_NUM_THREADS 2 matmul_hybrid.exe %SIZE% %SIZE%
echo.
echo.

echo Test 6: Hybride - 2 proc MPI x 4 threads OpenMP
echo ------------------------------------------------
mpiexec -n 2 -env OMP_NUM_THREADS 4 matmul_hybrid.exe %SIZE% %SIZE%
echo.
echo.

echo Test 7: Hybride - 4 proc MPI x 2 threads OpenMP
echo ------------------------------------------------
mpiexec -n 4 -env OMP_NUM_THREADS 2 matmul_hybrid.exe %SIZE% %SIZE%
echo.
echo.

echo ========================================
echo BENCHMARK TERMINE
echo ========================================

pause