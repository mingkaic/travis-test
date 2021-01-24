cmake_minimum_required(VERSION 3.17)

project(cppkg C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PACKAGE_TESTS "Build the tests" ON)

set(cppkg_INSTALL_default ON)
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
   set(cppkg_INSTALL_default OFF)
endif()
set(cppkg_INSTALL ${cppkg_INSTALL_default} CACHE BOOL "Generate installation target")
set(cppkg_INSTALL_BINDIR "bin" CACHE STRING "Installation directory for executables")
set(cppkg_INSTALL_LIBDIR "lib" CACHE STRING "Installation directory for libraries")
set(cppkg_INSTALL_INCLUDEDIR "include" CACHE STRING "Installation directory for headers")
set(cppkg_INSTALL_CMAKEDIR "lib/cmake/${PACKAGE_NAME}" CACHE STRING "Installation directory for cmake config files")

#### third_parties ####

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(gRPC CONFIG REQUIRED)

#### local libraries ####

include_directories(
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>
    $<INSTALL_INTERFACE:.>
)

# types
add_library(types INTERFACE)

# fmts
add_library(fmts fmts/src/fmts.cpp)
target_link_libraries(fmts PUBLIC types)

# diff
add_library(diff diff/src/msg.cpp)
target_link_libraries(diff PUBLIC fmts)

# logs
add_library(logs logs/src/logs.cpp)
target_link_libraries(logs PUBLIC fmts)

# error
add_library(error error/src/error.cpp)
target_link_libraries(error PUBLIC fmts)

# egrpc
add_library(egrpc STATIC egrpc/src/iclient_async.cpp)
target_link_libraries(egrpc PUBLIC error logs gRPC::grpc++ gRPC::grpc++_unsecure)

# estd
add_library(estd estd/src/strs.cpp)
target_link_libraries(estd PUBLIC logs)

# exam
add_library(exam INTERFACE)
target_link_libraries(exam INTERFACE fmts logs ${CONAN_LIBS_GTEST})

# flag
add_library(flag flag/src/flag.cpp)
target_link_libraries(flag PUBLIC logs ${CONAN_LIBS_BOOST})

# jobs
add_library(jobs INTERFACE)
target_link_libraries(jobs INTERFACE logs)

