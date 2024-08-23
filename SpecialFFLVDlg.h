/*
 *
 * FLV ��ʽ������
 * FLV Format Analysis
 *
 * ������ Lei Xiaohua
 * leixiaohua1020@126.com
 * �й���ý��ѧ/���ֵ��Ӽ���
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * FLV��װ��ʽ��������
 * FLV Container Format Analysis Tools
 *
 *
 */


#pragma once
#include "resource.h"
#include "stdafx.h"
#include "flvparse.h"

 // CSpecialFFLVDlg �Ի���
class CSpecialFFLVDlg : public CDialogEx
{
	// ����
public:
	CSpecialFFLVDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SPECIAL_F_FLV };

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
	CMFCEditBrowseCtrl m_fflvinputurl;
	CMFCPropertyGridCtrl m_fflvbasicinfo;
	afx_msg void OnBnClickedFFlvInputurlOpen();
	char fileurl[MAX_URL_LENGTH];
	CListCtrl m_fflvtaglist;
	CMFCPropertyGridProperty* Headergroup;
	CMFCPropertyGridProperty* TagData_fbgroup;
	CMFCPropertyGridProperty* ScriptData_fbgroup;
	int AppendTLInfo(int type, int datasize, int timestamp, int streamid, int keyframe, const unsigned char* headbyte, const char* nal_unit_type, const char* sps, const char* pps);
	int AppendBInfo(CString dst_group, CString property_name, CString value, CString remark);
	int ShowBInfo();
	int tl_index;
	int ParseTagData_fb(int type, char data_f_b);
	int ParseScriptData(const unsigned char* meta, int meta_size);
	int ParseScriptType(const unsigned char* meta, const char* name);
	int ParseScriptArrayData(const unsigned char* meta);
	void SystemClear();
	//-----------------
	void OnCustomdrawMyList(NMHDR* pNMHDR, LRESULT* pResult);
	CButton m_fflvtaglistmaxnum;
	CButton m_fflvtaglist_onlykey;
	CButton m_fflvtaglist_onlyvideo;

	afx_msg void OnBnClickedFFlvAbout();
	//-------
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//ר�����ڼ����ַ���
	CString text;
	const int maxheadbytelen = 70;
	int headbytelen;
};
