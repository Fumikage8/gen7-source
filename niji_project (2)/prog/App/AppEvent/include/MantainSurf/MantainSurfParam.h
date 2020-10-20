//======================================================================
/**
* @file MantainSurfParam.h
* @date 2016/11/22 19:22:10
* @author saito_nozomu
* @brief マンタインサーフ格納パラメータ
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================
#if !defined( __APP_EVENT_MANTAIN_SURF_PARAM_H_INCLUDED__ )
#define __APP_EVENT_MANTAIN_SURF_PARAM_H_INCLUDED__
#pragma once

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

typedef struct{
  bool in_tutorial;
  u32 in_corse_id;
  u32 in_score;
  u32 out_score;
} MANTAIN_SURF_PARAM;

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // __APP_EVENT_MANTAIN_SURF_PARAM_H_INCLUDED__