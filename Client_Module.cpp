//
// Created by zhaobq on 2016/12/20.
//

#include <boost/python.hpp>
#include "MPI_Client.cpp"
#include "IRecv_handler.h"
using namespace boost::python;

BOOST_PYTHON_MODULE(MPI_Client)
        {
                class_<Recv_Pack>("Recv_Pack", init<int, char*>)
                        .def_readonly("ibuf", &Recv_Pack::ibuf)
                        .def_readonly("sbuf", &Recv_Pack::sbuf)
                ;

                class_<IRecv_handler_Wrapper, boost::noncopyable>("IRecv_handler")
                .def("handler_recv", pure_virtual(&IRecv_handler::handler_recv))
                ;

                class_<MPI_Client>("MPI_Client", init<IRecv_handler*, char* ,char*>)
                    .def("initialize", &MPI_Client::initialize)
                    .def("run", &MPI_Client::run)
                    .def("stop", &MPI_Client::stop)
                    .def("finalize", &&MPI_Client::finalize)
                    .def("send_action", &MPI_Client::send_action)
                    .def("set_wid", &MPI_Client::set_wid)

        }