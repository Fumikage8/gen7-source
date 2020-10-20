#if PM_DEBUG

//======================================================================
/**
 * @file    DynamicHeaderTestFrame.cpp
 * @date    2015/10/06 13:14:14
 * @author  fukushima_yuya
 * @brief   簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <Test/FukushimaTest/source/DynamicHeaderTest/DynamicHeaderTestFrame.h>
#include <Test/FukushimaTest/source/DynamicHeaderTest/DynamicHeaderTestDraw.h>

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <System/include/ArcIdGetter.h>

// arc
#include <arc_index/lang_select.gaix>


// DynamicHeader
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

#define NIJI_LANG_SELECT_MSG_HEADER_FILE_PATH "arc_index/message.gaix"
#define NIJI_LANG_SELECT_ARC_HEADER_FILE_PATH "arc_def_index/arc_def.h"

#define NIJI_LANG_SELECT_MSG_TABLE_CAP_NUM 32
#define NIJI_LANG_SELECT_ARC_TABLE_CAP_NUM 2048

#include GFL_DYNAMIC_HEADER(NIJI_LANG_SELECT_MSG_HEADER_FILE_PATH)
#include GFL_DYNAMIC_HEADER(NIJI_LANG_SELECT_ARC_HEADER_FILE_PATH)



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

namespace
{
  enum{
    MM_SEQ_MAIN,
    MM_SEQ_END
  };

  enum{
    LOCAL_OPEN_START,
    LOCAL_READ_START,
    LOCAL_READ_WAIT,
    LOCAL_CLOSE_WAIT
  };
}


DynamicHeaderTestFrame::DynamicHeaderTestFrame()
{

}

DynamicHeaderTestFrame::~DynamicHeaderTestFrame()
{

}


applib::frame::Result DynamicHeaderTestFrame::InitFunc( void )
{
  if( m_InitSeq == 0 )
  {
    GFL_LOAD_HEADER_HASH( NIJI_LANG_SELECT_MSG_HEADER_FILE_PATH, NIJI_LANG_SELECT_MSG_TABLE_CAP_NUM );
    GFL_LOAD_HEADER_HASH( NIJI_LANG_SELECT_ARC_HEADER_FILE_PATH, NIJI_LANG_SELECT_ARC_TABLE_CAP_NUM );
    m_InitSeq++;
  }

  // メッセージデータの読み込み
  if( !LoadMsgData() ) return applib::frame::RES_CONTINUE;

  // レイアウトデータの読み込み
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  
  // Drawの生成
  m_pDraw = GFL_NEW(devHeap) DynamicHeaderTestDraw( m_pAppHeap, m_pAppRenderingManager );
  m_pDraw->Init( m_pArcReadBuff, m_pMsgReadBuff );
  m_pDraw->SetTitleViewUIListener( this );

  return applib::frame::RES_FINISH;
}

applib::frame::Result DynamicHeaderTestFrame::EndFunc( void )
{
  if( m_pDraw )
  {
    if( !m_pDraw->End() ) return applib::frame::RES_CONTINUE;
    GFL_SAFE_DELETE( m_pDraw );
  }

  GflHeapFreeMemory( m_pArcReadBuff );

  GflHeapFreeMemory( m_pMsgReadBuff );

  GFL_UNLOAD_HEADER_HASH( NIJI_LANG_SELECT_MSG_HEADER_FILE_PATH );
  GFL_UNLOAD_HEADER_HASH( NIJI_LANG_SELECT_ARC_HEADER_FILE_PATH );

  return applib::frame::RES_FINISH;
}

applib::frame::Result DynamicHeaderTestFrame::UpdateFunc( void )
{
  if( m_pDraw ) m_pDraw->Update();

  if( GetSubSeq() == MM_SEQ_END )
  {
    return applib::frame::RES_FINISH;;
  }

  return applib::frame::RES_CONTINUE;
}

void DynamicHeaderTestFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pDraw ) m_pDraw->DrawTree( no );
}

bool DynamicHeaderTestFrame::LoadLayoutData( void )
{
  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = NIJI_ARCID_DEFINE_IN_HEADER(ARCID_LANG_SELECT);
  u32 dataID = GARC_lang_select_lang_select_applyt_COMP;

  switch( m_ReadSeq )
  {
  case LOCAL_OPEN_START:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_ReadSeq = LOCAL_READ_START;
    }
    //break;  //!< no break

  case LOCAL_READ_START:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pArcReadBuff,
        devHeap,
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      m_ReadSeq = LOCAL_READ_WAIT;
    }
    //break;  //!< no break

  case LOCAL_READ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pArcReadBuff ) ) break;

      app::util::FileAccessor::FileClose( arcID, devHeap );
      m_ReadSeq = LOCAL_CLOSE_WAIT;
    }
    //break;  //!< no break

  case LOCAL_CLOSE_WAIT:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_ReadSeq = 0;

      return true;
    }
    //break;  //!< no break
  }

  return false;
}

bool DynamicHeaderTestFrame::LoadMsgData( void )
{
  if( m_InitSeq >= 2 ) return true;

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = print::GetMessageArcId();
  u32 dataID = GFL_VALUE_DEFINE_IN_HEADER(GARC_message_language_select_DAT);

  switch( m_ReadSeq )
  {
  case LOCAL_OPEN_START:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_ReadSeq = LOCAL_READ_START;
    }
    //break;  //!< no break

  case LOCAL_READ_START:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pMsgReadBuff,
        devHeap,
        false,
        4 );

      m_ReadSeq = LOCAL_READ_WAIT;
    }
    //break;  //!< no break

  case LOCAL_READ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pMsgReadBuff ) ) break;

      // Drawの生成

      app::util::FileAccessor::FileClose( arcID, devHeap );
      m_ReadSeq = LOCAL_CLOSE_WAIT;
    }
    //break;  //!< no break

  case LOCAL_CLOSE_WAIT:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_ReadSeq = 0;
      m_InitSeq++;

      return true;
    }
    //break;  //!< no break
  }

  return false;
}
void DynamicHeaderTestFrame::Setup( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager )
{
  m_pAppHeap = pHeap;
  m_pAppRenderingManager = pRenderingManager;
}

void DynamicHeaderTestFrame::EndLangSelect( void )
{
  SetSubSeq( MM_SEQ_END );
}


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG