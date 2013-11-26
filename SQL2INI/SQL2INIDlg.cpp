
	 // SQL2INIDlg.cpp : 实现文件
#include "stdafx.h"

#include "../BaseCode/BaseFunc.h"
#include "../BaseCode/IniMgr.h"
#include "SQLIniCheckMgr.h"

#include "SQL2INI.h"
#include "SQL2INIDlg.h"

#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
namespace
{

// ============================================================================
// ==============================================================================
CString GetEdtStr(const CEdit &rEdit)
{
	//~~~~~~~~~
	CString rStr;
	//~~~~~~~~~

	rEdit.GetWindowText(rStr);
	return rStr;
}
}
// ============================================================================
//    用于应用程序“关于”菜单项的 CAboutDlg 对话框
// ============================================================================
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV 支持

// ----------------------------------------------------------------------------
//    实现
// ----------------------------------------------------------------------------
protected:
	DECLARE_MESSAGE_MAP()
};

// ============================================================================
// ==============================================================================

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

// ============================================================================
// ==============================================================================
void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ============================================================================
//    CSQL2INIDlg 对话框
// ============================================================================
CSQL2INIDlg::CSQL2INIDlg(CWnd *pParent /* NULL */ ) : CDialogEx(CSQL2INIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// ============================================================================
// ==============================================================================
void CSQL2INIDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ADDR, m_edtAddr);
	DDX_Control(pDX, IDC_EDIT_PORT, m_edtPort);
	DDX_Control(pDX, IDC_EDIT_USER, m_edtUser);
	DDX_Control(pDX, IDC_EDIT_PSW, m_edtPsw);
	DDX_Control(pDX, IDC_EDIT_DB, m_edtDB);
	DDX_Control(pDX, IDC_EDIT_LOG, m_edtLog);
}

BEGIN_MESSAGE_MAP(CSQL2INIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CSQL2INIDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDOK, &CSQL2INIDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_LOG, &CSQL2INIDlg::OnEnChangeEditLog)
END_MESSAGE_MAP()

// ============================================================================
//    CSQL2INIDlg 消息处理程序
// ============================================================================
BOOL CSQL2INIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。 IDM_ABOUTBOX
	// 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CMenu *pSysMenu = GetSystemMenu(FALSE);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (pSysMenu != NULL) {

		//~~~~~~~~~~~~~~~~~
		BOOL bNameValid;
		CString strAboutMenu;
		//~~~~~~~~~~~~~~~~~

		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	// 执行此操作
	SetIcon(m_hIcon, TRUE);		// 设置大图标
	SetIcon(m_hIcon, FALSE);	// 设置小图标

	SetLogEdit(&m_edtLog);
	this->LoadConfig();

	return TRUE;				// 除非将焦点设置到控件，否则返回 TRUE
}

// ============================================================================
// ==============================================================================
void CSQL2INIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {

		//~~~~~~~~~~~~~~~
		CAboutDlg dlgAbout;
		//~~~~~~~~~~~~~~~

		dlgAbout.DoModal();
	} else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ============================================================================
//    如果向对话框添加最小化按钮，则需要下面的代码
//    来绘制该图标。对于使用文档/视图模型的 MFC 应用程序， 这将由框架自动完成。
// ============================================================================
void CSQL2INIDlg::OnPaint()
{
	if (IsIconic()) {

		//~~~~~~~~~~~~~~
		CPaintDC dc(this);	// 用于绘制的设备上下文
		//~~~~~~~~~~~~~~

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM> (dc.GetSafeHdc()), 0);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GetClientRect(&rect);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}
}

// ============================================================================
//    当用户拖动最小化窗口时系统调用此函数取得光标 显示。
// ============================================================================
HCURSOR CSQL2INIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ============================================================================
// ==============================================================================
void CSQL2INIDlg::OnBnClickedBtnConnect()
{
	this->SaveConfig();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CString cstrAddr = GetEdtStr(m_edtAddr);
	CString cstrUser = GetEdtStr(m_edtUser);
	CString cstrPsw = GetEdtStr(m_edtPsw);
	CString cstrDB = GetEdtStr(m_edtDB);
	int nPort = 3306;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	sscanf_s(GetEdtStr(m_edtPort), "%d", &nPort);
	if (!CSQLIniCheckMgr::GetInstance().Connect(cstrAddr, cstrUser, cstrPsw, cstrDB, nPort)) {
		return;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::vector<std::string> vecRuleFiles = GetDirFilePathList("./Rules", ".rule");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	for (std::vector < std::string >::const_iterator it = vecRuleFiles.begin(); it != vecRuleFiles.end(); ++it) {
		CSQLIniCheckMgr::GetInstance().Process(it->c_str());
	}

	LogInfoIn("处理完成");
}

// ============================================================================
// ==============================================================================
void CSQL2INIDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

// ============================================================================
// ==============================================================================
void CSQL2INIDlg::OnEnChangeEditLog()
{
	// TODO: 如果该控件是 RICHEDIT 控件，它将不 发送此通知，除非重写
	// CDialogEx::OnInitDialog() 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。 ;
	// TODO: 在此添加控件通知处理程序代码
}

// ============================================================================
// ==============================================================================
void CSQL2INIDlg::LoadConfig(void)
{
	this->LoadConfig(m_edtAddr);
	this->LoadConfig(m_edtPort);
	this->LoadConfig(m_edtUser);
	this->LoadConfig(m_edtPsw);
	this->LoadConfig(m_edtDB);
}

// ============================================================================
// ==============================================================================
void CSQL2INIDlg::LoadConfig(CEdit &rEdit)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CString cstrSection = this->GetConfigSection(&rEdit);
	CString cstrText;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GetPrivateProfileString(cstrSection, "Text", "", cstrText.GetBuffer(MAX_STRING), MAX_STRING, CONFIG_INI);
	rEdit.SetWindowText(cstrText);
}

// ============================================================================
// ==============================================================================
void CSQL2INIDlg::SaveConfig(void) const
{
	this->SaveConfig(m_edtAddr);
	this->SaveConfig(m_edtPort);
	this->SaveConfig(m_edtUser);
	this->SaveConfig(m_edtPsw);
	this->SaveConfig(m_edtDB);
}

// ============================================================================
// ==============================================================================
void CSQL2INIDlg::SaveConfig(const CEdit &rEdit) const
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CString cstrSection = this->GetConfigSection(&rEdit);
	CString cstrText;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	WritePrivateProfileString(cstrSection, "Text", GetEdtStr(rEdit), CONFIG_INI);
}

// ============================================================================
// ==============================================================================
CString CSQL2INIDlg::GetConfigSection(const CWnd *pWnd) const
{
	//~~~~~~~~~~~~~~~~~~~~~~~
	char szSection[MAX_STRING];
	//~~~~~~~~~~~~~~~~~~~~~~~

	_snprintf_s(szSection, sizeof(szSection), "%d-%d", this->GetDlgCtrlID(), pWnd->GetDlgCtrlID());
	return szSection;
}
