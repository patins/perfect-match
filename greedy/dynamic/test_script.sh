#!/bin/bash
echo "TESTING DYNAMIC GREEDY for b=$1...."
echo "--------------------------"
echo "Normal (250) graph"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_250_minus_one.mtx $1 74 193 88.15390884999535
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_250.mtx -b $1
echo "--------------------------"
echo "Normal (750) graph"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_750_minus_one.mtx $1 221 734 95.78582635012981
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_750.mtx -b $1
echo "--------------------------"
echo "Normal (1000) graph"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_1000_minus_one.mtx $1 153 666 95.78582635012981
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_1000.mtx -b $1
echo "--------------------------"
echo "Normal (1500) graph"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_1500_minus_one.mtx $1 459 947 107.65687653199008
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_1500.mtx -b $1
echo "--------------------------"
echo "Normal (2000) graph"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_2000_minus_one.mtx $1 1262 1662 124.91365383951769
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_2000.mtx -b $1
echo "-------------------------"
echo "Normal (2500) graph"
echo "-------------------------"
./main ../../data/dyngreedy/lognormal_2500_minus_one.mtx $1 1719 2460 153.29653662748112
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_2500.mtx -b $1
echo "--------------------------"
echo "Normal (250) graph -- INCOMPLETE"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_250_incomplete_minus_one.mtx $1 11 223 50.71581051374501
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_250_incomplete.mtx -b $1
echo "--------------------------"
echo "Normal (750) graph -- INCOMPLETE"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_750_incomplete_minus_one.mtx $1 221 734 95.78582635012981
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_750_incomplete.mtx -b $1
echo "--------------------------"
echo "Normal (1000) graph -- INCOMPLETE"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_1000_incomplete_minus_one.mtx $1 298 787 67.99038810439797
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_1000_incomplete.mtx -b $1
echo "--------------------------"
echo "Normal (1500) graph -- INCOMPLETE"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_1500_incomplete_minus_one.mtx $1 470 1027 100.61055295008249
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_1500_incomplete.mtx -b $1
echo "--------------------------"
echo "Normal (2000) graph -- INCOMPLETE"
echo "--------------------------"
./main ../../data/dyngreedy/lognormal_2000_incomplete_minus_one.mtx $1 1262 1662 124.91365383951769
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_2000_incomplete.mtx -b $1
echo "-------------------------"
echo "Normal (2500) graph -- INCOMPLETE"
echo "-------------------------"
./main ../../data/dyngreedy/lognormal_2500_incomplete_minus_one.mtx $1 1423 1801 146.0888550771525
../../bSuitorIntel/bMatching -f ../../data/dyngreedy/lognormal_2500_incomplete.mtx -b $1
