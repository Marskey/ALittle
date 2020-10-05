
#include "ClientReceiver.h"
#include "ClientServer.h"

#include "Carp/carp_log.hpp"
#include "../ServerSystem/ServerSchedule.h"

#include <functional>

namespace ALittle
{

void ClientReceiver::NextReadHeadBinary()
{
	// ���socket�Ѿ����ͷţ���ֱ�ӷ���
	if (!m_socket) return;

	// �첽��ȡ����
	asio::async_read(*m_socket, asio::buffer(m_message_head, sizeof(m_message_head))
					, std::bind(&ClientReceiver::HandleReadHeadBinary, this->shared_from_this()
					, std::placeholders::_1, std::placeholders::_2));
}

void ClientReceiver::HandleReadHeadBinary(const asio::error_code& ec, std::size_t actual_size)
{
	if (ec)
	{
		// �ͷ��ڴ�
		if (m_memory) { free(m_memory); m_memory = 0; }
		// ��ȡʧ����˵�������ӶϿ��ˣ�֪ͨ��server
		CARP_SYSTEM("ClientReceiver::HandleReadHead receive failed:" << ec.value());
		ClientServerPtr server = m_server.lock();
		if (server)	server->HandleOuterDisconnected(this->shared_from_this());
		return;
	}

	// ��Э��ͷ��ȡ���ݰ���С
	CARP_MESSAGE_SIZE message_size = *(CARP_MESSAGE_SIZE*)m_message_head;
	
	// ������ݰ���СΪ0����ô��ֱ��ִ�����
	if (message_size == 0)
	{
		// ��ȡ��ɲ���
		ReadComplete();
		// ������һ����ȡ
		NextReadHeadBinary();
		return;
	}

	// ��Э���壬��ô�������ڴ�
	if (m_memory) { free(m_memory); m_memory = 0; }
	m_memory = malloc(message_size);
	
	// ��ȡЭ����
	asio::async_read(*m_socket, asio::buffer(m_memory, message_size)
		, std::bind(&ClientReceiver::HandleReadBodyBinary, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void ClientReceiver::HandleReadBodyBinary(const asio::error_code& ec, std::size_t actual_size)
{
	if (ec)
	{
		// �ͷ��ڴ�
		if (m_memory) { free(m_memory); m_memory = 0; }
		// ��ȡʧ����˵���ǶϿ��ˣ�֪ͨ��server
		CARP_SYSTEM("ClientReceiver::HandleReadBody receive failed:" << ec.value());
		ClientServerPtr server = m_server.lock();
		if (server) server->HandleOuterDisconnected(this->shared_from_this());
		return;
	}

	// ��ȡ��ɲ���
	ReadComplete();
	// ������һ����ȡ
	NextReadHeadBinary();
}

void ClientReceiver::SendBinary(const CarpMessage& message)
{
	// ����Ѿ��رգ���ô�Ͳ��������ݰ�
	if (m_is_connected == false) return;

	// ��ȡ��Ϣ���ܴ�С
	CARP_MESSAGE_SIZE message_size = message.GetTotalSize();
	// ��ȡ��Ϣ��ID
	CARP_MESSAGE_ID message_id = message.GetID();
	// ��ȡRPCID
	CARP_MESSAGE_RPCID message_rpcid = message.GetRpcID();

	// �����ڴ��С
	int memory_size = message_size + CARP_PROTOCOL_HEAD_SIZE;

	// �����ڴ�
	void* memory = malloc(memory_size);
    if (memory == nullptr)
    {
        CARP_ERROR("memory is null");
        return;
    }
	char* body_memory = static_cast<char*>(memory);

	// д����Ϣ����С��ID
	memcpy(body_memory, &message_size, sizeof(CARP_MESSAGE_SIZE));
	body_memory += sizeof(CARP_MESSAGE_SIZE);
	memcpy(body_memory, &message_id, sizeof(CARP_MESSAGE_ID));
	body_memory += sizeof(CARP_MESSAGE_ID);
	memcpy(body_memory, &message_rpcid, sizeof(CARP_MESSAGE_RPCID));
	body_memory += sizeof(CARP_MESSAGE_RPCID);

	// ϵ�л���Ϣ
	message.Serialize(body_memory);

	// �������ݰ�
	SendPocket(memory, memory_size);
}

} // ALittle
