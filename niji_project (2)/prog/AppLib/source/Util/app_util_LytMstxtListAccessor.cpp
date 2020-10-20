//==============================================================================
/**
 @file    app_util_LytMstxtListAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.10
 @brief   gfl_lyt_mstxt_list_maker.rbが生成するmstxt_listファイルのアクセッサー 
 */
//==============================================================================


// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_LytMstxtListAccessor.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


LytMstxtListAccessor::LytMstxtListAccessor(void)
  : m_buf(NULL),
    m_version(0),
    m_bflytInfoNum(0),
    m_bflytInfoStartBytePosTable(NULL),
    m_lytDatMessageInfoNum(0),
    m_lytDatMessageInfoStartBytePosTable(NULL)
{}
LytMstxtListAccessor::~LytMstxtListAccessor()
{}

void LytMstxtListAccessor::SetMstxtListBuf(const void* buf)
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

    // lytdatに収録されているレイアウト全体で使っているメッセージ情報の個数
    m_lytDatMessageInfoNum = *(reinterpret_cast<const u32*>(&m_buf[byteCount]));
    byteCount += 4;

    // メッセージ情報の開始バイト位置のテーブル
    if(m_lytDatMessageInfoNum > 0)
    {
      m_lytDatMessageInfoStartBytePosTable = reinterpret_cast<const u32*>(&m_buf[byteCount]);
    }
  }
  GFL_ASSERT(m_buf);
}

void LytMstxtListAccessor::UnsetMstxtListBuf(void)
{
  m_buf = NULL;
  m_version = 0;
  m_bflytInfoNum = 0;
  m_bflytInfoStartBytePosTable = NULL;
  m_lytDatMessageInfoNum = 0;
  m_lytDatMessageInfoStartBytePosTable = NULL;
}

bool LytMstxtListAccessor::DoesLytExist(u32 lytresId) const
{
  const u8* bflytInfoBuf = getBflytInfoBuf(lytresId);
  return (bflytInfoBuf != NULL);
}

u32 LytMstxtListAccessor::GetMessageArcNum(u32 lytresId) const
{
  const u8*  bflytInfoBuf                 = NULL;
  u32        messageInfoNum               = 0;
  const u32* messageInfoStartBytePosTable = NULL;
  getBflytMessageInfo(lytresId, &bflytInfoBuf, &messageInfoNum, &messageInfoStartBytePosTable);
  return messageInfoNum;
}
u32 LytMstxtListAccessor::GetMessageArcId(u32 lytresId, u32 index) const
{
  const u8* messageInfoBuf = getMessageInfoBufByIndex(lytresId, index);
  if(messageInfoBuf)
  {
    u32 arcId = *(reinterpret_cast<const u16*>(&messageInfoBuf[0]));
    return arcId;
  }
  return gfl2::fs::ArcFile::ARCID_NULL;
}
u32 LytMstxtListAccessor::GetMessageArcDatNumByIndex(u32 lytresId, u32 index) const
{
  const u8* messageInfoBuf = getMessageInfoBufByIndex(lytresId, index);
  u32        arcDatNum     = 0;
  const u16* arcDatIdTable = NULL;
  getMessageArcDatNumAndArcDatIdTableStatic(messageInfoBuf, &arcDatNum, &arcDatIdTable);
  return arcDatNum;
}
u32 LytMstxtListAccessor::GetMessageArcDatNumByArcId(u32 lytresId, u32 arcId) const
{
  const u8* messageInfoBuf = getMessageInfoBufByArcId(lytresId, arcId);
  u32        arcDatNum     = 0;
  const u16* arcDatIdTable = NULL;
  getMessageArcDatNumAndArcDatIdTableStatic(messageInfoBuf, &arcDatNum, &arcDatIdTable);
  return arcDatNum;
}
const u16* LytMstxtListAccessor::GetMessageArcDatIdTableByIndex(u32 lytresId, u32 index) const
{
  const u8* messageInfoBuf = getMessageInfoBufByIndex(lytresId, index);
  u32        arcDatNum     = 0;
  const u16* arcDatIdTable = NULL;
  getMessageArcDatNumAndArcDatIdTableStatic(messageInfoBuf, &arcDatNum, &arcDatIdTable);
  return arcDatIdTable;
}
const u16* LytMstxtListAccessor::GetMessageArcDatIdTableByArcId(u32 lytresId, u32 arcId) const
{
  const u8* messageInfoBuf = getMessageInfoBufByArcId(lytresId, arcId);
  u32        arcDatNum     = 0;
  const u16* arcDatIdTable = NULL;
  getMessageArcDatNumAndArcDatIdTableStatic(messageInfoBuf, &arcDatNum, &arcDatIdTable);
  return arcDatIdTable;
}

