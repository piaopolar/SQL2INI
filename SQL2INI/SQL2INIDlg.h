
// SQL2INIDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSQL2INIDlg �Ի���
class CSQL2INIDlg : public CDialogEx
{
// ����
public:
	CSQL2INIDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SQL2INI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtAddr;
	CEdit m_edtPort;
	CEdit m_edtUser;
	CEdit m_edtPsw;
	CEdit m_edtDB;
	CEdit m_edtLog;
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEditLog();
private:
	void LoadConfig(void);
	void LoadConfig(CEdit& rEdit);

	CString GetConfigSection(const CWnd *pWnd) const;
	void SaveConfig(void) const;
	void SaveConfig(const CEdit& rEdit) const;
};
