
#ifndef _ALITTLE_CLIENTSERVER_H_
#define _ALITTLE_CLIENTSERVER_H_

#include <asio/asio.hpp>
#include "ALittle/LibCommon/Protocol/Message.h"

#include <map>
#include <string>
#include <memory>
#include <chrono>

namespace ALittle
{

class ClientReceiver;
typedef std::shared_ptr<ClientReceiver> ClientReceiverPtr;
typedef std::weak_ptr<ClientReceiver> ClientReceiverWeakPtr;

class ClientServer;
typedef std::shared_ptr<ClientServer> ClientServerPtr;
typedef std::weak_ptr<ClientServer> ClientServerWeakPtr;

typedef std::shared_ptr<asio::ip::tcp::socket> SocketPtr;
typedef std::shared_ptr<asio::ip::tcp::acceptor> AcceptorPtr;
typedef asio::basic_waitable_timer<std::chrono::system_clock> AsioTimer;
typedef std::shared_ptr<AsioTimer> AsioTimerPtr;

class ServerSchedule;

class ClientServer : public std::enable_shared_from_this<ClientServer>
{
public:
	friend ClientReceiver;

public:
	// ����������
    // yun_ip �Ʒ�������ӳ��ip
	// ip	��������IP
	// port �������Ķ˿�
	// heartbeat TCP������������ʱ�䣬��λ��
	bool Start(const std::string& yun_ip, const std::string& ip, int port, int heartbeat, ServerSchedule* schedule);

	// �رշ�����
	void Close();

private:
	// �ȴ���һ��socket����
	void NextAccept(int error_count);

	// �����µ�socket����
	void HandleAccept(const asio::error_code& ec, SocketPtr socket, int error_count);

public:
	// ��ȡ���صķ�����ip�Ͷ˿�
    const std::string& GetYunIp() const { return m_yun_ip; }
	const std::string& GetIp() const { return m_ip; }
	int GetPort() const { return m_port; }

private:
	AcceptorPtr m_acceptor;		// �����������ڽ����µ�socket
    std::string m_yun_ip;       // �Ʒ����������IP
	std::string m_ip;			// ���ط�������IP
	int m_port;					// ���ط������Ķ˿�

///////////////////////////////////////////////////////////////////////////////////////////////
	
private:
	// ����һ���µ�socket
	void HandleOuterConnect(SocketPtr socket);

	// ����ĳ�����ӶϿ���
	void HandleOuterDisconnected(ClientReceiverPtr receiver);

public:
	// ������Ϣ�¼�
	void HandleClientMessage(ClientReceiverPtr receiver, MESSAGE_SIZE message_size, MESSAGE_ID message_id, MESSAGE_RPCID message_rpcid, void* memory);
	
private:
	// ����ͻ������Ӷ���
	std::set<ClientReceiverPtr> m_outer_set;	// container outer
	ServerSchedule* m_schedule;

private:
	// ���������һ��ʱ����ͻ��˷���������
	void ServerSendHeatbeat(const asio::error_code& ec, int interval);
	// ��������ʱ��
	AsioTimerPtr m_heartbeat_timer;

public:
	ClientServer();
	~ClientServer();
};

} // ALittle

#endif // _ALITTLE_CLIENTSERVER_H_
