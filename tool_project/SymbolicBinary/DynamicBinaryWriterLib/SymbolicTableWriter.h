#pragma once

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string.h>

// Ruby用ライブラリのヘッダとしても使えるようにする
#ifdef BUILD_DLL
#define DLLEXPORT __declspec(dllexport)
#define CEXTERN_BEGIN 
#define CEXTERN_END

#else
#define DLLEXPORT 
#define CEXTERN_BEGIN extern "C" {
#define CEXTERN_END   }

//gcc-3 -mno-cygwinのコンパイルを通すために必要
#undef write
#undef close

#endif



CEXTERN_BEGIN

/**
 */
class DLLEXPORT SymbolicTableWriter
{
public:
	SymbolicTableWriter(const char* file_path);
	~SymbolicTableWriter();

	void close();


    /**
	 * @breaf シンボル参照を記録する
     * フォーマット : R,[startAdrr hex表記],[data_size hex表記],[symbol](,[comment])
     * 例 :
     *   R,000000A0,0002,SYM_1,Ref in file_1.txt
     *   R,000000E0,0002,SYM_2
	 * @param
     */
	void recordRefSymbol(const unsigned int startAddr, const unsigned int data_size, const char* symbol, const char* comment = NULL);

    /*
	  シンボル定義を記録する
      フォーマット : D,[symbol],[value (Crypt2OctetStrでエンコード)],[type(値の型を示す文字列)]
	  type(値の型を示す文字列)はRubyのArray.packメソッドのフォーマットに準拠する
      例:
        D,SYM_1,\001\030, S
     */
	void recordDefSymbol(const char* symbol, const unsigned char* value, const unsigned int value_size, const char* type);

private:
   #pragma warning(disable:4251)
	std::ofstream symbolFp;
   #pragma warning(default:4251)

	/*
	  bytesを\000\000...形式の文字列にエンコード
	  例 : [0,255,10,1] => \000\255\010\001
	 */
	void Crypt2OctetStr(const unsigned char* bytes, const unsigned int byte_size, char* outStr);
};

CEXTERN_END
