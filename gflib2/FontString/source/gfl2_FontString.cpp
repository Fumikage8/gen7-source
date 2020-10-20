#include <iostream>
#include <gfx/include/gfl2_types.h>
#include <FontString/include/gfl2_FontString.h>
#include <assert.h>
#include <malloc.h>

namespace gfl2 { namespace fontstring {
FontString::Impl* FontString::s_pImpl = NULL;

class FontCharset
{
public:
	explicit FontCharset(const u16 maxSize);
	~FontCharset();
	u16& operator()(const c16 idx);
	const u16& operator()(const c16 idx) const;
	u16 size() const;

private:
	FontCharset(const FontCharset&);
	FontCharset& operator=(const FontCharset&);
	const u16 m_maxSize;	// !<@brief 最大使用可能領域
	u16* m_charcodes;		// !<@brief 文字コード配列
};

FontCharset::FontCharset(const u16 maxSize) : 
	m_maxSize(maxSize),
	m_charcodes(NULL)
{
	// @attention: mallocをheapに変えること
	m_charcodes = (u16*)malloc(sizeof(u16) * (m_maxSize + 1));
	memset(m_charcodes, 0, sizeof(u16) * (maxSize + 1));
}

FontCharset::~FontCharset()
{
	// @attention: freeをheapに変えること
	free(m_charcodes);
}

u16& FontCharset::operator()(const c16 idx)
{
	return m_charcodes[idx];
}

const u16& FontCharset::operator()(const c16 idx) const
{
	return m_charcodes[idx];
}

u16 FontCharset::size() const
{
	u16 size = 0;
	for (u16 i = 0; i < m_maxSize; ++i)
	{
		if(m_charcodes[i])
		{
			++size;
		}
	}
	return size;
}

class FontString::Impl
{
public:
	Impl(const c8* pCharsetBinaryData, s32 binarySize);
	~Impl();
	c16* convertUTF16(c16* pOut, const c16* wstr);

private:
	FontCharset* m_pFontCharset;
};

FontString::Impl::Impl(const c8* pCharsetBinaryData, s32 binarySize)
{
	// @attention: newをheapに置き換える.
	m_pFontCharset = new  FontCharset(USHRT_MAX);
	u16* pStart = (u16*)pCharsetBinaryData;
	u16* pEnd = pStart + binarySize / 2;
	u16 index = 0;
	for(u16* p = pStart; p < pEnd; ++p)
	{
		(*m_pFontCharset)(*p) = index;
		++index;
	}
}

FontString::Impl::~Impl()
{
	// @attention: deleteをheapに置き換える.
	delete m_pFontCharset;
	m_pFontCharset = NULL;
}

c16* FontString::Impl::convertUTF16(c16* pOut, const c16* wstr)
{
	u16 cntFont;
	u16 fontLength = (u16)wcslen(wstr);
	for(cntFont = 0; cntFont < fontLength; ++cntFont)
	{
		// ASCIIの範囲内ならそのまま出力
		u16 idx = wstr[cntFont];
		pOut[cntFont] = (idx < 0x007F) ? idx : (*m_pFontCharset)(idx);
		assert((idx > 0 && pOut[cntFont] > 0) && "対応していない文字が検出されました");
	}
	return pOut;
}

void FontString::Create(const c8* pCharsetBinaryData, s32 binarySize)
{
	assert(!s_pImpl && "FontString::Create() is called twice!");
	s_pImpl = new Impl(pCharsetBinaryData, binarySize);
}
void FontString::Destroy()
{
	assert(s_pImpl && "FontString::Destroy() is called twice!");
	// @attention: deleteをheapに置き換える.
	delete s_pImpl;
	s_pImpl = NULL;
}

c16* FontString::ConvertUTF16(c16* pOut, const c16* wstr)
{
	return s_pImpl->convertUTF16(pOut, wstr);
}

}}