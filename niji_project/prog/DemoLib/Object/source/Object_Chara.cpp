//======================================================================
/**
 * @file	Object_Chara.cpp
 * @brief	オブジェクト GfMdl
 * @author  ariizumi
 * @data  15/03/16
 */
//======================================================================

#include <math/include/gfl2_math.h>
#include <str/include/gfl2_Str.h> //ノード名比較
#include <util/include/gfl2_std_string.h>

#include <model/include/gfl2_BaseModel.h>
#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_DressUpModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include <DemoLib/Object/include/Object_Chara.h>
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

//Obj内のみで有効なID
#define INVALID_CHARAID (0xFFFFFFFF)

Object_Chara::Object_Chara(ObjectRenderPipeLine *pipe,poke_3d::model::CharaModelFactory *charaFactory,poke_3d::model::DressUpModelResourceManager *dressUpResourceManager)
:Object_GfMdl(pipe)
,mCharaFactory(charaFactory)
,mDressUpModelResourceManager(dressUpResourceManager)
,mCharaId(INVALID_CHARAID)
,mDressParam(NULL)
{
  mSceneOpt = 0;
  mFollowPoke = NULL;
  mFollowObject = NULL;
  mIsBattleModel = false;

  for(int z=0;z<MAX_DRESSUP_ANISLOT;z++)
  {
    anmResNodeDressUp[z]=NULL;
  }
}

Object_Chara::~Object_Chara()
{
}

bool Object_Chara::IsMale(void)
{
  if(IsBattleModel()) return(IsMale_Battle());
  return(IsMale_Field());
}

bool Object_Chara::IsFemale(void)
{
  if(IsBattleModel()) return(IsFemale_Battle());
  return(IsFemale_Field());
}

void Object_Chara::UpdateAlways(int step)
{
  Object_GfMdl::UpdateAlways(step);
  if( mReserveAnime != -1 )
  {
    if( mModel->IsAnimationLastFrame() )
    {
      ChangeAnimation(mReserveAnime);
      SetAnimationIsLoop(true);
      mReserveAnime = -1;
    }
  }
}

//こっちはUpdateCoreの後に呼ばれる。座標反映後の更新はこちらで。
void Object_Chara::UpdateAlwaysAfterCore(int step)
{
  Object_GfMdl::UpdateAlwaysAfterCore(step);
  if( mAttachModel )
  {
    mAttachModel->UpdateAnimation();
  }
}

//モデル生成
void Object_Chara::StartLoad(const LOAD_PARAM &param)
{
  mCharaId = param.charaId;
  mDressParam = param.dressParam;
  mCharaFactory->LoadModelAsync(mCharaId);
  if( mDressParam )
  {
    mDressUpModelResourceManager->LoadDressUpPartsAsync( *mDressParam );
  }
  if( param.attachModelId != INVALID_MODEL_ID )
  {
    mAttachModelId = param.attachModelId;
    gfl2::std::MemCopy(param.attachJoint,mAttachJoint,32);
    mCharaFactory->LoadModelAsync(mAttachModelId);
  }
  else
  {
    mAttachModelId = INVALID_MODEL_ID;
  }
}

bool Object_Chara::IsFinishLoad(void)
{
  if( mCharaFactory->IsModelLoaded( mCharaId ) == false )
  {
    return false;
  }
  if( mDressParam &&
      mDressUpModelResourceManager->IsDressUpPartsLoaded( *mDressParam ) == false )
  {
    return false;
  }
  if( mAttachModelId != INVALID_MODEL_ID )
  {
    if( mCharaFactory->IsModelLoaded( mAttachModelId ) == false )
    {
      return false;
    }
  }
  return true;
}

