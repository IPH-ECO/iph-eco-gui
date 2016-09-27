#!/bin/sh

# This script builds the simulation model library
rm *.a *.dylib *.o *.mod
gfortran -c domain_types.f90 simulation_manager.f90
ar rc libiph-eco-model.a *.o
gfortran -shared -o libiph-eco-model.dylib domain_types.o simulation_manager.o
cp -v libiph-eco-model.dylib ../build/Debug/
