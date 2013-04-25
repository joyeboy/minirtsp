/******************************************************************************

  Copyright (C), 20013-, GuangZhou JUAN Electronics Co., Ltd.

 ******************************************************************************
  File Name    : rtsplib.c
  Version       : Initial Draft
  Author        : kaga
  Created       : 2013/04/25
  Last Modified : 2013/04/25
  Description   : rtsp  utils , reference to rfc2326
 
  History       : 
  1.Date        : 2013/04/25
    	Author      : kaga
 	Modification: Created file	
******************************************************************************/

#include "rtsplib.h"













int RTSP_init(Rtsp_t *r,int fd)
{
	r->sock = fd;
	RTSP_SOCK_init(fd);
	
	return true;
}
int RTSP_destroy(Rtsp_t *r)
{
	return true;
}


