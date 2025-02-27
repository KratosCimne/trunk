#!/bin/bash
#
#  Compile the program with GFORTRAN.
#
gfortran -fopenmp SolveLS2.f liblapack.a libblas.a 
#  Compile the program with IFORT.
#
#ifort -openmp -parallel -fpp sgefa_openmp.f
#
mv a.out sgefa
#
#  Run with 1, 2, 4 and 8 threads.
#
echo "Run with 1 thread."
export OMP_NUM_THREADS=1
./sgefa 
#
#echo "Run with 2 threads."
#export OMP_NUM_THREADS=2
#./sgefa 
#
#echo "Run with 4 threads."
#export OMP_NUM_THREADS=4
#./sgefa 
#
#echo "Run with 8 threads."
#export OMP_NUM_THREADS=8
#./sgefa 
#
#  Discard the executable file.
#
rm sgefa
#
echo "Program output written to out.txt"
