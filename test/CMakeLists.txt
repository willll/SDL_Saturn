cmake_minimum_required( VERSION 3.0 )

set( SEGALIB $ENV{SATURN_SBL}/segalib )
set( SEGASGL $ENV{SATURN_SGL} )

set( SATURN_COMMON $ENV{SATURN_COMMON} )
set( SATURN_CMAKE $ENV{SATURN_CMAKE} )

find_package(SDL_Saturn REQUIRED)

project( test VERSION 1.0 )

#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/checkkeys )          # no keyboard
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/graywin )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/helloworld )
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/loopwave )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testalpha )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testbitmap )
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testcdrom )          # TODO
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testerror )          # Requires threads
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testgamma )
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testgl )             # Requires GL
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testhread )          # Requires threads
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testjoystick )
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testkeys )           # no keyboard
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testlock )           # Requires threads
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testpalette )
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testsem )            # Requires threads
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testsprite )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testtimer )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testtypes )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testver )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testvidinfo )
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testwin )
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/testwm )
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/threadwin )         # Requires threads
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/torturethread )     # Requires threads
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/torturethread_org ) # Requires threads
