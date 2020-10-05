
#ifndef _ALITTLE_SESSIONCONNECTION_H_
#define _ALITTLE_SESSIONCONNECTION_H_

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <functional>

#include "RouteIdDefine.h"

class CarpMessage;

namespace ALittle
{

class ConnectEndpoint;
typedef std::shared_ptr<ConnectEndpoint> ConnectEndpointPtr;
typedef std::weak_ptr<ConnectEndpoint> ConnectEndpointWeakPtr;

class SessionConnection;
typedef std::shared_ptr<SessionConnection> SessionConnectionPtr;
typedef std::weak_ptr<SessionConnection> SessionConnectionWeakPtr;

// �Ự�ڵ���Ϣ
class SessionConnection : public std::enable_shared_from_this<SessionConnection>
{
public:
	enum class Status
	{
		SCS_IDLE,			// ����
		SCS_CONNECTING,		// ��������
		SCS_CONNECTED,		// ���ӳɹ�
	};

public:
	SessionConnection() : m_status(Status::SCS_IDLE), m_connect_index(0), m_connect_key(0) {}
	~SessionConnection();
	friend class RouteSystem;
	
public:
	inline CONNECT_KEY GetConnectKey() const { return m_connect_key; }
	// ��ȡ�����ӵ��route_id
	inline ROUTE_ID GetRouteId() const
	{
		if (m_connect_index == 0)
			return m_vector_route_path.back();
		else
			return m_vector_route_path.front();
	}
    // ��ȡ�����ӵ��route_type
    inline ROUTE_TYPE GetRouteType() const { return RouteIdDefine::CalcRouteType(GetRouteId()); }
	inline ROUTE_NUM GetRouteNum() const { return RouteIdDefine::CalcRouteNum(GetRouteId()); }
	bool IsConnected() const { return m_status == Status::SCS_CONNECTED; }

	bool Send(const CarpMessage& msg);
	
private:
	bool Close();

private:
	Status m_status;									// ��ǰ����״̬
	std::vector<ROUTE_ID> m_vector_route_path;	// ����·��
	std::list<ROUTE_ID> m_list_route_path;		// ����·��
	unsigned short m_connect_index;						// ��ǰ�����������·���еĽڵ��±�

	CONNECT_KEY m_connect_key;					// ����ID

	ConnectEndpointWeakPtr m_pre_endpoint;		// ��һ���ڵ�
	ConnectEndpointWeakPtr m_next_endpoint;		// ��һ���ڵ�

public:
	// �Ự���ӻص�
	// reason, SessionConnectionPtr
	typedef std::function<void(const std::string*, SessionConnectionPtr)> ConnectRouteCallback;
	std::vector<ConnectRouteCallback> callback_list;	// �ص������б�
};

} // ALittle

#endif // _ALITTLE_SESSIONCONNECTION_H_
