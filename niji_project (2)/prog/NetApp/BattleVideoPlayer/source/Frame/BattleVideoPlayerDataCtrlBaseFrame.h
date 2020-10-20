//=============================================================================
/**
 * @file	 BattleVideoPlayerDataListFrame.h
 * @brief	 データ操作フレーム(共通機能)
 * @author yuto_uchida
 * @date	 2015.10.01
 */
//=============================================================================
#ifndef BATTLE_VIDEO_PLAYER_DATA_CTRL_BASE_FRAME_H_INCLUDED
#define BATTLE_VIDEO_PLAYER_DATA_CTRL_BASE_FRAME_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"

//  UIViewのインクルード
#include "NetApp/BattleVideoPlayer/source/BattleVideoPlayerUIViewPool.h"

#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerProcAppParam.h"
#include "NetApp/BattleVideoPlayer/source/frame/BattleVideoPlayerDataDelFlow.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  class UIViewPool;

  /** データ操作基本フレーム */
  class DataCtrlBaseFrame : 
  public applib::frame::CellSuper
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(DataCtrlBaseFrame);

  public:
    enum Result
    {
      RESULT_NONE = 0,
      RESULT_CANCEL,
      RESULT_VIDEO_PLAY,
      RESULT_BATTLE_AI,
      RESULT_VIDEO_DEL,
      RESULT_FATAL,
    };

    struct FRAME_PARAM
    {
      struct IN_DATA
      {
        IN_DATA()
        {
          Reset();
        }

        void Reset()
        {
          pUIViewPool       = NULL;
          pVideoDataManager = NULL;
          pVideoData        = NULL;
        }

        //  UIViewプール
        UIViewPool* pUIViewPool;

        //  リストが参照するデータ
        NetAppLib::BattleVideoPlayer::VideoDataManager*               pVideoDataManager;

        NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData;
      } in;

      struct OUT_DATA
      {
        OUT_DATA()
        {
          Reset();
        }

        void Reset()
        {
          result  = RESULT_NONE;
          dualTargetIdx = BTL_CLIENT_ENEMY1;
        }

        u32     dualTargetIdx;
        Result  result;

      } out;
    };

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    DataCtrlBaseFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~DataCtrlBaseFrame();

    //------------------------------------------------------------------
    /**
     * @brief	  動作に必要な引数を登録
     * @note
     *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
     */
    //------------------------------------------------------------------
    void Initialize( FRAME_PARAM* pFrameParam );

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

  protected:
    //  選択ボタンidの割り振り
    enum
    {
      BUTTON_ID_VS       = 0,
      BUTTON_ID_UPLOAD,
      BUTTON_ID_DEL,
    };

    //  初期化のシーケンス
    enum
    {
      INIT_ROOT_SEQ_SETTING_INIT = 0,
      INIT_ROOT_SEQ_SETTING_WAIT,
      INIT_ROOT_SEQ_SETTING_END,

      INIT_ROOT_SEQ_OTHER,
    };

    //  更新のシーケンス
    enum
    {
      //  データ操作
      UPDATE_ROOT_SEQ_DATA_CTRL_START  = 0,
      UPDATE_ROOT_SEQ_DATA_CTRL_WAIT,
      UPDATE_ROOT_SEQ_DATA_CTRL_END,

      //  データ削除
      UPDATE_ROOT_SEQ_DATA_DEL_START,
      UPDATE_ROOT_SEQ_DATA_DEL_WAIT,
      UPDATE_ROOT_SEQ_DATA_DEL_END,

      //  模擬の相手選択
      UPDATE_ROOT_SEQ_DATA_PLAYER_SELECT_START,
      UPDATE_ROOT_SEQ_DATA_PLAYER_SELECT_WAIT,
      UPDATE_ROOT_SEQ_DATA_PLAYER_SELECT_END,

      //  バトルチーム選択メッセージ
      UPDATE_ROOT_SEQ_DATA_BTL_TEAM_SELECT_MESSAGE,
      UPDATE_ROOT_SEQ_DATA_BTL_TEAM_SELECT_MESSAGE_END,

      //  汎用メッセージ
      UPDATE_ROOT_SEQ_MESSAGE,
      //  YES/NOメニューメッセージ
      UPDATE_ROOT_SEQ_YES_NO_MENU_MESSAGE,

      UPDATE_ROOT_SEQ_OTHER
    };

    //  破棄のシーケンス
    enum
    {
      END_ROOT_SEQ_START = 0,
      END_ROOT_SEQ_WAIT,
      END_ROOT_SEQ_END,

      END_ROOT_SEQ_OTHER,
    };

    //  操作更新結果
    struct UpdataDataCtrlResult
    {
      UpdataDataCtrlResult()
      {
        Reset();
      }

      void Reset()
      {
        pushBtn = LowDataCtrlView::BUTTON_ID_NONE;
        bCancel = false;
        bFatal  = false;
      };

      LowDataCtrlView::ButtonId pushBtn;
      bool  bCancel;
      bool  bFatal;
    };

    const UpdataDataCtrlResult& GetUpdateDataCtrlResult() const { return m_updateDataCtrlResult; }

    //  シーケンス設定
    virtual u32 _StartSeq( const u32 seq );

    //  メッセージ表示して終ったら指定したシーケンスに遷移
    u32 _ShowMessageSeq( const u32 strId, const u32 retSeq, const bool bShowBG );

    //  メニューウィンドウYES/NOを表示して終ったら指定したシーケンスに遷移
    u32 _ShowYesNoMenuMessageSeq( const u32 strId, const u32 yesRetSeq, const u32 noRetSeq, const bool bShowBG );

    FRAME_PARAM*  m_pFrameParam;

  private:
    void _Clear()
    {
      m_pFrameParam = NULL;
      m_initResult  = applib::frame::RES_CONTINUE;
      m_endResult   = applib::frame::RES_CONTINUE;

      m_messageRetSeq = 0;
      m_yesNoMenuMessageYesSeq = 0;
      m_yesNoMenuMessageNoSeq  = 0;
    }

    //  データ削除のフロー
    DataDelFlow   m_dataDelFlow;

    UpdataDataCtrlResult  m_updateDataCtrlResult;
    applib::frame::Result m_initResult;
    applib::frame::Result m_endResult;

    u32 m_messageRetSeq;
    u32 m_yesNoMenuMessageYesSeq;
    u32 m_yesNoMenuMessageNoSeq;
};

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )

#endif // BATTLE_VIDEO_PLAYER_DATA_CTRL_BASE_FRAME_H_INCLUDED
