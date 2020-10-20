//======================================================================
/**
 * @file TeamSelectErrorRegulationFrame.h
 * @date 2015/07/03 15:19:55
 * @author uchida_yuto
 * @brief バトルチーム選択レギュレーション違反フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_ERROR_REGULATION_FRAME_H_INCLUDED__
#define __TEAM_SELECT_ERROR_REGULATION_FRAME_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_std_string.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"

#include "NetApp/TeamSelect/source/RequestSequence/TeamSelectResponseListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  class TeamSelectUIViewPool;

  /**
   * @class TeamSelectErrorRegulationFrame
   * @brief バトルチーム選択確認フレーム
   * @note
   *   あくまで終了させるのは使う側でEnd()を呼んで終わらせる
   *   フレーム内で終了させない
   *   なぜなら通信エラーなどでいつ使う側で終了するかのタイミングがわからない
   *   フレーム内で終了と使う側での終了するパターンで混在するので統一して使う側で終了させるようにした
   */
  class TeamSelectErrorRegulationFrame : public applib::frame::CellSuper, public app::tool::InfoWindowBookTypeListener
  {
      GFL_FORBID_COPY_AND_ASSIGN(TeamSelectErrorRegulationFrame);
  public:
    enum Push
    {
      PUSH_NONE         = 0,
      PUSH_CANCEL,
      PUSH_CHK_REGULATION
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
          pUIViewPool = NULL;

          NGTypeNum   = 0;
          bOpenRegulationButton  = true;
          gfl2::std::MemClear( aResultList, sizeof( aResultList ) );
        }

        TeamSelectUIViewPool* pUIViewPool;

        //  レギュレーション違反遷移可能か
        bool  bOpenRegulationButton;

        //  違反データ数
        u32 NGTypeNum;
        //  違反データ内容
        REGULATION_CHECK_RESULT  aResultList[ REGULATION_NG_TYPE_MAX ];

      } in;

      struct _tag_out_param_
      {
        void Clear()
        {
          bEnd  = false;
          push  = PUSH_NONE;
        }

        bool  bEnd;
        Push  push;
      } out;
    };

    TeamSelectErrorRegulationFrame() { _Clear(); }
    virtual ~TeamSelectErrorRegulationFrame() {}

    //------------------------------------------------------------------
    /**
     * @brief   動作に必要な引数を登録
     * @note
     *    必ず最初に呼び出さないと後の動作保障はなし
     */
    //------------------------------------------------------------------
    void Initialize( struct APP_PARAM* pAppParam );

    //------------------------------------------------------------------
    /**
     * @brief   PROCの初期化を行う関数
     * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result InitFunc();

    //------------------------------------------------------------------
    /**
     * @brief   PROCのメイン更新を行う関数
     * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result UpdateFunc();

    //------------------------------------------------------------------
    /**
     * @brief   PROCのメイン描画を行う関数
     *          純粋仮想ではありません。
     */
    //------------------------------------------------------------------
    virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no);

    //------------------------------------------------------------------
    /**
     * @brief   PROCの終了を行う関数
     * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result EndFunc();

//  app::tool::InfoWindowBookTypeListener の仮想関数
    //! @brief  クローズし切ったときに呼ばれるイベントハンドラ
    virtual void OnClose();

    //! @brief  ページが変わり切ったときに呼ばれるイベントハンドラ
    //! @param[in]  pageNo  これから表示するページ番号(0から開始。表示上の1ページ目が0になる。)
    //! @note  オープンし切ったときにも呼ばれる
    virtual void OnChangePage(u32 /*pageNo*/);

    //! @brief  コマンドのボタン(Aボタン)を押し切ったときに呼ばれるイベントハンドラ
    //! @param[in]  pageNo  表示されているページ番号(0から開始。表示上の1ページ目が0になる。)
    virtual void OnCommand(u32 /*pageNo*/);

  private:
    //------------------------------------------------------------------
    /**
     * @brief     チームビューのマスク描画更新
     * @param[in] 違反したレギュレーションデータidx
     */
    //------------------------------------------------------------------
    void _UpdateTeamViewMaskDraw( const u32 errorRegulationDataIdx );

    void _Clear()
    {
      m_pAppParam               = NULL;
      m_pEnterButtonTextStrBuf  = NULL;
      gfl2::std::MemClear( m_pErrorMessageStrBufArray, sizeof( m_pErrorMessageStrBufArray ) );
    }

    struct APP_PARAM* m_pAppParam;
    gfl2::str::StrBuf*  m_pEnterButtonTextStrBuf;
    gfl2::str::StrBuf*  m_pErrorMessageStrBufArray[ REGULATION_NG_TYPE_MAX ];
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_ERROR_REGULATION_FRAME_H_INCLUDED__