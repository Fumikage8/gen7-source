// ============================================================================
/*
 * @file NBRApplicationSystem.h
 * @brief 通信対戦受付アプリのアプリケーションシステムです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_APPLICATIONSYSTEM_H_INCLUDE )
#define NBR_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

// View
#include "NetApp/NBR/source/View/NBRParentView.h"
// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

class NBRBuilder;

class NBRApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRApplicationSystem );

public:
  NBRApplicationSystem();
  virtual~NBRApplicationSystem();

  void Build( app::util::Heap* pAppHeap, NetApp::NBR::ApplicationSystem::NBRBuilder* pApplicationSystemBuilder, NBRNet* pNet );


private:
  virtual void LoadResourceRequest();
  virtual void CreateView();
  //! @brief ビューの作成が完了したか
  virtual bool IsCreateViewComplete();
  //! @brief 独自Viewの更新
  virtual void UpdateView();
  //! @brief 独自Viewの描画
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  //! @brief 独自Viewが破棄可能か
  virtual bool IsExitView();

  //! @brief 独自通信クラスの作成
  virtual void CreateNet();
  //! @brief 独自通信クラスが完了したか
  virtual bool IsCreateNetComplete();
  //! @brief 独自通信クラスの更新
  virtual void UpdateNet();

private:

  View::NBRParentView*  m_pParentView;    //! 親ビュー
  NBRNet*               m_pNet;           //! 通信クラス外から貰う

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)


#endif // NBR_APPLICATIONSYSTEM_H_INCLUDE
