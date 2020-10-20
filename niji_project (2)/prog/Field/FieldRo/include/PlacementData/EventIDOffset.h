//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventIDOffset.h
 *  @brief  特殊なイベントID用のオフセット
 *  @author Masanori Kanamaru
 *  @date   2015.10.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_EVENTID_OFFSET_H__ )
#define __FIELD_EVENTID_OFFSET_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

#include "niji_reference_files/script/FieldPawnTypes.h"

GFL_NAMESPACE_BEGIN(Field);

enum EVENID_OFFSET
{
  BERRY_POINT_START = 1900, // きのみスポット(最大10)
  TRAFFIC_ACTOR_START = 2000, // 往来NPC(最大10)
  PR_ROCK_ACTOR_START = 3000, // 怪力岩の岩
  EVENT_MODEL_START = FieldScript::SCRIPT_SP_CHR_EVENT_ID_START, // イベント用に動的に生成したモデル。
  DROPITEM_ACTOR_START = FieldScript::SCRIPT_DROPITEM_EVENT_ID_START,  // DropItem
};

GFL_NAMESPACE_END(Field);


#endif // __FIELD_EVENTID_OFFSET_H__