//======================================================================
/**
 * @file	Object_Poke.cpp
 * @brief	オブジェクト PokeModel
 * @author  ariizumi
 * @data  15/05/16
 */
//======================================================================

#include <math/include/gfl2_math.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include <DemoLib/Object/include/Object_Poke.h>
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>



GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

Object_Poke::Object_Poke(ObjectRenderPipeLine *pipe)
:Object()
,mRenderingPipeLine(pipe)
{
  mIsAddRenderPath = false;
  mReserveAnime = -1;
  mSlotAnimationLoaded[0] = false;
  mSlotAnimationLoaded[1] = false;
  mIsEgg = false;
}

Object_Poke::~Object_Poke()
{
}

void Object_Poke::UpdateAnimation(int step,int slot)
{
  GFL_PRINT("*Skip AniFrame:%i\n",step);

  if(slot==0 && mModel->GetAnimationEndFrame()<=step+1) 
  {
    if( mReserveAnime != -1 )
    {     
       ChangeAnimationDirect(mReserveAnime,true);
       SetAnimationIsLoop(true);
       mReserveAnime = -1;      
    }
    mModel->UpdateAnimation();
    return;
  }

  UpdateAlways(step);  
  mModel->UpdateAnimation();
}

void Object_Poke::UpdateAlways(int step)
{
  mModel->SetVisible(mIsVisible);

  if(step!=1)
  {
    {    
      f32 curFrame = mModel->GetAnimationFrame();
      f32 frame = curFrame + (step-1);
      if(frame>curFrame || !mModel->IsAnimationLoop()) mModel->SetAnimationFrame(frame);    
    }

    if(mSlotAnimationLoaded[1])
    {
      f32 curFrame = mModel->GetAnimationFrame(1);
      f32 frame = curFrame + (step-1);
      if(frame>curFrame  || !mModel->IsAnimationLoop(1)) mModel->SetAnimationFrame(frame,1);    
    }
  }

  if( mReserveAnime != -1 )
  {
    if( IsFinishAnimation() )
    {
      ChangeAnimationDirect(mReserveAnime);
      SetAnimationIsLoop(true);
      mReserveAnime = -1;
    }
  }
}

void Object_Poke::UpdatePosition(void)
{
  gfl2::math::Vector3 pos;
  gfl2::math::Vector3 scale;
  gfl2::math::Vector3 rotDeg;
  GetCalcPosition(&pos);
  GetCalcRotate(&rotDeg);
  GetCalcScale(&scale);
  gfl2::math::Vector3 rot(gfl2::math::ConvDegToRad(rotDeg.x),gfl2::math::ConvDegToRad(rotDeg.y),gfl2::math::ConvDegToRad(rotDeg.z));

  mModel->SetPosition(pos);
  mModel->SetRotation(rot,GetRotationOrder());
  mModel->SetScale(scale);

  mModel->Update();
}

void Object_Poke::UpdateCore(int step)
{
  gfl2::math::Vector3 pos;
  gfl2::math::Vector3 scale;
  gfl2::math::Vector3 rotDeg;
  GetCalcPosition(&pos);
  GetCalcRotate(&rotDeg);
  GetCalcScale(&scale);
  gfl2::math::Vector3 rot(gfl2::math::ConvDegToRad(rotDeg.x),gfl2::math::ConvDegToRad(rotDeg.y),gfl2::math::ConvDegToRad(rotDeg.z));

  mModel->SetMegaGangarEffectState(pos.y);

  mModel->SetPosition(pos);
  mModel->SetRotation(rot,GetRotationOrder());
  mModel->SetScale(scale);

}

//モデル生成
void Object_Poke::StartLoad(gfl2::fs::AsyncFileManager *fileMng, const LOAD_PARAM &param)
{
  mModelSys = param.modelSys;
  mModel = mModelSys->CreateModelSimple(param.param,*param.opt);
  mIsEgg = param.param->isEgg;
}


bool Object_Poke::IsFinishLoad(void)
{
  return mModel->IsFinishLoad();
}
void Object_Poke::Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator,bool useShader)
{
  ResetParam();
  SetVisible(true);
  SetIsEnable(true);
  mModel->Create();
  if( useShader )
  {
    mModel->SetupColorShader();
  }
}


void Object_Poke::ForceDelete(void)
{
  SetIsEnable(true);
  Delete();
}

void Object_Poke::Delete(void)
{
  if( IsEnable() )
  {
    SetIsEnable(false);
    mModelSys->DeleteModel(mModel);
    mModel = NULL;
  }
}

