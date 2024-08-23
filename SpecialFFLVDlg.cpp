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
#include <stdint.h>
#include "stdafx.h"
#include "SpecialFFLV.h"
#include "SpecialFFLVDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


 // ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CSpecialFFLVDlg �Ի���




CSpecialFFLVDlg::CSpecialFFLVDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpecialFFLVDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpecialFFLVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_F_FLV_INPUTURL, m_fflvinputurl);
	DDX_Control(pDX, IDC_F_FLV_BASICINFO, m_fflvbasicinfo);
	DDX_Control(pDX, IDC_F_FLV_TAGLIST, m_fflvtaglist);
	DDX_Control(pDX, IDC_F_FLV_TAGLIST_MAXNUM, m_fflvtaglistmaxnum);
	DDX_Control(pDX, IDC_F_FLV_TAGLIST_ONLYKEY, m_fflvtaglist_onlykey);
	DDX_Control(pDX, IDC_F_FLV_TAGLIST_ONLYVIDEO, m_fflvtaglist_onlyvideo);
}

BEGIN_MESSAGE_MAP(CSpecialFFLVDlg, CDialogEx)
	ON_BN_CLICKED(IDC_F_FLV_INPUTURL_OPEN, &CSpecialFFLVDlg::OnBnClickedFFlvInputurlOpen)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_F_FLV_TAGLIST, OnCustomdrawMyList)
	ON_BN_CLICKED(IDC_F_FLV_ABOUT, &CSpecialFFLVDlg::OnBnClickedFFlvAbout)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CSpecialFFLVDlg ��Ϣ�������

