#include "stdafx.h"
#include "DevCommVisionTcp.h"
#pragma warning(disable:4996)

CDevCommVisionTcp::CDevCommVisionTcp()
{
	m_clientSocket = 0;
	m_serverSocket = 0;
	m_bConnection		=	FALSE;

	memset(m_szUID, 0x00, sizeof(m_szUID));
	//add cxh
	m_hClientEvent = 0;
	m_hServerEvent = 0;
	m_bIsRcvData = false;
	m_Port = 0;
	memset(m_ipAddr, 0x00, sizeof(m_ipAddr));
	m_bStopThread = false;

	m_DevType = TCP_NULL;
	//
}

CDevCommVisionTcp::~CDevCommVisionTcp()
{
	int nRes = 0;
	m_bStopThread = true;
	if (m_hClientEvent) WSASetEvent(m_hClientEvent);
	if (m_hServerEvent) WSASetEvent(m_hServerEvent);
	Sleep(100);

	if (m_clientSocket != 0)	nRes = closesocket(m_clientSocket);
	if (m_serverSocket != 0)	nRes = closesocket(m_serverSocket);

	if (nRes)
	{
		nRes = ERROR_FAIL_CLOSE;
	}
	else
	{
		if (m_clientSocket != 0)	m_clientSocket = 0;
		if (m_serverSocket != 0)	m_serverSocket = 0;
	}
	if (m_hClientEvent) WSACloseEvent(m_hClientEvent);
	if (m_hServerEvent) WSACloseEvent(m_hServerEvent);

	m_bConnection = FALSE;
}

//读取配置文件中的信息
void CDevCommVisionTcp::ReadINI(CString& m_ipAddr, unsigned short& m_Port, DevType m_DevType)
{
#ifdef _TEST_
	// 192.168.1.100	5000	TCP_S/TCP_C
	m_ipAddr = "192.168.0.197";
	m_Port = 888;
	m_DevType = TCP_C;	//TCP_S	OK
#else
	// 192.168.1.100	5000	TCP_S/TCP_C
	CString PathFlie = _T("./Setting/SocketCfg.ini");
	TCHAR szIpAddr[20] = { 0 };
	TCHAR szPort[10] = { 0 };
	if (GetFileAttributes(PathFlie) == INVALID_FILE_ATTRIBUTES) {
		//AfxMessageBox(_T("SocketCfg.ini no Exist！"));
		return;
	}

	CString strType[3] = {_T(""),};
	if (m_DevType == TCP_S)
	{
		strType[0] = _T("TCP_S");
		strType[1] = _T("Vision IPAddress");
		strType[2] = _T("Vision IPPort");
	}
	else if (m_DevType == TCP_C)
	{
		strType[0] = _T("TCP_C");
		strType[1] = _T("Main IPAddress");
		strType[2] = _T("Main IPPort");
	}

	GetPrivateProfileString(strType[0], strType[1], _T("127.0.0.1"), szIpAddr, sizeof(szIpAddr), PathFlie);
	GetPrivateProfileString(strType[0], strType[2], _T("5001"), szPort, sizeof(szPort), PathFlie);
	//GetPrivateProfileString(_T("VisionTcp"), _T("DevType"), _T("TCP_C"), szType, sizeof(szType), PathFlie);

	//m_DevType = TCP_NULL;
	//TCHAR_to_char(szIpAddr, m_ipAddr, _countof(szIpAddr));
	m_ipAddr = szIpAddr;
	m_Port = _ttoi(szPort);
	//CString strType = szType;
	//if (strType == _T("TCP_S"))
	//	m_DevType = TCP_S;
	//if (strType == _T("TCP_C"))
	//{
	//	m_DevType = TCP_C;
	//}

#endif

}

//将TCHAR类型转换为char类型
void CDevCommVisionTcp::TCHAR_to_char(TCHAR* tStr, char* szBuf, int bufSize)
{
#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, 0, tStr, -1, szBuf, bufSize, NULL, NULL);
#else
	strncpy(szBuf, tStr, bufSize);
#endif
}
//将CString类型转换为char类型
void CDevCommVisionTcp::CString_To_Char(const CString& str, char* szBuf, int bufSize)
{
#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, 0, str, -1, szBuf, bufSize, NULL, NULL);
#else
	strncpy(szBuf, str, bufSize);
#endif
}