//ObjectRenderPipeLine経由で追加する
void Object_Poke::AddRenderPath(int opt)
{
  if( IsEnable() && IsAddRenderPath() == false )
  {
    mSceneOpt = opt;  //自動削除タスクのため覚えておく
    mRenderingPipeLine->AddObject_Poke(mModel,opt);
    mIsAddRenderPath = true;
  }
}

void Object_Poke::RemoveRenderPath(void)
{
  if( IsEnable() && IsAddRenderPath() )
  {
    mModel->CleanColorShader();
    mRenderingPipeLine->RemoveObject_Poke(mModel,mSceneOpt);
    mIsAddRenderPath = false;
  }
}

//継承
void Object_Poke::SetVisible(const bool flg)
{
  if( IsEnable() )
  {
    mModel->SetVisible(flg);
    Object::SetVisible(flg);
  }
}

void Object_Poke::SetShadowVisible(const bool flg)
{
  if( IsEnable() )
  {
    if( mModel->GetShadowModel() )
    {
      mModel->GetShadowModel()->SetVisible(flg);
    }
  }
}

/**
 * @brief カラーシェーダの色の設定
 * @param color カラーシェーダで適用する色
 * @param executeFrame 実行するフレーム
 * @note executeFrameは基本1. 処理落ち発生の際に, 確実に実行してほしいフレーム数を指定する
 */
void Object_Poke::SetShaderColor(const gfl2::math::Vector4& col, s32 executeFrame)
{
  if(mModel==NULL) return;
  mModel->SetShaderColor(col, executeFrame);
}

const gfl2::math::Vector4 Object_Poke::GetShaderColor(void)const
{
  return mModel->GetShaderColor();
}

//アニメーション関係
// @value forceReset モーション番号が同じだった場合リセットをかけるか？
void Object_Poke::ChangeAnimation( PokeTool::MODEL_ANIME type , bool forceReset )
{
  ChangeAnimationDirect(type,forceReset);
}

void Object_Poke::ChangeAnimationDirect( int type , bool forceReset )
{
  if( IsEnable() )
  {
    mReserveAnime = -1;
    mSlotAnimationLoaded[0] = false;
    mModel->ChangeAnimationDirect(type,forceReset);
  }
}

PokeTool::MODEL_ANIME Object_Poke::GetAnimationType(void)
{
  return mModel->GetAnimationType();
}

f32 Object_Poke::GetAnimationMaxFrame(void)
{
  return mModel->GetAnimationEndFrame();
}

void Object_Poke::SetAnimationFrame(f32 frame)
{
  mModel->SetAnimationFrame(frame);
}

bool Object_Poke::IsFinishAnimation(void)
{
  return mModel->IsAnimationLastFrame();
}

bool Object_Poke::IsAvailableAnimation( PokeTool::MODEL_ANIME type )
{
  return mModel->IsAvailableAnimation(type);
}
f32 Object_Poke::GetAnimationSpeed(void)
{
  return mModel->GetAnimationStepFrame();
}

void Object_Poke::SetAnimationSpeed(f32 spd)
{
  mModel->SetAnimationStepFrame(spd);
}


//リソース渡し再生(全力技・デモ等外部でモーションを持つ場合)
//戻す際は普通にChangeAnimationしてください。
void Object_Poke::ChangeAnimationResource( gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes,int aniSlot )
{
  if( IsEnable() )
  {
    GFL_ASSERT_MSG(aniSlot<2,"Slot index is Over!!\n");
    mModel->ChangeAnimationResource(anmRes,aniSlot);
    if(anmRes!=NULL)
    {
      if( aniSlot == 0 )
      {
        mReserveAnime = -1;
      }
      mSlotAnimationLoaded[aniSlot] = true;
    }
    else
    {
      mSlotAnimationLoaded[aniSlot] = false;
    }
  }
}
bool Object_Poke::IsPlayAnimationResource(int aniSlot )
{
  return mSlotAnimationLoaded[aniSlot];
}


void Object_Poke::SetAnimationIsLoop( bool flg,int aniSlot )
{
  mModel->SetAnimationLoop(flg,aniSlot);
}

bool Object_Poke::GetNodePosition( PokeTool::MODEL_NODE node , int idx , gfl2::math::Vector3 *ret)
{
  if(mModel==NULL) return(false); // NULL Check!
  return mModel->GetEffectNodePosition(node,idx,ret);
}

//追従処理用
void Object_Poke::UpdateAnimationForce(void)
{
  //単体で更新してしまう
  //gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFastFromChild(GetPokeModel()->GetModelInstanceNode());
  //@attencion TraverseModelFastFromChildだと、親のnode自体は更新されないのでSetPositionの座標が反映されない
  //専用関数用意予定→見送り
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();
}

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
