#pragma once
#include <atlstr.h>
#include <thread>
#include <WS2tcpip.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <mutex>
#include <algorithm>
#include <deque>

#define ERROR_FAIL_STARTUP			-5001
#define ERROR_FAIL_SOCKET			-5002
#define ERROR_FAIL_SET_OPT			-5003
#define ERROR_FAIL_CONNET			-5004
#define ERROR_FAIL_CREATE_THREAD	-5005
#define ERROR_FAIL_CLOSE			-5006
#define ERROR_FAIL_BIND				-5007
#define ERROR_FAIL_LISTEN			-5008
#define ERROR_FAIL_ACCEPT			-5009
#define ERROR_SET_OPTION			-5010

#define WM_SOCKET					 WM_USER + 102
#define MAX_RCV_BUF					 8192

#pragma comment(lib, "Ws2_32.lib")

class CDevCommVisionTcp
{
public:
	CDevCommVisionTcp();
	virtual ~CDevCommVisionTcp();
	enum DevType{
		TCP_S = 1,
		TCP_C,
		TCP_NULL
	};

	enum VisionState {
		Repeated_alarm = 0,
		Minor_alarm = 1,
		No_alarm
	};

	enum MessageType {
		Heartbeat = 0,
		status_query,
		infer_request
	};
private:
	char m_ipAddr[20];
	unsigned short m_Port;
	CString m_szRcvBuf;
	DWORD lastAttemptTime = 0;
	DWORD now;

	std::mutex m_sendMutex;
	std::mutex m_recvMutex;
	std::mutex m_getRecvMutex;
	std::mutex m_WaitrecvMutex;
	std::condition_variable m_recvCv;
	std::condition_variable m_WaitConnect;
public:
	bool m_bStopThread;
	bool m_bIsRcvData;
	std::deque<std::string> m_vtRcvBuf;
	DevType m_DevType;
	BOOL	IsAlive();
	void	CloseConnect();
	BOOL	SendToVision(std::string str);
	bool	GetVisionResult(std::string& szBuf,MessageType mesType);
	void	DevConnect(DevType m_DevType);
	bool	WaitResponse(std::string& rcvBuf, CDevCommVisionTcp::MessageType mesType);
	bool	WaitConnect();
protected:
	WSAEVENT m_hClientEvent;   // 客户端事件句柄
	WSAEVENT m_hServerEvent;   // 服务器监听事件句柄

	bool		m_bConnection;
	char		m_szUID[20];
	SOCKET		m_serverSocket;
	SOCKET		m_clientSocket;

	//HWND		m_hWindow;
private:
	void	SocketEventLoop();
	int		m_fnConnectClient();
	int		m_fnConnectServer();
	int		m_fnAcceptWithTimeout(int timeout_ms);
	void	ReadINI(CString& m_ipAddr, unsigned short& m_Port, DevType m_DevType);
	void	TCHAR_to_char(TCHAR* tStr, char* szBuf, int bufSize);
	void	CString_To_Char(const CString& str, char* szBuf, int bufSize);
};