BOOL CSpecialFFLVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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
	//����ѡ���б���ߣ���ͷ����������
	DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_ONECLICKACTIVATE;
	//�����񣻵���ѡ�񣻸�����ʾѡ����
	//��Ƶ


	m_fflvtaglist.ModifyStyle(0, LVS_SINGLESEL | LVS_REPORT | LVS_SHOWSELALWAYS);
	m_fflvtaglist.SetExtendedStyle(dwExStyle);

	text.LoadString(IDS_LIST_TITLE_NUM);
	m_fflvtaglist.InsertColumn(0, text, LVCFMT_CENTER, 50, 0);
	text.LoadString(IDS_LIST_TITLE_TYPE);
	m_fflvtaglist.InsertColumn(1, text, LVCFMT_CENTER, 85, 0);
	text.LoadString(IDS_LIST_TITLE_DATASIZE);
	m_fflvtaglist.InsertColumn(2, text, LVCFMT_CENTER, 95, 0);
	text.LoadString(IDS_LIST_TITLE_TIMESTAMP);
	m_fflvtaglist.InsertColumn(3, text, LVCFMT_CENTER, 100, 0);
	text.LoadString(IDS_LIST_TITLE_STREAMID);
	m_fflvtaglist.InsertColumn(4, text, LVCFMT_CENTER, 95, 0);
	text.LoadString(IDS_LIST_DATA_TYPE_VIDEO_KEY);
	m_fflvtaglist.InsertColumn(5, text, LVCFMT_CENTER, 50, 0);
	//text.LoadString("asdf");
	m_fflvtaglist.InsertColumn(6, "nal type", LVCFMT_RIGHT, 100, 0);
	m_fflvtaglist.InsertColumn(7, "sps", LVCFMT_LEFT, 100, 0);
	m_fflvtaglist.InsertColumn(8, "pps/len", LVCFMT_RIGHT, 50, 0);
	m_fflvtaglist.InsertColumn(9, "headbyte", LVCFMT_LEFT, 200, 0);

	//Propertylist��ʼ��------------------------
	m_fflvbasicinfo.EnableHeaderCtrl(FALSE);
	m_fflvbasicinfo.EnableDescriptionArea();
	m_fflvbasicinfo.SetVSDotNetLook();
	m_fflvbasicinfo.MarkModifiedProperties();
	//�ѵ�һ�е�����һЩ-----------------------
	HDITEM item;
	item.cxy = 120;
	item.mask = HDI_WIDTH;
	m_fflvbasicinfo.GetHeaderCtrl().SetItem(0, &item);

	//----
	Headergroup = new CMFCPropertyGridProperty(_T("File Header"));
	TagData_fbgroup = new CMFCPropertyGridProperty(_T("TagData Header"));
	ScriptData_fbgroup = new CMFCPropertyGridProperty(_T("ScriptData"));
	//-----------
	tl_index = 0;
	m_fflvtaglistmaxnum.SetCheck(1);
	//-------------
	m_fflvinputurl.EnableFileBrowseButton(
		NULL,
		_T("FLV Files (*.flv)|*.flv|All Files (*.*)|*.*||")
	);
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//m_fflvlang.InsertString(0,_T("Chinese"));
	//m_fflvlang.InsertString(1,_T("English"));

	_CrtSetBreakAlloc(822);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSpecialFFLVDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSpecialFFLVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	//���Ի���Ӧ����ӵ�OnInitDialog�У�����û��Ч��

}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSpecialFFLVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSpecialFFLVDlg::OnBnClickedFFlvInputurlOpen()
{
	SystemClear();
	CString strFilePath;
	m_fflvinputurl.GetWindowText(strFilePath);

#ifdef _UNICODE
	USES_CONVERSION;
	strcpy(fileurl, W2A(strFilePath));
#else
	strcpy(fileurl, strFilePath);
#endif

	int argc = 4;
	char** argv = (char**)malloc(MAX_URL_LENGTH);
	argv[0] = (char*)malloc(MAX_URL_LENGTH);
	argv[1] = (char*)malloc(MAX_URL_LENGTH);
	argv[2] = (char*)malloc(MAX_URL_LENGTH);
	argv[3] = (char*)malloc(MAX_URL_LENGTH);
	strcpy(argv[0], "dummy");
	strcpy(argv[1], fileurl);
	strcpy(argv[2], "dummy.dat");
	strcpy(argv[3], "-s");
	flvparse(this, argc, argv);
	free(argv[0]);
	free(argv[1]);
	free(argv[2]);
	free(argv[3]);
	free(argv);
	ShowBInfo();
}
//���һ����¼
//ÿ���ֶεĺ��壺���ͣ����ݴ�С��ʱ�����streamid��data�ĵ�һ���ֽ�
int CSpecialFFLVDlg::AppendTLInfo(int type, int datasize, int timestamp, int streamid, int keyframe, const unsigned char* headbyte, const char* nal_unit_type, const char* sps, const char* pps) {
	//���ѡ���ˡ�������5000�������ж��Ƿ񳬹�5000��
	if (m_fflvtaglistmaxnum.GetCheck() == 1 && tl_index > 5000) {
		return 0;
	}
	CString temp_index, temp_type, temp_datasize, temp_timestamp, temp_streamid, temp_keyframe,
		temp_headbyte, temp_nal_unit_type, temp_sps, temp_pps;
	int nIndex = 0;
	switch (type) {
	case 8:
		text.LoadString(IDS_LIST_DATA_TYPE_AUDIO); break;
	case 9:
		text.LoadString(IDS_LIST_DATA_TYPE_VIDEO); break;
	case 18:
		text.LoadString(IDS_LIST_DATA_TYPE_SCRIPT); break;
	default:
		text.LoadString(IDS_UNKNOWN); break;
	}
	if ((m_fflvtaglist_onlykey.GetCheck() == 0 || (m_fflvtaglist_onlykey.GetCheck() == 1 && keyframe == 1)) && (m_fflvtaglist_onlyvideo.GetCheck() == 0 || (m_fflvtaglist_onlyvideo.GetCheck() == 1 && type == 9)))
	{
		temp_type.Format(_T("%s"), text);
		temp_index.Format(_T("%d"), tl_index);
		temp_datasize.Format(_T("%d"), datasize);
		temp_timestamp.Format(_T("%d"), timestamp);
		temp_streamid.Format(_T("%d"), streamid);
		temp_keyframe.Format(_T("%d"), keyframe);
		for (int i = 0; i < headbytelen; i++)
		{
			temp_headbyte.AppendFormat("%02x ", headbyte[i]);
		}
		temp_sps.Format(_T("%s"), sps);
		temp_pps.Format(_T("%s"), pps);
		temp_nal_unit_type.Format(_T("%s"), nal_unit_type);
		//��ȡ��ǰ��¼����
		nIndex = m_fflvtaglist.GetItemCount();
		//���С����ݽṹ
		LV_ITEM lvitem;
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = nIndex;
		lvitem.iSubItem = 0;
		//ע��vframe_index������ֱ�Ӹ�ֵ��
		//���ʹ��f_indexִ��Format!�ٸ�ֵ��
		lvitem.pszText = temp_index.GetBuffer();
		//------------------------
		m_fflvtaglist.InsertItem(&lvitem);
		m_fflvtaglist.SetItemText(nIndex, 1, temp_type);
		m_fflvtaglist.SetItemText(nIndex, 2, temp_datasize);
		m_fflvtaglist.SetItemText(nIndex, 3, temp_timestamp);
		m_fflvtaglist.SetItemText(nIndex, 4, temp_streamid);
		m_fflvtaglist.SetItemText(nIndex, 5, temp_keyframe);
		m_fflvtaglist.SetItemText(nIndex, 6, temp_nal_unit_type);
		m_fflvtaglist.SetItemText(nIndex, 7, temp_sps);
		m_fflvtaglist.SetItemText(nIndex, 8, temp_pps);
		m_fflvtaglist.SetItemText(nIndex, 9, temp_headbyte);
		tl_index++;
	}

	return TRUE;
}
int CSpecialFFLVDlg::AppendBInfo(CString dst_group, CString property_name, CString value, CString remark) {
	if (dst_group.Compare(_T("Header")) == 0) {
		Headergroup->AddSubItem(new CMFCPropertyGridProperty(property_name, (_variant_t)value, remark));
	}
	else if (dst_group.Compare(_T("TagData_fb")) == 0) {
		TagData_fbgroup->AddSubItem(new CMFCPropertyGridProperty(property_name, (_variant_t)value, remark));
	}
	else if (dst_group.Compare(_T("ScriptData")) == 0) {
		ScriptData_fbgroup->AddSubItem(new CMFCPropertyGridProperty(property_name, (_variant_t)value, remark));
	}
	else {

	}
	return TRUE;
}
int CSpecialFFLVDlg::ShowBInfo() {
	m_fflvbasicinfo.AddProperty(Headergroup);
	m_fflvbasicinfo.AddProperty(TagData_fbgroup);
	m_fflvbasicinfo.AddProperty(ScriptData_fbgroup);
	return TRUE;
}
//����TagData�ĵ�һ���ֽ�
int CSpecialFFLVDlg::ParseTagData_fb(int type, char data_f_b) {
	CString temp_v_frametype, temp_v_codec, temp_a_format, temp_a_rate, temp_a_size, temp_a_type;
	int x;


	switch (type) {
	case 8: {
		text.LoadString(IDS_UNKNOWN);
		x = data_f_b & 0xF0;
		x = x >> 4;
		switch (x)
		{
		case 0:temp_a_format.Format(_T("Linear PCM, platform endian")); break;
		case 1:temp_a_format.Format(_T("ADPCM")); break;
		case 2:temp_a_format.Format(_T("MP3")); break;
		case 3:temp_a_format.Format(_T("Linear PCM, little endian")); break;
		case 4:temp_a_format.Format(_T("Nellymoser 16-kHz mono")); break;
		case 5:temp_a_format.Format(_T("Nellymoser 8-kHz mono")); break;
		case 6:temp_a_format.Format(_T("Nellymoser")); break;
		case 7:temp_a_format.Format(_T("G.711 A-law logarithmic PCM")); break;
		case 8:temp_a_format.Format(_T("G.711 mu-law logarithmic PCM")); break;
		case 9:temp_a_format.Format(_T("reserved")); break;
		case 10:temp_a_format.Format(_T("AAC")); break;
		case 11:temp_a_format.Format(_T("Speex")); break;
		case 14:temp_a_format.Format(_T("MP3 8-Khz")); break;
		case 15:temp_a_format.Format(_T("Device-specific sound")); break;
		default:temp_a_format.Format(_T("%s"), text); break;
		}
		x = data_f_b & 0x0C;
		x = x >> 2;
		switch (x)
		{
		case 0:temp_a_rate.Format(_T("5.5-kHz")); break;
		case 1:temp_a_rate.Format(_T("1-kHz")); break;
		case 2:temp_a_rate.Format(_T("22-kHz")); break;
		case 3:temp_a_rate.Format(_T("44-kHz")); break;
		default:temp_a_rate.Format(_T("%s"), text); break;
		}
		x = data_f_b & 0x02;
		x = x >> 1;
		switch (x)
		{
		case 0:temp_a_size.Format(_T("8Bit")); break;
		case 1:temp_a_size.Format(_T("16Bit")); break;
		default:temp_a_size.Format(_T("%s"), text); break;
		}
		x = data_f_b & 0x01;
		switch (x)
		{
		case 0:temp_a_type.Format(_T("Mono")); break;
		case 1:temp_a_type.Format(_T("Stereo")); break;
		default:temp_a_type.Format(_T("%s"), text); break;
		}


		text.LoadString(IDS_INFO_TAG_AUDIOCODEC);
		AppendBInfo(_T("TagData_fb"), text.GetBuffer(), temp_a_format, text.GetBuffer());
		text.LoadString(IDS_INFO_TAG_AUDIO_SFMT);
		AppendBInfo(_T("TagData_fb"), text.GetBuffer(), temp_a_rate, text.GetBuffer());
		text.LoadString(IDS_INFO_TAG_AUDIO_SR);
		AppendBInfo(_T("TagData_fb"), text.GetBuffer(), temp_a_size, text.GetBuffer());
		text.LoadString(IDS_INFO_TAG_AUDIO_TYPE);
		AppendBInfo(_T("TagData_fb"), text.GetBuffer(), temp_a_type, text.GetBuffer());

		break;
	}
	case 9: {
		text.LoadString(IDS_UNKNOWN);
		x = data_f_b & 0xF0;
		x = x >> 4;
		switch (x)
		{
		case 1:temp_v_frametype.Format(_T("keyframe (for AVC, a seekable frame)")); break;
		case 2:temp_v_frametype.Format(_T("inter frame (for AVC, a nonseekable frame)")); break;
		case 3:temp_v_frametype.Format(_T("disposable inter frame (H.263 only)")); break;
		case 4:temp_v_frametype.Format(_T("generated keyframe (reserved for server use only)")); break;
		case 5:temp_v_frametype.Format(_T("video info/command frame")); break;
		default:temp_v_frametype.Format(_T("%s"), text); break;
		}
		x = data_f_b & 0x0F;
		switch (x)
		{
		case 1:temp_v_codec.Format(_T("JPEG (currently unused)")); break;
		case 2:temp_v_codec.Format(_T("Sorenson H.263")); break;
		case 3:temp_v_codec.Format(_T("Screen video")); break;
		case 4:temp_v_codec.Format(_T("On2 VP6")); break;
		case 5:temp_v_codec.Format(_T("On2 VP6 with alpha channel")); break;
		case 6:temp_v_codec.Format(_T("Screen video version 2")); break;
		case 7:temp_v_codec.Format(_T("AVC")); break;
		default:temp_v_codec.Format(_T("%s"), text); break;
		}
		text.LoadString(IDS_INFO_TAG_VIDEOCODEC);


		AppendBInfo(_T("TagData_fb"), text, temp_v_codec, text);

		break;
	}
	case 18:
		break;
	default:break;
	}
	return 0;
}

