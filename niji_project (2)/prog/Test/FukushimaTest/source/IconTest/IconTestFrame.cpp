#if PM_DEBUG

//======================================================================
/**
 * @file IconTestFrame.cpp
 * @date 2015/07/17 16:41:17
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "Test/FukushimaTest/source/IconTest/IconTestFrame.h"
#include "Test/FukushimaTest/source/IconTest/IconTestDraw.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <Print/include/WordSet.h>

// gflib2
#include <str/include/gfl2_Str.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/common_pokelist.gaix>
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum{
    MM_SEQ_NONE,
    MM_SEQ_END
  };

  enum{
    LOCAL_READY,
    LOCAL_OPEN_START,
    LOCAL_READ_START,
    LOCAL_READ_WAIT,
    LOCAL_CLOSE_WAIT
  };
}

//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
IconTestFrame::IconTestFrame( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pIconDraw( NULL )
  , m_pArcReadBuff( NULL )
  , m_ReadSeq( LOCAL_OPEN_START )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//------------------------------------------------------------------------------
IconTestFrame::~IconTestFrame( void )
{

}


//------------------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------------------
void IconTestFrame::Setup( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
{
  m_pAppHeap = pHeap;
  m_pAppRenderingManager = pAppRenderingManager;
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result IconTestFrame::InitFunc( void )
{
  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = ARCID_CMN_POKELIST;
  u32 dataID = GARC_common_pokeList_common_pokeList_applyt_COMP;

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

      // メッセージデータの生成
      m_pMsgData = GFL_NEW(devHeap) gfl2::str::MsgData(
        print::GetMessageArcId(), GARC_message_pokemon_status_DAT, devHeap, gfl2::str::MsgData::LOAD_FULL );

      // ワードセットの生成
      m_pWordSet = GFL_NEW(devHeap) print::WordSet( 16, 64, devHeap );

      m_pIconDraw = GFL_NEW(devHeap) IconTestDraw( m_pAppHeap, m_pAppRenderingManager );
      m_pIconDraw->Init( m_pArcReadBuff, m_pMsgData, m_pWordSet );
      m_pIconDraw->SetTitleViewUIListener( this );

      app::util::FileAccessor::FileClose( arcID, devHeap );

      m_ReadSeq = LOCAL_CLOSE_WAIT;
    }
    //break;  //!< no break

  case LOCAL_CLOSE_WAIT:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_ReadSeq = 0;

      return applib::frame::RES_FINISH;
    }
    //break;  //!< no break
  }

  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result IconTestFrame::UpdateFunc( void )
{
  if( m_pIconDraw ) m_pIconDraw->UpdateTree();

  applib::frame::Result result;

  switch( GetSubSeq() )
  {
  case MM_SEQ_END:
    {
      result = applib::frame::RES_FINISH;
    } break;

  default:
    {
      result = applib::frame::RES_CONTINUE;
    }
  }

  return result;
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void IconTestFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pIconDraw ) m_pIconDraw->DrawTree( no );
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result IconTestFrame::EndFunc( void )
{
  if( m_pIconDraw )
  {
    if( !m_pIconDraw->End() )
    {
      return applib::frame::RES_CONTINUE;
    }

    GFL_DELETE m_pIconDraw;
  }

  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
  GflHeapFreeMemory( m_pArcReadBuff );

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief	  終了通知
 */
//------------------------------------------------------------------------------
void IconTestFrame::OnEnd( void )
{
  SetSubSeq( MM_SEQ_END );
}


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG