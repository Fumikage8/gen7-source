//======================================================================
/**
 * @file	TaskFunc.cpp
 * @brief	Object用タスク関数
 * @author	ariizumi
 * @data	15/04/23
 */
//======================================================================
#include <util/include/gfl2_std_string.h>
#include <Debug/include/gfl2_DebugPrint.h>
#include <Math/include/gfl2_TransporterTypes.h>

#include "sound/include/Sound.h"

#include "DemoLib/Object/include/TaskSystem.h"
#include "DemoLib/Object/include/TaskFunc.h"

#include "DemoLib/Object/include/Object_GfMdl.h"
#include "DemoLib/Object/include/Object_Chara.h"
#include "DemoLib/Object/include/Object_Effect.h"
#include "DemoLib/Object/include/Object_Layout.h"
#include "DemoLib/Object/include/Object_Cluster.h"
#include "DemoLib/Object/include/Object_Poke.h"
#include "DemoLib/Object/include/Object_Light.h"


GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

//----------------------------------------------------------------------
//  Object汎用
//----------------------------------------------------------------------
struct TaskWork_ObjectMoveCommon
{
  gfl2::math::TransBase<gfl2::math::Vector3> *mTrans;
  Object *mObj;
};

//Transporterを返す関数
gfl2::math::TransBase<gfl2::math::Vector3>* TaskUtil_CreateTransBase(gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type)
{
  switch(type)
  {
  case MOVE_LINER:
    return GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransLiner<f32> >(start,end,step,heap);
  case MOVE_FIRST_SLOW:
    return GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveSlow<f32> >(start,end,step,heap);
  case MOVE_FIRST_FAST:
    return GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveFast<f32> >(start,end,step,heap);
  case MOVE_SLOW_FAST_SLOW:
    return GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveSFS<f32> >(start,end,step,heap);
  }
  return NULL;
}

//Object移動
void TaskFunc_ObjectMove(Task *task, void *userWork,int step)
{
  TaskWork_ObjectMoveCommon *work = static_cast<TaskWork_ObjectMoveCommon*>(userWork);

  // Frame Skip:
  for(;step>1;step--) work->mTrans->Update();

  const gfl2::math::Vector3 pos = work->mTrans->Update();
  work->mObj->SetPosition(pos);
  if( work->mTrans->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTrans;
  }
}

void CreateTask_ObjectMove( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type)
{
  Task *task = taskSys->Create(TaskFunc_ObjectMove,sizeof(TaskWork_ObjectMoveCommon),TASK_PRI_LOWEST);
  TaskWork_ObjectMoveCommon *work = static_cast<TaskWork_ObjectMoveCommon*>(task->GetWork());

  work->mObj = target;
  work->mTrans = TaskUtil_CreateTransBase(heap,target,start,end,step,type);
}

//Object Offset移動
void TaskFunc_ObjectMoveOffset(Task *task, void *userWork,int step)
{
  TaskWork_ObjectMoveCommon *work = static_cast<TaskWork_ObjectMoveCommon*>(userWork);

  // Frame Skip:
  for(;step>1;step--) work->mTrans->Update();

  const gfl2::math::Vector3 pos = work->mTrans->Update();
  work->mObj->SetOffset(pos);
  if( work->mTrans->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTrans;
  }
}

void CreateTask_ObjectMoveOffset( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type)
{
  Task *task = taskSys->Create(TaskFunc_ObjectMoveOffset,sizeof(TaskWork_ObjectMoveCommon),TASK_PRI_LOWEST);
  TaskWork_ObjectMoveCommon *work = static_cast<TaskWork_ObjectMoveCommon*>(task->GetWork());

  work->mObj = target;
  work->mTrans = TaskUtil_CreateTransBase(heap,target,start,end,step,type);
}

//Objectスケール
void TaskFunc_ObjectScale(Task *task, void *userWork,int step)
{
  TaskWork_ObjectMoveCommon *work = static_cast<TaskWork_ObjectMoveCommon*>(userWork);

  // Frame Skip:
  for(;step>1;step--) work->mTrans->Update();

  const gfl2::math::Vector3 scale = work->mTrans->Update();
  work->mObj->SetScale(scale);
  if( work->mTrans->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTrans;
  }
}
void CreateTask_ObjectScale( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type)
{
  Task *task = taskSys->Create(TaskFunc_ObjectScale,sizeof(TaskWork_ObjectMoveCommon),TASK_PRI_LOWEST);
  TaskWork_ObjectMoveCommon *work = static_cast<TaskWork_ObjectMoveCommon*>(task->GetWork());

  work->mObj = target;
  work->mTrans = TaskUtil_CreateTransBase(heap,target,start,end,step,type);
}

