cmake_minimum_required(VERSION 2.6)

# name of your project
project(obecfp)

# set the module path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

# find and setup openbabel
find_package(OpenBabel)
if(NOT OPENBABEL_FOUND)
  message(FATAL_ERROR "Can't find Open Babel")
endif()
include_directories(${OPENBABEL_INCLUDE_DIRS} C:/Tools/openbabel/msvc-dependencies/include)

# create a list of source files (easier to maintain)
set(sources main.cpp ecfp.cpp ecfp.h)

# the executable
add_executable(genecfp ${sources})
target_link_libraries(genecfp ${OPENBABEL_LIBRARIES})
install(TARGETS genecfp DESTINATION bin)

