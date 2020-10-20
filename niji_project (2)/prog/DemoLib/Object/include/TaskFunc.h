#ifndef __TASKFUNC_H__
#define __TASKFUNC_H__
//======================================================================
/**
 * @file	TaskFunc.h
 * @brief	Object用タスク関数
 * @author	ariizumi
 * @data	15/04/23
 */
//======================================================================

#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

class TaskSystem;
class Object;
class Object_GfMdl;
class Object_Effect;
class Object_Layout;
class Object_Poke;
class Object_Chara;
class Object_Cluster;
class Object_Light;

typedef enum
{
  MOVE_LINER,
  MOVE_FIRST_FAST,
  MOVE_FIRST_SLOW,
  MOVE_SLOW_FAST_SLOW,
}TASK_MOVE_TYPE;

typedef enum
{
  AXIS_X,
  AXIS_Y,
  AXIS_Z,
  AXIS_X_ROT, //揺れ用(本体の回転を加味する、XZの併用不可
  AXIS_Z_ROT, //揺れ用(本体の回転を加味する、XZの併用不可
}TASK_AXIS_TYPE;
//----------------------------------------------------------------------
//  Object汎用
//----------------------------------------------------------------------
extern void CreateTask_ObjectMove( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type = MOVE_LINER );
extern void CreateTask_ObjectMoveOffset( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type = MOVE_LINER );
extern void CreateTask_ObjectScale( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type = MOVE_LINER );
extern void CreateTask_ObjectRotate( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type = MOVE_LINER );
extern void CreateTask_ObjectRotateOffset( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type = MOVE_LINER );
extern void CreateTask_ObjMoveShake( 
                    TaskSystem *taskSys , 
                    gfl2::heap::HeapBase *heap,
                    Object *target ,
                    f32 startRate , 
                    f32 endRate , 
                    const f32 startDec, 
                    const f32 endDec, 
                    const u32 step , 
                    TASK_AXIS_TYPE axis);

//----------------------------------------------------------------------
//  Object_GfMdl用関数
//----------------------------------------------------------------------
//モデル自動削除(1フレ遅れ対応版)
extern void CreateTask_DeleteObject_GfMdl(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_GfMdl *obj , const int frame);

//コンスタントカラー操作
extern void CreateTask_ObjectGfMdl_ChangeConstantColor(TaskSystem *taskSys , 
                                                       gfl2::heap::HeapBase *heap, 
                                                       Object_GfMdl *obj , 
                                                       const char* matName , 
                                                       const int colNo , 
                                                       const gfl2::math::Vector4 &start ,
                                                       const gfl2::math::Vector4 &end ,
                                                       const int frame );

//----------------------------------------------------------------------
//  Object_Chara用関数
//----------------------------------------------------------------------
//モデル自動削除(1フレ遅れ対応版)
extern void CreateTask_DeleteObject_Chara(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Chara *obj , const int frame);


//----------------------------------------------------------------------
//  Object_Effect用関数
//----------------------------------------------------------------------
//エフェクト自動削除(1フレ遅れ対応版)
extern void CreateTask_DeleteObject_Effect(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Effect *obj , const int frame);

//----------------------------------------------------------------------
//  Object_Cluster用関数
//----------------------------------------------------------------------
//群体自動削除(1フレ遅れ対応版)
extern void CreateTask_DeleteObject_Cluster(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Cluster *obj , const int frame);

//----------------------------------------------------------------------
//  Object_Layout用関数
//----------------------------------------------------------------------
//LAYOUT自動削除(1フレ遅れ対応版)
extern void CreateTask_DeleteObject_Layout(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Layout *obj , const int frame);

// Layout PaneMove
extern void CreateTask_ObjectMove_LayoutPane( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Layout *obj,int paneIndex ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type = MOVE_LINER );

// Layout PaneScale
extern void CreateTask_ObjectScale_LayoutPane( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Layout *obj,int paneIndex ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type = MOVE_LINER );

// Layout PaneAlpha
extern void CreateTask_ObjectAlpha_LayoutPane( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Layout *obj,int paneIndex ,const u8 startAlpha ,const u8 endAlpha ,const u32 step , const TASK_MOVE_TYPE type = MOVE_LINER );


//----------------------------------------------------------------------
//  Object_Poke用関数
//----------------------------------------------------------------------
//POKEモデル自動削除(1フレ遅れ対応版)
extern void CreateTask_DeleteObject_Poke(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Poke *obj , const int frame);

//カラーシェーダー操作
extern void CreateTask_ChangePokeColorShader(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Poke *obj , const gfl2::math::Vector4 &start ,const gfl2::math::Vector4 &end , const int frame);


//----------------------------------------------------------------------
//  Object_Light用関数
//----------------------------------------------------------------------
//POKEモデル自動削除(1フレ遅れ対応版)
extern void CreateTask_DeleteObject_Light(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Light *obj , const int frame);

//----------------------------------------------------------------------
//  Soundパラメータ操作
//----------------------------------------------------------------------
//SE操作タスク用
enum
{
  SE_CTRL_VOLUME,
  SE_CTRL_PAN,
  SE_CTRL_PITCH,
  SE_CTRL_TEMPO,
};
//SE操作タスク
extern void CreateTask_SeCtrl( TaskSystem *taskSys , gfl2::heap::HeapBase *haep, f32 start , f32 end , int frame , int seIdx , int grpNo , int type );


GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
#endif // __TASKFUNC_H__
