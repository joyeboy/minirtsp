/******************************************************************************

  Copyright (C), 20013-, GuangZhou JUAN Electronics Co., Ltd.

 ******************************************************************************
  File Name    : sdplib.c
  Version       : Initial Draft
  Author        : kaga
  Created       : 2013/04/25
  Last Modified : 2013/04/25
  Description   : session description protocal  utils , reference to rfc4566
 
  History       : 
  1.Date        : 2013/04/25
    	Author      : kaga
 	Modification: Created file	
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdplib.h"

static const char * sdpSpecs[SDP_SPEC_CNT]=
{	
	"v=",
	"o=",
	"s=",
	"i=",
	"u=",
	"e=",
	"p=",
	"c=",
	"b=",
	"t=",
	"r=",
	"z=",
	"k=",
	"a=",
	"m=",
};
static const char *const sdpAttrs[]=
{
	"cat",
	"keywds",
	"tool",
	"ptime",
	"maxptime",
	"rtpmap",
	"recvonly",
	"sendrecv",
	"sendonly",
	"inactive",
	"orient",
	"type",
	"charset",
	"sdplang",
	"lang",
	"framerate",
	"quality",
	"fmtp",
	"control",
	"range",
	"etag",
	""	
};

static inline void sdp_add_line(char *key,char *value,char **dest)
{
	sprintf(*dest,"%s%s\r\n",key,value);
	*dest += strlen(*dest);
}
static inline void sdp_add_attr_line(char *name,char *value,char **dest)
{
	sprintf(*dest,"%s%s:%s\r\n",sdpSpecs[SDP_SPEC_ATTRIBUTE],name,value);
	*dest += strlen(*dest);
}

static int sdp_add(SessionDesc_t *sdp,int type,char **dest,int ext_arg)
{
	int i,n;
	char tmp[512];
	switch(type){
		case SDP_SPEC_VERSION:
			sdp_add_line(sdpSpecs[SDP_SPEC_VERSION],sdp->version,dest);
			break;
		case SDP_SPEC_ORIGIN:
			sprintf(tmp,"%s %d %d %s %s %s",sdp->originer.user_name,sdp->originer.session_id,
				sdp->originer.session_ver,sdp->originer.nettype,sdp->originer.addrtype,
				sdp->originer.addr);
			sdp_add_line(sdpSpecs[SDP_SPEC_ORIGIN],tmp,dest);
			break;
		case SDP_SPEC_SESSION_NAME:
			sdp_add_line(sdpSpecs[SDP_SPEC_SESSION_NAME],sdp->session_name,dest);
			break;
		case SDP_SPEC_SESSION_INFO:
			sdp_add_line(sdpSpecs[SDP_SPEC_SESSION_INFO],sdp->session_info,dest);
			break;
		case SDP_SPEC_URI:
			sdp_add_line(sdpSpecs[SDP_SPEC_URI],sdp->uri,dest);
			break;
		case SDP_SPEC_EMAIL:			
			sdp_add_line(sdpSpecs[SDP_SPEC_EMAIL],sdp->email,dest);
			break;
		case SDP_SPEC_PHONE:
			sdp_add_line(sdpSpecs[SDP_SPEC_PHONE],sdp->phone,dest);
			break;
		case SDP_SPEC_CONNECTION:
			if(ext_arg < 0){
				sprintf(tmp,"%s %s %s",sdp->conn_info.nettype,sdp->conn_info.addrtype,
					sdp->conn_info.addr);
			}else{
				sprintf(tmp,"%s %s %s",sdp->media[ext_arg].conn_info.nettype,
					sdp->media[ext_arg].conn_info.addrtype,
					sdp->media[ext_arg].conn_info.addr);
			}
			sdp_add_line(sdpSpecs[SDP_SPEC_CONNECTION],tmp,dest);
			break;
		case SDP_SPEC_BANDWIDTH:
			sdp_add_line(sdpSpecs[SDP_SPEC_BANDWIDTH],sdp->bandwidth,dest);
			break;
		case SDP_SPEC_TIMING:
			sprintf(tmp,"%d %d",sdp->time_active.start,sdp->time_active.end);
			sdp_add_line(sdpSpecs[SDP_SPEC_TIMING],tmp,dest);
			break;
		//case SDP_SPEC_REPEAT_TIME:
		//	break;
		//case SDP_SPEC_TIME_ZONE:break;
		case SDP_SPEC_ENCRYPT_KEY:break;
		//case SDP_SPEC_ATTRIBUTE:break;
		case SDP_SPEC_MEDIA_DESC:
			sprintf(tmp,"%s %d %s %s",sdp->media[ext_arg].media_n.type,
				sdp->media[ext_arg].media_n.port,
				sdp->media[ext_arg].media_n.protocal,
				sdp->media[ext_arg].media_n.format);
			sdp_add_line(sdpSpecs[SDP_SPEC_MEDIA_DESC],tmp,dest);
			break;
		default:break;
	}
}


SessionDesc_t *SDP_new_default(char *session_name,char *ip)
{
	SessionDesc_t *sdp=NULL;
	sdp = (SessionDesc_t *)malloc(sizeof(SessionDesc_t));
	if(sdp == NULL){
		RTSP_ERR("malloc for sdp failed");
		return NULL;
	}
	memset(sdp,0,sizeof(SessionDesc_t));
	sdp->buffer = (char *)malloc(sizeof(SDP_BUFFER_SIZE));
	if(sdp->buffer == NULL){
		RTSP_ERR("malloc for sdp buffer failed");
		free(sdp);
		return NULL;
	}
	sdp->spec_flag |= (1 << SDP_SPEC_VERSION);
	strcpy(sdp->version,SDP_VERSION);
	sdp->spec_flag |= (1 << SDP_SPEC_ORIGIN);
	strcpy(sdp->originer.user_name,SDP_NOTSUPPORT_USERID);
	sdp->originer.session_id = rand();
	sdp->originer.session_ver = SDP_DEFAULT_SESSION_VER;
	strcpy(sdp->originer.nettype,SDP_NETTYPE_INTERNET);
	strcpy(sdp->originer.addrtype,SDP_ADDR_TYPE_IPV4);
	strcpy(sdp->originer.addr,ip);
	sdp->spec_flag |= (1 << SDP_SPEC_SESSION_NAME);
	strcpy(sdp->session_name,session_name);	
	sdp->spec_flag |= (1 << SDP_SPEC_SESSION_INFO);
	strcpy(sdp->session_info,SDP_DEFAULT_SESSION_INFO);
	sdp->spec_flag |= (1 << SDP_SPEC_TIMING);
	sdp->time_active.start = 0;
	sdp->time_active.end = 0;
	// session attributions
	sdp->spec_flag |= ( 1<< SDP_SPEC_ATTRIBUTE);
	strcpy(sdp->attri[sdp->attr_num].name,sdpAttrs[SDP_ATTR_CONTROL]);
	sprintf(sdp->attri[sdp->attr_num].value,"*");
	sdp->attr_num++;
	
	// media descriptions
	sdp->media_num = 0;

	return sdp;
}

int SDP_add_h264(SessionDesc_t *sdp,char *control)
{
	MediaDesc_t *media= NULL;
	media=(MediaDesc_t *)&sdp->media[sdp->media_num];
	sdp->spec_flag |= (1 << SDP_SPEC_MEDIA_DESC);
	strcpy(media->media_n.type,SDP_MEDIA_TYPE_VIDEO);
	media->media_n.port = 0;
	strcpy(media->media_n.protocal,SDP_MEDIA_PROTOCAL_RTP_AVP);
	sprintf(media->media_n.format,"%d",SDP_MEDIA_FORMAT_H264);
	media->spec_flag |= (1 << SDP_SPEC_CONNECTION);
	strcpy(media->conn_info.nettype,SDP_NETTYPE_INTERNET);
	strcpy(media->conn_info.addrtype,SDP_ADDR_TYPE_IPV4);
	strcpy(media->conn_info.addr,"0.0.0.0");

	// attributions
	media->spec_flag |= ( 1<< SDP_SPEC_ATTRIBUTE);
	strcpy(media->attri[media->attr_num].name,sdpAttrs[SDP_ATTR_RTP_MAP]);
	sprintf(media->attri[media->attr_num].value,"96 H264/90000");
	media->attr_num++;
	strcpy(media->attri[media->attr_num].name,sdpAttrs[SDP_ATTR_CONTROL]);
	sprintf(media->attri[media->attr_num].value,"%s",control);
	media->attr_num++;
	
	sdp->media_num++;
	return 0;
}

int SDP_add_g711(SessionDesc_t *sdp,char *control)
{
	return 0;
}

int SDP_setup(SessionDesc_t *sdp)
{
	int i,j,n;
	char *ptr=sdp->buffer;
	char tmp[512];
	if(sdp == NULL || ptr == NULL){
		RTSP_ERR("sdp or buffer is null");
		return -1;
	}
	//
	for(i=0;i<SDP_SPEC_MEDIA_DESC;i++){
		if(i == SDP_SPEC_ATTRIBUTE){
			for(j=0;j<sdp->attr_num;j++){
				sdp_add_attr_line(sdp->attri[j].name,
					sdp->attri[j].value,&ptr);
			}
		}else{
			if(sdp->spec_flag & (1<<i)){
				sdp_add(sdp,i,&ptr,-1);
			}
		}
	}
	if(sdp->spec_flag & (1 << SDP_SPEC_MEDIA_DESC)){
		for(n=0;n<sdp->media_num;n++){
			sdp_add(sdp,SDP_SPEC_MEDIA_DESC,&ptr,n);
			for(i=0;i<SDP_SPEC_MEDIA_DESC;i++){
				if(i == SDP_SPEC_ATTRIBUTE){
					for(j=0;j<sdp->media[n].attr_num;j++){
						sdp_add_attr_line(sdp->media[n].attri[j].name,
							sdp->media[n].attri[j].value,&ptr);
					}
				}else{
					if(sdp->media[n].spec_flag & (1<<i)){
						sdp_add(sdp,i,&ptr,n);
					}
				}
			}
		}
	}
	printf("sdp setup output:\r\n:%s\r\n",sdp->buffer);
	return 0;
}

int SDP_parse(SessionDesc_t *sdp,char *src,int length)
{
	return 0;
}

int SDP_cleanup(SessionDesc_t *sdp)
{
	if(sdp){
		if(sdp->buffer){
			free(sdp->buffer);
			sdp->buffer=NULL;
		}
		free(sdp);
		sdp=NULL;
	}
	return 0;
}



