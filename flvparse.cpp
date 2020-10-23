/* 
 * 
 * FLV 格式分析器
 * FLV Format Analysis
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 * 
 * FLV封装格式分析工具
 * FLV Container Format Analysis Tools
 * 
 * 
 */
// This app splits an FLV file based on cue points located in a data file.
// It also separates the video from the audio.
// All the reverse_bytes action stems from the file format being big-endian
// and needing an integer to hold a little-endian version (for proper calculation).

#include "stdafx.h"
#include "flvparse.h"
#pragma pack(1)

//************ Constants
#define TAG_TYPE_AUDIO 8
#define TAG_TYPE_VIDEO 9
#define TAG_TYPE_SCRIPT 18
#define CUE_BLOCK_SIZE 32
#define FLAG_SEPARATE_AV 1

//*********** TYPEDEFs
typedef unsigned char ui_24[3];
typedef unsigned char byte;
typedef unsigned int uint;
typedef uint FLV_PREV_TAG_SIZE;

typedef struct {
	byte Signature[3];
	byte Version;
	byte Flags;
	uint DataOffset;
} FLV_HEADER;

typedef struct {
	byte TagType;
	ui_24 DataSize;
	ui_24 Timestamp;
	uint Reserved;
} FLV_TAG;

typedef struct {
	byte TagType;
	ui_24 DataSize;
	ui_24 Timestamp;
	byte TimestampExtend;
	ui_24 StreamID;
} FLV_TAG_NEW;

CSpecialFFLVDlg *dlg;
int fist_time_a = 0;
int fist_time_v = 0;
int fist_time_s = 0;

//********** local function prototypes
uint copymem(char *destination, char *source, uint byte_count);
uint fget(FILE *filehandle, char *buffer, uint buffer_size);
uint fput(FILE *filehandle, char *buffer, uint buffer_size);
FILE *open_output_file(byte tag_type) ;
void processfile(char *flv_filename, char *cue_file);
uint *read_cue_file(char *cue_file_name);
uint reverse_bytes(byte *buffer, char buffer_size);
uint xfer(FILE *input_file_handle, FILE *output_file_handle, uint byte_count);
uint xfer_empty(FILE *input_file_handle, FILE *output_file_handle, uint byte_count);

//********* global variables
uint current=0;
uint flags = 0;
char project_name[_MAX_PATH];

//Defines the entry point for the console application.
int flvparse(LPVOID lparam,int argc, char* argv[])
{
	dlg=(CSpecialFFLVDlg *)lparam;
	if (argc < 3) 
		printf("invalid command line\n");
	else {
		if ((argc==4) && (strstr(argv[3],"-s")!=NULL)) flags |= FLAG_SEPARATE_AV;
		processfile(argv[1], argv[2]);
	}

	return 0;
}

