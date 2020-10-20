//=============================================================================
/**
 * @file	 BattleVideoPlayerDataListLocalFrame.h
 * @brief	 データ選択フレーム(ローカルデータ用)
 * @author yuto_uchida
 * @date	 2015.09.25
 */
//=============================================================================
#ifndef BATTLE_VIDEO_PLAYER_DATA_LIST_LOCAL_FRAME_H_INCLUDED
#define BATTLE_VIDEO_PLAYER_DATA_LIST_LOCAL_FRAME_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"

//  派生の基本クラスフレーム
#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerDataListBaseFrame.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  class UIViewPool;

  /** データ選択フレーム */
  class DataListLocalFrame : public DataListBaseFrame
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(DataListLocalFrame);

  public:
    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    DataListLocalFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~DataListLocalFrame();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン更新を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result UpdateFunc();
};

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )

#endif // BATTLE_VIDEO_PLAYER_DATA_LIST_LOCAL_FRAME_H_INCLUDED
