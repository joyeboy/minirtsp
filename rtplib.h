#ifndef __RTPLIB_H__
#define __RTPLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "rtspdef.h"


#define RTP_VERSION				(2)
#define RTP_MTU_SIZE 			(1460)
#define RTP_DYNAMIC_TYPE		(96)
#define RTP_MAX_FRAGMENTATION	(10)

//#define RTP_SINGLE_UNIT	0-23
#define RTP_STAP_A	24//单一时间的组合包
#define RTP_STAP_B	25//单一时间的组合包
#define RTP_MTAP16	26//多个时间的组合包
#define RTP_MTAP24	27//多个时间的组合包
#define	RTP_FU_A	28//分片的单元
#define RTP_FU_B	29//分片的单元


typedef struct _rtp_header
{
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |V=2|P|X| CC|M|     PT      |       sequence number         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                           timestamp                           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |           synchronization source (SSRC) identifier            |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |            contributing source (CSRC) identifiers             |
// |                             ....                              |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    uint8_t csrc_cnt:4;
    uint8_t extension:1;
    uint8_t padding:1; 
    uint8_t version : 2;
    uint8_t payload_type:7;
    uint8_t marker:1;
    uint16_t sequence_number;
    uint32_t timestamp;
    uint32_t ssrc;
    // ignore the csrc id
}RtpHeader_t;


typedef struct _nalu
{
	union
	{
		struct
		{
			uint8_t type : 5;
			uint8_t nal_ref_idc : 2;
			uint8_t forbidden_zero_bit : 1; // must be 0
		};
		uint8_t padding;
	};
}Nalu_t;

typedef Nalu_t FUIndicator_t;

typedef struct _fu_header
{
	union
	{
		struct
		{
		    uint8_t type:5;
			uint8_t reserved_bit:1;
			uint8_t start_bit:1;
			uint8_t stop_bit:1;    
		};
		uint8_t padding;
	};
}FUHeader_t;

typedef struct _rtppacket
{
	int cnt;
	char payload[RTP_MAX_FRAGMENTATION][RTP_MTU_SIZE];
	int payload_size[RTP_MAX_FRAGMENTATION];
}RtpPacket_t;

extern int RTP_packet_h264(char *src,int len,RtpPacket_t *p,uint32_t ts,uint32_t ssrc);

#ifdef __cplusplus
}
#endif
#endif

