//
// Created by zhaobq on 2016/12/20.
//
#include "IRecv_handler.h"
#include <boost/python.hpp>
using namespace boost::python;

struct IRecv_handler_Wrapper : IRecv_handler, wrapper<IRecv_handler>
{
    void handler_recv_int(int mpi_tags, Pack_Int pack)
    {
        this->get_override("handler_recv_int")(mpi_tags, pack);
    };

    void handler_recv_str(int mpi_tags, Pack_Str pack)
    {
        this->get_override("handler_recv_str")(mpi_tags, pack);
    };
};

