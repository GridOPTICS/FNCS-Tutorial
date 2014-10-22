#!/bin/sh

fncsbroker 2 &
./sim_power > sim_power.out &
./sim_network > sim_network.out
