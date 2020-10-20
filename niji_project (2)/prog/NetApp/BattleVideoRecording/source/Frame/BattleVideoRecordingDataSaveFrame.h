//=============================================================================
/**
 * @file	 BattleVideoRecordingDataSaveFrame.h
 * @brief	 データ保存フレーム
 * @author yuto_uchida
 * @date	 2015.09.16
 */
//=============================================================================
#ifndef BATTLE_VIDEO_RECORDING_DATA_SAVE_FRAME_H_INCLUDED
#define BATTLE_VIDEO_RECORDING_DATA_SAVE_FRAME_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"

//  UIViewのインクルード
#include "NetApp/BattleVideoRecording/source/BattleVideoRecordingUIViewPool.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( ExtSavedata )

  class BattleRecorderSaveData;

GFL_NAMESPACE_END( ExtSavedata )

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoRecording )

  class UIViewPool;

  /** データチェックフレーム */
  class DataSaveFrame : 
  public applib::frame::CellSuper,
  public app::tool::GeneralLowerView::IEventHandler
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(DataSaveFrame);

  public:
    enum Result
    {
      RESULT_NONE  = 0,
      RESULT_SAVE_SUCCESS,
      RESULT_SAVE_CANCEL,
      RESULT_SAVE_ERROR,
      RESULT_SAVE_FATAL,
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
          pUIViewPool     = NULL;
          pExtSaveData    = NULL;
          pAppHeap        = NULL;
        }

        //  UIViewプール
        UIViewPool* pUIViewPool;

        ExtSavedata::BattleRecorderSaveData*  pExtSaveData;

        app::util::Heap*                      pAppHeap;
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
        }

        Result  result;
      } out;
    };

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    DataSaveFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~DataSaveFrame();

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

    //  2選択のコールバック
    virtual void  GeneralLowerView_OnEvent( app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode );

  private:
    //  ２選択ボタン表示開始/終了!
    void _StartTwoButtonsSelectView();
    void _EndTwoButtonsSelectView();

    //  変数初期化
    void _Clear()
    {
      m_pFrameParam           = NULL;
      m_twoButtonsEvent       = app::tool::GeneralLowerView::IEventHandler::EVENT_;
      m_endShopMessageNextSeq = 0;
      m_bFirstWaitAsyncSeq    = true;
    }

    FRAME_PARAM*  m_pFrameParam;

    u32 m_endShopMessageNextSeq;
    bool  m_bFirstWaitAsyncSeq;

    app::tool::GeneralLowerView::IEventHandler::EventCode   m_twoButtonsEvent;
};

GFL_NAMESPACE_END( BattleVideoRecording )
GFL_NAMESPACE_END( NetApp )

#endif // BATTLE_VIDEO_RECORDING_DATA_SAVE_FRAME_H_INCLUDED
