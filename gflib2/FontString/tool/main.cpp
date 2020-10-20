#include <gfl2_Typedef.h>
#include <rapidxml.hpp>
#include <iostream>
#include "Util.h"

using namespace rapidxml;

using namespace std;
using namespace gfl::String;

//いずれＩＯクラスができたら置き換える。
static c8* ReadFile( const char* filename, s32* p_size, s32 align = 16 )
{
	FILE		*hFile;
	int			file_size;
	char		*read_buff;

	fopen_s( &hFile, filename, "rb" );

	if ( hFile == NULL ){
		assert(0);
		return NULL;
	}

	fseek( hFile, 0, SEEK_END );
	file_size = ftell( hFile );
	*p_size = file_size;
	fseek( hFile, 0, SEEK_SET );

	file_size += align;//必ずNULLが入るようにしたい。
	u32 buf_size = file_size + ((-file_size) & (align-1));
	
	// 一旦mallocを使う
	read_buff = reinterpret_cast<c8*>( malloc(buf_size) );
	memset( read_buff, 0, buf_size );
	fread( read_buff, file_size, 1, hFile );
	fclose( hFile );

	return( read_buff );
}

static void CloseFile(c8* pData)
{
	// 一旦freeを使う
	free(pData);
}

void stringReplace(std::string& str, const std::string& from, const std::string& to)
{
	std::string::size_type pos = 0;
	while(pos = str.find(from, pos), pos != std::string::npos)
	{
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
}

c8* stringShiftC8(c8* str, s32 shift)
{
	c8* pStart = str;
	c8* pEnd = str + strlen(str);
	memmove(pStart + shift, pStart, pEnd - pStart + 1);
	return str + shift;
}

c8* stringReplaceC8(c8* str, const c8* from, const c8* to)
{
	s32 fromLength = (s32)strlen(from);
	s32 toLength = (s32)strlen(to);
	s32 shift = toLength - fromLength;	// 置換後の文字列の移動量
	c8* pStart = str;
	c8* pEnd = str + (s32)strlen(str);

	c8* p;
	while(p = strstr(pStart, from))
	{
		pStart = stringShiftC8(p + fromLength, shift);
		pEnd += shift;
		memmove(p, to, toLength);
	}

	return str;
}

c16* stringShiftC16(c16* str, s32 shift)
{
	c16* pStart = str;
	c16* pEnd = str + wcslen(str);
	memmove(pStart + shift, pStart, pEnd - pStart + 1);
	return str + shift;
}

c16* stringReplaceC16(c16* str, const c16* from, const c16* to)
{
	s32 fromLength = (s32)wcslen(from);
	s32 toLength = (s32)wcslen(to);
	s32 shift = toLength - fromLength;	// 置換後の文字列の移動量
	c16* pStart = str;
	c16* pEnd = str + (s32)wcslen(str);

	c16* p;
	while(p = wcsstr(pStart, from))
	{
		pStart = stringShiftC16(p + fromLength, shift);
		pEnd += shift;
		memmove(p, to, toLength * sizeof(c16));
	}

	return str;
}

s32 findComment(c8* str, c8** pCommentStart, c8** pCommentEnd)
{
	const c8* commentHead = "<!--";
	s32 commentHeadLength = (s32)strlen(commentHead);
	c8* pStart = str;
	c8* pEnd = str + strlen(str);

	s32 commentLength = 0;
	*pCommentStart = strstr(pStart, commentHead);
	if(*pCommentStart)
	{
		*pCommentEnd = strstr(*pCommentStart, ">");
		commentLength = (s32)(*pCommentEnd - *pCommentStart);
	}
	return commentLength;
}

s32 deleteComment(c8* str)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	c8* pCommentStart = NULL;
	c8* pCommentEnd = NULL;
	s32 commentLength = 0;
	s32 deleteCommentNum = 0;
	while(commentLength = findComment(str, &pCommentStart, &pCommentEnd))
	{
		memcpy(buff, pCommentStart, commentLength + 1);
		stringReplaceC8(str, buff, "");
		memset(buff, 0, sizeof(buff));
		++deleteCommentNum;
	}
	return deleteCommentNum;
}

class GfConverterXmlToBin : public rapidxml::xml_document<>
{
public:
	GfConverterXmlToBin(c8* pData, const c8* outputFilePath);
	virtual ~GfConverterXmlToBin();
};

