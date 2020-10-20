// ============================================================================
/*
 * @file NBRParentView.h
 * @brief 通信対戦受付アプリ 親ビュー
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBRPARENTVIEW_H_INCLUDE )
#define NBRPARENTVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "AppLib/include/Ui/UIInputListener.h"

#include "AppLib/include/Ui/UIView.h"

#include "NetApp/NBR/source/View/NBRViewDefine.h"

// サブビュー
#include "NetApp/NBR/source/View/SubView/NBRUpperViewVs.h"
#include "NetApp/NBR/source/View/SubView/NBRUpperViewPlayerInfoSimple.h"
#include "NetApp/NBR/source/View/SubView/NBRUpperViewPlayerInfoDefault.h"
#include "NetApp/NBR/source/View/SubView/NBRUpperViewRegulation.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButton.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerView2Button.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewTeamSelect.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewMultiSelect.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewMessageMenu.h"

// 前方宣言
namespace NetAppLib{
  namespace System{
    class ResourceManager;
    class ApplicationWorkBase;
  }
}
namespace app{
  namespace util{
    class AppRenderingManager;
  }
}


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


// 親ビュー（メインビュー）
class NBRParentView : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRParentView );
public:
  // コンストラクタ
  NBRParentView( NetAppLib::System::ApplicationWorkBase* pWorkBase );
  // デストラクタ
  virtual ~NBRParentView();

  // 初期化
  void Initialize( NetAppLib::System::ResourceManager*        pResourceManager,
                   app::util::AppRenderingManager*            pAppRenderingManager,
                   NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility );
  // 初期化待ち
  bool IsInitialize(); 

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // 描画中か
  virtual bool IsDrawing(void) const;

public:
  // 各サブビュー取得
  // 上画面
  NBRUpperViewVs*                 GetUpperViewVs()                const { return m_pUpperViewVs; };
  NBRUpperViewPlayerInfoSimple*   GetUpperViewPlayerInfoSimple()  const { return m_pUpperViewPlayerInfoSimple; };
  NBRUpperViewPlayerInfoDefault*  GetUpperViewPlayerInfoDefault() const { return m_pUpperViewPlayerInfoDefault; };
  NBRUpperViewRegulation*         GetUpperViewRegulation()        const { return m_pUpperViewRegulation; };
  // 下画面
  NBRLowerViewReturnButton*       GetLowerViewReturnButton()      const { return m_pLowerViewReturnButton; };
  NBRLowerView2Button*            GetLowerView2Button()           const { return m_pLowerView2Button; };
  NBRLowerViewTeamSelect*         GetLowerViewTeamSelect()        const { return m_pLowerViewTeamSelect; };
  NBRLowerViewMultiSelect*        GetLowerViewMultiSelect()       const { return m_pLowerViewMultiSelect; };

  // サブビューの変更（=AddSubView)
  void ChangeUpperSubView( UpperViewID viewId );
  void ChangeLowerSubView( LowerViewID viewId );

  // 下画面共通ビューが取得したい時
  NBRLowerViewBase*               GetLowerViewBase();
  // 何時でも使えるメッセージメニュー
  NBRLowerViewMessageMenu*        GetLowerViewMessageMenu()       const { return m_pLowerViewMessageMenu; };

private:

  // レイアウト構築
  void setupLayoutSystem( NetAppLib::System::ResourceManager* pResourceManager );

  // サブビュー作成
  void createSubView( NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility );
  // サブビュー削除
  void deleteSubView();

private:
  NetAppLib::System::ApplicationWorkBase* m_pWork;
  app::util::Heap*                  m_pHeap;      ///< ヒープメモリ
  app::util::G2DUtil::LAYOUT_WORK   m_layoutWork[ LAYOUT_WORK_ID_MAX ]; // レイアウトワーク
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  // サブビュー
  // 上画面
  NBRUpperViewVs*                 m_pUpperViewVs;
  NBRUpperViewPlayerInfoSimple*   m_pUpperViewPlayerInfoSimple;
  NBRUpperViewPlayerInfoDefault*  m_pUpperViewPlayerInfoDefault;
  NBRUpperViewRegulation*         m_pUpperViewRegulation;

  // 下画面
  NBRLowerViewReturnButton*       m_pLowerViewReturnButton;
  NBRLowerView2Button*            m_pLowerView2Button;
  NBRLowerViewTeamSelect*         m_pLowerViewTeamSelect;
  NBRLowerViewMultiSelect*        m_pLowerViewMultiSelect;
  NBRLowerViewMessageMenu*        m_pLowerViewMessageMenu;  //! このビューは常時Addされている

  // 一時保存用
  NBRUpperViewBase*        m_pNowUpperSubView;
  NBRLowerViewBase*        m_pNowLowerSubView;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBRPARENTVIEW_H_INCLUDE
