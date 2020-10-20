// ============================================================================
/*
 * @file NBRUpperViewPlayerInfoSimple.h
 * @brief 通信対戦受付アプリ 上画面：プレイヤー情報（ルール決まってない版）
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_UPPER_VIEW_PLAYERINFOSIMPLE_H_INCLUDE )
#define NBR_UPPER_VIEW_PLAYERINFOSIMPLE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

// ビューベース 
#include "NetApp/NBR/source/View/NBRUpperViewBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


class NBRUpperViewPlayerInfoSimple : public NBRUpperViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRUpperViewPlayerInfoSimple );
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

  //-------------------------------------
  /// プレイヤー用ペイン表示構造体
  //=====================================
  struct PlayerPane{
    gfl2::lyt::LytPaneIndex   root;       ///< 親
  };

  // コンストラクタ
  NBRUpperViewPlayerInfoSimple( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                  app::util::G2DUtil* pG2dUtil,
                  app::util::G2DUtil::LAYOUT_WORK* pLayoutWork,
                  NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility );
  // デストラクタ
  virtual~NBRUpperViewPlayerInfoSimple();

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

private:
  static const PlayerPane     m_playerPane[ PLACE_MAX ];      ///< プレイヤー表示用ペイン



};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_UPPER_VIEW_PLAYERINFOSIMPLE_H_INCLUDE
