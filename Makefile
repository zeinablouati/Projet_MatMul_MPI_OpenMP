# Makefile pour le projet de multiplication matrice-vecteur (C++)
# Usage: make sequential, make mpi, make hybrid, make all

CXX = g++
MPICXX = mpic++
CXXFLAGS = -O3 -Wall -Wextra -std=c++17
OMPFLAGS = -fopenmp

# Cibles
all: sequential

sequential: matmul_sequential.cpp
	$(CXX) $(CXXFLAGS) -o matmul_sequential matmul_sequential.cpp

mpi: matmul_mpi.cpp
	$(MPICXX) $(CXXFLAGS) -o matmul_mpi matmul_mpi.cpp

hybrid: matmul_hybrid.cpp
	$(MPICXX) $(CXXFLAGS) $(OMPFLAGS) -o matmul_hybrid matmul_hybrid.cpp

clean:
	rm -f matmul_sequential matmul_mpi matmul_hybrid *.o

# Tests rapides
test_sequential: sequential
	./matmul_sequential 1000 1000

test_mpi: mpi
	mpirun -np 4 ./matmul_mpi 1000 1000

test_hybrid: hybrid
	mpirun -np 2 ./matmul_hybrid 1000 1000

.PHONY: all clean test_sequential test_mpi test_hybrid