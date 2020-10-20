#if !defined( __HAND_FIELD_H__ )
#define __HAND_FIELD_H__
//=============================================================================================
/**
 * @file  hand_field.h
 * @brief ポケモンXY バトルシステム イベントファクター [フィールドエフェクト]
 * @author  taya
 *
 * @date  2010.12.21  作成
 */
//=============================================================================================
#pragma once

#include "../btl_common.h"
#include "../btl_field.h"
#include "../btl_EventFactor.h"

GFL_NAMESPACE_BEGIN(btl)
namespace handler {
namespace field {

extern BTL_EVENT_FACTOR*  Add( FieldStatus::EffectType effect, u8 sub_param );
extern void               Remove( BTL_EVENT_FACTOR* factor );


} // end of namespace 'field'
} // end of namespace 'handler'


GFL_NAMESPACE_END(btl)

#endif // __HAND_FIELD_H__
