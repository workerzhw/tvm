if(USE_LPU)
  list(APPEND RUNTIME_SRCS src/runtime/contrib/lpu/hello_module.cc)
  list(APPEND COMPILER_SRCS src/runtime/contrib/lpu/hello_module.cc)
  message(STATUS "Build with Hello support")
else()
  message(STATUS "NOT BUILD LPU SUPPORT")
endif(USE_LPU)

