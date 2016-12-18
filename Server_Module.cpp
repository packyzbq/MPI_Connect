//
// Created by zhaobq on 2016/12/16.
//
#include <boost/python.hpp>
#include "MPI_Server.cpp"
using namespace boost::python;

BOOST_PYTHON_MODULE("MPI_Server")
{
    class_<MPI_Server>("MPI_Server")

}
