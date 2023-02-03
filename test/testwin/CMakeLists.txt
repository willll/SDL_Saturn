cmake_minimum_required(VERSION 3.0)

SET(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_VERBOSE_MAKEFILE ON)

set( TARGET_NAME testwin )
set( TARGET_PATH testwin )

set(INSTALL_DESTINATION exe)

project( ${TARGET_NAME} VERSION 1.0
                  DESCRIPTION "Check the version and dynamic loading and endianness"
                  LANGUAGES C CXX ASM)

set(as_flags )

set( c_flags
        -Os -m2
        -save-temps
        -fno-lto
        -Wshadow
        -Wall
        -Wswitch
        #-Wpadded
        -fno-common -Wextra -Wno-narrowing -fno-web
        -fno-builtin -funit-at-a-time -Wwrite-strings
        -Wl,--strip-all -Wl,--verbose -mno-fsrra
        -Wno-padded
        #-Wno-unused
        #-Wno-implicit-fallthrough
        -maccumulate-outgoing-args -fomit-frame-pointer
        )

set(cxx_flags
        -O2 -g3 -m2
        -fpermissive
        -Wno-conversion-null
        -fno-lto -Wshadow -Wall -Wswitch -Wpadded
        -fno-common -Wextra -Wno-narrowing -fno-web
        -fno-builtin -funit-at-a-time -Wwrite-strings
        -Wl,--strip-all -Wl,--verbose -mno-fsrra
        -Wno-padded
        -maccumulate-outgoing-args
        -fomit-frame-pointer
        -fno-rtti
        -fno-exceptions
        -Wdeprecated-declarations   # FIXME!
         )

set(SOURCES
  ${SATURN_COMMON}/crt0.s
  testwin.c
  )

set(CD_FILES
      0.bin
    )

add_compile_definitions(
                        MODEL_S
                        _SH
                        )

add_executable( ${TARGET_NAME}.elf ${SOURCES} )

message("Looking for SDL : ${SDL_LIB}")

target_include_directories(
    ${TARGET_NAME}.elf PUBLIC
    ${SDL_LIB}
    ${SDL_LIB}/include
    ${SEGALIB}/include
    ${SEGASGL}/include
    )

target_compile_options(${TARGET_NAME}.elf PUBLIC $<$<COMPILE_LANGUAGE:C>:${c_flags}> )

target_link_options(
    ${TARGET_NAME}.elf PUBLIC
    "SHELL:-Xlinker -Map -Xlinker ${TARGET_NAME}.map"
    "SHELL:-Xlinker --strip-debug"
    "SHELL:-Xlinker -fno-lto"
    "SHELL:-m2 -nostartfiles"
    "SHELL:-T${SATURN_COMMON}/Saturn.lnk"
)

target_link_libraries( ${TARGET_NAME}.elf PUBLIC /saturn/SDL/lib//sdl.a )

add_custom_target(run_${TARGET_NAME} ALL DEPENDS ${TARGET_NAME}.elf ${TARGET_NAME}.bin IP.BIN)

add_custom_command(OUTPUT ${TARGET_NAME}.bin
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   COMMAND ${CMAKE_OBJCOPY}
                   ARGS -O binary ${TARGET_NAME}.elf ${TARGET_NAME}.bin )

add_custom_command(OUTPUT IP.BIN
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMAND $ENV{SATURN_IPMAKER}/IPMaker
                  ARGS ${ip_maker_verbose} -o ${CMAKE_CURRENT_BINARY_DIR}/IP.BIN
                        -t ${TARGET_NAME} -p 2 )

target_link_libraries( ${TARGET_NAME}.elf PUBLIC ${SEGASGL}/lib/sglarea.o )
target_link_libraries( ${TARGET_NAME}.elf PUBLIC ${SEGASGL}/lib/libsgl.a )
target_link_libraries( ${TARGET_NAME}.elf PUBLIC ${SEGALIB}/lib/sega_mem.a )
target_link_libraries( ${TARGET_NAME}.elf PUBLIC ${SEGALIB}/lib/sega_gfs.a )
target_link_libraries( ${TARGET_NAME}.elf PUBLIC ${SEGALIB}/lib/sega_csh_sgl.a )
target_link_libraries( ${TARGET_NAME}.elf PUBLIC ${SEGALIB}/lib/sega_dma.a )
target_link_libraries( ${TARGET_NAME}.elf PUBLIC ${SEGALIB}/lib/sega_int.a )
target_link_libraries( ${TARGET_NAME}.elf PUBLIC ${SEGALIB}/lib/sega_sys.a )
target_link_libraries( ${TARGET_NAME}.elf PUBLIC ${SEGALIB}/lib/sega_cdc.a )
target_link_libraries( ${TARGET_NAME}.elf PUBLIC ${SEGALIB}/lib/sega_int.a )

include(${SATURN_CMAKE}/install.cmake)
include(${SATURN_CMAKE}/create_cd.cmake)
