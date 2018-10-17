# source file
set(INTERFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE HEADERS "${INTERFACE_DIR}/*.h" "${INTERFACE_DIR}/*.hpp")

# target
add_library(tools INTERFACE)
target_sources(tools INTERFACE ${HEADERS})
target_include_directories(tools INTERFACE
  $<BUILD_INTERFACE:${INTERFACE_DIR}>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
)

add_subdirectory(tests)