typedef union endian_checker endian_checker_t;

union endian_checker
{
	long endian_value;
	char byte_array[sizeof(long)];
};
static endian_checker_t EndianChecker = { 1 };


int is_little_endian(void)
{
	return EndianChecker.byte_array[0];
}


double read_be64double(unsigned char* data)
{
	double value;
	unsigned char number_data[8];

	if (is_little_endian())
	{
		number_data[0] = data[7];
		number_data[1] = data[6];
		number_data[2] = data[5];
		number_data[3] = data[4];
		number_data[4] = data[3];
		number_data[5] = data[2];
		number_data[6] = data[1];
		number_data[7] = data[0];
		memmove(&value, number_data, 8);
	}
	else
	{
		memmove(&value, data, 8);
	}

	return value;
}

int CSpecialFFLVDlg::ParseScriptArrayData(const unsigned char* meta)
{
	int ecma_array_len = 0;
	int keyname_len = 0;
	//char keyname[32];
	char* keyname = new char[0xffff + 1];
	memset(keyname, 0, 0xffff + 1);
	const unsigned char* p = (unsigned char*)meta;
	int i = 0;

	ecma_array_len |= *p;
	p++;
	ecma_array_len = ecma_array_len << 8;
	ecma_array_len |= *p;
	p++;
	ecma_array_len = ecma_array_len << 8;
	ecma_array_len |= *p;
	p++;
	ecma_array_len = ecma_array_len << 8;
	ecma_array_len |= *p;
	p++;

	for (i = 0; i < ecma_array_len; i++) {
		keyname_len = 0;
		keyname_len |= *p;
		p++;
		keyname_len = keyname_len << 8;
		keyname_len |= *p;
		p++;
		if (keyname_len > 0xffff + 1)
		{
			MessageBox("error len");
		}
		memset(keyname, 0, 0xffff + 1);
		strncpy((char*)keyname, (const char*)p, keyname_len);
		p += keyname_len;
		p += ParseScriptType(p, keyname);
	}
	delete[]keyname;

	if (*p == 0 && *(p + 1) == 0 && *(p + 2) == 9) {
		p += 3;
	}
	return p - meta;

}

