@echo off
REM Script de compilation pour Windows avec MS-MPI

echo === Compilation matmul_mpi.cpp avec MS-MPI ===

g++ -O3 -Wall -Wextra -std=c++17 ^
    -I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include" ^
    -L"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" ^
    -o matmul_mpi.exe matmul_mpi.cpp ^
    -lmsmpi

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✓ Compilation reussie !
    echo.
    echo Pour executer avec 4 processus:
    echo   mpiexec -n 4 matmul_mpi.exe 1000 1000
) else (
    echo.
    echo X Erreur de compilation
)

pause