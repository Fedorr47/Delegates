cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
file(GLOB_RECURSE GLOB_SOURCE "*.cpp")
SET(SOURCES ${GLOB_SOURCE})

file(GLOB_RECURSE GLOB_HEADER "*.h")