//
// Created by zhaobq on 2017/1/9.
//

#include <boost/python.hpp>
#include "IRecv_handler_Wrapper.cpp"
#include "IRecv_buffer.h"
using namespace boost::python;

BOOST_PYTHON_MODULE(IRecv_Module){

    class_<Pack_Int>("Pack_Int", init<int>())
        .def_readonly("buf", &Pack_Int::buf)
    ;

    class_<Pack_Str>("Pack_Str", init<char*>())
        .def_readonly("buf", &Pack_Str::buf)
    ;

    class_<IRecv_handler_Wrapper, boost::noncopyable>("IRecv_handler")
        //.def("handler_recv", pure_virtual(&IRecv_handler::handler_recv))
        .def("handler_recv_int", pure_virtual(&IRecv_handler::handler_recv_int))
        .def("handler_recv_str", pure_virtual(&IRecv_handler::handler_recv_str))
    ;

    class_<Pack>("Pack")
        .def_readonly("tag", &Pack::tag)
        .def_readonly("ibuf", &Pack::ibuf)
        .def_readonly("sbuf", &Pack::sbuf)
        .def_readonly("size", &Pack::size)
    ;

    class_<IRecv_buffer>("IRecv_buffer")
        .def("get", &IRecv_buffer::get)
        .def("put", &IRecv_buffer::put)
        .def("empty", &IRecv_buffer::empty)
    ;
}