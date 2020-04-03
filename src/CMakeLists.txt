project(View3D_Exes)

cmake_minimum_required(VERSION 2.8)

if(MSVC)
  add_definitions( -D_CRT_SECURE_NO_DEPRECATE )
endif()

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
 
set(HDRS prtyp.h
         view3d.h
         tmpstore.h
         types.h)

add_executable(view3d ${HDRS} ${SRCS})

if(NOT MSVC)
    target_link_libraries(view3d m)
endif()
