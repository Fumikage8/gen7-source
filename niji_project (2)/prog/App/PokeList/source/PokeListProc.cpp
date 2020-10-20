//==============================================================================
/**
 * @file    PokeListProc.cpp
 * @author  fukushima_yuya
 * @date    2015.03.03
 * @brief   ポケモンリスト画面プロック
 */
//==============================================================================


// niji
#include "App/PokeList/include/PokeListProc.h"
#include "App/PokeList/source/PokeListFrame.h"
#include "App/PokeList/source/PokeListJoinFrame.h"
#include "App/PokeList/source/PokeListSimpleFrame.h"
#include "App/PokeList/source/PokeListWazaSodateyaFrame.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <App/Bag/include/BagFrame.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>
#include <System/include/HeapDefine.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
 //------------------------------------------------------------------------------
Proc::Proc( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pFrameManager( NULL )
  , m_pManagerHeap( NULL )
  , m_PokeListAppParam()
  , m_pPokeListContextParam( NULL )
  , m_pFieldFrame( NULL )
  , m_pJoinFrame( NULL )
  , m_pSimpleFrame( NULL )
  , m_pWazaSodateyaFrame( NULL )
  , m_pStatusAppParam( NULL )
  , m_pBagAppParam( NULL )
  , m_pCommonIcon( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
Proc::~Proc( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::InitFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );

  if( m_pAppRenderingManager == NULL )
  {
    //HEAPID_APP  本体に関するものはAPPめもりーから確保します
    gfl2::heap::HeapBase* pAppHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( pDevHeap, SIZE_PROC_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false );

    // アプリ用ヒープの生成
    m_pAppHeap = GFL_NEW(m_pManagerHeap) app::util::Heap();
    m_pAppHeap->LocalHeapCreate( pAppHeap, pDevHeap, SIZE_MY_APP,SIZE_MY_APPDEV );

    // フレームマネージャの生成
    m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
    m_pFrameManager->SetAppFrameListener(this);

    // レンダリングマネージャの生成
    m_pAppRenderingManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();
    {
      app::util::AppRenderingManager::Description desc;
      {
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_EFFECT;
      }

      gfl2::fs::AsyncFileManager * afm = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
      m_pAppRenderingManager->StartAsyncCreate( desc, m_pAppHeap, NULL, afm );
    }

    // 状態異常アイコン
    m_pCommonIcon = GFL_NEW(m_pAppHeap->GetDeviceHeap()) app::tool::AppCommonGrpIconData();
    m_pCommonIcon->Initialize( m_pAppHeap );
  }

  m_pAppRenderingManager->Update();

  if( m_pAppRenderingManager->IsCreated() && m_pCommonIcon->IsReady() )
  {
    // ポケモンリストのフレームの生成
    CreatePokeListFrame();

    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );

  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void Proc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo no )
{
  GFL_UNUSED( pManager );

  m_pAppRenderingManager->Update();

  m_pFrameManager->Draw( no );
  m_pAppRenderingManager->Draw( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 *  1/30フレームの②フレーム目の処理です。
 *  1/60フレーム動作時には、使われません。
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::NextFrameMainFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::EndFunc( gfl2::proc::Manager* pManager )
{
  // 終了処理が完了するまで待つ
  if( m_pFrameManager->End() == true )
  {
    return gfl2::proc::RES_CONTINUE;
  }
  if( !m_pAppRenderingManager->CanDestroy() )
  {
    return gfl2::proc::RES_CONTINUE;
  }

  GFL_UNUSED( pManager );

  m_pCommonIcon->Terminate();
  GFL_SAFE_DELETE( m_pCommonIcon );
  GFL_SAFE_DELETE( m_pAppRenderingManager );
  GFL_SAFE_DELETE( m_pFrameManager );
  GFL_SAFE_DELETE( m_pAppHeap );

  gfl2::heap::Manager::DeleteHeap( m_pManagerHeap );

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief	  EndFuncが完了した直後に呼び出される関数
 */
//------------------------------------------------------------------------------
void Proc::EndFuncFinish( applib::frame::CellSuper* pEndCell )
{
  FUKUSHIMA_PRINT("EndFuncFinish呼び出し\n");

  if( pEndCell == m_pFieldFrame )
  {
    m_pFieldFrame = NULL;
  }
  else if( pEndCell == m_pJoinFrame )
  {
    m_pJoinFrame = NULL;
  }
  else if( pEndCell == m_pSimpleFrame )
  {
    m_pSimpleFrame = NULL;
  }
  else if( pEndCell == m_pWazaSodateyaFrame )
  {
    m_pWazaSodateyaFrame = NULL;
  }
}




//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト画面の生成
 */
//------------------------------------------------------------------------------
void Proc::CreatePokeListFrame( void )
{
  m_PokeListAppParam.mpAppHeap = m_pAppHeap;
  m_PokeListAppParam.mpAppRenderingManager = m_pAppRenderingManager;
  m_PokeListAppParam.mpAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
  m_PokeListAppParam.mpFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
  m_PokeListAppParam.mpCmnIcon = m_pCommonIcon;

  Mode mode = m_pPokeListContextParam->mode;

  switch( mode )
  {
  case MODE_SIMPLE:
  case MODE_SIMPLE2:
  case MODE_BATTLE:
    {
      m_pSimpleFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, PokeListSimpleFrame>( this->GetFrameManager() );
      m_pSimpleFrame->Setup( &m_PokeListAppParam, m_pPokeListContextParam );
    } break;

  case MODE_JOIN:
    {
      m_pJoinFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, PokeListJoinFrame>( this->GetFrameManager() );
      m_pJoinFrame->Setup( &m_PokeListAppParam, m_pPokeListContextParam );
      m_pJoinFrame->SetOutParam( m_pStatusAppParam, NULL );
    } break;

  case MODE_WAZAOSHIE:
  case MODE_SODATEYA:
    {
      m_pWazaSodateyaFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, PokeListWazaSodateyaFrame>( this->GetFrameManager() );
      m_pWazaSodateyaFrame->Setup( &m_PokeListAppParam, m_pPokeListContextParam );
      m_pWazaSodateyaFrame->SetOutParam( m_pStatusAppParam, NULL );
    } break;

  default:
    {
      m_pFieldFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, PokeListFrame>( this->GetFrameManager() );
      m_pFieldFrame->Setup( &m_PokeListAppParam, m_pPokeListContextParam );
      m_pFieldFrame->SetOutParam( m_pStatusAppParam, m_pBagAppParam );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト画面で使用する外部設定パラメータのセット
 */
//------------------------------------------------------------------------------
void Proc::SetAppParam( GameSys::GameManager * pGameMan, CONTEXT_PARAM * pContext, app::bag::APP_PARAM * pBagAppParam, Status::APP_PARAM * pStatusParam )
{
  m_PokeListAppParam.mpGameManager = pGameMan;

  m_pPokeListContextParam = pContext;
  m_pBagAppParam = pBagAppParam;
  m_pStatusAppParam = pStatusParam;
}

GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
