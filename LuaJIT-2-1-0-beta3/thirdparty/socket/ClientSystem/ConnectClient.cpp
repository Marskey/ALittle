
#include "ConnectClient.h"
#include "../ServerSystem/SocketSchedule.h"
extern "C" {
	#include "../socket.h"
}

namespace ALittle
{
	 
ConnectClient::ConnectClient(_socket* c, SocketSchedule* schedule, int id)
: m_port(0), m_binary_value(0), m_excuting(false), m_is_connecting(false), m_id(id)
{
}

ConnectClient::~ConnectClient()
{
	Close();
	// �ͷ��ڴ�
	if (m_binary_value) { free(m_binary_value); m_binary_value = 0; }
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
	// if (m_binary_value) { free(m_binary_value); m_binary_value = 0; }
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
	socket_event* event = socket_createevent(m__socket);
	event->type = socket_event_types::MSG_CONNECT_FAILED;
	event->id = m_id;
	socket_addevent(m__socket, event, 0);

	// �ͷ��Լ�
	// ��������������ã���������൱��delete�Լ�
	m_schedule->m_connect_map.erase(m_id);
}

void ConnectClient::HandleConnectSucceed()
{
	socket_event* event = socket_createevent(m__socket);
	event->type = socket_event_types::MSG_CONNECT_SUCCEED;
	event->id = m_id;
	socket_addevent(m__socket, event, 0);
}

void ConnectClient::HandleDisconnected()
{
	socket_event* event = socket_createevent(m__socket);
	event->type = socket_event_types::MSG_DISCONNECTED;
	event->id = m_id;
	socket_addevent(m__socket, event, 0);

	// �ͷ��Լ�
	// ��������������ã���������൱��delete�Լ�
	m_schedule->m_connect_map.erase(m_id);
}

void ConnectClient::ReadNumber(int size, int read_type)
{
	// ����Ѿ��ͷ��˾�ֱ�ӷ���
	if (!m_socket) return;

	// ��ʼ����Э��ͷ
	asio::async_read(*m_socket, asio::buffer(m_simple_value, size)
					, std::bind(&ConnectClient::HandleReadNumber, this->shared_from_this()
					, std::placeholders::_1, std::placeholders::_2, read_type));
}

void ConnectClient::HandleReadNumber(const asio::error_code& ec, std::size_t actual_size, int type)
{
	if (ec)
	{
		// �ͷ��ڴ�
		if (m_binary_value) { free(m_binary_value); m_binary_value = 0; }
		ExecuteDisconnectCallback();
		return;
	}

	socket_event* event = socket_createevent(m__socket);
	event->type = (socket_event_types)type;
	switch (event->type)
	{
	case socket_event_types::MSG_READ_UINT8: event->int_value = *((unsigned char*)m_simple_value); break;
	case socket_event_types::MSG_READ_INT8: event->int_value = *((char*)m_simple_value); break;
	case socket_event_types::MSG_READ_UINT16: event->int_value = *((unsigned short*)m_simple_value); break;
	case socket_event_types::MSG_READ_INT16: event->int_value = *((short*)m_simple_value); break;
	case socket_event_types::MSG_READ_UINT32: event->int_value = *((unsigned int*)m_simple_value); break;
	case socket_event_types::MSG_READ_INT32: event->int_value = *((int*)m_simple_value); break;
	case socket_event_types::MSG_READ_UINT64: event->int_value = *((unsigned long long*)m_simple_value); break;
	case socket_event_types::MSG_READ_INT64: event->int_value = *((long long*)m_simple_value); break;
	case socket_event_types::MSG_READ_FLOAT: event->double_value = *((float*)m_simple_value); break;
	case socket_event_types::MSG_READ_DOUBLE: event->double_value = *((double*)m_simple_value); break;
	}
	socket_addevent(m__socket, event, 0);
}

void ConnectClient::ReadBinary(int size, int read_type)
{
	// ����Ѿ��ͷ��˾�ֱ�ӷ���
	if (!m_socket) return;
	if (m_binary_value != 0) return;

	m_binary_value = (char*)malloc(size);

	// ��ʼ����Э��ͷ
	asio::async_read(*m_socket, asio::buffer(m_binary_value, size)
		, std::bind(&ConnectClient::HandleReadBinary, this->shared_from_this()
			, std::placeholders::_1, std::placeholders::_2, read_type));
}

void ConnectClient::HandleReadBinary(const asio::error_code& ec, std::size_t actual_size, int type)
{
	if (ec)
	{
		// �ͷ��ڴ�
		if (m_binary_value) { free(m_binary_value); m_binary_value = 0; }
		ExecuteDisconnectCallback();
		return;
	}

	socket_event* event = socket_createevent(m__socket);
	event->type = (socket_event_types)type;
	event->binary_value = m_binary_value;
	m_binary_value = 0;
	socket_addevent(m__socket, event, 0);
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

} // ALittle
