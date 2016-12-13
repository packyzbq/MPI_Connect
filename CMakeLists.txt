cmake_minimum_required(VERSION 3.6)
project(MPI_Connect)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

link_directories(../mpi-lib/lib)
include_directories(../mpi-lib/include)

set(SOURCE_FILES main.cpp MPI_Connect_Base.cpp MPI_Connect_Base.h IRecv_handler.h MPI_Util.h MPI_Server.cpp MPI_Server.h)
add_executable(MPI_Connect ${SOURCE_FILES})