//==============================================================================
/**
 @file    app_util_LytFontListAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.14
 @brief   gfl_lyt_font_list_maker.rbが生成するfont_listファイルのアクセッサー 
 */
//==============================================================================


// nijiのインクルード
#include <AppLib/include/Util/app_util_LytFontListAccessor.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


LytFontListAccessor::LytFontListAccessor(void)
  : m_buf(NULL),
    m_version(0),
    m_bflytInfoNum(0),
    m_bflytInfoStartBytePosTable(NULL),
    m_lytDatFontNum(0),
    m_lytDatFontIdTable(NULL)
{}
LytFontListAccessor::~LytFontListAccessor()
{}

void LytFontListAccessor::SetFontListBuf(const void* buf)
{
  if(buf == NULL)
  {
    GFL_ASSERT(0);
    return;
  }

  m_buf = static_cast<const u8*>(buf);

  if(m_buf)
  {
    u32 byteCount = 0;

    // シグネチャチェックしない
    byteCount += 4;

    // バージョン
    m_version = *(reinterpret_cast<const u16*>(&m_buf[byteCount]));  // バージョンチェックしない(0しかないので)
    byteCount += 2;

    // lytdatに収録されているレイアウトbflytInfoの個数
    m_bflytInfoNum = *(reinterpret_cast<const u16*>(&m_buf[byteCount]));
    byteCount += 2;

    // bflytInfoの開始バイト位置のテーブル
    m_bflytInfoStartBytePosTable = reinterpret_cast<const u32*>(&m_buf[byteCount]);
    byteCount += 4 * m_bflytInfoNum;

    // lytdatに収録されているレイアウト全体で使っているフォントの個数
    m_lytDatFontNum = *(reinterpret_cast<const u16*>(&m_buf[byteCount]));
    byteCount += 2;

    // フォント番号のテーブル
    if(m_lytDatFontNum > 0)
    {
      m_lytDatFontIdTable = reinterpret_cast<const u16*>(&m_buf[byteCount]);
    }
  }
  GFL_ASSERT(m_buf);
}

void LytFontListAccessor::UnsetFontListBuf(void)
{
  m_buf = NULL;
  m_version = 0;
  m_bflytInfoNum = 0;
  m_bflytInfoStartBytePosTable = NULL;
  m_lytDatFontNum = 0;
  m_lytDatFontIdTable = NULL;
}

u32 LytFontListAccessor::GetLytDatFontNum(void) const
{
  return m_lytDatFontNum;
}
const u16* LytFontListAccessor::GetLytDatFontIdTable(void) const
{
  return m_lytDatFontIdTable;
}

bool LytFontListAccessor::DoesLytExist(u32 lytresId) const
{
  const u8* bflytInfoBuf = getBflytInfoBuf(lytresId);
  return (bflytInfoBuf != NULL);
}

u32 LytFontListAccessor::GetFontNum(u32 lytresId) const
{
  const u8* bflytInfoBuf = getBflytInfoBuf(lytresId);
  if(bflytInfoBuf)
  {
    u32 byteCount = 2;
    u32 fontNum = *(reinterpret_cast<const u16*>(&bflytInfoBuf[byteCount]));
    return fontNum;
  }
  return 0;
}

const u16* LytFontListAccessor::GetFontIdTable(u32 lytresId) const
{
  const u8* bflytInfoBuf = getBflytInfoBuf(lytresId);
  if(bflytInfoBuf)
  {
    u32 byteCount = 2+2;
    const u16* fontIdTable = reinterpret_cast<const u16*>(&bflytInfoBuf[byteCount]);
    return fontIdTable;
  }
  return NULL;
}


const u8* LytFontListAccessor::getBflytInfoBuf(u32 lytresId) const
{
  if(m_buf)
  {
    for(u32 i=0; i<m_bflytInfoNum; ++i)
    {
      const u8* bflytInfoBuf = &m_buf[ m_bflytInfoStartBytePosTable[i] ];
      
      u32 l_lytresId = *(reinterpret_cast<const u16*>(&bflytInfoBuf[0]));
      if(l_lytresId == lytresId) return bflytInfoBuf;
    }
  }
  GFL_ASSERT(m_buf);
  return NULL;
}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

