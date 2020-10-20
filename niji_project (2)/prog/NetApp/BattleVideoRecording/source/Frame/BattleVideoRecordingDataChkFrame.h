//=============================================================================
/**
 * @file	 BattleVideoRecordingDataChkFrame.h
 * @brief	 データチェックフレーム
 * @author yuto_uchida
 * @date	 2015.09.16
 */
//=============================================================================
#ifndef BATTLE_VIDEO_RECORDING_DATA_CHK_FRAME_H_INCLUDED
#define BATTLE_VIDEO_RECORDING_DATA_CHK_FRAME_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "Battle/include/battle_def.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( ExtSavedata )

  class BattleRecorderSaveData;

GFL_NAMESPACE_END( ExtSavedata )

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoRecording )

  class UIViewPool;

  /** データチェックフレーム */
  class DataChkFrame : public applib::frame::CellSuper
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(DataChkFrame);

  public:
    enum Result
    {
      RESULT_APP_END  = 0,
      RESULT_GO_SAVE,
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
          serverVersion   = BTL_NET_SERVER_VERSION;
          bRecNG          = false;
        }

        //  UIViewプール
        UIViewPool* pUIViewPool;

        ExtSavedata::BattleRecorderSaveData*  pExtSaveData;

        app::util::Heap*                      pAppHeap;
        u32   serverVersion;
        bool  bRecNG;

      } in;

      struct OUT_DATA
      {
        OUT_DATA()
        {
          Reset();
        }

        void Reset()
        {
          result  = RESULT_APP_END;
        }

        Result result;
      } out;
    };

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    DataChkFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~DataChkFrame();

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

  private:
    //  変数初期化
    void _Clear()
    {
      m_pFrameParam = NULL;
    }

    FRAME_PARAM*  m_pFrameParam;
};

GFL_NAMESPACE_END( BattleVideoRecording )
GFL_NAMESPACE_END( NetApp )

#endif // BATTLE_VIDEO_RECORDING_DATA_CHK_FRAME_H_INCLUDED
