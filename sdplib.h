/******************************************************************************

  Copyright (C), 20013-, GuangZhou JUAN Electronics Co., Ltd.

 ******************************************************************************
  File Name    : sdplib.h
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
#ifndef __SDPLIB_H__
#define __SDPLIB_H__
	
#ifdef __cplusplus
	extern "C" {
#endif

#include "rtspdef.h"
#include "rtsplog.h"

/***********************************************************************
*configure
***********************************************************************/
#define SDP_BUFFER_SIZE				(1024*5)
#define SDP_MAX_MEDIA_LEVEL			(2)
#define SDP_MAX_ATTR_NUM			(8)
#define SDP_DEFAULT_SESSION_VER		(1)
#define SDP_DEFAULT_SESSION_NAME	"default session"
#define SDP_DEFAULT_SESSION_INFO	"minisdp"
#define	SDP_DEFAULT_EMAIL			"default@email.com (default)"
#define SDP_DEFAULT_PHONE			"88888888"
#define SDP_DEFAULT_VIDEO_CONTROL	"track=video"
#define SDP_DEFAULT_AUDIO_CONTROL	"track=audio"

/**********************************************************************
* sdp specification mirco
**********************************************************************/
#define SDP_MEDIA_TYPE				"application/sdp"
#define SDP_VERSION					"0"
#define SDP_NOTSUPPORT_USERID		"-"

#define SDP_NETTYPE_INTERNET		"IN"

#define SDP_ADDR_TYPE_IPV4			"IP4"
#define SDP_ADDR_TYPE_IPV6			"IP6"

#define SDP_MEDIA_TYPE_VIDEO		"video"
#define SDP_MEDIA_TYPE_AUDIO		"audio"
#define SDP_MEDIA_TYPE_TEXT			"text"
#define SDP_MEDIA_TYPE_APPLICATION	"application"
#define SDP_MEDIA_TYPE_MESSAGE		"message"

#define SDP_MEDIA_PROTOCAL_RTP_AVP	"RTP/AVP"
#define SDP_MEDIA_PROTOCAL_RTP_SAVP	"RTP/SAVP"

#define SDP_ENCRYPT_TYPE_CLEAR		"clear"		// clear:<encryption key>
#define SDP_ENCRYPT_TYPE_BASE64		"base64"	// base64:<encoded encryption key>
#define SDP_ENCRYPT_TYPE_URI		"uri"		// uri:<uri to obtain key>	
#define SDP_ENCRYPT_TYPE_PROMPT		"prompt"	// prompt   // no key

#define SDP_SPEC_VERSION			0
#define SDP_SPEC_ORIGIN				1
#define SDP_SPEC_SESSION_NAME		2
#define SDP_SPEC_SESSION_INFO		3
#define SDP_SPEC_URI				4
#define SDP_SPEC_EMAIL				5
#define SDP_SPEC_PHONE				6
#define SDP_SPEC_CONNECTION			7
#define SDP_SPEC_BANDWIDTH			8
#define SDP_SPEC_TIMING				9
#define SDP_SPEC_REPEAT_TIME		10
#define SDP_SPEC_TIME_ZONE			11
#define SDP_SPEC_ENCRYPT_KEY		12
#define SDP_SPEC_ATTRIBUTE			13
#define SDP_SPEC_MEDIA_DESC			14
#define SDP_SPEC_CNT				15


#define SDP_ATTR_CATEGORY			(0)
#define SDP_ATTR_KEY_WORDS			(1)
#define SDP_ATTR_TOOL				(2)
#define SDP_ATTR_PACKET_TIME		(3)
#define SDP_ATTR_MAX_PTIME			(4)
#define SDP_ATTR_RTP_MAP			(5)
#define SDP_ATTR_RECVONLY			(6)
#define SDP_ATTR_SENDRECV			(7)
#define SDP_ATTR_SENDONLY			(8)
#define SDP_ATTR_INACTIVE			(9)
#define SDP_ATTR_ORIENTATION		(10)
#define SDP_ATTR_CONFERENCE_TYPE	(11)
#define SDP_ATTR_CHARSET			(12)
#define SDP_ATTR_SDP_LANG			(13)
#define SDP_ATTR_LANG				(14)
#define SDP_ATTR_FRAME_RATE			(15)
#define SDP_ATTR_QUALITY			(16)
#define SDP_ATTR_FMTP				(17)
#define SDP_ATTR_CONTROL			(18)
#define SDP_ATTR_RANGE				(19)
#define SDP_ATTR_ENTITY_TAG			(20)

#define SDP_MEDIA_FORMAT_H264		(96)
#define SDP_MEDIA_FORMAT_G711		(31)

typedef struct _attrubute
{
	char name[32];
	char value[64];
}Attribute_t;

typedef struct _media_desc
{
	uint32_t spec_flag;
	// m= <media> <port> <proto> <fmt> ...
	struct{
		char type[16];
		int port;
		char protocal[16];
		char format[32];
	}media_n;
	// i=*
	char media_info[128];
	// c=*
	struct{
		char nettype[8];
		char addrtype[8];
		char addr[32];
	}conn_info;
	// k=*
	char encryt_key[128];
	// a=*
	int attr_num;
	Attribute_t attri[SDP_MAX_ATTR_NUM];
}MediaDesc_t;

typedef struct
{	
	char *buffer;
	uint32_t spec_flag;
	// v=
	char version[8];
	// o= <username> <session id> <session version> <net type> <address type> <unicast-address>
	struct{
		char user_name[32];
		uint32_t session_id;
		int session_ver;
		char nettype[8];
		char addrtype[8];
		char addr[20];
	}originer;
	// s=
	char session_name[32];
	// i=*
	char session_info[128];
	// u=*
	char uri[128];
	// e=* <email> {(user name)}
	char email[64];
	// p=*
	char phone[32];
	// c=* <nettype> <addrtype> <connect-address>
	struct{
		char nettype[8];
		char addrtype[8];
		char addr[20];
	}conn_info;
	// b=*  //zero or more bandwidth information lines
	char bandwidth[128];
	// z=*
	char zone[32];
	// k=*
	struct{
		char method;
	}encrypt_key;
	char encrpt_key[128];
	// a=*
	int attr_num;
	Attribute_t attri[SDP_MAX_ATTR_NUM];
	// t= <start time> <end time>
	struct{
		uint32_t start;
		uint32_t end;
	}time_active;
	// r=*
	char repeat_time[64];
	// m=*
	int media_num;
	MediaDesc_t media[SDP_MAX_MEDIA_LEVEL];
}SessionDesc_t;

extern SessionDesc_t *SDP_new_default(char *session_name,char *ip);
extern int SDP_add_h264(SessionDesc_t *sdp,char *control);
extern int SDP_add_g711(SessionDesc_t *sdp,char *control);
extern int SDP_setup(SessionDesc_t *sdp);
extern int SDP_parse(SessionDesc_t *sdp,char *src,int length);
extern int SDP_cleanup(SessionDesc_t *sdp);


#ifdef __cplusplus
}
#endif
#endif


