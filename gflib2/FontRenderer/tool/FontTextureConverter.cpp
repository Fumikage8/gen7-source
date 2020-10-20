#include <iostream>
#include <cassert>
#include "tga_struct.h"
#include "ImageBlock.h"

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

	printf_s("解像度 : %d x %d\n", tgaHeader.imageWidth, tgaHeader.imageHeight);
	int readSizeBlockWidth = getInputInteger("変換前の画像の一文字当たりの横幅を入力してください : ");
	int readSizeBlockHeight = getInputInteger("変換前の画像の一文字当たりの縦幅を入力してください : ");
	printf_s("入力画像の一文字あたりのサイズ : %d x %d\n", readSizeBlockWidth, readSizeBlockHeight);
	if(tgaHeader.imageWidth % readSizeBlockWidth || tgaHeader.imageHeight % readSizeBlockHeight)
	{
		printf_s("解像度と一致しません\n");
		printf_s("Enterキーを押してください\n");
		free(readBuff);
		return -1;
	}
	int readNumBlockWidth = tgaHeader.imageWidth / readSizeBlockWidth;
	int readNumBlockHeight = tgaHeader.imageHeight / readSizeBlockHeight;
	printf_s("横に%d文字, 縦に%d文字\n", readNumBlockWidth, readNumBlockHeight);

	int writeSizeBlockWidth = getInputInteger("出力時の一文字当たりの横幅を入力してください : ");
	int writeSizeBlockHeight = getInputInteger("出力時の一文字当たりの縦幅を入力してください : ");
	int writeNumBlockWidth = getInputInteger("出力時に横に何文字詰めるのかを入力してください : ");
	if((readNumBlockWidth * readNumBlockHeight) % writeNumBlockWidth)
	{
		printf_s("変換前の解像度と設定された値では端数が生じます\n");
		printf_s("Enterキーを押してください\n");
		free(readBuff);
		return -1;
	}
	int writeNumBlockHeight = (readNumBlockWidth * readNumBlockHeight) / writeNumBlockWidth;
	printf_s("出力画像の一文字あたりのサイズ : %d x %d\n", writeSizeBlockWidth, writeSizeBlockHeight);
	printf_s("横に%d文字, 縦に%d文字\n", writeNumBlockWidth, writeNumBlockHeight);

	int trimX = 0;
	int trimY = 0;
	if(writeSizeBlockWidth < readSizeBlockWidth || writeSizeBlockHeight < readSizeBlockHeight)
	{
		trimX = getInputInteger("変換前の画像のブロックの切り抜き開始位置(X座標)を入力してください : ");
		trimY = getInputInteger("変換前の画像のブロックの切り抜き開始位置(Y座標)を入力してください : ");
	}

	int divisionNum = getInputInteger("出力画像の分割数を入力してください : ");
	
	// ピクセル部分を読み込み
	readPixelDataSize = fileSize - (sizeof(TGA_HEADER) + sizeof(TGA_FOOTER));
	readPixelData = (unsigned char*)malloc(readPixelDataSize);
	memcpy(readPixelData, &readBuff[sizeof(TGA_HEADER)], readPixelDataSize);
	printf_s("readPixelDataSize = %d\n", readPixelDataSize);

	// フッタ部分を読み込み
	memcpy(&tgaFooter, &readBuff[sizeof(TGA_HEADER) + readPixelDataSize], sizeof(TGA_FOOTER));

	// 設定値に合わせてピクセルデータを書き込み
	int writePixelDataSize = 4 * writeNumBlockWidth * writeNumBlockHeight * writeSizeBlockWidth * writeSizeBlockHeight;
	unsigned char* writePixelData =  reinterpret_cast<unsigned char*>(malloc(writePixelDataSize));

	// FIXME: imageType が0x02になっていればフルカラー = 32[bit] = 4[Byte]という処理を加える
	ImageBlockArray readImageBlockArray(
		readPixelData,
		readNumBlockWidth,
		readNumBlockHeight,
		readSizeBlockWidth,
		readSizeBlockHeight,
		COLOR_RGBA);
	// 書き込み側のブロックサイズに合わせてトリミング
	readImageBlockArray.setTrimRect(
		trimX,
		trimY,
		writeSizeBlockWidth,
		writeSizeBlockHeight);

	ImageBlockArray writeImageBlockArray(
		writePixelData,
		writeNumBlockWidth,
		writeNumBlockHeight,
		writeSizeBlockWidth,
		writeSizeBlockHeight,
		COLOR_RGBA);

	// 1次元配列のように扱って各ブロックをコピー
	for(int i = 0; i < writeNumBlockWidth * writeNumBlockHeight; ++i)
	{
		writeImageBlockArray(i) = readImageBlockArray(i);
	}

	char exportFilePath[1024];
	char exportFileName[256];
	memset(exportFilePath, 0, sizeof(exportFilePath));
	memset(exportFileName, 0, sizeof(exportFileName));
	printf_s("出力するファイル名を入力してください\n");
	gets_s(exportFileName, sizeof(exportFileName));
	for(int i = 0; i < divisionNum; ++i)
	{
		sprintf_s(exportFilePath, "%s%s_%02d.tga", importFilePath, exportFileName, i);
		writeImageBlockArray.exportAsTga(
			exportFilePath,
			writeNumBlockWidth * i,
			writeNumBlockHeight / divisionNum);
		printf_s("%s\t出力完了\n", exportFilePath);
	}

	// 読み込んだバッファ領域の解放
	free(writePixelData);
	free(readPixelData);
	free(readBuff);

	// 入力待ち
	printf_s("変換が完了しました\nEnterキーを押してください\n");
	getchar();
	return 0;
}