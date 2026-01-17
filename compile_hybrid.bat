@echo off
REM Script de compilation pour Windows avec MS-MPI + OpenMP

echo === Compilation matmul_hybrid.cpp avec MS-MPI + OpenMP ===

g++ -O3 -Wall -Wextra -std=c++17 -fopenmp ^
    -I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include" ^
    -L"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" ^
    -o matmul_hybrid.exe matmul_hybrid.cpp ^
    -lmsmpi

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✓ Compilation reussie !
    echo.
    echo Pour executer avec 2 processus MPI et 4 threads OpenMP:
    echo   set OMP_NUM_THREADS=4
    echo   mpiexec -n 2 matmul_hybrid.exe 1000 1000
    echo.
    echo Ou directement:
    echo   mpiexec -n 2 -env OMP_NUM_THREADS 4 matmul_hybrid.exe 1000 1000
) else (
    echo.
    echo X Erreur de compilation
)

pause