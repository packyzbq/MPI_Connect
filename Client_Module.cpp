//
// Created by zhaobq on 2016/12/20.
//
#include <boost/python.hpp>
#include "MPI_Client.cpp"
#include "IRecv_buffer.h"
#include "MPI_Connect_Base.cpp"
#include "IRecv_handler_Wrapper.cpp"
using namespace boost::python;


BOOST_PYTHON_MODULE(Client_Module)
{

    class_<MPI_Client>("MPI_Client", init<IRecv_buffer*, char* ,char*>())
        .def("initialize", &MPI_Client::initialize)
        .def("run", &MPI_Client::run)
        .def("stop", &MPI_Client::stop)
        .def("finalize", &MPI_Client::finalize)
        .def("send_string", &MPI_Client::send_string)
        .def("send_int", &MPI_Client::send_int)
    ;
}