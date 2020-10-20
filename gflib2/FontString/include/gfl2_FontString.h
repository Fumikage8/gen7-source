#ifndef GFLIB2_FONTSTRING_FONTSTRING_H_INCLUDED
#define GFLIB2_FONTSTRING_FONTSTRING_H_INCLUDED

#include <types/include/gfl2_Typedef.h>

namespace gfl2 { namespace fontstring {

class FontString
{
public:
	static void Create(const c8* pCharsetBinaryData, s32 binarySize);
	static void Destroy();
	static c16* ConvertUTF16(c16* pOut, const c16* wstr);

private:
	class Impl;
	static Impl* s_pImpl;
	
	// 標準のコンストラクタ, デストラクタ, コピーコンストラクタ及び代入を禁止
	FontString();
	~FontString();
	FontString(const FontString&);
	FontString& operator=(const FontString&);
};

}}

#endif