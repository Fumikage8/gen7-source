// ============================================================================
/*
 * @file SuperTrainingApplicationSystem.cpp
 * @brief すごい特訓アプリのアプリケーションシステムです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#include "App/SuperTraining/source/System/SuperTrainingApplicationSystem.h"
#include "App/SuperTraining/source/System/SuperTrainingBuilder.h"
#include "App/SuperTraining/source/SuperTrainingResourceID.h"

#include <arc_def_index/arc_def.h>

#include "System/include/ArcIdGetter.h"

//  リソースのインクルード
#include <arc_index/tokkunMenu.gaix>
#include <arc_index/MenuCursor.gaix>
#include <arc_index/field_status.gaix>

//  メッセージのインクルード
#include <arc_index/message.gaix>

//  Viewのインクルード
#include "App/SuperTraining/source/System/SuperTrainingViewList.h"
#include "App/SuperTraining/source/System/SuperTrainingViewObjects.h"
#include "App/SuperTraining/source/System/SuperTrainingViewSystemAccessor.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)


  SuperTrainingApplicationSystem::SuperTrainingApplicationSystem()
  {
    _Clear();
  }


  SuperTrainingApplicationSystem::~SuperTrainingApplicationSystem()
  {
    GFL_SAFE_DELETE( m_pViewList );
    GFL_SAFE_DELETE( m_pViewObjects );
    GFL_SAFE_DELETE( m_pViewSystemAccessor );
  }


  void SuperTrainingApplicationSystem::Build( App::SuperTraining::System::SuperTrainingWork* pSuperTrainingWork, App::SuperTraining::System::SuperTrainingBuilder* pApplicationSystemBuilder )
  {
    GFL_ASSERT( pSuperTrainingWork );

    m_pWork = pSuperTrainingWork;

    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    GFL_ASSERT( pAppHeap );

    //  View関連のインスタンス作成
    {
      m_pViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::SuperTraining::System::ViewList( pAppHeap );
      m_pViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::SuperTraining::System::ViewObjects();
      m_pViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::SuperTraining::System::ViewSystemAccessor( m_pViewList, m_pViewObjects );

      //  workにViewのアクセサを参照
      m_pWork->SetViewSystemAccessor( m_pViewSystemAccessor );
    }

    ApplicationSystemBase::Build( pApplicationSystemBuilder );
  }


  void SuperTrainingApplicationSystem::LoadResourceRequest()
  {
    //  レイアウトのロード
    {
      m_pResourceManager->AddLoadRequest( App::SuperTraining::SUPER_TRAINING_RESOURCE_ID_LAYOUT, ARCID_TOKKUN_MENU_DATA, GARC_tokkunMenu_tokkunMenu_applyt_COMP, true, true );
      m_pResourceManager->AddLoadRequest( App::SuperTraining::SUPER_TRAINING_RESOURCE_ID_UPPLER_STATUS_LAYOUT, ARCID_FIELD_STATUS, GARC_field_status_field_status_applyt_COMP, true, true );
    }

    //  メッセージロード
    {
      m_pResourceManager->AddLoadRequest( App::SuperTraining::SUPER_TRAINING_RESOURCE_ID_MESSAGE, print::GetMessageArcId(), GARC_message_tokkun_DAT, false, true );
      // @attention アイテム名の直接取得がルール違反だが、nijiで問題ないとのことなので、momijiでも変更しない。
      m_pResourceManager->AddLoadRequest( App::SuperTraining::SUPER_TRAINING_RESOURCE_ID_ITEM_TEXT, print::GetMessageArcId(), GARC_message_itemname_DAT, false, true );
      m_pResourceManager->AddLoadRequest( App::SuperTraining::SUPER_TRAINING_RESOURCE_ID_UPPLER_STATUS_MESSAGE, print::GetMessageArcId(), GARC_message_pokemon_status_DAT, false, true );
    }

    //  レイアウトのカーソルロード
    m_pResourceManager->AddLoadRequest( NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
  }


  void SuperTrainingApplicationSystem::CreateMessageData()
  {
    //  メッセージ制御を作成
    m_pMessageUtility->LoadFromMemory( GARC_message_tokkun_DAT, m_pResourceManager->GetResourceBuffer( App::SuperTraining::SUPER_TRAINING_RESOURCE_ID_MESSAGE ) );
    m_pMessageUtility->LoadFromMemory( GARC_message_itemname_DAT, m_pResourceManager->GetResourceBuffer( App::SuperTraining::SUPER_TRAINING_RESOURCE_ID_ITEM_TEXT ) );
    m_pMessageUtility->LoadFromMemory( GARC_message_pokemon_status_DAT, m_pResourceManager->GetResourceBuffer( App::SuperTraining::SUPER_TRAINING_RESOURCE_ID_UPPLER_STATUS_MESSAGE ) );

    m_pWork->SetMessageUtility( m_pMessageUtility );
  }


  void SuperTrainingApplicationSystem::CreateView()
  {
  }


  void SuperTrainingApplicationSystem::UpdateView()
  {
    m_pViewList->Update();
  }


  void SuperTrainingApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pViewList->Draw( displayNo );
  }


  //! @brief アプリケーションの終了処理を行います。
  //! @retval true 終了処理中
  //! @retval false 終了処理完了
  bool SuperTrainingApplicationSystem::End()
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
    {
      return NetAppLib::System::ApplicationSystemBase::End();
    }

    return  true;
  }


  bool SuperTrainingApplicationSystem::IsExitView()
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
  bool SuperTrainingApplicationSystem::IsCreateViewComplete()
  {
    bool  bComplete = true;

    if( m_pViewObjects->InitFunc( m_pWork, m_pNetAppMessageMenu ) == false )
    {
      bComplete = false;
    }

    return bComplete;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)
