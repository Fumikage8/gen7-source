/**
* @brief  ポケモンふれあい用定義
* @file   FieldPokemonFureaiTypes.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDPOKEMONFUREAIDEFINE_H_INCLUDED__ )
#define __FIELDPOKEMONFUREAIDEFINE_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Math.h>
#if 0
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
#endif


#include "niji_reference_files/script/FieldPokemonFureaiScriptTypes.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( PokemonFureai );

struct POKEMON_SETTING_DATA
{
  s32 model_id;
  s32 base_stance;
  f32 nadenade_distance;
  f32 nadenade_angle;
  f32 nadenade_x;
  s32 move_mode;
  s32 rot_type;
  f32 speed;
  s32 yorokobi1_type;
  s32 yorokobi2_type;
  s32 okori_type;
  s32 nemuri_type;
  f32 icon_scale;
  s32 seikaku;
  s32 fureaiPointBorder;
  s32 kaiten_asobi_type;
};

GFL_NAMESPACE_END( PokemonFureai );
GFL_NAMESPACE_END( Field );

#endif // __FIELDPOKEMONFUREAIDEFINE_H_INCLUDED__
