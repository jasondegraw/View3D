project(View3D_Exes)

cmake_minimum_required(VERSION 2.8)

set(SRCS ctrans.c
         getdat.c
         heap.c
         misc.c
         polygn.c
         readvf.c
         savevf.c
         test3d.c
         v3main.c
         view3d.c
         viewobs.c
         viewpp.c
         viewunob.c
         tmpstore.c)

add_executable(view3d ${SRCS})
