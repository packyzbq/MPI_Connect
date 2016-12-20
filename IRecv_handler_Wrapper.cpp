//
// Created by zhaobq on 2016/12/20.
//
#include "IRecv_handler.h"
#include <boost/python.hpp>
using namespace boost::python;

struct IRecv_handler_Wrapper : IRecv_handler, wrapper<IRecv_handler>
{
    void handler_recv(int mpi_tags, Recv_Pack pack)
    {
        this->get_override("handler_recv")();
    };
};