enum script_data_type {
	Number = 0,
	Boolean,
	String,
	Object,
	MovieClip,
	Null,
	Undefined,
	Reference,
	EcmaArray,
	ObjectEndMarker,
	StringArray,
	Date,
	LongString,
};

int CSpecialFFLVDlg::ParseScriptType(const unsigned char* meta, const char* namep)
{
	char name[1024] = { 0 };
	if (!namep || strlen(namep) == 0)
	{
		strcpy(name, "EMPTY_STR");
	}
	else
	{
		strcpy(name, namep);
	}
	//MessageBox(name);
	int ret = 0;
	const unsigned char* p = meta;
	//char string_output[512];
	char* string_output = new char[0xffff + 1];
	unsigned long long number = 0;
	double double_number = 0.0;

	double numValue = 0;
	bool boolValue = false;
	CString strOutTemp;
	CString strTemp;
	CString strLongTemp;

	switch (*p) {
	case Number:
		//MessageBox("Number");
		p++;
		numValue = read_be64double((unsigned char*)(p));
		strOutTemp.Format("%.2f", numValue);
		p += 8;
		break;

	case Boolean:
		//MessageBox("Boolean");
		p++;
		if (*p != 0x00) {
			boolValue = true;
		}
		strOutTemp.Format("%s", boolValue ? "true" : "false");

		p++;
		break;

	case String:
		//MessageBox("String");
		p++;
		ret |= *p;
		p++;
		ret = ret << 8;
		ret |= *p;
		p++;
		memset(string_output, 0, 0xffff + 1);
		strncpy(string_output, (const char*)(p), ret);
		strOutTemp = string_output;
		p += ret;
		break;

	case Object:
		//MessageBox("Object");
		p++;
		break;

	case MovieClip:
		//MessageBox("MovieClip");
		p++;
		break;

	case Null:
		//MessageBox("Null");
		p++;
		break;

	case Undefined:
		//MessageBox("Undefined");
		p++;
		break;

	case Reference:
		//MessageBox("Reference");
		p++;
		break;

	case EcmaArray:
		//MessageBox("EcmaArray");
		p++;
		ret = ParseScriptArrayData(p);
		p += ret;
		break;

	case ObjectEndMarker:
		//MessageBox("ObjectEndMarker");
		p++;
		break;

	case StringArray:
		//MessageBox("StringArray");
		p++;
		break;

	case Date:
		//MessageBox("Date");
		p++;
		p += 10;
		strOutTemp = "type date not impl";
		break;

	case LongString:
		//MessageBox("LongString");
		p++;
		ret = 0;
		ret |= *p;
		p++;
		ret = ret << 8;
		ret |= *p;
		p++;
		ret = ret << 8;
		ret |= *p;
		p++;;
		ret = ret << 8;
		ret |= *p;
		p++;
		if (ret > 0xffff)
		{
			memset(string_output, 0, 0xffff + 1);
			strncpy((char*)string_output, (const char*)(p), ret);
			strOutTemp = string_output;
		}
		else
		{
			strOutTemp = "too long";
		}
		p += ret;
		break;

	default:
		strOutTemp.Format("default type %02X", *p);
		p++;
		break;
	}
	if (strOutTemp.GetLength() == 0)
	{
		strOutTemp = "EMPTY_STR";
	}
	AppendBInfo(_T("ScriptData"), name, strOutTemp, name);

	delete[]string_output;
	return p - meta;
}

