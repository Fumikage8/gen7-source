// ============================================================================
/*
 * @file ApplicationSystemBase.cpp
 * @brief アプリケーションシステムの基底クラスです。
 * @date 2015.03.26
 */
// ============================================================================
#include <Fade/include/gfl2_FadeManager.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Util/AppCountryRegionNameUtility.h"
#include "AppLib/include/Ui/UIViewManager.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "GameSys/include/GameManager.h"
#include "System/include/nijiAllocator.h"
#include "System/include/HeapDefine.h"
#include <arc_def_index/arc_def.h>
#include <arc_index/MenuWindow.gaix>
#include <arc_index/MenuCursor.gaix>

#include  <debug/include/gfl2_DebugLogWindow.h>

using namespace app::util;

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


ApplicationSystemBase::ApplicationSystemBase() :
  m_pFrameSwitcher( NULL ),
  m_pFrameFactory( NULL ),
  m_pResourceManager( NULL ),
  m_pNijiAllocator( NULL ),
  m_pAppRenderingManager( NULL ),
  m_pAppRenderingManagers( NULL ),
  m_pAppFrameManager( NULL ),
  m_pMessageUtility( NULL ),
  m_pNetAppMessageMenu( NULL ),
  m_pNetAppTrainerIconUtility( NULL ),
  m_pAppCountryRegionNameUtility( NULL ),
  m_pAppCommonGrpIconData( NULL ),
  m_eLoadSequence( LOAD_SEQUENCE_APP_RENDER_MANAGER_CREATE_WAIT ),
  m_eEndSequence( END_SEQUENCE_START ),
  m_pExecuteFrame( NULL ),
  m_pWork( NULL ),
  m_StartFrameID( NetAppLib::System::FRAME_ID_INVALID ),
  m_AppRenderingManagerCount( 0 ),
  m_isSuspend(false),
  m_isNonUseFade(false),
  m_isEnableInputExclusiveKeyMode(false),
  m_isUIViewInputExclusiveKey(false),
  m_isFadeOutForNetworkError(false)
{
#if PM_DEBUG
  m_debugLoadStartTime = 0;
#endif
}


ApplicationSystemBase::~ApplicationSystemBase()
{
  Destroy();

  // UIViewマネージャのボタン監視モード
  {
    if( m_isEnableInputExclusiveKeyMode )
    {
      app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
      man->SetInputExclusiveKeyByButtonAnimation( m_isUIViewInputExclusiveKey );
    }
  }

#if PM_DEBUG
#if GF_PLATFORM_CTR
  gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"ApplicationSystem : Exit msec[%lld]\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() - m_debugLoadStartTime );
#endif
#endif
}


