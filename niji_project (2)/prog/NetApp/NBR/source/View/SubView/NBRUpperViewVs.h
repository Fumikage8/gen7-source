// ============================================================================
/*
 * @file NBRUpperViewVs.h
 * @brief 通信対戦受付アプリ 上画面：VS
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_UPPER_VIEW_VS_H_INCLUDE )
#define NBR_UPPER_VIEW_VS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

// ビューベース 
#include "NetApp/NBR/source/View/NBRUpperViewBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


class NBRUpperViewVs : public NBRUpperViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRUpperViewVs );
public:
  NBRUpperViewVs( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                  app::util::G2DUtil* pG2dUtil,
                  app::util::G2DUtil::LAYOUT_WORK* pLayoutWork,
                  NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility );
  virtual~NBRUpperViewVs();

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
  * @param[in]  no          立ち位置Index
  * @param[in]  playerData  表示するプレイヤーデータ
  */
  //--------------------------------------------------------------------------
  void SetPlayerData( u8 no, Savedata::MyStatus* pMyStatus );

private:

  // テキスト初期化
  void InitializeText();


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_UPPER_VIEW_VS_H_INCLUDE
