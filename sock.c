#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <errno.h>

#include "sock.h"

int tcp_server_init(int listen_port) {
    int ret;
    int fd;
    fd= socket(AF_INET, SOCK_STREAM, 0);
    if (fd<0) {
        RTSP_ERR("create tcp socket failed.\n");
        return RTSP_RET_FAIL;
    }
	
    int on = 1;
    ret=setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	RTSP_ASSERT(ret>=0,"set port reuse failed");

	struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(listen_port);
    addr.sin_addr.s_addr=INADDR_ANY;
    ret= bind(fd,(struct sockaddr *)&addr,sizeof(struct sockaddr));
    if (ret<0) {
        RTSP_ERR("bind failed @ errno=%d",errno);
        exit(1);
    } else {
        RTSP_INFO("bind ok");
    }
    ret = listen(fd,32);
    if (ret<0) {
        RTSP_ERR("listen failed @ errno=%d",errno);
        exit(1);
    } else {
        RTSP_INFO("listen start success @%d",RTSP_DEFAULT_PORT);
    }

    return fd;
}


int tcp_client_init(int fd) {
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

int udp_server_init(int port,int timeout)
{
	int sock;
	int ret;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock<0){
		RTSP_ERR("create udp socket failed.");
		return -1;
	}
	// set addr reuse
    int on = 1;
    ret=setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	RTSP_ASSERT(ret>=0,"set port reuse failed");
	//set send timeout
	struct timeval timeo = {timeout, 0};
	ret=setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
	RTSP_ASSERT(ret>=0,"set send timeout failed.");
	//set receive timeout
	ret=setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,&timeo,sizeof(timeo));
	RTSP_ASSERT(ret>=0,"set receive timeout failed.");
	struct sockaddr_in my_addr;
	bzero(&my_addr,sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	//bind 
	ret = bind(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
	RTSP_ASSERT(ret>=0,"bind failed.");
	RTSP_DEBUG("create udp port:%d(sock:%d) ok.",port,sock);

	return sock;
}

int udp_client_init(int timeout)
{
	int sock;
	int ret;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock<0){
		RTSP_ERR("create udp socket failed.");
		return -1;
	}
	//set send timeout
	struct timeval timeo = {timeout, 0};
	ret=setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
	RTSP_ASSERT(ret>=0,"set send timeout failed.");
	//set receive timeout
	ret=setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,&timeo,sizeof(timeo));
	RTSP_ASSERT(ret>=0,"set receive timeout failed.");
	RTSP_DEBUG("create udp sock:%d ok.",sock);

	return sock;
}


int tcp_read(Rtsp_t *r,char *buf,int size) {
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

int tcp_write(Rtsp_t *r,char *buf,int size) {
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

int udp_read(Rtsp_t *r,char *buf,int size) 
{
	return 0;
}

int udp_write(Rtsp_t *r,char *buf,int size) 
{
	return 0;
}


