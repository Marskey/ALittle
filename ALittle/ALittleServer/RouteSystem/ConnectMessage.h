
#ifndef _ALITTLE_CONNECTMESSAGE_H_
#define _ALITTLE_CONNECTMESSAGE_H_

#include "Carp/carp_message.hpp"
#include "RouteIdDefine.h"

namespace ALittle
{

enum
{
	_QConnectRegister			= 100,
	_AConnectRegister			= 101,
	_QConnect2ConnectRPCRead	= 102,
	_QConnect2ConnectRPCWrite	= 102,
	_AConnect2ConnectRPCRead	= 103,
	_AConnect2ConnectRPCWrite	= 103,
};

// ������Ŀ��ע���Լ�������Ŀ������˭
CARP_MESSAGE_MACRO(QConnectRegister
	, ROUTE_ID, route_id);

// ������ע���Լ���������������˭
CARP_MESSAGE_MACRO(AConnectRegister
	, ROUTE_ID, route_id);

// �����Ե�RPC

// ���Э�����ڶ�ȡ
CARP_MESSAGE_MACRO(QConnect2ConnectRPCRead
    , int, message_id
    , int, message_size);
// ���Э�����ڷ���
CARP_MESSAGE_MACRO(QConnect2ConnectRPCWrite
	, int, message_id
	, CarpMessagePtrWapper<CarpMessage>, message_body);


// Ӧ���Ե�RPC

// ���Э�����ڶ�ȡ
CARP_MESSAGE_MACRO(AConnect2ConnectRPCRead
	, int, message_id
	, int, message_size);

// ���Э�����ڷ���
CARP_MESSAGE_MACRO(AConnect2ConnectRPCWrite
	, int, message_id
	, CarpMessagePtrWapper<CarpMessage>, message_body);

} // ALittle

#endif // _ALITTLE_CONNECTMESSAGE_H_
