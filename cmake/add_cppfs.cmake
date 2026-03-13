include(FetchContent)

FetchContent_Declare(cppfs
	GIT_REPOSITORY https://github.com/cginternals/cppfs
	GIT_TAG v1.2.0
)

set(OPTION_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(cppfs)
