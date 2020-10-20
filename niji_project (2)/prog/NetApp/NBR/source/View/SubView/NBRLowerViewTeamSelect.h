// ============================================================================
/*
 * @file NBRLowerViewTeamSelect.h
 * @brief 通信対戦受付アプリ 下画面：チーム選択画面
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_LOWER_VIEW_TEAMSELECT_H_INCLUDE )
#define NBR_LOWER_VIEW_TEAMSELECT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "AppLib/include/Ui/UIInputListener.h"

// ビューベース 
#include "NetApp/NBR/source/View/NBRLowerViewBase.h"
// リスナー
#include "NetApp/NBR/source/View/SubView/NBRLowerViewTeamSelectListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


class NBRLowerViewTeamSelect : public NBRLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRLowerViewTeamSelect );

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
    BATTLE_START_BUTTON_PASSIVE,  //! 勝負開始ボタンをパッシブ
    CANNOT_JOIN_BUTTON_PASSIVE,   //! 参加出来ないボタンをパッシブ
  };

public:
  // コンストラクタ
  NBRLowerViewTeamSelect( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                       app::util::G2DUtil* pG2dUtil,
                       app::util::G2DUtil::LAYOUT_WORK* pLayoutWork );
  // デストラクタ
  virtual~NBRLowerViewTeamSelect();

  // イベントリスナー
  void SetListener( NBRLowerViewTeamSelectListener* pNBRLowerViewTeamSelectListener );
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

  /**
   * @breif  BGM選択部分の表示・非表示
   * @param  isVisible         true = 表示, false = 非表示
   */
  void SetVisibleBgmSelect( const bool isVisible );

  /**
   * @breif  チーム名の設定
   * @param  teamName   チーム名の文字
   */
  void SetTeamName( gfl2::str::StrBuf* teamName );

  /**
   * @breif  チーム名クリア（初期値「ーーー」）にする
   */
  void ClearTeamName();

  /**
   * @breif  BGMテキスト変更
   * @param  id   
   */
  void ChangeBgmText( u8 id, const bool isNiji );

private:

  //-------------------------------------
  /// ボタンID定義
  //=====================================
  enum BUTTON_ID
  {
    BUTTON_ID_TEAM,         ///< 対戦チームボタン
    BUTTON_ID_CANNOT_JOIN,  ///< 参加出来ないボタン
    BUTTON_ID_BATTLE_START, ///< 勝負開始ボタン
    BUTTON_ID_BGM_L,        ///< BGM選択左ボタン
    BUTTON_ID_BGM_R,        ///< BGM選択右ボタン
    BUTTON_ID_RETURN,       ///< 戻るボタン
    BUTTON_MAX,
  };

  void setupButton();

  // テキスト初期化
  void initializeText();

private:

  NBRLowerViewTeamSelectListener*    m_pListener;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_LOWER_VIEW_TEAMSELECT_H_INCLUDE
