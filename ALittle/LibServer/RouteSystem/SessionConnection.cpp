
#include "SessionConnection.h"
#include "ALittle/LibServer/ServerSystem/ServerSchedule.h"

#include "ALittle/LibCommon/Helper/LogHelper.h"
#include "ALittle/LibCommon/Helper/StringHelper.h"

#include "RouteMessage.h"
#include "ConnectEndpoint.h"

namespace ALittle
{

SessionConnection::~SessionConnection()
{
}

bool SessionConnection::Send(const Message& msg)
{
	// ������ڵ㲻����ʼ������յ㣬��ô��û��Ȩ���������ݰ�
	if (m_connect_index > 0 && m_connect_index + 1 < (int)m_vector_route_path.size())
	{
		ALITTLE_ERROR(u8"���ڵ㲻������Ự���ӵĶ˵㣬���ܷ������ݰ� " << GetConnectKey());
		return false;
	}

	// �ҵ���Ӧ�����ӵ�
	ConnectEndpointPtr target_endpoint = m_pre_endpoint.lock();
	if (!target_endpoint) target_endpoint = m_next_endpoint.lock();
	if (!target_endpoint)
	{
		ALITTLE_ERROR(u8"pre_endpoint��next_endpoint���ǿ�ָ�룬���ܷ������ݰ� " << CONNECT2S(GetConnectKey()));
		return false;
	}

	// �������ݰ�
	ConnectRouteMessageWrite wrap_msg;
	wrap_msg.connect_key = GetConnectKey();
	wrap_msg.message_id = msg.GetID();
	wrap_msg.message_rpcid = msg.GetRpcID();
	wrap_msg.message_body = (Message*)&msg;
	target_endpoint->Send(wrap_msg);

	return true;
}

bool SessionConnection::Close()
{
	// ������Ǵ�������״̬�ģ����ܹر�����
	if (m_status != SessionConnection::Status::SCS_CONNECTED)
		return false;

	// �������·���������յ㣬��ôû��Ȩ���ر�����
	if (m_connect_index > 0 && m_connect_index + 1 < (int)m_vector_route_path.size())
		return false;
	
	// �ѵ�ǰ״̬���Ϊ
	m_status = SessionConnection::Status::SCS_IDLE;

	// ��ǰ��ͺ���Ľڵ�֪ͨ�Ͽ����ӵ����ݰ�
	ConnectRouteDisconnected msg;
	msg.connect_key = m_connect_key;
	ConnectEndpointPtr pre_endpoint = m_pre_endpoint.lock();
	if (pre_endpoint && pre_endpoint->IsConnected()) pre_endpoint->Send(msg);
	pre_endpoint = ConnectEndpointPtr();
	ConnectEndpointPtr next_endpoint = m_next_endpoint.lock();
	if (next_endpoint && next_endpoint->IsConnected()) next_endpoint->Send(msg);
	next_endpoint = ConnectEndpointPtr();
	return true;
}

} // ALittle
