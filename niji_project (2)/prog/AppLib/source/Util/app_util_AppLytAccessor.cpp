//==============================================================================
/**
 @file    app_util_AppLytAccessor.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.01.16
 @brief   applytファイルのアクセッサー 
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_AppLytAccessor.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


AppLytAccessor::AppLytAccessor(void)
  : m_buf(NULL),
    m_infoTableAccessor(),
    m_mstxtListAccessor(),
    m_fontListAccessor(),
    m_version(0),
    m_infoTableBlockBuf(NULL),
    m_mstxtListBlockBuf(NULL),
    m_fontListBlockBuf(NULL),
    m_lytdatBlockBuf(NULL)
{
  STATIC_ASSERT( sizeof(u32)==sizeof(gfl2::lyt::LytArcIndex) );
      // u32配列をgfl2::lyt::LytArcIndex配列として使っているので、サイズが変わっていないことをチェック。
}
AppLytAccessor::~AppLytAccessor()
{}

void AppLytAccessor::SetAppLytBuf(const void* buf)
{
  if(buf == NULL)
  {
    GFL_ASSERT(0);
    return;
  }

  // デバイスメモリチェックはしない。VRAMのこともあり得るから。
  // nijiではほとんどデバイスメモリになるので、そうでないことはほぼあり得ないから。

  // アラインメントチェック
  {
    u32 addr = reinterpret_cast<u32>(buf);
    if(addr % APPLYT_ALIGN != 0)
    {
      GFL_ASSERT_MSG(0, "applytアラインメントエラー\n");  // 開発中に気付かせるためのASSERT。
      return;
    }
  }

  m_buf = static_cast<const u8*>(buf);
  
  u32 byteCount = 0;

  // シグネチャチェックしない
  byteCount += 4;   

  // バージョン
  m_version = *(reinterpret_cast<const u16*>(&m_buf[byteCount]));
  byteCount += 2;

  if(m_version<0  || 2<m_version)
  {
    GFL_ASSERT(0);  // サポートされていないバージョン。開発中に気付かせるためのASSERT。
  }

  // ブロック(バージョンによって異なる)
  if(m_version == 0)
  {
    m_infoTableBlockBuf = getBlockBuf(0);
    m_lytdatBlockBuf    = getBlockBuf(1);
  }
  else if(m_version == 1)
  {
    m_infoTableBlockBuf = getBlockBuf(0);
    m_mstxtListBlockBuf = getBlockBuf(1);
    m_lytdatBlockBuf    = getBlockBuf(2);
  }
  else if(m_version == 2)
  {
    m_infoTableBlockBuf = getBlockBuf(0);
    m_mstxtListBlockBuf = getBlockBuf(1);
    m_fontListBlockBuf  = getBlockBuf(2);
    m_lytdatBlockBuf    = getBlockBuf(3);
  }

  // info_table
  m_infoTableAccessor.SetInfoTableBuf(m_infoTableBlockBuf);

  // mstxt_list
  if(m_mstxtListBlockBuf) m_mstxtListAccessor.SetMstxtListBuf(m_mstxtListBlockBuf);

  // font_list
  if(m_fontListBlockBuf) m_fontListAccessor.SetFontListBuf(m_fontListBlockBuf);
}

void AppLytAccessor::UnsetAppLytBuf(void)
{
  m_buf = NULL;
  m_infoTableAccessor.UnsetInfoTableBuf();
  m_mstxtListAccessor.UnsetMstxtListBuf();
  m_fontListAccessor.UnsetFontListBuf();
}

const void* AppLytAccessor::GetLytInfoTable(void) const
{
  return m_infoTableBlockBuf;
}
const void* AppLytAccessor::GetLytMstxtList(void) const
{
  return m_mstxtListBlockBuf;
}
const void* AppLytAccessor::GetLytFontList(void) const
{
  return m_fontListBlockBuf;
}
const void* AppLytAccessor::GetLytDat(void) const
{
  return m_lytdatBlockBuf;
}
  

u32 AppLytAccessor::GetPartsMax( u32 lytresId ) const
{
  return m_infoTableAccessor.GetPartsPaneNum(lytresId);
}

u32 AppLytAccessor::GetAnimeMax( u32 lytresId ) const
{
  return m_infoTableAccessor.GetAnimNum(lytresId);
}

const gfl2::lyt::LytArcIndex * AppLytAccessor::GetAnimeTable( u32 lytresId ) const
{
  return m_infoTableAccessor.GetAnimTable(lytresId);
}

const u16 * AppLytAccessor::GetPartsPaneTable( u32 lytresId ) const
{
  return m_infoTableAccessor.GetPartsPaneTable(lytresId);
}

const u16 * AppLytAccessor::GetPartsLayoutTable( u32 lytresId ) const
{
  return m_infoTableAccessor.GetPartsLytTable(lytresId);
}
  
const u8* AppLytAccessor::getBlockBuf(u32 blockIndex) const
{
  // m_bufとm_versionが設定されている前提
  const u8* blockBuf = NULL;
  if(m_buf)
  {
    u32 pos = *(reinterpret_cast<const u32*>(&m_buf[4+2+2+(4+4)*blockIndex]));
    blockBuf = &m_buf[pos];
  }
  return blockBuf;
}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
