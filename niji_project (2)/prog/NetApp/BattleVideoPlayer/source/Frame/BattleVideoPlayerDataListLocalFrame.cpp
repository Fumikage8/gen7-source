//=============================================================================
/**
 * @file	 BattleVideoPlayerDataListLocalFrame.h
 * @brief	 データ選択フレーム(ローカルデータ用)
 * @author yuto_uchida
 * @date	 2015.09.25
 */
//=============================================================================
#pragma once

#include "NetApp/BattleVideoPlayer/source/frame/BattleVideoPlayerDataListLocalFrame.h"

//  UIViewのインクルード
#include "NetApp/BattleVideoPlayer/source/BattleVideoPlayerUIViewPool.h"

//  gfl2のインクルード

//  nijiのインクルード

//  リソースのインクルード

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   */
  //------------------------------------------------------------------
  DataListLocalFrame::DataListLocalFrame(void)
  : DataListBaseFrame()
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  DataListLocalFrame::~DataListLocalFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン更新を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  applib::frame::Result DataListLocalFrame::UpdateFunc()
  {
    applib::frame::Result result  = DataListBaseFrame::UpdateFunc();

    u32 seq = DataListBaseFrame::GetSubSeq();
    if( seq == SEQ_SELECT_ENTRY )
    {
      //  選択した
    }

    return  result;
  }

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )
