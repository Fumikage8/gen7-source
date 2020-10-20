//======================================================================
/**
 * @file TeamSelectAppParam.h
 * @date 2015/07/01 19:29:55
 * @author uchida_yuto
 * @brief チーム選択アプリProcで公開してるパラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_APP_PARAM_H_INCLUDED__
#define __TEAM_SELECT_APP_PARAM_H_INCLUDED__
#pragma once

//  gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_std_string.h>
#include <str/include/gfl2_Str.h>

// pmlib
#include <pml/include/pmlib.h>

//  チーム選択アプリ通信のインクルード
#include "NetApp/TeamSelect/Source/RequestSequence/TeamSelectRequestClient.h"

#include "NetApp/TeamSelect/Include/TeamSelectEventAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(PokeTool)
  class BattleParty;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)

  class RegulationDrawInfo;

GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  //  Procの結果一覧
  enum ProcResultType
  {
    PROC_RESULT_TYPE_NEXT_QR_READ_SCENE = 0,
    PROC_RESULT_TYPE_NEXT_TEAM_SETTING_SCENE,
    PROC_RESULT_TYPE_NEXT_REQULATION_CHK_SCENE,
    PROC_RESULT_TYPE_TEAM_SET,
    PROC_RESULT_TYPE_CANCEL,
    PROC_RESULT_TYPE_ERROR
  };

  //  チームステータス
  enum TeamStatus
  {
    //  チームが空
    TEAM_STATUS_NONE  = 0,
    //  チームがQR読み込み依頼
    TEAM_STATUS_QR_READ_REQUEST,
    //  通常チーム選択
    TEAM_STATUS_PARTY_SELECT
  };

  //  ロックしているタイプ
  enum LockStatus
  {
    //  ロックしていない
    LOCK_STATUS_NONE  = 0,
    LOCK_STATUS_LIVE,
    LOCK_STATUS_NET,
    LOCK_STATUS_FRIEND,
  };

  enum
  {
    CURRENT_TEAM_SELECT_IDX_DEFAULT  = 0xffffff,
    APPOIND_ENTER_TEAM_LIST_IDX_ERROR = 0xffffffff,
  };

  enum
  {
    TEAM_DATA_MAX = 8,
  };

  //  チームデータ
  struct TEAM_DATA
  {
    TEAM_DATA()
    {
      pPokeParty        = NULL;
      status            = TEAM_STATUS_NONE;
      lockStatus        = LOCK_STATUS_NONE;
      no                = 0;
      bPokeTemochiData  = false;
      bQRReaderData     = false;
    }

    //  ポケモンパーティー
    PokeTool::BattleParty*  pPokeParty;

    //  現在のステータス
    TeamStatus          status;

    //  ロックステータス
    LockStatus          lockStatus;

    //  チーム番号
    u32                 no;

    //  手持ちポケモンデータかどうか
    bool                bPokeTemochiData;

    //  QRリーダーのデータがあるか
    //  status == TEAM_STATUS_QR_READ_REQUEST のみ有効
    bool                bQRReaderData;
  };

  //  公開アプリ
  struct APP_PARAM
  {
    APP_PARAM()
    {
      in.Clear();
      out.Clear();
    }

    //  入力パラメータ
    struct _tag_in_param_
    {
      void Clear()
      {
        gfl2::std::MemClear( aTeamData, sizeof( aTeamData ) );

        teamDataNum             = 0;
        bOpenRegulationButton   = true;
        bQRPokePartyButton      = true;
        pRegulationDrawInfo     = NULL;
        currentTeamListIdx      = CURRENT_TEAM_SELECT_IDX_DEFAULT;
        appointEnterTeamListIdx = APPOIND_ENTER_TEAM_LIST_IDX_ERROR;
        tournamentType          = TOURNAMENT_TYPE_LIVE;
      }

      TournamentType  tournamentType;

      //  チーム選択データ
      TEAM_DATA aTeamData[ TEAM_DATA_MAX ];
      u32   teamDataNum;
      u32   currentTeamListIdx;
      s32   appointEnterTeamListIdx;
      bool  bOpenRegulationButton;
      bool  bQRPokePartyButton;

      NetAppLib::UI::RegulationDrawInfo*      pRegulationDrawInfo;              //! レギュレーションの情報

    } in;

    //  出力パラメータ
    struct _tag_out_param_
    {
      void Clear()
      {
        procResultType  = PROC_RESULT_TYPE_ERROR;
        selectTeamIdx   = CURRENT_TEAM_SELECT_IDX_DEFAULT;
      }

      ProcResultType  procResultType;
      u32             selectTeamIdx;
    } out;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_APP_PARAM_H_INCLUDED__