@echo off
REM Script pour collecter les résultats de benchmark et les sauvegarder

echo ========================================
echo COLLECTE DE RESULTATS - Plusieurs tailles
echo ========================================
echo.

REM Créer un fichier de résultats
set RESULTS=benchmark_results.txt
echo Benchmark Results - %date% %time% > %RESULTS%
echo ========================================== >> %RESULTS%
echo. >> %RESULTS%

REM Tester avec différentes tailles
for %%S in (500 1000 2000 5000) do (
    echo. >> %RESULTS%
    echo ===== Matrice %%S x %%S ===== >> %RESULTS%
    echo. >> %RESULTS%
    
    echo Testing size %%S x %%S...
    
    echo Sequential: >> %RESULTS%
    matmul_sequential.exe %%S %%S >> %RESULTS%
    
    echo. >> %RESULTS%
    echo MPI 2 proc: >> %RESULTS%
    mpiexec -n 2 matmul_mpi.exe %%S %%S >> %RESULTS%
    
    echo. >> %RESULTS%
    echo MPI 4 proc: >> %RESULTS%
    mpiexec -n 4 matmul_mpi.exe %%S %%S >> %RESULTS%
    
    echo. >> %RESULTS%
    echo Hybrid 2x2: >> %RESULTS%
    mpiexec -n 2 -env OMP_NUM_THREADS 2 matmul_hybrid.exe %%S %%S >> %RESULTS%
    
    echo. >> %RESULTS%
    echo ------------------------------------------ >> %RESULTS%
)

echo.
echo ========================================
echo Résultats sauvegardés dans: %RESULTS%
echo ========================================
echo.

type %RESULTS%

pause