// ============================================================================
/*
 * @file BattleTreeApplicationSystem.cpp
 * @brief バトルツリーアプリのアプリケーションシステムです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/System/BattleTreeApplicationSystem.h"
#include "App/BattleTree/source/System/BattleTreeBuilder.h"
#include "App/BattleTree/source/BattleTreeResourceID.h"

#include <arc_def_index/arc_def.h>

#include "System/include/ArcIdGetter.h"

//  リソースのインクルード
#include <arc_index/battletreeLyt.gaix>
#include <arc_index/MenuCursor.gaix>

//  メッセージのインクルード
#include <arc_index/message.gaix>

//  Viewのインクルード
#include "App/BattleTree/source/System/BattleTreeViewList.h"
#include "App/BattleTree/source/System/BattleTreeViewObjects.h"
#include "App/BattleTree/source/System/BattleTreeViewSystemAccessor.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)


  BattleTreeApplicationSystem::BattleTreeApplicationSystem()
  {
    _Clear();
  }


  BattleTreeApplicationSystem::~BattleTreeApplicationSystem()
  {
    GFL_SAFE_DELETE( m_pViewSystemAccessor );
    GFL_SAFE_DELETE( m_pViewList );
    GFL_SAFE_DELETE( m_pViewObjects );
  }


  void BattleTreeApplicationSystem::Build( App::BattleTree::System::BattleTreeWork* pBattleTreeWork, App::BattleTree::System::BattleTreeBuilder* pApplicationSystemBuilder )
  {
    m_pWork = pBattleTreeWork;

    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    GFL_ASSERT( pAppHeap );

    //  View関連のインスタンス作成
    {
      m_pViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::System::ViewList( pAppHeap );
      m_pViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::System::ViewObjects();
      m_pViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::System::ViewSystemAccessor( m_pViewList, m_pViewObjects );

      //  workにViewのアクセサを参照
      m_pWork->SetViewSystemAccessor( m_pViewSystemAccessor );
    }

    ApplicationSystemBase::Build( pApplicationSystemBuilder );
  }


  void BattleTreeApplicationSystem::LoadResourceRequest()
  {
    //  レイアウトのロード
    m_pResourceManager->AddLoadRequest( App::BattleTree::BATTLE_TREE_RESOURCE_ID_LAYOUT, ARCID_BATTLE_TREE_LYT_DATA, GARC_battletreeLyt_battletreeLyt_applyt_COMP, true, true );

    //  メッセージロード
    m_pResourceManager->AddLoadRequest( App::BattleTree::BATTLE_TREE_RESOURCE_ID_MESSAGE, print::GetMessageArcId(), GARC_message_battletrainer_select_DAT, false, true );

    //  レイアウトのカーソルロード
    m_pResourceManager->AddLoadRequest( NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
  }


  void BattleTreeApplicationSystem::CreateMessageData()
  {
    //  メッセージ制御を作成
    m_pMessageUtility->LoadFromMemory( GARC_message_battletrainer_select_DAT, m_pResourceManager->GetResourceBuffer( App::BattleTree::BATTLE_TREE_RESOURCE_ID_MESSAGE ) );

    m_pWork->SetMessageUtility( m_pMessageUtility );
  }


  void BattleTreeApplicationSystem::CreateView()
  {
  }


  void BattleTreeApplicationSystem::UpdateView()
  {
    m_pViewList->Update();
  }


  void BattleTreeApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pViewList->Draw( displayNo );
  }

  //! @brief ローディング処理を行います。
  //! @retval true ロード中
  //! @retval false ロード完了
  bool BattleTreeApplicationSystem::Loading()
  {
    bool  bRet  = NetAppLib::System::ApplicationSystemBase::Loading();

    if( bRet == false )
    {
      //  トレーナーアイコンロードを設定して色んな場所から参照できるようにする
      m_pWork->SetTrainerIconUtilty( m_pNetAppTrainerIconUtility );
    }

    return  bRet;
  }

  //! @brief アプリケーションの終了処理を行います。
  //! @retval true 終了処理中
  //! @retval false 終了処理完了
  bool BattleTreeApplicationSystem::End()
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

  bool BattleTreeApplicationSystem::IsExitView()
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
  bool BattleTreeApplicationSystem::IsCreateViewComplete()
  {
    bool  bComplete = true;

    if( m_pViewObjects->InitFunc( m_pWork ) == false )
    {
      bComplete = false;
    }

    return bComplete;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
