#ifndef __RTSPLIB_H__
#define __RTSPLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "rtsplog.h"
#include "authentication.h"
#include "rtplib.h"
#include "rtcplib.h"
#include "netstream.h"

#define RTSP_TRANSPORT_UDP	(0)
#define RTSP_TRANSPORT_TCP	(1)

#define RTSP_UNICAST		(0)
#define	RTSP_MULTICAST		(1)
#define RTSP_BROADCASET		(2)

typedef struct _Rtsp
{
	int sock;
	int trigger;

	uint32_t session_id;

	int transport;// udp or tcp
	int cast_type;// unicast or multicast or broadcast

	Authentication_t auth;
	RtspStream_t s;
}Rtsp_t;

// global interface for rtsp client and rtsp server
extern int RTSP_init(Rtsp_t *r);
extern int RTSP_destroy(Rtsp_t *r);

extern int RTSP_add_stream(Rtsp_t *r,const char *stream_name);
extern int RTSP_remove_stream(Rtsp_t *r,const char *stream_name);

extern int RTSP_read_message(Rtsp_t *r);
extern int RTSP_parse_message(Rtsp_t *r);

extern int RTSP_request_option(Rtsp_t *r,const char *sMethodList[]);
extern int RTSP_request_



#endif
#endif

