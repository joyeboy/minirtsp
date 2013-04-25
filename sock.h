#ifndef __RTSP_SOCK_H__
#define __RTSP_SOCK_H__

#include "rtspdef.h"
#include "rtsplog.h"
#include "rtsplib.h"

#define RTSP_SOCK_TIMEOUT	(5)
#define RTSP_DEFAULT_PORT	(554)

int RTSP_SOCK_init(int fd);
int RTSP_SOCK_init2(char *ip,int port);
int RTSP_SOCK_init3();
int RTSP_SOCK_read(Rtsp_t *r,char *buf,int size);
int RTSP_SOCK_write(Rtsp_t *r,char *buf,int size);

#endif

