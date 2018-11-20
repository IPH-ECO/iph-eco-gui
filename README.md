# Description

IPH-ECO is a water quality simulation model. It's core algorithm was built in Fortran and its UI uses Qt/C++ and VTK library. The application has three main phases described below.

## Input

- Structured and unstructured mesh generation
- Hydrodynamic data
- Mesh specific data
- Water quality data
- Meteorological data

## Processing

The application uses a simulation manager UI where the user can start/resume/stop model simulations. Internally, each simulation corresponds to a thread.

## Output

After the processing phase, the user can view the generated data by model in form of:

- Charts
- Graphical report (vectorized or 3d-point)

## Setup

Install CMake and run `cmake .` on the project folder.

## Screenshots

Please check the `screenshots` folder.
