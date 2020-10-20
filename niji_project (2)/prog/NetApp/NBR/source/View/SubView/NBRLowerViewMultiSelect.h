// ============================================================================
/*
 * @file NBRLowerViewMultiSelect.h
 * @brief 通信対戦受付アプリ 下画面：チーム選択画面
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_LOWER_VIEW_MULTISELECT_H_INCLUDE )
#define NBR_LOWER_VIEW_MULTISELECT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "AppLib/include/Ui/UIInputListener.h"

#include "Savedata/include/MyStatus.h"

// ビューベース 
#include "NetApp/NBR/source/View/NBRLowerViewBase.h"
// リスナー
#include "NetApp/NBR/source/View/SubView/NBRLowerViewMultiSelectListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


class NBRLowerViewMultiSelect : public NBRLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRLowerViewMultiSelect );

public:
/**
   * @enum VisibleMode
   * 表示モード定義
   */
  enum VisibleMode
  {
    ALL_VIEW_ON,                  //! 全て表示
    ALL_VIEW_OFF,                 //! 全て非表示
    RETURN_BUTTON_ONLY_ON,        //! 戻るボタンのみ表示
    CURSOR_OFF,                   //! カーソル非表示
    CURSOR_ON,                    //! カーソル表示
    TEAM_DECIDE_BUTTON_PASSIVE,   //! チーム決定ボタンをパッシブ
    TEAM_DECIDE_BUTTON_ACTIVE,    //! チーム決定ボタンをアクティブ
  };

public:
  // コンストラクタ
  NBRLowerViewMultiSelect( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                       app::util::G2DUtil* pG2dUtil,
                       app::util::G2DUtil::LAYOUT_WORK* pLayoutWork );
  // デストラクタ
  virtual~NBRLowerViewMultiSelect();

  // イベントリスナー
  void SetListener( NBRLowerViewMultiSelectListener* pNBRLowerViewMultiSelectListener );
  void RemoveListener();

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

public:

  //--------------------------------------------------------------------------
  /**
  * @breif  ビュー全体の表示・非表示
  * @param  visibleMode   表示モード定義
  */
  //--------------------------------------------------------------------------
  void SetVisible( const VisibleMode visibleMode );

  //--------------------------------------------------------------------------
  /**
  * @breif  テキスト初期化
  */
  //--------------------------------------------------------------------------
  void InitializeText();

  //--------------------------------------------------------------------------
  /**
  * @breif      プレイヤー決定
  * @param[in]  pos         何番目か(0～3)
  * @param[in]  pMyStatus   表示するプレイヤーのマイステータス
  */
  //--------------------------------------------------------------------------
  void DecidePosPlayer( u8 pos, Savedata::MyStatus* pMyStatus );

private:

  //-------------------------------------
  /// ボタンID定義
  //=====================================
  enum BUTTON_ID
  {
    BUTTON_ID_NO_1,         ///< 1番目のプレイヤー選択ボタン
    BUTTON_ID_NO_2,         ///< 2番目のプレイヤー選択ボタン
    BUTTON_ID_NO_3,         ///< 3番目のプレイヤー選択ボタン
    BUTTON_ID_NO_4,         ///< 4番目のプレイヤー選択ボタン
    BUTTON_ID_DECIDE,       ///< 決定ボタン
    BUTTON_ID_RETURN,       ///< 戻るボタン
    BUTTON_MAX,
  };

  void setupButton();

private:

  NBRLowerViewMultiSelectListener*    m_pListener;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_LOWER_VIEW_MULTISELECT_H_INCLUDE