int CSpecialFFLVDlg::ParseScriptData(const unsigned char* meta, int meta_size)
{
	const unsigned char* p = meta;
	int ret = 0;
	while (p) {
		if (p - meta >= meta_size) {
			break;
		}
		ret = ParseScriptType(p, "");
		p += ret;
	}
	return p - meta;

	unsigned int arrayLen = 0;
	unsigned int offset = 13;
	unsigned int nameLen = 0;
	double numValue = 0;
	bool boolValue = false;
	//unsigned char string_output[512];
	char* string_output = new char[0xffff + 1];

	if (meta[offset++] == 0x08) {

		arrayLen |= meta[offset++];
		arrayLen = arrayLen << 8;
		arrayLen |= meta[offset++];
		arrayLen = arrayLen << 8;
		arrayLen |= meta[offset++];
		arrayLen = arrayLen << 8;
		arrayLen |= meta[offset++];

		//cerr << "ArrayLen = " << arrayLen << endl;
	}
	else {
		//TODO:
		//cerr << "metadata format error!!!" << endl;
		return -1;
	}

	for (unsigned int i = 0; i < arrayLen; i++) {
		CString strOutTemp;
		CString strTemp;
		CString strLongTemp;

		unsigned long long number = 0;
		numValue = 0;
		boolValue = false;
		nameLen = 0;
		nameLen |= meta[offset++];
		nameLen = nameLen << 8;
		nameLen |= meta[offset++];
		//cerr << "name length=" << nameLen << " ";
		//unsigned char keyname[32];
		char* keyname = new char[0xffff + 1];
		memset(keyname, 0, 0xffff + 1);
		strncpy((char*)keyname, (const char*)(meta + offset), nameLen);
		//	fprintf(stdout, "keyname = [%s]\n", keyname);
		CString name(keyname);
		delete[]keyname;
#ifdef DEBUG
		printf("\norign \n");
		for (unsigned int i = 0; i < nameLen + 3; i++) {
			printf("%x ", meta[offset + i]);
		}
		printf("\n");
#endif
		offset += nameLen;
		//cerr << "name=" << name << " ";
#ifdef DEBUG
		printf("memcpy\n");
		for (unsigned int i = 0; i < nameLen; i++) {
			printf("%x ", name[i]);
		}
		printf("\n");
#endif
		switch (meta[offset++]) {
		case 0x0: //Number type
			numValue = read_be64double((unsigned char*)(meta + offset));
			strOutTemp.Format("%.2f", numValue);
			offset += 8;
			break;

		case 0x1: //Boolean type
			if (offset++ != 0x00) {
				boolValue = true;
			}
			strOutTemp.Format("%s", boolValue ? "true" : "false");
			break;

		case 0x2: //String type
			nameLen = 0;
			nameLen |= meta[offset++];
			nameLen = nameLen << 8;
			nameLen |= meta[offset++];
			strTemp = CString((char*)meta[offset], nameLen);
			memset(string_output, 0, 0xffff + 1);
			strncpy((char*)string_output, (const char*)(meta + offset), nameLen);
			strOutTemp = string_output;
			offset += nameLen;
			break;

		case 0x12: //Long string type
			nameLen = 0;
			nameLen |= meta[offset++];
			nameLen = nameLen << 8;
			nameLen |= meta[offset++];
			nameLen = nameLen << 8;
			nameLen |= meta[offset++];
			nameLen = nameLen << 8;
			nameLen |= meta[offset++];
			memset(string_output, 0, 0xffff + 1);
			strncpy((char*)string_output, (const char*)(meta + offset), nameLen);
			strOutTemp = string_output;
			offset += nameLen;
			break;

			//FIXME:
		default:
			break;
		}
		delete[]string_output;
		AppendBInfo(_T("ScriptData"), name, strOutTemp, name);

#if 0
		if (strncmp(name, "duration", 8) == 0) {
			m_duration = numValue;
		}
		else if (strncmp(name, "width", 5) == 0) {
			m_width = numValue;
		}
		else if (strncmp(name, "height", 6) == 0) {
			m_height = numValue;
		}
		else if (strncmp(name, "framerate", 9) == 0) {
			m_framerate = numValue;
		}
		else if (strncmp(name, "videodatarate", 13) == 0) {
			m_videodatarate = numValue;
		}
		else if (strncmp(name, "audiodatarate", 13) == 0) {
			m_audiodatarate = numValue;
		}
		else if (strncmp(name, "videocodecid", 12) == 0) {
			m_videocodecid = numValue;
		}
		else if (strncmp(name, "audiosamplerate", 15) == 0) {
			m_audiosamplerate = numValue;
		}
		else if (strncmp(name, "audiosamplesize", 15) == 0) {
			m_audiosamplesize = numValue;
		}
		else if (strncmp(name, "audiocodecid", 12) == 0) {
			m_audiocodecid = numValue;
		}
		else if (strncmp(name, "stereo", 6) == 0) {
			m_stereo = boolValue;
		}
#endif
	}
	return 0;

}

