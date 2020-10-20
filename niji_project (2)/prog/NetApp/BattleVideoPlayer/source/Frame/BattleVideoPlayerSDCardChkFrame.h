//=============================================================================
/**
 * @file	 BattleVideoPlayerSDCardChkFrame.h
 * @brief	 SDカードチェックフレーム
 * @author yuto_uchida
 * @date	 2015.09.24
 */
//=============================================================================
#ifndef BATTLE_VIDEO_PLAYER_SD_CARD_CHK_FRAME_H_INCLUDED
#define BATTLE_VIDEO_PLAYER_SD_CARD_CHK_FRAME_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( NetAppLib )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  class VideoDataManager;

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetAppLib )

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  class UIViewPool;

  /** データチェックフレーム */
  class SDCardChkFrame : public applib::frame::CellSuper
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(SDCardChkFrame);

  public:
    enum Result
    {
      RESULT_NONE = 0,
      RESULT_SUCCESS,
      RESULT_ERROR,
      RESULT_FATAL
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
    SDCardChkFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~SDCardChkFrame();

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
      m_sysMessageEndNextSeq  = 0;
    }

    FRAME_PARAM*  m_pFrameParam;
    u32           m_sysMessageEndNextSeq;
};

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )

#endif // BATTLE_VIDEO_PLAYER_SD_CARD_CHK_FRAME_H_INCLUDED
