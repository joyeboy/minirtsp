/******************************************************************************

  Copyright (C), 20013-, GuangZhou JUAN Electronics Co., Ltd.

 ******************************************************************************
  File Name    : rtplib.c
  Version       : Initial Draft
  Author        : kaga
  Created       : 2013/04/25
  Last Modified : 2013/04/25
  Description   : A transport protocal for real time application  utils , reference to rfc3550,rfc3984
 
  History       : 
  1.Date        : 2013/04/25
    	Author      : kejiazhw@gmail.com(kaga)
 	Modification: Created file	
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtplib.h"
#include "rtsplog.h"

int RTP_packet_h264(char *src,int len,RtpPacket_t *p,uint32_t ts,uint32_t ssrc)
{
#define H264_NALU_OFFSET		(4)	/*start code */
#define H264_NALU_DATA_OFFSET	(5)	/*start code & nalu header*/
	static int cseq=0;
	char *ptr=NULL,*psrc=NULL;
	int frag_size;
	Nalu_t nalu;
	FUIndicator_t *fu_indicator;
	FUHeader_t *fu_header;
	int b_fragment = false;
	RtpHeader_t rtpHeader;
	if((len-H264_NALU_DATA_OFFSET) > RTP_MTU_SIZE){
		b_fragment = true;
	}
	RTSP_DEBUG("rtp start,src:%p len:%d end:%p",src,len,src+len);
	rtpHeader.version = RTP_VERSION;
	rtpHeader.padding = false;
	rtpHeader.extension = false;
	rtpHeader.csrc_cnt = 0;
	rtpHeader.marker = false;
	rtpHeader.payload_type = RTP_DYNAMIC_TYPE;
	rtpHeader.sequence_number = ++cseq;
	rtpHeader.timestamp = ts;
	rtpHeader.ssrc = ssrc;

	if(b_fragment == false){
		p->cnt = 1;
		ptr=p->payload[0];
		rtpHeader.marker = true;
		memcpy(ptr,&rtpHeader,sizeof(RtpHeader_t));
		ptr+=sizeof(RtpHeader_t);
		memcpy(ptr,src+H264_NALU_OFFSET,len-H264_NALU_OFFSET);
		p->payload_size[0]=sizeof(RtpHeader_t) + len -H264_NALU_OFFSET;
	}else{
		int i=0;
		len -=5;
		p->cnt = (len+RTP_MTU_SIZE - 1)/RTP_MTU_SIZE;
		psrc = src + H264_NALU_DATA_OFFSET;
		for(i=0;i<p->cnt;i++){
			ptr = p->payload[i];
			if(len > RTP_MTU_SIZE)
				frag_size = RTP_MTU_SIZE;
			else
				frag_size = len;
			memcpy(ptr,&rtpHeader,sizeof(RtpHeader_t));
			ptr+=sizeof(RtpHeader_t);
			nalu.padding = *ptr;
			fu_indicator=(FUIndicator_t *)ptr;
			fu_indicator->forbidden_zero_bit = 0;
			fu_indicator->nal_ref_idc = nalu.nal_ref_idc;
			fu_indicator->type = RTP_FU_A;
			ptr++;
			fu_header=(FUHeader_t *)ptr;
			fu_header->start_bit = (i == 0) ? 1 : 0;
			fu_header->stop_bit = (i == (p->cnt -1))  ? 1 : 0;
			fu_header->type = nalu.type;
			ptr++;
			printf("ptr:%p psrc:%p size:%d\n",ptr,psrc,frag_size);
			memcpy(ptr,psrc,frag_size);
			p->payload_size[i]=sizeof(RtpHeader_t) + 1 + 1 + frag_size ;
			len-=frag_size;
			psrc +=frag_size;
		}
	}
	RTSP_DEBUG("rtp packet done,cnt:%d",p->cnt);
	return RTSP_RET_OK;
}



