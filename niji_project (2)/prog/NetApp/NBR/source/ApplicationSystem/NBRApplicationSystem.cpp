// ============================================================================
/*
 * @file NBRApplicationSystem.cpp
 * @brief 通信対戦受付アプリのアプリケーションシステムです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/ApplicationSystem/NBRApplicationSystem.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRBuilder.h"
#include "NetApp/NBR/source/NBRResourceID.h"

#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/NBR.gaix>
#include <arc_index/RegulationSelect.gaix>
#include <arc_index/MenuWindow.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


NBRApplicationSystem::NBRApplicationSystem() :
  m_pParentView( NULL )
 ,m_pNet(NULL)
{
}


NBRApplicationSystem::~NBRApplicationSystem()
{
  GFL_SAFE_DELETE( m_pParentView );
}


void NBRApplicationSystem::Build( app::util::Heap* pAppHeap, NetApp::NBR::ApplicationSystem::NBRBuilder* pApplicationSystemBuilder, NBRNet* pNet )
{
  // 親ビュー作成
  m_pParentView = GFL_NEW( pAppHeap->GetDeviceHeap() ) View::NBRParentView( pApplicationSystemBuilder->GetWork() ); 
  pApplicationSystemBuilder->SetParentView( m_pParentView );

  // 通信クラス保存(実体はEvent）
  m_pNet = pNet;

  // ビューを渡してからビルド
  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void NBRApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NBR_RESOURCE_ID_NBR,            ARCID_NBR,          GARC_NBR_NBR_applyt_COMP,               true, true );
  m_pResourceManager->AddLoadRequest( NBR_RESOURCE_ID_COMMON_WINDOW,  ARCID_MENU_WINDOW,  GARC_MenuWindow_MenuWindow_applyt_COMP, true, true);
  m_pResourceManager->AddLoadRequest( NBR_RESOURCE_ID_MENU_CURSOR,    ARCID_MENU_CURSOR,  GARC_MenuCursor_MenuCursor_applyt_COMP, true, true);
  m_pResourceManager->AddLoadRequest( NBR_RESOURCE_ID_REGULATIONSELECT,ARCID_REGULATION_SELECT, GARC_RegulationSelect_RegulationSelect_applyt_COMP, true, true );
}


void NBRApplicationSystem::CreateView()
{
  // ビルドで生成したもので親ビューに必要なものを渡す
  m_pParentView->Initialize( m_pResourceManager, m_pAppRenderingManager, *m_pNetAppTrainerIconUtility );
}

//! @brief ビューの作成が完了したか
bool NBRApplicationSystem::IsCreateViewComplete()
{
  return m_pParentView->IsInitialize();
}

//! @brief 独自Viewの更新
void NBRApplicationSystem::UpdateView()
{
  if( m_pParentView != NULL )
  {
    m_pParentView->UpdateTree();
  }
}

//! @brief 独自Viewの描画
void NBRApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_pParentView != NULL )
  {
    m_pParentView->DrawTree( displayNo );
  }
}

//! @brief 独自Viewが破棄可能か
bool NBRApplicationSystem::IsExitView()
{
  if( m_pParentView != NULL && m_pParentView->IsDrawing() )
  {// 破棄中
    return false;
  }
  // 破棄OK
  return true;
}

//! @brief 独自通信クラスの作成
void NBRApplicationSystem::CreateNet()
{
  // NBREventに移植
  //m_pNet->Initialize();
}

//! @brief 独自通信クラスが完了したか
bool NBRApplicationSystem::IsCreateNetComplete()
{
  // NBREventに移植
  //return m_pNet->IsInitialize();
  return true;
}

//! @brief 独自通信クラスの更新
void NBRApplicationSystem::UpdateNet()
{
  if( m_pNet != NULL )
  {
    m_pNet->Update();
  }
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
