/**
* @brief  ポケモンふれあい用関数
* @file   FieldPokemonFureaiTool.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDPOKEMONFUREAITOOL_H_INCLUDED__ )
#define __FIELDPOKEMONFUREAITOOL_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Math.h>

// niji
#include "Savedata/include/EventWork.h"

#if 0
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
#endif


#include "niji_reference_files/script/FieldPokemonFureaiScriptTypes.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldContactPokemonAccessor.h"
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"

#include <niji_conv_header/field/FieldPokemonFureaiSleepType.h>


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( PokemonFureai );

s32 GetFureaiNo(Field::MoveModel::FieldMoveModel* pFieldMoveModel);
s32 GetPoint( const FieldContactPokemonAccessor::ContactPokemonData* pData );
s32 GetRank( Field::MoveModel::FieldMoveModel* pFieldMoveModel );
s32 AddPoint( const FieldContactPokemonAccessor::ContactPokemonData* pData, s32 value = 1 );
s32 AddPointByFieldIcon(Field::MoveModel::FieldMoveModel* pFieldMoveModel, s32 iconType);
s32 AddPoint( Field::MoveModel::FieldMoveModel* pFieldMoveModel, s32 value = 1 );
void SetPoint( s32 fureai_no, b32 point );
b32 IsAddPoint( const FieldContactPokemonAccessor::ContactPokemonData* pData );
b32 CreateEffect(Field::Effect::Type in_eType, Field::MoveModel::FieldMoveModel* pFieldMoveModel, bool in_bPlaySe = false, b32 voice_play = false, b32 is_keep = false, s32* pDataIndex = NULL);
b32 CloseEffect(Field::MoveModel::FieldMoveModel* pFieldMoveModel);
Field::Effect::IEffectBase* GetEffect(Field::MoveModel::FieldMoveModel* pFieldMoveModel);
b32 HaveEffect(Field::MoveModel::FieldMoveModel* pFieldMoveModel);
b32 PlayVoice(Field::MoveModel::FieldMoveModel* pFieldMoveModel, s32 type);
b32 IsStartSleep( Field::MoveModel::FieldMoveModel* pFieldMoveModel );

b32 IsNickNameKnow(const FieldContactPokemonAccessor::ContactPokemonData* pData);
void SetNickNameKnow(const FieldContactPokemonAccessor::ContactPokemonData* pData, b32 flag );

// なでなで
b32 IsNadeNadeRequestEventEnd( s32 fureai_no );
void SetNadeNadeRequestEventEnd( s32 fureai_no, b32 flag );

// 睡眠
void SetSleepEnd( s32 fureai_no, b32 flag );
b32 IsSleepEnd( s32 fureai_no );

b32 IsNickNameKnow( s32 fureai_no );
void SetNickNameKnow( s32 fureai_no, b32 flag );
s32 GetSaveWorkIndex( s32 fureai_no );
s32 GetTimeFlagIndex( s32 fureai_no );

// 親から子を取得(いない場合はNULL)
MoveModel::FieldMoveModel* GetChildPokemon( MoveModel::FieldMoveModel* pParent );

// 子から親を取得(いない場合はNULL)
MoveModel::FieldMoveModel* GetParentNpc( MoveModel::FieldMoveModel* pPokemon );


void VoiceUpdate(void);

GFL_NAMESPACE_END( PokemonFureai );
GFL_NAMESPACE_END( Field );

#endif // __FIELDPOKEMONFUREAITOOL_H_INCLUDED__