void Object_Chara::Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator)
{
  mCharaFactory->SetupModel(allocator,mCharaId);
  if( mDressParam )
  {
    mDressUpModelResourceManager->SetupDressUpParts( allocator , *mDressParam  );
  }
  if( mAttachModelId != INVALID_MODEL_ID )
  {
    mCharaFactory->SetupModel(allocator,mAttachModelId);
  }

  LOAD_PARAM param;
  param.charaId = mCharaId;
  param.dressParam = mDressParam;
  param.attachModelId = mAttachModelId;
  gfl2::std::MemCopy(mAttachJoint,param.attachJoint,32);
  CreateNoLoad(param , heap , allocator );
  mReserveAnime = -1;
  mIsLoadData = true;
}

void Object_Chara::CreateNoLoad(const LOAD_PARAM &param,gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator)
{
  mCharaId = param.charaId;
  mDressParam = param.dressParam;

  if( mDressParam )
  {
    mDressModel = GFL_NEW(heap)poke_3d::model::DressUpModel();
    mCharaModel = mDressModel;
    mCharaFactory->CreateDressUpModel(allocator,heap,mDressModel,mDressUpModelResourceManager,mCharaId);
    mModel = mDressModel;
  }
  else
  {
    mDressModel = NULL;
    mCharaModel = GFL_NEW(heap)poke_3d::model::CharaModel();
    mCharaFactory->CreateModel(allocator,heap,mCharaModel,mCharaId);
    mModel = mCharaModel;
  }

  if( param.attachModelId != INVALID_MODEL_ID )
  {
    mAttachModelId = param.attachModelId;
    gfl2::std::MemCopy(param.attachJoint,mAttachJoint,32);

    mAttachModel = GFL_NEW(heap)poke_3d::model::CharaModel();
    mCharaFactory->CreateModel(allocator,heap,mAttachModel,mAttachModelId);

    s32 jointIndex = GetCharaModel()->GetJointIndex(mAttachJoint);

    GetCharaModel()->GetModelInstanceNode()->GetJointInstanceNode( jointIndex )->AddChild( mAttachModel->GetModelInstanceNode() );
  }
  else
  {
    mAttachModel = NULL;
  }
  
  SetIsEnable(true);
  SetVisible(true);
  mReserveAnime = -1;
  mIsLoadData = false;
}


void Object_Chara::FreeDressUpAnimationNodes(void)
{
  for(int z=0;z<MAX_DRESSUP_ANISLOT;z++)
  {
    GFL_SAFE_DELETE(anmResNodeDressUp[z]);
  }
}

void Object_Chara::Delete(void)
{
  if( IsEnable() )
  {
    SetIsEnable(false);
    
    if( mAttachModel )
    {
      mAttachModel->Destroy();
      GFL_SAFE_DELETE(mAttachModel);
    }

    mModel->Destroy();
    GFL_SAFE_DELETE(mModel);
    FreeDressUpAnimationNodes();
    mDressModel = NULL;
    mModel = NULL;

    if( mIsLoadData )
    {
      if( mAttachModelId != INVALID_MODEL_ID )
      {
        mCharaFactory->UnloadModel(mAttachModelId);
      }
      if( mDressParam )
      {
        mDressUpModelResourceManager->UnloadDressUpParts(*mDressParam);
      }
      mCharaFactory->UnloadModel(mCharaId);
    }
    mDressParam = NULL;
    mCharaId = INVALID_CHARAID;
  }
}

//ObjectRenderPipeLine経由で追加する
void Object_Chara::AddRenderPath(int opt)
{
  if( IsEnable() && IsAddRenderPath() == false )
  {
    mSceneOpt = opt;  //自動削除タスクのため覚えておく
    if( mDressModel )
    {
      mRenderingPipeLine->AddObject_DressUpChara(mDressModel,mSceneOpt);
    }
    else
    {
      mRenderingPipeLine->AddObject_Chara(mCharaModel,mSceneOpt);
    }
    if( mAttachModel )
    {
      mRenderingPipeLine->AddObject_Chara(mAttachModel,mSceneOpt);
    }
    mIsAddRenderPath = true;
  }
}

