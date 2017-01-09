//
// Created by zhaobq on 2016/12/20.
//
#include <boost/python.hpp>
#include "MPI_Client.cpp"
#include "MPI_Connect_Base.cpp"
#include "IRecv_handler_Wrapper.cpp"
using namespace boost::python;


BOOST_PYTHON_MODULE(Client_Module)
{
    class_<Pack_Int>("Pack_Int", init<int>())
        .def_readonly("buf", &Pack_Int::buf)
    ;

    class_<Pack_Str>("Pack_Str", init<char*>())
        .def_readonly("buf", &Pack_Str::buf)
    ;

    class_<IRecv_handler_Wrapper, boost::noncopyable>("IRecv_handler")
        //.def("handler_recv", pure_virtual(&IRecv_handler::handler_recv))
        .def("handler_recv_int", pure_virtual(handler_recv_int))
        .def("handler_recv_str", pure_virtual(handler_recv_str))
    ;

    class_<MPI_Client>("MPI_Client", init<IRecv_handler*, char* ,char*>())
        .def("initialize", &MPI_Client::initialize)
        .def("run", &MPI_Client::run)
        .def("stop", &MPI_Client::stop)
        .def("finalize", &MPI_Client::finalize)
        .def("send_string", &MPI_Client::send_string)
        .def("send_int", &MPI_Client::send_int)
        .def("set_wid", &MPI_Client::set_wid)
    ;
}