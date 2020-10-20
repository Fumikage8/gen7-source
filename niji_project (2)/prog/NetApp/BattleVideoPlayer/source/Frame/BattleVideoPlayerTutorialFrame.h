//=============================================================================
/**
 * @file	 BattleVideoPlayerTutorialFrame.h
 * @brief	 初回起動フレーム
 * @author yuto_uchida
 * @date	 2015.09.24
 */
//=============================================================================
#ifndef BATTLE_VIDEO_PLAYER_TUTORIAL_FRAME_H_INCLUDED
#define BATTLE_VIDEO_PLAYER_TUTORIAL_FRAME_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"

//  UIViewのインクルード
#include "NetApp/BattleVideoPlayer/source/BattleVideoPlayerUIViewPool.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  class UIViewPool;

  /** 初回起動フレーム */
  class TutorialFrame : 
  public applib::frame::CellSuper,
  public app::tool::GeneralLowerView::IEventHandler
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(TutorialFrame);

  public:
    enum Result
    {
      RESULT_NONE  = 0,
      RESULT_SELECT_OK,
      RESULT_SELECT_CANCEL
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
        }

        //  UIViewプール
        UIViewPool* pUIViewPool;
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
    TutorialFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~TutorialFrame();

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

    //  選択のコールバック
    virtual void  GeneralLowerView_OnEvent( app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode );

  private:
    //  選択ボタン表示開始/終了!
    void _StartSelectButtonsSelectView();
    void _EndSelectButtonsSelectView();

    //  選択ボタンを押した
    bool _IsPushSelect();

    //  変数初期化
    void _Clear()
    {
      m_pFrameParam = NULL;
      m_selectButtonsEvent = app::tool::GeneralLowerView::IEventHandler::EVENT_;
    }

    FRAME_PARAM*  m_pFrameParam;

    app::tool::GeneralLowerView::IEventHandler::EventCode  m_selectButtonsEvent;
};

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )

#endif // BATTLE_VIDEO_PLAYER_TUTORIAL_FRAME_H_INCLUDED
