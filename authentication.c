#include "rtspdef.h"
#include "authentication.h"


static int basic_validate()
{
	return RTSP_RET_OK;
}

static int digest_validate()
{
	return RTSP_RET_OK;
}

int AUTH_init(struct _authentication *auth,int type,const char *user,const char *pwd)
{
	return RTSP_RET_OK;
}
int AUTH_destroy(struct _authentication *auth)
{
	return RTSP_RET_OK;
}
int AUTH_validate(struct _authentication *auth)
{
	return RTSP_RET_OK;
}


