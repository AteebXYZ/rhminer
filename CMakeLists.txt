cmake_minimum_required(VERSION 2.8)

option(RH_DEBUG_TARGET "Compile in Debug" OFF)
option(RH_CPU_ONLY "Compile only cpu code" ON)
option(RH_ARCH "Architecture name" CPU)
option(RH_NO_SSE4 "Disable sse4 optimizations" OFF)

project(rhminer)
set(RH_ARCH CPU)

set(CMAKE_CXX_STANDARD 11)

if(RH_NO_SSE4)
	message(STATUS "Disabling SSe4 intrinsics")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -mssse3 ")
	set(RH_ARCH CPU_OLDGEN)
	add_definitions(-DRHMINER_NO_SSE4)
else()
	message(STATUS "ENABLING SSe4 intrinsics")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -msse4.1")
endif(RH_NO_SSE4)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake")

include_directories(./)
include_directories(..)
include_directories(rhminer)

include (CheckCCompilerFlag)
include (CheckCSourceCompiles)
include (CheckIncludeFile)
include (CheckIncludeFiles)
include (CheckFunctionExists)
include (CheckSymbolExists)
include (CMake/flags.cmake)


if(RH_CPU_ONLY)
	add_definitions(-DRH_COMPILE_CPU_ONLY)
else(RH_CPU_ONLY)
    message(STATUS "Conditional SSe4")
    add_definitions(-DRHMINER_COND_SSE4)
endif(RH_CPU_ONLY)

add_definitions(-DRH_ARCH=${RH_ARCH})


if(RH_DEBUG_TARGET)
	message(STATUS "Building ${PROJECT_NAME} in DEBUG")
	add_definitions( -Wall -fexceptions -Wfatal-errors -Wno-sign-compare -Wno-deprecated-declarations -Wno-unused-variable)
	add_definitions(-D_DEBUG)
	set(CMAKE_BUILD_TYPE Debug)
else(RH_DEBUG_TARGET)
	message(STATUS "Building ${PROJECT_NAME} for ${RH_ARCH} architecture in Release")
    add_definitions( -Wall -Ofast -fomit-frame-pointer -fexceptions -Wfatal-errors -Wno-sign-compare -Wno-deprecated-declarations -Wno-unused-variable)
	set(CMAKE_BUILD_TYPE Release)
	add_definitions(-DNDEBUG)
endif(RH_DEBUG_TARGET)


set(HEADERS
    rhminer/precomp.h    
    corelib/basetypes.h
    corelib/Common.h
    corelib/CommonData.h
    corelib/rh_compiler.h
    corelib/rh_endian.h
    corelib/FixedHash.h
    corelib/Guards.h
    corelib/Log.h
    corelib/PascalWork.h
    corelib/utils.h
    corelib/vector_ref.h
    corelib/Worker.h
    corelib/miniweb.h
    MinersLib/CLMinerBase.h
    MinersLib/CPUMiner.h
    MinersLib/CudaHostMinerProxy.h
    MinersLib/Farm.h
    MinersLib/GenericCLMiner.h
    MinersLib/GenericFarmClient.h
    MinersLib/GenericMinerClient.h
    MinersLib/Global.h
    MinersLib/GpuManager.h
    MinersLib/KernelOffsetManager.h
    MinersLib/Miner.h
    MinersLib/StratumClient.h
    MinersLib/wrapadl.h
    MinersLib/wrapamdsysfs.h
    MinersLib/wraphelper.h
    MinersLib/wrapnvml.h
    MinersLib/Algo/blake2b.h
    MinersLib/Algo/blake2s.h
    MinersLib/Algo/sph-haval.h
    MinersLib/Algo/sph_blake.h
    MinersLib/Algo/sph_panama.h
    MinersLib/Algo/sph_sha0.h
    MinersLib/Algo/sph_sha1.h
    MinersLib/Algo/sph_sha2.h  
    MinersLib/Algo/sph_types.h
    MinersLib/Pascal/PascalCommon.h
    MinersLib/Pascal/RandomHash.h
    MinersLib/Pascal/RandomHashCLMiner.h
    MinersLib/Pascal/RandomHashCPUMiner.h
    MinersLib/Pascal/RandomHashHostCudaMiner.h
    MinersLib/Pascal/RandomHash_core.h
    MinersLib/Pascal/RandomHash_def.h
    MinersLib/Pascal/RandomHash_inl.h
    MinersLib/Pascal/RandomHash_mersenne_twister.h
    MinersLib/Pascal/RandomHash_MurMur3_32.h
    rhminer/ClientManager.h
    rhminer/CommandLineManager.h
   )