void Object_Chara::RemoveRenderPath(void)
{
  if( IsEnable() && IsAddRenderPath() )
  {
    if( mDressModel )
    {
      mRenderingPipeLine->RemoveObject_DressUpChara(mDressModel,mSceneOpt);
    }
    else
    {
      mRenderingPipeLine->RemoveObject_Chara(mCharaModel,mSceneOpt);
    }
    if( mAttachModel )
    {
      mRenderingPipeLine->RemoveObject_Chara(mAttachModel,mSceneOpt);
    }
    mIsAddRenderPath = false;
  }
}

//継承
void Object_Chara::SetVisible(const bool flg)
{
  Object_GfMdl::SetVisible(flg);
  if( mAttachModel )
  {
    mAttachModel->SetVisible(flg);
  }
}

void Object_Chara::ChangeAnimation( u32 animeIdx )
{
  if( IsEnable() )
  {
    mAnimeIdx = animeIdx;
    mModel->ChangeAnimation(animeIdx);
    if( mAttachModel )
    {
      mAttachModel->ChangeAnimation(animeIdx);
    }
  }
}

void Object_Chara::ChangeAnimationResource( gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes,int aniSlot )
{
  if( IsEnable() )
  {
    mModel->ChangeAnimationByResourceNode(anmRes,aniSlot);
  }
}

void Object_Chara::ChangeAnimationResourceDressUp( void *buf, gfl2::gfx::IGLAllocator *allocator  )
{
  if( IsEnable() && mDressModel)
  {    
    if(IsBattleModel()) ChangeAnimationResourceDressUp_Battle(buf,allocator);
    else ChangeAnimationResourceDressUp_Field(buf,allocator);  
  }
}

void Object_Chara::ClearDressUpAnimation(void)
{
  mDressModel->ChangeAnimationByResourceNode(NULL,0);

  if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_FACE)!=NULL)
  {
    if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_FACE)->GetAnimationController() != NULL)
    {
      mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_FACE)->ChangeAnimationByResourceNode(NULL);
    }
  }

  if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAIR)!=NULL)
  {
    if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAIR)->GetAnimationController() != NULL)
    {
      mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAIR)->ChangeAnimationByResourceNode(NULL);
    }
  }

  if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_TOPS)!=NULL)
  {
    if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_TOPS)->GetAnimationController() != NULL)
    {
      mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_TOPS)->ChangeAnimationByResourceNode(NULL);
    }
  }

  if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_BOTTOMS)!=NULL)
  {
    if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_BOTTOMS)->GetAnimationController() != NULL)
    {
      mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_BOTTOMS)->ChangeAnimationByResourceNode(NULL);
    }
  }

  if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAT)!=NULL)
  {
    if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAT)->GetAnimationController() != NULL)
    {
      mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAT)->ChangeAnimationByResourceNode(NULL);
    }
  }
}

void Object_Chara::SetAnimationSpeed(f32 speed,int aniSlot )
{
  Object_GfMdl::SetAnimationSpeed(speed,aniSlot);
  if( mAttachModel )
  {
    mAttachModel->SetAnimationStepFrame(speed,aniSlot);
  }
}

void Object_Chara::SetConstantColor( int contantColorIndex , const gfl2::math::Vector4 &col)
{
  if( IsEnable() )
  {
    {
      int matNum = GetCharaModel()->GetMaterialCount();
      for( int mat = 0 ; mat<matNum ; mat++ )
      {
        GetCharaModel()->SetMaterialColorConstant(mat,contantColorIndex,col);
      }
    }

    if( GetDressUpModel() )
    {
      for( int i=0;i<poke_3d::model::DressUpModel::PARTS_MODEL_COUNT;i++)
      {
        const poke_3d::model::DressUpModel::PartsModel type = static_cast<poke_3d::model::DressUpModel::PartsModel>(i);
        if( GetDressUpModel()->GetPartsModel(type) )
        {
          int matNumDress = GetDressUpModel()->GetPartsModel(type)->GetMaterialCount();
          for( int mat = 0 ; mat<matNumDress ; mat++ )
          {
            GetDressUpModel()->GetPartsModel(type)->SetMaterialColorConstant(mat,contantColorIndex,col);
          }
        }
      }
    }
  }
}


GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
