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

#include "rtsplib.h"


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


static const char * const rtspMethods[RTSP_METHOD_CNT]=
{
	"DESCRIBE",
	"ANNOUNCE",
	"GET PARAMETER",
	"OPTIONS"
	"PAUSE",
	"PLAY",
	"RECORD",
	"REDIRECT",
	"SETUP",
	"SET_PARAMETER",
	"TEARDOWN",
	""
};
static const char *rtspAllowMethods[]="SETUP,OPTIONS,PLAY,SETUP,TEARDOWN";

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
		sscanf(tmp,"%s%[^\r\n]",ret);
		return 0;
	}
}
static inline int http_get_url(const char *src,char *ret)
{
	if(sscanf(src,"%*s %s %*s",ret)==1){
		return 0;
	}else{
		return -1;
	}
}


static int rtsp_send_packet(Rtsp_t *r)
{
	
}


static int rtsp_handle_describe(Rtsp_t *r)
{
	RTSP_DEBUG("not yet");
	return RTSP_RET_OK;
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
	
	ret=rtsp_send_packet(r);
	return ret;
}
static int rtsp_handle_play(Rtsp_t *r)
{
	RTSP_DEBUG("not yet");
	return RTSP_RET_OK;
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
	const char *format=
		"%s %d %s\r\n"\
		"CSeq: %d\r\n"\
		"Transport: %s;%s\r\n"\
		"\r\n";
	sprintf(r->payload,format,RTSP_VERSION,
		rtspRStatusCodes[RTSP_RSC_OK].code,
		rtspRStatusCodes[RTSP_RSC_OK].info,
		r->cseq,
		rtspAllowMethods);
	r->payload_size = strlen(r->payload);
	
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
	RTSP_DEBUG("not yet");
	return RTSP_RET_OK;
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
}
int RTSP_request_describe(Rtsp_t *r)
{
}
int RTSP_request_annouce(Rtsp_t *r)
{
}
int RTSP_request_get_parameter(Rtsp_t *r)
{
}
int RTSP_request_set_parameter(Rtsp_t *r)
{
}
int RTSP_request_play(Rtsp_t *r)
{
}
int RTSP_request_pause(Rtsp_t *r)
{
}
int RTSP_request_record(Rtsp_t *r)
{
}
int RTSP_request_redirect(Rtsp_t *r)
{
}
int RTSP_request_setup(Rtsp_t *r)
{
}
int RTSP_request_teardown(Rtsp_t *r)
{
}


int RTSP_init(Rtsp_t *r,int fd)
{
	r->sock = fd;
	r->state = RTSP_STATE_INIT;
	r->cseq = 0;
	RTSP_SOCK_init(fd);
	
	return true;
}
int RTSP_destroy(Rtsp_t *r)
{
	close(r->sock);
	r->sock = -1;
	r->state = RTSP_STATE_INIT;
	r->cseq = 0;
	return true;
}

int RTSP_read_message(Rtsp_t *r)
{
	int ret=read(r->sock,r->payload,RTSP_BUF_SIZE,0);
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
	if(http_get_number(r->payload,"CSeq:",&r->cseq)==-1){
		RTSP_ERR("invaild request format,not found cseq");
		return RTSP_RET_FAIL;
	}
	RTSP_DEBUG("rtsp request <<%s>> cseq:%d",rtspMethods[i],r->cseq);
	switch(i){
		case RTSP_METHOD_DESCRIBE:
			rtsp_handle_describe(r);
			break;
		case RTSP_METHOD_ANNOUNCE:
			rtsp_handle_announce(r);
			break;
		case RTSP_METHOD_GET_PARAMETER:
			rtsp_handle_get_parameter(r);
			break;
		case RTSP_METHOD_OPTIONS:
			rtsp_handle_options(r);
			break;
		case RTSP_METHOD_PAUSE:
			rtsp_handle_pause(r);
			break;
		case RTSP_METHOD_PLAY:
			rtsp_handle_play(r);
			break;
		case RTSP_METHOD_RECORD:
			rtsp_handle_record(r);
			break;
		case RTSP_METHOD_REDIRECT:
			rtsp_handle_redirect(r);
			break;
		case RTSP_METHOD_SETUP:
			rtsp_handle_setup(r);
			break;
		case RTSP_METHOD_SET_PARAMETER:
			rtsp_handle_set_parameter(r);
			break;
		case RTSP_METHOD_TEARDOWN:
			rtsp_handle_teardown(r);
			break;
		default:
			rtsp_handle_notallowed_method(r);
			break;
	}
	ret RTSP_RET_OK;
}

