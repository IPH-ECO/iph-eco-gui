#!/bin/sh

# This script builds the simulation model library
rm *.a *.dylib *.o *.mod
gfortran -c *.f90
ar rc libsimulationmanager.a *.o
gfortran -shared -o libsimulationmanager.dylib domain_types.o simulation_manager.o
cp -v libsimulationmanager.dylib ../build/Debug/
