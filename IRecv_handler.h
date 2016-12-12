//
// Created by zhaobq on 2016/12/12.
//

#ifndef MPI_CONNECT_IRECV_HANDLER_H
#define MPI_CONNECT_IRECV_HANDLER_H

#endif //MPI_CONNECT_IRECV_HANDLER_H

class IRecv_handler{
public:
    virtual void handler_recv(int mpi_tags, void* buf)=0;
};