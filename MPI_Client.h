//
// Created by zhaobq on 2016/12/13.
//

#ifndef MPI_CONNECT_MPI_CLIENT_H
#define MPI_CONNECT_MPI_CLIENT_H

#include "MPI_Connect_Base.h"


class MPI_Client : public MPI_Connect_Base{
public:
    MPI_Client(IRecv_buffer* mh, char* svc_name, char* uuid);
    ~MPI_Client();

    int initialize();
    void run();
    int stop();
    int finalize();

    bool new_msg_come(ARGS *args);
//    void send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    int send_int(int buf, int msgsize, int dest, int tag);
    int send_string(char* buf, int msgsize, int dest, int tag);
    void recv_handle(int tag, void* buf, int length, MPI_Datatype type, MPI_Comm comm);
    //TODO add recv_bcast()

private:
    char* svc_name_;
    char portname[MPI_MAX_PORT_NAME];

    MPI_Comm sc_comm_;
    string uuid_= "";
};


#endif //MPI_CONNECT_MPI_CLIENT_H
