// ============================================================================
/*
 * @file NBRUpperViewPlayerInfoDefault.h
 * @brief 通信対戦受付アプリ 上画面：プレイヤー情報（ルール決まった後版）
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_UPPER_VIEW_PLAYERINFODEFAULT_H_INCLUDE )
#define NBR_UPPER_VIEW_PLAYERINFODEFAULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

// ビューベース 
#include "NetApp/NBR/source/View/NBRUpperViewBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


class NBRUpperViewPlayerInfoDefault : public NBRUpperViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRUpperViewPlayerInfoDefault );
public:

  //-------------------------------------
  /// 表示位置
  //=====================================
  enum Place{
    PLACE_LEFT,
    PLACE_CENTER,
    PLACE_RIGHT,
    PLACE_MAX
  };


  /**
   * @enum PlayerStatus
   * プレイヤの状態
   */
  enum PlayerStatus{
    PLAYER_STATUS_SELECTING,  //! 選んでいる
    PLAYER_STATUS_FINISH,     //! 完了
  };

  //-------------------------------------
  /// プレイヤー用ペイン表示構造体
  //=====================================
  struct PlayerPane{
    gfl2::lyt::LytPaneIndex   root;       ///< 親
  };


  // コンストラクタ
  NBRUpperViewPlayerInfoDefault( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                  app::util::G2DUtil* pG2dUtil,
                  app::util::G2DUtil::LAYOUT_WORK* pLayoutWork,
                  NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility );
  // デストラクタ
  virtual~NBRUpperViewPlayerInfoDefault();


public:
  //--------------------------------------------------------------------------
  /**
  * @breif  ビュー全体の表示・非表示
  * @param  isVisible         true = 表示, false = 非表示
  */
  //--------------------------------------------------------------------------
  void SetAllVisible( const bool isVisible );

  //--------------------------------------------------------------------------
  /**
  * @breif      プレイヤーの表示
  * @param[in]  place       表示位置
  * @param[in]  pMyStatus   表示するプレイヤーのマイステータス
  */
  //--------------------------------------------------------------------------
  void SetPlayerData( const Place place, Savedata::MyStatus* pMyStatus );

  //--------------------------------------------------------------------------
  /**
  * @breif      プレイヤー状態の設定
  * @param[in]  place     表示位置
  * @param[in]  status    プレイヤー状態
  */
  //--------------------------------------------------------------------------
  void SetPlayerStatus( const Place place, PlayerStatus status );

private:
  static const PlayerPane     m_playerPane[ PLACE_MAX ];      ///< プレイヤー表示用ペイン

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_UPPER_VIEW_PLAYERINFODEFAULT_H_INCLUDE
