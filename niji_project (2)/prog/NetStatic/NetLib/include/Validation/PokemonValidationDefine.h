//======================================================================
/**
 * @file  PokemonValidationDefine.h
 *
 * @author  suzuki_hisanao
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef _NETAPP_POKEMON_VALIDATION_DEFINE_H_
#define _NETAPP_POKEMON_VALIDATION_DEFINE_H_
#pragma once

#include <nex.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Validation)


/**
 * @brief 不正チェックサーバからの返却コード
 */
typedef enum
{
  RESULT_VALID     = 0,  //!< 正常なデータとして証明
  RESULT_INVALID   = 1,  //!< 不正なデータとして証明
  RESULT_RSA_ERROR = 2,  //!< 証明書のバージョン違い
}ValidationResultCode;


GFL_NAMESPACE_END(Validation)
GFL_NAMESPACE_END(NetLib)


#endif  // _POKEMON_VALIDATION_DEFINE_H_
#endif