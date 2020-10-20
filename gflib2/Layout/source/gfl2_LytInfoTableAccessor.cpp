//==============================================================================
/**
 @file    gfl2_LytInfoTableAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.02.27
 @brief   gfl_lyt_table_maker.rbが生成するlytdatの情報テーブルのファイルのアクセッサー 
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_LytInfoTableAccessor.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)


LytInfoTableAccessor::LytInfoTableAccessor(void)
  : m_buf(NULL),
    m_version(0),
    m_bflytInfoNum(0),
    m_bflytInfoStartBytePosTable(NULL)
{}
LytInfoTableAccessor::~LytInfoTableAccessor()
{}

void LytInfoTableAccessor::SetInfoTableBuf(const void* buf)
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
  }
  GFL_ASSERT(m_buf);
}

void LytInfoTableAccessor::UnsetInfoTableBuf(void)
{
  m_buf = NULL;
  m_version = 0;
  m_bflytInfoNum = 0;
  m_bflytInfoStartBytePosTable = NULL;
}

bool LytInfoTableAccessor::DoesLytExist(u32 lytresId) const
{
  const u8* bflytInfoBuf = getBflytInfoBuf(lytresId);
  return (bflytInfoBuf != NULL);
}

u32 LytInfoTableAccessor::GetPartsPaneNum(u32 lytresId) const
{
  const u8* bflytInfoBuf = getBflytInfoBuf(lytresId);
  if(bflytInfoBuf)
  {
    u32 byteCount = 2;
    u32 paneNum = *(reinterpret_cast<const u16*>(&bflytInfoBuf[byteCount]));
    return paneNum;
  }
  return 0;
}

u32 LytInfoTableAccessor::GetAnimNum(u32 lytresId) const
{
  const u8* bflytInfoBuf = getBflytInfoBuf(lytresId);
  if(bflytInfoBuf)
  {
    u32 byteCount = 2+2;
    u32 animNum = *(reinterpret_cast<const u16*>(&bflytInfoBuf[byteCount]));
    return animNum;
  }
  return 0;
}

const u16* LytInfoTableAccessor::GetPartsPaneTable(u32 lytresId) const
{
  u32 byteCount = 2+2+2+2;
  return getU16Table(lytresId, byteCount);
}

const u16* LytInfoTableAccessor::GetPartsLytTable(u32 lytresId) const
{
  u32 byteCount = 2+2+2+2+4;
  return getU16Table(lytresId, byteCount);
}

const u32* LytInfoTableAccessor::GetAnimTable(u32 lytresId) const
{
  u32 byteCount = 2+2+2+2+4+4;
  return getU32Table(lytresId, byteCount);
}

const u8* LytInfoTableAccessor::getBflytInfoBuf(u32 lytresId) const
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

const u32* LytInfoTableAccessor::getU32Table(u32 lytresId, u32 byteCountOfStartBytePosInBflytInfoBuf) const
{
  const u8* bflytInfoBuf = getBflytInfoBuf(lytresId);
  if(bflytInfoBuf)
  {
    u32 startBytePos = *(reinterpret_cast<const u32*>(&bflytInfoBuf[byteCountOfStartBytePosInBflytInfoBuf]));
    if(startBytePos > 0)
    {
      const u32* table = reinterpret_cast<const u32*>(&bflytInfoBuf[startBytePos]);
      return table;
    }
  }
  return NULL;
}

const u16* LytInfoTableAccessor::getU16Table(u32 lytresId, u32 byteCountOfStartBytePosInBflytInfoBuf) const
{
  const u8* bflytInfoBuf = getBflytInfoBuf(lytresId);
  if(bflytInfoBuf)
  {
    u32 startBytePos = *(reinterpret_cast<const u32*>(&bflytInfoBuf[byteCountOfStartBytePosInBflytInfoBuf]));
    if(startBytePos > 0)
    {
      const u16* table = reinterpret_cast<const u16*>(&bflytInfoBuf[startBytePos]);
      return table;
    }
  }
  return NULL;
}


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

