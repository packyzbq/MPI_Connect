//
// Created by zhaobq on 2016/12/13.
//

#include "MPI_Server.h"
#include <cstring>
#include <iomanip>

#define DEBUG

MPI_Server::MPI_Server(IRecv_buffer* rh, char *svc_name) : MPI_Connect_Base(rh) {
    svc_name_ = svc_name;
    recv_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
    //send_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
    accept_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
    comm_list_mutex = PTHREAD_MUTEX_INITIALIZER;
    //cout << "Irecv_handler ptr= " << Irecv_handler << endl;
};

MPI_Server::~MPI_Server() {
    if(!recv_flag && !send_flag)
        stop();
    cout << "[Server] end..." << endl;
}

int MPI_Server::initialize() {
    cout << "--------------------Server init start-------------------" << endl;
    int merr= 0;
    int msglen = 0;
    char errmsg[MPI_MAX_ERROR_STRING];

    int provided;
    MPI_Init_thread(0,0,MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(hostname, &msglen);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
    cout << "[Server]: Host: " << hostname << ",Proc: "<< myrank << ", Server initialize..." << endl;
    merr = MPI_Open_port(MPI_INFO_NULL, port);
    if(merr){
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Server]: Error in Open port :" << errmsg<<endl;
        //TODO ERROR HANDLE
        return MPI_ERR_CODE::OPEN_PORT_ERR;
    }

    cout << "[Server]: Host: " << hostname << ",Proc: "<< myrank << ",Server opening port on <" << port <<">" << endl;

    merr = MPI_Publish_name(svc_name_, MPI_INFO_NULL, port);
    if(merr){
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Server]: Error in publish_name :" << errmsg<<endl;
        //TODO ERROR HANDLE
        return MPI_ERR_CODE::PUBLISH_SVC_ERR;
    }
    cout << "[Server]: publish service <" << svc_name_ << ">" << endl;
    //MPI_Barrier(MPI_COMM_WORLD);

    //start recv thread
    pthread_create(&recv_t ,NULL, MPI_Connect_Base::recv_thread, this);
    while(true){
        pthread_mutex_lock(&recv_flag_mutex);
        if(!recv_flag) {
            pthread_mutex_unlock(&recv_flag_mutex);
            break;
        }
        pthread_mutex_unlock(&recv_flag_mutex);
    }
    cout << "[Server]: receive thread start..." << endl;
    //recv_thread(this);

    //start send thread
    //pthread_create(&send_t, NULL, MPI_Connect_Base::send_thread, this);
    //while(send_flag);
    //cout << "[Server]: send thread start..." << endl;

    //start accept thread
    pthread_create(&pth_accept, NULL, MPI_Server::accept_conn_thread, this);
    while(true){
        pthread_mutex_lock(&accept_flag_mutex);
        if(!accept_conn_flag){
            pthread_mutex_unlock(&accept_flag_mutex);
            break;
        }
        pthread_mutex_unlock(&accept_flag_mutex);
    }
    cout << "[Server]: accept thread start..." << endl;

    cout << "--------------------Server init finish--------------------" << endl;
    return MPI_ERR_CODE::SUCCESS;
}

