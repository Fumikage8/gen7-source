// ============================================================================
/*
 * @file P2PTradeTypes.h
 * @brief P2P交換アプリで使用する型の定義ファイルです。
 * @date 2015.11.11
 * @author Akira endo
 */
// ============================================================================
#if !defined( P2PTRADETYPES_H_INCLUDE )
#define P2PTRADETYPES_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "pml/include/pokepara/pml_PokemonParam.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


typedef struct
{
  u32       messageID;
  u32       monsno;
  u8        pokemonParamBuffer[ pml::pokepara::PokemonParam::DATASIZE ];
} MessageInfo;


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

#endif // P2PTRADETYPES_H_INCLUDE
