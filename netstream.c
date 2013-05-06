#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtspdef.h"
#include "rtsplib.h"
#include "netstream.h"
#include "rtsplog.h"

#ifdef NOCROSS

typedef struct _test_264
{
	uint32_t flag;
	uint32_t size;
	uint32_t isidr;
}Test264Frame_t;
	

int RTSP_STREAM_init(RtspStream_t *s,const char *name) 
{
	FILE *src;
	strcpy(s->name,name);
	s->inspeed= 83000;
	s->timestamp=0;
	s->data =NULL;
	s->size = 0;
	src = fopen(name,"r");
    if (src == NULL){
		RTSP_ERR("init stream %s failed.",name);
        return RTSP_RET_FAIL;
    }
	s->param = (void *) src;
	RTSP_INFO("rtmp stream(%s) init success.",name);
	return RTSP_RET_OK;
}

int RTSP_STREAM_next(RtspStream_t *s) 
{	
	static uint32_t base_ts=0;
	FILE *src = (FILE *)s->param;
	char *buffer=NULL;
	Test264Frame_t frame;
    int ret=fread(&frame,sizeof(Test264Frame_t),1,src);
    if (ret <= 0) {
        return RTSP_RET_FAIL;
    } else {
    	if(s->data == NULL){
			buffer = malloc(128*1024);
			if(buffer == NULL){
				printf("malloc for stream buffer failed\n");
				return RTSP_RET_FAIL;
			}
			s->data = buffer;
    	}
		ret=fread(s->data,frame.size,1,src);
		if(ret <= 0)
			return RTSP_RET_FAIL;
    }
    s->size=frame.size;
    s->timestamp=base_ts;
	s->type = RTSP_STREAM_TYPE_VIDEO;
	s->isKeyFrame=frame.isidr;
	
	base_ts +=83;
    return RTSP_RET_OK;
}

int RTSP_STREAM_destroy(RtspStream_t *s) 
{
	if(s->data) free(s->data);
	FILE *src = (FILE *)s->param;
    fclose(src);
	return RTSP_RET_OK;
}

int RTSP_STREAM_reset(RtspStream_t *s) 
{
	FILE *src = (FILE *)s->param;
    fclose(src);
    s->param= (void *)fopen(s->name,"r");
    if (s->param == NULL)
        return RTSP_RET_FAIL;
	return RTSP_RET_OK;
}

#else
#include "sdk/sdk_api.h"
#include "mediabuf.h"


int RTSP_STREAM_init(RtspStream_t *s,const char *name) 
{
    int mediabuf_ch;
	MEDIABUF_USER_t* user = NULL;

    mediabuf_ch = MEDIABUF_lookup_byname(name);
    if (mediabuf_ch < 0) {
		RTSP_ERR("lookup by name failed,name:%s",name);
        return RTSP_RET_FAIL;
    }
	strcpy(s->name,name);
    user = MEDIABUF_attach(mediabuf_ch);
    if (!user) {
		RTSP_ERR("mediabuf attach failed,name:%s",name);
        return RTSP_RET_FAIL;
    }
	s->param = (void *)user;
    s->inspeed = MEDIABUF_get_inspeed(mediabuf_ch);
	s->timestamp=0;
	s->data =NULL;
	s->size = 0;
	
    MEDIABUF_sync(user);

	RTSP_INFO("rtmp stream(%s)(inspeed:%d) init success.",name,s->inspeed);
	
	return RTSP_RET_OK;
}


int RTSP_STREAM_next(RtspStream_t *s) 
{
	int ret;
	MEDIABUF_USER_t* user =(MEDIABUF_USER_t*) s->param;
    int out_success=false;
	
    if (user==NULL) {
        RTSP_ERR("mediabuf not ready!");
        return -1;
    }
	
    if (0 == MEDIABUF_out_lock(user)) {
		SDK_ENC_BUF_ATTR_t* enc_attr = NULL;
		ssize_t enc_size;
        if (0 == MEDIABUF_out(user,(void **)&enc_attr, NULL, &enc_size)) {
			out_success = true;
			s->size = enc_attr->data_sz;
			s->timestamp = enc_attr->timestamp_us/ 1000;
			s->data = (void*)(enc_attr + 1);
			s->isKeyFrame = 0;
			if(SDK_ENC_BUF_DATA_H264 == enc_attr->type && enc_attr->h264.ref_counter % 2 == 0){
				s->isKeyFrame = enc_attr->h264.keyframe;
				s->type =  RTSP_STREAM_TYPE_VIDEO;
			}else if(SDK_ENC_BUF_DATA_G711A == enc_attr->type){
				s->type = RTSP_STREAM_TYPE_AUDIO;
			}else{
				s->type = RTSP_STREAM_TYPE_NOT_SUPPORT;
			}
        } else {
            RTSP_INFO("mediabuf get data failed!");
        }
		MEDIABUF_out_unlock(user);

    } else {
        RTSP_ERR("mediabuf lock failed!");
    }
	
    return true == out_success ? RTSP_RET_OK : RTSP_RET_FAIL;
}

int RTSP_STREAM_destroy(RtspStream_t *s) 
{
	MEDIABUF_USER_t* user =(MEDIABUF_USER_t*) s->param;
	MEDIABUF_detach(user);

    return RTSP_RET_OK;
}
int RTSP_STREAM_reset(RtspStream_t *s) 
{
	MEDIABUF_USER_t* user =(MEDIABUF_USER_t*) s->param;
	MEDIABUF_detach(user);
	RTSP_STREAM_init(s,s->name);
    return RTSP_RET_OK;
}

#endif