#### install ####
if(cppkg_INSTALL)
    foreach(_HDR
        diff/diff.hpp
        diff/format.hpp
        diff/msg.hpp
        diff/myers_diff.hpp
        egrpc/client_async.hpp
        egrpc/client_async_stream.hpp
        egrpc/egrpc.hpp
        egrpc/iclient.hpp
        egrpc/iclient_async.hpp
        egrpc/icqueue.hpp
        egrpc/iserver_async.hpp
        egrpc/server_async.hpp
        egrpc/server_async_stream.hpp
        error/error.hpp
        estd/algorithm.hpp
        estd/cast.hpp
        estd/config.hpp
        estd/contain.hpp
        estd/estd.hpp
        estd/hashlist.hpp
        estd/range.hpp
        estd/strs.hpp
        exam/exam.hpp
        exam/mock_log.hpp
        exam/nosupport_log.hpp
        flag/flag.hpp
        fmts/fmts.hpp
        fmts/istringable.hpp
        jobs/jobs.hpp
        jobs/managed_job.hpp
        jobs/scope_guard.hpp
        jobs/sequence.hpp
        logs/ilogs.hpp
        logs/logs.hpp
        types/strs.hpp
        types/types.hpp
    )
        get_filename_component(_PATH ${_HDR} PATH)
        install(FILES ${_HDR} DESTINATION "${cppkg_INSTALL_INCLUDEDIR}/${_PATH}")
    endforeach()

    set(TargetFile ${PROJECT_NAME}Targets)
    install(TARGETS
        diff
        egrpc
        error
        estd
        exam
        flag
        fmts
        jobs
        logs
        types
        EXPORT ${TargetFile}
        RUNTIME DESTINATION ${cppkg_INSTALL_BINDIR}
        LIBRARY DESTINATION ${cppkg_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${cppkg_INSTALL_LIBDIR}
    )
    install(EXPORT ${TargetFile}
        DESTINATION ${cppkg_INSTALL_CMAKEDIR}
        NAMESPACE ${PROJECT_NAME}::
    )
endif()

#### tests ####

if(PACKAGE_TESTS)
    enable_testing()
endif()

# fmts
set(FMTS_TEST fmts_test)
add_executable(${FMTS_TEST} fmts/test/main.cpp)
target_link_libraries(${FMTS_TEST} ${CONAN_LIBS_GTEST} fmts)
add_test(NAME ${FMTS_TEST} COMMAND ${FMTS_TEST})

# diff
set(DIFF_TEST diff_test)
add_executable(${DIFF_TEST} diff/test/main.cpp)
target_link_libraries(${DIFF_TEST} ${CONAN_LIBS_GTEST} diff)
add_test(NAME ${DIFF_TEST} COMMAND ${DIFF_TEST})

# logs
set(LOGS_TEST logs_test)
add_executable(${LOGS_TEST} logs/test/main.cpp)
target_link_libraries(${LOGS_TEST} ${CONAN_LIBS_GTEST} logs)
add_test(NAME ${LOGS_TEST} COMMAND ${LOGS_TEST})

# error
set(ERROR_TEST error_test)
add_executable(${ERROR_TEST} error/test/main.cpp)
target_link_libraries(${ERROR_TEST} ${CONAN_LIBS_GTEST} error)
add_test(NAME ${ERROR_TEST} COMMAND ${ERROR_TEST})

# egrpc
find_program(_PROTOBUF_PROTOC protoc ${CONAN_BIN_DIRS_PROTOBUF} NO_DEFAULT_PATH)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

get_filename_component(EGRPC_TEST_PROTO "${CMAKE_CURRENT_SOURCE_DIR}/egrpc/test/test.proto" ABSOLUTE)
set(EGRPC_TEST_PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/test/test.pb.cc")
set(EGRPC_TEST_PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/test/test.pb.h")
set(EGRPC_TEST_GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/test/test.grpc.pb.cc")
set(EGRPC_TEST_GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/egrpc/test/test.grpc.pb.h")
add_custom_command(
OUTPUT "${EGRPC_TEST_PROTO_SRCS}" "${EGRPC_TEST_PROTO_HDRS}" "${EGRPC_TEST_GRPC_SRCS}" "${EGRPC_TEST_GRPC_HDRS}"
COMMAND ${_PROTOBUF_PROTOC}
ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${CMAKE_CURRENT_SOURCE_DIR}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${EGRPC_TEST_PROTO}"
DEPENDS "${EGRPC_TEST_PROTO}")
add_library(egrpc_test_proto_obj OBJECT ${EGRPC_TEST_PROTO_SRCS} ${EGRPC_TEST_GRPC_SRCS})
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(EGRPC_TEST egrpc_test)
add_executable(${EGRPC_TEST} egrpc/test/main.cpp $<TARGET_OBJECTS:egrpc_test_proto_obj>)
target_link_libraries(${EGRPC_TEST} egrpc estd exam)
add_test(NAME ${EGRPC_TEST} COMMAND ${EGRPC_TEST})

# estd
set(ESTD_TEST estd_test)
add_executable(${ESTD_TEST}
    estd/test/test_cast.cpp
    estd/test/test_config.cpp
    estd/test/test_contain.cpp
    estd/test/test_hashlist.cpp
    estd/test/test_range.cpp
    estd/test/test_strs.cpp
    estd/test/main.cpp)
target_link_libraries(${ESTD_TEST} estd exam)
add_test(NAME ${ESTD_TEST} COMMAND ${ESTD_TEST})

# exam
set(EXAM_TEST exam_test)
add_executable(${EXAM_TEST} exam/test/main.cpp)
target_link_libraries(${EXAM_TEST} exam)
add_test(NAME ${EXAM_TEST} COMMAND ${EXAM_TEST})

# flag
set(FLAG_TEST flag_test)
add_executable(${FLAG_TEST} flag/test/main.cpp)
target_link_libraries(${FLAG_TEST} ${CONAN_LIBS_GTEST} flag)
add_test(NAME ${FLAG_TEST} COMMAND ${FLAG_TEST})

# jobs
set(JOBS_TEST jobs_test)
add_executable(${JOBS_TEST} jobs/test/main.cpp)
target_link_libraries(${JOBS_TEST} ${CONAN_LIBS_GTEST} jobs)
add_test(NAME ${JOBS_TEST} COMMAND ${JOBS_TEST})
