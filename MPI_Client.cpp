//
// Created by zhaobq on 2016/12/13.
//

#include "MPI_Client.h"
#include <cstring>
#include <iomanip>

#define DEBUG

MPI_Client::MPI_Client(IRecv_handler *mh, char* svc_name, char* port):MPI_Connect_Base(mh){
    if((svc_name == NULL && port != NULL) ) {
        strcpy(portname, port);
        svc_name_ = "";
    }
    else if((svc_name != NULL && port == NULL) || (svc_name != NULL && port != NULL)) {
        svc_name_= svc_name;
        strcpy(portname, "");
    }
    else {
        cout << "[Client-Error]: client construct error, no service name either portname" << endl;
        //TODO add error handle
    }
}

MPI_Client::~MPI_Client() {
    //TODO delete someting
    if(!recv_flag && !send_flag)
        stop();
}

int MPI_Client::initialize() {
    cout << setfill('-') << setw(10) << "Client init start" << setfill('-') << setw(10) << endl;
    cout << "[Client]: client initail..." << endl;
    int merr= 0;
    int msglen = 0;
    char errmsg[MPI_MAX_ERROR_STRING];

    int provide;
    MPI_Init_thread(0,0, MPI_THREAD_MULTIPLE, &provide);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    pthread_create(&recv_t, NULL, MPI_Connect_Base::recv_thread, this);
    while(recv_flag);
    cout << "[Client]: recv thread start...." << endl;
    //recv_thread(this);
    //pthread_create(&send_t, NULL, MPI_Connect_Base::send_thread, this);
    //while(send_flag);
    //cout << "[Client]: send thread start...." << endl;

    if(strlen(portname) == 0) {
        cout << "[Client]: finding service name <" << svc_name_ << "> ..." <<endl;
        merr = MPI_Lookup_name(svc_name_, MPI_INFO_NULL, portname);
        if(merr){
            MPI_Error_string(merr, errmsg, &msglen);
            cout << "[Client-error]: Lookup service name error, msg: "<< errmsg << endl;
            return MPI_ERR_CODE::LOOKUP_SVC_ERR;
            //TODO Add error handle
        }
    }
    cout << "[Client]: service found on port:<" << portname << ">" << endl;

    //while(recv_flag || send_flag);
    merr = MPI_Comm_connect(portname, MPI_INFO_NULL,0, MPI_COMM_SELF, &sc_comm_);
    if(merr){
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Client-error]: Connect to Server error, msg: " << errmsg << endl;
        return MPI_ERR_CODE::CONNECT_ERR;
        //TODO Add error handle
    }
    cout << "[Client]: client connect to server, comm = " << sc_comm_ << endl;
    int rank;
    MPI_Comm_rank(sc_comm_,&rank);
    cout << setfill('-') << setw(10) << "Client init finish" << setfill('-') << setw(10) << endl;
    //send(&wid_, 1, 0, MPI_INT, MPI_Tags::MPI_REGISTEY, sc_comm_);
    send_action(&wid_, 1, MPI_INT, 0, MPI_Tags::MPI_REGISTEY, sc_comm_);

    return MPI_ERR_CODE::SUCCESS;
}

int MPI_Client::stop() {
    cout << setfill('-') << setw(10) << "stop Client" << setfill('-') << setw(10) << endl;
    cout << "[Client]: stop Client..." << endl;
    int merr= 0;
    int msglen = 0;
    char errmsg[MPI_MAX_ERROR_STRING];
    recv_flag = true;
    send_flag = true;
    //TODO add disconnect send
    int tmp = 0;
    //send(&tmp, 1, 0, MPI_INT, MPI_Tags::MPI_DISCONNECT, sc_comm_);
    send_action(&tmp, 1, MPI_INT, 0, MPI_Tags::MPI_DISCONNECT, sc_comm_);
    //pthread_cancel(send_t);
    merr = MPI_Comm_disconnect(&sc_comm_);
    if(merr){
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Client-Error]: disconnect error :" << errmsg << endl;
        return MPI_ERR_CODE::DISCONN_ERR;
    }
    cout << "[Client]: disconnected..." << endl;
    finalize();
    cout << setfill('-') << setw(10) << "Client stop finish" << setfill('-') << setw(10) << endl;
    return MPI_ERR_CODE::SUCCESS;
}

