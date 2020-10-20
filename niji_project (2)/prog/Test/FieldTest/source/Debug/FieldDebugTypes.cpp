/**
* @brief  フィールドデバッグ定義
* @file   FieldDebugTypes.cpp
* @author ikuta_junya
* @data   2015.03.08
*/

// @note フィールド上のデバック定義は可能な限りここに集める

#if PM_DEBUG

#include "test/FieldTest/include/debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( debug );


// 表示フラグの実体
s32 FieldDebugTypes::s_DrawSwitchTable[ FieldDebugTypes::DRAW_SWITCH_MAX ] = { 

#define DEFINE(x, y, z) y,
#include "test/FieldTest/include/debug/FieldDebugDrawSwitch.h"
#undef DEFINE
};

// 表示フラグ定義名の文字列
static const c16* const sc_DebugNutsSetNameTable[ FieldDebugTypes::DRAW_SWITCH_MAX ] = {
#define DEFINE(x, y, z) L ## #x,
#include "test/FieldTest/include/debug/FieldDebugDrawSwitch.h"
#undef DEFINE
};

// 表示フラグの値最大値
static s32 sc_DrawSwitchCountMaxTable[ FieldDebugTypes::DRAW_SWITCH_MAX ] = { 
#define DEFINE(x, y, z) z,
#include "test/FieldTest/include/debug/FieldDebugDrawSwitch.h"
#undef DEFINE
};

// 表示フラグの値の初期値
static s32 sc_DrawSwitchInitialValueTable[ FieldDebugTypes::DRAW_SWITCH_MAX ] = { 
#define DEFINE(x, y, z) y,
#include "test/FieldTest/include/debug/FieldDebugDrawSwitch.h"
#undef DEFINE
};

// DebugTick
#if FIELD_DEBUG_TICK_ENABLE
gfl2::debug::DebugTickDump FieldDebugTypes::s_TickWork( FieldDebugTypes::TickDefine::MAX );
#endif // FIELD_DEBUG_TICK_ENABLE

// DebugTickログ表示名の文字列
static const c8* const sc_DebugTickFieldNameTable[ FieldDebugTypes::TickDefine::MAX ] = {
#define DEFINE(x,y) y,
#include "test/FieldTest/include/debug/FieldDebugTickDefine.h"
#undef DEFINE
};

//
// デバッグstatic変数の実体
//

FieldDebugTypes::DRAW_INFO FieldDebugTypes::s_DrawInfo =
{
  0,  // drawbleNodeNum
  0,  // visibleNodeNum
  0,  // visibleTagNum
  0,  // transformNodeWorldUpdateNum
  0   // transformNodeWorldSkipNum
};


/**
* @brief 表示フラグの定義値から定義名を取得する
*
* @param  drawSwitch    表示フラグの定義値
*
* @return 定義名(不正な場合はNULL)
*/ 
const c16* FieldDebugTypes::GetDrawSwitchLabel( DrawSwitch drawSwitch )
{
  if( drawSwitch >= DRAW_SWITCH_MAX )
  {
    return 0;
  }

  return sc_DebugNutsSetNameTable[ drawSwitch ];
}

/**
* @brief 表示フラグの定義値から値の最大値を取得する
*
* @param  drawSwitch    表示フラグの定義値
*
* @return 最大値(不正な場合は0)
*/ 
s32 FieldDebugTypes::GetDrawSwitchCountMax( DrawSwitch drawSwitch )
{
  if( drawSwitch >= DRAW_SWITCH_MAX )
  {
    return 0;
  }

  return sc_DrawSwitchCountMaxTable[ drawSwitch ];
}

/**
* @brief 表示フラグの定義値から値の初期値を取得する
*
* @param  drawSwitch    表示フラグの定義値
*
* @return 初期値(不正な場合は0)
*/ 
s32 FieldDebugTypes::GetDrawSwitchInitialValue( DrawSwitch drawSwitch )
{
  if( drawSwitch >= DRAW_SWITCH_MAX )
  {
    return 0;
  }

  return sc_DrawSwitchInitialValueTable[ drawSwitch ];
}

/**
* @brief DebugTick定義値から定義名を取得する
*
* @param index DebugTickの定義値
*
* @return 定義名(不正な場合はNULL)
*/ 
const c8* FieldDebugTypes::GetTickLabel( FieldDebugTypes::TickDefine::Index index )
{
  if( index >= FieldDebugTypes::TickDefine::MAX )
  {
    return 0;
  }

  return sc_DebugTickFieldNameTable[ index ];
}

GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( Field );

#endif // #if PM_DEBUG