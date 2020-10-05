
#include "RouteSystem.h"
#include "../ServerSystem/ServerSchedule.h"

#include "Carp/carp_log.hpp"

#include "ConnectClient.h"
#include "ConnectServer.h"
#include "RouteMessage.h"
#include "ConnectMessage.h"
#include "RouteDefine.h"

namespace ALittle
{

void RouteSystem::ConnectRoute(ROUTE_ID route_id, SessionConnection::ConnectRouteCallback callback)
{
	// ����Ƿ��Ѿ����ӳɹ�
	CONNECT_KEY connect_key = RouteIdDefine::CalcConnectKey(m_route_id, route_id);
	auto it = m_connect_route_map.find(connect_key);
	if (it != m_connect_route_map.end())
	{
		if (callback) callback(nullptr, it->second);
		return;
	}

	// ���û�����ӳɹ�����ô�ͽ�������
	SearchRoute(route_id, std::bind(&RouteSystem::ConnectRouteSearchRouteCallback, this
								, std::placeholders::_1, std::placeholders::_2
								, callback));
}

void RouteSystem::ConnectRouteSearchRouteCallback(const std::string* reason
												, const std::list<ROUTE_ID>& route_id_list
												, SessionConnection::ConnectRouteCallback callback)
{
	// ���û��������·������ô�ͷ���ʧ��
	if (reason != nullptr)
	{
		if (callback) callback(reason, SessionConnectionPtr());
		return;
	}

	// ��ʼ����
	ConnectRoute(route_id_list, callback);
}

void RouteSystem::ConnectRoute(const std::list<ROUTE_ID>& route_id_list, SessionConnection::ConnectRouteCallback callback)
{
	if (!callback)
	{
		CARP_ERROR("callback is null");
		return;
	}

	// ����б�
	if (route_id_list.size() < 2)
	{
		std::string reason = u8"route_id_list ����Ҫ������Ԫ�أ���ʼ����յ�";
		callback(&reason, SessionConnectionPtr());
		return;
	}

	// ��ʼ��������Լ�
	if (route_id_list.front() != m_route_id)
	{
		std::string reason = u8"route_id_list ��ʼ��������Լ�";
		callback(&reason, SessionConnectionPtr());
		return;
	}

	// ����key
	CONNECT_KEY connect_key = RouteIdDefine::CalcConnectKey(route_id_list.front(), route_id_list.back());

	// ���ܳ����ظ��ڵ�
	std::set<ROUTE_ID> route_id_map;
	for (auto route_it = route_id_list.begin(); route_it != route_id_list.end(); ++route_it)
	{
		if (route_id_map.find(*route_it) != route_id_map.end())
		{
			std::string reason = u8"route_id_list �г����ظ��ڵ� route_id:" + ROUTE2S(*route_it);
			callback(&reason, SessionConnectionPtr());
			return;
		}
		route_id_map.insert(*route_it);
	}

	SessionConnectionPtr connect_info;
	auto connect_it = m_connect_route_map.find(connect_key);
	if (connect_it != m_connect_route_map.end())
	{
		// ����Ѿ������ӳɹ��ˣ���ôֱ�ӷ���
		if (connect_it->second->m_status == SessionConnection::Status::SCS_CONNECTED)
		{
			callback(nullptr, connect_it->second);
			return;
		}

		// ������������ӣ���ô��ӻص������ٷ���
		if (connect_it->second->m_status == SessionConnection::Status::SCS_CONNECTING)
		{
			connect_it->second->callback_list.push_back(callback);
			return;
		}

		// ��ȡ��Ϣ�ڴ�
		connect_info = connect_it->second;
	}
	else
	{
		// û�оʹ���һ��
		connect_info = SessionConnectionPtr(new SessionConnection);
		connect_info->m_connect_key = connect_key;
		m_connect_route_map[connect_key] = connect_info;
	}

	// ���ûص�
	connect_info->callback_list.push_back(callback);

	// ����·�������ҳ�ʼ�±�
	connect_info->m_connect_index = 0;
	connect_info->m_vector_route_path.clear();
	connect_info->m_list_route_path = route_id_list;
	for (auto route_it = route_id_list.begin(); route_it != route_id_list.end(); ++route_it)
		connect_info->m_vector_route_path.push_back(*route_it);

	// ������������
	QConnectRouteInfo msg;
	msg.connect_index = connect_info->m_connect_index + 1;
	msg.route_path = connect_info->m_vector_route_path;

	// ���Ҷ�Ӧ�Ķ˵�
	ROUTE_ID route_id = msg.route_path[msg.connect_index];
	auto endpoint_it = m_endpoint_map.find(route_id);
	if (endpoint_it == m_endpoint_map.end())
	{
		std::vector<SessionConnection::ConnectRouteCallback> callback_list = connect_info->callback_list;
		m_connect_route_map.erase(connect_key);
		
		std::string reason = "route_id:" + ROUTE2S(m_route_id) + u8" �Ҳ���route_id:" + ROUTE2S(route_id);
		for (size_t i = 0; i < callback_list.size(); ++i)
			callback_list[i](&reason, SessionConnectionPtr());
		return;
	}

	// ����״̬
	connect_info->m_status = SessionConnection::Status::SCS_CONNECTING;
	// ������һ���ڵ�
	connect_info->m_next_endpoint = endpoint_it->second;

	// ����һ���ڵ㷢����������
	ConnectEndpoint::RPCCallback ce_callback = std::bind(&RouteSystem::ConnectRouteImplCallback, this
		, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		, connect_key);
	endpoint_it->second->SendWithCallback(msg, ce_callback);
}

void RouteSystem::ConnectRouteImplCallback(const std::string* reason, int message_id, void* memory, int memroy_size, CONNECT_KEY connect_key)
{
	auto it = m_connect_route_map.find(connect_key);
	if (it == m_connect_route_map.end()) return;

	// ���������þ�ʧ�ܣ���ô���ǵ���ʧ����
	if (reason != nullptr)
	{
		std::vector<SessionConnection::ConnectRouteCallback> callback_list = it->second->callback_list;
		m_connect_route_map.erase(it);
		for (size_t i = 0; i < callback_list.size(); ++i)
			callback_list[i](reason, SessionConnectionPtr());
		return;
	}

	// ����Ƿ�����ȷ��Ӧ���
	if (message_id != AConnectRouteInfo::GetStaticID())
	{
		std::string tmp = "route_id:" + ROUTE2S(m_route_id) + u8" Ӧ�������һ��AConnectRouteInfo! message_id:" + std::to_string(message_id);
		CARP_ERROR(tmp);
		std::vector<SessionConnection::ConnectRouteCallback> callback_list = it->second->callback_list;
		m_connect_route_map.erase(it);
		for (size_t i = 0; i < callback_list.size(); ++i)
			callback_list[i](&tmp, SessionConnectionPtr());
		return;
	}

	// ����Ӧ���
	AConnectRouteInfo msg;
	if (msg.Deserialize(memory, memroy_size) < 0)
	{
		std::string tmp = "route_id:" + ROUTE2S(m_route_id) + u8" AConnectRouteInfo Deserialize ʧ��!";
		CARP_ERROR(tmp);
		std::vector<SessionConnection::ConnectRouteCallback> callback_list = it->second->callback_list;
		m_connect_route_map.erase(it);
		for (size_t i = 0; i < callback_list.size(); ++i)
			callback_list[i](&tmp, SessionConnectionPtr());
		return;
	}
	
	// �ܵ����˵�������ӳɹ���
	SessionConnectionPtr session = it->second;
	session->m_status = SessionConnection::Status::SCS_CONNECTED;
	for (size_t i = 0; i < session->callback_list.size(); ++i)
		session->callback_list[i](nullptr, session);
	session->callback_list.clear();

	// ���ûص�
    HandleConnectNotify(true, session);
}

void RouteSystem::ConnectRouteTransCallback(const std::string* reason, int message_id, void* memory, int memroy_size
											, ConnectEndpointWeakPtr src_endpoint, int src_rpcid, CONNECT_KEY connect_key)
{
	// �����ǰ�����Ӳ������ˣ���ôֱ�ӷ���
	ConnectEndpointPtr endpoint = src_endpoint.lock();
	if (!endpoint)
	{
		m_connect_route_map.erase(connect_key);
		return;
	}

	// ����Ѿ��Ͽ������ˣ���ôֱ�ӷ���
	if (endpoint->IsConnected() == false)
	{
		m_connect_route_map.erase(connect_key);
		return;
	}

	if (reason != nullptr)
	{
		m_connect_route_map.erase(connect_key);
		endpoint->SendError(src_rpcid, *reason);
		return;
	}

	if (message_id != AConnectRouteInfo::GetStaticID())
	{
		std::string tmp = "route_id:" + ROUTE2S(m_route_id) + u8"Ӧ�������һ��AConnectRouteInfo! message_id:" + std::to_string(message_id);
		CARP_ERROR(tmp);
		m_connect_route_map.erase(connect_key);
		endpoint->SendError(src_rpcid, tmp);
		return;
	}

	AConnectRouteInfo msg;
	if (msg.Deserialize(memory, memroy_size) < 0)
	{
		std::string tmp = "route_id:" + ROUTE2S(m_route_id) + u8"AConnectRouteInfo Deserialize ʧ��!";
		CARP_ERROR(tmp);
		m_connect_route_map.erase(connect_key);
		endpoint->SendError(src_rpcid, tmp);
		return;
	}

	// ����Ƿ����
	auto it = m_connect_route_map.find(connect_key);
	if (it == m_connect_route_map.end())
	{
		std::string tmp = "route_id:" + ROUTE2S(m_route_id) + u8" connect_key ������:" + CONNECT2S(connect_key);
		CARP_ERROR(tmp);
		endpoint->SendError(src_rpcid, tmp);
		return;
	}

	// �����ӱ��Ϊ���ӳɹ�
	it->second->m_status = SessionConnection::Status::SCS_CONNECTED;
	endpoint->SendResponse(src_rpcid, msg);
}

} // ALittle