//Object回転
void TaskFunc_ObjectRotate(Task *task, void *userWork,int step)
{
  TaskWork_ObjectMoveCommon *work = static_cast<TaskWork_ObjectMoveCommon*>(userWork);
  
  for(;step>1;step--) work->mTrans->Update();
  const gfl2::math::Vector3 rot = work->mTrans->Update();

  work->mObj->SetRotate(rot);
  if( work->mTrans->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTrans;
  }
}
void CreateTask_ObjectRotate( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type)
{
  Task *task = taskSys->Create(TaskFunc_ObjectRotate,sizeof(TaskWork_ObjectMoveCommon),TASK_PRI_LOWEST);
  TaskWork_ObjectMoveCommon *work = static_cast<TaskWork_ObjectMoveCommon*>(task->GetWork());

  work->mObj = target;
  work->mTrans = TaskUtil_CreateTransBase(heap,target,start,end,step,type);
}

//Object回転(オフセット
void TaskFunc_ObjectRotateOffset(Task *task, void *userWork,int step)
{
  TaskWork_ObjectMoveCommon *work = static_cast<TaskWork_ObjectMoveCommon*>(userWork);
  
  for(;step>1;step--) work->mTrans->Update();
  const gfl2::math::Vector3 rot = work->mTrans->Update();

  work->mObj->SetRotateOffset(rot);
  if( work->mTrans->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTrans;
  }
}
void CreateTask_ObjectRotateOffset( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object *target ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type)
{
  Task *task = taskSys->Create(TaskFunc_ObjectRotateOffset,sizeof(TaskWork_ObjectMoveCommon),TASK_PRI_LOWEST);
  TaskWork_ObjectMoveCommon *work = static_cast<TaskWork_ObjectMoveCommon*>(task->GetWork());

  work->mObj = target;
  work->mTrans = TaskUtil_CreateTransBase(heap,target,start,end,step,type);
}

//オブジェ揺らし
struct TaskWork_ObjMoveShake
{
  Object *mObj;

  gfl2::math::TransSin *mTransSin;
  gfl2::math::TransLinerF32 *mTransRate;
  TASK_AXIS_TYPE mAxis;
  u32 padding;
};

void TaskFunc_ObjMoveShake(Task *task, void *userWork,int step)
{
  TaskWork_ObjMoveShake *work = static_cast<TaskWork_ObjMoveShake*>(userWork);

  // FrameSkip:
  for(;step>1;step--)
  {
    work->mTransSin->Update();
    work->mTransRate->Update();
  }


  gfl2::math::Vector3 ofs = work->mObj->GetOffset();
  {
    f32 sin = work->mTransSin->Update();
    const f32 rate = work->mTransRate->Update();

    sin *= rate;
  
  
    switch( work->mAxis )
    {
    case AXIS_X:
      ofs.x = sin;
      break;
    case AXIS_Y:
      ofs.y = sin;
      break;
    case AXIS_Z:
      ofs.z = sin;
      break;

    case AXIS_X_ROT:
      ofs.x = sin;
      ofs.z = 0;
      break;
    case AXIS_Z_ROT:
      ofs.x = 0;
      ofs.z = sin;
      break;
    }
  }
  
  if( work->mAxis == AXIS_X_ROT ||
      work->mAxis == AXIS_Z_ROT )
  {
    f32 sinV,cos;
    const f32 wx = ofs.x;
    const f32 wz = ofs.z;
    gfl2::math::SinCosDeg( &sinV,&cos,-work->mObj->GetRotateY() );
    ofs.x = wx*cos - wz*sinV;
    ofs.z = wx*sinV + wz*cos;
  }

  work->mObj->SetOffset(ofs);
  if( work->mTransSin->IsEnd() == true &&
      work->mTransRate->IsEnd() == true )
  {
    GFL_DELETE work->mTransSin;
    GFL_DELETE work->mTransRate;
    task->GetSystem()->Delete(task);
  }

}

