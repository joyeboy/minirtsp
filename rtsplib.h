#ifndef __RTSPLIB_H__
#define __RTSPLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "rtsplog.h"
#include "authentication.h"
#include "sdplib.h"
#include "rtplib.h"
#include "rtcplib.h"
#include "netstream.h"

/****************************************************************
* configure mirco
****************************************************************/
#define RTSP_BUF_SIZE			(1024*16)
#define RTSP_CLIENT_PORT_BEGIN	(5200)
#define RTSP_SERVER_PORT_BEGIN	(7200)
#define RTSP_CHANNEL_BEGIN		(0)

#define RTSP_SOCK_TIMEOUT	(5)
#define RTSP_DEFAULT_PORT	(554)
#define RTSP_VERSION		"RTSP/1.0"

#define RTSP_TRANSPORT_UDP	(0)
#define RTSP_TRANSPORT_TCP	(1)

#define RTSP_UNICAST		(0)
#define	RTSP_MULTICAST		(1)

#define RTSP_MODE_PLAY		(0)
#define RTSP_MODE_RECORD	(1)

typedef enum{
	RTSP_STATE_INIT,
	RTSP_STATE_READY,
	RTSP_STATE_PLAYING,
	RTSP_STATE_RECORDING,
	RTSP_STATE_CNT
}RtspState_t;

typedef enum{
	RTSP_METHOD_DESCRIBE = 0,
	RTSP_METHOD_ANNOUNCE,
	RTSP_METHOD_GET_PARAMETER,
	RTSP_METHOD_OPTIONS,
	RTSP_METHOD_PAUSE,
	RTSP_METHOD_PLAY,
	RTSP_METHOD_RECORD,
	RTSP_METHOD_REDIRECT,
	RTSP_METHOD_SETUP,	//8	
	RTSP_METHOD_SET_PARAMETER,
	RTSP_METHOD_TEARDOWN,
	RTSP_METHOD_CNT
}RtspMethod_t;

typedef struct _RtspPacket
{
	int cseq;
	int body_size;
	char *body;
}RtspPacket_t;

typedef struct _RtspTransport
{
	
}RtspTransport_t;

typedef struct _Rtsp
{
	int sock;
	int trigger;
	//uri
	char ip_me[20];
	int port;
	char stream[32];
	
	//transport
	int b_interleavedMode;
	int low_transport;	// udp or tcp
	int client_port;
	int server_port;
	int channel;
	int cast_type;//unicast or multicast
	int work_mode;//record or play
	
	RtspState_t state;
	uint32_t session_id;

	int cseq;
	int payload_size;
	char payload[RTSP_BUF_SIZE];

	Authentication_t auth;
	RtspStream_t s;

	SessionDesc_t *sdp;
}Rtsp_t;

// global interface for rtsp client and rtsp server
extern int RTSP_init(Rtsp_t *r,int sock);
extern int RTSP_destroy(Rtsp_t *r);

//extern int RTSP_add_stream(Rtsp_t *r,const char *stream_name);
//extern int RTSP_remove_stream(Rtsp_t *r,const char *stream_name);

extern int RTSP_read_message(Rtsp_t *r);
extern int RTSP_parse_message(Rtsp_t *r);

extern int RTSP_request_options(Rtsp_t *r,const char *sMethodList[]);
extern int RTSP_request_describe(Rtsp_t *r);
extern int RTSP_request_annouce(Rtsp_t *r);
extern int RTSP_request_get_parameter(Rtsp_t *r);
extern int RTSP_request_set_parameter(Rtsp_t *r);
extern int RTSP_request_play(Rtsp_t *r);
extern int RTSP_request_pause(Rtsp_t *r);
extern int RTSP_request_record(Rtsp_t *r);
extern int RTSP_request_redirect(Rtsp_t *r);
extern int RTSP_request_setup(Rtsp_t *r);
extern int RTSP_request_teardown(Rtsp_t *r);

#ifdef __cplusplus
}
#endif
#endif

