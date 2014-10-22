#!/bin/sh

fncsbroker 2 &
./sim_powerA > sim_powerA.out &
./sim_powerB > sim_powerB.out