void CreateTask_ObjMoveShake( 
              TaskSystem *taskSys , 
              gfl2::heap::HeapBase *heap, 
              Object *target ,
              f32 startRate , 
              f32 endRate , 
              const f32 startDec, 
              const f32 endDec, 
              const u32 step , 
              TASK_AXIS_TYPE axis)
{
  Task *task = taskSys->Create(TaskFunc_ObjMoveShake,sizeof(TaskWork_ObjMoveShake),TASK_PRI_DEFAULT);
  TaskWork_ObjMoveShake *work = static_cast<TaskWork_ObjMoveShake*>(task->GetWork());

  work->mObj = target;
  work->mTransSin = GFL_NEW(heap) gfl2::math::TransSin(startDec,endDec,step,1);
  work->mTransRate = GFL_NEW(heap) gfl2::math::TransLinerF32(startRate,endRate,step);
  work->mAxis = axis;

}

//----------------------------------------------------------------------
//  Object_Mdl用
//----------------------------------------------------------------------

//モデル自動削除(1フレ遅れ対応版)
struct TaskWork_DeleteObject_GfMdl
{
  Object_GfMdl *mObj;
  int mFrame;
};

void TaskFunc_DeleteObject_GfMdl(Task *task, void *userWork,int step)
{
  TaskWork_DeleteObject_GfMdl *work = static_cast<TaskWork_DeleteObject_GfMdl*>(userWork);
  
  // DO NOT PROCESS DURING SYSTEM SYNC:  
  if(task->GetSystem()->IsSyncMode()) step = 0;

  work->mFrame-=step;

  if( work->mFrame <= 0 )
  {
    if( work->mObj->IsAddRenderPath() )
    {
      work->mObj->RemoveRenderPath();
    }
    else
    {
      work->mObj->Delete();
      task->GetSystem()->Delete(task);
    }
  }
}

void CreateTask_DeleteObject_GfMdl(TaskSystem *taskSys , gfl2::heap::HeapBase *haep, Object_GfMdl *obj , const int frame)
{
  Task *task = taskSys->Create(TaskFunc_DeleteObject_GfMdl,sizeof(TaskWork_DeleteObject_GfMdl),TASK_PRI_LOWEST);
  TaskWork_DeleteObject_GfMdl *work = static_cast<TaskWork_DeleteObject_GfMdl*>(task->GetWork());
  
  work->mObj = obj;
  work->mFrame = frame;
}

//コンスタントカラー操作
struct TaskWork_ObjectGfMdl_ChangeConstantColor
{
  Object_GfMdl *mObj;
  gfl2::math::TransBase<gfl2::math::Vector4> *mTrans;
  char matName[32];
  int  colNo;
};

void TaskFunc_ObjectGfMdl_ChangeConstantColor(Task *task, void *userWork,int step)
{
  TaskWork_ObjectGfMdl_ChangeConstantColor *work = static_cast<TaskWork_ObjectGfMdl_ChangeConstantColor*>(userWork);

  // Frame Skip:
  for(;step>1;step--) work->mTrans->Update();

  const gfl2::math::Vector4 col = work->mTrans->Update();
  if( work->matName[0] == '\0' )
  {
    work->mObj->SetConstantColor(NULL,work->colNo,col);
  }
  else
  {
    work->mObj->SetConstantColor(work->matName,work->colNo,col);
  }

  if( work->mTrans->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTrans;
  }
}

void CreateTask_ObjectGfMdl_ChangeConstantColor(
                                TaskSystem *taskSys , 
                                gfl2::heap::HeapBase *heap, 
                                Object_GfMdl *obj , 
                                const char* matName , 
                                const int colNo , 
                                const gfl2::math::Vector4 &start ,
                                const gfl2::math::Vector4 &end ,
                                const int frame )
{
  Task *task = taskSys->Create(TaskFunc_ObjectGfMdl_ChangeConstantColor,sizeof(TaskWork_ObjectGfMdl_ChangeConstantColor),TASK_PRI_DEFAULT);
  TaskWork_ObjectGfMdl_ChangeConstantColor *work = static_cast<TaskWork_ObjectGfMdl_ChangeConstantColor*>(task->GetWork());

  work->mObj = obj;
  work->mTrans = GFL_NEW(heap) gfl2::math::TransVec4<gfl2::math::TransLiner<f32> >(start,end,frame,heap);
  work->colNo = colNo;
  if( matName )
  {
#if   defined(GF_PLATFORM_CTR)
    nw::ut::strcpy(work->matName, 32, matName);
#elif defined(GF_PLATFORM_WIN32)
    strcpy_s(work->matName, 32, matName);  // 終端の'\0'を含めた長さを渡す
#endif  // GF_PLATFORM_
  }
  else
  {
    work->matName[0] = '\0';
  }

}

