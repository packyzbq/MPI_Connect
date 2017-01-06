//
// Created by zhaobq on 2016/12/12.
//

#ifndef MPI_CONNECT_IRECV_HANDLER_H
#define MPI_CONNECT_IRECV_HANDLER_H

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
    //TODO need remove
    virtual void handler_recv(int mpi_tags, Recv_Pack pack)=0;
    virtual void handler_recv(int mpi_tags, Pack_Int pack)=0;
    virtual void handler_recv(int mpi_tags, Pack_Str pack)=0;
};

#endif //MPI_CONNECT_IRECV_HANDLER_H