//连接设备
void CDevCommVisionTcp::DevConnect(DevType m_DevType)
{
	if (m_clientSocket || m_serverSocket)
		return;
	this->m_DevType = m_DevType;
	CString strAddr;
	ReadINI(strAddr, m_Port, m_DevType);
	CString_To_Char(strAddr, m_ipAddr, strAddr.GetLength());
	if (m_DevType == TCP_S)
	{
		m_fnConnectServer();
		m_bStopThread = false;
	}
	else if (m_DevType == TCP_C)
	{
		m_fnConnectClient();
		m_bStopThread = false;
	}
	else
	{
		//弹窗或者报警
	}
	//启动通信线程
	std::thread(&CDevCommVisionTcp::SocketEventLoop, this).detach();
}

//等待并获取指定类型的响应消息
bool CDevCommVisionTcp::WaitResponse(std::string& rcvBuf, CDevCommVisionTcp::MessageType mesType)
{
	std::unique_lock<std::mutex> lock(m_WaitrecvMutex);
	MessageType msgType;
	bool waitResult = false;
	waitResult = m_recvCv.wait_for(lock, std::chrono::milliseconds(1000000), [&] {
		if (!m_bConnection)
			return true;
		return std::any_of(
			m_vtRcvBuf.begin(),
			m_vtRcvBuf.end(),
			[&](const std::string& m) {
				if (m.find("infer_done") != std::string::npos)
					msgType = infer_request;
				else if (m.find("status_resp") != std::string::npos)
					msgType = status_query;
				else if(m.find("heartbeat_ack") != std::string::npos)
					msgType = Heartbeat;
				return msgType == mesType;
			});
		});
	if(!waitResult)
		return false;
	if (!m_bConnection)
		return false;
	for (auto it = m_vtRcvBuf.begin(); it != m_vtRcvBuf.end(); ++it)
	{
		if (mesType == infer_request && it->find("infer_done") != std::string::npos)
		{
			rcvBuf = *it;
			m_vtRcvBuf.erase(it);
			return true;
		}
		else if (mesType == status_query && it->find("status_resp") != std::string::npos)
		{
			rcvBuf = *it;
			m_vtRcvBuf.erase(it);
			return true;
		}
		else if (mesType == Heartbeat && it->find("heartbeat_ack") != std::string::npos)
		{
			rcvBuf = *it;
			m_vtRcvBuf.erase(it);
			return true;
		}
	}
	return false;
}

//等待连接成功
bool CDevCommVisionTcp::WaitConnect()
{
	std::unique_lock<std::mutex> lock(m_WaitrecvMutex);
	MessageType msgType;
	bool waitResult = m_WaitConnect.wait_for(lock, std::chrono::milliseconds(10000), [&] {
		return IsAlive();
		});
	if(waitResult)
		return true;
	else
		return false;
}

