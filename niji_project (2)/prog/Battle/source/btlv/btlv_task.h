//======================================================================
/**
 * @file	btlv_object.cpp
 * @brief	バトル描画 バトル専用タスク処理
 * @author	ariizumi
 * @data	15/05/05
 */
//======================================================================
#if !defined( __BTLV_TASK_H__ )
#define __BTLV_TASK_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>

#include "DemoLib/Object/include/TaskSystem.h"
#include "DemoLib/Object/include/TaskFunc.h"

#include "btlv_local_def.h"

GFL_NAMESPACE_BEGIN(btl)

class BattleViewSystem;
class BtlvObject_PokeModel;

//----------------------------------------------------------------------
//  ポケモン用
//----------------------------------------------------------------------
//ヒットバック
void CreateTask_PokeHitBack( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , BtlvObject_PokeModel* poke );
//登場演出
void CreateTask_PokeIntroMotion( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , BtlvPos vpos , BtlvObject_PokeModel* poke );
//ポケモン読み替え
void CreateTask_ReloadPokemon( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , BtlvPos vpos );
void CreateTask_HenshinPokemon( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , BtlvPos vpos , BtlvPos target);

//レアエフェクト
void CreateTask_PokeRareEffect( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore );

//ポケモン振り向き
void CreateTask_PokeLookAt( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , BtlvObject_PokeModel *poke , const gfl2::math::Vector3 &pos , int frame);

//----------------------------------------------------------------------
//  Obj用
//----------------------------------------------------------------------
//カメラ追従移動
void CreateTask_ObjectCameraAutoRotate( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , DemoLib::Obj::Object *obj, f32 length ,int frame);


//----------------------------------------------------------------------
//  カメラ用
//----------------------------------------------------------------------
//カメラ移動
extern void CreateTask_MoveCamera( DemoLib::Obj::TaskSystem *taskSys , 
                                   gfl2::heap::HeapBase *heap, 
                                   BattleViewSystem *btlvCore , 
                                   const gfl2::math::Vector3 &pos ,
                                   const gfl2::math::Vector3 &trg ,
                                   const f32 fov , 
                                   const f32 twist , 
                                   const u32 step , 
                                   DemoLib::Obj::TASK_MOVE_TYPE type = DemoLib::Obj::MOVE_LINER );
//カメラ揺らし
extern void CreateTask_ShakeCamera( 
                    DemoLib::Obj::TaskSystem *taskSys , 
                    gfl2::heap::HeapBase *heap, 
                    BattleViewSystem *btlvCore ,
                    f32 startRate , 
                    f32 endRate , 
                    const f32 dec, 
                    const u32 step , 
                    DemoLib::Obj::TASK_AXIS_TYPE axis);
//カメラ回転(Trg中心)
extern void CreateTask_RotateCamera_CenterTrg( 
                    DemoLib::Obj::TaskSystem *taskSys , 
                    gfl2::heap::HeapBase *heap, 
                    BattleViewSystem *btlvCore ,
                    const gfl2::math::Vector3 &pos , 
                    const gfl2::math::Vector3 &trg , 
                    f32 rotRad , 
                    int opt , 
                    int frame );
//カメラ回転(Pos中心)
extern void CreateTask_RotateCamera_CenterPos( 
                    DemoLib::Obj::TaskSystem *taskSys , 
                    gfl2::heap::HeapBase *heap, 
                    BattleViewSystem *btlvCore ,
                    const gfl2::math::Vector3 &pos , 
                    const gfl2::math::Vector3 &trg , 
                    f32 rotRad , 
                    int opt , 
                    int frame );



GFL_NAMESPACE_END(btl)


#endif  //__BTLV_TASK_H__