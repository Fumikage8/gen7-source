//======================================================================
/**
 * @file	SequencePlayerTask.c++
 * @brief	SEQタスクシステム
 * @author	Pete
 * @data	15/5/7
 */
//======================================================================

#include <Math/include/gfl2_TransporterTypes.h>

#include "DemoLib/Object/include/TaskSystem.h"
#include "DemoLib/Object/include/TaskFunc.h"

#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include "DemoLib/SequencePlayer/include/SequencePlayerTask.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

	//カメラ移動
struct TaskWork_MoveCamera
{
	SequenceViewSystem* mCore;

	gfl2::math::TransBase<gfl2::math::Vector3> *mTransPos;
	gfl2::math::TransBase<gfl2::math::Vector3> *mTransTrg;
	gfl2::math::TransLinerF32 *mTransFov;
};

void TaskFunc_CameraMove(DemoLib::Obj::Task *task, void *userWork,int step)
{
	TaskWork_MoveCamera *work = static_cast<TaskWork_MoveCamera*>(userWork);

  gfl2::math::Vector3 pos;
	gfl2::math::Vector3 trg;

  pos.Set(0.0f,0.0f,0.0f);
  trg.Set(0.0f,0.0f,0.0f);
	
  for(int n=0;n<step;n++)
  {
    pos = work->mTransPos->Update();
    trg = work->mTransTrg->Update();
    work->mTransFov->Update();
  }

	if(step>0) work->mCore->SetCameraPosition(pos,trg);
	
  if( work->mTransPos->IsEnd() == true &&
		work->mTransTrg->IsEnd() == true &&
		work->mTransFov->IsEnd() == true )
	{
		task->GetSystem()->Delete(task);
		GFL_DELETE work->mTransPos;
		GFL_DELETE work->mTransTrg;
		GFL_DELETE work->mTransFov;
	}
}


void CreateTask_MoveCamera( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, SequenceViewSystem *seqCore , const gfl2::math::Vector3 &pos ,const gfl2::math::Vector3 &trg ,const f32 fov , const u32 step , DemoLib::Obj::TASK_MOVE_TYPE type )
{
	DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_CameraMove,sizeof(TaskWork_MoveCamera),DemoLib::Obj::TASK_PRI_LOWEST);
	TaskWork_MoveCamera *work = static_cast<TaskWork_MoveCamera*>(task->GetWork());

	work->mCore = seqCore;

	gfl2::math::Vector3 sPos = seqCore->GetCameraPosition();
	gfl2::math::Vector3 sTrg = seqCore->GetCameraTarget();
	f32 sFov = seqCore->GetCameraFov();
	f32 eFov;
	if( fov == 0 )
	{
		eFov = sFov;
	}
	else
	{
		eFov = fov;
	}

	switch(type)
	{
	case DemoLib::Obj::MOVE_LINER:
		work->mTransPos = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransLinerF32 >(sPos,pos,step,heap);
		work->mTransTrg = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransLinerF32 >(sTrg,trg,step,heap);
		break;
	case DemoLib::Obj::MOVE_FIRST_FAST:
		work->mTransPos = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveFastF32 >(sPos,pos,step,heap);
		work->mTransTrg = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveFastF32 >(sTrg,trg,step,heap);
		break;
	case DemoLib::Obj::MOVE_FIRST_SLOW:
		work->mTransPos = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveSlowF32 >(sPos,pos,step,heap);
		work->mTransTrg = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveSlowF32 >(sTrg,trg,step,heap);
		break;
	case DemoLib::Obj::MOVE_SLOW_FAST_SLOW:
		work->mTransPos = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveSFS<f32> >(sPos,pos,step,heap);
		work->mTransTrg = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveSFS<f32> >(sTrg,trg,step,heap);
		break;
	}
	work->mTransFov = GFL_NEW(heap) gfl2::math::TransLinerF32(sFov,eFov,step);
}

//カメラ揺らし
struct TaskWork_SEQ_ShakeCamera
{
  SequenceViewSystem* mCore;

  gfl2::math::TransSin *mTransSin;
  gfl2::math::TransLinerF32 *mTransRate;
  DemoLib::Obj::TASK_AXIS_TYPE mAxis;
};

void TaskFunc_SEQ_ShakeCamera( DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_SEQ_ShakeCamera *work = static_cast<TaskWork_SEQ_ShakeCamera*>(userWork);

  f32 sin = 0.0f;
  f32 rate = 0.0f;

  for(int n=0;n<step;n++)
  {
    sin = work->mTransSin->Update();
    rate = work->mTransRate->Update();
  }

  sin *= rate;

  gfl2::math::Vector3 ofs = work->mCore->GetCameraOffset();

  switch( work->mAxis )
  {
  case DemoLib::Obj::AXIS_X:
    ofs.x = sin;
    break;
  case DemoLib::Obj::AXIS_Y:
    ofs.y = sin;
    break;
  case DemoLib::Obj::AXIS_Z:
    ofs.z = sin;
    break;
  }

  if(step>0) work->mCore->SetCameraOffset(ofs);
  
  if( work->mTransSin->IsEnd() == true &&
    work->mTransRate->IsEnd() == true )
  {
    GFL_DELETE work->mTransSin;
    GFL_DELETE work->mTransRate;
    task->GetSystem()->Delete(task);
  }
}