//客户端连接函数
int	CDevCommVisionTcp::m_fnConnectClient()
{
	int nRes = 0;

	DWORD dwOption = 0;
	char szConnectMesg[256];
	char szMesg[256];

	memset(szConnectMesg, 0x00, sizeof(szConnectMesg));
	memset(szMesg, 0x00, sizeof(szMesg));

	// 1. 扩档快 家南 檬扁拳.
	WORD version = MAKEWORD(2, 2);
	WSADATA wsa;

	if (WSAStartup(version, &wsa) != 0)
	{
		return ERROR_FAIL_STARTUP;
	}

	// 2. 家南 积己.
	m_clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_clientSocket == INVALID_SOCKET)
	{
		return ERROR_FAIL_SOCKET;
	}

	int  nBufSize = MAX_RCV_BUF;
	nRes = setsockopt(m_clientSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nBufSize, sizeof(nBufSize));
	if (SOCKET_ERROR == nRes)
	{
		return ERROR_SET_OPTION;
	}

	// 3. 扩档快 皋矫瘤 殿废.
	//WSAAsyncSelect(m_clientSocket, m_hWindow, WM_SOCKET, FD_CONNECT|FD_CLOSE|FD_READ|FD_WRITE);
	//WSAEventSelect(m_clientSocket, m_hWindow, FD_CONNECT | FD_CLOSE | FD_READ | FD_WRITE);

	// 4. Connect.
	sockaddr_in stAddress;
	memset(&stAddress, 0x00, sizeof(stAddress));

	stAddress.sin_family = AF_INET;
	inet_pton(AF_INET, m_ipAddr, &stAddress.sin_addr.s_addr);
	//stAddress.sin_addr.s_addr	= inet_addr(m_ipAddr); 
	stAddress.sin_port = ::htons(m_Port);

	// 等待事件触发
	m_hClientEvent = WSACreateEvent();
	WSAEventSelect(m_clientSocket, m_hClientEvent, FD_CONNECT | FD_CLOSE | FD_READ);

	if (SOCKET_ERROR == connect(m_clientSocket, (SOCKADDR*)&stAddress, sizeof(stAddress)))
	{
		int nLastError = WSAGetLastError();

		if (nLastError != WSAEWOULDBLOCK)
		{
			return ERROR_FAIL_CONNET;
		}
	}

	DWORD dw = WSAWaitForMultipleEvents(1, &m_hClientEvent, FALSE, 2000, FALSE); // 2秒
	if (dw == WSA_WAIT_TIMEOUT)
	{
		closesocket(m_clientSocket);
		m_clientSocket = 0;
		m_hClientEvent = NULL;
	}
	else if (dw == WSA_WAIT_FAILED)
	{
		m_clientSocket = 0;
		m_hClientEvent = NULL;
	}

	// 成功触发事件，检查是否是连接成功
	WSANETWORKEVENTS netEvents;
	WSAEnumNetworkEvents(m_clientSocket, m_hClientEvent, &netEvents);
	if (netEvents.lNetworkEvents & FD_CONNECT)
	{
		if (netEvents.iErrorCode[FD_CONNECT_BIT] == 0)
		{
			m_bConnection = TRUE;
			m_WaitConnect.notify_all();
			return 0; // OK
		}
		else
		{
			closesocket(m_clientSocket);
			m_clientSocket = 0;
			m_hClientEvent = NULL;
		}
	}
	return -1005; // 没触发连接事件
}

//服务器连接函数
int	CDevCommVisionTcp::m_fnConnectServer()
{
	int nRes = 0;

	// 1. 扩加 檬扁拳.
	WORD version = MAKEWORD(2, 2);
	WSADATA wsa;

	if (WSAStartup(version, &wsa) != 0)
	{
		return ERROR_FAIL_STARTUP;
	}

	// 2. 家南积己
	m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_serverSocket == INVALID_SOCKET)
	{
		return ERROR_FAIL_SOCKET;
	}

	// 2-2. 地址复用.
	BOOL bSet = TRUE;
	setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&bSet, sizeof(bSet));
	// 3. Select
	//WSAAsyncSelect(m_serverSocket, m_hWindow, WM_SOCKET, FD_CLOSE|FD_ACCEPT);
	//WSAAsyncSelect(m_serverSocket, m_hWindow, WM_SOCKET, FD_ACCEPT);
	//WSAEventSelect(m_serverSocket, m_hWindow, FD_ACCEPT);

	// 4. bind
	sockaddr_in stAddress;
	memset(&stAddress, 0x00, sizeof(stAddress));

	stAddress.sin_family = AF_INET;
	inet_pton(AF_INET, m_ipAddr, &stAddress.sin_addr.s_addr);
	stAddress.sin_port = ::htons(m_Port);
	nRes = ::bind(m_serverSocket, (SOCKADDR*)&stAddress, sizeof(stAddress));
	if (nRes == SOCKET_ERROR)
	{
		return ERROR_FAIL_BIND;
	}

	// 5. listen
	nRes = ::listen(m_serverSocket, SOMAXCONN);
	if (nRes == SOCKET_ERROR)
	{
		return ERROR_FAIL_LISTEN;
	}
	//std::thread(&CWinSocket::m_fnReceive, this).detach();  // OK
	m_hServerEvent = WSACreateEvent();
	if (m_hServerEvent == WSA_INVALID_EVENT)
		return -1002;
	WSAEventSelect(m_serverSocket, m_hServerEvent, FD_ACCEPT /*| FD_READ | FD_WRITE | FD_CLOSE*/);	//Client Connect	read	send	Client close connect
	m_fnAcceptWithTimeout(5000);
	return nRes;
}

