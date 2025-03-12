
include(FetchContent)

FetchContent_Declare(eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0
)
FetchContent_GetProperties(eigen)
if(NOT eigen_POPULATED)
  FetchContent_Populate(eigen)
  add_library(eigen INTERFACE)
  target_include_directories(eigen INTERFACE ${eigen_SOURCE_DIR})
endif()
