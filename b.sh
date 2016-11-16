#!/bin/bash

mv bin bin_bkup
mkdir bin

cd serial/
./build.sh
./build_tour.sh
cd -

cd mpi/
./build_mpi.sh
cd -

bin/tsp 15_city.txt 15
bin/tour_tsp 15_city.txt 15
for i in 1 2 4 6 8 12 15
do
	echo "Running with ${i} processes"
	mpiexec -n ${i} bin/tsp_mpi book_15_city.txt
done
