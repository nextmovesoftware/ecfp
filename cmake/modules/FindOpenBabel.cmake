# FindOpenBabel.cmake
# Copyright (C) 2012-2013 NextMove Software
# Try to find Open Babel headers and libraries
# Defines:
#
#  OPENBABEL_FOUND - system has Open Babel
#  OPENBABEL_INCLUDE_DIRS - the Open Babel include directories
#  OPENBABEL_LIBRARIES - Link these to use Open Babel

if(OPENBABEL_INCLUDE_DIRS AND OPENBABEL_LIBRARIES)
  # in cache already or user-specified
  set(OPENBABEL_FOUND TRUE)

else()

  if(NOT OPENBABEL_INCLUDE_DIRS)
    if(WIN32)
      find_path(OPENBABEL_INCLUDE_DIR openbabel/obconversion.h
        PATHS
        ${OPENBABEL_DIR}\\include
        $ENV{OPENBABEL_INCLUDE_DIR}
        $ENV{OPENBABEL_INCLUDE_DIR}\\openbabel-2.0
        $ENV{OPENBABEL_INCLUDE_PATH}
        $ENV{OPENBABEL_INCLUDE_PATH}\\openbabel-2.0
        $ENV{OPENBABEL_DIR}\\include
        $ENV{OPENBABEL_PATH}\\include
        $ENV{OPENBABEL_BASE}\\include
        C:\\OpenBabel\\include
      )
      find_path(OPENBABEL_INCLUDE_DIR2 openbabel/babelconfig.h
        PATHS
        ${OPENBABEL_DIR}/build/include
        ${OPENBABEL_DIR}/windows-vc2008/build/include
        $ENV{OPENBABEL_DIR}/windows-vc2008/build/include
        )
      set(OPENBABEL_INCLUDE_DIRS ${OPENBABEL_INCLUDE_DIR} ${OPENBABEL_INCLUDE_DIR2})
    else()
      find_path(OPENBABEL_INCLUDE_DIRS openbabel/obconversion.h
        PATHS
        ${OPENBABEL_DIR}/include/openbabel-2.0
        ${OPENBABEL_DIR}/include
        $ENV{OPENBABEL_INCLUDE_DIR}/openbabel-2.0
        $ENV{OPENBABEL_INCLUDE_DIR}
        $ENV{OPENBABEL_INCLUDE_PATH}/openbabel-2.0
        $ENV{OPENBABEL_INCLUDE_PATH}
        $ENV{OPENBABEL_DIR}/include/openbabel-2.0
        $ENV{OPENBABEL_DIR}/include
        $ENV{OPENBABEL_PATH}/include/openbabel-2.0
        $ENV{OPENBABEL_PATH}/include
        $ENV{OPENBABEL_BASE}/include/openbabel-2.0
        $ENV{OPENBABEL_BASE}/include
        /usr/include/openbabel-2.0
        /usr/include
        /usr/local/include/openbabel-2.0
        /usr/local/include
        /usr/local/openbabel/include/openbabel-2.0
        /usr/local/openbabel/include
        /usr/local/openbabel-2.0/include/openbabel-2.0
        /usr/local/openbabel-2.0/include
        ~/include/openbabel-2.0
        ~/include
      )
    endif()
    if(OPENBABEL_INCLUDE_DIRS)
      message(STATUS "Found Open Babel include files at ${OPENBABEL_INCLUDE_DIRS}")
    endif()
  endif()

  if(NOT OPENBABEL_LIBRARY_DIR)
    find_library(OPENBABEL_LIBRARIES NAMES openbabel openbabel-2
      PATHS
      ${OPENBABEL_DIR}/lib
      ${OPENBABEL_DIR}/windows-vc2008/build/src/Release
      ${OPENBABEL_DIR}/build/src/Release
      $ENV{OPENBABEL_LIBRARIES}
      $ENV{OPENBABEL_DIR}/lib
      $ENV{OPENBABEL_DIR}/windows-vc2008/build/src/Release
      $ENV{OPENBABEL_PATH}/lib
      $ENV{OPENBABEL_BASE}/lib
      /usr/lib
      /usr/local/lib
      ~/lib
      $ENV{LD_LIBRARY_PATH}
    )
    if(OPENBABEL_LIBRARIES)
      message(STATUS "Found Open Babel library at ${OPENBABEL_LIBRARIES}")
    endif()
  endif()

  if(OPENBABEL_INCLUDE_DIRS AND OPENBABEL_LIBRARIES)
    set(OPENBABEL_FOUND TRUE)
  endif()

  mark_as_advanced(OPENBABEL_INCLUDE_DIRS OPENBABEL_LIBRARIES
    OPENBABEL_INCLUDE_DIR OPENBABEL_INCLUDE_DIR2)
endif()

