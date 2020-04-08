
#include "ConnectClient.h"
#include "../ServerSystem/ServerSchedule.h"
extern "C" {
	#include "../net.h"
}

#include "../Helper/StringHelper.h"

namespace ALittle
{
	 
ConnectClient::ConnectClient(_net* c, ServerSchedule* schedule, int id)
: m_port(0), m_memory(0), m_excuting(false), m_is_connecting(false), m_message_head(), m_id(id)
{
}

ConnectClient::~ConnectClient()
{
	Close();
	// �ͷ��ڴ�
	if (m_memory) { free(m_memory); m_memory = 0; }
}

void ConnectClient::Connect(const std::string& ip, unsigned int port)
{
	// ����Ƿ���������
	if (IsConnected() || IsConnecting()) return;

	// ���Ϊ��������
	m_is_connecting = true;
	// ����һ��socket����
	m_socket = SocketPtr(new asio::ip::tcp::socket(m_schedule->GetIOService()));
	// ����һ��Ŀ������������ӵ�
	asio::ip::tcp::endpoint ep(asio::ip::address_v4::from_string(ip), port);

	// ���沢��ʼ��
	m_ip = ip;
	m_port = port;

	// ��ʼ�첽����
	m_socket->async_connect(ep, std::bind(&ConnectClient::HandleAsyncConnect, this->shared_from_this(), std::placeholders::_1));
}

void ConnectClient::HandleAsyncConnect(const asio::error_code& ec)
{
	if (ec)
	{
		// �����־����ӡ����Ϊ�����̫�࣬�ֲ���Ҫ
		// ALITTLE_SYSTEM(u8"ConnectClient ����ʧ��: " << SUTF8(asio::system_error(ec).what()) << ", ip:" << m_ip << ", port:" << m_port);
		
		// ��������ʧ��
		HandleConnectFailed();
		return;
	}
	// ���� nodelay
	m_socket->set_option(asio::ip::tcp::no_delay(true));
	
	// ���Ϊ�������ڷ���
	m_excuting = false;
	// ���Ϊ������������
	m_is_connecting = false;

	// ��ʼ������Ϣ��
	NextReadHead();

	// �������ӳɹ�
	HandleConnectSucceed();
}

bool ConnectClient::IsConnected()
{
	return m_is_connecting == false && m_socket != SocketPtr();
}

bool ConnectClient::IsConnecting()
{
	return m_is_connecting;
}

void ConnectClient::Close()
{
	// �ͷŴ����͵���Ϣ��
	auto end = m_pocket_list.end();
	for (auto it = m_pocket_list.begin(); it != end; ++it)
		free(it->memory);
	m_pocket_list.clear();

	// ���Ϊ�������ڷ���
	m_excuting = false;
	// ���Ϊ������������
	m_is_connecting = false;

	// �ͷ�socket
	if (m_socket)
	{
		asio::error_code ec;
		m_socket->close(ec);
	}
	m_socket = SocketPtr();

	// ���ﲻҪ�����ͷ�m_memory������asio������
	// �ŵ��������������ͷ�
	// if (m_memory) { free(m_memory); m_memory = 0; }
}

void ConnectClient::ExecuteDisconnectCallback()
{
	// ���socket�ǿ�ָ�룬��ô�϶�����������Close�����
	// ���ʱ�����ڶϿ����ӣ����Բ�Ҫ����HandleDisconnected()
	bool close_by_self = (m_socket == SocketPtr());

	// �رգ��ڲ����m_socket����Ϊ��ָ��
	// ���Լ�ʹͬʱ��Ϊ����ʧ�ܻ��߷���ʧ�ܶ�������ExecuteDisconnectCallbackҲ�����ε���HandleDisconnected
	Close();

	// ��������Լ��رյģ���ô�͵��ûص�
	if (close_by_self == false) HandleDisconnected();
}

void ConnectClient::HandleConnectFailed()
{
	net_event* event = net_createevent(m_net);
	event->type = net_event_types::MSG_CONNECT_FAILED;
	event->id = m_id;
	net_addevent(m_net, event, 0);

	// �ͷ��Լ�
	// ��������������ã���������൱��delete�Լ�
	m_schedule->m_connect_map.erase(m_id);
}

void ConnectClient::HandleConnectSucceed()
{
	net_event* event = net_createevent(m_net);
	event->type = net_event_types::MSG_CONNECT_SUCCEED;
	event->id = m_id;
	net_addevent(m_net, event, 0);
}

void ConnectClient::HandleDisconnected()
{
	net_event* event = net_createevent(m_net);
	event->type = net_event_types::MSG_DISCONNECTED;
	event->id = m_id;
	net_addevent(m_net, event, 0);

	// �ͷ��Լ�
	// ��������������ã���������൱��delete�Լ�
	m_schedule->m_connect_map.erase(m_id);
}

void ConnectClient::NextReadHead()
{
	// ����Ѿ��ͷ��˾�ֱ�ӷ���
	if (!m_socket) return;

	// ��ʼ����Э��ͷ
	asio::async_read(*m_socket, asio::buffer(m_message_head, sizeof(m_message_head))
					, std::bind(&ConnectClient::HandleReadHead, this->shared_from_this()
					, std::placeholders::_1, std::placeholders::_2));
}

void ConnectClient::HandleReadHead(const asio::error_code& ec, std::size_t actual_size)
{
	if (ec)
	{
		// �ͷ��ڴ�
		if (m_memory) { free(m_memory); m_memory = 0; }
		ExecuteDisconnectCallback();
		return;
	}

	// ��ȡЭ���С
	MESSAGE_SIZE message_size = *(MESSAGE_SIZE*)m_message_head;
	
	// �����ڴ�
	if (m_memory) { free(m_memory); m_memory = 0; }
	m_memory = malloc(message_size + PROTOCOL_HEAD_SIZE);
	char* body_memory = (char*)m_memory;

	// Э��ͷ���Ƶ��ڴ�
	memcpy(body_memory, m_message_head, sizeof(m_message_head));

	// ���û��Э�����ʾ��ȡ���
	if (message_size == 0)
	{
		ReadComplete();
		NextReadHead();
		return;
	}

	// ��ʼ��ȡЭ����
	asio::async_read(*m_socket, asio::buffer((char*)m_memory + PROTOCOL_HEAD_SIZE, message_size)
		, std::bind(&ConnectClient::HandleReadBody, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void ConnectClient::HandleReadBody(const asio::error_code& ec, std::size_t actual_size)
{
	if (ec)
	{
		// �ͷ��ڴ�
		if (m_memory) { free(m_memory); m_memory = 0; }
		// ֪ͨ�Ͽ�����
		ExecuteDisconnectCallback();
		return;
	}

	// ����Э��
	ReadComplete();
	// ������ȡ��һ����Ϣ��
	NextReadHead();
}

void ConnectClient::ReadComplete()
{
	// ��ȡЭ���С
	MESSAGE_SIZE message_size = *(MESSAGE_SIZE*)m_message_head;
	// ���͸�����ϵͳ
	m_schedule->Execute(std::bind(&ConnectClient::HandleMessage, this->shared_from_this(), m_memory, message_size + PROTOCOL_HEAD_SIZE));
	// �ڴ��Ѿ��ƽ���ȥ��HandleMessage�Ḻ���ͷ�
	// ������0
	m_memory = 0;
}

void ConnectClient::Send(const _write_factory* factory)
{
	// ��ȡЭ���С
	MESSAGE_SIZE message_size = factory->size;
	// ��ȡЭ��ID
	MESSAGE_ID message_id = factory->id;
	// ��ȡRPCID
	MESSAGE_RPCID message_rpcid = factory->rpc_id;

	// Э���С = Э�����С + Э��ͷ��С
	unsigned int memory_size = message_size + PROTOCOL_HEAD_SIZE;

	// �����ڴ�
	void* memory = malloc(memory_size);
	char* body_memory = (char*)memory;

	// ����Э��ͷ��Ϣ
	memcpy(body_memory, &message_size, sizeof(MESSAGE_SIZE));
	body_memory += sizeof(MESSAGE_SIZE);
	memcpy(body_memory, &message_id, sizeof(MESSAGE_ID));
	body_memory += sizeof(MESSAGE_ID);
	memcpy(body_memory, &message_rpcid, sizeof(MESSAGE_RPCID));
	body_memory += sizeof(MESSAGE_RPCID);
	
	// ���л���Ϣ��
	if (factory->size > 0)
		memcpy(body_memory, &(kv_A(factory->memory, 0)), factory->size);
	
	// ����
	SendPocket(memory, memory_size);
}

void ConnectClient::SendPocket(void* memory, int memory_size)
{
	// �����ڴ�ṹ
	PocketInfo info;
	info.memory_size = memory_size;
	info.memory = memory;

	// ��ӵ��������б�
	m_pocket_list.push_back(info);
	// ����Ѿ����ڷ����ˣ���ô��ֱ�ӷ���
	if (m_excuting) return;
	// ������ڷ���
	m_excuting = true;
	// ����һ����Ϣ��
	NextSend();
}

void ConnectClient::NextSend()
{
	// ������б��ǿյģ�����socket�Ѿ��ر��ˣ�ֱ�ӷ���
	if (m_pocket_list.empty() || !m_socket)
	{
		m_excuting = false;
		return;
	}

	// ��ȡһ���ṹ��
	PocketInfo info = m_pocket_list.front();
	m_pocket_list.pop_front();

	// ����
	asio::async_write(*m_socket, asio::buffer(info.memory, info.memory_size)
		, std::bind(&ConnectClient::HandleSend, this->shared_from_this()
		, std::placeholders::_1, std::placeholders::_2, info.memory));
}

void ConnectClient::HandleSend(const asio::error_code& ec, std::size_t bytes_transferred, void* memory)
{
	// �ͷ��ڴ�
	if (memory) free(memory);

	// ������
	if (ec)
	{
		// ���ﲻ֪ͨ�Ͽ����ӣ��ȴ������ǲ���֪ͨ�Ͽ�
		ExecuteDisconnectCallback();
		return;
	}
	
	// ������һ����
	NextSend();
}

void ConnectClient::HandleMessage(void* memory, int memory_size)
{
	net_event* event = net_createevent(m_net);
	event->type = net_event_types::MSG_MESSAGE;
	event->id = m_id;
	event->rfactory = net_createrfactory(m_net, (char*)memory, memory_size);
	net_addevent(m_net, event, 0);
}

} // ALittle