void CSpecialFFLVDlg::SystemClear() {
	Headergroup->RemoveAllOptions();
	TagData_fbgroup->RemoveAllOptions();
	ScriptData_fbgroup->RemoveAllOptions();


	Headergroup = new CMFCPropertyGridProperty(_T("Header"));
	TagData_fbgroup = new CMFCPropertyGridProperty(_T("TagData Header"));
	ScriptData_fbgroup = new CMFCPropertyGridProperty(_T("ScriptData"));
	m_fflvbasicinfo.RemoveAll();
	m_fflvtaglist.DeleteAllItems();
	tl_index = 0;
}
//ListCtrl����ɫ
void CSpecialFFLVDlg::OnCustomdrawMyList(NMHDR* pNMHDR, LRESULT* pResult)
{
	//This code based on Michael Dunn's excellent article on
	//list control custom draw at http://www.codeproject.com/listctrl/lvcustomdraw.asp

	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.
	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		// This is the notification message for an item.  We'll request
		// notifications before each subitem's prepaint stage.

		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
	{

		COLORREF clrNewTextColor, clrNewBkColor;

		int    nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);

		CString strTemp = m_fflvtaglist.GetItemText(nItem, 1);
		CString key_strTemp = m_fflvtaglist.GetItemText(nItem, 5);
		CString text_video, text_audio, text_script;
		text_video.LoadString(IDS_LIST_DATA_TYPE_VIDEO);
		text_audio.LoadString(IDS_LIST_DATA_TYPE_AUDIO);
		text_script.LoadString(IDS_LIST_DATA_TYPE_SCRIPT);

		if (strTemp.Compare(text_video) == 0) {
			clrNewTextColor = RGB(0, 0, 0);		//Set the text
			if (0 == key_strTemp.Compare("1"))
			{
				clrNewBkColor = RGB(255, 0, 0);		//��ɫ
			}
			else
			{
				clrNewBkColor = RGB(0, 255, 255);		//��ɫ
			}
		}
		else if (strTemp.Compare(text_audio) == 0) {
			clrNewTextColor = RGB(0, 0, 0);		//text 
			clrNewBkColor = RGB(255, 255, 0);		//��ɫ
		}
		else if (strTemp.Compare(text_script) == 0) {
			clrNewTextColor = RGB(0, 0, 0);		//text
			clrNewBkColor = RGB(255, 153, 0);		//����ɫ
		}
		else {
			clrNewTextColor = RGB(0, 0, 0);		//text
			clrNewBkColor = RGB(255, 255, 255);			//��ɫ
		}

		pLVCD->clrText = clrNewTextColor;
		pLVCD->clrTextBk = clrNewBkColor;


		// Tell Windows to paint the control itself.
		*pResult = CDRF_DODEFAULT;


	}
}

void CSpecialFFLVDlg::OnBnClickedFFlvAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}


void CSpecialFFLVDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialogEx::OnDropFiles(hDropInfo);
	LPTSTR pFilePathName = (LPTSTR)malloc(MAX_URL_LENGTH);
	::DragQueryFile(hDropInfo, 0, pFilePathName, MAX_URL_LENGTH);  // ��ȡ�Ϸ��ļ��������ļ�������ؼ���
	m_fflvinputurl.SetWindowText(pFilePathName);
	::DragFinish(hDropInfo);   // ע����������٣��������ͷ�Windows Ϊ�����ļ��ϷŶ�������ڴ�
	free(pFilePathName);
}