int MPI_Server::stop() {
    cout << "--------------------Server stop start--------------------"<< endl;
    int merr= 0;
    int msglen = 0;
    char errmsg[MPI_MAX_ERROR_STRING];
    cout << "[Server]: Ready to stop..." << endl;
    cout << "[Server]: Unpublish service name..." << endl;
    merr = MPI_Unpublish_name(svc_name_, MPI_INFO_NULL, port);
    if(merr){
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Server]: Unpublish service name error: "<< errmsg << endl;
        return MPI_ERR_CODE::UNPUBLISH_ERR;
    }

    if(!comm_list.empty()){
        cout <<"[Server]: client still working ,cannot stop server..." << endl;
        return MPI_ERR_CODE::STOP_ERR;
    }
    // TODO Add force stop
    //stop threads
    set_accept_t_stop();
    set_recv_stop();
    //set_send_stop();

    int ret;
    ret = pthread_cancel(pth_accept);
    cout <<"[Server]: cancel accept thread, exit code=" << ret << endl;

    //ret = pthread_cancel(send_t);
    //cout <<"[Server]: cancel accept thread, exit code=" << ret << endl;

    //map<int ,MPI_Comm>::iterator iter;
    //for(iter = client_comm_list.begin(); iter != client_comm_list.end(); iter++){
    //    MPI_Comm_disconnect(&(iter->second));
    //}

    finalize();
    cout << "--------------------Server stop finish--------------------"  << endl;

    return MPI_ERR_CODE::SUCCESS;
}

int MPI_Server::finalize() {
    int ret;
    ret = pthread_join(pth_accept, NULL);
    cout << "[Server]: accept thread stop, exit code=" << ret << endl;
    ret = pthread_join(recv_t, NULL);
    cout << "[Server]: recv_thread stop, exit code=" << ret << endl;
    //ret = pthread_join(send_t, NULL);
    //cout << "[Server]: send_thread stop, exit code=" << ret << endl;

    //pthread_mutex_destroy(&send_mtx);
    //pthread_mutex_destroy(&sendmsg_mtx);
    //pthread_cond_destroy(&send_thread_cond);

    MPI_Finalize();
    return MPI_ERR_CODE::SUCCESS;
}

bool MPI_Server::new_msg_come(ARGS *args) {
    if(comm_list.empty())
        return false;

    int merr= 0;
    int msglen = 0;
    char errmsg[MPI_MAX_ERROR_STRING];
    MPI_Status *stat;
    int flag;
    list<List_Entry>::iterator iter;
    for(iter = comm_list.begin(); iter != comm_list.end(); iter++){
        stat = new MPI_Status();
        merr = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, iter->comm , &flag, stat);
        if(merr){
            MPI_Error_string(merr, errmsg, &msglen);
            cout << "[Server-Error]: " << errmsg << endl;
            //TODO Add error handle
        }
        if(flag) {
#ifdef DEBUG
            cout << "[Server]: dectect a new msg <source=" << stat->MPI_SOURCE << ";tag=" << stat->MPI_TAG << ">" <<endl;
#endif
            //args = new ARGS();
            args->newcomm = (iter->comm);
            args->arg_stat = *stat;
            args->datatype = analyz_type(stat->MPI_TAG);
            args->source_rank = stat->MPI_SOURCE;
            args->print();
            delete(stat);
            return true;
        }
        //free(flag);
        delete(stat);
    }
    return false;
}