void ApplicationSystemBase::Build( NetAppLib::System::IApplicationSystemBuilder* pApplicationSystemBuilder )
{
#if PM_DEBUG
#if GF_PLATFORM_CTR
  m_debugLoadStartTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
#endif
#endif

  {
    m_pFrameSwitcher = pApplicationSystemBuilder->CreateApplicationFrameSwitcher();
    m_pFrameFactory = pApplicationSystemBuilder->CreateApplicationFrameFactory();
  }

  m_pWork = pApplicationSystemBuilder->GetApplicationWork();

  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
  gfl2::heap::HeapBase* pDeviceHeap = m_pWork->GetDeviceHeap();
  
  // リソースマネージャ
  {
    u32 applicationResourceCount = pApplicationSystemBuilder->GetApplicationResourceCount();

    m_pResourceManager = GFL_NEW( pDeviceHeap ) NetAppLib::System::ResourceManager( pAppHeap, applicationResourceCount + NetAppLib::System::COMMON_RESOURCE_COUNT );

    m_pWork->SetResourceManager( m_pResourceManager );
  }
  
  // レンダリングマネージャ
  {

#if PM_DEBUG
    GFL_PRINT( "----------------------------------------------\n" );
    GFL_PRINT( "AppRenderingManager create start.\n" );
    residentDeviceDebugPrint();
#endif // PM_DEBUG


    m_pNijiAllocator = GFL_NEW( pDeviceHeap ) ::System::nijiAllocator( pDeviceHeap );

    m_AppRenderingManagerCount = pApplicationSystemBuilder->CreateAppRenderingManagerCount();

    m_pAppRenderingManagers = GFL_NEW_ARRAY( pDeviceHeap ) app::util::AppRenderingManager[ m_AppRenderingManagerCount ];

    AppRenderingManager::Description* pDescription = pApplicationSystemBuilder->CreateAppRenderingManagerDescription();
    AppRenderingManager::Description* pDescriptions = pApplicationSystemBuilder->CreateAppRenderingManagerDescriptions();

    if( pDescription == NULL && pDescriptions == NULL )
    {
      pDescription = GFL_NEW( pDeviceHeap->GetLowerHandle() ) AppRenderingManager::Description(); 
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder         = AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].lytWkMax            = 32;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder         = AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].lytWkMax            = 32;

      // デフォルト設定で動かなくなったら以下の設定にする
//      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder         = AppRenderingManager::RenderOrderType::LAYOUT;
//      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].lytWkMax            = 32;
      /*
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].renderOrder         = AppRenderingManager::RenderOrderType::LAYOUT;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].lytWkMax            = 8;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].pokeModelNumMax     = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].modelNumMax         = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].charaModelNumMax    = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::UPPER_DISPLAY ].dressUpModelNumMax  = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].renderOrder         = AppRenderingManager::RenderOrderType::LAYOUT;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].lytWkMax            = 8;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].pokeModelNumMax     = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].modelNumMax         = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].charaModelNumMax    = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ].dressUpModelNumMax  = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].renderOrder         = AppRenderingManager::RenderOrderType::LAYOUT;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].lytWkMax            = 8;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].pokeModelNumMax     = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].modelNumMax         = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].charaModelNumMax    = 0;
      pDescription->renderDesc[ AppRenderingManager::RenderPlaceType::OFF_SCREEN    ].dressUpModelNumMax  = 0;
      */
      pDescription->isUpperDisplayStereo                                                                  = false;
    }

    {
      gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

      AppRenderingManager::Description* pTemp = ( pDescriptions ) ? pDescriptions : pDescription;

      for( u32 i = 0; i < m_AppRenderingManagerCount; ++i )
      {
        m_pAppRenderingManagers[i].SyncCreate( pTemp[i], pAppHeap, m_pNijiAllocator, pAsyncFileManager );
      }
    }

    m_pAppRenderingManager = &m_pAppRenderingManagers[0];

    m_pWork->SetAppRenderingManager( m_pAppRenderingManager );

    GFL_SAFE_DELETE( pDescription );
    GFL_SAFE_DELETE_ARRAY( pDescriptions );
  }

  // フレームマネージャ
  {
    gfl2::heap::HeapBase* pAppFrameManagerHeap = m_pWork->GetAppFrameManagerHeap();

    m_pAppFrameManager = GFL_NEW( pAppFrameManagerHeap ) applib::frame::Manager( pAppFrameManagerHeap );

    m_StartFrameID = pApplicationSystemBuilder->GetApplicationStartFrameID();
  }

  // メッセージユーティリティ
  {
    if( pApplicationSystemBuilder->IsCreateMessageUtility() )
    {
      u32 dataNum = 0;
      u32 wordMax = 0;
      u32 bufLen  = 0;
      pApplicationSystemBuilder->GetMessageUtilityParameter( dataNum, wordMax, bufLen );

      m_pMessageUtility = GFL_NEW( pDeviceHeap ) NetAppLib::Message::MessageUtility( pDeviceHeap, dataNum, wordMax, bufLen );
      m_pWork->SetMessageUtility( m_pMessageUtility );
    }
  }

  // メッセージメニュー
  {
    if( pApplicationSystemBuilder->IsCreateNetAppMessageMenu() )
    {
      m_pNetAppMessageMenu = GFL_NEW( pDeviceHeap ) NetAppLib::UI::NetAppMessageMenu( m_pWork );
    }
  }

  // トレーナーアイコンユーティリティ
  {
    if( pApplicationSystemBuilder->IsCreateAppToolTrainerIconRendering() )
    {
      u32 iconNum = pApplicationSystemBuilder->GetTrainerIconNum();
      app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType = pApplicationSystemBuilder->GetTrainerIconDrawDisplayType();

      gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

      m_pNetAppTrainerIconUtility = GFL_NEW( pDeviceHeap ) NetAppLib::Util::NetAppTrainerIconUtility();
      m_pNetAppTrainerIconUtility->Create( m_pAppRenderingManager, iconNum, displayType, pApplicationSystemBuilder->IsCreateAppToolTrainerIconRenderingByNPCOnly() );
    }
  }

  // 国名と地域名のユーティリティ
  {
    if( pApplicationSystemBuilder->IsCreateAppCountryRegionNameUtility() )
    {
      m_pAppCountryRegionNameUtility = GFL_NEW( pDeviceHeap ) app::util::AppCountryRegionNameUtility();
      m_pAppCountryRegionNameUtility->LoadStart();
      m_pWork->SetAppCountryRegionNameUtility( m_pAppCountryRegionNameUtility );
    }
  }

  // 共通アイコン置き換えユーティリティ
  {
    if( pApplicationSystemBuilder->IsCreateAppCommonGrpIconData() )
    {
      m_pAppCommonGrpIconData = GFL_NEW( pDeviceHeap ) app::tool::AppCommonGrpIconData();
      m_pAppCommonGrpIconData->Initialize( pAppHeap );
      m_pWork->SetAppCommonGrpIconData( m_pAppCommonGrpIconData );
    }
  }

  // UIViewマネージャのボタン監視モード
  {
    m_isEnableInputExclusiveKeyMode = pApplicationSystemBuilder->IsEnableInputExclusiveKeyMode();

    if( m_isEnableInputExclusiveKeyMode )
    {
      app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
      m_isUIViewInputExclusiveKey = man->GetInputExclusiveKeyByButtonAnimation();
      man->SetInputExclusiveKeyByButtonAnimation( true );
    }
  }

}

