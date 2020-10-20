//=============================================================================
/**
 * @file	 BattleVideoPlayerDataListFrame.h
 * @brief	 データ選択フレーム
 * @author yuto_uchida
 * @date	 2015.09.25
 */
//=============================================================================
#ifndef BATTLE_VIDEO_PLAYER_DATA_LIST_FRAME_H_INCLUDED
#define BATTLE_VIDEO_PLAYER_DATA_LIST_FRAME_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/app_tool_MessageMenuView.h"

//  UIViewのインクルード
#include "NetApp/BattleVideoPlayer/source/BattleVideoPlayerUIViewPool.h"

#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerProcAppParam.h"
#include "NetApp/BattleVideoPlayer/source/frame/BattleVideoPlayerDataDelFlow.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  class DataDelFlow;

  /** データ選択共通フレーム */
  class DataListFrame : 
  public applib::frame::CellSuper, 
  public LowDataListViewDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(DataListFrame);

  public:
    enum Result
    {
      RESULT_NONE  = 0,
      RESULT_CANCEL,
      RESULT_NON_DATA,
      RESULT_DATA_SELECT,
      RESULT_DATA_FATAL,
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
        }

        //  リストが参照するデータ
        NetAppLib::BattleVideoPlayer::VideoDataManager*               pVideoDataManager;

        //  リストのスクロールデータ
        LowDataListView::LIST_SCROLL_DATA listScrollData;

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
          result    = RESULT_NONE;
          infoMode  = UIViewPool::UPP_INFO_VIEW_MODE_DETAIL;
          selectDataIdx = 0;
        }

        Result  result;

        u32     selectDataIdx;

        //  インフォモード
        UIViewPool::UppInfoViewMode     infoMode;

        //  リストのスクロールデータ
        LowDataListView::LIST_SCROLL_DATA listScrollData;

      } out;
    };

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    DataListFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~DataListFrame();

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

    //  リストのアクティブ位置
    virtual LowDataListViewDelegate::ActiveItemResult LowDataListViewDelegate_OnActiveCursor( const u32 idx );

  protected:
    //  初期化のシーケンス
    enum
    {
      INIT_ROOT_SEQ_SETTING  = 0,
      INIT_ROOT_SEQ_SETTING_WAIT,

      INIT_ROOT_SEQ_OTHER,
    };

    //  更新のシーケンス
    enum
    {
      //  ビデオデータ選択
      SEQ_SELECT_START  = 0,
      SEQ_SELECT_WAIT,
      SEQ_SELECT_ENTRY,

      //  複数ビデオデータ選択削除
      SEQ_DEL_SELECT_START,
      SEQ_DEL_SELECT_WAIT,

      //  削除フロー
      SEQ_DEL_DATA_START,
      SEQ_DEL_DATA_WAIT,

      SEQ_END,

      SEQ_OTHER,
    };

    //  終了のシーケンス
    enum
    {
      END_ROOT_SEQ_END_FUNC  = 0,
      END_ROOT_SEQ_DATA,

      END_ROOT_SEQ_OTHER,
    };

    //  変数初期化
    void _Clear()
    {
      m_activeSelectIdx = 0;
      m_pFrameParam   = NULL;

      m_initResult  = applib::frame::RES_CONTINUE;
      m_endResult   = applib::frame::RES_CONTINUE;
    }

    //  情報状態設定
    void _SetInfoState( const UIViewPool::UppInfoViewMode mode );

    //  情報切り替え
    void _ChangeInfoState();

    //  データ削除のフロー
    DataDelFlow   m_dataDelFlow;

    //  フレームの外部パラメータ
    FRAME_PARAM*  m_pFrameParam;

    u32 m_activeSelectIdx;

    applib::frame::Result m_initResult;
    applib::frame::Result m_endResult;
};

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )

#endif // BATTLE_VIDEO_PLAYER_DATA_LIST_FRAME_H_INCLUDED