u32 LytMstxtListAccessor::GetTextBoxPaneNum(u32 lytresId) const
{
  u32                    textBoxPaneNum       = 0;
  const TextBoxPaneInfo* textBoxPaneInfoTable = NULL;
  getTextBoxPaneNumAndTextBoxPaneInfoTable(lytresId, &textBoxPaneNum, &textBoxPaneInfoTable);
  return textBoxPaneNum;
}
const LytMstxtListAccessor::TextBoxPaneInfo* LytMstxtListAccessor::GetTextBoxPaneInfoTable(u32 lytresId) const
{
  u32                    textBoxPaneNum       = 0;
  const TextBoxPaneInfo* textBoxPaneInfoTable = NULL;
  getTextBoxPaneNumAndTextBoxPaneInfoTable(lytresId, &textBoxPaneNum, &textBoxPaneInfoTable);
  return textBoxPaneInfoTable;
}

u32 LytMstxtListAccessor::GetLytDatMessageArcNum(void) const
{
  return m_lytDatMessageInfoNum;
}
u32 LytMstxtListAccessor::GetLytDatMessageArcId(u32 index) const
{
  const u8* messageInfoBuf = getLytDatMessageInfoBufByIndex(index);
  if(messageInfoBuf)
  {
    u32 arcId = *(reinterpret_cast<const u16*>(&messageInfoBuf[0]));
    return arcId;
  }
  return gfl2::fs::ArcFile::ARCID_NULL;
}
u32 LytMstxtListAccessor::GetLytDatMessageArcDatNumByIndex(u32 index) const
{
  const u8* messageInfoBuf = getLytDatMessageInfoBufByIndex(index);
  u32        arcDatNum     = 0;
  const u16* arcDatIdTable = NULL;
  getMessageArcDatNumAndArcDatIdTableStatic(messageInfoBuf, &arcDatNum, &arcDatIdTable);
  return arcDatNum;
}
u32 LytMstxtListAccessor::GetLytDatMessageArcDatNumByArcId(u32 arcId) const
{
  const u8* messageInfoBuf = getLytDatMessageInfoBufByArcId(arcId);
  u32        arcDatNum     = 0;
  const u16* arcDatIdTable = NULL;
  getMessageArcDatNumAndArcDatIdTableStatic(messageInfoBuf, &arcDatNum, &arcDatIdTable);
  return arcDatNum;
}
const u16* LytMstxtListAccessor::GetLytDatMessageArcDatIdTableByIndex(u32 index) const
{
  const u8* messageInfoBuf = getLytDatMessageInfoBufByIndex(index);
  u32        arcDatNum     = 0;
  const u16* arcDatIdTable = NULL;
  getMessageArcDatNumAndArcDatIdTableStatic(messageInfoBuf, &arcDatNum, &arcDatIdTable);
  return arcDatIdTable;
}
const u16* LytMstxtListAccessor::GetLytDatMessageArcDatIdTableByArcId(u32 arcId) const
{
  const u8* messageInfoBuf = getLytDatMessageInfoBufByArcId(arcId);
  u32        arcDatNum     = 0;
  const u16* arcDatIdTable = NULL;
  getMessageArcDatNumAndArcDatIdTableStatic(messageInfoBuf, &arcDatNum, &arcDatIdTable);
  return arcDatIdTable;
}

