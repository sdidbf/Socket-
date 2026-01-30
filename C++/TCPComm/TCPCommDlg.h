
// TCPCommDlg.h : 头文件
//

#pragma once

#include "DevCommVisionTcp.h"
#include <string>
#include <fstream>
#include <queue>
#include <vector>


// CTCPCommDlg 对话框
class CTCPCommDlg : public CDialogEx
{
// 构造
public:
	CTCPCommDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TCPCOMM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//afx_msg	void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:
	CButton m_btnTCPConnState;
	CComboBox m_cmbDetectType;
	CEdit m_edtServerIP;

	int m_fnLoadData();
	int m_fnSaveData();

	afx_msg void OnBnClickedBtnSave();

	CString PathFlie;
	CString m_szServerIP;

	CDevCommVisionTcp* Server;
	CDevCommVisionTcp* Client;

private:
	bool stopThread = false;
	std::string CStringToString(const CString& cs);
	CString StringToCString(const std::string& str);

	void CommRun(CDevCommVisionTcp* DevComm);

	bool m_bHeartbeat = true;
	bool m_bRecv = true;
	bool m_bSend = true;

	CString PathLogFlie;
	CString PathImageFlie;

	FILETIME ft_old;
	FILETIME ft_new;

	void ProcessImageFile(const CString& filePath);
	bool ProcessAllImagesInFolder(const CString& folderPath);
	void WriteLog(const CString& logFilePath, const CString& content);
	bool CreateDirectoryRecursive(const CString& folder);

	bool GetNewImageExist(const CString& filePath);

	bool m_bNewImage = true;
	bool m_brequesting = false;

	std::queue<std::string> m_imageQueue;
	std::queue<std::string> m_imageIDQueue;
	std::queue<std::string> m_imageNameQueue;
	std::vector<std::string*> m_imageInfoQueue;
};
