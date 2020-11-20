#add_clang_executable(example
#  Example.cpp
#  )
find_package(FMT REQUIRED)
find_package(PythonInterp 2.7.12 REQUIRED)
find_package(PythonLibs 2.7.12 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
add_clang_executable(matcher 
  matcher.cpp tis.cpp)
target_link_libraries(matcher
  PRIVATE
  clangTooling
  clangASTMatchers
  clangRewrite
  fmt::fmt
  ${PYTHON_LIBRARIES}
  )

target_include_directories(matcher PUBLIC SYSTEM .)
