#!/bin/bash
echo "TESTING DYNAMIC GREEDY for b=$1...."
echo "--------------------------"
echo "Normal (10) graph"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_10_minus_one.mtx $1 4 6 6.374324881586213
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_10.mtx -b $1
echo "--------------------------"
echo "Normal (25) graph"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_25_minus_one.mtx $1 11 12  47.121599812950414
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_25.mtx -b $1
echo "--------------------------"
echo "Normal (100) graph"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_100_minus_one.mtx $1 35 61 50.71581051374501
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_100.mtx -b $1
echo "--------------------------"
echo "Normal (250) graph"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_250_minus_one.mtx $1 74 193 88.15390884999535
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_250.mtx -b $1
echo "--------------------------"
echo "Normal (1000) graph"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_1000_minus_one.mtx $1 153 666 95.78582635012981
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_1000.mtx -b $1
echo "-------------------------"
echo "Normal (2500) graph"
echo "-------------------------"
./main ../../data/dyngreedy/lognormal_2500_minus_one.mtx $1 1719 2460 153.29653662748112
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_2500.mtx -b $1
echo "--------------------------"
echo "Normal (10) graph -- INCOMPLETE"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_10_incomplete_minus_one.mtx $1 0 8 2.154232968599504
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_10_incomplete.mtx -b $1
echo "--------------------------"
echo "Normal (25) graph -- INCOMPLETE"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_25_incomplete_minus_one.mtx $1 11 12 47.121599812950414
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_25_incomplete.mtx -b $1
echo "--------------------------"
echo "Normal (100) graph -- INCOMPLETE"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_100_incomplete_minus_one.mtx $1 55 78 23.38411006180524
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_100_incomplete.mtx -b $1
echo "--------------------------"
echo "Normal (250) graph -- INCOMPLETE"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_250_incomplete_minus_one.mtx $1 11 223 50.71581051374501
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_250_incomplete.mtx -b $1
echo "--------------------------"
echo "Normal (1000) graph -- INCOMPLETE"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_1000_incomplete_minus_one.mtx $1 298 787 67.99038810439797
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_1000_incomplete.mtx -b $1
echo "-------------------------"
echo "Normal (2500) graph -- INCOMPLETE"
echo "-------------------------"
./main ../../data/dyngreedy/lognormal_2500_incomplete_minus_one.mtx $1 1423 1801 146.0888550771525
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_2500_incomplete.mtx -b $1
