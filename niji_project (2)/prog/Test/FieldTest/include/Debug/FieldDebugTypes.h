/**
* @brief  フィールドデバッグ定義
* @file   FieldDebugTypes.h
* @author ikuta_junya
* @data   2015.03.08
*/

// @note フィールド上のデバック定義は可能な限りここに集める

#if !defined( __FIELDDEBUGTYPES_H_INCLUDED__ )
#define __FIELDDEBUGTYPES_H_INCLUDED__
#pragma once

#if PM_DEBUG
#if GFL_DEBUG
#define FIELD_DEBUG_TICK_ENABLE 1
#else // GFL_DEBUG
#define FIELD_DEBUG_TICK_ENABLE 0
#endif // GFL_DEBUG
#else // PM_DEBUG
#define FIELD_DEBUG_TICK_ENABLE 0
#endif // PM_DEBUG

#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugTick.h>

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( debug );

/**
* @brief フィールドデバッグ定義クラス、現状ではstaticメンバーのみを持つ想定でいます
*/
class FieldDebugTypes
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldDebugTypes );

public:

  // 描画関連情報
  struct DRAW_INFO
  {
    u32 drawbleNodeNum;
    u32 visibleNodeNum;
    u32 visibleTagNum;
    u32 transformNodeTraverseCount;
    u32 transformNodeWorldMatrixUpdateCount;
    u32 transformNodeWorldMatrixUpdateSkipCount;
  };

  //
  // 表示制御フラグ 
  //

  // 表示制御フラグの定義
  enum DrawSwitch
  {
#define DEFINE(x, y, z) x,
#include "FieldDebugDrawSwitch.h"
#undef DEFINE

    DRAW_SWITCH_MAX
  };

  static s32 s_DrawSwitchTable[ DRAW_SWITCH_MAX ];    // 表示制御フラグ
  
  /**
  * @brief 表示フラグの定義値から定義名を取得する
  *
  * @param  drawSwitch    表示フラグの定義値
  *
  * @return 定義名(不正な場合はNULL)
  */ 
  static const c16* GetDrawSwitchLabel( DrawSwitch drawSwitch );

  /**
  * @brief 表示フラグの定義値から値の最大値を取得する
  *
  * @param  drawSwitch    表示フラグの定義値
  *
  * @return 最大値(不正な場合は0)
  */ 
  static s32 GetDrawSwitchCountMax( DrawSwitch drawSwitch );

  /**
  * @brief 表示フラグの定義値から値の初期値を取得する
  *
  * @param  drawSwitch    表示フラグの定義値
  *
  * @return 初期値(不正な場合は0)
  */ 
  static s32 GetDrawSwitchInitialValue( DrawSwitch drawSwitch );


  // Field用DebugTick定義
  struct TickDefine
  {
    enum Index
    {
#define DEFINE(x, y) x,
#include "FieldDebugTickDefine.h"
#undef DEFINE

      MAX,
    };
  };

#if FIELD_DEBUG_TICK_ENABLE
  static gfl2::debug::DebugTickDump s_TickWork;   // Tick計測ワーク
#endif // FIELD_DEBUG_TICK_ENABLE

  /**
  * @brief DebugTick定義値から定義名を取得する
  *
  * @param index DebugTickの定義値
  *
  * @return 定義名(不正な場合はNULL)
  */ 
  static const c8* GetTickLabel( TickDefine::Index index );

  // 変数
  static DRAW_INFO     s_DrawInfo;

private:



};  // class FieldDebugTypes

GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( Field );

#endif // #if PM_DEBUG



//----------------------------------------------------------------------------
// フィールドDebugTick計測定義
//
// ・使い方
// １、FieldDebugTickDefine.hに定義を追加する
// ２、↓のようにTick計測を有効にする (登録してある全ての定義の計測が有効になる)
//     FIELD_DEBUG_TICK_SET_FLAG( true)
// ３、↓のように計測処理を組み込む
//     FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::定義名);
//     計測したい処理
//     FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::定義名);
//----------------------------------------------------------------------------
#if FIELD_DEBUG_TICK_ENABLE

