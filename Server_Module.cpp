//
// Created by zhaobq on 2016/12/16.
//
#include <boost/python.hpp>
#include "MPI_Server.cpp"
#include "MPI_Connect_Base.cpp"
#include "IRecv_handler_Wrapper.cpp"
using namespace boost::python;


BOOST_PYTHON_MODULE(Server_Module)
{
    class_<MPI_Server>("MPI_Server", init<IRecv_buffer*, char*>())
        .def("run", &MPI_Server::run)
        .def("initialize", &MPI_Server::initialize)
        .def("stop", &MPI_Server::stop)
        .def("finalize", &MPI_Server::finalize)
        .def("recv_handle", &MPI_Server::recv_handle)
        .def("send_int", &MPI_Server::send_int)
        .def("send_string", &MPI_Server::send_string)
        .def("disconnect_client", &MPI_Server::disconnect_client)
        .def("bcast", &MPI_Server::bcast)
        ;

}
