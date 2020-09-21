
#ifndef _ALITTLE_ROUTEMESSAGE_H_
#define _ALITTLE_ROUTEMESSAGE_H_

#include "Carp/carp_message.hpp"
#include "RouteIdDefine.h"

namespace ALittle
{

enum
{
	_QSearchRouteInfo			= 200,
	_ASearchRouteInfo			= 201,
	_QConnectRouteInfo			= 202,
	_AConnectRouteInfo			= 203,
	_ConnectRouteDisconnected	= 204,
	_ConnectRouteMessageRead	= 205,
	_ConnectRouteMessageWrite	= 205,
};

// ��������·��
CARP_MESSAGE_MACRO(QSearchRouteInfo
	, std::list<ROUTE_ID>, route_path);
	
// Ӧ������·��
CARP_MESSAGE_MACRO(ASearchRouteInfo
	, std::list<ROUTE_ID>, route_id_list);
	
// ��������·��
CARP_MESSAGE_MACRO(QConnectRouteInfo
	, int, connect_index
	, std::vector<ROUTE_ID>, route_path);
	
// Ӧ�����ӳɹ�
CARP_MESSAGE_MACRO(AConnectRouteInfo);

// ·�����ӶϿ���
CARP_MESSAGE_MACRO(ConnectRouteDisconnected
	, CONNECT_KEY, connect_key);

// ·�ɻ����ϵ���Ϣ�������ڶ�ȡ
CARP_MESSAGE_MACRO(ConnectRouteMessageRead
    , CONNECT_KEY, connect_key
    , int, message_id
	, int, message_rpcid
    , int, message_size);

// ·�ɻ����ϵ���Ϣ��������д
CARP_MESSAGE_MACRO(ConnectRouteMessageWrite
	, CONNECT_KEY, connect_key
	, int, message_id
	, int, message_rpcid
	, CarpMessagePtrWapper<CarpMessage>, message_body);

} // ALittle

#endif // _ALITTLE_ROUTEMESSAGE_H_
