cmake_minimum_required(VERSION 3.13.4)
project(DeadFeaturePass LANGUAGES CXX)

# Find a system LLVM installation. Set LLVM_DIR if cmake can't find it,
# e.g. -DLLVM_DIR=$(llvm-config --cmakedir)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION} from ${LLVM_DIR}")

# Match the C++ standard and flags used to build LLVM.
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# LLVM headers and definitions.
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND "${LLVM_DEFINITIONS}")
add_compile_options(${LLVM_DEFINITIONS_LIST})

# LLVM is typically built without RTTI. Match that to keep the plugin
# loadable.
if(NOT LLVM_ENABLE_RTTI)
  add_compile_options(-fno-rtti)
endif()

# Build the pass as a shared loadable plugin for opt.
add_library(DeadFeaturePass MODULE
  DeadFeaturePass.cpp
)

# On macOS opt loads with dlopen; allow undefined symbols (they resolve
# inside the opt process at load time).
if(APPLE)
  set_target_properties(DeadFeaturePass PROPERTIES
    LINK_FLAGS "-undefined dynamic_lookup"
  )
endif()
