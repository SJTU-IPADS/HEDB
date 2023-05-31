# find_package(PostgreSQL REQUIRED)

set(CMAKE_CXX_STANDARD 14)
include_directories(
                    ${PostgreSQL_INCLUDE_DIRS}
                    ${PostgreSQL_SERVER_INCLUDE_DIRS}
                    include
                    ../include
                    )
# include(CTest)

if ( ${TEE_TYPE} STREQUAL "SGX" )
  set(interface SGXInterface)
elseif ( ${TEE_TYPE} STREQUAL "TZ" )
  set(interface TZInterface)
elseif(${TEE_TYPE} STREQUAL "SIM" )
  set(interface SimInterface)
endif()




file(GLOB SRC
  "interface/*.cpp"
  "rr/*.cpp"
  "*.cpp"
  "../utils/*.c"
  )

add_postgresql_extension(
    encdb
    VERSION 0.0.1
    SOURCES ${SRC}
    SCRIPTS encdb--0.0.1.sql
  )

# extension depend on enclave interface to be build first. (e.g. on sgx, enclave_u.* files)
add_dependencies(encdb ${interface})
target_link_libraries(encdb ${interface})

if (ENABLE_TEST_OPS)
add_subdirectory(test)
endif()

