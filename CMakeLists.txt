cmake_minimum_required(VERSION 3.12)

project(UraniumLang)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)
find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXXFLAGS} -Wall -Wno-return-local-addr")

file(GLOB LIB_SRCS library/*.cpp)
add_library(ulang_lib STATIC ${LIB_SRCS})
set_target_properties(ulang_lib PROPERTIES OUTPUT_NAME "ulang")

file(GLOB COMP_SRCS compiler/*.cpp)
add_executable(ulang ${COMP_SRCS})
target_link_libraries(ulang ulang_lib ${LLVM_LIBRARIES} ${LLVM_SYSTEM_LIBS} LLVMCore LLVMIRReader LLVMBitReader LLVMBitWriter LLVMCodeGen LLVMScalarOpts)

target_include_directories(ulang_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/library)
target_include_directories(ulang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/library)

file(GLOB RUN_SRCS runtime/*.cpp)
add_library(ulang_runtime STATIC ${RUN_SRCS})
set_target_properties(ulang_runtime PROPERTIES OUTPUT_NAME "ulangrun")

target_include_directories(ulang_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/library)

set_target_properties(ulang_lib ulang ulang_runtime
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/library/libulang.a
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/compiler/ulang
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/runtime/libulangrt.a
)

add_custom_target(make_dirs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/library/obj
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/compiler/obj
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/runtime/obj
)

add_dependencies(ulang ulang_lib)
add_dependencies(ulang_runtime make_dirs)
