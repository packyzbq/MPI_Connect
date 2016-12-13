//
// Created by zhaobq on 2016/12/13.
//

#ifndef MPI_CONNECT_MPI_CLIENT_H
#define MPI_CONNECT_MPI_CLIENT_H

#include "MPI_Connect_Base.h"


class MPI_Client : public MPI_Connect_Base{
public:
    MPI_Client(IRecv_handler *mh, char* svc_name, char* port);
    ~MPI_Client();

    int initialize();
    void run();
    int stop();
    int finalize();

    bool new_msg_come(ARGS *args);
//    void send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    int send_action(void* buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    void recv_handle(int tag, void* buf, MPI_Comm comm);
    //TODO add recv_bcast()
    void set_wid(int wid);

private:
    char* svc_name_;
    char portname[MPI_MAX_PORT_NAME];

    int wid_= 0;

    MPI_Comm sc_comm_;
};


#endif //MPI_CONNECT_MPI_CLIENT_H