void* MPI_Server::accept_conn_thread(void *ptr) {
    //pthread_t mypid = pthread_self();
    int merr= 0;
    int msglen = 0;
    char errmsg[MPI_MAX_ERROR_STRING];
    pthread_mutex_lock(&(((MPI_Server*)ptr)->accept_flag_mutex));
    ((MPI_Server*)ptr)->accept_conn_flag = false;
    bool acc_flag = ((MPI_Server*)ptr)->accept_conn_flag;
    pthread_mutex_unlock(&(((MPI_Server*)ptr)->accept_flag_mutex));
    cout << "[Server] host: "<< ((MPI_Server*)ptr)->hostname <<", accept connection thread start..." << endl;

    while(!acc_flag) {
        merr = 0;
        MPI_Comm newcomm;
        merr = MPI_Comm_accept(((MPI_Server*)ptr)->port, MPI_INFO_NULL, 0, MPI_COMM_SELF, &newcomm);
        if(merr){
            MPI_Error_string(merr, errmsg, &msglen);
            cout << "[Server-Error]: accept client error, msg: " << errmsg << endl;
        }
        MPI_Barrier(newcomm);
        List_Entry tmp_item;
        tmp_item.comm = newcomm;
        pthread_mutex_lock(&(((MPI_Server*)ptr)->comm_list_mutex));
        ((MPI_Server*)ptr)->comm_list.push_back(tmp_item);
        pthread_mutex_unlock(&(((MPI_Server*)ptr)->comm_list_mutex));
        //TODO receive worker MPI_REGISTEY tags and add to master, in recv_thread() function or ABC recv_commit() function
#ifdef DEBUG
        cout << "[Server]:Host: " << ((MPI_Server*)ptr)->hostname << ", Proc: "<< ((MPI_Server*)ptr)->myrank << ", receive new connection...; MPI_COMM="<< newcomm << endl;
#endif
        //TODO add to bcast_comm/group
        //delete(&tmp_item);
        //free(&newcomm);

        pthread_mutex_lock(&(((MPI_Server*)ptr)->accept_flag_mutex));
        acc_flag = ((MPI_Server*)ptr)->accept_conn_flag;
        pthread_mutex_unlock(&(((MPI_Server*)ptr)->accept_flag_mutex));
    }
    cout << "[Server] host: "<< ((MPI_Server*)ptr)->hostname << ", accept connection thread stop..." << endl;
    return 0;
}

void MPI_Server::recv_handle(int tag, void *buf, MPI_Datatype type,MPI_Comm comm) {
    //TODO set different conditions
    int merr, msglen;
    string msg = (char *)buf;
    char errmsg[MPI_MAX_ERROR_STRING];

    switch(tag){
        case MPI_REGISTEY: {
#ifdef DEBUG
            cout << "get a registery from worker:" << msg << endl;
#endif
            if(comm_list.size() == 0)
                cout << "[Server-Error]: comm_list has no MPI_Comm" << endl;
            //pack = new Recv_Pack((*(int *) buf), NULL);
            list<List_Entry>::iterator iter;
            int size = 0;
            pthread_mutex_lock(&comm_list_mutex);
            for (iter = comm_list.begin(); iter != comm_list.end(); iter++, size++) {
                if (iter->comm == comm) {
                    iter->uuid = msg;
#ifdef DEBUG
                    cout << "[Server]: register worker " << msg << " success" << endl;
#endif
                }
            }
            if (size > comm_list.size()) {
                cout << "[Server-Error]: register error, no compatible MPI_COMM" << endl;
                //TODO Add error handle
            }
            pthread_mutex_unlock(&comm_list_mutex);
        }
            break;
        case MPI_DISCONNECT:{
            cout << "[Server] worker :" << msg<< " require disconnect" << endl;
            //pack = new Recv_Pack((*(int *) buf), NULL);
            bool found = false;
            list<List_Entry>::iterator iter;
            pthread_mutex_lock(&comm_list_mutex);
            for(iter = comm_list.begin(); iter != comm_list.end(); iter++){
                if(iter->comm == comm && iter->uuid == msg){
                    found = true;
                    merr = MPI_Comm_disconnect(&(iter->comm));
                    if(merr){
                        MPI_Error_string(merr, errmsg, &msglen);
                        cout <<"[Server-Error]: disconnect error: " << errmsg << endl;
                    }
#ifdef DEBUG
                    cout << "[Server]: find MPI_Comm and wid, removing worker..." << endl;
#endif
                    comm_list.erase(iter);
                    break;
                }
            }
            pthread_mutex_unlock(&comm_list_mutex);
            if(!found) {
#ifdef DEBUG
                cout << "[Server-Error]: can't find correspond MPI_Comm and wid" << endl;
#endif
            }
        }
            break;
        default: {
            //Irecv_handler->handler_recv(tag, buf);
            cout << "[Server-Error]: unrecorgnized type" << endl;
            break;
        }
    }
/*    if(type == MPI_INT) {
        Pack_Int pack = Pack_Int((*(int *) buf));
#ifdef DEBUG
        cout << "irecv_handler ptr=" << Irecv_handler << endl;
        Irecv_handler->handler_recv_int(tag, pack);
    }
    else if(type == MPI_CHAR) {
        Pack_Str pack = Pack_Str((char *) buf);
        Irecv_handler->handler_recv_str(tag, pack);
    }
    else {
#ifdef DEBUG
        cout << "[Server-Error]: Recv datatype error" << endl;
#endif
        //TODO add error handler
    }
*/
    //delete(pack);
}