//----------------------------------------------------------------------
//  Object_Chara用
//----------------------------------------------------------------------

//モデル自動削除(1フレ遅れ対応版)
struct TaskWork_DeleteObject_Chara
{
  Object_Chara *mObj;
  int mFrame;
};

void TaskFunc_DeleteObject_Chara(Task *task, void *userWork,int step)
{
  TaskWork_DeleteObject_Chara *work = static_cast<TaskWork_DeleteObject_Chara*>(userWork);

  // DO NOT PROCESS DURING SYSTEM SYNC:  
  if(task->GetSystem()->IsSyncMode()) step = 0;

  work->mFrame-=step;

  if( work->mFrame <= 0 )
  {
    if( work->mObj->IsAddRenderPath() )
    {
      work->mObj->RemoveRenderPath();      
    }
    else
    {      
      work->mObj->Delete();
      task->GetSystem()->Delete(task);
    }
  }
}

void CreateTask_DeleteObject_Chara(TaskSystem *taskSys , gfl2::heap::HeapBase *haep, Object_Chara *obj , const int frame)
{
  Task *task = taskSys->Create(TaskFunc_DeleteObject_Chara,sizeof(TaskWork_DeleteObject_Chara),TASK_PRI_LOWEST);
  TaskWork_DeleteObject_Chara *work = static_cast<TaskWork_DeleteObject_Chara*>(task->GetWork());

  work->mObj = obj;
  work->mFrame = frame;
}


//----------------------------------------------------------------------
//  Object_Effect用関数
//----------------------------------------------------------------------
//エフェクト自動削除(1フレ遅れ対応版)
struct TaskWork_DeleteObject_Effect
{
  Object_Effect *mObj;
  int mFrame;
};

void TaskFunc_DeleteObject_Effect(Task *task, void *userWork,int step)
{
  TaskWork_DeleteObject_Effect *work = static_cast<TaskWork_DeleteObject_Effect*>(userWork);

  // DO NOT PROCESS DURING SYSTEM SYNC:  
  if(task->GetSystem()->IsSyncMode()) step = 0;

  work->mFrame-=step;

  if( work->mFrame <= 0 )
  {
    if( work->mObj->IsAddRenderPath() )
    {
      work->mObj->RemoveRenderPath();
    }
    else
    {
      work->mObj->Delete();
      task->GetSystem()->Delete(task);
    }
  }
}
void CreateTask_DeleteObject_Effect(TaskSystem *taskSys , gfl2::heap::HeapBase *haep, Object_Effect *obj , const int frame)
{
  Task *task = taskSys->Create(TaskFunc_DeleteObject_Effect,sizeof(TaskWork_DeleteObject_Effect),TASK_PRI_LOWEST);
  TaskWork_DeleteObject_Effect *work = static_cast<TaskWork_DeleteObject_Effect*>(task->GetWork());

  work->mObj = obj;
  work->mFrame = frame;
}
//----------------------------------------------------------------------
//  Object_Cluster用関数
//----------------------------------------------------------------------
//群体自動削除(1フレ遅れ対応版)
struct TaskWork_DeleteObject_Cluster
{
  Object_Cluster *mObj;
  int mFrame;
};

void TaskFunc_DeleteObject_Cluster(Task *task, void *userWork,int step)
{
  TaskWork_DeleteObject_Cluster *work = static_cast<TaskWork_DeleteObject_Cluster*>(userWork);

  // DO NOT PROCESS DURING SYSTEM SYNC:  
  if(task->GetSystem()->IsSyncMode()) step = 0;

  work->mFrame-=step;

  if( work->mFrame <= 0 )
  {
    if( work->mObj->Delete() )
    {
      task->GetSystem()->Delete(task);
    }
  }
}

