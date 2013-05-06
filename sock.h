#ifndef __RTSP_SOCK_H__
#define __RTSP_SOCK_H__

#include "rtspdef.h"
#include "rtsplog.h"
#include "rtsplib.h"


int tcp_server_init(int listen_port);
int tcp_client_init(int fd);
int udp_server_init(int port,int timeout);
int udp_client_init(int timeout);
int tcp_read(Rtsp_t *r,char *buf,int size);
int tcp_write(Rtsp_t *r,char *buf,int size);
int udp_read(Rtsp_t *r,char *buf,int size);
int udp_write(Rtsp_t *r,char *buf,int size); 

#endif