void CreateTask_ShakeCamera( 
  DemoLib::Obj::TaskSystem *taskSys , 
  gfl2::heap::HeapBase *heap, 
  SequenceViewSystem *core ,
  f32 startRate , 
  f32 endRate , 
  const f32 dec, 
  const u32 step , 
  DemoLib::Obj::TASK_AXIS_TYPE axis)
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_SEQ_ShakeCamera,sizeof(TaskWork_SEQ_ShakeCamera),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_SEQ_ShakeCamera *work = static_cast<TaskWork_SEQ_ShakeCamera*>(task->GetWork());

  work->mCore = core;
  work->mTransSin = GFL_NEW(heap) gfl2::math::TransSin(0,dec*360,step,1);
  work->mTransRate = GFL_NEW(heap) gfl2::math::TransLinerF32(startRate,endRate,step);
  work->mAxis = axis;
}

//----------------------------------------------------------------------
//  BLOOM/GLARE用
//----------------------------------------------------------------------

struct TaskWork_BloomAnimation
{
  SequenceViewSystem* mCore;
  gfl2::math::TransBase<gfl2::math::Vector4> *mTransWeightRGB;  
  gfl2::math::TransLinerF32 *mTransIntensity;
  gfl2::math::TransLinerF32 *mTransRange;
  gfl2::math::TransLinerF32 *mTransStrength;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mDrawEnvResourceAlphaMask;

  bool mOnlyParam;
};


void TaskFunc_BloomAnimation(DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_BloomAnimation *work = static_cast<TaskWork_BloomAnimation*>(userWork);
  
  for(int n=0;n<step;n++)
  {
    work->mTransWeightRGB->Update();
    work->mTransIntensity->Update();
    work->mTransRange->Update();
    work->mTransStrength->Update();
  }

  work->mCore->GetRenderSys()->GetRenderingPipeLine()->SetBloomParameters(work->mTransWeightRGB->GetValue(),
    work->mTransIntensity->GetValue(),
    work->mTransRange->GetValue(),
    work->mTransStrength->GetValue() );
  
  if( work->mTransWeightRGB->IsEnd() == true &&
    work->mTransIntensity->IsEnd() == true &&
    work->mTransRange->IsEnd() == true &&
    work->mTransStrength->IsEnd() )
  {
    if (work->mOnlyParam == false)
    {
      if (work->mCore->GetRenderSys()->GetRenderingPipeLine()->IsEnabledBloom())
      {
        work->mCore->GetRenderSys()->GetRenderingPipeLine()->DisableBloom();
        work->mCore->GetRenderSys()->GetRenderingPipeLine()->SetBloomMaskTexture(NULL); // Remove Mask Texture
      }

      else
      {
        GFL_SAFE_DELETE(work->mDrawEnvResourceAlphaMask);
        task->GetSystem()->Delete(task);
        GFL_DELETE work->mTransWeightRGB;
        GFL_DELETE work->mTransIntensity;
        GFL_DELETE work->mTransRange;
        GFL_DELETE work->mTransStrength;
      }
    }
    else
    {
      task->GetSystem()->Delete(task);
      GFL_DELETE work->mTransWeightRGB;
      GFL_DELETE work->mTransIntensity;
      GFL_DELETE work->mTransRange;
      GFL_DELETE work->mTransStrength;
    }
  }
}

void CreateTask_BloomAnimation( void *texMaskBuf,DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, SequenceViewSystem *seqCore , 
                                      const gfl2::math::Vector3 &weightRGBStart ,const gfl2::math::Vector3 &weightRGBEnd ,
                                      const f32 intensityStart , const f32 intensityEnd ,
                                      const f32 rangeStart , const f32 rangeEnd ,
                                      const f32 strengthStart , const f32 strengthEnd ,
                                      const u32 step )
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_BloomAnimation,sizeof(TaskWork_BloomAnimation),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_BloomAnimation *work = static_cast<TaskWork_BloomAnimation*>(task->GetWork());

  seqCore->GetRenderSys()->GetRenderingPipeLine()->EnableBloom();

  work->mCore = seqCore;
  
  work->mTransWeightRGB = GFL_NEW(heap) gfl2::math::TransVec4<gfl2::math::TransLiner<f32> >(weightRGBStart,weightRGBEnd,step,heap);
  work->mTransIntensity = GFL_NEW(heap) gfl2::math::TransLinerF32(intensityStart,intensityEnd,step);
  work->mTransRange = GFL_NEW(heap) gfl2::math::TransLinerF32(rangeStart,rangeEnd,step);
  work->mTransStrength = GFL_NEW(heap) gfl2::math::TransLinerF32(strengthStart,strengthEnd,step);

  work->mOnlyParam = false;

  // Bloom Mask Texture:
  gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
  texData.SetTextureData(static_cast<c8*>(texMaskBuf));
  work->mDrawEnvResourceAlphaMask = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(seqCore->GetGLAllocator(), &texData);

  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* texResNode = poke_3d::renderer::BloomRenderPath::GetTextureResourceNode(work->mDrawEnvResourceAlphaMask);
  seqCore->GetRenderSys()->GetRenderingPipeLine()->SetBloomMaskTexture(texResNode->GetTexture());
}


