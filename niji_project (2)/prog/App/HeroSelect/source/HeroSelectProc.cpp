//======================================================================
/**
 * @file    HeroSelectProc.cpp
 * @date    2015/08/25 19:41:53
 * @author  fukushima_yuya
 * @brief   容姿・性別選択画面PROC
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/HeroSelect/include/HeroSelectProc.h>
#include <App/HeroSelect/include/HeroSelectFrame.h>

#include <App/StrInput/include/StrInputFrame.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <GameSys/include/GameData.h>
#include <System/include/HeapDefine.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32 SIZE_PROC_MANAGER = 4 * 1024;        //!< 4Kバイト マネージャーのヒープは荒れるので別途管理
static const u32 SIZE_MY_APP = 1 * 1024 * 1024;       //!< 1Mバイト
static const u32 SIZE_MY_APPDEV = 4 * 1024 * 1024;    //!< 4Mバイト

enum {
  SEQ_HERO_SELECT,
  SEQ_STRINPUT,
  SEQ_NUM,
};


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
  , m_FrameCount( 0 )
  , m_pHeroSelectFrame( NULL )
  , m_pHeroSelectParam( &m_HeroSelectParam )
  , m_pStrInputFrame( NULL )
  , m_Result( gfl2::proc::RES_CONTINUE )
  , m_Seq( SEQ_HERO_SELECT )
  , m_pMyStatus( NULL )
  , m_pMisc( NULL )
  , m_pFieldMenu( NULL )
{
  GameSys::GameManager* gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  GameSys::GameData* gd = gm->GetGameData();

  m_pMyStatus = gd->GetPlayerStatus();
  m_pMisc = gd->GetMisc();
  m_pFieldMenu = gd->GetFieldMenu();
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

    // このアプリ全体に必要なメモリを作成
    m_pAppHeap = GFL_NEW(m_pManagerHeap) app::util::Heap();
    m_pAppHeap->LocalHeapCreate( pAppHeap, pDevHeap, SIZE_MY_APP,SIZE_MY_APPDEV );
    m_pAppHeap->SetFileReadHeap( pDevHeap );

    //マネージャーを作成
    m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
    m_pFrameManager->SetAppFrameListener(this);

    gfl2::fs::AsyncFileManager * afm = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
    //自分のパイプラインを作成
    m_pAppRenderingManager  = GFL_NEW(m_pAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();
    app::util::AppRenderingManager::Description desc;
    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    m_pAppRenderingManager->StartAsyncCreate( desc, m_pAppHeap, NULL, afm );

    gfl2::heap::HeapBase* heap = m_pAppHeap->GetDeviceHeap();
    m_pStrBuf = GFL_NEW(heap) gfl2::str::StrBuf( 32, heap );
  }

  if( m_pAppRenderingManager->IsCreated() )
  {
    // フレームの生成
    CreateHeroSelectFrame();

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
    if( m_Seq == SEQ_HERO_SELECT )
    {
      if( m_pHeroSelectParam->endMode == END_MODE_STRINPUT )
      {
        m_pHeroSelectParam->name_input = 1;
        CreateStrInputFrame();                //!< 名前入力へ
      }
      else if( m_pHeroSelectParam->endMode == END_MODE_NEXT )
      {
        m_Result = gfl2::proc::RES_FINISH;    //!< イントロデモ後半へ
      }
    }
    else if( m_Seq == SEQ_STRINPUT )
    {
      CreateHeroSelectFrame();
    }

    return m_Result;
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
  GFL_UNUSED( pManager );

  // 終了処理が完了するまで待つ
  if( m_pFrameManager->End() == true )
  {
    return gfl2::proc::RES_CONTINUE;
  }
  if( !m_pAppRenderingManager->CanDestroy() )
  {
    return gfl2::proc::RES_CONTINUE;
  }
  if( m_FrameCount == 0 )
  {
    m_FrameCount++;
    return gfl2::proc::RES_CONTINUE;
  }

  GFL_SAFE_DELETE( m_pStrBuf );
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
  GFL_PRINT("EndFuncFinish呼び出し\n");

  if( m_pHeroSelectFrame == pEndCell )
  {
    m_pHeroSelectFrame = NULL;
  }
  else if( m_pStrInputFrame == pEndCell )
  {
    m_pStrInputFrame = NULL;

    // 名前を設定
    m_pMyStatus->SetNameString( m_StrInputParam.strBuf );
    // ニックネームを設定
    m_pFieldMenu->SetNickNameString( m_StrInputParam.strBuf );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   フレームの生成
 */
//------------------------------------------------------------------------------
void Proc::CreateHeroSelectFrame( void )
{
  m_pHeroSelectParam->my_status = m_pMyStatus;

  m_pHeroSelectFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, HeroSelectFrame>( this->GetFrameManager() );
  m_pHeroSelectFrame->Setup( m_pHeroSelectParam, m_pAppHeap, m_pAppRenderingManager );

  m_Seq = SEQ_HERO_SELECT;
}

//------------------------------------------------------------------------------
/**
 * @brief   フレームの生成
 */
//------------------------------------------------------------------------------
void Proc::CreateStrInputFrame( void )
{
  StrInput::APP_PARAM* param = &m_StrInputParam;

  GameSys::GameManager* gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  param->mpGameProcManager = gm->GetProcManager();
  param->mpAppRenderingManager = m_pAppRenderingManager;
  param->mpHeap = m_pAppHeap;

  param->type = StrInput::TYPE_CHARACTER_NAME;
  param->isFieldSubProc = false;
  param->language = System::GetLang();
  param->strBuf = m_pStrBuf;
  param->skipInitGraphicMemory = true;
  param->pSaveMisc = m_pMisc;

  m_pStrInputFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, StrInput::StrInputFrame>( this->GetFrameManager() );
  m_pStrInputFrame->Setup( param );

  m_Seq = SEQ_STRINPUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータの設定
 * @param   pParam    パラメータのポインタ
 */
//------------------------------------------------------------------------------
void Proc::SetAppParam( APP_PARAM* pParam )
{
  m_pHeroSelectParam = pParam;
}

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )
