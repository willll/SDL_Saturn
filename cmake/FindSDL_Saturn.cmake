message("<FindSDL_Saturn.cmake>")

SET(SDL_Saturn_SEARCH_PATHS
	$ENV{SATURN_ROOT}/SDL
)

FIND_PATH(SDL_Saturn_INCLUDE_DIR SDL.h
	HINTS
	$ENV{SATURN_ROOT}
	PATH_SUFFIXES include
	PATHS ${SDL_Saturn_SEARCH_PATHS}
)

FIND_LIBRARY(SDL_Saturn_LIBRARY_TEMP
	NAMES SDL_Saturn
	HINTS
	$ENV{SATURN_ROOT}
	PATH_SUFFIXES lib
	PATHS ${SDL_Saturn_SEARCH_PATHS}
)

set(SDL_Saturn_LIBRARIES ${SDL_Saturn_LIBRARY}/libsdl.a )
set(SDL_Saturn_INCLUDES ${SDL_Saturn_INCLUDE_DIR} )

message("</FindSDL_Saturn.cmake>")

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL_Saturn REQUIRED_VARS SDL_Saturn_LIBRARY SDL_Saturn_INCLUDE_DIR)
