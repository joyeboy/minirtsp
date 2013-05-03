#include "rtsplib.h"
#include "rtspd.h"
//#include "sdk/sdk_api.h"
//#include "mediabuf.h"


SPOOK_SESSION_PROBE_t MINIRTSP_probe(const void* msg, ssize_t msg_sz)
{
	if((memcmp(msg,"OPTIONS",strlen("OPTIONS")) == 0)
		|| (memcmp(msg,"DESCRIBE",strlen("DESCRIBE")) == 0)
		|| (memcmp(msg,"SET_PARAMETER",strlen("SET_PARAMETER")) == 0)
		|| (memcmp(msg,"GET_PARAMETER",strlen("GET_PARAMETER")) == 0)){
		return SPOOK_PROBE_MATCH;
	}
	return SPOOK_PROBE_MISMATCH;
}

SPOOK_SESSION_LOOP_t MINIRTSP_loop(uint32_t* trigger, int sock, time_t* read_pts, const void* msg, ssize_t msg_sz)
{
	return rtsp_server_process(&sock);
}