//检测连接是否存活
BOOL CDevCommVisionTcp::IsAlive()
{
	/*if (!m_bConnection)
	return FALSE;*/

	char buf;
	int res = send(m_clientSocket, &buf, 0, 0);
	if (res == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err == WSAENOTCONN || err == WSAECONNRESET || err == WSAENETDOWN)
		{
			m_bConnection = FALSE;
		}
		return FALSE;
	}
	return TRUE;
}

//Socket事件循环
void CDevCommVisionTcp::SocketEventLoop()
{
	WSAEVENT hEvents[2] = { m_hServerEvent, m_hClientEvent };
	while (!m_bStopThread)
	{
		if (m_DevType == TCP_S)
		{
			if (m_hServerEvent == NULL && m_hClientEvent == NULL)
				continue;
			if (m_hServerEvent != NULL && m_hClientEvent == NULL)
			{
				DWORD dwIndex = WSAWaitForMultipleEvents(1, &m_hServerEvent, FALSE, WSA_INFINITE, FALSE);
				int iEventIndex = dwIndex - WSA_WAIT_EVENT_0;
				if (iEventIndex >= 0 && iEventIndex < 2)
				{
					WSANETWORKEVENTS netEvents;
					SOCKET sock = m_serverSocket;
					WSAEVENT hEvent = m_hServerEvent;

					if (WSAEnumNetworkEvents(sock, hEvent, &netEvents) == SOCKET_ERROR)
						continue;

					if (netEvents.lNetworkEvents & FD_ACCEPT)
					{
						//重连
						SOCKADDR_IN caddr;
						int len = sizeof(caddr);
						m_clientSocket = accept(m_serverSocket, (SOCKADDR*)&caddr, &len);

						if (m_clientSocket != INVALID_SOCKET)
						{
							// 为客户端 socket 创建事件并绑定监听
							m_hClientEvent = WSACreateEvent();
							WSAEventSelect(m_clientSocket, m_hClientEvent, FD_READ | FD_WRITE | FD_CLOSE);

							m_bConnection = TRUE;
						}
						else
						{
							// accept() 失败
						}
						hEvents[1] = m_hClientEvent;
					}

				}
				continue;
			}

			//hEvent = (m_DevType == 1) ? m_hServerEvent : m_hClientEvent;
			DWORD dwIndex = WSAWaitForMultipleEvents(2, hEvents, FALSE, WSA_INFINITE, FALSE);
			int iEventIndex = dwIndex - WSA_WAIT_EVENT_0;

			if (iEventIndex >= 0 && iEventIndex < 2)
			{
				WSANETWORKEVENTS netEvents;
				SOCKET sock = (iEventIndex == 0) ? m_serverSocket : m_clientSocket;
				WSAEVENT hEvent = hEvents[iEventIndex];

				if (WSAEnumNetworkEvents(sock, hEvent, &netEvents) == SOCKET_ERROR)
					continue;

				if (netEvents.lNetworkEvents & FD_ACCEPT)
				{
					m_fnAcceptWithTimeout(5000);	//5000ms等待时间
					//m_fnAccept();
				}
				if (netEvents.lNetworkEvents & FD_CONNECT)
				{
					if (netEvents.iErrorCode[FD_CONNECT_BIT] == 0)
					{
						m_bConnection = TRUE;
						m_WaitConnect.notify_all();
					}
				}
				if (netEvents.lNetworkEvents & FD_CLOSE)
				{
					m_bConnection = FALSE;
					m_WaitConnect.notify_all();
					m_recvCv.notify_all();
					closesocket(sock);

					if (sock == m_clientSocket)
					{
						m_clientSocket = 0;
						if (m_hClientEvent != NULL)
						{
							WSACloseEvent(m_hClientEvent);
							m_hClientEvent = NULL;
						}
					}
					if (sock == m_serverSocket)
					{
						m_serverSocket = 0;
						if (m_hServerEvent != NULL)
						{
							WSACloseEvent(m_hServerEvent);
							m_hServerEvent = NULL;
						}
					}
				}
				if (netEvents.lNetworkEvents & FD_READ)
				{
					char buf[2048] = { 0 };
					int nRecv = recv((m_DevType == 1) ? m_clientSocket : m_serverSocket, buf, sizeof(buf), 0);
					if (nRecv > 0)
					{
						std::string msg(buf, nRecv);
						{
							std::lock_guard<std::mutex> lock(m_recvMutex);
							m_vtRcvBuf.push_back(msg);
							m_bIsRcvData = true;
							m_recvCv.notify_all();
						}
					}
					//CStringA strA(buf);  
					//MessageBoxA(NULL, strA, "接收", MB_OK | MB_TOPMOST);
				}
			}
		}
		else if (m_DevType == TCP_C)
		{
			if (m_hClientEvent != NULL)
			{
				//hEvent = (m_DevType == 1) ? m_hServerEvent : m_hClientEvent;
				//DWORD dwIndex = WSAWaitForMultipleEvents(1, &m_hClientEvent, FALSE, WSA_INFINITE, FALSE);
				//int iEventIndex = dwIndex - WSA_WAIT_EVENT_0;
				WSANETWORKEVENTS netEvents;
				SOCKET sock = m_clientSocket;
				WSAEVENT hEvent = m_hClientEvent;

				if (WSAEnumNetworkEvents(sock, hEvent, &netEvents) == SOCKET_ERROR)
					continue;

				//if (netEvents.lNetworkEvents & FD_ACCEPT)
				//{
				//	m_fnAcceptWithTimeout(5000);	//5000ms等待时间
				//	//m_fnAccept();
				//}
				if (netEvents.lNetworkEvents & FD_CONNECT)
				{
					if (netEvents.iErrorCode[FD_CONNECT_BIT] == 0)
					{
						m_bConnection = TRUE;
						m_WaitConnect.notify_all();
					}
				}
				if (netEvents.lNetworkEvents & FD_CLOSE)
				{
					m_bConnection = FALSE;
					closesocket(sock);
					m_WaitConnect.notify_all();
					m_recvCv.notify_all();
					m_clientSocket = 0;
					if (m_hClientEvent != NULL)
					{
						WSACloseEvent(m_hClientEvent);
						m_hClientEvent = NULL;
					}
					//if (sock == m_serverSocket)
					//{
					//	m_serverSocket = 0;
					//	m_hServerEvent = NULL;
					//}
				}
				if (netEvents.lNetworkEvents & FD_READ)
				{
					char buf[2048] = { 0 };
					int nRecv = recv(m_clientSocket, buf, sizeof(buf), 0);
					std::string msg(buf, nRecv);
					{
						std::lock_guard<std::mutex> lock(m_recvMutex);
						m_vtRcvBuf.push_back(msg);
						m_bIsRcvData = true;
						m_recvCv.notify_all();
					}
					//CStringA strA(buf); 
					//MessageBoxA(NULL, strA, "接收", MB_OK | MB_TOPMOST);


					//std::vector<char> buf(4096); // 动态缓冲区
					//int nRecv = recv(m_clientSocket, buf.data(), buf.size(), 0);
					//if (nRecv > 0)
					//{
					//	m_szRcvBuf = CString(buf.data(), nRecv);
					//	m_bIsRcvData = true;
					//}
					//else if (nRecv == 0)
					//{
					//	// 连接已正常关闭
					//}
					//else
					//{
					//}
				}
			}
			else
			{
				DWORD now = GetTickCount();
				if (now - lastAttemptTime >= 5000) // 每 2 秒尝试一次连接
				{
					m_fnConnectClient();
					lastAttemptTime = now;
					//OutputDebugString(_T("尝试重连...\n"));
				}
			}

		}
	}
}

