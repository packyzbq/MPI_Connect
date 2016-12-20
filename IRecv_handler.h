//
// Created by zhaobq on 2016/12/12.
//

#ifndef MPI_CONNECT_IRECV_HANDLER_H
#define MPI_CONNECT_IRECV_HANDLER_H

#endif //MPI_CONNECT_IRECV_HANDLER_H

#include <string>
using namespace std;

struct Recv_Pack{
    int ibuf;
    std::string sbuf;

    Recv_Pack(int i, char* str){
        if(i == NULL)
            sbuf = str;
        else
            ibuf = i;
    }
};

struct IRecv_handler
{
    virtual void handler_recv(int mpi_tags, Recv_Pack pack)=0;
};

