//
// Created by zhaobq on 2016/12/12.
//

#ifndef MPI_CONNECT_MPI_UTIL_H
#define MPI_CONNECT_MPI_UTIL_H

//传输用的tags， 每个tags中包括了传输的数据类型
class MPI_Tags{
public:
    const static int MPI_RECV_INT         = 0;
    const static int MPI_RECV_CHAR        = 1;
    const static int MPI_SEND_INT         = 2;
    const static int MPI_SEND_CHAR        = 3;
    const static int MPI_BCAST_INT        = 4;
    const static int MPI_BCAST_CHAR       = 5;

    const static int MPI_HEART_BEAT       = 10;
    const static int MPI_REGISTEY         = 11;
    const static int MPI_DISCONNECT       = 12;

    const static int MPI_BCAST_REQ        = 20; //用于广播询问task剩余情况
    const static int MPI_BCAST_ACK       = 21; //用于回复广播询问task
};


#endif //MPI_CONNECT_MPI_UTIL_H
