//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldProc.h
 *	@brief  フィールドプロセス
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
///#if !defined( __VIEWER_PROC_H__ )
///#define __VIEWER_PROC_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_List.h>

// niji
#include "GameSystem/include/GameProc.h"
#include "System/include/GflvUse.h"


#include "ViewerProc/include/FieldRo/Fieldmap.h"

GFL_NAMESPACE_BEGIN( Field );

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class ViewerProc : public GameSystem::GameProc
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ViewerProc );



public:
  /**------------------------------------------------------------------
   *      公開定義
   ------------------------------------------------------------------*/


  /**------------------------------------------------------------------
   *      公開関数
   ------------------------------------------------------------------*/

public:
  /**------------------------------------------------------------------
   *      GameProc仮想関数
   ------------------------------------------------------------------*/
  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  ViewerProc( void/*FieldmapDesc* desc*/ );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ViewerProc( void );

  /**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	  PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  /**
  * @brief   PROCのメインを行う関数
  *  1/30フレームの②フレーム目の処理です。
  *  1/60フレーム動作時には、使われません。
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
#if 0
  //------------------------------------------------------------------
  /**
   * @brief   PROCのイベント起動検知処理
   *
   *  以下のタイミングで行うべき、イベント起動処理
   *  　GameProc->EventCheckFunc
   *  　EventManager->Main
   *  　GameProc->Main
   */
  //------------------------------------------------------------------
  virtual void EventCheckFunc(GameSystem::GameManager* pManager);
#endif

  /**
  * @brief   FrameModeの取得
  * @retval   FrameMode
  *
  * フレームモードを切り替えるときは、戻り値を変えてください。
  */
  ///virtual GameSystem::FrameMode GetFrameMode( void ) const { return GameSystem::FRAMEMODE_30; }



private:
  Fieldmap* m_pFieldmap;
  ///FieldmapDesc m_desc;
};

GFL_NAMESPACE_END( Field );

///#endif	// __VIEWER_PROC_H__

