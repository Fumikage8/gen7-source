//======================================================================
/**
 * @file RealRegionPokemon.h
 * @date    2012.08.26
 * @author  tamada
 * @brief リアル地域ポケモンのフォルムを決定する
 * @par Copyright
 * @note 2015/12/22 19:31:26 移植 onoue masayuki
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __REALREGIONPOKEMON_H_INCLUDED__
#define __REALREGIONPOKEMON_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(PokeTool)
GFL_NAMESPACE_BEGIN(RealRegion)

//-----------------------------------------------------------------------------
/**
 * @brief リアル地域ポケモンのフォルム分布を取得する
 * @param latitude    緯度  0x0000～0x4000（0～90度）,0xc000～0xffff(-90～-0.05度）
 * @param longitude   経度  0x0000～0xffff（0～360度）
 *
 * @return  u8  フォルム指定  （0オリジン）
 */
//-----------------------------------------------------------------------------

extern u8 DecideRegion( u16 latitude, u16 longitude );

GFL_NAMESPACE_END(RealRegion)
GFL_NAMESPACE_END(PokeTool)

#endif // __REALREGIONPOKEMON_H_INCLUDED__