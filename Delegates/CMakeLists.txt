# CMakeList.txt : CMake project for Delegates, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_subdirectory ("DelegateTemplate")
add_subdirectory ("VerySimpleDelegate")
# Add source to this project's executable.
SET(SOURCES Delegates.cpp)

# Add source to this project's executable.
add_executable (Delegates ${SOURCES})

# TODO: Add tests and install targets if needed.
