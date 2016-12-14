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
    void *rb = nullptr;
    char errmsg[MPI_MAX_ERROR_STRING];

    pthread_t pid;
    pid = pthread_self();
    ARGS* args = new ARGS;
    MPI_Status recv_st;

    MPI_Comm_rank(MPI_COMM_WORLD, &(((MPI_Connect_Base*)ptr)->myrank));
    MPI_Comm_size(MPI_COMM_WORLD, &(((MPI_Connect_Base*)ptr)->w_size));

#ifdef DEBUG
    cout<<"<recv thread>: Proc: "<< ((MPI_Connect_Base*)ptr)->myrank << ", Pid: " << pid << ", receive thread start...  "<<endl;
#endif
    ((MPI_Connect_Base*)ptr)->recv_flag = false;
    // TODO add exception handler -> OR add return code
    while(!((MPI_Connect_Base*)ptr)->recv_flag){
        if(((MPI_Connect_Base*)ptr)->new_msg_come(args)){
#ifdef DEBUG
            cout <<"<recv thread>: detect a new message" << endl;
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
                cout << "<recv thread>: receive error: " << errmsg << endl;
                //TODO error handle return code
            }
#ifdef DEBUG
            cout << "<recv thread>: receive a message <-- <" << rb << ">" << endl;
#endif
            MPI_Barrier(args->newcomm);
#ifdef DEBUG
            cout << "<recv thread>: handled by recv_handler" << endl;
#endif
            ((MPI_Connect_Base*)ptr)->recv_handle(args->arg_stat.MPI_TAG, rb, args->newcomm);

        }
        if(!rb)
            delete(rb);
    }


//    delete(&merr);
//    delete(&errmsg);
//    delete(&msglen);
//    delete(&msgsz);
    delete(args);
    return 0;
}

/*
void* MPI_Connect_Base::send_thread(void *ptr) {
    //TODO add return code

    //发送函数，在平时挂起，使用 send唤醒 来发送信息
    pthread_t pid = pthread_self();
    SendMSG* smsg;

    int merr = 0;
    int msglen = 0;
    char errmsg[MPI_MAX_ERROR_STRING];

#ifdef DEBUG
    cout<< "<send_thread>: Proc: " << ((MPI_Connect_Base*)ptr)->myrank << ", Send thread start..., pid = " << pid << endl;
#endif
    ((MPI_Connect_Base*)ptr)->send_flag = false;
    pthread_mutex_lock(&(((MPI_Connect_Base*)ptr)->send_mtx));
    while(!((MPI_Connect_Base*)ptr)->send_flag){

        pthread_cond_wait(&(((MPI_Connect_Base*)ptr)->send_thread_cond), &(((MPI_Connect_Base*)ptr)->send_mtx));
        pthread_mutex_lock(&(((MPI_Connect_Base*)ptr)->sendmsg_mtx));

        smsg = &(((MPI_Connect_Base*)ptr)->sendmsg);
#ifdef DEBUG
        cout << "<send_thread>: Send restart..., send msg =<" << smsg->buf_ << "," << smsg->dest_ <<"," <<smsg->tag_ << ">" << endl;
#endif
        merr = MPI_Send(smsg->buf_, smsg->msgsize_, smsg->datatype_, smsg->dest_, smsg->tag_, smsg->comm_);
        if(merr){
            MPI_Error_string(merr, errmsg, &msglen);
            cout << "<send_thread>: send error: " << errmsg << endl;
            //TODO error handle return code
        }
#ifdef DEBUG
        cout << "<send_thread>: Send finish..." << endl;
#endif
        pthread_mutex_unlock(&(((MPI_Connect_Base*)ptr)->sendmsg_mtx));
    }
    pthread_mutex_unlock(&(((MPI_Connect_Base*)ptr)->send_mtx));

//    delete(smsg);
//    delete(&merr);
//    delete(&msglen);
//    delete(&errmsg);
    return 0;
}
*/

/*
void MPI_Connect_Base::send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm) {
    //唤醒send_thread， 发送信息

    pthread_mutex_lock(&send_mtx);
    pthread_mutex_lock(&sendmsg_mtx);
    sendmsg.init(buf, msgsize, dest, datatype, tag, comm);
    pthread_mutex_unlock(&sendmsg_mtx);
    pthread_cond_signal(&send_thread_cond);
    pthread_mutex_unlock(&send_mtx);
}
*/
bool MPI_Connect_Base::new_msg_come(ARGS *args) {
    cout << "[Error]: father function, error to reach" << endl;
    return NULL;
}

MPI_Datatype MPI_Connect_Base::analyz_type(int tags) {
    if (tags >= 10)
        return MPI_PACKED;
    else if(tags == 0 || tags % 2 == 0)
        return MPI_INT;
    else
        return MPI_CHAR;
}

void MPI_Connect_Base::set_recv_stop() {
    recv_flag = true;
}

void MPI_Connect_Base::set_send_stop() {
    send_flag = true;
}