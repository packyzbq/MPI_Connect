//
// Created by zhaobq on 2016/12/16.
//
#include <boost/python.hpp>
#include "MPI_Server.cpp"
#include "IRecv_handler_Wrapper.cpp"
using namespace boost::python;

BOOST_PYTHON_MODULE(MPI_Server)
{
    class_<Recv_Pack>("Recv_Pack", init<int, char*>)
        .def_readonly("ibuf", &Recv_Pack::ibuf)
        .def_readonly("sbuf", &Recv_Pack::sbuf)
        ;

    class_<IRecv_handler_Wrapper, boost::noncopyable>("IRecv_handler")
        .def("handler_recv", pure_virtual(&IRecv_handler::handler_recv))
        ;
    class_<MPI_Server>("MPI_Server", init<IRecv_handler*, char*>)
        .def("run", MPI_Server::run)
        .def("initialize", MPI_Server::initialize)
        .def("stop", MPI_Server::stop)
        .def("finalize", MPI_Server::finalize)
        .def("new_msg_come", MPI_Server::new_msg_come)
        .def("recv_handle", MPI_Server::recv_handle)
        .def("send_action", MPI_Server::send_action)
        .def("disconnect_client", MPI_Server::disconnect_client)
        .def("bcast", MPI_Server::bcast)
        ;

}
