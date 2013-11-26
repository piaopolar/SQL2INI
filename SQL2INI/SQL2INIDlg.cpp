
	 // SQL2INIDlg.cpp : ʵ���ļ�
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
//    ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
// ============================================================================
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV ֧��

// ----------------------------------------------------------------------------
//    ʵ��
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
//    CSQL2INIDlg �Ի���
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
//    CSQL2INIDlg ��Ϣ�������
// ============================================================================
BOOL CSQL2INIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С� IDM_ABOUTBOX
	// ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	// ִ�д˲���
	SetIcon(m_hIcon, TRUE);		// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);	// ����Сͼ��

	SetLogEdit(&m_edtLog);
	this->LoadConfig();

	return TRUE;				// ���ǽ��������õ��ؼ������򷵻� TRUE
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
//    �����Ի��������С����ť������Ҫ����Ĵ���
//    �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó��� �⽫�ɿ���Զ���ɡ�
// ============================================================================
void CSQL2INIDlg::OnPaint()
{
	if (IsIconic()) {

		//~~~~~~~~~~~~~~
		CPaintDC dc(this);	// ���ڻ��Ƶ��豸������
		//~~~~~~~~~~~~~~

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM> (dc.GetSafeHdc()), 0);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GetClientRect(&rect);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}
}

// ============================================================================
//    ���û��϶���С������ʱϵͳ���ô˺���ȡ�ù�� ��ʾ��
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

	LogInfoIn("�������");
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
	// TODO: ����ÿؼ��� RICHEDIT �ؼ��������� ���ʹ�֪ͨ��������д
	// CDialogEx::OnInitDialog() ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С� ;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
