//======================================================================
/**
 * @file	SequencePlayerTask.h
 * @brief	SEQ専用タスク処理
 * @author	Pete
 * @data	15/05/07
 */
//======================================================================
#if !defined( __SEQUENCEPLAYER_TASK_H__ )
#define __SEQUENCEPLAYER_TASK_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <math/include/gfl2_math.h>

#include "DemoLib/SequencePlayer/include/Player.h"

#include "DemoLib/Object/include/TaskSystem.h"
#include "DemoLib/Object/include/TaskFunc.h"

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

class BattleViewSystem;

//----------------------------------------------------------------------
//  カメラ用
//----------------------------------------------------------------------
 
extern void CreateTask_MoveCamera( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, SequenceViewSystem *seqCore , const gfl2::math::Vector3 &pos ,const gfl2::math::Vector3 &trg ,const f32 fov , const u32 step , DemoLib::Obj::TASK_MOVE_TYPE type = DemoLib::Obj::MOVE_LINER );
extern void CreateTask_ShakeCamera(DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, SequenceViewSystem *core ,f32 startRate , f32 endRate , const f32 dec, const u32 step , DemoLib::Obj::TASK_AXIS_TYPE axis);

//----------------------------------------------------------------------
//  BLOOM/GLARE用
//----------------------------------------------------------------------

extern void CreateTask_BloomAnimation( void *texMaskBuf,DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, SequenceViewSystem *seqCore , 
                                      const gfl2::math::Vector3 &weightRGBStart ,const gfl2::math::Vector3 &weightRGBEnd ,
                                      const f32 intensityStart , const f32 intensityEnd ,
                                      const f32 rangeStart , const f32 rangeEnd ,
                                      const f32 strengthStart , const f32 strengthEnd ,
                                      const u32 step);

extern void CreateTask_BloomAnimationParam(DemoLib::Obj::TaskSystem *taskSys, gfl2::heap::HeapBase *heap, SequenceViewSystem *seqCore,
                                      const gfl2::math::Vector3 &weightRGBStart, const gfl2::math::Vector3 &weightRGBEnd,
                                      const f32 intensityStart, const f32 intensityEnd,
                                      const f32 rangeStart, const f32 rangeEnd,
                                      const f32 strengthStart, const f32 strengthEnd,
                                      const u32 step);

//----------------------------------------------------------------------
//  FOG用
//----------------------------------------------------------------------

extern void CreateTask_FogAnimation( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, SequenceViewSystem *seqCore , 
                                      const gfl2::math::Vector3 &colorRGBStart ,const gfl2::math::Vector3 &colorRGBEnd ,
                                      const f32 nearStart , const f32 nearEnd ,
                                      const f32 farStart , const f32 farEnd ,
                                      const f32 strengthStart , const f32 strengthEnd ,
                                      const u32 step);

	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)


#endif  //__SEQUENCEPLAYER_TASK_H__
