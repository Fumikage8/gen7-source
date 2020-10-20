// ============================================================================
/*
 * @file EndRollApplicationSystem.cpp
 * @brief エンドロールアプリのアプリケーションシステムです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#include "App/EndRoll/source/System/EndRollApplicationSystem.h"
#include "App/EndRoll/source/System/EndRollBuilder.h"
#include "App/EndRoll/source/EndRollResourceID.h"

//  Viewのインクルード
#include "App/EndRoll/source/System/EndRollViewList.h"
#include "App/EndRoll/source/System/EndRollViewObjects.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <arc_def_index/arc_def.h>
#include <arc_index/EndrollLyt.gaix>
#include <arc_index/EndrollGrpFont.gaix>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)


EndRollApplicationSystem::EndRollApplicationSystem() :
  m_pEndRollWork( NULL )
{
  _Clear();
}


EndRollApplicationSystem::~EndRollApplicationSystem()
{
  GFL_SAFE_DELETE( m_pViewSystemAccessor );
  GFL_SAFE_DELETE( m_pViewList );
  GFL_SAFE_DELETE( m_pViewObjects );
}


void EndRollApplicationSystem::Build( App::EndRoll::System::EndRollWork* pEndRollWork, App::EndRoll::System::EndRollBuilder* pApplicationSystemBuilder )
{
  m_pEndRollWork = pEndRollWork;

  ApplicationSystemBase::Build( pApplicationSystemBuilder );

  //  上下画面の色を黒にする（終了したら元に戻す）
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

    u32 loopNum = GFL_NELEMS( m_aOldClearSetting );
    GameSys::GameManager::BUFFER_CLEAR_SETTING  newClearSetting;
    for( u32 i = 0; i < loopNum; ++i )
    {
      pGameManager->GetBufferClearSetting( (gfl2::gfx::CtrDisplayNo)i, &m_aOldClearSetting[ i ] );

      //  copy
      {
        newClearSetting = m_aOldClearSetting[ i ];
        newClearSetting.color.SetR( 0.0f );
        newClearSetting.color.SetG( 0.0f );
        newClearSetting.color.SetB( 0.0f );
      }

      pGameManager->SetBufferClearSetting( (gfl2::gfx::CtrDisplayNo)i, newClearSetting );
    }
  }

  app::util::Heap* pAppHeap = m_pEndRollWork->GetAppHeap();
  GFL_ASSERT( pAppHeap );

  //  View関連のインスタンス作成
  {
    m_pViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::EndRoll::System::EndRollViewList( pAppHeap );
    m_pViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::EndRoll::System::EndRollViewObjects();
    m_pViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::EndRoll::System::EndRollViewSystemAccessor( m_pViewList, m_pViewObjects );

    //  workにViewのアクセサを参照
    m_pEndRollWork->SetViewSystemAccessor( m_pViewSystemAccessor );
  }
}


void EndRollApplicationSystem::LoadResourceRequest()
{
  //  レイアウトデータをロード
  {
    m_pResourceManager->AddLoadRequest( App::EndRoll::APPENDROLL_RESOURCE_ID_LAYOUT, ARCID_END_ROLL_LYT_DATA, GARC_EndrollLyt_EndrollLyt_applyt_COMP, true, true );

    //  グラフィックフォントをロード
    //  日本語と英語しか出さないがデータ９言語分に日本語と英語のみのフォントを用意するのでプログラム側で指定の必要はない
    {
      m_pResourceManager->AddLoadRequest( App::EndRoll::APPENDROLL_RESOURCE_ID_GRPFONT_LAYOUT, ARCID_ENDROLL_GRPFONT, GARC_EndrollGrpFont_EndrollGrpFont_applyt_COMP, true, true );
    }
  }
}


void EndRollApplicationSystem::CreateMessageData()
{
}


void EndRollApplicationSystem::CreateView()
{
}


void EndRollApplicationSystem::UpdateView()
{
  m_pViewList->Update();
}


void EndRollApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pViewList->Draw( displayNo );
}


bool EndRollApplicationSystem::IsExitView()
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
bool EndRollApplicationSystem::End()
{
  switch( m_endSeq )
  {
    case 0:
    {
      //  最初にViewの破棄をする
      if( m_pViewObjects->EndFunc() == false )
      {
        break;
      }

      ++m_endSeq;

      //  すぐに処理するために break スルー
    }

    case 1:
    {
      if( NetAppLib::System::ApplicationSystemBase::End() == false )
      {
        //  上下画面の色を元に戻す
        {
          GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

          u32 loopNum = GFL_NELEMS( m_aOldClearSetting );
          for( u32 i = 0; i < loopNum; ++i )
          {
            pGameManager->SetBufferClearSetting( (gfl2::gfx::CtrDisplayNo)i, m_aOldClearSetting[ i ] );
          }
        }

        return  false;
      }

      break;
    }
  }

  return  true;
}

//! @brief ビューの作成が完了したか
bool EndRollApplicationSystem::IsCreateViewComplete()
{
  bool  bComplete = true;
  if( m_pViewObjects->InitFunc( m_pEndRollWork ) == false )
  {
    bComplete = false;
  }

  return bComplete;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)