//processfile is the central function
void processfile(char *in_file, char *cue_file){
	//---------
	fist_time_a=0;
	fist_time_v=0;
	//-------------
	FILE *ifh=NULL, *cfh=NULL, *vfh=NULL, *afh = NULL;
	FLV_HEADER flv;
	FLV_TAG_NEW tag;
	FLV_PREV_TAG_SIZE pts, pts_z=0;
	uint *cue, ts=0, ts_new=0, ts_offset=0, ptag=0;
	
	printf("Processing [%s] with cue file [%s]\n", in_file, cue_file);
	
	//open the input file
	if ( (ifh = fopen(in_file, "rb")) == NULL) {
		AfxMessageBox(_T("Failed to open files!"));
		return;
	}

	//set project name
	strncpy(project_name, in_file, strstr(in_file, ".flv")-in_file);
	
	//build cue array
	cue = read_cue_file(cue_file);

	//capture the FLV file header
	//输出Header信息
	fget(ifh, (char *)&flv, sizeof(FLV_HEADER));
	CString temp_str;
	temp_str.Format(_T("0x %X %X %X"),flv.Signature[0],flv.Signature[1],flv.Signature[2]);
	//dlg->AppendBInfo("Header","Signature",temp_str,"为FLV文件标识，一般总为“FLV”");
	dlg->AppendBInfo(_T("Header"),_T("Signature"),temp_str,_T("Signature"));
	temp_str.Format(_T("0x %X"),flv.Version);
	//dlg->AppendBInfo("Header","Version",temp_str,"为FLV文件版本，目前为0x01");
	dlg->AppendBInfo(_T("Header"),_T("Version"),temp_str,_T("Version"));
	temp_str.Format(_T("0x %X"),flv.Flags);
	//dlg->AppendBInfo("Header","Flag",temp_str,"倒数第一bit是1表示有视频，倒数第三bit是1表示有音频，其他都应该是0");
	dlg->AppendBInfo(_T("Header"),_T("Flag"),temp_str,_T("Flag"));
	temp_str.Format(_T("0x %X"),flv.DataOffset);
	//dlg->AppendBInfo("Header","HeaderSize",temp_str,"整个文件头的长度，一般是9（3+1+1+4），有时候后面还有些别的信息，就不是9了");
	dlg->AppendBInfo(_T("Header"),_T("HeaderSize"),temp_str,_T("HeaderSize"));

	//move the file pointer to the end of the header
	fseek(ifh, reverse_bytes((byte *)&flv.DataOffset, sizeof(flv.DataOffset)), SEEK_SET);

	//process each tag in the file
	//process each tag in the file
	do {
		//capture the PreviousTagSize integer
		pts = _getw(ifh);
		//extract the tag from the input file
		fget(ifh, (char *)&tag, sizeof(FLV_TAG_NEW));
		if (feof(ifh))
			break;
		//输出Tag信息-----------------------
		int temp_datasize = tag.DataSize[0] * 65536 + tag.DataSize[1] * 256 + tag.DataSize[2];
		int temp_timestamp = tag.TimestampExtend * 16777216 + tag.Timestamp[0] * 65536 + tag.Timestamp[1] * 256 + tag.Timestamp[2];
		int temp_StreamID = tag.StreamID[0] * 65536 + tag.StreamID[1] * 256 + tag.StreamID[2];

		//还需要获取TagData的第一个字节---------------------------------
		int video_type = 0;
		if (tag.TagType == TAG_TYPE_VIDEO)
		{
			char temp_tag_f_b;
			fget(ifh, &temp_tag_f_b, 1);
			video_type = (temp_tag_f_b >> 4) & 0xF;
			//倒回去，不影响下面的操作
			fseek(ifh, -1, SEEK_CUR);
			if (fist_time_v == 0) {
				dlg->ParseTagData_fb(9, temp_tag_f_b);
				fist_time_v = 1;
			}
		}
		else if (tag.TagType == TAG_TYPE_AUDIO)
		{
			if (fist_time_a == 0) {
				char temp_tag_f_b;
				fget(ifh, &temp_tag_f_b, 1);
				dlg->ParseTagData_fb(8, temp_tag_f_b);
				//倒回去，不影响下面的操作
				fseek(ifh, -1, SEEK_CUR);
				fist_time_a = 1;
			}
		}
		else if (tag.TagType == TAG_TYPE_SCRIPT)
		{
#if 0
			fist_time_s = 0;
			if (fist_time_s == 0) {

				fist_time_s = 1;
				int metasize = reverse_bytes((byte *)&tag.DataSize, sizeof(tag.DataSize));
				char *meta_date = new char[metasize];
				memset(meta_date, 0, metasize);
				fget(ifh, meta_date, metasize);
				fseek(ifh, -metasize, SEEK_CUR);

				FILE * fm = fopen("./xxx.dat", "wb");
				fwrite(meta_date, metasize, 1, fm);
				fclose(fm);

				dlg->ParseScriptData(meta_date, metasize);
				delete[]meta_date;
			}
#endif			

		}
		//-----------------------------

		dlg->AppendTLInfo(tag.TagType, temp_datasize, temp_timestamp, temp_StreamID, video_type);
		//skip the data of this tag
		//if (!feof(ifh)) {
			fseek(ifh, reverse_bytes((byte *)&tag.DataSize, sizeof(tag.DataSize)), SEEK_CUR);
		//}
	} while (!feof(ifh));
	
	//finished...close all file pointers
	_fcloseall();

	//feedback to user
	dlg->text.LoadString(IDS_FINISH);
	AfxMessageBox(dlg->text);

}


