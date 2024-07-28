cmake_minimum_required(VERSION 3.5)

set( CMAKE_SYSTEM_NAME Generic )
set( CMAKE_VERBOSE_MAKEFILE ON )

set( CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "" )
set( CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "" )
set( CMAKE_EXE_LINKER_FLAGS "" )
set( CMAKE_STATIC_LIBRARY_SUFFIX ".a" )
set( CMAKE_STATIC_LIBRARY_PREFIX "" )

if (NOT DEFINED SEGALIB)
  set( SEGALIB $ENV{SATURN_SBL}/segalib )
endif()
if (NOT DEFINED SEGASGL)
  set( SEGASGL $ENV{SATURN_SGL} )
endif()

if (NOT DEFINED SATURN_COMMON)
  set( SATURN_COMMON $ENV{SATURN_COMMON} )
endif()
if (NOT DEFINED SATURN_CMAKE)
  set( SATURN_CMAKE $ENV{SATURN_CMAKE} )
endif()
if (NOT DEFINED SATURN_ROOT)
  set( SATURN_ROOT $ENV{SATURN_ROOT} )
endif()

set( SDL_LIB ${CMAKE_CURRENT_SOURCE_DIR} )

set( SDL_DEST_DIR ${SATURN_ROOT}/SDL )

project( SDL VERSION 1.0
          DESCRIPTION "Sega Saturn SDL 1.2.5"
          LANGUAGES C )

set( as_flags )

set( c_flags
        -Os -m2
        -fno-lto
        -Wshadow
        -Wall
        -Wswitch
        -Wno-unused-variable
        -Wfatal-errors
        -fomit-frame-pointer
        -Wl,--strip-all -Wl,--verbose -mno-fsrra
        -Wpointer-sign
         )

set( SOURCES
  src/SDL.c
  src/SDL_error.c
  src/SDL_fatal.c
  src/SDL_saturn.c
  src/SDL_getenv.c
  src/SDL_log.c
  src/cdrom/SDL_cdrom.c
  src/cdrom/saturn/SDL_syscdrom.c
  src/file/SDL_rwops.c
  src/file/saturn/gfs_wrap.c
  src/video/SDL_gamma.c
  src/video/SDL_surface.c
  src/video/SDL_cursor.c
  src/video/SDL_video.c
  src/video/SDL_pixels.c
  src/video/SDL_bmp.c
  #src/video/SDL_cursor.c
  src/video/SDL_RLEaccel.c
  src/video/SDL_blit.c
  src/video/SDL_blit_0.c
  src/video/SDL_blit_1.c
  src/video/SDL_blit_N.c
  src/video/SDL_blit_A.c
  src/video/SDL_bmp.c
  src/video/saturn/SDL_satevents.c
  src/video/saturn/SDL_satmouse.c
  src/video/saturn/SDL_satvideo.c
  src/video/dummy/SDL_nullevents.c
  src/video/dummy/SDL_nullmouse.c
  src/video/dummy/SDL_nullvideo.c
  src/timer/SDL_timer.c
  src/timer/saturn/SDL_systimer.c
  src/audio/SDL_audio.c
  src/audio/SDL_wave.c
  src/audio/SDL_mixer.c
  src/audio/SDL_audiomem.c
  src/audio/SDL_audiocvt.c
  src/thread/SDL_thread.c
  src/thread/saturn/SDL_systhread.c
  src/thread/saturn/SDL_syscond.c
  src/thread/saturn/SDL_sysmutex.c
  src/thread/saturn/SDL_syssem.c
  src/events/SDL_events.c
  src/events/SDL_keyboard.c
  src/events/SDL_mouse.c
  src/events/SDL_active.c
  src/events/SDL_quit.c
  src/events/SDL_resize.c
  src/endian/SDL_endian.c
  src/joystick/SDL_joystick.c
  src/joystick/saturn/SDL_sysjoystick.c

  )

add_compile_definitions( MODEL_S
                        _SH
                        DISABLE_AUDIO
                        ENABLE_SATURN
                        #DISABLE_JOYSTICK
                        DISABLE_CDROM
                        DISABLE_THREADS
                        DISABLE_ENDIAN
                        #DISABLE_FILE
                        NO_SIGNAL_H
                        DISABLE_STDIO
                        USE_DUFFS_LOOP
                        USE_SPRITES
                        )

add_library( sdl STATIC
             ${SOURCES} )

target_include_directories(
    sdl PUBLIC
    .
    src
    src/events
    src/video
    src/cdrom
    src/joystick
    src/file
    include
    ${SEGALIB}/include
    ${SEGASGL}/include
    )

target_compile_options(sdl PUBLIC $<$<COMPILE_LANGUAGE:C>:${c_flags}> )

target_link_options(
    sdl PUBLIC
    "SHELL:-Xlinker -Map -Xlinker sdl.map"
    "SHELL:-Xlinker --strip-debug"
    "SHELL:-Xlinker -fno-lto"
    "SHELL:-m2 -nostartfiles"
    "SHELL:-T${SATURN_COMMON}/Saturn.lnk"
)

target_link_libraries( sdl PUBLIC ${SEGASGL}/lib/sglarea.o )
target_link_libraries( sdl PUBLIC ${SEGALIB}/lib/sega_per_vbt.a )
target_link_libraries( sdl PUBLIC ${SEGASGL}/lib/libsgl.a )
target_link_libraries( sdl PUBLIC ${SEGALIB}/lib/sega_sat.a )
target_link_libraries( sdl PUBLIC ${SEGALIB}/lib/sega_sys.a )
target_link_libraries( sdl PUBLIC ${SEGALIB}/lib/sega_cdc.a )
target_link_libraries( sdl PUBLIC ${SEGALIB}/lib/sega_clib.a )
target_link_libraries( sdl PUBLIC ${SEGALIB}/lib/sega_per.a )

set_target_properties( sdl PROPERTIES PREFIX "lib" )

install( TARGETS sdl DESTINATION lib )
install( DIRECTORY include/ DESTINATION include )
install( DIRECTORY src/ DESTINATION src )
install( DIRECTORY cmake/ DESTINATION ${SATURN_CMAKE} )