//void MPI_Server::send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm) {
//    cout << "[Server]: send message..." << endl;
//    MPI_Connect_Base::send(buf, msgsize, dest, datatype, tag, comm);
//    cout << "[Server]: send finish, send thread sleep..." << endl;
//}

int MPI_Server::send_string(char *buf, int msgsize, string dest_uuid, int tag) {
#ifdef DEBUG
    cout << "[Server]: send message...<" << buf <<","<<dest_uuid <<"," <<tag  << ">"<< endl;
#endif
    int merr = 0;
    int msglen = msgsize;
    char errmsg[MPI_MAX_ERROR_STRING];
    MPI_Comm send_comm = NULL;
    list<List_Entry>::iterator iter;
    for (iter = comm_list.begin(); iter != comm_list.end(); iter++){
        if(iter->uuid == dest_uuid)
            send_comm = iter->comm;
    }
    if(send_comm == NULL) {
#ifdef  DEBUG
        cout << "[Server-Error]: can't find send comm" << endl;
#endif
        //TODO add error handler
    }
    merr = MPI_Send(buf, msgsize, MPI_CHAR, 0, tag, send_comm);
    if(merr){
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Server-Error]: send fail...error: " << errmsg << endl;
        return MPI_ERR_CODE::SEND_FAIL;
    }
#ifdef DEBUG
    cout << "[Server]: start barrier..." << endl;
#endif
    merr = MPI_Barrier(send_comm);
    if(merr){
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Server-Error]: barrier fail...error: " << errmsg << endl;
        return MPI_ERR_CODE::BARRIER_FAIL;
    }
#ifdef DEBUG
    cout << "[Server]: start barrier..." << endl;
#endif
    return MPI_ERR_CODE::SUCCESS;
}

int MPI_Server::send_int(int buf, int msgsize, string dest_uuid, int tag){
#ifdef DEBUG
    cout << "[Server]: send message...<" << buf <<","<<dest_uuid <<"," <<tag  << ">"<< endl;
#endif
    int merr = 0;
    int msglen = msgsize;
    char errmsg[MPI_MAX_ERROR_STRING];
    MPI_Comm send_comm = NULL;
    list<List_Entry>::iterator iter;
    for (iter = comm_list.begin(); iter != comm_list.end(); iter++){
        if(iter->uuid == dest_uuid)
            send_comm = iter->comm;
    }
    merr = MPI_Send(&buf, msgsize, MPI_INT, 0, tag, send_comm);
    if(merr){
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Server-Error]: send fail...error: " << errmsg << endl;
        return MPI_ERR_CODE::SEND_FAIL;
    }
#ifdef DEBUG
    cout << "[Server]: start barrier..." << endl;
#endif
    merr = MPI_Barrier(send_comm);
    if(merr){
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Server-Error]: barrier fail...error: " << errmsg << endl;
        return MPI_ERR_CODE::BARRIER_FAIL;
    }
#ifdef DEBUG
    cout << "[Server]: start barrier..." << endl;
#endif
    return MPI_ERR_CODE::SUCCESS;
}

void MPI_Server::run() {

    // TODO Server work flow, add exception handle

    initialize();

    //TODO add some work
}

bool MPI_Server::disconnect_client(int w_uuid) {
    //TODO
}

void MPI_Server::bcast(void *buf, int msgsz, MPI_Datatype datatype, int tags) {
    //TODO
}