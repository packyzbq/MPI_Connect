//
// Created by zhaobq on 2016/12/12.
//

#include "MPI_Connect_Base.h"
#include <cstdlib>
#include <iostream>

#define DEBUG
using namespace std;

void* MPI_Connect_Base::recv_thread(void *ptr) {
    int msgsz, merr, msglen;
    void* rb;
    char errmsg[MPI_MAX_ERROR_STRING];
    ((MPI_Connect_Base*)ptr)->recv_flag = false;

    pthread_t pid;
    pid = pthread_self();
    ARGS* args = new ARGS;
    MPI_Status recv_st;

    MPI_Comm_rank(MPI_COMM_WORLD, &(((MPI_Connect_Base*)ptr)->myrank));
    MPI_Comm_size(MPI_COMM_WORLD, &(((MPI_Connect_Base*)ptr)->w_size));

#ifdef DEBUG
    cout<<"<thread_recv>: Proc: "<< ((MPI_Connect_Base*)ptr)->myrank << ", Pid: " << pid << ", receive thread start...  "<<endl;
#endif
    // TODO add exception handler -> OR add return code
    while(!((MPI_Connect_Base*)ptr)->recv_flag){
        if(((MPI_Connect_Base*)ptr)->new_msg_come(args)){
#ifdef DEBUG
            cout <<"<thread_recv>: detect a new message" << endl;
            args->print();
#endif
            MPI_Get_count(&(args->arg_stat), args->datatype, &msgsz);
            switch (args->datatype)
            {
                case MPI_INT:
                    rb = new int[msgsz];
                    break;
                case MPI_CHAR:
                    rb = new char[msgsz];
                    break;
                default:
                    rb = new char[msgsz];
                    break;
            }
            merr = MPI_Recv(rb, msgsz, args->datatype, args->arg_stat.MPI_SOURCE, args->arg_stat.MPI_TAG, args->newcomm, &recv_st);
            if(merr){
                MPI_Error_string(merr, errmsg, &msglen);
                cout << "<thread_recv>: receive error: " << errmsg << endl;
                //TODO error handle return code
            }
#ifdef DEBUG
            cout << "<thread_recv>: receive a message <-- <" << rb << ">" << endl;
#endif
            MPI_Barrier(args->newcomm);
#ifdef DEBUG
            cout << "<thread_recv>: handled by recv_handler" << endl;
#endif
            ((MPI_Connect_Base*)ptr)->recv_handle(args->arg_stat.MPI_TAG, rb, args->newcomm);

        }
    }

    return 0;
}

void* MPI_Connect_Base::send_thread(void *ptr) {
    //TODO add return code
    ((MPI_Connect_Base*)ptr)->send_flag = false;
    //发送函数，在平时挂起，使用 send唤醒 来发送信息
    pthread_t pid = pthread_self();
    //SendMSG* smsg;

#ifdef DEBUG
    cout<< "<send_thread>: Proc: "<< "Send thread start..., pid = " << pid << endl;
#endif
    pthread_mutex_lock(&(((MPI_Connect_Base*)ptr)->send_mtx));
    while(!((MPI_Connect_Base*)ptr)->send_flag){

        pthread_cond_wait(&(((MPI_Connect_Base*)ptr)->send_thread_cond), &(((MPI_Connect_Base*)ptr)->send_mtx));
        pthread_mutex_lock(&(((MPI_Connect_Base*)ptr)->sendmsg_mtx));

        smsg = ((MPI_Connect_Base*)ptr)->getSendmsg();
#ifdef DEBUG
        cout << "<send_thread>: Send restart..., send msg =<" << smsg->buf_ << "," << smsg->dest_ <<"," <<smsg->tag_ << endl;
#endif
        MPI_Send(smsg->buf_, smsg->msgsize_, smsg->datatype_, smsg->dest_, smsg->tag_, smsg->comm_);
#ifdef DEBUG
        cout << "<send_thread>: Send finish..." << endl;
#endif
        pthread_mutex_unlock(&(((MPI_Connect_Base*)ptr)->sendmsg_mtx));
    }
    pthread_mutex_unlock(&(((MPI_Connect_Base*)ptr)->send_mtx));

    return 0;
}