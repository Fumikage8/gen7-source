#include <iomanip>
#include <fstream>
#include <iostream>
#include <string.h>
#include <string>

#include "SymbolicTableWriter.h"

// Ruby用ライブラリのヘッダとしても使えるようにする
#ifdef BUILD_DLL
#define DLLEXPORT __declspec(dllexport)
#define CEXTERN_BEGIN 
#define CEXTERN_END

#else
#define DLLEXPORT 
#define CEXTERN_BEGIN extern "C" {
#define CEXTERN_END   }
#endif


CEXTERN_BEGIN

//
class DLLEXPORT SymbolicBinaryWriter {

private:
#pragma warning(disable:4251)
	std::ofstream binaryFp;
#pragma warning(default:4251)

	SymbolicTableWriter tableWriter;

public:
	/*
	  シンボリックバイナリ(filename)とシンボルテーブル(filename.symbol)ファイルを作成する
	 */
	SymbolicBinaryWriter(const char* filename);
	~SymbolicBinaryWriter();

	void close();

	void write(const char* data, const unsigned int data_size);

	//
	void writeWithSymbol(const unsigned int data_size, const char* symbol, const char* comment = NULL);

	/*
	  シンボル定義の記録
	  See : SymbolTableWriter::recordDefSymbol
	 */
	void recordDefSymbol(const char* symbol, const unsigned char* value, const unsigned int value_size, const char* type);

	//
	void setPos(const unsigned int address);
	//
	unsigned int getPos();
};

CEXTERN_END

//
class SymbolicBinaryWriterException : public std::exception{};