/**
 * @brief 一時停止
 * @note  フレームの更新を停止する
 */
void ApplicationSystemBase::Suspend()
{
  m_isSuspend = true;

  if( m_pExecuteFrame )
  {
    m_pExecuteFrame->OnSuspend();
    m_pWork->SetSuspend( true );
  }
}

/**
 * @brief 再起動
 * @note  フレームの更新を再開する
 */
void ApplicationSystemBase::Resume()
{
  m_isSuspend = false;

  if( m_pExecuteFrame )
  {
    m_pExecuteFrame->OnResume();
    m_pWork->SetSuspend( false );
  }
}

void ApplicationSystemBase::Destroy()
{
  // 共通アイコン置き換えユーティリティ
  {
    if( m_pAppCommonGrpIconData )
    {
      m_pAppCommonGrpIconData->Terminate();
    }
    GFL_SAFE_DELETE( m_pAppCommonGrpIconData );
  }

  // 国名と地域名のユーティリティ
  {
    GFL_SAFE_DELETE( m_pAppCountryRegionNameUtility );

    m_pWork->SetAppCountryRegionNameUtility( NULL );
  }

  // トレーナーアイコンユーティリティ
  {
    GFL_SAFE_DELETE( m_pNetAppTrainerIconUtility );
  }

  // メッセージメニュー
  {
    GFL_SAFE_DELETE( m_pNetAppMessageMenu );
  }

  // メッセージユーティリティ
  {
    GFL_SAFE_DELETE( m_pMessageUtility );
    
    m_pWork->SetMessageUtility( NULL );
  }

  // フレームマネージャ
  {
    GFL_SAFE_DELETE( m_pAppFrameManager );
  }

  // レンダリングマネージャ
  {
    m_pAppRenderingManager = NULL;

    GFL_SAFE_DELETE_ARRAY( m_pAppRenderingManagers );

    GFL_SAFE_DELETE( m_pNijiAllocator );

    m_pWork->SetAppRenderingManager( NULL );
  }

  // リソースマネージャ
  {
    GFL_SAFE_DELETE( m_pResourceManager );

    m_pWork->SetResourceManager( NULL );
  }
  
  {
    GFL_SAFE_DELETE( m_pFrameFactory );
    GFL_SAFE_DELETE( m_pFrameSwitcher );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  リソースのローディング処理
 *	@retval true ローディング中
 *	@retval false ロード完了
 */
//-----------------------------------------------------------------------------
bool ApplicationSystemBase::Loading()
{
  updateCountryRegionNameUtility();

  switch( m_eLoadSequence )
  {
    case LOAD_SEQUENCE_APP_RENDER_MANAGER_CREATE_WAIT:
    {
      updateRenderingManagers();
      if( isCreatedRenderingManagers() )
      {
#if PM_DEBUG
        GFL_PRINT( "----------------------------------------------\n" );
        GFL_PRINT( "AppRenderingManager create end.\n" );
        residentDeviceDebugPrint();
#endif // PM_DEBUG

        if( m_pNetAppTrainerIconUtility )
        {
          m_eLoadSequence = LOAD_SEQUENCE_APP_TRAINER_ICON_CREATE_WAIT;
        }
        else
        {
          m_eLoadSequence = LOAD_SEQUENCE_APP_COMMON_GRP_ICON_DATA_WAIT;
        }
      }
    }
    break;

    case LOAD_SEQUENCE_APP_TRAINER_ICON_CREATE_WAIT:
    {
      m_pNetAppTrainerIconUtility->Update();
      if( m_pNetAppTrainerIconUtility->IsCreate() )
      {
        m_eLoadSequence = LOAD_SEQUENCE_APP_COMMON_GRP_ICON_DATA_WAIT;
      }
    }
    break;

    case LOAD_SEQUENCE_APP_COMMON_GRP_ICON_DATA_WAIT:
    {
      if( m_pAppCommonGrpIconData )
      {
        if( m_pAppCommonGrpIconData->IsReady() )
        {
          m_eLoadSequence = LOAD_SEQUENCE_LOAD_START;
        }
      }
      else
      {
        m_eLoadSequence = LOAD_SEQUENCE_LOAD_START;
      }
    }
    break;

    case LOAD_SEQUENCE_LOAD_START:
    {
      if( m_pNetAppMessageMenu )
      {
        m_pResourceManager->AddLoadRequest( NetAppLib::System::COMMON_RESOURCE_ID_MENU_WINDOW, ARCID_MENU_WINDOW, GARC_MenuWindow_MenuWindow_applyt_COMP, true, true );
        m_pResourceManager->AddLoadRequest( NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
      }

      LoadResourceRequest();

      m_eLoadSequence = LOAD_SEQUENCE_LOAD_WAIT;
    }
    break;
    
    case LOAD_SEQUENCE_LOAD_WAIT:
    {
      m_pResourceManager->Update();
      if( m_pResourceManager->IsLoadFinished() )
      {
        CreateMessageData();

        if( m_pNetAppMessageMenu )
        {
          m_pNetAppMessageMenu->Setup( NetAppLib::System::COMMON_RESOURCE_ID_MENU_WINDOW, NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR, m_pMessageUtility );
        }

        CreateView();
        CreateNet();

        m_eLoadSequence = LOAD_SEQUENCE_CREATE_VIEW_WAIT;
      }
    }
    break;

    case LOAD_SEQUENCE_CREATE_VIEW_WAIT:
    {
      UpdateView();
      if( IsCreateViewComplete() && IsCreateNetComplete() )
      {
        AddModel();
        SetupCamera();
        // 初期フレームの作成
        {
          m_pExecuteFrame = m_pFrameFactory->CreateNetAppFrame( m_StartFrameID );
          m_pAppFrameManager->CallProc( m_pExecuteFrame );
          m_pExecuteFrame->SetMyManager( m_pAppFrameManager );
          if( !m_isNonUseFade ){
            m_pExecuteFrame->FadeInRequest();
          }
          m_pWork->SetExecuteFrameID( m_StartFrameID );
        }
#if PM_DEBUG
#if GF_PLATFORM_CTR
        gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"ApplicationSystem : Loading msec[%lld]\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() - m_debugLoadStartTime );
#endif
#endif

        return false;
      }
    }
    break;
  }
  return true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーションを動かす為の各種モジュールの更新
 *	@retval true 実行しているFrameが存在しない(Procを終了させる)
 *	@retval false 実行しているFrameが存在している(Procを継続させる)
 */
//-----------------------------------------------------------------------------
bool ApplicationSystemBase::Update()
{
  updateCountryRegionNameUtility();

  m_pResourceManager->Update();

  updateRenderingManagers();

  if( m_pNetAppTrainerIconUtility )
  {
    m_pNetAppTrainerIconUtility->Update();
  }

  UpdateNet();
  UpdateView();

  // 一時停止フラグが立っている場合はフレーム更新処理しない
  if( m_isSuspend == true )
  {
    // フレームの更新はしないが、初期化までは通さないと画面セットアップがなされていない
    // その為、フレームがMainまで行っていない場合は更新処理させる。
    if( m_pAppFrameManager->GetBaseProcessInitialized() == true )
    {
      return false;
    }
    // ↑画面を表示してもフェード中はサスペンド状態になっているため、見た目がよくなかった。
    //  画面のセットアップ時に初めて表示する方がまだ良かったのでそうする。ichiraku
    //  ↑追記
    //  スクリプト中にフィールド戻ってきた時に画面が何も出ない状態になった為戻した。（SetVisibleが呼ばれないから）
  }

  applib::frame::MainStatus eAppFrameMainStatus = m_pAppFrameManager->Main();
  
  NetAppLib::System::FrameResult frameResult = m_pWork->GetFrameResult();

  // 有効なFrameが存在する
  if( eAppFrameMainStatus == applib::frame::MAINSTATUS_VALID )
  {
    // Frameの結果が設定された
    if( frameResult != NetAppLib::System::FRAME_RESULT_INVALID )
    {
      NetAppLib::System::FrameID executeFrameID = m_pWork->GetExecuteFrameID();
      NetAppLib::System::FrameID nextFrameID = m_pFrameSwitcher->GetNextFrameID( executeFrameID, frameResult );

      // 次に実行するFrameが無い
      if( nextFrameID == NetAppLib::System::FRAME_ID_INVALID )
      {
        // フェードアウトをリクエストする
        if( !m_isNonUseFade && m_pExecuteFrame )
        {
          m_pExecuteFrame->FadeOutRequest();
        }
      }
    }
  }
  // Frameが存在しない
  else
  if( eAppFrameMainStatus == applib::frame::MAINSTATUS_NULL )
  {
    // Frameの結果が設定された
    if( frameResult != NetAppLib::System::FRAME_RESULT_INVALID )
    {
      NetAppLib::System::FrameID executeFrameID = m_pWork->GetExecuteFrameID();
      NetAppLib::System::FrameID nextFrameID = m_pFrameSwitcher->GetNextFrameID( executeFrameID, frameResult );

      // 次に実行するFrameがある
      if( nextFrameID != NetAppLib::System::FRAME_ID_INVALID )
      {
        m_pExecuteFrame = m_pFrameFactory->CreateNetAppFrame( nextFrameID );
        m_pAppFrameManager->CallProc( m_pExecuteFrame );
        m_pExecuteFrame->SetMyManager( m_pAppFrameManager );
      }
      // 次に実行するFrameが無い
      else
      {
        m_pWork->SetLastExecuteFrameID( executeFrameID );
      }
      m_pWork->SetExecuteFrameID( nextFrameID );
      m_pWork->SetFrameResult( NetAppLib::System::FRAME_RESULT_INVALID );

#if PM_DEBUG
      appHeapDebugPrint();
      residentDeviceDebugPrint();
#endif // PM_DEBUG
    }

    if( m_pWork->GetExecuteFrameID() == NetAppLib::System::FRAME_ID_INVALID )
    {
      return true;
    }
  }

  return false;
}


#if PM_DEBUG

void ApplicationSystemBase::appHeapDebugPrint()
{
  GFL_PRINT( "----------------------------------------------\n" );
  GFL_PRINT( "NetApp memory debug\n" );
  GFL_PRINT( "----- System Heap -----\n" );
  GFL_PRINT( "Total size                = %d\n", m_pWork->GetAppHeap()->GetSystemHeap()->GetTotalSize() );
  GFL_PRINT( "Total free size           = %d\n", m_pWork->GetAppHeap()->GetSystemHeap()->GetTotalFreeSize() );
  GFL_PRINT( "Total allocatable size    = %d\n", m_pWork->GetAppHeap()->GetSystemHeap()->GetTotalAllocatableSize() );
  GFL_PRINT( "Memory fragmentation size = %d\n", m_pWork->GetAppHeap()->GetSystemHeap()->GetTotalFreeSize() - m_pWork->GetAppHeap()->GetSystemHeap()->GetTotalAllocatableSize() );
  GFL_PRINT( "Use memory size           = %d\n", m_pWork->GetAppHeap()->GetSystemHeap()->GetTotalSize() - m_pWork->GetAppHeap()->GetSystemHeap()->GetTotalFreeSize() );

  GFL_PRINT( "----- Device Heap -----\n" );
  GFL_PRINT( "Total size                = %d\n", m_pWork->GetAppHeap()->GetDeviceHeap()->GetTotalSize() );
  GFL_PRINT( "Total free size           = %d\n", m_pWork->GetAppHeap()->GetDeviceHeap()->GetTotalFreeSize() );
  GFL_PRINT( "Total allocatable size    = %d\n", m_pWork->GetAppHeap()->GetDeviceHeap()->GetTotalAllocatableSize() );
  GFL_PRINT( "Memory fragmentation size = %d\n", m_pWork->GetAppHeap()->GetDeviceHeap()->GetTotalFreeSize() - m_pWork->GetAppHeap()->GetDeviceHeap()->GetTotalAllocatableSize() );
  GFL_PRINT( "Use memory size           = %d\n", m_pWork->GetAppHeap()->GetDeviceHeap()->GetTotalSize() - m_pWork->GetAppHeap()->GetDeviceHeap()->GetTotalFreeSize() );
  GFL_PRINT( "----------------------------------------------\n" );
}


void ApplicationSystemBase::residentDeviceDebugPrint()
{
  gfl2::heap::HeapBase* pResidentDevice = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );

  GFL_PRINT( "----------------------------------------------\n" );
  GFL_PRINT( "Regident device memory debug\n" );

  GFL_PRINT( "Total size                = %d\n", pResidentDevice->GetTotalSize() );
  GFL_PRINT( "Total free size           = %d\n", pResidentDevice->GetTotalFreeSize() );
  GFL_PRINT( "Total allocatable size    = %d\n", pResidentDevice->GetTotalAllocatableSize() );
  GFL_PRINT( "Memory fragmentation size = %d\n", pResidentDevice->GetTotalFreeSize() - pResidentDevice->GetTotalAllocatableSize() );
  GFL_PRINT( "Use memory size           = %d\n", pResidentDevice->GetTotalSize() - pResidentDevice->GetTotalFreeSize() );

  GFL_PRINT( "----------------------------------------------\n" );

}

#endif // PM_DEBUG


void ApplicationSystemBase::PreDraw()
{
  preDrawRenderingManagers();
}


void ApplicationSystemBase::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pAppFrameManager->Draw( displayNo );

  DrawView( displayNo );

  if( m_pNetAppTrainerIconUtility )
  {
    m_pNetAppTrainerIconUtility->Draw( displayNo );
  }

  drawRenderingManagers( displayNo );
}

//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーション終了処理
 *  @retval  true:終了処理中
 *  @retval  false:終了処理完了
 */
//-----------------------------------------------------------------------------
bool ApplicationSystemBase::End()
{
  // @fix GFNMcat[2505]：通信エラー時にフェードアウトが呼ばれない
  if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() )
  {
    if( !m_isNonUseFade && !m_isFadeOutForNetworkError )
    {
      m_isFadeOutForNetworkError = true; // 一度切り

      gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f );
      gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f );

      // Captureありでブラックフェードを開始
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col, gfl2::Fade::FADE_DEFAULT_SYNC, true );


      // @fix GFNMCat[5228]：フェードアウト後にフレームのInitでフェードインが呼ばれてしまうタイミングを解消
      if( m_pAppFrameManager->IsMainHandle() )
      {
        if( m_pExecuteFrame )
        {
          m_pExecuteFrame->FadeOutExecuted();
        }
      }
    }
  }

  bool bResult = m_pAppFrameManager->End();

  // View破棄可能かチェック
  if( IsExitView() == false )
  {
    return true;
  }

  switch( m_eEndSequence )
  {
    case END_SEQUENCE_START:
    {
#if PM_DEBUG
#if GF_PLATFORM_CTR
      m_debugLoadStartTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
#endif
#endif
      if( m_pNetAppTrainerIconUtility )
      {
        m_pNetAppTrainerIconUtility->Delete();

        m_eEndSequence = END_SEQUENCE_APP_TRAINER_ICON_TERMINATE_WAIT;
      }
      else
      {
        m_eEndSequence = END_SEQUENCE_APP_COUNTRY_REGION_NAME_TERMINATE_START;
      }
    }
    break;

    case END_SEQUENCE_APP_TRAINER_ICON_TERMINATE_WAIT:
    {
      m_pNetAppTrainerIconUtility->Update();
      if( m_pNetAppTrainerIconUtility->IsDelete() )
      {
        m_eEndSequence = END_SEQUENCE_APP_COUNTRY_REGION_NAME_TERMINATE_START;
      }
    }
    break;

    case END_SEQUENCE_APP_COUNTRY_REGION_NAME_TERMINATE_START:
    {
      if( m_pAppCountryRegionNameUtility )
      {
        m_pAppCountryRegionNameUtility->LoadEndRequest();

        m_eEndSequence = END_SEQUENCE_APP_COUNTRY_REGION_NAME_TERMINATE_WAIT;
      }
      else
      {
        m_eEndSequence = END_SEQUENCE_REMOVE_MODEL;
      }
    }
    break;

    case END_SEQUENCE_APP_COUNTRY_REGION_NAME_TERMINATE_WAIT:
    {
      m_pAppCountryRegionNameUtility->Update();
      if( !m_pAppCountryRegionNameUtility->IsLoading() )
      {
        m_eEndSequence = END_SEQUENCE_REMOVE_MODEL;
      }
    }
    break;

    case END_SEQUENCE_REMOVE_MODEL:
    {
      UnsetCamera();
      RemoveModel();

      m_eEndSequence = END_SEQUENCE_APP_RENDERING_MANAGER_TERMINATE_WAIT;
    }
    break;

    case END_SEQUENCE_APP_RENDERING_MANAGER_TERMINATE_WAIT:
    {
      if( canDestroyRenderingManagers() )
      {
        return bResult;
      }
    }
    break;
  }
  
  return true;
}


