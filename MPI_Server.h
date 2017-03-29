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
    int ept_worker_no;
    list<List_Entry> comm_list;
    pthread_mutex_t comm_list_mutex;
    char port[MPI_MAX_PORT_NAME];

    pthread_t pth_accept;
    pthread_mutex_t accept_flag_mutex;
    bool accept_conn_flag = true;

    bool allowstop = false;

public:
    MPI_Server(IRecv_buffer* rh, char* svc_name, int excepted_worker_no);

    ~MPI_Server();

    void run();
    int initialize();
    int stop();
    int finalize();

    bool new_msg_come(ARGS *args);
    void recv_handle(int tag, void* buf, int length, MPI_Datatype type,MPI_Comm comm);

//    void send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    int send_string(char *buf, int msgsize, string dest_uuid, int tag);
    int send_int(int buf, int msgsize, string dest_uuid, int tag);
    static void* accept_conn_thread(void* ptr);

    bool disconnect_client(int w_uuid);
    void bcast(void *buf, int msgsz, MPI_Datatype datatype, int tags);

    void set_accept_t_stop(){
        pthread_mutex_lock(&accept_flag_mutex);
        accept_conn_flag = true;
        pthread_mutex_unlock(&accept_flag_mutex);
    };
    int get_Commlist_size(){
        return comm_list.size();
    };
    bool get_stop_permit(){
        return allowstop;
    };
    void print_Commlist(){
        list<List_Entry>::iterator iter;
        pthread_mutex_lock(&comm_list_mutex);
        for(iter = comm_list.begin(); iter != comm_list.end(); iter++){
            cout << iter->uuid <<" :: " << iter->comm << endl;
        }
        pthread_mutex_unlock(&comm_list_mutex);
    };
};


#endif //MPI_CONNECT_MPI_SERVER_H
