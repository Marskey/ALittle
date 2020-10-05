
#include "ClientReceiver.h"
#include "ClientServer.h"

#include "Carp/carp_log.hpp"
#include "../ServerSystem/ServerSchedule.h"

#include <functional>

namespace ALittle
{

ClientReceiver::ClientReceiver(SocketPtr socket, ClientServerWeakPtr server, ServerSchedule* schedule)
    : m_socket(socket), m_server(server), m_memory(0), m_excuting(false), m_is_connected(true), m_schedule(schedule)
	, m_is_websocket(false), m_websocket_buffer(0), m_frame_buffer(), m_has_mark(false), m_opcode(0), m_data_length(0)
	, m_head_size(0), m_body_size(0), m_current_is_message(false)
	, m_mark(), m_message_head()
{
	// ��ȡ�ͻ��˵Ĺ���IP
	m_remote_ip = socket->remote_endpoint().address().to_string();
	m_remote_port = socket->remote_endpoint().port();
}

ClientReceiver::~ClientReceiver()
{
	// �ر�socket���ͷ���Դ
	Close();
	// �ͷ��ڴ�
	if (m_memory) { free(m_memory); m_memory = 0; }
	if (m_websocket_buffer) { free(m_websocket_buffer); m_websocket_buffer = 0; }
}

void ClientReceiver::NextReadHeadFirst()
{
	asio::async_read(*m_socket, asio::buffer(m_message_head, sizeof(m_message_head))
		, std::bind(&ClientReceiver::HandleReadHeadFirst, this->shared_from_this()
			, std::placeholders::_1, std::placeholders::_2));
}

void ClientReceiver::HandleReadHeadFirst(const asio::error_code& ec, std::size_t actual_size)
{
	if (ec)
	{
		CARP_SYSTEM("receive failed:" << ec.value());
		ClientServerPtr server = m_server.lock();
		if (server)	server->HandleOuterDisconnected(this->shared_from_this());
		return;
	}

	// check first char, if is 'G' then is websocket, otherwise binary
	if (m_message_head[0] != 'G')
		HandleReadHeadBinary(ec, actual_size);
	else
	{
		m_is_websocket = true;
		HandleWebSocketHandShakeReceive(ec, actual_size);
	}
}

void ClientReceiver::ReadComplete()
{
	// ���ô�СΪ0
	m_head_size = 0;
	m_body_size = 0;

	// ���������Ƿ񻹴���
	ClientServerPtr server = m_server.lock();
	if (!server)
	{
		// �ͷ��ڴ�
		if (m_memory) { free(m_memory); m_memory = 0; }
		return;
	}

	// ��ȡЭ��ͷ����Ϣ
	CARP_MESSAGE_SIZE* message_size = (CARP_MESSAGE_SIZE*)m_message_head;
	CARP_MESSAGE_ID* message_id = (CARP_MESSAGE_ID*)(m_message_head + sizeof(CARP_MESSAGE_SIZE));
	CARP_MESSAGE_RPCID* message_rpcid = (CARP_MESSAGE_RPCID*)(m_message_head + sizeof(CARP_MESSAGE_SIZE) + sizeof(CARP_MESSAGE_ID));

	// ͨ����Ϣ����ִ�з��Ͳ���
	m_schedule->Execute(std::bind(&ClientServer::HandleClientMessage, server, this->shared_from_this()
		, *message_size, *message_id, *message_rpcid, m_memory));
	// �ڴ��Ѿ��ƽ���ȥ�ˣ�HandleClientMessage��������ڲ��Ḻ���ͷ�
	// ��������
	m_memory = 0;
}

void ClientReceiver::Close()
{
	// �ͷ����ڷ��͵����ݰ�
	auto end = m_pocket_list.end();
	for (auto it = m_pocket_list.begin(); it != end; ++it)
		free(it->memory);
	m_pocket_list.clear();

	// ��ǲ�������ִ��
	m_excuting = false;

	// �ر�socket
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

void ClientReceiver::Send(const CarpMessage& message)
{
	if (m_is_websocket)
		SendWebSocket(message);
	else
		SendBinary(message);
}

void ClientReceiver::SendPocket(void* memory, int memory_size)
{
	// ������Ϣ
	PocketInfo info;
	info.memory_size = memory_size;
	info.memory = memory;

	// ���һ�����ݰ�
	m_pocket_list.push_back(info);
	// ����������ڷ��ͣ���ô�ͷ���
	if (m_excuting) return;
	// ���Ϊ���ڷ���
	m_excuting = true;
	// ����һ����
	NextSend();
}

void ClientReceiver::NextSend()
{
	// ���û�����ݰ��ˣ�����socket�Ѿ����ر��ˣ��ͷ���
	if (m_pocket_list.empty() || !m_socket)
	{
		m_excuting = false;
		return;
	}

	// ��ȡ�ڴ�����
	PocketInfo info = m_pocket_list.front();
	m_pocket_list.pop_front();

	// �첽����
	asio::async_write(*m_socket, asio::buffer(info.memory, info.memory_size)
					, std::bind(&ClientReceiver::HandleSend, this->shared_from_this()
					, std::placeholders::_1, std::placeholders::_2, info.memory));
}

void ClientReceiver::HandleSend(const asio::error_code& ec, std::size_t bytes_transferred, void* memory)
{
	// �ͷ��ڴ�
	if (memory) free(memory);

	if (ec)
	{
		// �������ʧ���ˣ�˵���Ѿ��Ͽ�������
		CARP_SYSTEM("send failed:" << ec.value());
		ClientServerPtr server = m_server.lock();
		if (server) server->HandleOuterDisconnected(this->shared_from_this());
		return;
	}

	// ������һ�����ݰ�
	NextSend();
}

} // ALittle
