# find_package(PostgreSQL REQUIRED)

set(CMAKE_CXX_STANDARD 14)
include_directories(
                    ${PostgreSQL_INCLUDE_DIRS}
                    ${PostgreSQL_SERVER_INCLUDE_DIRS}
                    include
                    ../include
                    )
# include(CTest)

if ( ${TEE_TYPE} STREQUAL "CVM" )
  set(interface CVMInterface)
endif()

file(GLOB SRC
  "*.cpp"
  "udt/*.cpp"
  "interface/*.cpp"
  "ops_client/*.cpp"
  "rr/*.cpp"
  "../utils/*.cpp"
  )

add_postgresql_extension(
    hedb
    VERSION 1.0
    SOURCES ${SRC}
    SCRIPTS hedb--1.0.sql
  )

add_dependencies(hedb ${interface})
target_link_libraries(hedb ${interface})

if (ENABLE_TEST_OPS)
add_subdirectory(test)
endif()

