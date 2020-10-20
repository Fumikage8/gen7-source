#include <iostream>
#include "tga_struct.h"

using namespace std;

int getInputInteger(const char* message)
{
	int w = 0;
	char buff[256];
	memset(buff, 0, sizeof(buff));
	while(w <= 0)
	{
		printf_s(message);
		gets_s(buff, sizeof(buff) - 1);
		w = atoi(buff);
	}
	return w;
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printf_s("tga形式のテクスチャをドラッグ&ドロップしてください\n");
		printf_s("Enterキーを押してください\n");
		getchar();
		return -1;
	}

	printf_s("変換対象ファイル:%s\n", argv[1]);
	// 拡張子を調べる
	char* extPos = strrchr(argv[1], '.');
	if(!extPos)
	{
		printf_s("tgaファイルではありません. tgaファイルをドラッグ&ドロップしてください\n");
		printf_s("Enterキーを押してください\n");
		getchar();
		return -1;
	}

	if(strcmp(extPos, ".tga") != 0)
	{
		printf_s("tgaファイルではありません. tgaファイルをドラッグ&ドロップしてください\n");
		printf_s("Enterキーを押してください\n");
		getchar();
		return -1;
	}

	char* delimiterPos = strrchr(argv[1], '\\');
	char importFilePath[1024];
	memset(importFilePath, 0, sizeof(importFilePath));
	memcpy(importFilePath, argv[1], delimiterPos - argv[1] + 1);

	FILE* pFile;
	int fileSize;
	unsigned char* readBuff;

	TGA_HEADER tgaHeader;
	TGA_FOOTER tgaFooter;

	// TGAに格納されている画素情報
	unsigned int readPixelDataSize;
	unsigned char* readPixelData;

	fopen_s(&pFile, argv[1], "rb");

	if(!pFile)
	{
		printf_s("ファイルが見つかりませんでした\nファイル名:%s\n", argv[1]);
		printf_s("Enterキーを押してください\n");
		return -1;
	}

	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	readBuff = reinterpret_cast<unsigned char*>(malloc(fileSize));
	memset(readBuff, 0, fileSize);
	fread(readBuff, fileSize, 1, pFile);
	fclose(pFile);

	// ヘッダ部分を読み込み
	memcpy(&tgaHeader, readBuff, sizeof(tgaHeader));
	// ピクセル部分を読み込み
	readPixelDataSize = fileSize - (sizeof(TGA_HEADER) + sizeof(TGA_FOOTER));
	readPixelData = reinterpret_cast<unsigned char*>(malloc(readPixelDataSize));
	memcpy(readPixelData, &readBuff[sizeof(TGA_HEADER)], readPixelDataSize);
	printf_s("readPixelDataSize = %d\n", readPixelDataSize);
	// フッタ部分を読み込み
	memcpy(&tgaFooter, &readBuff[sizeof(TGA_HEADER) + readPixelDataSize], sizeof(TGA_FOOTER));

	printf_s("解像度\t: %d x %d\n", tgaHeader.imageWidth, tgaHeader.imageHeight);

	int pitchLength = 0;
	pitchLength = getInputInteger("ピッチ幅を入力してください : ");

	int pitchNum = tgaHeader.imageHeight * tgaHeader.imageWidth / pitchLength;
	char* pitchData = reinterpret_cast<char*>(malloc(pitchNum));

	// FIXME : 4Byte(フルカラー)のtga以外の場合でも読み込めるようにする
	char* pitchBuff = reinterpret_cast<char*>(malloc(pitchLength * 4));
	memset(pitchBuff, 0, pitchLength * 4);

	for(int i = 0; i < pitchNum; ++i)
	{
		// ピッチ幅分だけ読み込み, 連続している黒のピクセル数を数え上げる
		memcpy(pitchBuff, &readPixelData[i * pitchLength * 4], pitchLength * 4);
		unsigned long pitchColor = 0xFF000000;
		bool pitchStart = false;
		char pitchCount = 0;
		for(int j = 0; j < pitchLength; ++j)
		{
			unsigned long color;
			memcpy(&color, &pitchBuff[j * 4], sizeof(color));
			if(!pitchStart && (color == pitchColor))
			{
				pitchStart = true;
			}

			if(pitchStart && (color != pitchColor))
			{
				break;
			}

			if(pitchStart)
			{
				++pitchCount;
			}
		}
		pitchData[i] = pitchCount;
	}

	char exportFilePath[1024];
	char exportFileName[256];
	memset(exportFilePath, 0, sizeof(exportFilePath));
	memset(exportFileName, 0, sizeof(exportFileName));
	printf_s("出力するファイル名を入力してください : ");
	gets_s(exportFileName, sizeof(exportFileName));
	if(strlen(exportFileName) <= 0)
	{
		strcpy_s(exportFileName, "pitch_length");
	}
	sprintf_s(exportFilePath, "%s%s.pitch", importFilePath, exportFileName);
	fopen_s(&pFile, exportFilePath, "wb");
	fwrite(pitchData, pitchNum, 1, pFile);
	fclose(pFile);
	printf_s("%s\t出力完了\n", exportFilePath);

	free(pitchBuff);
	free(pitchData);
	free(readPixelData);
	free(readBuff);

	printf_s("Enterキーを押してください\n");
	getchar();
	return 0;
}