//数値変更のみ
void CreateTask_BloomAnimationParam(DemoLib::Obj::TaskSystem *taskSys, gfl2::heap::HeapBase *heap, SequenceViewSystem *seqCore,
  const gfl2::math::Vector3 &weightRGBStart, const gfl2::math::Vector3 &weightRGBEnd,
  const f32 intensityStart, const f32 intensityEnd,
  const f32 rangeStart, const f32 rangeEnd,
  const f32 strengthStart, const f32 strengthEnd,
  const u32 step)
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_BloomAnimation, sizeof(TaskWork_BloomAnimation), DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_BloomAnimation *work = static_cast<TaskWork_BloomAnimation*>(task->GetWork());

  work->mCore = seqCore;

  work->mTransWeightRGB = GFL_NEW(heap) gfl2::math::TransVec4<gfl2::math::TransLiner<f32> >(weightRGBStart, weightRGBEnd, step, heap);
  work->mTransIntensity = GFL_NEW(heap) gfl2::math::TransLinerF32(intensityStart, intensityEnd, step);
  work->mTransRange = GFL_NEW(heap) gfl2::math::TransLinerF32(rangeStart, rangeEnd, step);
  work->mTransStrength = GFL_NEW(heap) gfl2::math::TransLinerF32(strengthStart, strengthEnd, step);

  work->mOnlyParam = true;
}
//----------------------------------------------------------------------
//  FOG用
//----------------------------------------------------------------------

struct TaskWork_FogAnimation
{
  SequenceViewSystem* mCore;
  gfl2::math::TransBase<gfl2::math::Vector4> *mTransColorRGB;  
  gfl2::math::TransLinerF32 *mTransNear;
  gfl2::math::TransLinerF32 *mTransFar;
  gfl2::math::TransLinerF32 *mTransStrength;  
};


void TaskFunc_FogAnimation(DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_FogAnimation *work = static_cast<TaskWork_FogAnimation*>(userWork);

  for(int n=0;n<step;n++)
  {
    work->mTransColorRGB->Update();
    work->mTransNear->Update();
    work->mTransFar->Update();
    work->mTransStrength->Update();
  }

    work->mCore->GetRenderSys()->GetRenderingPipeLine()->SetFogParameters(work->mTransColorRGB->GetValue(),
    work->mTransNear->GetValue(),
    work->mTransFar->GetValue(),
    work->mTransStrength->GetValue() );

  if( work->mTransColorRGB->IsEnd() == true &&
    work->mTransNear->IsEnd() == true &&
    work->mTransFar->IsEnd() == true &&
    work->mTransStrength->IsEnd() )
  {
    work->mCore->GetRenderSys()->GetRenderingPipeLine()->DisableFog();     
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTransColorRGB;
    GFL_DELETE work->mTransNear;
    GFL_DELETE work->mTransFar;
    GFL_DELETE work->mTransStrength;
  
  }
}


void CreateTask_FogAnimation( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, SequenceViewSystem *seqCore , 
                               const gfl2::math::Vector3 &colorRGBStart ,const gfl2::math::Vector3 &colorRGBEnd ,
                               const f32 nearStart , const f32 nearEnd ,
                               const f32 farStart , const f32 farEnd ,
                               const f32 strengthStart , const f32 strengthEnd ,
                               const u32 step )
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_FogAnimation,sizeof(TaskWork_FogAnimation),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_FogAnimation *work = static_cast<TaskWork_FogAnimation*>(task->GetWork());

  seqCore->GetRenderSys()->GetRenderingPipeLine()->EnableFog();

  work->mCore = seqCore;
  work->mTransColorRGB = GFL_NEW(heap) gfl2::math::TransVec4<gfl2::math::TransLiner<f32> >(colorRGBStart,colorRGBEnd,step,heap);
  work->mTransNear = GFL_NEW(heap) gfl2::math::TransLinerF32(nearStart,nearEnd,step);
  work->mTransFar = GFL_NEW(heap) gfl2::math::TransLinerF32(farStart,farEnd,step);
  work->mTransStrength = GFL_NEW(heap) gfl2::math::TransLinerF32(strengthStart,strengthEnd,step);  
}


	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

