#ifndef __RTSP_STREAM_H__
#define __RTSP_STREAM_H__

#include "rtspdef.h"

#define RTSP_STREAM_TYPE_NOT_SUPPORT		(0)
#define RTSP_STREAM_TYPE_AUDIO			(8)
#define RTSP_STREAM_TYPE_VIDEO			(9)// SAME WITH RTSP AUDIO & VIDEO
#define RTSP_STREAM_TYPE_META				(0x12)

typedef struct _rtsp_stream {
    char name[64];
    //int ref;
    void *param;
	uint32_t inspeed;
	uint32_t timestamp;
	int isKeyFrame;
	int type;// stream type
    char *data;
    uint32_t size;
}RtspStream_t;

int RTSP_STREAM_init(RtspStream_t *s,const char *name);
int RTSP_STREAM_next(RtspStream_t *s);
int RTSP_STREAM_destroy(RtspStream_t *s);
int RTSP_STREAM_reset(RtspStream_t *s);


#endif