void ApplicationSystemBase::updateRenderingManagers()
{
  for( u32 i = 0; i < m_AppRenderingManagerCount; ++i )
  {
    m_pAppRenderingManagers[i].Update();
  }
}


bool ApplicationSystemBase::isCreatedRenderingManagers()
{
  bool isCreated = true;

  for( u32 i = 0; i < m_AppRenderingManagerCount; ++i )
  {
    if( !m_pAppRenderingManagers[i].IsCreated() )
    {
      isCreated = false;
    }
  }

  return isCreated;
}


void ApplicationSystemBase::preDrawRenderingManagers()
{
  for( u32 i = 0; i < m_AppRenderingManagerCount; ++i )
  {
    m_pAppRenderingManagers[i].PreDraw();
  }
}


void ApplicationSystemBase::drawRenderingManagers( gfl2::gfx::CtrDisplayNo displayNo )
{
  for( u32 i = 0; i < m_AppRenderingManagerCount; ++i )
  {
    m_pAppRenderingManagers[i].Draw( displayNo );
  }
}


bool ApplicationSystemBase::canDestroyRenderingManagers()
{
  bool canDestroy = true;

  for( u32 i = 0; i < m_AppRenderingManagerCount; ++i )
  {
    if( !m_pAppRenderingManagers[i].CanDestroy() )
    {
      canDestroy = false;
    }
  }

  return canDestroy;
}


void ApplicationSystemBase::updateCountryRegionNameUtility()
{
  if( m_pAppCountryRegionNameUtility )
  {
    m_pAppCountryRegionNameUtility->Update();
  }
}


//! @brief 強制でフェードを使用しない
void ApplicationSystemBase::NonUseFadeRequest()
{
  GFL_PRINT("ApplicationSystemBase::NonUseFadeRequest\n");
  m_isNonUseFade = true;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)
