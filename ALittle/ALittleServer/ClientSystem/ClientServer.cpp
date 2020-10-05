
#include "ClientReceiver.h"
#include "ClientServer.h"

#include "Carp/carp_log.hpp"
#include "../ServerSystem/ServerSchedule.h"

namespace ALittle
{

ClientServer::ClientServer()
: m_port(0), m_schedule(nullptr)
{
}

ClientServer::~ClientServer()
{
	Close();
}

bool ClientServer::Start(const std::string& yun_ip, const std::string& ip, int port, int heartbeat, ServerSchedule* schedule)
{
	m_schedule = schedule;

	// ����Ѿ������˾�ֱ�ӷ���
	if (m_acceptor)
	{
		CARP_ERROR("Client server already started(ip: " << m_ip << ", port:" << m_port << ")");
		return false;
	}

	// ����һ����������������
	try
	{
		if (ip.size())
			m_acceptor = AcceptorPtr(new asio::ip::tcp::acceptor(schedule->GetIOService()
			, asio::ip::tcp::endpoint(asio::ip::address_v4::from_string(ip), port), false));
		else
			m_acceptor = AcceptorPtr(new asio::ip::tcp::acceptor(schedule->GetIOService()
			, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port), false));
	}
	catch (asio::error_code& ec)
	{
		m_acceptor = AcceptorPtr();
		CARP_ERROR("ClientServer: " << ip << " start failed at port: " << port << " error: " << ec.value());
		return false;
	}

	// ������ʱ��i
	m_heartbeat_timer = AsioTimerPtr(new AsioTimer(schedule->GetIOService(), std::chrono::seconds(heartbeat)));
	m_heartbeat_timer->async_wait(std::bind(&ClientServer::ServerSendHeatbeat, this->shared_from_this(), std::placeholders::_1, heartbeat));

	// ��ʼ����
	NextAccept(0);

    m_yun_ip = yun_ip;
	m_ip = ip;
	m_port = port;
	
	CARP_SYSTEM("ClientServer: start succeed at " << m_ip << ":" << m_port);
	return true;
}

void ClientServer::Close()
{
	// �رղ��ͷŽ�����
	if (m_acceptor)
	{
		m_acceptor->close();
		m_acceptor = AcceptorPtr();
	}

	// �ͷŶ�ʱ��
	if (m_heartbeat_timer)
	{
		m_heartbeat_timer->cancel();
		m_heartbeat_timer = AsioTimerPtr();
	}

	// �ر����пͻ�������
	auto end = m_outer_set.end();
	for (auto it = m_outer_set.begin(); it != end; ++it)
		(*it)->Close();
	m_outer_set.clear();

	CARP_SYSTEM("ClientServer: stop succeed.");
}

void ClientServer::NextAccept(int error_count)
{
	// ���������Ƿ�Ϸ�
	if (!m_acceptor) return;

	// ����һ��Socket����
	SocketPtr socket = SocketPtr(new asio::ip::tcp::socket(m_schedule->GetIOService()));
	// ��ʼ�ȴ�����
	m_acceptor->async_accept(*socket, std::bind(&ClientServer::HandleAccept, this->shared_from_this()
        , std::placeholders::_1, socket, error_count));
}

void ClientServer::HandleAccept(const asio::error_code& ec, SocketPtr socket, int error_count)
{
	if (ec)
	{
		CARP_ERROR("ClientServer accept failed: " << ec.value());
		if (error_count > 100)
			Close();
		else
			NextAccept(error_count + 1);
		return;
	}
	// ����Ϊno delay
	socket->set_option(asio::ip::tcp::no_delay(true));

	// �����µ�socket
	HandleOuterConnect(socket);
	
	// ������һ������
	NextAccept(0);
}

void ClientServer::HandleOuterConnect(SocketPtr socket)
{
	// ����һ���ͻ�������
	ClientReceiverPtr receiver = ClientReceiverPtr(new ClientReceiver(socket, this->shared_from_this(), m_schedule));
	// ��������
	m_outer_set.insert(receiver);

	// �ͻ����������Ͽ�ʼ�������ݰ�
	receiver->NextReadHeadFirst();

	// ֪ͨ�ͻ������ӽ�����
	m_schedule->HandleClientConnect(receiver);
}

void ClientServer::HandleOuterDisconnected(ClientReceiverPtr receiver)
{
	// �رղ��Ƴ��ͻ�������
	receiver->Close();
	m_outer_set.erase(receiver);

	// ֪ͨ�Ͽ�����
	m_schedule->HandleClientDisconnect(receiver);
}

void ClientServer::HandleClientMessage(ClientReceiverPtr receiver, CARP_MESSAGE_SIZE message_size, CARP_MESSAGE_ID message_id, CARP_MESSAGE_RPCID message_rpcid, void* memory)
{
	// ֪ͨ������Ϣ��
	m_schedule->HandleClientMessage(receiver, message_size, message_id, message_rpcid, memory);
	// �ͷ��ڴ�
	if (memory) free(memory);
}

void ClientServer::ServerSendHeatbeat(const asio::error_code& ec, int interval)
{
	// �����пͻ��˷���������
	HeartbeatMessage msg;
	auto end = m_outer_set.end();
	for (auto it = m_outer_set.begin(); it != end; ++it)
		(*it)->Send(msg);

	if (!m_heartbeat_timer) return;
	m_heartbeat_timer->expires_at(std::chrono::system_clock::now() + std::chrono::seconds(interval));
	m_heartbeat_timer->async_wait(std::bind(&ClientServer::ServerSendHeatbeat, this->shared_from_this(), std::placeholders::_1, interval));
}

} // ALittle
