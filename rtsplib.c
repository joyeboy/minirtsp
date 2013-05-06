/******************************************************************************

  Copyright (C), 20013-, GuangZhou JUAN Electronics Co., Ltd.

 ******************************************************************************
  File Name    : rtsplib.c
  Version       : Initial Draft
  Author        : kaga
  Created       : 2013/04/25
  Last Modified : 2013/04/25
  Description   : rtsp  utils , reference to rfc2326
 
  History       : 
  1.Date        : 2013/04/25
    	Author      : kaga
 	Modification: Created file	
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include <netinet/tcp.h>
//#include <netinet/in.h>
#include "rtsplib.h"
#include "sock.h"


typedef struct AVal {
    char *av_val;
    int av_len;
}AVal;
#define AVC(str)	{str,sizeof(str)-1}
#define AVMATCH(a1,a2)	((a1)->av_len == (a2)->av_len && !memcmp((a1)->av_val,(a2)->av_val,(a1)->av_len))
#define SAVC(x)	static const AVal av_##x = AVC(#x)
#define STR2AVAL(av,str)	av.av_val = str; av.av_len = strlen(av.av_val)

// method definitions
SAVC(OPTIONS);
SAVC(DESCRIBE);
SAVC(ANNOUNCE);
SAVC(SETUP);
SAVC(PLAY);
SAVC(PAUSE);
SAVC(TEARDOWN);
SAVC(GET_PARAMETER);
SAVC(SET_PARAMETER);
SAVC(REDIRECT);
SAVC(RECORD);
//response status code definitions
typedef struct _RStatusCode
{
	int code;
	const char *info;
}RStatusCode_t;
enum{
	RTSP_RSC_CONTINUE = 0,			//// -0
	RTSP_RSC_OK,						////-1
	RTSP_RSC_CREATED,
	RTSP_RSC_LOW_STORAGE,
	RTSP_RSC_MULTI_CHOICES,
	RTSP_RSC_MOVED_PERMANENTLY,		////-5
	RTSP_RSC_MOVED_TEMPORARILY,
	RTSP_RSC_SEE_OTHER,
	RTSP_RSC_NOT_MODIFIED,
	RTSP_RSC_USE_PROXY,
	RTSP_RSC_BAD_REQUEST,			////-10
	RTSP_RSC_UNAUTHORIZED,
	RTSP_RSC_PAYMENT_REQUIRED,
	RTSP_RSC_FORBIDDEN,
	RTSP_RSC_NOT_FOUND,
	RTSP_RSC_METHOD_NOT_ALLOWED,		////-15
	RTSP_RSC_NOT_ACCEPTABLE,
	RTSP_RSC_PROXY_AUTHEN_REQUIRED,
	RTSP_RSC_REQUEST_TIMEOUT,
	RTSP_RSC_GONE,
	RTSP_RSC_LENGTH_REQUIRED,		////-20
	RTSP_RSC_PRECONDITION_FAILED,
	RTSP_RSC_REQUEST_ENTITY_TOOLARGE,
	RTSP_RSC_REQUEST_URI_TOOLARGE,
	RTSP_RSC_UNSUPPORTED_MEDIA,
	RTSP_RSC_PARAMETER_NOT_UNDERSTOOD,	////-25
	RTSP_RSC_CONFERENCE_NOT_FOUND,
	RTSP_RSC_NOT_ENOUGHT_BANDWIDTH,
	RTSP_RSC_SESSION_NOT_FOUND,
	RTSP_RSC_METHOD_NOT_VAILD,
	RTSP_RSC_HEADER_FIELD_NOT_VAILD,		////-30
	RTSP_RSC_INVALID_RANGE,
	RTSP_RSC_PARAMETER_IS_READONLY,
	RTSP_RSC_AGGREGATE_NOT_ALLOWED,
	RTSP_RSC_ONLY_AGGREGATE_ALLOWED,
	RTSP_RSC_UNSUPPORTED_TRANSPORT,		////-35
	RTSP_RSC_DESITIATION_UNREACHABLE,
	RTSP_RSC_INTERNAL_SERVER_ERROR,
	RTSP_RSC_NOT_IMPLEMENTED,
	RTSP_RSC_GAD_GATEWAY,
	RTSP_RSC_SERVICE_UNAVAILABLE,		////-40
	RTSP_RSC_GATEWAY_TIMEOUT,
	RTSP_RSC_RTSP_VERSION_NOTSUPPORTED,
	RTSP_RSC_OPTION_NOTSUPPORTED,
	RTSP_RSC_END							////-44
};


static RStatusCode_t rtspRStatusCodes[]=
{
	{100,"Continue"},					////-0
	{200,"OK"},
	{201,"Created"},
	{250,"Low on Storage Space"},
	{300,"Multiple Choices"},
	{301,"Moved Permanently"},
	{302,"Moved Temporarily"},
	{303,"See Other"},
	{304,"Not MOdified"},
	{305,"Use Proxy"},
	{400,"Bad Request"},				////-10
	{401,"Unauthorized"},
	{402,"Payment Required"},
	{403,"Forbidden"},
	{404,"Not Found"},
	{405,"Method Not Allowed"},
	{406,"Not Acceptable"},
	{407,"Proxy Authentication Required"},
	{408,"Request Time-out"},
	{410,"Gone"},
	{411,"Length Required"},			////-20
	{412,"Precondition Failed"},
	{413,"Request Entity Too Large"},
	{414,"Request-URI Too Large"},
	{415,"Unsupported Media Type"},
	{451,"Parameter Not Understood"},
	{452,"Conference Not Found"},
	{453,"Not Enough Bandwidth"},
	{454,"Session Not Found"},
	{455,"Method Not Valid in This State"},
	{456,"Header Field Not Vaild for Resource"},	////-30
	{457,"Invalid Range"},
	{458,"Parameter Is Read-Only"},
	{459,"Aggregate operation not allowed"},
	{460,"Only aggregate operation allowed"},
	{461,"Unsupported transport"},
	{462,"Destination unreachable"},
	{500,"Internal Server Error"},
	{501,"Not Implemented"},
	{502,"Bad Gateway"},
	{503,"Service Unavailable"},					////-40
	{504,"Gateway Time-out"},
	{505,"RTSP Version not supported"},
	{551,"Option not supported"},					////-43
};


static const char  *rtspMethods[RTSP_METHOD_CNT]=
{
	"DESCRIBE",
	"ANNOUNCE",
	"GET PARAMETER",
	"OPTIONS",
	"PAUSE",
	"PLAY",
	"RECORD",
	"REDIRECT",
	"SETUP",
	"SET_PARAMETER",
	"TEARDOWN",
	//""
};
static const char *rtspAllowMethods="SETUP,OPTIONS,DESCRIBE,PLAY,TEARDOWN";

static inline int http_get_number(const char *src,
	const char *key,int *ret)
{
	char *tmp=strstr(src,key);
	if(tmp == NULL){
		*ret=-1;
		return -1;
	}else{
		tmp+=strlen(key);
		tmp++;
		sscanf(tmp,"%d",ret);
		return 0;
	}
}
static inline int http_get_string(const char *src,
	const char *key,char *ret)
{
	char *tmp=strstr(src,key);
	if(tmp == NULL){
		*ret=0;
		return -1;
	}else{
		tmp+=strlen(key);
		tmp++;
		sscanf(tmp,"%[^\r\n]",ret);
		return 0;
	}
}
static inline int http_get_url(const char *src,char *ip,int *port,char *stream)
{
	char tmp[128];
	char *p=NULL;
	if(sscanf(src,"%*s %s %*s",tmp)==1){
		
		if(strncmp(tmp,"rtsp://",strlen("rtsp://")) != 0){
			RTSP_ERR("request url format wrong");
			return -1;
		}
		if((p = strstr(tmp+strlen("rtsp://"),":")) ==NULL){
			*port = RTSP_DEFAULT_PORT;
			sscanf(tmp,"rtsp://%[^/]%s",ip,stream);
		}else{
			sscanf(tmp,"rtsp://%[^:]:%d%s",ip,port,stream);
		}
		RTSP_DEBUG("ip:%s stream:%s port:%d",ip,stream,*port);
		return 0;
	}else{
		return -1;
	}
}

static int rtsp_check_setup_url(Rtsp_t *r)
{
	RTSP_DEBUG("setup steam:%s",r->stream);
	return RTSP_RET_OK;
}
static int rtsp_parse_transport(Rtsp_t *r,char *buf)
{
	char *p,*q;
	char transport[128];
	p=transport;
	*buf=0;
	if(http_get_string(r->payload,"Transport:",transport) == RTSP_RET_FAIL){
		return RTSP_RET_FAIL;
	}
	if((q=strstr(transport,"RTP/AVP/TCP")) != NULL){
		r->low_transport = RTSP_TRANSPORT_TCP;
		strcat(buf,"RTP/AVP/TCP;");
	}else{
		strcat(buf,"RTP/AVP;");
		r->low_transport = RTSP_TRANSPORT_UDP;
	}
	if((q=strstr(p,"multicast"))!=NULL){
		r->cast_type = RTSP_MULTICAST;
		strcat(buf,"multicast;");
	}else{
		r->cast_type = RTSP_UNICAST;
		strcat(buf,"unicast;");
	}
	if((q=strstr(p,"interleaved"))!=NULL){
		r->b_interleavedMode = true;
		q+=strlen("interleaved=");
		sscanf(q,"%d%*s",&r->channel);
		q=buf+strlen(buf);
		sprintf(q,"interleaved=%d-%d;",r->channel,r->channel + 1);
	}else{
		if((q=strstr(p,"client_port"))!=NULL){
			q+=strlen("client_port=");
			printf("q:%s\n",q);
			sscanf(q,"%d%*s",&r->client_port);
		}
		if((q=strstr(p,"server_port"))!=NULL){
			q+=strlen("server_port=");
			sscanf(q,"%d%*s",&r->server_port);
		}
		q=buf+strlen(buf);
		sprintf(q,"client_port=%d-%d;server_port=%d-%d",
			r->client_port,r->client_port+ 1,r->server_port,r->server_port+1);
	}
	if((q=strstr(p,"mode"))!=NULL){
		q+=strlen("mode=\"");
		if(strcmp(q,"PLAY") == 0){
			r->work_mode =RTSP_MODE_PLAY;
			strcat(buf,";mode=\"PLAY\"");
		}else if(strcmp(q,"RECORD") == 0){
			r->work_mode = RTSP_MODE_RECORD;
			strcat(buf,";mode=\"RECORD\"");
		}
	}
	//ssrc
	// ttl
	//....
	RTSP_DEBUG("parse transport:client_port:%d server_port:%d,%s",
	r->client_port,r->server_port,r->cast_type ? "multicast" : "unicast");
	RTSP_DEBUG("Transport: %s",buf);
	return RTSP_RET_OK;
}

static void rtsp_gmt_time_string(char *ret,int size)
{
	//const char * const szMonth[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul",
	//	"Aug","Sep","Oct","Nov","Dec"};
	time_t t;
	struct tm *ptm;
	time(&t);
	//ptm=gmtime(&t);
	//sprintf(ret,"%2d %s %04d %02d:%02d:%02d GMT",ptm->tm_mday,szMonth[ptm->tm_mon],
	//	ptm->tm_year+1900,ptm->tm_hour,ptm->tm_min,ptm->tm_mday);
	strftime(ret, size, "%a, %b %d %Y %H:%M:%S GMT", gmtime(&t));
}

static int rtsp_send_packet(Rtsp_t *r)
{
	int ret=send(r->sock,r->payload,r->payload_size,0);
	if(ret != r->payload_size){
		RTSP_ERR("rtsp send packet(size:%d) failed.",r->payload_size);
		return RTSP_RET_FAIL;
	}
	RTSP_DEBUG("rtsp send packet(size:%d) ok",r->payload_size);
	return RTSP_RET_OK;
}


static int rtsp_handle_describe(Rtsp_t *r)
{
	int ret;
	char tmp[128];
	const char *format=
		"%s %d %s\r\n"\
		"CSeq: %d\r\n"\
		"Content-Type: %s\r\n"\
		"Content-Length: %d\r\n"\
		"\r\n"\
		"%s";
	if(http_get_string(r->payload,"Accept:",tmp)==RTSP_RET_FAIL){
		return RTSP_RET_FAIL;
	}
	if(strstr(tmp,SDP_MEDIA_TYPE) == NULL){
		RTSP_ERR("unsupport Accept type:%s",tmp);
		// send error ack here...
		return RTSP_RET_FAIL;
	}
	if(r->sdp == NULL){
		if((r->sdp=SDP_new_default(r->stream,r->ip_me))==NULL){
			return RTSP_RET_FAIL;
		}
		SDP_add_h264(r->sdp,SDP_DEFAULT_VIDEO_CONTROL);
	}
	SDP_setup(r->sdp);
	sprintf(r->payload,format,RTSP_VERSION,
		rtspRStatusCodes[RTSP_RSC_OK].code,
		rtspRStatusCodes[RTSP_RSC_OK].info,
		r->cseq,SDP_MEDIA_TYPE,
		strlen(r->sdp->buffer),
		r->sdp->buffer);
	r->payload_size=strlen(r->payload);
	RTSP_DEBUG("ack (size:%d):\r\n%s\r\n",r->payload_size,r->payload);
	
	ret=rtsp_send_packet(r);
	return ret;
}
static int rtsp_handle_announce(Rtsp_t *r)
{
	RTSP_DEBUG("not yet");
	return RTSP_RET_OK;
}
static int rtsp_handle_get_parameter(Rtsp_t *r)
{
	RTSP_DEBUG("not yet");
	return RTSP_RET_OK;
}
static int rtsp_handle_options(Rtsp_t *r)
{
	int ret;
	const char *format=
		"%s %d %s\r\n"\
		"CSeq: %d\r\n"\
		"Public: %s\r\n"
		"\r\n";
	sprintf(r->payload,format,RTSP_VERSION,
		rtspRStatusCodes[RTSP_RSC_OK].code,
		rtspRStatusCodes[RTSP_RSC_OK].info,
		r->cseq,
		rtspAllowMethods);
	r->payload_size = strlen(r->payload);
	RTSP_DEBUG("ack (size:%d):\r\n%s\r\n",r->payload_size,r->payload);
	
	ret=rtsp_send_packet(r);
	return ret;
}
static int rtsp_handle_play(Rtsp_t *r)
{
	int ret;
	char tmp[128];
	const char *format=
		"%s %d %s\r\n"\
		"CSeq: %d\r\n"\
		"Date: %s\r\n"
		"\r\n";
	rtsp_gmt_time_string(tmp,sizeof(tmp));
	sprintf(r->payload,format,RTSP_VERSION,
		rtspRStatusCodes[RTSP_RSC_OK].code,
		rtspRStatusCodes[RTSP_RSC_OK].info,
		r->cseq,
		tmp);
	r->payload_size = strlen(r->payload);
	RTSP_DEBUG("ack (size:%d):\r\n%s\r\n",r->payload_size,r->payload);
	
	ret=rtsp_send_packet(r);
	return ret;
}
static int rtsp_handle_pause(Rtsp_t *r)
{
	RTSP_DEBUG("not yet");
	return RTSP_RET_OK;
}
static int rtsp_handle_record(Rtsp_t *r)
{
	RTSP_DEBUG("not yet");
	return RTSP_RET_OK;
}
static int rtsp_handle_redirect(Rtsp_t *r)
{
	RTSP_DEBUG("not yet");
	return RTSP_RET_OK;
}
static int rtsp_handle_setup(Rtsp_t *r)
{
	int ret;
	char tmp[256];
	const char *format=
		"%s %d %s\r\n"\
		"CSeq: %d\r\n"\
		"Session: %ld\r\n"\
		"Transport: %s\r\n"\
		"\r\n";
	if(rtsp_check_setup_url(r) == RTSP_RET_FAIL){
		RTSP_ERR("session not found");
		// send err ack here
		return RTSP_RET_FAIL;
	}
	if(rtsp_parse_transport(r,tmp)== RTSP_RET_FAIL){
		// send error ack here...
		return RTSP_RET_FAIL;
	}
	
	sprintf(r->payload,format,RTSP_VERSION,
		rtspRStatusCodes[RTSP_RSC_OK].code,
		rtspRStatusCodes[RTSP_RSC_OK].info,
		r->cseq,
		r->session_id,
		tmp);
	r->payload_size = strlen(r->payload);
	RTSP_DEBUG("ack (size:%d):\r\n%s\r\n",r->payload_size,r->payload);
	
	ret=rtsp_send_packet(r);
	return ret;
}

static int rtsp_handle_set_parameter(Rtsp_t *r)
{
	RTSP_DEBUG("not yet");
	return RTSP_RET_OK;
}
static int rtsp_handle_teardown(Rtsp_t *r)
{
	int ret;
	const char *format=
		"%s %d %s\r\n"\
		"CSeq: %d\r\n"\
		"\r\n";
	sprintf(r->payload,format,RTSP_VERSION,
		rtspRStatusCodes[RTSP_RSC_OK].code,
		rtspRStatusCodes[RTSP_RSC_OK].info,
		r->cseq);
	r->payload_size = strlen(r->payload);
	RTSP_DEBUG("ack (size:%d):\r\n%s\r\n",r->payload_size,r->payload);
	
	ret=rtsp_send_packet(r);
	return ret;
}
static int rtsp_handle_notallowed_method(Rtsp_t *r)
{
	int ret;
	const char *format=
		"%s %d %s\r\n"\
		"CSeq: %d\r\n"\
		"Allow: %s\r\n"
		"\r\n";
	sprintf(r->payload,format,RTSP_VERSION,
		rtspRStatusCodes[RTSP_RSC_METHOD_NOT_ALLOWED].code,
		rtspRStatusCodes[RTSP_RSC_METHOD_NOT_ALLOWED].info,
		r->cseq,
		rtspAllowMethods);
	r->payload_size = strlen(r->payload);
	
	ret=rtsp_send_packet(r);
	return ret;
}

int RTSP_request_options(Rtsp_t *r,const char *sMethodList[])
{
	return RTSP_RET_OK;
}
int RTSP_request_describe(Rtsp_t *r)
{
	return RTSP_RET_OK;
}
int RTSP_request_annouce(Rtsp_t *r)
{
	return RTSP_RET_OK;
}
int RTSP_request_get_parameter(Rtsp_t *r)
{
	return RTSP_RET_OK;
}
int RTSP_request_set_parameter(Rtsp_t *r)
{
	return RTSP_RET_OK;
}
int RTSP_request_play(Rtsp_t *r)
{
	return RTSP_RET_OK;
}
int RTSP_request_pause(Rtsp_t *r)
{
	return RTSP_RET_OK;
}
int RTSP_request_record(Rtsp_t *r)
{
	return RTSP_RET_OK;
}
int RTSP_request_redirect(Rtsp_t *r)
{
	return RTSP_RET_OK;
}
int RTSP_request_setup(Rtsp_t *r)
{
	return RTSP_RET_OK;
}
int RTSP_request_teardown(Rtsp_t *r)
{
	return RTSP_RET_OK;
}


int RTSP_init(Rtsp_t *r,int fd)
{
	memset(r,0,sizeof(Rtsp_t));
	r->sock = fd;
	r->state = RTSP_STATE_INIT;
	r->cseq = 0;
	r->b_interleavedMode = false;
	r->client_port = RTSP_CLIENT_PORT_BEGIN;
	r->server_port = RTSP_SERVER_PORT_BEGIN;
	r->channel = RTSP_CHANNEL_BEGIN;
	r->cast_type = RTSP_UNICAST;
	r->work_mode = RTSP_MODE_PLAY;
	r->low_transport = RTSP_TRANSPORT_UDP;
	r->session_id = 12345678;//rand();
	tcp_client_init(fd);
	
	return true;
}
int RTSP_destroy(Rtsp_t *r)
{
	RTSP_INFO("rtsp destroy.");
	close(r->sock);
	r->sock = -1;
	r->state = RTSP_STATE_INIT;
	r->cseq = 0;
	if(r->sdp){
		SDP_cleanup(r->sdp);
	}
	return true;
}

int RTSP_read_message(Rtsp_t *r)
{
	int ret=recv(r->sock,r->payload,RTSP_BUF_SIZE,0);
	if(ret < 0){
		RTSP_ERR("read message failed");
		return RTSP_RET_FAIL;
	}
	r->payload_size = ret;
	return 0;
}
int RTSP_parse_message(Rtsp_t *r)
{
	int i,ret;
	for(i=0;i<RTSP_METHOD_CNT;i++){
		if(memcmp(r->payload,rtspMethods[i],strlen(rtspMethods[i])) == 0){
			break;
		}
	}
	if(i == RTSP_METHOD_CNT){
		RTSP_ERR("unknow method");
		return RTSP_RET_FAIL;
	}
	if(http_get_url(r->payload,r->ip_me,&r->port,r->stream)==RTSP_RET_FAIL){
		return RTSP_RET_FAIL;
	}
	if(http_get_number(r->payload,"CSeq:",&r->cseq)==-1){
		RTSP_ERR("invaild request format,not found cseq");
		return RTSP_RET_FAIL;
	}
	RTSP_DEBUG("rtsp request %d <<%s>> cseq:%d",i,rtspMethods[i],r->cseq);
	switch(i){
		case RTSP_METHOD_DESCRIBE:
			ret=rtsp_handle_describe(r);
			break;
		//case RTSP_METHOD_ANNOUNCE:
		//	ret=rtsp_handle_announce(r);
		//	break;
		//case RTSP_METHOD_GET_PARAMETER:
		//	ret=rtsp_handle_get_parameter(r);
		//	break;
		case RTSP_METHOD_OPTIONS:
			ret=rtsp_handle_options(r);
			break;
		//case RTSP_METHOD_PAUSE:
		//	ret=rtsp_handle_pause(r);
		//	break;
		case RTSP_METHOD_PLAY:
			ret=rtsp_handle_play(r);
			r->state = RTSP_STATE_PLAYING;
			break;
		//case RTSP_METHOD_RECORD:
		//	ret=rtsp_handle_record(r);
		//	break;
		//case RTSP_METHOD_REDIRECT:
		//	ret=rtsp_handle_redirect(r);
		//	break;
		case RTSP_METHOD_SETUP:
			ret=rtsp_handle_setup(r);
			r->state = RTSP_STATE_READY;
			break;
		//case RTSP_METHOD_SET_PARAMETER:
		//	rtsp_handle_set_parameter(r);
		//	break;
		case RTSP_METHOD_TEARDOWN:
			ret=rtsp_handle_teardown(r);
			r->state = RTSP_STATE_INIT;
			break;
		default:
			ret=rtsp_handle_notallowed_method(r);
			break;
	}
	return ret;
}

int RTSP_send_frame(Rtsp_t *r,char *src,int len,uint32_t ts)
{
	int i;
	RtpPacket_t rtpp;
	int sock = -1; 
	struct sockaddr_in addr;

	sock =udp_server_init(r->server_port,RTSP_SOCK_TIMEOUT);
	//sock = udp_client_init(RTSP_SOCK_TIMEOUT);
	if(sock < 0)
		return RTSP_RET_FAIL;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(r->client_port);
	addr.sin_addr.s_addr = inet_addr("192.168.2.82");
	if(RTP_packet_h264(src,len,&rtpp,ts,r->session_id)==RTSP_RET_OK){
		for(i=0;i<rtpp.cnt;i++){
			if(sendto(sock,
				rtpp.payload[i],rtpp.payload_size[i],0,
				(struct sockaddr *)&addr,
				sizeof(struct sockaddr_in))!= rtpp.payload_size[i]){
				RTSP_ERR("send to(%#10x:%d) frame failed,sock:%d",
					addr.sin_addr.s_addr,r->client_port,sock);
				return RTSP_RET_FAIL;
			}
			else{
				RTSP_DEBUG("%d: send frame ok,size:%d",i,rtpp.payload_size[i]);
			}
		}
	}
	RTSP_DEBUG("rtsp send frame done");
	return RTSP_RET_OK;
}

