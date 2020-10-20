﻿//==============================================================================
/**
 * @file	  BoxLowFolderDrawListener.h
 * @brief	  描画からファンクションへリスナー
 * @author	k.ohno
 * @date	  2015.08.15
 */
// =============================================================================

#if !defined( __BOX_LOW_FOLDER_DRAW_LISTENER_H__ )
#define	__BOX_LOW_FOLDER_DRAW_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxLowFolderDrawListener {


public:

  //--------------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnButtonAction( int button_no ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   キー操作通知
   * @param		dir  キー方向
   * @retval  true = 操作実行
   * @retval  false = 操作失敗
   */
  //--------------------------------------------------------------------------
  virtual bool OnKeyAction( u32 dir ){ return false; }



  //--------------------------------------------------------------------------
  /**
   * @brief   タッチ開始
   */
  //--------------------------------------------------------------------------
  virtual void TouchFolder( int index ){};
  //--------------------------------------------------------------------------
  /**
   * @brief   つかみ開始
   */
  //--------------------------------------------------------------------------
  virtual void OnFolderHand( int x, int y, int index ){};
  //--------------------------------------------------------------------------
  /**
   * @brief   フォルダー移動
   */
  //--------------------------------------------------------------------------
  virtual void MoveFolder( int index,int index2 ){};
  //--------------------------------------------------------------------------
  /**
   * @brief   フォルダーにポケモンを入れる
   */
  //--------------------------------------------------------------------------
  virtual void DropFolder( int index ){};
  //--------------------------------------------------------------------------
  /**
   * @brief   カーソル移動もしくはフォルダーに入る
   */
  //--------------------------------------------------------------------------
  virtual void ChangeFolder( int index,int index2 ){};
  //--------------------------------------------------------------------------
  /**
   * @brief   フォルダーにポケモンを入れる
   */
  //--------------------------------------------------------------------------
  virtual bool DragDrop( int index ){ return false; };

  //--------------------------------------------------------------------------
  /**
   * @brief   タッチの最初の検査
   * @param   touchしたかどうか
   * @return  これ以上処理しない場合true 通常false
   */
  //--------------------------------------------------------------------------
  virtual bool OnTouchFirstHook(bool ){ return false; }

  //--------------------------------------------------------------------------
  /**
   * @brief   インプットイネーブルの操作
   * @return  操作用リスナーならtrue 通常true
   */
  //--------------------------------------------------------------------------
  virtual bool GetInputEnableFlg(void){ return true; }

  
	enum{
    LOWFLD_ACTION_CANCEL,
    LOWFLD_ACTION_DECIDE,
    LOWFLD_ACTION_UP,
    LOWFLD_ACTION_DOWN,
    LOWFLD_ACTION_LEFT,
    LOWFLD_ACTION_RIGHT,
    LOWFLD_ACTION_MODE_CHANGE,
    LOWFLD_ACTION_FIND,
	};


};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif	/* __BOX_LOW_FOLDER_DRAW_LISTENER_H__ */
