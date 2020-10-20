//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldSubScreenProc.h
 *	@brief  フィールドの下画面 イベント無しで動いている物
 *	@author	tomoya takahashi   => k.ohno
 *	@date		2011.06.02         => 2015.5.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELDSUBSCREENPROC_H__
#define __FIELDSUBSCREENPROC_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <proc/include/gfl2_ProcManager.h>
#include "AppLib/include/Util/app_util_heap.h"
#include "FieldSubScreenProcManager.h"

// リクエストID定義
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( SubScreen );

//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *    サブウィンドウプロセス
 */
//-----------------------------------------------------------------------------
class FieldSubScreenProc : public gfl2::proc::BaseProcess
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldSubScreenProc); //コピーコンストラクタ＋代入禁止

public:

  //! コンストラクタ
  FieldSubScreenProc( void ) : gfl2::proc::BaseProcess(), m_EndReq(false){}

  //! デストラクタ
  virtual ~FieldSubScreenProc(void){}

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager) = 0;
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager) = 0;
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void   DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo displayNo){GFL_UNUSED(pManager);};
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager) = 0;

  //----------------------------------------------------------------------------
  /**
   *	@brief  フィールド復帰　再稼働
   */
  //-----------------------------------------------------------------------------
  virtual void Resume( void ) = 0;

  //----------------------------------------------------------------------------
  /**
   *	@brief  フィールド削除　一時停止
   */
  //-----------------------------------------------------------------------------
  virtual void Suspend( void ) = 0;

  //----------------------------------------------------------------------------
  /**
   *	@brief  下画面の種類を返す
   */
  //-----------------------------------------------------------------------------
  virtual SubWindowId GetSubWindowId() = 0;

  /**
   * @brief	  起動リクエストを返す
   *          フィールドでポーリングされます。
   */
  virtual FieldMenuOutRequestID GetEventRequestID() = 0;

  //------------------------------------------------------------------
  /**
   * @brief	  終了リクエストのチェック
   */
  //------------------------------------------------------------------
  bool IsReqEnd( void ) const { return m_EndReq; }

  //------------------------------------------------------------------
  /**
   * @brief	  終了リクエストの発行
   */
  //------------------------------------------------------------------
  inline void ReqEnd( void ){ m_EndReq = true; }

private:

  bool  m_EndReq;   ///< 終了リクエストフラグ


};



GFL_NAMESPACE_END( SubScreen );
GFL_NAMESPACE_END( Field );

#endif	// __FIELDSUBSCREENPROC_H__

