//======================================================================
/**
 * @file    FieldApplicationLauncher.h
 * @brief   15/06/09
 * @author  k.ohno
 * @data    フィールドサブスクリーンのランチャー
 */
//======================================================================

#ifndef __FIELD_APPLICATION_LUNCHER_H__
#define __FIELD_APPLICATION_LUNCHER_H__
#pragma once


#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

#include "App\FieldMenu\include\FieldMenuFrame.h"
#include "Field/FieldStatic/include/Item/FieldItemUse.h"
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"
#include "Battle/Background/include/BgSystem.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Application)

//! フィールドから生成されるデータ
struct FieldmapData
{
  // バッグ
  Field::FieldItemUse::ITEM_USE_CHECK_WORK item_use_work;   //!< 道具使用ワーク
  Field::weather::WeatherKind weather;                      //!< 天候（進化用）
  // かわいがり
  btl::BgSystem::SETUP_PARAM battle_bg_param;               //!< バトル背景

};



/* -------------------------------------------------------------------------*/
/**
 * @brief 下画面関連のイベント起動
 *
 * @param p_menu      フィールドメニュー
 * @param p_gman      ゲームマネージャ
 * @param pStopInterestBG trueの場合、看板注目を切る
 *
 *  @retval 起動したイベント
 *  @retval NULL  特に何もおきない。
 *  @fix GFBTS[2575]:アプリを呼び出す際に、動作モデルのロックが出来ていなかった問題を修正
 */
/* -------------------------------------------------------------------------*/
GameSys::GameEvent* CallEvent(  Field::SubScreen::FieldMenuOutRequestID no, GameSys::GameManager* p_gman, b32* pStopInterestBG,App::FieldMenu::FieldMenuFrame *pFrame );


GameSys::GameEvent* CallAppEvent( Field::SubScreen::FieldMenuOutRequestID no, GameSys::GameManager* p_gman, FieldmapData* fieldmap_data );


//bool CheckEvent( App::FieldMenu::FieldMenuFrame* pFrame );


//-----------------------------------------------------------------------------
/**
 * @brief   フィールドからのデータを生成
 *
 * @param   gm    ゲームマネージャ
 * @param   data  生成場所
 */
//-----------------------------------------------------------------------------
void SetupFieldmapData( GameSys::GameManager * gm, FieldmapData * data );


GFL_NAMESPACE_END(Application)
GFL_NAMESPACE_END(Field)


#endif// __FIELD_APPLICATION_LUNCHER_H__
