View3D
======

View3D is a C program for the computation of geometric view factors.

This is a slightly modified version of View3D 3.3, which was the last version actively developed by the original author. I am calling this version 3.3.1.  As with the original code, this version is in the public domain, and is made available without any warranty:

> This software was developed at the National Institute of Standards
> and Technology by employees of the Federal Government in the course
> of their official duties.  Pursuant to title 17 Section 105 of the
> United States Code this software is not subject to copyright
> protection and is in the public domain. These programs are
> experimental systems. NIST assumes no responsibility whatsoever for
> their use by other parties, and makes no guarantees, expressed or
> implied, about its quality, reliability, or any other
> characteristic.  We would appreciate acknowledgement if the software
> is used. This software can be redistributed and/or modified freely
> provided that any derivative works bear some notice that they are
> derived from it, and any modified versions bear some notice that
> they have been modified.

Only two changes to the program have been made to compile with GCC (on platforms other than Windows):

1. The inclusion of conio.h in misc.c has been moved inside the #if statement so that it is not included for GCC.

2. A #define macro (named STRCMPI) is used to switch between strcmpi and strcasecmp depending upon the compiler.

It is likely (though I haven't tried it) that these modifications will not be sufficient to compile the code with the MINGW compiler(s).  The only other issue that I know of is that on Windows XP it will crash (or fail) in certain situations, particularly with larger input files. This appears to be a memory problem (probably in the temporary polygon
memory setup), and disabling optimization sometimes helps.

Build files for make (one for Windows system and one for non-Windows systems) and scons are included.  Your mileage may vary.

Jason DeGraw

April 26, 2011
