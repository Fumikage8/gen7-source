//======================================================================
/**
 * @file TeamSelectEventAppParam.h
 * @date 2015/08/03 19:53:03
 * @author uchida_yuto
 * @brief チーム選択イベントで公開しているパラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_EVENT_APP_PARAM_H_INCLUDED__
#define __TEAM_SELECT_EVENT_APP_PARAM_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

//  前方宣言
GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)

  class RegulationDrawInfo;

GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  //  アプリ結果
  enum EventAppResult
  {
    //  チーム選択せずに終了
    EVENT_APP_RESULT_CANCEL = 0,
    //  チーム選択して終了
    EVENT_APP_RESULT_TEAM_SELECT
  };

  enum
  {
    //  チーム選択でerror値
    TEAM_SELECT_ERROR = 0xffff,
    //  選択したのがQRチームだった
    TEAM_SELECT_QR  = 0xff,
    //  手持ちのチーム選択
    TEAM_SELECT_TEMOCHI = 0x80
  };

  //  大会タイプ
  enum TournamentType
  {
    //  大会
    TOURNAMENT_TYPE_NONE  = 0,

    //  ライブ大会
    TOURNAMENT_TYPE_LIVE,
    //  インターネット大会
    TOURNAMENT_TYPE_NET,
  };

  /**
   * @struct EVENT_APP_PARAM
   * @brief  TeamSelectEventを起動する時の入力と出力データ
   */
  struct EVENT_APP_PARAM
  {
    //  入力構造他
    struct IN_PARAM
    {
      IN_PARAM()
      {
        pRegulationDrawInfo     = NULL;
        bAddLocalHavePokeParty  = true;
        bAddQRPokeParty         = true;
        tournamentType          = TOURNAMENT_TYPE_NONE;
        bOpenRegulationButton   = false;
      }

      NetAppLib::UI::RegulationDrawInfo*      pRegulationDrawInfo;              //! レギュレーションの情報

      //  手持ちポケモンバーティーを選択対象にするか
      bool  bAddLocalHavePokeParty;
      //  QRポケモンパーティーを選択対象にするか
      bool  bAddQRPokeParty;

      //  大会の種類
      TournamentType  tournamentType;

      //  レギュレーション詳細画面へ遷移するボタンを表示するか（デフォルト非表示）
      bool bOpenRegulationButton;
    } in;

    //  出力構造体
    struct OUT_PARAM
    {
      OUT_PARAM()
      {
        result        = EVENT_APP_RESULT_CANCEL;
        teamIdx       = TEAM_SELECT_ERROR;
        pPokeParty    = NULL;
      }

      //  アプリ終了時の結果
      EventAppResult  result;

      //  アプリ内で選択したチーム選択の配列idx
      //  result == EVENT_APP_RESULT_TEAM_SELECT の場合に値が入っている
      //  TEAM_SELECT_IDX_QR の値が入ったいた場合はQR用のチームを選択した
      u32 teamIdx;

      //  チーム選択したポケモンパーティーを出力
      //  使う側でメモリは確保しないといけない
      pml::PokeParty*     pPokeParty;

    } out;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_EVENT_APP_PARAM_H_INCLUDED__