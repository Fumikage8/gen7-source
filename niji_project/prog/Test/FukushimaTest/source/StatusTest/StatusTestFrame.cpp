#if PM_DEBUG

//==============================================================================
/**
 * @file    StatusTestFrame.cpp
 * @author  fukushima_yuya
 * @date    2015.06.26
 * @brief   ステータス画面フレーム
 */
//==============================================================================

// niji
#include "Test/FukushimaTest/source/StatusTest/StatusTestFrame.h"
#include "Test/FukushimaTest/source/StatusTest/Lower/StatusTestLowerDraw.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/field_status.gaix>
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
TestStatusFrame::TestStatusFrame( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pAppParam( NULL )
  , m_pStatusLowerDraw( NULL )
  , m_pArcReadBuff( NULL )
  , m_ReadSeq( LOCAL_OPEN_START )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
TestStatusFrame::~TestStatusFrame( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result TestStatusFrame::InitFunc( void )
{
  GFL_ASSERT( m_pAppParam );

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = ARCID_FIELD_STATUS;
  u32 dataID = GARC_field_status_field_status_applyt_COMP;

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

      //m_pStatusUpper = GFL_NEW(pDevHeap) StatusUpperDraw( m_pAppHeap, m_pAppRenderingManager );
      //m_pStatusUpper->Init( m_pAppParam, m_pArcReadBuff, m_pMsgData, m_pWordSet );

      //m_pStatusLower = GFL_NEW(pDevHeap) StatusLowerDraw( m_pAppHeap, m_pAppRenderingManager );
      //m_pStatusLower->Init( m_pAppParam, m_pArcReadBuff, m_pMsgData, m_pWordSet );
      //m_pStatusLower->SetTitleViewUIListener( this );

      //m_pStatusLower->AddSubView( m_pStatusUpper );

      m_pStatusLowerDraw = GFL_NEW( devHeap ) TestStatusLowerDraw( m_pAppHeap, m_pAppRenderingManager );
      m_pStatusLowerDraw->Init( m_pAppParam, m_pArcReadBuff, m_pMsgData, m_pWordSet );
      m_pStatusLowerDraw->SetTitleViewUIListener( this );

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
applib::frame::Result TestStatusFrame::UpdateFunc( void )
{
  if( m_pStatusLowerDraw ) m_pStatusLowerDraw->UpdateTree();

  applib::frame::Result result;

  switch( GetSubSeq() )
  {
  case MM_SEQ_END:
    {
      //m_pAppParam->status_end = true;
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
void TestStatusFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pStatusLowerDraw ) m_pStatusLowerDraw->DrawTree( no );
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result TestStatusFrame::EndFunc( void )
{
  if( m_pStatusLowerDraw )
  {
    if( !m_pStatusLowerDraw->End() )
    {
      return applib::frame::RES_CONTINUE;
    }

    GFL_DELETE m_pStatusLowerDraw;
  }

  //if( m_pStatusUpper )
  //{
  //  if( !m_pStatusUpper->End() )
  //  {
  //    return applib::frame::RES_CONTINUE;
  //  }

  //  GFL_DELETE m_pStatusUpper;
  //}

  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
  GflHeapFreeMemory( m_pArcReadBuff );

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------------------
void TestStatusFrame::Setup( App::Status::APP_PARAM* pAppParam )
{
  m_pAppParam = pAppParam;

  //m_pAppHeap = m_pAppParam->mpAppHeap;
  //m_pAppRenderingManager = m_pAppParam->mpAppRenderingManager;
}




//==============================================================================
/**
 *      ↓　以下がリスナー　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   ステータス画面の終了通知
 */
//------------------------------------------------------------------------------
void TestStatusFrame::EndStatus( void )
{
  SetSubSeq( MM_SEQ_END );
}


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG