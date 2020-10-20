//======================================================================
/**
 * @file LunaSolDiveParam.h
 * @date 2016/11/28 14:50:43
 * @author saito_nozomu
 * @brief ルナソルダイブ格納パラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined( __APP_EVENT_LUNASOL_DIVE_PARAM_H_INCLUDED__ )
#define __APP_EVENT_LUNASOL_DIVE_PARAM_H_INCLUDED__
#pragma once

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

struct LUNASOL_DIVE_PARAM{
  bool in_first_mode; //イベント用初回モードか？　trueでイベント初回
  u32 out_distance;          //進んだ距離
  u32 out_distortion_type;   //ひずみタイプ
  u32 out_rarelity;          //ひずみのレアリティ
  u32 out_zone_id;           //ゾーンID
  u32 out_monsno;      //レアリティの高いひずみに入ったときの出現する、伝説、準伝のモンスターナンバー
  gfl2::math::Vector3 out_pos; //飛び先座標
};

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // __APP_EVENT_LUNASOL_DIVE_PARAM_H_INCLUDED__