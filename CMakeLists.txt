CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(View3D)

set(BUILD_DOCUMENTATION OFF CACHE BOOL "Set this variable to ON to build documentation")

add_subdirectory(src)

if(BUILD_DOCUMENTATION)
  find_package(LATEX)
  if(PDFLATEX_COMPILER)
    add_subdirectory(doc)
  else()
    message(WARNING "pdflatex not found, documentation will not be built.")
  endif()
endif()