void CreateTask_DeleteObject_Cluster(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Cluster *obj , const int frame)
{
  Task *task = taskSys->Create(TaskFunc_DeleteObject_Cluster,sizeof(TaskWork_DeleteObject_Cluster),TASK_PRI_LOWEST);
  TaskWork_DeleteObject_Cluster *work = static_cast<TaskWork_DeleteObject_Cluster*>(task->GetWork());

  work->mObj = obj;
  work->mFrame = frame;
}


//----------------------------------------------------------------------
//  Object_LAYOUT用
//----------------------------------------------------------------------

//LAYOUT自動削除(1フレ遅れ対応版)
struct TaskWork_DeleteObject_Layout
{
	Object_Layout *mObj;
	int mFrame;
};

void TaskFunc_DeleteObject_Layout(Task *task, void *userWork,int step)
{
	TaskWork_DeleteObject_Layout *work = static_cast<TaskWork_DeleteObject_Layout*>(userWork);

  // DO NOT PROCESS DURING SYSTEM SYNC:  
  if(task->GetSystem()->IsSyncMode()) step = 0;

	work->mFrame-=step;

	if( work->mFrame <= 0 )
	{
		if( work->mObj->IsAddRenderPath() )
		{
			work->mObj->RemoveRenderPath();
		}
		else
		{
			work->mObj->Delete();
			task->GetSystem()->Delete(task);
		}
	}
}

void CreateTask_DeleteObject_Layout(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Layout *obj , const int frame)
{
	Task *task = taskSys->Create(TaskFunc_DeleteObject_Layout,sizeof(TaskWork_DeleteObject_Layout),TASK_PRI_LOWEST);
	TaskWork_DeleteObject_Layout *work = static_cast<TaskWork_DeleteObject_Layout*>(task->GetWork());

	work->mObj = obj;
	work->mFrame = frame;
}


struct TaskWork_LayoutMoveCommon
{
  gfl2::math::TransBase<gfl2::math::Vector3> *mTrans;
  Object_Layout *mObj;
  int paneIndex;
};


//LayoutObjectPane移動
void TaskFunc_ObjectMove_LayoutPane(Task *task, void *userWork,int step)
{
  TaskWork_LayoutMoveCommon *work = static_cast<TaskWork_LayoutMoveCommon*>(userWork);

  // Frame Skip:
  for(;step>1;step--) work->mTrans->Update();

  const gfl2::math::Vector3 pos = work->mTrans->Update();
  work->mObj->SetPanePosition(pos,work->paneIndex);
  if( work->mTrans->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTrans;
  }
}

void CreateTask_ObjectMove_LayoutPane( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Layout *obj,int paneIndex ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type )
{
  Task *task = taskSys->Create(TaskFunc_ObjectMove_LayoutPane,sizeof(TaskWork_LayoutMoveCommon),TASK_PRI_LOWEST);
  TaskWork_LayoutMoveCommon *work = static_cast<TaskWork_LayoutMoveCommon*>(task->GetWork());

  work->mObj = obj;
  work->paneIndex = paneIndex;
  work->mTrans = TaskUtil_CreateTransBase(heap,obj,start,end,step,type);
}

//LayoutObjectPaneスケール
void TaskFunc_ObjectScale_LayoutPane(Task *task, void *userWork,int step)
{
  TaskWork_LayoutMoveCommon *work = static_cast<TaskWork_LayoutMoveCommon*>(userWork);

  // Frame Skip:
  for(;step>1;step--) work->mTrans->Update();

  const gfl2::math::Vector3 scale = work->mTrans->Update();
  work->mObj->SetPaneScale(scale,work->paneIndex);
  if( work->mTrans->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTrans;
  }
}
void CreateTask_ObjectScale_LayoutPane( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Layout *obj,int paneIndex ,const gfl2::math::Vector3 &start ,const gfl2::math::Vector3 &end ,const u32 step , const TASK_MOVE_TYPE type)
{
  Task *task = taskSys->Create(TaskFunc_ObjectScale_LayoutPane,sizeof(TaskWork_LayoutMoveCommon),TASK_PRI_LOWEST);
  TaskWork_LayoutMoveCommon *work = static_cast<TaskWork_LayoutMoveCommon*>(task->GetWork());

  work->mObj = obj;
  work->paneIndex = paneIndex;
  work->mTrans = TaskUtil_CreateTransBase(heap,obj,start,end,step,type);
}