int MPI_Client::finalize() {
    int ret;
//    ret = pthread_join(send_t, NULL);
//    cout <<"[Client]: send thread stop, exit code=" << ret << endl;
    ret = pthread_join(recv_t, NULL);
    cout <<"[Client]: recv thread stop, exit code=" << ret << endl;
//    pthread_mutex_destroy(&send_mtx);
//    pthread_mutex_destroy(&sendmsg_mtx);
//    pthread_cond_destroy(&send_thread_cond);
//    MPI_Finalize();
    return 0;
}

bool MPI_Client::new_msg_come(ARGS *args) {
    if(sc_comm_ == 0x0)
        return false;
    int merr = 0;
    int msglen = 0;
    char errmsg[MPI_MAX_ERROR_STRING];

    MPI_Status *stat = new MPI_Status;
    int flag = 0;
    merr = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, sc_comm_, &flag, stat);
    if (merr) {
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Client-Error]: Iprobe message error :" << errmsg << endl;
        return false;
    }
    if (flag) {
        args = new ARGS();
        args->arg_stat = *stat;
        args->datatype = analyz_type(stat->MPI_TAG);
        args->source_rank = stat->MPI_SOURCE;
        args->newcomm = sc_comm_;
        flag = 0;
        delete (stat);
        return true;
    } else {
        delete(stat);
        return false;
    }
}

//void MPI_Client::send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm) {
//    cout << "[Client]: send message...<" << (*(int*)buf)<< ","<< msgsize << "," << dest << ">"<< endl;
//    MPI_Connect_Base::send(buf, msgsize, dest, datatype, tag, comm);
//    cout << "[Client]: send finish, send thread sleep..." << endl;
//}

int MPI_Client::send_action(void *buf, int msgsize, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
#ifdef DEBUG
        cout << "[Client]: send message...<" << buf <<","<<dest <<"," <<tag  << ">"<< endl;
#endif
        int merr = 0;
        int msglen = 0;
        char errmsg[MPI_MAX_ERROR_STRING];

        merr = MPI_Send(buf, msgsize, datatype, dest,  tag, comm);
        if(merr){
            MPI_Error_string(merr, errmsg, &msglen);
            cout << "[Client-Error]: send fail...error: " << errmsg << endl;
            return MPI_ERR_CODE::SEND_FAIL;
        }
    MPI_Barrier(comm);
        return MPI_ERR_CODE::SUCCESS;
}

void MPI_Client::run() {
    // TODO main thread for client
    initialize();
}

void MPI_Client::recv_handle(int tag, void *buf, MPI_Comm comm) {
    // TODO add conditions
    int merr, msglen;
    char errmsg[MPI_MAX_ERROR_STRING];
    switch (tag){
        case MPI_Tags::MPI_BCAST_REQ:{}
            break;
        case MPI_Tags::MPI_DISCONNECT:{
            MPI_Barrier(comm);
            if(comm != sc_comm_) {
#ifdef DEBUG
                cout << "[Client-Error]: disconnect error: MPI_Comm is not matching" << endl;
#endif
                //TODO error handle
            }
            merr = MPI_Comm_disconnect(&sc_comm_);
            if(merr){
                MPI_Error_string(merr, errmsg, &msglen);
                cout << "[Client-Error]: disconnect error: " << errmsg << endl;
                //TODO Add error handle
            }
        }
        default:
            Irecv_handler->handler_recv(tag, buf);
    }

}

void MPI_Client::set_wid(int wid) {
    wid_ = wid;
}