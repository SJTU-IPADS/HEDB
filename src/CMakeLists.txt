cmake_minimum_required(VERSION 3.9)
project(extension)

# set(ENABLE_TEST_OPS true)

if(ENABLE_TEST_OPS)
    add_compile_definitions(ENABLE_TEST_OPS)
endif()

set(TEE_TYPE "" CACHE STRING "Type of TEE (SGX, TZ, SIM, etc)")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wunused-variable -Wunused-but-set-variable")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-variable -Wunused-but-set-variable")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(PostgreSQL)
if(PostgreSQL_FOUND)

set(_link_flags "${PostgreSQL_SHARED_LINK_OPTIONS}")
foreach(_dir ${PostgreSQL_SERVER_LIBRARY_DIRS})
    set(_link_flags "${_link_flags} -L${_dir}")
endforeach()

endif()


file(GLOB DES_SRCS
    # enclave/plain_ops/*.c
    enclave/desenitizer.cpp
    enclave/simulate/crypto.c
)
add_executable(desenitizer ${DES_SRCS})
target_include_directories(desenitizer 
        PRIVATE enclave/include
        PRIVATE enclave
        PRIVATE include
        PRIVATE enclave/simulate
        )
target_link_libraries(desenitizer mbedtls mbedcrypto pthread)

# set(OLD_COMPILER ${CMAKE_C_COMPILER})
# set(OLD_CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} )

# set(CMAKE_C_COMPILER clang)
# # set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-flto")


# target_compile_options(klee_wrapper PUBLIC -emit-llvm)

# set(CMAKE_C_COMPILER ${OLD_COMPILER})
# set(CMAKE_EXE_LINKER_FLAGS ${OLD_CMAKE_EXE_LINKER_FLAGS})


# find_program(CCACHE_FOUND ccache)
# if(CCACHE_FOUND)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
#     set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
# endif(CCACHE_FOUND)


