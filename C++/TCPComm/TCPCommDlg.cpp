// TCPCommDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "TCPComm.h"
#include "TCPCommDlg.h"
#include "afxdialogex.h"

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTCPCommDlg 对话框



CTCPCommDlg::CTCPCommDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTCPCommDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME1);

	PathFlie = _T("./Setting/Setting.ini");

	Server = new CDevCommVisionTcp;
	Client = new CDevCommVisionTcp;
	m_szServerIP = "";
	ft_old = { 0, 0 };
	ft_new = { 0, 0 };
}

void CTCPCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTN_COMM_STATE, m_btnTCPConnState);
	DDX_Control(pDX, IDC_CMB_DETECT_TYPE, m_cmbDetectType);
	DDX_Control(pDX, IDC_EDT_DEVICE_ID, m_edtServerIP);


	m_fnLoadData();
}

BEGIN_MESSAGE_MAP(CTCPCommDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SAVE, &CTCPCommDlg::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CTCPCommDlg 消息处理程序

BOOL CTCPCommDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	//Server->DevConnect(CDevCommVisionTcp::TCP_S);
	Client->DevConnect(CDevCommVisionTcp::TCP_C);

	//std::thread(&CTCPCommDlg::CommRun, this, Server).detach();
	std::thread(&CTCPCommDlg::CommRun, this, Client).detach();
	//this->HideCaret();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//void CTCPCommDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialogEx::DoDataExchange(pDX);
//
//	DDX_Control(pDX, IDC_BTN_COMM_STATE	,	m_btnTCPConnState	);
//	DDX_Control(pDX, IDC_CMB_DETECT_TYPE,	m_cmbDetectType		);
//}

void CTCPCommDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTCPCommDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTCPCommDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CTCPCommDlg::m_fnLoadData()
{
	TCHAR szTemp[20] = { 0 };
	CString strTemp;
	TCHAR *key;
	key = new TCHAR[14336];
	int	nLength = 0;

	//load Detect Type from config file
	GetPrivateProfileSection(_T("DetectModul"), key, 14336, PathFlie);
	while (*key!=0x00)
	{
		m_cmbDetectType.AddString(key);
		nLength = _tcslen(key);
		key = key + nLength + 1;
	}

	//get Detect Type and show
	GetPrivateProfileString(_T("Setting"), _T("DetectType"), _T("YOLO_A"), szTemp, sizeof(szTemp), PathFlie);
	strTemp.Format(szTemp);
	if (strTemp.IsEmpty())
		m_cmbDetectType.SetWindowText(_T(""));
	else
		m_cmbDetectType.SelectString(-1, szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	GetPrivateProfileString(_T("DeviceInfo"), _T("machineId"), _T(""),szTemp, sizeof(szTemp), PathFlie);
	strTemp.Format(szTemp);
	m_szServerIP = strTemp;
	m_edtServerIP.SetWindowText(strTemp);

	memset(szTemp, 0, sizeof(szTemp));
	GetPrivateProfileString(_T("DeviceInfo"), _T("LogFilePath"), _T(""), szTemp, sizeof(szTemp), PathFlie);
	strTemp.Format(szTemp);
	PathLogFlie = strTemp;

	memset(szTemp, 0, sizeof(szTemp));
	GetPrivateProfileString(_T("DeviceInfo"), _T("ImageFilePath"), _T(""), szTemp, sizeof(szTemp), PathFlie);
	strTemp.Format(szTemp);
	PathImageFlie = strTemp;
	return 0;
}

int CTCPCommDlg::m_fnSaveData()
{
	CString strTemp;

	int nDetectType = m_cmbDetectType.GetCurSel();
	m_cmbDetectType.GetLBText(nDetectType, strTemp);

	WritePrivateProfileString(_T("Setting"), _T("DetectType"), strTemp, PathFlie);
	return 0;
}


void CTCPCommDlg::OnBnClickedBtnSave()
{
	// TODO:  在此添加控件通知处理程序代码
	m_fnSaveData();
}

inline std::string CTCPCommDlg::CStringToString(const CString& cs)
{
	CT2A ascii(cs);
	return std::string(ascii);
}

inline CString CTCPCommDlg::StringToCString(const std::string& str)
{
	CA2W wide(str.c_str());
	return CString(wide);
}

void CTCPCommDlg::CommRun(CDevCommVisionTcp* DevComm)
{
	//Heartbeat thread
	std::thread([=](CDevCommVisionTcp* DevCommTemp) {
		try
		{
			//标志Heartbeat是否还在运行
			m_bHeartbeat = true;
			static int seq = 0;
			std::string rcvBuf;
			while (m_bHeartbeat)
			{
				json jMsg;
				jMsg["type"] = "heartbeat";
				jMsg["machineId"] = CStringToString(m_szServerIP);
				jMsg["seq"] = seq;
				if(DevCommTemp->WaitConnect() == false)
					continue;	//10s连接不上处理
					
				if (!DevCommTemp->SendToVision(jMsg.dump()))
				{
					//DevCommTemp->CloseConnect();
					//DevCommTemp->DevConnect(CDevCommVisionTcp::TCP_S);
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					continue;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				//看后续会有什么处理
				if(DevCommTemp->WaitResponse(rcvBuf, CDevCommVisionTcp::Heartbeat)){}
				if (rcvBuf.empty())
					continue;

				json jTemp = json::parse(rcvBuf);
				if (jTemp["type"] != "heartbeat_ack")
					continue;
				else if (jTemp["type"] == "heartbeat_ack")
				{
					//if (jTemp["machineId"] == CStringToString(m_szServerIP) || jTemp["seq"] == seq)
					DevCommTemp->SendToVision("OK");
				}
				rcvBuf = "";
				seq++;
				std::this_thread::sleep_for(std::chrono::milliseconds(3000));
			}
			m_bHeartbeat = false;
		}
		catch (CMemoryException* e) {}
	}, DevComm).detach();

	//Send and Recv thread
	std::thread([=](CDevCommVisionTcp* DevCommTemp){
		//标志Recv是否还在运行
		m_bRecv = true;
		std::string rcvBuf;
		json jMsg;
		int nlimit = 0;
		int nPending = 0;
		int SendImagelength = 0;
		int ncount = 0;
		while (m_bRecv)
		{
			//会一直访问存图片的文件夹，当发现存在新图片时会给视觉发送信号
			//ProcessAllImagesInFolder(PathImageFlie);
			if (!GetNewImageExist(PathImageFlie))
				continue;
			//处理图片队列
			while(m_imageQueue.size() > 0)
			{
				//查询当前处理状态
				jMsg["type"] = "status_query";
				jMsg["machineId"] = CStringToString(m_szServerIP);
				//查询当前通信状态
				if (DevCommTemp->WaitConnect() == TRUE)
				{
				}
				else
				{
					//10s连接不上处理
					continue;
				}
				for(ncount=0; ncount <3; ncount++) // 重试3次
				{
					if (!DevCommTemp->SendToVision(jMsg.dump()))
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
						continue;
					}
					else
					{
						if (DevCommTemp->WaitResponse(rcvBuf, CDevCommVisionTcp::status_query)) 
						{
							if (rcvBuf.empty())
								continue;
							json jTemp = json::parse(rcvBuf);
							//判断当前Vision设备发送的json消息是否是本机，且未达到处理上限
							if (jTemp["machineId"] == CStringToString(m_szServerIP) && jTemp["pending"] < jTemp["limit"])
							{
								nlimit = jTemp["limit"];
								nPending = jTemp["pending"];
								SendImagelength = nlimit - nPending;
								m_brequesting = true;
								rcvBuf = "";
								break;
							}
						}
						else
							continue;
					}
				}
				if (ncount >= 3)
				{
					//处理
					continue;
				}
				jMsg.clear();
				if (m_brequesting && SendImagelength > 0)
				{
					/*
					{"type":"infer_request",
					"machineId":"MACHINE_01",
					"requestId":"9f2c9b7c2c1241f9a5c9a7c4a2e7d111",
					"uncPath":"\\\\WIN7-PC-01\\\\ImgShare\\\\inbox\\\\CELL123_ST02_DV05_20260118T083004_TYPEA_0001.png",
					"fileName":"CELL123_ST02_DV05_20260118T083004_TYPEA_0001.png",
					"modelKey":"YOLO_A"}
					*/
					//上传图片信息
					jMsg["type"] = "infer_request";
					jMsg["machineId"] = CStringToString(m_szServerIP);
					jMsg["requestId"] = m_imageIDQueue.front();
					jMsg["uncPath"] = m_imageQueue.front();
					jMsg["fileName"] = m_imageNameQueue.front();
					jMsg["modelKey"] = CStringToString(m_szServerIP);
					if (!DevCommTemp->SendToVision(jMsg.dump()))
					{
						/*DevCommTemp->CloseConnect();
						DevCommTemp->DevConnect(CDevCommVisionTcp::TCP_S);*/
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
						continue;
					}
					//if(DevCommTemp->WaitResponse(rcvBuf, CDevCommVisionTcp::infer_request))
					//{ }
					std::string strTempFileIfon[2] = {m_imageIDQueue.front(), m_imageQueue.front()};
					m_imageInfoQueue.push_back(strTempFileIfon);
					m_imageIDQueue.pop();
					m_imageQueue.pop();
					m_imageNameQueue.pop();
				}
				//上传图片信息
				//std::string imagePath = m_imageQueue.front();
				//m_imageQueue.pop();
				//jMsg["type"] = "infer_request";
				//jMsg["machineId"] = CStringToString(m_szServerIP);
				//jMsg["imagePath"] = imagePath;
				//if (!DevCommTemp->SendToVision(jMsg.dump()))
				//{
				//	DevCommTemp->CloseConnect();
				//	DevCommTemp->DevConnect(CDevCommVisionTcp::TCP_S);
				//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				//	continue;
				//}
				//std::this_thread::sleep_for(std::chrono::milliseconds(100));
				//看后续会有什么处理
				//if (DevCommTemp->WaitResponse(rcvBuf, CDevCommVisionTcp::infer_request)) {}
				//if (rcvBuf.empty())
				//	continue;
				//json jTemp = json::parse(rcvBuf);
				//if (jTemp["type"] != "infer_response")
				//	continue;
				//else if (jTemp["type"] == "infer_response")
				//{
				//	//if (jTemp["machineId"] == CStringToString(m_szServerIP))
				//	DevCommTemp->SendToVision("OK");
				//}
				rcvBuf = "";
				jMsg.clear();
				m_brequesting = false;
			}
			//if (DevCommTemp->WaitConnect() == false)
			//	continue;
			//std::this_thread::sleep_for(std::chrono::seconds(10));	//
		}
		m_bRecv = false;
	}, DevComm).detach();

	//接收线程
	std::thread([=](CDevCommVisionTcp* DevCommTemp) {
		m_bRecv = true;
		while (m_bRecv)
		{
			std::string rcvBuf;
			json jMsg;
			int nlimit = 0;
			int nPending = 0;
			int SendImagelength = 0;
			int pos = 0;
			std::vector<std::string*> m_imageInfoTempQueue = m_imageInfoQueue;
			if (DevCommTemp->WaitConnect() == TRUE)
			{
			}
			else
			{
				//10s连接不上处理
				continue;
			}
			if (DevCommTemp->WaitResponse(rcvBuf, CDevCommVisionTcp::infer_request))
			{
				jMsg = json::parse(rcvBuf);
				while (m_imageInfoQueue.size())
				{
					if (jMsg["requestId"] == m_imageInfoQueue.at(pos)[0])
					{
						m_imageInfoQueue.erase(m_imageInfoQueue.begin() + pos);
						DevCommTemp->SendToVision(std::to_string(pos));
						break;
					}
					pos++;
				}
			}
			else
				continue;
		}
	}, DevComm).detach();

}

// 单个图片处理函数
void CTCPCommDlg::ProcessImageFile(const CString& filePath)
{
	//检查文件是否存在
	if (GetFileAttributes(filePath) == INVALID_FILE_ATTRIBUTES)
		return;
	//else
	//{
	//	CreateFileW(filePath,
	//		GENERIC_READ,
	//		FILE_SHARE_READ,
	//		NULL,
	//		OPEN_EXISTING,
	//		FILE_ATTRIBUTE_NORMAL,
	//		NULL);
	//}

	//获取文件信息
	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	if (!GetFileAttributesEx(filePath, GetFileExInfoStandard, &fileInfo))
		return;

	//文件创建时间
	SYSTEMTIME stUTC, stLocal;
	FileTimeToSystemTime(&fileInfo.ftCreationTime, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
	CString strCreateTime;
	strCreateTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
		stLocal.wYear, stLocal.wMonth, stLocal.wDay,
		stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

	//文件修改时间
	FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
	CString strModifyTime;
	strModifyTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
		stLocal.wYear, stLocal.wMonth, stLocal.wDay,
		stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

	//ft_new = fileInfo.ftLastWriteTime;
	//  < 0  -> ft_new <  ft_old
	//  == 0 -> ft_new == ft_old
	//  > 0  -> ft_new >  ft_old
	//假如不删除图片情况下，可以用来记录最新处理的文件时间点
	if(CompareFileTime(&fileInfo.ftLastWriteTime, &ft_old)<=0)
		return; // 已处理过该文件
	if(CompareFileTime(&fileInfo.ftLastWriteTime, &ft_new)>=0)
		ft_new = fileInfo.ftLastWriteTime;
	
	//图片路径
	m_imageQueue.push(CTCPCommDlg::CStringToString(filePath).insert(0,"\\WIN7-PC-01\\"));

	std::string strTempFile = CTCPCommDlg::CStringToString(filePath); 
	//获取文件名
	std::string strTempImageFile = CTCPCommDlg::CStringToString(PathImageFlie + _T("\\"));
	strTempFile.erase(0, strTempFile.find(strTempImageFile) + strTempImageFile.length());

	m_imageNameQueue.push(strTempFile);
	//图片唯一标识 = 文件名 + 时间戳（hour + minute + second）
	m_imageIDQueue.push(strTempFile.erase(strTempFile.find(".png"), strTempFile.length()) + std::to_string(stLocal.wHour) + std::to_string(stLocal.wMinute) + std::to_string(stLocal.wSecond));

	//Log
	CString logStr;
	logStr.Format(_T("文件: %s, 创建: %s, 修改: %s"),
		filePath, strCreateTime, strModifyTime);
	WriteLog(PathLogFlie, logStr);

	//删除文件
	//if (DeleteFile(filePath))
	//	wprintf(L"文件删除成功: %s\n", (LPCTSTR)filePath);
	//else
	//	wprintf(L"文件删除失败: %s\n", (LPCTSTR)filePath);
}

//批量处理文件夹下所有图片
bool CTCPCommDlg::ProcessAllImagesInFolder(const CString& folderPath)
{
	CString searchPath = folderPath + _T("\\*.png"); // 可改成 *.png 等
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(searchPath, &findData);
	int nQueryCount = 0;
	nQueryCount = m_imageQueue.size();

	if (hFind == INVALID_HANDLE_VALUE)
	{
		wprintf(L"没有找到图片文件\n");
		return false;
	}

	do
	{
		CString filePath = folderPath + _T("\\") + findData.cFileName;
		ProcessImageFile(filePath);
	} while (FindNextFile(hFind, &findData));
	if (CompareFileTime(&ft_old, &ft_new) < 0)
		ft_old = ft_new; //更新已处理的最新文件时间点
	ft_new = { 0, 0 };
	FindClose(hFind);
	if(nQueryCount < m_imageQueue.size())
		return true;
	else
		return false;
}

void CTCPCommDlg::WriteLog(const CString& logFilePath, const CString& content)
{
	//打开文件，追加写入
	SYSTEMTIME st;
	GetLocalTime(&st);  // 获取本地时间
	CString strlogFile;
	strlogFile.Format(_T("%s\\%d\\%d\\%d\\process_%d.txt"), logFilePath, st.wYear, st.wMonth, st.wDay, st.wHour);

	std::ofstream logFile;
	CT2A pathA(strlogFile); // CString -> std::string

	CString dir = strlogFile.Left(strlogFile.ReverseFind(_T('\\')));
	CTCPCommDlg::CreateDirectoryRecursive(dir);	//递归创建目录，防止目录不存在导致打开文件失败

	logFile.open(std::string(pathA), std::ios::app);
	if (!logFile.is_open())
		return;

	// CString -> std::string
	CT2A contentA(content);
	logFile << contentA << std::endl;

	logFile.close();
}

bool CTCPCommDlg::CreateDirectoryRecursive(const CString& folder)
{
	if (GetFileAttributes(folder) != INVALID_FILE_ATTRIBUTES)
		return true; // 已存在

	int pos = folder.ReverseFind(_T('\\'));
	if (pos != -1)
	{
		CString parent = folder.Left(pos);
		CreateDirectoryRecursive(parent); //递归创建父目录
	}

	return CreateDirectory(folder, NULL) || GetFileAttributes(folder) != INVALID_FILE_ATTRIBUTES;
}

//遍历文件夹，判断是否存在新图片
bool CTCPCommDlg::GetNewImageExist(const CString& filePath)
{
	return ProcessAllImagesInFolder(PathImageFlie);
}
