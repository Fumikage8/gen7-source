#pragma once

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string.h>

// Ruby�p���C�u�����̃w�b�_�Ƃ��Ă��g����悤�ɂ���
#ifdef BUILD_DLL
#define DLLEXPORT __declspec(dllexport)
#define CEXTERN_BEGIN 
#define CEXTERN_END

#else
#define DLLEXPORT 
#define CEXTERN_BEGIN extern "C" {
#define CEXTERN_END   }

//gcc-3 -mno-cygwin�̃R���p�C����ʂ����߂ɕK�v
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
	 * @breaf �V���{���Q�Ƃ��L�^����
     * �t�H�[�}�b�g : R,[startAdrr hex�\�L],[data_size hex�\�L],[symbol](,[comment])
     * �� :
     *   R,000000A0,0002,SYM_1,Ref in file_1.txt
     *   R,000000E0,0002,SYM_2
	 * @param
     */
	void recordRefSymbol(const unsigned int startAddr, const unsigned int data_size, const char* symbol, const char* comment = NULL);

    /*
	  �V���{����`���L�^����
      �t�H�[�}�b�g : D,[symbol],[value (Crypt2OctetStr�ŃG���R�[�h)],[type(�l�̌^������������)]
	  type(�l�̌^������������)��Ruby��Array.pack���\�b�h�̃t�H�[�}�b�g�ɏ�������
      ��:
        D,SYM_1,\001\030, S
     */
	void recordDefSymbol(const char* symbol, const unsigned char* value, const unsigned int value_size, const char* type);

private:
   #pragma warning(disable:4251)
	std::ofstream symbolFp;
   #pragma warning(default:4251)

	/*
	  bytes��\000\000...�`���̕�����ɃG���R�[�h
	  �� : [0,255,10,1] => \000\255\010\001
	 */
	void Crypt2OctetStr(const unsigned char* bytes, const unsigned int byte_size, char* outStr);
};

CEXTERN_END