//fget - fill a buffer or structure with bytes from a file
uint fget(FILE *fh, char *p, uint s) {
	uint i;
	for (i=0; i<s; i++)
		*(p+i) = (char)fgetc(fh);
	return i;
}

//fput - write a buffer or structure to file
uint fput(FILE *fh, char *p, uint s) {
	uint i;
	for (i=0; i<s; i++)
		fputc(*(p+i), fh);
	return i;
}

//utility function to overwrite memory
uint copymem(char *d, char *s, uint c) {
	uint i;
	for (i=0; i<c; i++)
		*(d+i) = *(s+i);
	return i;
}

//reverse_bytes - turn a BigEndian byte array into a LittleEndian integer
uint reverse_bytes(byte *p, char c) {
	int r = 0;
	int i;
	for (i=0; i<c; i++) 
		r |= ( *(p+i) << (((c-1)*8)-8*i));
	return r;
}

//xfer - transfers *count* bytes from an input file to an output file
uint xfer(FILE *ifh, FILE *ofh, uint c) {
	uint i;
	for (i=0; i<c; i++)
		fputc(fgetc(ifh),ofh);
	return i;
}

uint xfer_empty(FILE *ifh, FILE *ofh, uint c) {
	uint i;
	for (i=0; i<c; i++)
		fgetc(ifh);
	return i;
}

//This function handles iterative file naming and opening
FILE* open_output_file(byte tag) {

	//instantiate two buffers
	char file_name[_MAX_PATH], ext[4];

	//determine the file extension
	strcpy(ext, (tag==TAG_TYPE_AUDIO ? "mp3\0" : "flv\0"));
	
	//build the file name
	sprintf(file_name, "%s_%i.%s", project_name, current, ext);

	//return the file pointer
	return fopen(file_name, "wb");
		
}

//read in the cue points from file in a list format
uint * read_cue_file(char *fn) {
	FILE * cfh;
	uint ms, n, count = 0;
	char sLine[13];
	unsigned short ts[5];
	float ts_f = 0;

	//instantiate the heap pointer
	uint * p = (uint *) malloc((uint) 4); 
	
	//try opening the cue file
	if ( (cfh = fopen(fn, "r")) != NULL) {
		
		//grab the first string
		n = fscanf(cfh, "%12s", sLine);

		//loop until there are no more strings
		while (n==1) {	

			//reset milliseconds
			ms = 0;

			//check to see if in timestamp format
			if ((sLine[2]==':') && (sLine[5]==':') && (sLine[8]==':')) {
				
				//replace the colons with tabs
				sLine[2]='\t'; sLine[5]='\t'; sLine[8]='\t';

				//extract the timestamp values
				sscanf(sLine, "%d %d %d %d", &ts[0], &ts[1], &ts[2], &ts[3]);

				//calculate the timestamp as milliseconds
				ms = (ts[0]*3600 + ts[1]*60 + ts[2])*1000 + ts[3];

			} else {

				//just see if there is a decimal notation of milliseconds
				sscanf(sLine, "%f", &ts_f);
				ms = (uint) ts_f * 1000;

			}

			//if a cuepoint was found on this line
			if (ms > 0) {

				//dynamically reallocate memory space as necessary
				if (count % CUE_BLOCK_SIZE == 0)
					p = (uint *) realloc(p, (uint) ((count + CUE_BLOCK_SIZE)*sizeof(int)));

				//set the cue value
				*(p + count++) = ms;

			}

			//grab the next string
			n = fscanf(cfh, "%12s", sLine);
		}
	}

	//set the last cue point to max int
	*(p+count) = 0xFFFFFFFF;

	//return the pointer to the heap allocation
	return p;
}