//LayoutObjectPaneスケール
void TaskFunc_ObjectAlpha_LayoutPane(Task *task, void *userWork,int step)
{
  TaskWork_LayoutMoveCommon *work = static_cast<TaskWork_LayoutMoveCommon*>(userWork);

  // Frame Skip:
  for(;step>1;step--) work->mTrans->Update();

  const gfl2::math::Vector3 v = work->mTrans->Update();

  u8 alpha = (u8)v.x;

  work->mObj->SetPaneAlpha(alpha,work->paneIndex);
  if( work->mTrans->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTrans;
  }
}
void CreateTask_ObjectAlpha_LayoutPane( TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Layout *obj,int paneIndex ,const u8 startAlpha ,const u8 endAlpha ,const u32 step , const TASK_MOVE_TYPE type)
{
  Task *task = taskSys->Create(TaskFunc_ObjectAlpha_LayoutPane,sizeof(TaskWork_LayoutMoveCommon),TASK_PRI_LOWEST);
  TaskWork_LayoutMoveCommon *work = static_cast<TaskWork_LayoutMoveCommon*>(task->GetWork());

  gfl2::math::Vector3 start;
  start.x = (f32)startAlpha;
  start.y = start.z = 0.0f;

  gfl2::math::Vector3 end;
  end.x = (f32)endAlpha;
  end.y = end.z = 0.0f;

  work->mObj = obj;
  work->paneIndex = paneIndex;
  work->mTrans = TaskUtil_CreateTransBase(heap,obj,start,end,step,type);
}

//----------------------------------------------------------------------
//  Object_Poke用関数
//----------------------------------------------------------------------
//モデル自動削除(1フレ遅れ対応版)
struct TaskWork_DeleteObject_Poke
{
  Object_Poke *mObj;
  int mFrame;
};

void TaskFunc_DeleteObject_Poke(Task *task, void *userWork,int step)
{
  TaskWork_DeleteObject_Poke *work = static_cast<TaskWork_DeleteObject_Poke*>(userWork);

  // DO NOT PROCESS DURING SYSTEM SYNC:  
  if(task->GetSystem()->IsSyncMode()) step = 0;

  work->mFrame-=step;

  if( work->mFrame <= 0 )
  {
    if( work->mObj->IsAddRenderPath() )
    {      
      work->mObj->RemoveRenderPath();      
    }
    else
    {      
      work->mObj->Delete();
      task->GetSystem()->Delete(task);      
    }
  }
}

void CreateTask_DeleteObject_Poke(TaskSystem *taskSys , gfl2::heap::HeapBase *haep, Object_Poke *obj , const int frame)
{
  Task *task = taskSys->Create(TaskFunc_DeleteObject_Poke,sizeof(TaskWork_DeleteObject_Poke),TASK_PRI_LOWEST);
  TaskWork_DeleteObject_Poke *work = static_cast<TaskWork_DeleteObject_Poke*>(task->GetWork());

  work->mObj = obj;
  work->mFrame = frame;
}

//カラーシェーダー操作
struct TaskWork_ChangePokeColorShader
{
	Object_Poke *mObj;
  gfl2::math::TransBase<gfl2::math::Vector4> *mTrans;
};

void TaskFunc_ChangePokeColorShader(Task *task, void *userWork,int step)
{
	TaskWork_ChangePokeColorShader *work = static_cast<TaskWork_ChangePokeColorShader*>(userWork);

  // Frame Skip:
  for(;step>1;step--) work->mTrans->Update();

  const gfl2::math::Vector4 col = work->mTrans->Update();
  //GFL_PRINT("[%f][%f][%f][%f]\n",col.x,col.y,col.z,col.w);

  // [fix]GFNMcat[1886]：【カラーシェーダーの最後の１フレが実行されないときがある】の修正
  // バトルの処理落ち時の色の設定は, 処理落ちを考慮して, 3フレーム実行するように設定
  static const s32 s_BattleColorShaderExecuteFrame = 3;
  work->mObj->SetShaderColor(col, s_BattleColorShaderExecuteFrame);

  if( work->mTrans->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTrans;
  }
}
void CreateTask_ChangePokeColorShader(TaskSystem *taskSys , gfl2::heap::HeapBase *heap, Object_Poke *obj , const gfl2::math::Vector4 &start ,const gfl2::math::Vector4 &end , const int frame)
{
	Task *task = taskSys->Create(TaskFunc_ChangePokeColorShader,sizeof(TaskWork_ChangePokeColorShader),TASK_PRI_DEFAULT);
	TaskWork_ChangePokeColorShader *work = static_cast<TaskWork_ChangePokeColorShader*>(task->GetWork());

	work->mObj = obj;
  work->mTrans = GFL_NEW(heap) gfl2::math::TransVec4<gfl2::math::TransLiner<f32> >(start,end,frame,heap);

}

