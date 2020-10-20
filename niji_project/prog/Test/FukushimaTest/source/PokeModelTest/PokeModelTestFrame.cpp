#if PM_DEBUG

//======================================================================
/**
 * @file PokeModelTestFrame.cpp
 * @date 2015/09/07 11:21:52
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <Test/FukushimaTest/source/PokeModelTest/PokeModelTestFrame.h>
#include <Test/FukushimaTest/source/PokeModelTest/PokeModelTestDraw.h>

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <Print/include/WordSet.h>
// gflib2
#include <str/include/gfl2_Str.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

// poke_3d_libのインクルード
#include <model/include/gfl2_BaseCamera.h>

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
PokeModelTestFrame::PokeModelTestFrame( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pPokeModelDraw( NULL )
  , m_pArcReadBuff( NULL )
  , m_ReadSeq( LOCAL_OPEN_START )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pCameraDrawEnvNode( NULL )
  , m_pCamera( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//------------------------------------------------------------------------------
PokeModelTestFrame::~PokeModelTestFrame( void )
{

}


//------------------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------------------
void PokeModelTestFrame::Setup( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
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
applib::frame::Result PokeModelTestFrame::InitFunc( void )
{
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

      // カメラ
      app::util::AppRenderingManager::CreateDefaultCamera(m_pAppHeap, &m_pCameraDrawEnvNode, &m_pCamera);
      m_pCamera->SetAspectRatio( 320.0f / 240.0f );
      m_pAppRenderingManager->SetCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pCamera );

      m_pPokeModelDraw = GFL_NEW(devHeap) PokeModelTestDraw( m_pAppHeap, m_pAppRenderingManager );
      m_pPokeModelDraw->Init( m_pArcReadBuff, m_pMsgData, m_pWordSet );
      m_pPokeModelDraw->SetTitleViewUIListener( this );

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
applib::frame::Result PokeModelTestFrame::UpdateFunc( void )
{
  if( m_pPokeModelDraw ) m_pPokeModelDraw->UpdateTree();

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
void PokeModelTestFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pPokeModelDraw ) m_pPokeModelDraw->DrawTree( no );
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result PokeModelTestFrame::EndFunc( void )
{
  if( m_pPokeModelDraw )
  {
    if( !m_pPokeModelDraw->End() )
    {
      return applib::frame::RES_CONTINUE;
    }

    GFL_DELETE m_pPokeModelDraw;
  }

  m_pAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY );

  GFL_SAFE_DELETE( m_pCameraDrawEnvNode );
  GFL_SAFE_DELETE( m_pCamera );

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
void PokeModelTestFrame::OnEnd( void )
{
  SetSubSeq( MM_SEQ_END );
}


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG