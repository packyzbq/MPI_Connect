//
// Created by zhaobq on 2017/1/9.
//

#ifndef MPI_CONNECT_IRECV_BUFFER_H
#define MPI_CONNECT_IRECV_BUFFER_H


#include <string>
#include <pthread.h>
#include <queue>
#include <cstdlib>
using namespace std;

struct Pack{
    int tag;
    int ibuf=0;
    string sbuf="";
};

struct IRecv_buffer{
    queue<Pack> buffer;
    pthread_mutex_t mutex;

    IRecv_buffer(){
        mutex = PTHREAD_MUTEX_INITIALIZER;
    };

    void put(Pack p){
        pthread_mutex_lock(&mutex);
        buffer.push(p);
        pthread_mutex_unlock(&mutex);
    };

    Pack get(){
        pthread_mutex_lock(&mutex);
        if(buffer.empty()){
            pthread_mutex_unlock(&mutex);
            Pack p = Pack();
            p.tag = -1;
            return p;
        }
        else {
            Pack p = buffer.front();
            buffer.pop();
            pthread_mutex_unlock(&mutex);
            return p;
        }
    };

    bool empty(){
        pthread_mutex_lock(&mutex);
        bool flag = buffer.empty();
        pthread_mutex_unlock(&mutex);
        return flag;
    };
};
#endif //MPI_CONNECT_IRECV_BUFFER_H