//----------------------------------------------------------------------
//  Object_Light用関数
//----------------------------------------------------------------------
//モデル自動削除(1フレ遅れ対応版)
struct TaskWork_DeleteObject_Light
{
  Object_Light *mObj;
  int mFrame;
};

void TaskFunc_DeleteObject_Light(Task *task, void *userWork,int step)
{
  TaskWork_DeleteObject_Light *work = static_cast<TaskWork_DeleteObject_Light*>(userWork);

  // DO NOT PROCESS DURING SYSTEM SYNC:  
  if(task->GetSystem()->IsSyncMode()) step = 0;

  work->mFrame-=step;

  if( work->mFrame <= 0 )
  {
    if( work->mObj->IsAddRenderPath() )
    {
      work->mObj->RemoveRenderPath();      
      GFL_PRINT("Task->Removing Light from RenderPath\n");
    }
    else
    {
      work->mObj->Delete();
      task->GetSystem()->Delete(task);      
      GFL_PRINT("Task->Deleting Light Object");
    }
  }
}

void CreateTask_DeleteObject_Light(TaskSystem *taskSys , gfl2::heap::HeapBase *haep, Object_Light *obj , const int frame)
{
  Task *task = taskSys->Create(TaskFunc_DeleteObject_Light,sizeof(TaskWork_DeleteObject_Light),TASK_PRI_LOWEST);
  TaskWork_DeleteObject_Light *work = static_cast<TaskWork_DeleteObject_Light*>(task->GetWork());

  work->mObj = obj;
  work->mFrame = frame;
}

//----------------------------------------------------------------------
//  Soundパラメータ操作
//----------------------------------------------------------------------
//SE操作タスク
struct TaskWork_SeCtrl
{
  int mSeIndex;
  int mGrpNo;
  int mType;
  gfl2::math::TransLinerF32 *mTrans;
};
void TaskFunc_SeCtrl(Task *task, void *userWork,int step)
{
  TaskWork_SeCtrl *work = static_cast<TaskWork_SeCtrl*>(task->GetWork());

  // Frame Skip:
  for(;step>1;step--) work->mTrans->Update();

  const f32 val = work->mTrans->Update();
  
  switch( work->mType )
  {
  case SE_CTRL_VOLUME:
    Sound::ChangeSEVolume( work->mSeIndex , val , 0 , work->mGrpNo );
    break;
  case SE_CTRL_PAN:
    Sound::ChangeSEPan( work->mSeIndex , val , work->mGrpNo );
    break;
  case SE_CTRL_PITCH:
    Sound::ChangeSEPitch( work->mSeIndex , val , work->mGrpNo );
    break;
  case SE_CTRL_TEMPO:
    Sound::ChangeSETempo( work->mSeIndex , val , work->mGrpNo );
    break;
  }
  
  if( work->mTrans->IsEnd() )
  {
    GFL_DELETE work->mTrans;
    task->GetSystem()->Delete(task);      
  }
}

void CreateTask_SeCtrl( TaskSystem *taskSys , gfl2::heap::HeapBase *haep, f32 start , f32 end , int frame , int seIdx , int grpNo , int type )
{
  Task *task = taskSys->Create(TaskFunc_SeCtrl,sizeof(TaskWork_SeCtrl),TASK_PRI_LOWEST);
  TaskWork_SeCtrl *work = static_cast<TaskWork_SeCtrl*>(task->GetWork());

  work->mSeIndex = seIdx;
  work->mGrpNo = grpNo;
  work->mType = type;
  work->mTrans = GFL_NEW(haep) gfl2::math::TransLinerF32(start,end,frame);

}


GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
