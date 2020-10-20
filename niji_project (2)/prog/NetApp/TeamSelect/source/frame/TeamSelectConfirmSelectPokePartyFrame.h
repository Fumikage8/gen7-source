//======================================================================
/**
 * @file TeamSelectConfirmSelectPokePartyFrame.h
 * @date 2015/07/03 15:19:55
 * @author uchida_yuto
 * @brief バトルチーム選択確認フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_CONFIRM_SELECT_POKE_PARTY_FRAME_H_INCLUDED__
#define __TEAM_SELECT_CONFIRM_SELECT_POKE_PARTY_FRAME_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/Frame/AppFrameCellSuper.h"

//  アプリでの公開用
#include "NetApp/TeamSelect/include/TeamSelectAppParam.h"
#include "NetApp/TeamSelect/Include/TeamSelectEventAppParam.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  class TeamSelectUIViewPool;

  /**
   * @class TeamSelectConfirmSelectPokePartyFrame
   * @brief バトルチーム選択確認フレーム
   * @note
   *   あくまで終了させるのは使う側でEnd()を呼んで終わらせる
   *   フレーム内で終了させない
   *   なぜなら通信エラーなどでいつ使う側で終了するかのタイミングがわからない
   *   フレーム内で終了と使う側での終了するパターンで混在するので統一して使う側で終了させるようにした
   */
  class TeamSelectConfirmSelectPokePartyFrame : public applib::frame::CellSuper
  {
      GFL_FORBID_COPY_AND_ASSIGN(TeamSelectConfirmSelectPokePartyFrame);
  public:
    enum Push
    {
      PUSH_NONE         = 0,
      PUSH_YES,
      PUSH_NO
    };

    //  アプリパラメータ
    struct APP_PARAM
    {
      APP_PARAM()
      {
        Reset();

        in.Clear();
      };

      void Reset()
      {
        out.Clear();
      }

      struct _tag_in_param_
      {
        void Clear()
        {
          pUIViewPool           = NULL;
          pSelectTeamViewParam  = NULL;
          tournamentType        = TOURNAMENT_TYPE_NONE;
        }

        TournamentType          tournamentType;

        struct TEAM_VIEW_PARAM* pSelectTeamViewParam;
        TeamSelectUIViewPool*   pUIViewPool;
      } in;

      struct _tag_out_param_
      {
        void Clear()
        {
          bEnd          = false;
          push          = PUSH_NONE;
        }

        bool  bEnd;
        Push  push;

      } out;
    };

    TeamSelectConfirmSelectPokePartyFrame() { _Clear(); }
    virtual ~TeamSelectConfirmSelectPokePartyFrame() {}

    //------------------------------------------------------------------
    /**
     * @brief	  動作に必要な引数を登録
     * @note
     *    必ず最初に呼び出さないと後の動作保障はなし
     */
    //------------------------------------------------------------------
    void Initialize( struct APP_PARAM* pAppParam );

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの初期化を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result InitFunc();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン更新を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result UpdateFunc();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン描画を行う関数
     *          純粋仮想ではありません。
     */
    //------------------------------------------------------------------
    virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no);

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの終了を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result EndFunc();

  private:
    void _Clear()
    {
      m_pAppParam = NULL;
    }

    struct APP_PARAM* m_pAppParam;

  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_CONFIRM_SELECT_POKE_PARTY_FRAME_H_INCLUDED__