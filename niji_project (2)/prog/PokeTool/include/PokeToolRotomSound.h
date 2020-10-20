//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		PokeToolRoptomSound.cpp
 *	@brief  ロトム図鑑用SE
 *	@author	ariizumi nobuhiko
 *	@date		2017.04.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __POKE_TOOL_ROTOM_SOUND_H__)
#define __POKE_TOOL_ROTOM_SOUND_H__

#include <pml/include/pmlib.h>

GFL_NAMESPACE_BEGIN(PokeTool)

// ロトム図鑑連動用のSEを鳴らす関数です。
// monsNo モンスター番号
// formNo フォルム番号
// sex    性別
// seIdx  SEの番号(0～3)
extern void PlayRotomSound(MonsNo monsNo, FormNo formNo, int sex,int seIdx);

GFL_NAMESPACE_END(PokeTool)
#endif		// __POKE_TOOL_ROTOM_SOUND_H__
