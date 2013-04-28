#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "rtspdef.h"
#include "rtsplog.h"
#include "rtsplib.h"


static int server_fd=-1;
static int toggle=true;
static int rtsp_user_num=0;

static void signal_handle(int sign_no) 
{
    toggle=false;
    RTSP_DEBUG("rtsp server kill!");
    close(server_fd);
    sleep(2);
    exit(0);
}

void* rtsp_server_process(void *para) 
{
	int ret;
	int *sock=(int *)para;
	Rtsp_t r;
	fd_set read_set;
	fd_set write_set;
	struct timeval timeout={0,10*1000};
	
    pthread_detach(pthread_self());
    RTSP_init(&r, *sock);

	while(1){
		FD_ZERO(&read_set);
		FD_SET(r.sock,&read_set);
		ret=select(r.sock+1,&read_set,NULL,NULL,&timeout);
		if(ret < 0){
			RTSP_ERR("select failed");
			break;
		}else if(ret == 0){
			//
		}else{
			if(FD_ISSET(r.sock,&read_set)){
				if(RTSP_read_message(&r)==RTSP_RET_FAIL){
					break;
				}
				if(RTSP_parse_message(&r) == RTSP_RET_FAIL){
					break;
				}
			}
			if(r.state == RTSP_STATE_PLAYING){
				
			}
		}
	}

	RTSP_destroy(&r);
	
	return NULL;
}


int rtsp_server_main(int argc,char *argv[]) {
    int client_fd;
    signal(SIGINT,signal_handle);
    server_fd=RTSP_SOCK_init3();
    do {
        struct sockaddr client;
        socklen_t size=sizeof(struct sockaddr);
        if ((client_fd = accept(server_fd,(struct sockaddr *)&client,&size)) == -1) {
            RTSP_ERR("accept failed, errno=%d!!!",errno);
            exit(1);
        } else {
            struct sockaddr_in sin=*((struct sockaddr_in *)&client);
            RTSP_INFO("accept connect from:%s sockfd:%d",inet_ntoa(sin.sin_addr),client_fd);

            pthread_t tid=0;
            if (pthread_create(&tid,NULL,rtsp_server_process,(void *)&client_fd) != 0) {
                RTSP_ERR("create thread failed!");
                exit(1);
            } else {
                RTSP_INFO("create thread success , tid=%d",(int)tid);
            }
        }
    } while (toggle);

    return 0;
}



