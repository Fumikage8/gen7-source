//======================================================================
/**
 * @file TeamSelectTopPokePartySelectFrame.h
 * @date 2015/07/02 15:19:55
 * @author uchida_yuto
 * @brief 先頭画面のポケモンパーティ選択フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_TOP_POKE_PARTY_SELECT_FRAME_H_INCLUDED__
#define __TEAM_SELECT_TOP_POKE_PARTY_SELECT_FRAME_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/Frame/AppFrameCellSuper.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class PokeIcon;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  class TeamSelectUIViewPool;

  /**
   * @class TeamSelectTopPokePartySelectFrame
   * @brief 先頭画面のポケモンパーティ選択フレーム
   * @note
   *   あくまで終了させるのは使う側でEnd()を呼んで終わらせる
   *   フレーム内で終了させない
   *   なぜなら通信エラーなどでいつ使う側で終了するかのタイミングがわからない
   *   フレーム内で終了と使う側での終了するパターンで混在するので統一して使う側で終了させるようにした
   */
  class TeamSelectTopPokePartySelectFrame : public applib::frame::CellSuper
  {
      GFL_FORBID_COPY_AND_ASSIGN(TeamSelectTopPokePartySelectFrame);
  public:
    enum Push
    {
      PUSH_NONE         = 0,
      PUSH_TEAM_SETTING,
      PUSH_SELECT_POKE_PARTY,
      PUSH_SELECT_QR_PARTY
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

      //  入力
      struct _tag_in_
      {
        void Clear()
        {
          pUIViewPool           = NULL;
          pPokeIconResManager   = NULL;
        }

        TeamSelectUIViewPool* pUIViewPool;

        //  ポケモンのアイコン管理
        app::tool::PokeIcon*  pPokeIconResManager;
      } in;

      //  出力
      struct _tag_out_
      {
        void Clear()
        {
          bEnd          = false;
          push          = PUSH_NONE;
          selectTeamIdx = 0;
        }

        bool  bEnd;
        Push  push;
        u32   selectTeamIdx;
      } out;
    };

    TeamSelectTopPokePartySelectFrame() { _Clear(); }
    virtual ~TeamSelectTopPokePartySelectFrame() {}

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
    enum TeamLockMessageEventType
    {
      TEAM_LOCK_MESSAGE_EVENT_TYPE_LIVE = 0,
      TEAM_LOCK_MESSAGE_EVENT_TYPE_NET,
      TEAM_LOCK_MESSAGE_EVENT_TYPE_FRIEND,
    };

    //  UpdateFunc()内のシーケンス一覧
    enum
    {
      SEQ_USER_INPUT = 0,
      SEQ_WAIT_LOCK_MESSAGE,
      SEQ_SELECT_EVENT
    };

    //------------------------------------------------------------------
    /**
     * @brief   入力処理シーケンスを再起動
     * @note
     *          リストViewの入力処理処理が終わった後に再度処理を動かす時に実行
     */
    //------------------------------------------------------------------
    void _ReturnSeqUserInput();

    //------------------------------------------------------------------
    /**
     * @brief チームロックメッセージイベント開始
     */
    //------------------------------------------------------------------
    bool _StartTeamLockMessageEvent( const TeamLockMessageEventType type );

    //  ヘルプメッセージを表示
    void _ShowHelpMessage();

    void _Clear()
    {
      m_pAppParam     = NULL;
      m_moveTime      = 0;
    }

    struct APP_PARAM* m_pAppParam;

    f32 m_moveTime;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_TOP_POKE_PARTY_SELECT_FRAME_H_INCLUDED__