
#ifndef _ALITTLE_CONNECTCLIENT_H_
#define _ALITTLE_CONNECTCLIENT_H_

#include <asio/asio.hpp>

#include <memory>
#include <functional>
#include <chrono>
#include <string>

struct _net;
struct _write_factory;

namespace ALittle
{

class ServerSchedule;
class ConnectClient;
typedef std::shared_ptr<ConnectClient> ConnectClientPtr;

typedef std::shared_ptr<asio::ip::tcp::socket> SocketPtr;

typedef unsigned int MESSAGE_SIZE;
typedef int MESSAGE_ID;
typedef int MESSAGE_RPCID;
#define PROTOCOL_HEAD_SIZE 12

class ConnectClient : public std::enable_shared_from_this<ConnectClient>
{
public:
	ConnectClient(_net* c, ServerSchedule* schedule, int id);
	~ConnectClient();

//���Ӳ���/////////////////////////////////////////////////////////////////////////////////
public:
	/* �첽����Ŀ�������
	 * @param route_id: ·��ID��ȫ��Ψһ
	 * @param ip: Ŀ�������IP
	 * @param port: Ŀ��������˿�
	 */
	void Connect(const std::string& ip, unsigned int port);

	// �ж��Ƿ��Ѿ�����
	bool IsConnected();

	// �Ƿ���������
	bool IsConnecting();

	// �ر�����
	void Close();

private:
	std::string m_ip;			// Ŀ���������IP
	unsigned int m_port;		// Ŀ��������˿�

public:
	// ��ȡĿ�������IP�Ͷ˿�
	const std::string& GetIP() const { return m_ip; }
	unsigned int GetPort() const { return m_port; }

private:
	// �첽����
	void HandleAsyncConnect(const asio::error_code& ec);
	bool m_is_connecting;

private:
	// ����Ͽ�����
	void ExecuteDisconnectCallback();
	// ��������ʧ��
	void HandleConnectFailed();
	// �������ӳɹ�
	void HandleConnectSucceed();
	// ����Ͽ�����
	void HandleDisconnected();
	// ������Ϣ��
	void HandleMessage(void* memory, int memory_size);

//��ȡ��Ϣ������/////////////////////////////////////////////////////////////////////////////////
public:
	// ��ȡЭ��
	void NextReadHead();
	void HandleReadHead(const asio::error_code& ec, std::size_t actual_size);
	void HandleReadBody(const asio::error_code& ec, std::size_t actual_size);

private:
	// ����Э��
	void ReadComplete();

public:
	SocketPtr m_socket;					// socket

private:
	// ����Э��ͷ
	char m_message_head[PROTOCOL_HEAD_SIZE];
	// ����Э����
	void* m_memory;

//������Ϣ������/////////////////////////////////////////////////////////////////////////////////
public:
	// ������Ϣ��
	void Send(const _write_factory* message);

private:
	// ������
	void SendPocket(void* memory, int memory_size);
	void NextSend();
	void HandleSend(const asio::error_code& ec, std::size_t bytes_transferred, void* memory);

private:
	struct PocketInfo { int memory_size; void* memory; };
	std::list<PocketInfo> m_pocket_list;  // �����͵����ݰ��б�

	bool m_excuting;	// is in sending

private:
	ServerSchedule* m_schedule;
	_net* m_net;
	int m_id;
};

} // ALittle

#endif // _ALITTLE_CONNECTCLIENT_H_