set(SOURCES
    MinersLib/Pascal/RandomHash_Cpu.cpp
    MinersLib/Pascal/RandomHashCLMiner.cpp
    MinersLib/Pascal/RandomHashCPUMiner.cpp
    MinersLib/Pascal/RandomHashHostCudaMiner.cpp
    MinersLib/CLMinerBase.cpp
    MinersLib/Farm.cpp
    MinersLib/GenericCLMiner.cpp
    MinersLib/GenericMinerClient.cpp
    MinersLib/Global.cpp
    MinersLib/GpuManager.cpp
    MinersLib/KernelOffsetManager.cpp
    MinersLib/Miner.cpp
    MinersLib/StratumClient.cpp
    MinersLib/wrapadl.cpp
    MinersLib/wrapamdsysfs.cpp
    MinersLib/wrapnvml.cpp
    MinersLib/Algo/blake.cpp
    MinersLib/Algo/blake2b.cpp
    MinersLib/Algo/blake2s.cpp
    MinersLib/Algo/Pascal.cpp
    MinersLib/Algo/haval.cpp
    MinersLib/Algo/sha0.cpp
    MinersLib/Algo/sha2.cpp    
    MinersLib/Algo/sha1.cpp
    MinersLib/Algo/sph_panama.cpp
    corelib/CommonData.cpp
    corelib/FixedHash.cpp
    corelib/Log.cpp
    corelib/PascalWork.cpp
    corelib/utils.cpp
    corelib/Worker.cpp
    corelib/miniweb.cpp
    rhminer/ClientManager.cpp
    rhminer/CommandLineManager.cpp
    rhminer/main.cpp
    libs/jsoncpp/dist/jsoncpp.cpp
   )
   
# find_package(jsoncpp REQUIRED)
# include_directories(jsoncpp-1.8.0/include)
# target_link_libraries(${PROJECT_NAME} jsoncpp)


set(Boost_USE_STATIC_LIBS   OFF)
set(JSONCPP_LIB_BUILD_STATIC OFF)

#find_package(jsoncpp REQUIRED)
#include_directories(${JSONCPP_INCLUDE_DIRS})
#link_libraries(${JSONCPP_LIBRARIES})

# cmake_policy(SET CMP0167 NEW)
# find_package( Boost REQUIRED COMPONENTS atomic chrono system thread )
# include_directories(${Boost_INCLUDE_DIR})
# link_libraries(${BOOST_LIBRARIES})


set(PU_SPEC "cpu")



#CHECK_INCLUDE_FILE (syslog.h HAVE_SYSLOG_H)
#if (HAVE_SYSLOG_H)
#    add_definitions(/DHAVE_SYSLOG_H)
#endif()

#Fetch boost
include(FetchContent)
FetchContent_Declare(
    boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.83.0
    SOURCE_DIR ${CMAKE_BINARY_DIR}/libs/boost
)

FetchContent_MakeAvailable(boost)

set(BOOST_ROOT ${CMAKE_BINARY_DIR}/libs/boost)
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_NO_SYSTEM_PATHS ON)

# Check OS and run the appropriate build commands
if(WIN32)
    # Windows: Use bootstrap.bat and b2.exe
    execute_process(
        COMMAND cmd /c bootstrap.bat
        WORKING_DIRECTORY ${BOOST_ROOT}
    )

    execute_process(
        COMMAND b2.exe link=static threading=multi --with-atomic --with-chrono --with-system --with-thread stage
        WORKING_DIRECTORY ${BOOST_ROOT}
    )
else()
    # Linux/macOS: Use bootstrap.sh and b2
    execute_process(
        COMMAND ./bootstrap.sh
        WORKING_DIRECTORY ${BOOST_ROOT}
    )

    execute_process(
        COMMAND ./b2 link=static threading=multi --with-atomic --with-chrono --with-system --with-thread stage
        WORKING_DIRECTORY ${BOOST_ROOT}
    )
endif()

# Find and link Boost
find_package(Boost REQUIRED COMPONENTS atomic chrono system thread)

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${BOOST_LIBRARYDIR})
else()
    message(FATAL_ERROR "Boost NOT found")
endif()


# JSONCPP Handling (Keep this)
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/jsoncpp/dist)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE JSON_DLL)
endif()



set(CMAKE_EXE_LINKER_FLAGS "-L/lib64/ -lstdc++")

find_package(Boost REQUIRED COMPONENTS atomic chrono system thread)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/rhminer_${RH_ARCH})
message(STATUS "Output directory is ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${RH_CUDA_LIB} ${LIBS} ${LIBRARIES} ${EXTRA_LIBS} ${CMAKE_DL_LIBS} Boost::atomic Boost::chrono Boost::system Boost::thread ${RH_OCLL})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/jsoncpp/dist)

if(WIN32)
    target_compile_definitions(my_app PRIVATE JSON_DLL)
endif()