GfConverterXmlToBin::GfConverterXmlToBin(c8* pData, const c8* outputFilePath) : rapidxml::xml_document<>()
{
	//this->parse< parse_no_utf8 | parse_no_entity_translation>( pData );
	this->parse<parse_default>( pData );
	const xml_node<>* top_node = first_node("letter-order");
	for( const xml_node<>* node = top_node->first_node("body"); node; node = node->next_sibling( ) )
	{
		string	nodeName = node->name();
		for(const xml_node<>* charcode_node = node->first_node("order"); charcode_node; charcode_node = charcode_node->next_sibling("order"))
		{
			string charcodeNodeValue = charcode_node->value();
			stringReplace(charcodeNodeValue, " ", "");
			stringReplace(charcodeNodeValue, "++", " ");

			wstring charcodeUtf16 = Util::UTF8toUTF16(charcodeNodeValue);
			const c16* charCodeUtf16Bin = charcodeUtf16.c_str();
			s32 size = (s32)(sizeof(c16) * charcodeUtf16.size());
			c16* charCodeUtf16BinOutput = (c16*)malloc(size);
			memcpy(charCodeUtf16BinOutput, charCodeUtf16Bin, size);

			// ASCIIの範囲内のマイナスは飛ばして、残りのマイナスをNULLに書き換える
			c16 minus = L'-';
			for(c16* p = charCodeUtf16BinOutput + L'-' + 1; p < charCodeUtf16BinOutput + size; ++p)
			{
				if (*p == minus)
				{
					*p = L'\0';
				}
			}

			FILE* hFile;
			fopen_s(&hFile, outputFilePath, "wb");
			if(hFile == NULL)
			{
				assert(0);
				break;
			}
			fwrite(charCodeUtf16BinOutput, sizeof(c16) * charcodeUtf16.length(), 1, hFile);
			fclose(hFile);
			cout << "出力先 : " << outputFilePath << endl;
			cout << "出力した文字数 : " << charcodeUtf16.length() << endl;

			break;
		}
		break;
	}
}

GfConverterXmlToBin::~GfConverterXmlToBin()
{
}

int main(s32 argc, c8** argv)
{
	if(argc != 2)
	{
		printf_s("xml形式のテキストデータをドラッグ&ドロップしてください\n");
		printf_s("Enterキーを押してください\n");
		getchar();
		return -1;
	}
	s32 size;
	c8* pXmlData = ReadFile(argv[1], &size);
	//c8* pXmlData = ReadFile("C:/next/git_gflib2/FontString/data/pokemon_wb.xlor", &size);
	// XMLパーサに渡す前にc8*型の時点で修正できるところを修正
	deleteComment(pXmlData);
	stringReplaceC8(pXmlData, "\n", "");
	stringReplaceC8(pXmlData, "\r", "");
	stringReplaceC8(pXmlData, "\t", "");
	stringReplaceC8(pXmlData, "&amp;", "&");
	stringReplaceC8(pXmlData, "&apos;", "\'");
	stringReplaceC8(pXmlData, "&quot;", "\"");
	// タグで残っている<sp/>と<null/>はココで予め置換
	stringReplaceC8(pXmlData, "<sp/>", "++");
	stringReplaceC8(pXmlData, "<null/>", "-");

	char* delimiterPos = strrchr(argv[1], '\\');
	char importFilePath[1024];
	memset(importFilePath, 0, sizeof(importFilePath));
	memcpy(importFilePath, argv[1], delimiterPos - argv[1] + 1);
	char exportFilePath[1024];
	char exportFileName[256];
	memset(exportFilePath, 0, sizeof(exportFilePath));
	memset(exportFileName, 0, sizeof(exportFileName));
	printf_s("出力するファイル名を入力してください\n");
	gets_s(exportFileName, sizeof(exportFileName));
	if(strlen(exportFileName) == 0)
	{
		strcpy_s(exportFileName, "font_binary");
	}
	sprintf_s(exportFilePath, "%s%s.bin", importFilePath, exportFileName);
	GfConverterXmlToBin convXmlToBin(pXmlData, exportFilePath);
	CloseFile(pXmlData);
	cout << "変換が完了しました. Enterキーを押してください" << endl;
	getchar();
	return 0;
}