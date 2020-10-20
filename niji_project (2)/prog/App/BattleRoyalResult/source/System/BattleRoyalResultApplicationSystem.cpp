// ============================================================================
/*
 * @file BattleRoyalResultApplicationSystem.cpp
 * @brief バトルロイヤルの結果アプリのアプリケーションシステムです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#include "App/BattleRoyalResult/source/System/BattleRoyalResultApplicationSystem.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultBuilder.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultWork.h"

//  Viewのインクルード
#include "App/BattleRoyalResult/source/System/BattleRoyalResultViewList.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultViewObjects.h"

/// idのインクルード
#include "App/BattleRoyalResult/source/BattleRoyalResultResourceID.h"

//  nijiのインクルード
#include "System/include/ArcIdGetter.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <arc_def_index/arc_def.h>
#include <arc_index/BattleRoyalResultLyt.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)


  BattleRoyalResultApplicationSystem::BattleRoyalResultApplicationSystem() :
    m_pWork( NULL )
  {
    _Clear();
  }


  BattleRoyalResultApplicationSystem::~BattleRoyalResultApplicationSystem()
  {
    GFL_SAFE_DELETE( m_pViewSystemAccessor );
    GFL_SAFE_DELETE( m_pViewList );
    GFL_SAFE_DELETE( m_pViewObjects );
  }


  void BattleRoyalResultApplicationSystem::Build( App::BattleRoyalResult::System::BattleRoyalResultWork* pBattleRoyalResultWork, App::BattleRoyalResult::System::BattleRoyalResultBuilder* pApplicationSystemBuilder )
  {
    GFL_ASSERT( pBattleRoyalResultWork );

    m_pWork = pBattleRoyalResultWork;

    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    GFL_ASSERT( pAppHeap );

    //  View関連のインスタンス作成
    {
      m_pViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleRoyalResult::System::BattleRoyalResultViewList( pAppHeap );
      m_pViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleRoyalResult::System::BattleRoyalResultViewObjects();
      m_pViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleRoyalResult::System::BattleRoyalResultViewSystemAccessor( m_pViewList, m_pViewObjects );

      //  workにViewのアクセサを参照
      m_pWork->SetViewSystemAccessor( m_pViewSystemAccessor );
    }

    ApplicationSystemBase::Build( pApplicationSystemBuilder );
  }


  void BattleRoyalResultApplicationSystem::LoadResourceRequest()
  {
    //  レイアウトデータをロード
    m_pResourceManager->AddLoadRequest( App::BattleRoyalResult::BATTLE_ROYAL_RESULT_RESOURCE_ID_LAYOUT, ARCID_BATTLE_ROYAL_RESULT_LYT_DATA, GARC_BattleRoyalResultLyt_BattleRoyalResultLyt_applyt_COMP, true, true );

    //  メッセージのロード
    m_pResourceManager->AddLoadRequest( App::BattleRoyalResult::BATTLE_ROYAL_RESULT_RESOURCE_ID_MESSAGE, print::GetMessageArcId(), GARC_message_battleroyal_result_DAT, false, true );
  }


  void BattleRoyalResultApplicationSystem::CreateMessageData()
  {
    //  メッセージ制御を作成
    m_pMessageUtility->LoadFromMemory( GARC_message_battleroyal_result_DAT, m_pResourceManager->GetResourceBuffer( App::BattleRoyalResult::BATTLE_ROYAL_RESULT_RESOURCE_ID_MESSAGE ) );
    m_pWork->SetMessageUtility( m_pMessageUtility );
  }


  void BattleRoyalResultApplicationSystem::CreateView()
  {
    //  トレーナーアイコンを設定
    m_pWork->SetTrainerIconUtil( m_pNetAppTrainerIconUtility );
  }


  void BattleRoyalResultApplicationSystem::UpdateView()
  {
    m_pViewList->Update();
  }


  void BattleRoyalResultApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pViewList->Draw( displayNo );
  }


  bool BattleRoyalResultApplicationSystem::IsExitView()
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

  //! @brief アプリケーションの終了処理を行います。
  //! @retval true 終了処理中
  //! @retval false 終了処理完了
  bool BattleRoyalResultApplicationSystem::End()
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

  //! @brief ビューの作成が完了したか
  bool BattleRoyalResultApplicationSystem::IsCreateViewComplete()
  {
    bool  bComplete = true;

    m_pNetAppTrainerIconUtility->Update();
    if( m_pViewObjects->InitFunc( m_pWork ) == false )
    {
      bComplete = false;
    }

    return bComplete;
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)
