//
// Created by zhaobq on 2016/12/12.
//

#ifndef MPI_CONNECT_MPI_CONNECT_BASE_H
#define MPI_CONNECT_MPI_CONNECT_BASE_H

#include "IRecv_handler.h"
#include "mpi.h"
#include "pthread.h"
#include "MPI_Util.h"
#include <map>
#include <iostream>

using namespace std;

struct SendMSG{ // 用于唤醒send进程后，send进程发送的内容
    void* buf_;
    int msgsize_;
    int dest_;
    int tag_;
    MPI_Datatype datatype_;
    MPI_Comm comm_;

    void init(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm){
        buf_ = buf;
        msgsize_ = msgsize;
        dest_ = dest;
        datatype_ = datatype;
        tag_ = tag;
        comm_ = comm;
    }

    void print(){
    }
};

struct ARGS{    //用于 new_msg_come 向 recv传递参数
    MPI_Comm newcomm;
    int source_rank;
    MPI_Datatype datatype;
    MPI_Status arg_stat;

    void print(){
        cout << "<args_info>: newcomm=" << newcomm << "; source=" << source_rank << "; datatype=" << datatype << "; status:{" << arg_stat.MPI_TAG << ";" <<arg_stat->count << "}" << endl;
    }
};

class MPI_Connect_Base {
protected:
    IRecv_handler *Irecv_handler;
    pthread_cond_t send_thread_cond;      //  用于挂起读/写线程时
    pthread_mutex_t recv_mtx, send_mtx, sendmsg_mtx;                     //  同上
    pthread_t recv_t, send_t;

    int myrank;
    int w_size;
    static int merr = 0;
    static int msglen = 0;
    static char errmsg[MPI_MAX_ERROR_STRING];
    static char hostname[MPI_MAX_PROCESSOR_NAME];

    bool recv_flag = true; //true = stop false = running
    bool send_flag = true;

    SendMSG sendmsg;

public:

    MPI_Connect_Base(IRecv_handler* rh){
        Irecv_handler = rh;
//      recv_thread_cond = PTHREAD_COND_INITIALIZER;
        send_thread_cond = PTHREAD_COND_INITIALIZER;
        recv_mtx = PTHREAD_MUTEX_INITIALIZER;
        send_mtx = PTHREAD_MUTEX_INITIALIZER;

        //MPI_Init(0,0);
        //MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
        //MPI_Comm_size(MPI_COMM_WORLD, &w_size);
        //MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
    };

    virtual ~MPI_Connect_Base(){};

    virtual int initialize(){cout << "[Error] father init..." << endl; return 0;};
    virtual void run(){cout << "[Error] father run..." << endl;};
    virtual int stop(){cout << "[Error] father stop..." << endl; return 0;};
    virtual int finalize(){ return 0;};

    static void* recv_thread(void* ptr);
    static void* send_thread(void* ptr);


    virtual bool new_msg_come(ARGS * args);
    virtual MPI_Datatype analyz_type(int tags);

    virtual void send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    //virtual void err_handler();
    void set_recv_stop();
    void set_send_stop();

    virtual void recv_handle(int tag, void* buf, MPI_Comm comm){cout << "[Error] father recv handler" << endl;}; //


};


#endif //MPI_CONNECT_MPI_CONNECT_BASE_H