//----------------------------------------------------------------------------
/**
 *  @brief  ダンプ機能のON・OFF
 *  @param  flag    ダンプ機能フラグ　true:ON   false:OFF
 */
//-----------------------------------------------------------------------------
#define FIELD_DEBUG_TICK_SET_FLAG(flag) Field::debug::FieldDebugTypes::s_TickWork.SetFlag( (flag) )

//----------------------------------------------------------------------------
/**
 *  @brief  ダンプ情報のリセット
 */
//-----------------------------------------------------------------------------
#define FIELD_DEBUG_TICK_RESET() Field::debug::FieldDebugTypes::s_TickWork.ResetTickDump()

//----------------------------------------------------------------------------
/**
 *  @brief  チック計測開始
 *  @param  index   格納先インデックス
 */
//-----------------------------------------------------------------------------
#define FIELD_DEBUG_TICK_START(index) Field::debug::FieldDebugTypes::s_TickWork.StartTickDump( (index), Field::debug::FieldDebugTypes::GetTickLabel(index) )

//----------------------------------------------------------------------------
/**
 *  @brief  チック計測終了
 *  @param  index   格納先インデックス
 */
//-----------------------------------------------------------------------------
#define FIELD_DEBUG_TICK_END(index) Field::debug::FieldDebugTypes::s_TickWork.EndTickDump( (index) )

//----------------------------------------------------------------------------
/**
 *  @brief  ダンプ
 *  @param  index   格納先インデックス
 */
//-----------------------------------------------------------------------------
#define FIELD_DEBUG_TICK_DUMP_ALL() Field::debug::FieldDebugTypes::s_TickWork.PrintAll()
#define FIELD_DEBUG_TICK_DUMP(index) Field::debug::FieldDebugTypes::s_TickWork.Print( (index) )

//----------------------------------------------------------------------------
/**
 *  @brief  μ秒を取得
 *  @param  index インデックス
 *  @retval μ秒 
 */
//-----------------------------------------------------------------------------
#define FIELD_DEBUG_TICK_US_GET(index) Field::debug::FieldDebugTypes::s_TickWork.GetTickNowUs((index))

//----------------------------------------------------------------------------
/**
 *  @brief  平均μ秒を取得
 *  @param  index インデックス
 *  @retval 平均μ秒
 */
//-----------------------------------------------------------------------------
#define FIELD_DEBUG_TICK_US_AVE_GET(index) Field::debug::FieldDebugTypes::s_TickWork.GetTickAveUs((index))

//----------------------------------------------------------------------------
/**
 *  @brief  最大μ秒を取得
 *  @param  index インデックス
 *  @retval 最大μ秒
 */
//-----------------------------------------------------------------------------
#define FIELD_DEBUG_TICK_US_MAX_GET(index) Field::debug::FieldDebugTypes::s_TickWork.GetTickMaxUs((index))

#else // FIELD_DEBUG_TICK_ENABLE

#define FIELD_DEBUG_TICK_SET_FLAG(flag) /**/
#define FIELD_DEBUG_TICK_RESET() /**/
#define FIELD_DEBUG_TICK_START(index) /**/
#define FIELD_DEBUG_TICK_END(index) /**/
#define FIELD_DEBUG_TICK_DUMP_ALL() /**/
#define FIELD_DEBUG_TICK_DUMP(index) /**/
#define FIELD_DEBUG_TICK_US_GET(index) /**/
#define FIELD_DEBUG_TICK_US_AVE_GET(index) /**/
#define FIELD_DEBUG_TICK_US_MAX_GET(index) /**/

#endif // FIELD_DEBUG_TICK_ENABLE




#endif // !defined( __FIELDDEBUGTYPES_H_INCLUDED__ )