// ============================================================================
/*
 * @file MiracleTradeDefine.h
 * @brief ミラクル交換アプリで使用する定数等の定義ファイルです。
 * @date 2015.07.06
 */
// ============================================================================
#if !defined( MIRACLETRADEDEFINE_H_INCLUDE )
#define MIRACLETRADEDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)

  static const u32  MIRACLETRADE_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //   1 MB
  static const u32  MIRACLETRADE_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 20;   //  20 MB

  static const u32  MIRACLETRADE_APP_WORK_HEAP_SIZE                = 1024 * 512;         // 500 KB

  static const u32  MIRACLETRADE_TRAINER_ICON_NUM                  = 1;

  static const u32  MIRACLETRADE_DEFAULT_NOTIFICATION_WAIT_FRAME   = 60;

  static const u32  MIRACLETRADE_TIME_OUT                          = 90;

#if defined( GF_PLATFORM_CTR )
  static const u32 SIGNATRUE_SIZE                                  = NetLib::Validation::PokemonValidation::SIGNATURE_SIZE;
#else
  static const u32 SIGNATRUE_SIZE                                  = 256;
#endif // defined( GF_PLATFORM_CTR )

  static const int POKE_IN_FRAME_TOP      =  75;
  static const int POKE_IN_FRAME_BOTTOM   = 175;
  static const int POKE_IN_FRAME_LEFT     = 150;
  static const int POKE_IN_FRAME_RIGHT    = 250;


GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEDEFINE_H_INCLUDE