if ( ${TEE_TYPE} STREQUAL "SGX" )

    include_directories(${SGX_INCLUDE_DIR} include enclave enclave/include)
    find_package(SGX REQUIRED)
    set(EDL enclave/sgx/enclave.edl)
    set(EDL_SEARCH_PATHS enclave/sgx)

    # add_library(SGXInterface STATIC ${U_SRCS})

    # file(GLOB T_SRCS LocalAttestationCode/*.c*)
    # add_trusted_library(LocalAttestationLib SRCS "${T_SRCS}" EDL LocalAttestationCode/LocalAttestationCode.edl EDL_SEARCH_PATHS ${EDL_SEARCH_PATHS})
    file(GLOB E_SRCS enclave/sgx/*.cpp enclave/kv/*.cpp enclave/*.c)
   
    add_enclave_library(enclave
                        USE_PREFIX
                        SRCS ${E_SRCS} 
                        EDL ${EDL} 
                        EDL_SEARCH_PATHS ${EDL_SEARCH_PATHS}
                        LDSCRIPT enclave/sgx/enclave.lds)
    ## DE
    enclave_sign(enclave KEY enclave/sgx/Enclave_private.pem CONFIG enclave/sgx/enclave.config.xml)
    add_compile_definitions(TEE_SGX)

    file(GLOB U_SRCS extension/interface/sgx/sgx_interface.cpp)
    include_directories(extension/include extension/interface/sgx)

    # custom installation for enclave.so and datafile.
    SET(ENCLAVE_INSTALL_DIR "/usr/local/lib/hedb")
    set(SIGNED_ENCLAVE_FILENAME "enclave.signed.so")
    SET(DATA_FILENAME "${ENCLAVE_INSTALL_DIR}/hedb.data")
    SET(ENCLAVE_FILENAME "${ENCLAVE_INSTALL_DIR}/${SIGNED_ENCLAVE_FILENAME}")
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${SIGNED_ENCLAVE_FILENAME} DESTINATION ${ENCLAVE_INSTALL_DIR})
    install(CODE "execute_process(COMMAND touch ${DATA_FILENAME})" )
    install(CODE "execute_process(COMMAND chown postgres:postgres ${DATA_FILENAME})")
   
    add_compile_definitions(ENCLAVE_FILENAME="${ENCLAVE_FILENAME}")
    add_compile_definitions(DATA_FILENAME="${DATA_FILENAME}")

    add_untrusted_library(SGXInterface
                        STATIC
                        SRCS ${U_SRCS} 
                        EDL ${EDL}
                        EDL_SEARCH_PATHS ${EDL_SEARCH_PATHS})
    add_dependencies(SGXInterface enclave-sign)

elseif ( ${TEE_TYPE} STREQUAL "TZ" )

    # https://cmake.org/Wiki/CMake_Useful_Variables
    set (CMAKE_TOOLCHAIN_FILE CMakeToolchain.txt)
    add_compile_options (-Wall)
    include(GNUInstallDirs)

   
    add_custom_target(build_ta ALL
        COMMAND ${CMAKE_MAKE_PROGRAM}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/enclave/trustzone
        COMMENT "trustzone TA target")

    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/enclave/trustzone/64974d40-92cd-4714-9e8f-442f3a4db3af.ta DESTINATION /lib/optee_armtz/ )    
        # 64974d40-92cd-4714-9e8f-442f3a4db3af.ta

    ######################
    project(TZInterface)
    set (USRC extension/interface/trustzone/tz_interface.c)

    add_library(${PROJECT_NAME} ${USRC})
    set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_include_directories(${PROJECT_NAME}
                PRIVATE extension/include 
                PRIVATE extension/include/trustzone
                PRIVATE extension/interface/trustzone
                PRIVATE include
                )

    target_link_libraries (${PROJECT_NAME} PRIVATE pthread)
    target_link_libraries (${PROJECT_NAME} PRIVATE teec)

    install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

elseif ( ${TEE_TYPE} STREQUAL "SIM" )

    add_compile_definitions(TEE_SIM) 
    file(GLOB SRCS 
        enclave/enc_*.c  
        enclave/like_match.c
        enclave/plain_ops/*.c
        enclave/kv/*.cpp 
        enclave/simulate/sim.cpp 
        enclave/simulate/crypto.c 
    )
    # add_executable(sim_ops ${SRCS})
    add_executable(sim_ops ${SRCS})
    target_include_directories(sim_ops 
        PRIVATE extension/include
        PRIVATE enclave/include
        PRIVATE enclave
        PRIVATE include
        )
    # link_directories(enclave/simulate enclave/)
    target_link_libraries(sim_ops mbedtls mbedcrypto pthread)
    # install 
    SET(INSTALL_DIR "/usr/local/lib/hedb")
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/sim_ops DESTINATION ${INSTALL_DIR})
    install(CODE "execute_process(COMMAND chown postgres:postgres ${INSTALL_DIR}/sim_ops )")



    if(PostgreSQL_FOUND)
    file(GLOB USRCS
        utils/sync.c
        extension/interface/simulate/sim_interface.cpp
        )
    add_library(SimInterface ${USRCS}) 

    target_include_directories(SimInterface
        PRIVATE extension/include 
        PRIVATE extension/interface/simulate
        PRIVATE include
        )
    
    target_include_directories(SimInterface 
        PRIVATE ${PostgreSQL_INCLUDE_DIRS}
        PRIVATE ${PostgreSQL_SERVER_INCLUDE_DIRS}
    )
    endif()

else()
    message( FATAL_ERROR "TEE_TYPE ${TEE_TYPE} not specified (SGX/TZ). CMake wil exit.")
endif()

## for extension
if(PostgreSQL_FOUND) 
add_subdirectory(extension)
endif()

OPTION(ENABLE_PARALLEL "parallel enc/dec in UDF" OFF) # disabled by default
IF(ENABLE_PARALLEL)
    add_compile_definitions(ENABLE_PARA)
ENDIF(ENABLE_PARALLEL)

# ## for test
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../test/ops-micro ${CMAKE_CURRENT_BINARY_DIR}/)
## KV and PARA is not compatible for now
# add_compile_definitions(ENABLE_KV)
# add_compile_definitions(ENABLE_PARA)
# add_compile_definitions(ENABLE_LOCAL_SIM)

