all: mpi openmp

mpi: fwalgo_mpi.c
	mpicc -o mpi fwalgo_mpi.c
openmp: fwalgo_openmp.c
	mpicc -o openmp fwalgo_openmp.c