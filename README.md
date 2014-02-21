View3D
======

View3D is a C program for the computation of geometric view factors.

This is View3D 4.0, a C language program for the calculation of geometric view factors. It utilizes a number of integration techniques to compute view factors between planar triangles or quadrilaterals in three dimensions. The original version of this program was developed at the National Institute of Standards and Technology by George Walton and was in the public domain. This version is released subject to the terms of the GNU GPL and is not in the public domain. Any files that have not been substantially modified are labeled as such and include the original NIST disclaimer. 

Documentation
-------------

This version is not intended to behave differently than versions 3.2 or 3.3.1. The documentation for those programs is avialable from a number of sources:

* Version 3.2 has been included with the US Department of Energy's EnergyPlus for some time. To install the program and documentation, during the main EnergyPlus installation select the optional view factor installation. EnergyPlus is available at www.energyplus.gov.
* Documentation for version 3.3.1 is available here: www.personal.psu.edu/jwd131/software/v3d33/manual/view3d33manual.html.

Installation
------------

View3D is a stand-alone executable. No installation is necessary.

Building the Program
--------------------

The program is written in portable C, and thus should compile on most systems with a C compiler. No features from newer C standards are used. Makefiles for GCC and the Visual Studio compiler are included. Presently, most development work is done on Windows with the MinGW (32 bit) compilers. To build the program

1. Download it and unpack it
2. Open a command prompt or terminal window, change directories to where you put the source, and run
    * `make` to compile with GCC
    * `nmake` to compile with the Visual Studio compiler

That's it. Standard caveats with respect to paths apply.

Running the Program
-------------------

The program is run from the command line:

    View3D inputfile.vs3 outputfile

This will produce an two files in the current directory: `outputfile` and `View3D.log`.

-- 
Jason DeGraw

February 21, 2014