////////////////////////////////////////////////////////////////////////////////
const u8* LytMstxtListAccessor::getBflytInfoBuf(u32 lytresId) const
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
const u8* LytMstxtListAccessor::getMessageInfoBufByIndex(u32 lytresId, u32 index) const
{
  const u8*  bflytInfoBuf                 = NULL;
  u32        messageInfoNum               = 0;
  const u32* messageInfoStartBytePosTable = NULL;
  getBflytMessageInfo(lytresId, &bflytInfoBuf, &messageInfoNum, &messageInfoStartBytePosTable);
  return getMessageInfoBufByIndexStatic(bflytInfoBuf, messageInfoNum, messageInfoStartBytePosTable, index);
}
const u8* LytMstxtListAccessor::getMessageInfoBufByArcId(u32 lytresId, u32 arcId) const
{
  const u8*  bflytInfoBuf                 = NULL;
  u32        messageInfoNum               = 0;
  const u32* messageInfoStartBytePosTable = NULL;
  getBflytMessageInfo(lytresId, &bflytInfoBuf, &messageInfoNum, &messageInfoStartBytePosTable);
  return getMessageInfoBufByArcIdStatic(bflytInfoBuf, messageInfoNum, messageInfoStartBytePosTable, arcId);
}
void LytMstxtListAccessor::getBflytMessageInfo(u32 lytresId, const u8** bflytInfoBuf, u32* messageInfoNum, const u32** messageInfoStartBytePosTable) const
{
  const u8*  l_bflytInfoBuf                 = NULL;
  u32        l_messageInfoNum               = 0;
  const u32* l_messageInfoStartBytePosTable = NULL;

  l_bflytInfoBuf = getBflytInfoBuf(lytresId);
  if(l_bflytInfoBuf)
  {
    u32 byteCount = 2;

    // レイアウトが使っているメッセージ情報の個数
    l_messageInfoNum = *(reinterpret_cast<const u16*>(&l_bflytInfoBuf[byteCount]));
    byteCount += 2;

    // メッセージ情報の開始バイト位置のテーブル(l_bflytInfoBufの先頭からの位置)
    if(l_messageInfoNum > 0)
    {
      l_messageInfoStartBytePosTable = reinterpret_cast<const u32*>(&l_bflytInfoBuf[byteCount]);
    }
  }

  *bflytInfoBuf                 = l_bflytInfoBuf;
  *messageInfoNum               = l_messageInfoNum;
  *messageInfoStartBytePosTable = l_messageInfoStartBytePosTable;
}
void  LytMstxtListAccessor::getTextBoxPaneNumAndTextBoxPaneInfoTable(u32 lytresId, u32* textBoxPaneNum, const TextBoxPaneInfo** textBoxPaneInfoTable) const
{
  u32                    l_textBoxPaneNum       = 0;
  const TextBoxPaneInfo* l_textBoxPaneInfoTable = NULL;

  const u8* bflytInfoBuf = getBflytInfoBuf(lytresId);
  if(bflytInfoBuf)
  {
    u32 byteCount = 2;
    u32 arcNum = *(reinterpret_cast<const u16*>(&bflytInfoBuf[byteCount]));
    byteCount += 2;

    byteCount += 4*arcNum;  // メッセージの開始バイト位置の分進める
   
    l_textBoxPaneNum = *(reinterpret_cast<const u32*>(&bflytInfoBuf[byteCount]));
    byteCount += 4;
    
    l_textBoxPaneInfoTable = reinterpret_cast<const TextBoxPaneInfo*>(&bflytInfoBuf[byteCount]);
  }

  *textBoxPaneNum       = l_textBoxPaneNum;
  *textBoxPaneInfoTable = l_textBoxPaneInfoTable;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
const u8* LytMstxtListAccessor::getLytDatMessageInfoBufByIndex(u32 index) const
{
  return getMessageInfoBufByIndexStatic(m_buf, m_lytDatMessageInfoNum, m_lytDatMessageInfoStartBytePosTable, index);
}
const u8* LytMstxtListAccessor::getLytDatMessageInfoBufByArcId(u32 arcId) const
{
  return getMessageInfoBufByArcIdStatic(m_buf, m_lytDatMessageInfoNum, m_lytDatMessageInfoStartBytePosTable, arcId);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
const u8* LytMstxtListAccessor::getMessageInfoBufByIndexStatic(const u8* buf, u32 messageInfoNum, const u32* messageInfoStartBytePosTable, u32 index)
{
  if(buf && index<messageInfoNum)
  {
    const u8* messageInfoBuf = &buf[ messageInfoStartBytePosTable[index] ];
    return messageInfoBuf;
  }
  else
  {
    GFL_ASSERT(0);  // bufがNULL、indexがオーバーしている
  }
  return NULL;
}
const u8* LytMstxtListAccessor::getMessageInfoBufByArcIdStatic(const u8* buf, u32 messageInfoNum, const u32* messageInfoStartBytePosTable, u32 arcId)
{
  if(buf)
  {
    for(u32 i=0; i<messageInfoNum; ++i)
    {
      const u8* messageInfoBuf = &buf[ messageInfoStartBytePosTable[i] ];
      
      u32 l_arcId = *(reinterpret_cast<const u16*>(&messageInfoBuf[0]));
      if(l_arcId == arcId) return messageInfoBuf;
    }
  }
  GFL_ASSERT(buf);
  return NULL;
}
void LytMstxtListAccessor::getMessageArcDatNumAndArcDatIdTableStatic(const u8* messageInfoBuf, u32* arcDatNum, const u16** arcDatIdTable)
{
  u32        l_arcDatNum     = 0;
  const u16* l_arcDatIdTable = NULL;

  if(messageInfoBuf)
  {
    u32 byteCount = 2;
    l_arcDatNum = *(reinterpret_cast<const u16*>(&messageInfoBuf[byteCount]));
    byteCount += 2;
    if(l_arcDatNum > 0)
    {
      l_arcDatIdTable = reinterpret_cast<const u16*>(&messageInfoBuf[byteCount]);
    }
  }

  *arcDatNum     = l_arcDatNum;
  *arcDatIdTable = l_arcDatIdTable;
}
////////////////////////////////////////////////////////////////////////////////





GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

