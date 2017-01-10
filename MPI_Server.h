//
// Created by zhaobq on 2016/12/13.
//

#ifndef MPI_CONNECT_MPI_SERVER_H
#define MPI_CONNECT_MPI_SERVER_H

#include <list>
#include "MPI_Connect_Base.h"


struct List_Entry{
    MPI_Comm comm;
    string uuid = "-1";

};

class MPI_Server : public  MPI_Connect_Base{
private:
    char* svc_name_;
    list<List_Entry> comm_list;
    pthread_mutex_t comm_list_mutex;
    char port[MPI_MAX_PORT_NAME];

    pthread_t pth_accept;
    pthread_mutex_t accept_flag_mutex;
    bool accept_conn_flag = true;

public:
    MPI_Server(IRecv_buffer* rh, char* svc_name);

    ~MPI_Server();

    void run();
    int initialize();
    int stop();
    int finalize();

    bool new_msg_come(ARGS *args);
    void recv_handle(int tag, void* buf, MPI_Datatype type,MPI_Comm comm);

//    void send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    int send_string(char *buf, int msgsize, string dest_uuid, int tag);
    int send_int(int buf, int msgsize, string dest_uuid, int tag);
    static void* accept_conn_thread(void* ptr);

    bool gen_client();
    bool disconnect_client(int w_uuid);
    void bcast(void *buf, int msgsz, MPI_Datatype datatype, int tags);

    void set_accept_t_stop(){
        pthread_mutex_lock(&accept_flag_mutex);
        accept_conn_flag = true;
        pthread_mutex_unlock(&accept_flag_mutex);
    };
};


#endif //MPI_CONNECT_MPI_SERVER_H
