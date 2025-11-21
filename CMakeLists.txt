cmake_minimum_required(VERSION 3.13)

project(ticktock)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG True)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
        COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
        COMMAND date +%s
        OUTPUT_VARIABLE CTIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

add_definitions(
        -DTT_BRANCH="${BRANCH}"
        -DTT_COMMIT="${COMMIT}"
        -DCOMPILE_TIME=${CTIME}
)

add_compile_options(
        -Wno-unused-result
        -Wno-format-truncation
        -Wno-psabi
        -Wno-deprecated-declarations
        -ffast-math
        -O3
)

include_directories(include)

add_library(ttlib STATIC
        src/admin.cpp
        src/global.cpp

        src/agg/aggregate.cpp
        src/agg/down.cpp
        src/agg/rate.cpp
        src/agg/rollup.cpp

        src/core/compress.cpp
        src/core/cp.cpp
        src/core/dp.cpp
        src/core/http.cpp
        src/core/memmgr.cpp
        src/core/mmap.cpp
        src/core/page.cpp
        src/core/part.cpp
        src/core/query.cpp
        src/core/task.cpp
        src/core/tcp.cpp
        src/core/ts.cpp
        src/core/tsdb.cpp
        src/core/udp.cpp

        src/utils/append.cpp
        src/utils/bitset.cpp
        src/utils/cal.cpp
        src/utils/config.cpp
        src/utils/fd.cpp
        src/utils/infnan.cpp
        src/utils/json.cpp
        src/utils/kv.cpp
        src/utils/logger.cpp
        src/utils/meta.cpp
        src/utils/meter.cpp
        src/utils/mqtt.cpp
        src/utils/range.cpp
        src/utils/stats.cpp
        src/utils/strbuf.cpp
        src/utils/sync.cpp
        src/utils/tag.cpp
        src/utils/timer.cpp
        src/utils/utils.cpp
)

add_executable(tt
        src/main.cpp
)

target_link_libraries(tt
        ttlib
        Threads::Threads
        ZLIB::ZLIB
)

if (ENV{ENABLE_MQTT} EQUAL "1")
    find_path(MOSQUITTO_INCLUDE_DIR mosquitto.h)
    find_library(MOSQUITTO_LIBRARY NAMES mosquitto mosquittopp)

    if (MOSQUITTO_INCLUDE_DIR AND MOSQUITTO_LIBRARY)
        message(STATUS "Found Mosquitto: ${MOSQUITTO_LIBRARY}")
        include_directories(${MOSQUITTO_INCLUDE_DIR})
        target_link_libraries(tt ${MOSQUITTO_LIBRARY})
        add_definitions(-DENABLE_MQTT)
    else ()
        message(FATAL_ERROR "Mosquitto not found.")
    endif ()
endif ()

####

add_executable(append
        tools/append.cpp
)

target_link_libraries(append
        ttlib
        Threads::Threads
)

####

add_executable(backfill
        tools/backfill.cpp
)

target_link_libraries(backfill
        ttlib
        ZLIB::ZLIB
)

####

add_executable(client_http
        tools/client_http.cpp
)

target_link_libraries(client_http
        ttlib
)

####

add_executable(client_tcp
        tools/client_tcp.c
)

target_link_libraries(client_tcp
        ttlib
        Threads::Threads
)

####

add_executable(inspect
        tools/inspect.cpp
)

target_link_libraries(inspect
        ttlib
        Threads::Threads
)

####

add_executable(iobench
        tools/iobench.cpp
)

target_link_libraries(iobench
        ttlib
        Threads::Threads
)

####

add_executable(single_client_tcp
        tools/single_client_tcp.cpp
)

target_link_libraries(single_client_tcp
        ttlib
)

####

add_executable(tt_env
        tools/tt_env.cpp
)

target_link_libraries(tt_env
        ttlib
)

####

add_executable(all_tests
        test/all_tests.cpp

        test/agg_test.cpp
        test/bitset_test.cpp
        test/cal_test.cpp
        test/compact_test.cpp
        test/compress_test.cpp
        test/cp_test.cpp
        test/dp_test.cpp
        test/json_test.cpp
        test/misc_test.cpp
        test/query_test.cpp
        test/task_test.cpp
)

target_include_directories(all_tests PRIVATE test/include)

target_link_libraries(all_tests
        ttlib
        Threads::Threads
)
