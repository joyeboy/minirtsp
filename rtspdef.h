#ifndef __RTSPDEF_H__
#define __RTSPDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RTSP_RET_OK				(0)
#define RTSP_RET_FAIL			(-1)

#ifndef true
#define true	(1)
#endif
#ifndef false
#define false	(0)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

//typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int 	uint32_t;


#ifdef __cplusplus
}
#endif
#endif

