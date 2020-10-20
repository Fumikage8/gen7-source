//==============================================================================
/**
 @file    ConfigProc.h
 @date    2015.05.08
 @note
          コンフィグプロセス
 */
//==============================================================================
#include "App/Config/include/ConfigProc.h"

//  gflib2
#include <Fade/include/gfl2_FadeManager.h>

// niji
#include "System/include/HeapDefine.h"
#include "Applib/include/Frame/AppFrameManager.h"
#include "Applib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Ui/UIViewManager.h"
#include "GameSys/include/GameData.h"

//  リソースのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/configMenu.gaix>

//  view
#include "App/Config/source/ConfigUIViewPool.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(config)

ConfigProc::ConfigProc() :
GameProc()
{
  _Clear();
}

ConfigProc::~ConfigProc()
{}

gfl2::proc::Result ConfigProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
{
  static const s32 HEAP_MEM_SIZE = 0xf0000;
  static const s32 DEV_MEM_SIZE  = 1024 * 1024 * 10;

  static const s32 LOCAL_HEAP_SIZE = 1024 * 600;
  static const s32 LOCAL_DEVICE_HEAP_SIZE  = 1024 * 1024 * 8;

  u32 seq = GameProc::GetSubSeq();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_TOTAL,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP(ConfigProc_Init,TICK_MAX);
#endif

  if( seq == 0 )
  {
#ifdef DEBUG_ONLY_FOR_uchida_yuto
    GFL_DEBUG_TICK_SET_FLAG(ConfigProc_Init,true);
    GFL_DEBUG_TICK_RESET(ConfigProc_Init);

    GFL_DEBUG_TICK_START(ConfigProc_Init, TICK_TOTAL, "ConfigProc_Init TOTAL");
#endif

    //  コンフィグ全体が使用するメモリを確保
    {
      gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      m_pHeapMem = GFL_CREATE_LOCAL_HEAP( parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  
      gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      m_pDevMem = GFL_CREATE_LOCAL_HEAP( parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
    }

    // ローカルヒープ生成
    {
      m_pUtilheap = GFL_NEW( m_pDevMem ) app::util::Heap();
      m_pUtilheap->LocalHeapCreate( m_pHeapMem, m_pDevMem, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
    }

    // パイプライン生成
    {
      app::util::AppRenderingManager::Description renderingMgrDesc;
      renderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      renderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      m_pRenderingManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::util::AppRenderingManager();
      m_pRenderingManager->SyncCreate( renderingMgrDesc, m_pUtilheap, NULL, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
      GFL_ASSERT( m_pRenderingManager->IsCreated() );  // 同期生成なので m_pRenderingManager->IsCreated() では待たない。
    }

    //  フレーム管理生成
    {
      m_pFrameManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) applib::frame::Manager( m_pUtilheap->GetDeviceHeap() );
    }

    //  メッセージ生成
    {
      m_pNormalMessage = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_option_DAT, m_pUtilheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );

      {
        GameSys::GameManager* pGameManager  = GameProc::GetGameManager();
        if( pGameManager->GetGameData()->GetConfig()->IsJapanese() )
        {
          m_pKanaMessage  = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_KANA ), GARC_message_option_DAT, m_pUtilheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
          m_pKanjiMessage = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_KANJI ), GARC_message_option_DAT, m_pUtilheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
        }
      }

      //  ワード設置作成
      m_pWordSet = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) print::WordSet( m_pUtilheap->GetDeviceHeap() );
    }

    //  viewプール作成
    {
      ConfigUIViewPool::INIT_PARAM  initParam;
      {
        GameSys::GameManager* pGameManager  = GameProc::GetGameManager();

        initParam.pAppMsgData     = m_pNormalMessage;
        initParam.pConfigData     = pGameManager->GetGameData()->GetConfig();
        initParam.pRenderManager  = m_pRenderingManager;
        initParam.pUtilHeap       = m_pUtilheap;
        initParam.pWordSet        = m_pWordSet;
      }

      m_pConfigUIViewPool = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) ConfigUIViewPool( initParam );
    }

    ++seq;
  }
  
  if( seq == 1 )
  {
    if( m_pConfigUIViewPool->InitFunc() == true )
    {
      ++seq;
    }
  }

  // フレーム生成
  if( seq == 2 )
  {
    GameSys::GameManager* pGameManager  = GameProc::GetGameManager();

    m_pConfigFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, ConfigFrame >( m_pFrameManager );

    {
      ConfigFrame::APP_PARAM::_tag_in_param* pInParam  = &m_configFrameAppParam.in_param;
      pInParam->pParentAppHeap    = m_pUtilheap->GetSystemHeap();
      pInParam->pParentDevHeap    = m_pUtilheap->GetDeviceHeap();
      pInParam->pRenderingManager = m_pRenderingManager;
      pInParam->pConfigData       = pGameManager->GetGameData()->GetConfig();
      pInParam->pViewPool         = m_pConfigUIViewPool;
      pInParam->pKanaMessage      = m_pKanaMessage;
      pInParam->pKanjiMessage     = m_pKanjiMessage;
      pInParam->pNormalMessage    = m_pNormalMessage;
    }

    m_pConfigFrame->Initialize( &m_configFrameAppParam );

    ++seq;
  }

  GameProc::SetSubSeq( seq );
  if( seq == 3 )
  {
#ifdef DEBUG_ONLY_FOR_uchida_yuto
    GFL_DEBUG_TICK_END(ConfigProc_Init, TICK_TOTAL);

    gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
    GFL_DEBUG_TICK_DUMP_ALL(ConfigProc_Init);
    gfl2::debug::ResetPrintFlag();
#endif

    // @fix GFNMCat[1434] 
    //  ボタンを同時に検知できないようにするために必要
    app::ui::UIViewManager* pUIViewMan = GFL_SINGLETON_INSTANCE( app::ui::UIViewManager );
    m_bOldInputKeyByButtonAnimation = pUIViewMan->GetInputExclusiveKeyByButtonAnimation();

    pUIViewMan->SetInputExclusiveKeyByButtonAnimation( true );

    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

gfl2::proc::Result ConfigProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
{
  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return gfl2::proc::RES_FINISH;
  }

  //  基幹viewの更新
  {
    ConfigLowerUIDraw*  pLowerUIDraw  = m_pConfigUIViewPool->GetRootLowView();
    ConfigUpperUIDraw*  pUpperUIDraw  = m_pConfigUIViewPool->GetRootUppView();

    pUpperUIDraw->UpdateTree();
    pLowerUIDraw->UpdateTree();
  }

  m_pRenderingManager->Update();

  return gfl2::proc::RES_CONTINUE;
}

void ConfigProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
{
  m_pRenderingManager->PreDraw();
}

void ConfigProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pFrameManager->Draw( displayNo );

  //  基幹viewの更新
  {
    ConfigLowerUIDraw*  pLowerUIDraw  = m_pConfigUIViewPool->GetRootLowView();
    ConfigUpperUIDraw*  pUpperUIDraw  = m_pConfigUIViewPool->GetRootUppView();

    pUpperUIDraw->DrawTree( displayNo );
    pLowerUIDraw->DrawTree( displayNo );
  }

  m_pRenderingManager->Draw( displayNo );
}

gfl2::proc::Result ConfigProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
{

#ifdef DEBUG_ONLY_FOR_uchida_yuto
  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_TOTAL,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP(ConfigProc_EndFunc,TICK_MAX);
#endif

  u32 seq = GameProc::GetSubSeq();
  if( seq == 0 )
  {
#ifdef DEBUG_ONLY_FOR_uchida_yuto
    GFL_DEBUG_TICK_SET_FLAG(ConfigProc_EndFunc,true);
    GFL_DEBUG_TICK_RESET(ConfigProc_EndFunc);

    GFL_DEBUG_TICK_START(ConfigProc_EndFunc, TICK_TOTAL, "ConfigProc_EndFunc TOTAL");
#endif

    gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

    ++seq;
  }

  if( seq == 1 )
  {
    // 終了処理が完了するまで待つ
    if( m_pFrameManager->End() == false )
    {
      ++seq;
    }
  }
  
  if( seq == 2 )
  {
    if( m_pConfigUIViewPool->EndFunc() == true )
    {
      ++seq;
    }
  }
  
  if( seq == 3 )
  {
    GFL_ASSERT( m_pRenderingManager->CanDestroy() );  // 同期生成なので m_pRenderingManager->CanDestroy() では待たない。
    GFL_SAFE_DELETE( m_pRenderingManager );

    GFL_SAFE_DELETE( m_pWordSet );
    GFL_SAFE_DELETE( m_pConfigUIViewPool );
    GFL_SAFE_DELETE( m_pKanjiMessage );
    GFL_SAFE_DELETE( m_pKanaMessage );
    GFL_SAFE_DELETE( m_pNormalMessage );

    //  文字モードによるリソース変更
    {
      //  メッセージを開放した後でないと指定したメッセージを使ってリソース設定しているので、アプリ側で作成したメモリを参照するのでリークが起きる
      if( m_configFrameAppParam.out_param.bMoziChange == true )
      {
        System::SetupLangResource( m_pUtilheap->GetDeviceHeap() );

        //  System::SetupLangResource()この関数内部で再設定されているので削除
        //  常駐バッファ再読み込み
        //WordSetLoader::ReloadGlobal();

        //InteractiveEvent MSG 再読み込み
        //  フィールドスクリプトのメッセージをリロード
        /*  
        field::script::System::GetInstance()->IEMessageReload();
        */
      }
    }

    GFL_SAFE_DELETE( m_pFrameManager );
    GFL_SAFE_DELETE( m_pUtilheap );

    if( m_pDevMem )
    {
      GFL_DELETE_HEAP(m_pDevMem);
      m_pDevMem = NULL;
    }

    if( m_pHeapMem )
    {
      GFL_DELETE_HEAP(m_pHeapMem);
      m_pHeapMem = NULL;
    }

    ++seq;
  }

  GameProc::SetSubSeq( seq );
  if( seq == 4 )
  {
#ifdef DEBUG_ONLY_FOR_uchida_yuto
    GFL_DEBUG_TICK_END(ConfigProc_EndFunc, TICK_TOTAL);

    gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
    GFL_DEBUG_TICK_DUMP_ALL(ConfigProc_EndFunc);
    gfl2::debug::ResetPrintFlag();
#endif

    // @fix GFNMCat[1434] 
    //  ボタンを同時に検知処理の後始末
    app::ui::UIViewManager* pUIViewMan = GFL_SINGLETON_INSTANCE( app::ui::UIViewManager );
    pUIViewMan->SetInputExclusiveKeyByButtonAnimation( m_bOldInputKeyByButtonAnimation );

    return gfl2::proc::RES_FINISH;
  }

  return  gfl2::proc::RES_CONTINUE;
}

GFL_NAMESPACE_END(config)
GFL_NAMESPACE_END(app)
