#ifndef __RTSP_LOG_H__
#define __RTSP_LOG_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define RTSP_TRACE_LEVEL (0)

#define RTSP_TRACE(syntax,fmt,arg...) \
	do{\
		printf("\033[%sm[%12s:%4d]\033[0m "fmt"\r\n", syntax, __FILE__, __LINE__, ##arg);\
	}while(0)


#if RTSP_TRACE_LEVEL == 0
#define RTSP_EMERG(fmt,arg...) RTSP_TRACE("1;30",fmt,##arg)
#else
#define RTSP_EMERG(fmt,arg...)
#endif

#if RTSP_TRACE_LEVEL <= 1
#define RTSP_ALERT(fmt,arg...) RTSP_TRACE("1;31",fmt,##arg)
#else
#define RTSP_ALERT(fmt,arg...)
#endif

#if RTSP_TRACE_LEVEL <= 2
#define RTSP_CRIT(fmt,arg...) RTSP_TRACE("1;32",fmt,##arg)
#else
#define RTSP_CRIT(fmt,arg...)
#endif

#if RTSP_TRACE_LEVEL <= 3
#define RTSP_ERR(fmt,arg...) RTSP_TRACE("1;33",fmt,##arg)
#else
#define RTSP_ERR(fmt,arg...)
#endif

#if RTSP_TRACE_LEVEL <= 4
#define RTSP_WARNING(fmt,arg...) RTSP_TRACE("1;34",fmt,##arg)
#else
#define RTSP_WARNING(fmt,arg...)
#endif

#if RTSP_TRACE_LEVEL <= 5
#define RTSP_NOTICE(fmt,arg...) RTSP_TRACE("1;35",fmt,##arg)
#else
#define RTSP_NOTICE(fmt,arg...)
#endif

#if RTSP_TRACE_LEVEL <= 6
#define RTSP_INFO(fmt,arg...) RTSP_TRACE("1;36",fmt,##arg)
#else
#define RTSP_INFO(fmt,arg...)
#endif

#if RTSP_TRACE_LEVEL <= 7
#define RTSP_DEBUG(fmt,arg...) RTSP_TRACE("1;37",fmt,##arg)
#else
#define RTSP_DEBUG(fmt,arg...)
#endif

#if RTSP_TRACE_LEVEL <= 3
#define RTSP_ASSERT(exp,fmt,arg...) \
		do{\
			if(!(exp)){\
				printf("\033[1;33m[%12s:%4d]\033[0m assert(\"%s\") "fmt"\r\n",\
					__FILE__, __LINE__, #exp, ##arg);\
				exit(1);\
			}\
		}while(0)
#else
#define RTSP_ASSERT(exp,fmt,arg...)
#endif

#ifdef __cplusplus
}
#endif
#endif //__RTSP_TRACE_H__


