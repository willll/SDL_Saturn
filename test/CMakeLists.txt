cmake_minimum_required( VERSION 3.0 )

set( SEGALIB $ENV{SATURN_SBL}/segalib )
set( SEGASGL $ENV{SATURN_SGL} )

set( SATURN_COMMON $ENV{SATURN_COMMON} )
set( SATURN_CMAKE $ENV{SATURN_CMAKE} )

set( SDL_LIB /saturn/noiz2sa-0.51a-saturn/SDL )

project( test VERSION 1.0 )

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/helloworld )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testver )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testtypes )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testtimer )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testcdrom )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testkeys )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testwin )
