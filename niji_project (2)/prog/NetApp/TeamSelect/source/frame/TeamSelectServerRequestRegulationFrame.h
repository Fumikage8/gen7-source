//======================================================================
/**
 * @file TeamSelectServerRequestRegulationFrame.h
 * @date 2015/07/03 15:19:55
 * @author uchida_yuto
 * @brief バトルチーム選択サーバー通知レギュレーション
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_SERVER_REQUEST_REGULATION_FRAME_H_INCLUDED__
#define __TEAM_SELECT_SERVER_REQUEST_REGULATION_FRAME_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_std_string.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"

//  チーム選択アプリ通信のインクルード
#include "NetApp/TeamSelect/Source/RequestSequence/TeamSelectRequestClient.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  class TeamSelectUIViewPool;

  /**
   * @class TeamSelectServerRequestRegulationFrame
   * @brief バトルチーム選択確認フレーム
   * @note
   *   あくまで終了させるのは使う側でEnd()を呼んで終わらせる
   *   フレーム内で終了させない
   *   なぜなら通信エラーなどでいつ使う側で終了するかのタイミングがわからない
   *   フレーム内で終了と使う側での終了するパターンで混在するので統一して使う側で終了させるようにした
   */
  class TeamSelectServerRequestRegulationFrame : public applib::frame::CellSuper, TeamSelectResponseListener
  {
      GFL_FORBID_COPY_AND_ASSIGN(TeamSelectServerRequestRegulationFrame);
  public:
    //  サーバー結果一覧
    enum ServerResultType
    {
      SERVER_RESULT_TYPE_NONE     = 0,
      SERVER_RESULT_TYPE_SUCCESS,
      SERVER_RESULT_TYPE_REGULATION_NG,
      SERVER_RESULT_TYPE_POKE_ERROR,
      SERVER_RESULT_TYPE_ERROR
    };

    //  サーバーの結果フォーマット
    typedef struct __result_server_data_
    {
      ServerResultType  type;

      union
      {
        struct
        {
          u32 NGTypeNum;
          //  最大数を確保
          REGULATION_CHECK_RESULT  aResultList[ REGULATION_NG_TYPE_MAX ];

        } regulationNG;

      } data;

      __result_server_data_()
      {
        Clear();
      }

      void Clear()
      {
        type  = SERVER_RESULT_TYPE_NONE;
        gfl2::std::MemClear( &data, sizeof( data ) );
      }

    } RESULT_SERVER_DATA;

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
          pServerRequestClient  = NULL;
        }

        TeamSelectRequestClient*  pServerRequestClient;
        TeamSelectUIViewPool*     pUIViewPool;

        //  通信クライアントに渡すリクエストパラメータ
        TeamSelectRequestClient::CHECK_REQUEST_PARAM      serverRequestClientParam;

      } in;

      struct _tag_out_param_
      {
        void Clear()
        {
          resultServerData.Clear();
          bEnd  = false;
        }

        RESULT_SERVER_DATA    resultServerData;
        bool                  bEnd;
      } out;
    };

    TeamSelectServerRequestRegulationFrame() { _Clear(); }
    virtual ~TeamSelectServerRequestRegulationFrame() {}

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

    //  ここからは TeamSelectResponseListenrの関数
    //--------------------------------------------------------------------------------------------
    /**
     * @brief   チェック成功
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnCheckSuccess();

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   レギュレーション違反
     * @param   NGTypeNum            NGになった検査種別の数（＝違反表示画面のページ数）
     * @param   checkPokeResultArray 検査結果の配列(NGTypeNum - 1までアクセス可能)
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnRegulationNG( u32 NGTypeNum , REGULATION_CHECK_RESULT* checkResultArray );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   ポケモンが不正
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnValidationNG();

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   通信エラー
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnError();

  private:
    void _Clear()
    {
      m_pAppParam             = NULL;
    }

    struct APP_PARAM* m_pAppParam;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_SERVER_REQUEST_REGULATION_FRAME_H_INCLUDED__