//======================================================================
/**
 * @file    FieldSubscreenLuncher.h
 * @brief   15/06/09
 * @author  k.ohno
 * @data    フィールドサブスクリーンのランチャー
 */
//======================================================================

#ifndef __FIELD_SUBSCREEN_LAUNCHER_H__
#define __FIELD_SUBSCREEN_LAUNCHER_H__
#pragma once


#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

#include "App\FieldMenu\include\FieldMenuFrame.h"

#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(SubScreen)

class FieldSubScreenProcManager;
class FieldSubScreen;

/* -------------------------------------------------------------------------*/
/**
 * @brief 下画面関連のイベント起動
 *
 * @param p_menu      フィールドメニュー
 * @param p_gman      ゲームマネージャ
 * @param pStopInterestBG trueの場合、看板注目を切る
 * @param pFocusStopBit   フォーカス停止命令
 *
 *  @retval true:メニュー用。プレイヤーを停止させ、下画面のSuspend呼ばすイベント起動
 *  @retval false: 通常イベント起動。プレイヤーは動いたまま、下画面のSuspend呼んでイベント起動
 *  @fix GFBTS[2575]:アプリを呼び出す際に、動作モデルのロックが出来ていなかった問題を修正
 */
/* -------------------------------------------------------------------------*/
int CallEvent( FieldSubScreenProcManager* pFieldSubScreenProcManager,FieldSubScreen* pSub , GameSys::GameManager* p_gman, b32* pStopInterestBG, u32* pFocusStopBit );

//ＸＭＥＮＵオープン専用
int CallEventForceMenuOpen( FieldSubScreenProcManager* pFieldSubScreenProcManager,FieldSubScreen* pSub, GameSys::GameManager* p_gman, b32* pStopInterestBG );

/* -------------------------------------------------------------------------*/
/**
 * @brief 下画面関連のイベント起動してよいかどうか
 * @return  OKの場合TRUE
 */
/* -------------------------------------------------------------------------*/
bool CheckEvent( FieldSubScreenProcManager* pFieldSubScreenProcManager );


enum{
  FIELDSUBSCREEN_NONE,
  FIELDSUBSCREEN_NORMAL,
  FIELDSUBSCREEN_MENU,

};






GFL_NAMESPACE_END(SubScreen)
GFL_NAMESPACE_END(Field)


#endif// __FIELD_SUBSCREEN_LAUNCHER_H__
