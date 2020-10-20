// ============================================================================
/*
 * @file KisekaeApplicationSystem.h
 * @brief きせかえアプリのアプリケーションシステムです。
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/System/KisekaeApplicationSystem.h"
#include "App/Kisekae/source/System/KisekaeBuilder.h"
#include "App/Kisekae/source/KisekaeResourceID.h"

//  gfl2のインクルード
#include <model/include/gfl2_BaseCamera.h>

//  nijiのインクルード
#include "System/include/ArcIdGetter.h"
#include "System/include/DressUp.h"
#include "AppLib/include/Tool/AppToolCharaSimpleModel.h"
#include "AppLib/include/Util/AppUtilFileControlState.h"
#include "AppLib/include/Tool/TimerIconView.h"
#include "GameSys/include/GameData.h"

//  リソースのインクルード
#include <arc_index/ShopKisekaeLytData.gaix>
#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/MenuCursor.gaix>

//  Viewのインクルード
#include "App/Kisekae/source/System/KisekaeViewList.h"
#include "App/Kisekae/source/System/KisekaeViewObjects.h"
#include "App/Kisekae/source/System/KisekaeViewSystemAccessor.h"
#include "App/Kisekae/source/Tool/CharaModelViewer.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

  ApplicationSystem::ApplicationSystem()
  {
    _Clear();
  }

  ApplicationSystem::~ApplicationSystem()
  {
    GFL_SAFE_DELETE( m_pIconFileCtrlState );
    GFL_SAFE_DELETE( m_pCharaModelViewer );
    GFL_SAFE_DELETE( m_pViewSystemAccessor );
    GFL_SAFE_DELETE( m_pViewList );
    GFL_SAFE_DELETE( m_pViewObjects );
    GFL_SAFE_DELETE( m_pUpperDrawEnvNode );
    GFL_SAFE_DELETE( m_pUpperCamera );
  }

  void ApplicationSystem::Build( App::Kisekae::System::Work* pWork, App::Kisekae::System::Builder* pApplicationSystemBuilder )
  {
    app::util::Heap* pAppHeap = pWork->GetAppHeap();
    GFL_ASSERT( pAppHeap );

    m_pWork = pWork;

    //  View関連のインスタンス作成
    {
      m_pViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::System::ViewList( pAppHeap );
      m_pViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::System::ViewObjects();
      m_pViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::System::ViewSystemAccessor( m_pViewList, m_pViewObjects );

      //  workにViewのアクセサを参照
      m_pWork->SetViewSystemAccessor( m_pViewSystemAccessor );
    }

    //  アイコンファイルをステート
    {
      m_pIconFileCtrlState  = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( m_pWork->GetIconArcId(), pAppHeap->GetDeviceHeap(), m_pWork->IsIconFileComp(), 128 );
    }

    ApplicationSystemBase::Build( pApplicationSystemBuilder );
  }

  void ApplicationSystem::LoadResourceRequest()
  {
    //  レイアウトロード
    m_pResourceManager->AddLoadRequest( App::Kisekae::RESOURCE_ID_LAYOUT, ARCID_SHOP_KISEKAE_LYT_DATA, GARC_ShopKisekaeLytData_ShopKisekaeLytData_applyt_COMP, true, true );

    //  メッセージロード
    {
      m_pResourceManager->AddLoadRequest( App::Kisekae::RESOURCE_ID_MESSAGE, print::GetMessageArcId(), GARC_message_kisekae_DAT, false, true );
      m_pResourceManager->AddLoadRequest( App::Kisekae::RESOURCE_ID_MESSAGE_DRESS_ITEM, print::GetMessageArcId(), GARC_message_kisekae_item_name_DAT, false, true );
      m_pResourceManager->AddLoadRequest( App::Kisekae::RESOURCE_ID_MESSAGE_DRESS_COLOR, print::GetMessageArcId(), GARC_message_kisekae_color_DAT, false, true );
    }

    //  アイテムと色のテキストロード

    //  レイアウトのカーソルロード
    m_pResourceManager->AddLoadRequest( NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );

    //  タイマーアイコンのレイアウトロード
    m_pResourceManager->AddLoadRequest( RESOURCE_ID_TIMER_ICON_LYT, app::tool::TimerIconUIView::GetArcId(), app::tool::TimerIconUIView::GetArcDataId(), app::tool::TimerIconUIView::IsArcDataComp(), true );

    //  アイコンファイルを開く
    m_pIconFileCtrlState->Start();
  }

  void ApplicationSystem::CreateMessageData()
  {
    //  メッセージ制御を作成
    m_pMessageUtility->LoadFromMemory( GARC_message_kisekae_DAT, m_pResourceManager->GetResourceBuffer( App::Kisekae::RESOURCE_ID_MESSAGE ) );
    m_pMessageUtility->LoadFromMemory( GARC_message_kisekae_item_name_DAT, m_pResourceManager->GetResourceBuffer( App::Kisekae::RESOURCE_ID_MESSAGE_DRESS_ITEM ) );
    m_pMessageUtility->LoadFromMemory( GARC_message_kisekae_color_DAT, m_pResourceManager->GetResourceBuffer( App::Kisekae::RESOURCE_ID_MESSAGE_DRESS_COLOR ) );

    m_pWork->SetMessageUtility( m_pMessageUtility );
  }

  void ApplicationSystem::CreateView()
  {
    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    GFL_ASSERT( pAppHeap );

    //  モデルビューアーを作成
    {
      //  初期化を作成
      app::tool::CharaModelViewer::INIT_DATA  initData;
      {
        app::util::AppRenderingManager::CreateDefaultCamera( pAppHeap, &m_pUpperDrawEnvNode, &m_pUpperCamera );

        initData.charaModelArcId  = ARCID_CHARA_MODEL_BATTLE_NORMAL;
        initData.pCharaModelDressupArcIdList  = ::System::DressUp::GetDressUpArcIdListBattle();

        initData.charaModelSetupParam.aRenderPlaceType[ 0 ]   = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;
        initData.charaModelSetupParam.pRenderingManager       = m_pAppRenderingManager;
        initData.charaModelSetupParam.pUseHeap                = pAppHeap->GetDeviceHeap();
        initData.pFileManager   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
        initData.pTimerIconBin  = m_pResourceManager->GetResourceBuffer( RESOURCE_ID_TIMER_ICON_LYT );
        initData.pCamera        = m_pUpperCamera;
        initData.pUtilHeap      = pAppHeap;

        //  プレイヤーのトレーナー情報を取得してモデル表示
        Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
        {
          APP_PARAM* appParam = m_pWork->GetAppParam();
          if( appParam->runType != EVENT_RUN_TYPE_FINDER_STUDIO )
          {
            initData.dressUpParam = pMyStatus->GetDressup();
          }
          else {
            initData.dressUpParam = appParam->inputDressUpParam;
          }
          initData.modelCharaId         = app::tool::CharaSimpleModel::GetCharaBattleCharaId( pMyStatus->GetSex() );
          initData.modelCharaAnimWaitId = app::tool::CharaSimpleModel::GetCharaBattltAnimeWaitId( pMyStatus->GetSex() );
        }
      }

      m_pCharaModelViewer  = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::CharaModelViewer( initData );
      //  workから参照できるように
      m_pWork->SetModelViewer( m_pCharaModelViewer );
    }
  }

  void ApplicationSystem::UpdateView()
  {
    m_pViewList->Update();

    m_pCharaModelViewer->UpdateFunc();
  }

  void ApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pViewList->Draw( displayNo );

    m_pCharaModelViewer->DrawFunc( displayNo );
  }

  //! @brief アプリケーションの終了処理を行います。
  //! @retval true 終了処理中
  //! @retval false 終了処理完了
  bool ApplicationSystem::End()
  {
    if( m_endSeq == 0 )
    {
      //  最初にViewの破棄をする
      if( m_pViewObjects->EndFunc() == false )
      {
        return  true;
      }

      ++m_endSeq;
    }
    else
    if( m_endSeq == 1 )
    {
      if( m_pCharaModelViewer->EndFunc() == false )
      {
        return true;
      }

      ++m_endSeq;
    }
    else
    if( m_endSeq == 2 )
    {
      m_pIconFileCtrlState->End();

      ++m_endSeq;
    }
    else
    if( m_endSeq == 3 )
    {
      m_pIconFileCtrlState->Update();
      if( m_pIconFileCtrlState->IsEnd() == false )
      {
        return  true;
      }

      ++m_endSeq;
    }
    else
    {
      if( NetAppLib::System::ApplicationSystemBase::End() == false )
      {
        //  きせかえカウントするか決定
        if( m_pWork->IsDressup() == true )
        {
          Savedata::IncRecord( Savedata::Record::RECID_KISEKAE_CNT );
        }

        return  false;
      }
    }

    return  true;
  }

  void ApplicationSystem::SetupCamera()
  {
    m_pAppRenderingManager->SetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pUpperCamera );
  }

  void ApplicationSystem::UnsetCamera()
  {
    m_pAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );
  }

  bool ApplicationSystem::IsExitView()
  {
    bool bExit = true;

    if( m_pViewObjects )
    {
      if( m_pViewObjects->IsDrawing() )
      {
        bExit = false;
      }
    }

    return bExit;
  }

  //! @brief ビューの作成が完了したか
  bool ApplicationSystem::IsCreateViewComplete()
  {
    bool  bComplete = true;

    if( m_pWork->IsReady() == false )
    {
      bComplete = false;
    }

    if( m_pCharaModelViewer )
    {
      if( m_pCharaModelViewer->InitFunc() == false )
      {
        bComplete = false;
      }
    }

    if( m_pViewObjects->InitFunc( m_pWork, m_pNetAppMessageMenu ) == false )
    {
      bComplete = false;
    }

    m_pIconFileCtrlState->Update();
    if( m_pIconFileCtrlState->IsManualIdleState() == false )
    {
      bComplete = false;
    }

    if( bComplete == true )
    {
      m_pWork->CreateItemList();
    }

    return bComplete;
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
