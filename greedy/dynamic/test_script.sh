#!/bin/bash
echo "TESTING DYNAMIC GREEDY for b=6...."
echo "--------------------------"
echo "Normal (small) graph"
echo "--------------------------"
./main ../../data/normal_small_minus_one.mtx 6 2 1 15.496714153011233
echo "--------------------------"
echo "Normal (big) graph"
echo "--------------------------"
./main ../../data/normal_big_minus_one.mtx 6 2 1 15.496714153011233
