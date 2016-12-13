//
// Created by zhaobq on 2016/12/13.
//

#ifndef MPI_CONNECT_MPI_SERVER_H
#define MPI_CONNECT_MPI_SERVER_H

#include <list>
#include "MPI_Connect_Base.h"


struct List_Entry{
    MPI_Comm comm;
    int wid = -1;

};

class MPI_Server : public  MPI_Connect_Base{
private:
    char* svc_name_;
    list<List_Entry> comm_list;
    char port[MPI_MAX_PORT_NAME];

    pthread_t pth_accept;
    bool accept_conn_flag = true;

public:
    MPI_Server(IRecv_handler *rh, char* svc_name) : MPI_Connect_Base(rh) {
        svc_name_ = svc_name;
    };

    ~MPI_Server(){cout<< "[Server]: ended..." << endl;}

    void run();
    int initialize();
    int stop();
    int finalize();

    bool new_msg_come(ARGS *args);
    void recv_handle(int tag, void* buf, MPI_Comm comm);

    void send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);

    static void* accept_conn_thread(void* ptr);

    bool gen_client();
    bool disconnect_client(int w_uuid);
    void bcast(void *buf, int msgsz, MPI_Datatype datatype, int tags);

    void set_accept_t_stop(){accept_conn_flag = true;};
};


#endif //MPI_CONNECT_MPI_SERVER_H
