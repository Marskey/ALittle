
#include "RouteSystem.h"
#include "ALittle/LibServer/ServerSystem/ServerSchedule.h"

#include "ALittle/LibCommon/Helper/LogHelper.h"
#include "ALittle/LibCommon/Helper/StringHelper.h"

#include "ConnectClient.h"
#include "ConnectServer.h"
#include "RouteMessage.h"
#include "ConnectMessage.h"
#include "RouteDefine.h"

namespace ALittle
{

void RouteSystem::SearchRoute(ROUTE_ID route_id, SearchRouteCallback callback)
{
	if (!callback)
	{
		ALITTLE_ERROR("callback is null");
		return;
	}
	
	// ���Ŀ���Ѿ��������ˣ�ֱ����ӵ��б����棬Ȼ�󷵻�
	ROUTE_ID search_key = route_id;
	auto search_it = m_search_route_map.find(search_key);
	if (search_it != m_search_route_map.end())
	{
		search_it->second.callback_list.push_back(callback);
		return;
	}
#ifdef SEARCH_ROUTE_LOG
	ALITTLE_INFO(m_route_id << u8" ����SearchRoute:" << search_key);
#endif // SEARCH_ROUTE_LOG

	// ����һ������
	SearchRouteInfo& info = m_search_route_map[search_key] = SearchRouteInfo();
	info.callback_list.push_back(callback);
	info.route_id = route_id;

	// ���ֱ���Ľڵ�
	for (auto it = m_endpoint_map.begin(); it != m_endpoint_map.end(); ++it)
	{
#ifdef SEARCH_ROUTE_LOG
		ALITTLE_INFO(search_key << u8" SearchRoute��ӵ�һ���ڵ�:" << it->second->GetTargetRouteId());
#endif // SEARCH_ROUTE_LOG
		info.AddRoute(it->second->GetTargetRouteId(), true);
	}

	// ���������������
	std::string tmp;
	SearchRouteImpl(search_key, &tmp);
}

void RouteSystem::SearchRouteImpl(ROUTE_ID search_key, const std::string* reason)
{
	auto search_it = m_search_route_map.find(search_key);
	if (search_it == m_search_route_map.end()) return;

	// ����Ѿ�û�нڵ���Կ��Լ���ˣ�˵��û��·��
	if (search_it->second.cur_index >= static_cast<int>(search_it->second.search_path.size()))
	{
#ifdef SEARCH_ROUTE_LOG
		ALITTLE_INFO(search_key << u8" SearchRouteû�нڵ���Լ���������:");
#endif // SEARCH_ROUTE_LOG

		std::string real_reason;
		if (reason != nullptr) real_reason = u8"��һ������ʧ�ܵ�ԭ��:" + *reason + ", ";
		real_reason += u8"���:�����Ѿ�û�нڵ���Լ��������ˣ��Ҳ���ͨ��Ŀ���·��, route_id:" + ROUTE2S(search_it->second.route_id);

		// �ȸ���һ�ݻص�������Ȼ���ӳ������Ƴ�
		std::vector<SearchRouteCallback> callback_list = search_it->second.callback_list;
		m_search_route_map.erase(search_it);

		// ����ӳ�����ûص�
		std::list<ROUTE_ID> temp;
		for (size_t i = 0; i < callback_list.size(); ++i)
			callback_list[i](&real_reason, temp);
		return;
	}

	// ȡ����ǰ�ڵ�
	SearchPathInfo& info = search_it->second.search_path[search_it->second.cur_index];
	// ��鵱ǰ�ڵ��Ƿ����������������Ͼͷ��ؽ��
	if (search_it->second.route_id != 0 && search_it->second.route_id == info.route_id)
	{
#ifdef SEARCH_ROUTE_LOG
		ALITTLE_INFO(search_key << u8" SearchRoute�ҵ�������������:");
#endif // SEARCH_ROUTE_LOG

		std::list<ROUTE_ID> temp;
		int cur_index = search_it->second.cur_index;
		while (cur_index >= 0)
		{
			temp.push_front(search_it->second.search_path[cur_index].route_id);
			cur_index = search_it->second.search_path[cur_index].parent_index;
		}

		// �ȸ���һ�ݻص�������Ȼ���ӳ������Ƴ�
		std::vector<SearchRouteCallback> callback_list = search_it->second.callback_list;
		m_search_route_map.erase(search_it);

		// ����ӳ�����ûص�
		temp.push_front(m_route_id);	// ���Լ�������б�ͷ������������·��
		for (size_t i = 0; i < callback_list.size(); ++i)
			callback_list[i](nullptr, temp);
		return;
	}

	// ���Ӧ���ն˷���ѯ������
	QSearchRouteInfo msg;
	int cur_index = search_it->second.cur_index;
	while (cur_index >= 0)
	{
		msg.route_path.push_front(search_it->second.search_path[cur_index].route_id);
		cur_index = search_it->second.search_path[cur_index].parent_index;
	}

	// ��ȡ��һ���ڵ㣬Ȼ���͡����û�ҵ�����ô����������ڵ�
	auto endpoint_it = m_endpoint_map.find(msg.route_path.front());
	if (endpoint_it == m_endpoint_map.end())
	{
#ifdef SEARCH_ROUTE_LOG
		ALITTLE_INFO(search_key << u8" SearchRoute�����ĳ�ʼ�ڵ��Ҳ����ˣ�������һ��");
#endif // SEARCH_ROUTE_LOG
		++ search_it->second.cur_index;
		std::string tmp = "route_id:" + ROUTE2S(m_route_id) + u8" �Ҳ���route_id:" + ROUTE2S(msg.route_path.front());
		SearchRouteImpl(search_key, &tmp);
		return;
	}

#ifdef SEARCH_ROUTE_LOG
	ALITTLE_INFO(search_key << u8" SearchRoute����ɿ���������" << endpoint_it->second->m_target_route_id
				<< u8", ·��Ϊ:" << StringHelper::JoinString(msg.route_path, "->"));
#endif // SEARCH_ROUTE_LOG
	// ��ʼѯ��
	ConnectEndpoint::RPCCallback callback = std::bind(&RouteSystem::SearchRouteImplCallback, this
		, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		, search_key);
	endpoint_it->second->SendWithCallback(msg, callback);
}

void RouteSystem::SearchRouteImplCallback(const std::string* reason, int message_id, void* memory, int memroy_size
										, ROUTE_ID search_key)
{
#ifdef SEARCH_ROUTE_LOG
	ALITTLE_INFO(search_key << u8" SearchRoute�յ��ɿ���Ӧ��");
#endif // SEARCH_ROUTE_LOG

	auto it = m_search_route_map.find(search_key);
	if (it == m_search_route_map.end()) return;

	// ���������þ�ʧ�ܣ���ô�ͽ�����һ���ڵ�
	if (reason != nullptr)
	{
		++ it->second.cur_index;
		SearchRouteImpl(search_key, reason);
		return;
	}

	// ����Ƿ�����ȷ��Ӧ���
	if (message_id != ASearchRouteInfo::GetStaticID())
	{
		std::string tmp = "route_id:" + ROUTE2S(m_route_id) + u8"Ӧ�������һ��ASearchRouteInfo! message_id:" + X2S(message_id);
		ALITTLE_ERROR(tmp);
		++it->second.cur_index;
		SearchRouteImpl(search_key, &tmp);
		return;
	}

	// ����Ӧ���
	ASearchRouteInfo msg;
	if (msg.Deserialize(memory, memroy_size) < 0)
	{
		std::string tmp = "route_id:" + ROUTE2S(m_route_id) + u8"ASearchRouteInfo Deserialize ʧ��!";
		ALITTLE_ERROR(tmp);
		++it->second.cur_index;
		SearchRouteImpl(search_key, &tmp);
		return;
	}

	// �ѻ�õ����½ڵ���ӽ�ȥ
	auto id_list_it = msg.route_id_list.begin();
	while (id_list_it != msg.route_id_list.end())
	{
#ifdef SEARCH_ROUTE_LOG
		ALITTLE_INFO(search_key << u8" SearchRoute�����ڵ�:" << *id_list_it);
#endif // SEARCH_ROUTE_LOG
		it->second.AddRoute(*id_list_it, false);
		++ id_list_it;
	}

	// ִ����һ���ڵ�
	++ it->second.cur_index;
	SearchRouteImpl(search_key, reason);
}

void RouteSystem::SearchRouteTransCallback(const std::string* reason, int message_id, void* memory, int memroy_size
										, ConnectEndpointWeakPtr src_endpoint, int src_rpcid)
{
	// �����ǰ�����Ӳ������ˣ���ôֱ�ӷ���
	ConnectEndpointPtr endpoint = src_endpoint.lock();
	if (!endpoint)
	{
		ALITTLE_SYSTEM(u8"�����Ѿ���������");
		return;
	}

	// ����Ѿ��Ͽ������ˣ���ôֱ�ӷ���
	if (endpoint->IsConnected() == false)
	{
		ALITTLE_SYSTEM(u8"���ӶϿ���");
		return;
	}

	ASearchRouteInfo msg;
	// ����ɿ��Ե���ʧ�ܣ���ô��ԭ��ֱ�Ӵ���ȥ
	if (reason != nullptr)
	{
		endpoint->SendError(src_rpcid, *reason);
		return;
	}

	if (message_id != ASearchRouteInfo::GetStaticID())
	{
		std::string reason = "route_id:" + ROUTE2S(m_route_id) + u8"Ӧ�������һ��ASearchRouteInfo! message_id:" + X2S(message_id);
		ALITTLE_ERROR(reason);
		endpoint->SendError(src_rpcid, reason);
		return;
	}
	if (msg.Deserialize(memory, memroy_size) < 0)
	{
		std::string reason = "route_id:" + ROUTE2S(m_route_id) + u8"ASearchRouteInfo Deserialize ʧ��!";
		ALITTLE_ERROR(reason);
		endpoint->SendError(src_rpcid, reason);
		return;
	}

	// �����ݰ�ת����ȥ
	endpoint->SendResponse(src_rpcid, msg);
}

} // ALittle
