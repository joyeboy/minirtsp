#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <errno.h>

#include "sock.h"

int RTSP_SOCK_init(int fd) {
    int ret;
    //set send timeout
    struct timeval timeo = {
        RTSP_SOCK_TIMEOUT, 0
    };
    ret=setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
	if(ret < 0){
    	RTSP_ERR("set send timeout failed.");
		exit(0);
	}
    //set receive timeout
    ret=setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeo,sizeof(timeo));
	if(ret < 0){
    	RTSP_ERR("set receive timeout failed.");
		exit(0);
	}
    int on = 1;
    ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));
	if(ret < 0){
    	RTSP_ERR("set nodelay failed.");
		exit(0);
	}
    return ret;
}

int RTSP_SOCK_init2(char *ip,int port) {
    int ret;
    int fd;
    fd= socket(AF_INET, SOCK_STREAM, 0);
    if (fd<0) {
        RTSP_ERR("create tcp socket failed.\n");
        return RTSP_RET_FAIL;
    }
    //set send timeout
    struct timeval timeo = {
        RTSP_SOCK_TIMEOUT, 0
    };
    ret=setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
	if(ret < 0){
    	RTSP_ERR("set send timeout failed.");
		exit(0);
	}
    //set receive timeout
    ret=setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeo,sizeof(timeo));
	if(ret < 0){
    	RTSP_ERR("set receive timeout failed.");
		exit(0);
	}

    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);
    ret= connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if (ret<0) {
        RTSP_ERR("connect failed @ errno=%d",errno);
        return RTSP_RET_FAIL;
    } else {
        RTSP_DEBUG("connect ok");
    }
    return fd;
}

int RTSP_SOCK_init3() {
    int ret;
    int fd;
    fd= socket(AF_INET, SOCK_STREAM, 0);
    if (fd<0) {
        RTSP_ERR("create tcp socket failed.\n");
        return RTSP_RET_FAIL;
    }

    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(RTSP_DEFAULT_PORT);
    addr.sin_addr.s_addr=INADDR_ANY;
    ret= bind(fd,(struct sockaddr *)&addr,sizeof(struct sockaddr));
    if (ret<0) {
        RTSP_ERR("bind failed @ errno=%d",errno);
        exit(1);
    } else {
        RTSP_INFO("bind ok");
    }
    ret = listen(fd,16);
    if (ret<0) {
        RTSP_ERR("listen failed @ errno=%d",errno);
        exit(1);
    } else {
        RTSP_INFO("listen start success @%d",RTSP_DEFAULT_PORT);
    }

    int on = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));

    return fd;
}


int RTSP_SOCK_read(Rtsp_t *r,char *buf,int size) {
	int fd=r->sock;
    int ret=0;
    int received=0;
    char *pbuf=buf;
	if(size == 0) return 0;
    while (1) {
#if RTSP_DEBUG
//        printf("=");
//		fflush(stdout);
#endif
        ret=recv(fd,pbuf,size-received,0);
        if (ret==-1) {
            if (errno==EAGAIN || errno==EINTR) {
                RTSP_DEBUG("######## tcp recv error %d ############",errno);
                continue;
            } else if (errno==ETIMEDOUT) {
                RTSP_ERR("######## tcp recv time out ##########");
                return 0;
            }
            RTSP_ERR("####### tcp recv error @%d ##############",errno);
            return -1;
        } else if (ret==0) {
            break;
        } else {
            pbuf+=ret;
            received+=ret;
        }
		if(received == size) break;
    }
    //buf[received]=0;
    RTSP_DEBUG("tcp recv %d",received);
#if RTSP_DEBUG
    //printf("%s.\n",buf);
#endif
    return received;
}

int RTSP_SOCK_write(Rtsp_t *r,char *buf,int size) {
	int fd=r->sock;
    int ret=send(fd,buf,size,0);
    if (ret != size) {
        RTSP_ERR("rtmp send @fd=%d error @%d",fd,errno);
        return RTSP_RET_FAIL;
    } else {
        RTSP_DEBUG("rtmp send @fd=%d size:%d success.",fd,size);
    }
    return RTSP_RET_OK;
}



