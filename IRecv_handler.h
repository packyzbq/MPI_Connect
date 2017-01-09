//
// Created by zhaobq on 2016/12/12.
//

#ifndef MPI_CONNECT_IRECV_HANDLER_H
#define MPI_CONNECT_IRECV_HANDLER_H

#include <string>
using namespace std;

class Pack_Int{
public:
    int buf;
    Pack_Int(int i):buf(i){};
};

class Pack_Str{
public:
    string buf;
    Pack_Str(char* s):buf(s){};
};


struct IRecv_handler
{
    virtual void handler_recv_int(int mpi_tags, Pack_Int pack)=0;
    virtual void handler_recv_str(int mpi_tags, Pack_Str pack)=0;
};

#endif //MPI_CONNECT_IRECV_HANDLER_H