//带超时的 accept 函数
int CDevCommVisionTcp::m_fnAcceptWithTimeout(int timeout_ms)
{
	if (m_serverSocket == 0 || m_hServerEvent == NULL)
		return -1;

	DWORD dwWait = WSAWaitForMultipleEvents(1, &m_hServerEvent, FALSE, timeout_ms, FALSE);
	if (dwWait == WSA_WAIT_TIMEOUT)
	{
		return -2; // 超时
	}
	else if (dwWait == WSA_WAIT_FAILED)
	{
		return -3; // 等待失败
	}

	// 触发 FD_ACCEPT，执行 accept()
	WSANETWORKEVENTS netEvents;
	WSAEnumNetworkEvents(m_serverSocket, m_hServerEvent, &netEvents);

	if (netEvents.lNetworkEvents & FD_ACCEPT)
	{
		if (netEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
		{
			SOCKADDR_IN caddr;
			int len = sizeof(caddr);
			m_clientSocket = accept(m_serverSocket, (SOCKADDR*)&caddr, &len);

			if (m_clientSocket != INVALID_SOCKET)
			{
				// 为客户端 socket 创建事件并绑定监听
				m_hClientEvent = WSACreateEvent();
				WSAEventSelect(m_clientSocket, m_hClientEvent, FD_READ | FD_WRITE | FD_CLOSE);

				m_bConnection = TRUE;
				return 0; // 成功
			}
			else
			{
				return -4; // accept() 失败
			}
		}
		else
		{
			return -5; // FD_ACCEPT 返回错误
		}
	}

	return -6; // 未触发 FD_ACCEPT
}

//关闭连接
void CDevCommVisionTcp::CloseConnect()
{
	int nRes = 0;
	if (m_clientSocket != 0)	nRes = closesocket(m_clientSocket);
	if (m_serverSocket != 0)	nRes = closesocket(m_serverSocket);

	if (nRes)
	{
		nRes = ERROR_FAIL_CLOSE;
	}
	else
	{
		if (m_clientSocket != 0)	m_clientSocket = 0;
		if (m_serverSocket != 0)	m_serverSocket = 0;
	}
	if (m_hClientEvent)
	{
		WSACloseEvent(m_hClientEvent);
		m_hClientEvent = NULL;
	}
	if (m_hServerEvent)
	{
		WSACloseEvent(m_hServerEvent);
		m_hServerEvent = NULL;
	}

	m_bConnection = FALSE;
	m_bStopThread = true;
}

//获取通信结果
bool CDevCommVisionTcp::GetVisionResult(std::string& szBuf, MessageType mesType)
{
	std::unique_lock<std::mutex> lock(m_getRecvMutex);
	if(m_vtRcvBuf.empty())
		return false;
	std::string strTemp = m_vtRcvBuf.front();
	switch (mesType)
	{
	case CDevCommVisionTcp::Heartbeat:
		if (!strTemp.compare("heartbeat_ack"))
			return false;
		break;
	case CDevCommVisionTcp::status_query:
		if(!strTemp.compare("status_resp"))
			return false;
		break;
	case CDevCommVisionTcp::infer_request:
		if (!strTemp.compare("infer_done"))
			return false;
		break;
	default:
		break;
	}
	m_vtRcvBuf.erase(m_vtRcvBuf.begin());
	szBuf = strTemp;
	if (m_vtRcvBuf.empty())
		m_bIsRcvData = false;
	return true;
}

//发送数据
BOOL CDevCommVisionTcp::SendToVision(std::string str)
{
	std::unique_lock<std::mutex> lock(m_sendMutex);
	Sleep(100);
	{
		int nSentlen = 0;
		int nTotalSentLen = 0;
		int nBreakCount = 0;
		int nLoopCount = 0;
		char* szSndBuf = new char[1024] {'\0'};
		char* temp = new char[str.length() + 1];
		strcpy(temp, str.c_str());
		szSndBuf = temp;
		int nBufSize = strlen(szSndBuf);
		if (nBufSize == 0 || m_clientSocket == 0)
			return FALSE;
		while (nTotalSentLen < nBufSize)
		{
			nSentlen = send(m_clientSocket, szSndBuf + nTotalSentLen, nBufSize - nTotalSentLen, 0);
			if (SOCKET_ERROR == nSentlen)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK && nBreakCount++ < 2)
				{
					Sleep(10);
					continue;
				}
				else
				{
					//报错 + 日志	发送失败
					//SEQ.ALARM_OCCURRED(E_WH_ALARM_SEND_VISION_FAILD);
					return FALSE;
				}
			}
			else
			{
				nTotalSentLen += nSentlen;
				if (nLoopCount++ > 5) return FALSE; // 防止异常阻塞
			}

			Sleep(10);
		}

		delete[] szSndBuf;
		return TRUE;
	}
}