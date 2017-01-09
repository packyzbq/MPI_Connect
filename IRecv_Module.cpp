//
// Created by zhaobq on 2017/1/9.
//

#include <boost/python.hpp>
#include "IRecv_handler_Wrapper.cpp"
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
        .def("handler_recv_int", pure_virtual(handler_recv_int))
        .def("handler_recv_str", pure_virtual(handler_recv_str))
    ;

}