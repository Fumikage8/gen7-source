//======================================================================
/**
 * @file	Object_Chara_Battle.cpp
 * @brief	Chara->Battle
 * @author  Pete
 * @data  15/09/03
 */
//======================================================================

#include <math/include/gfl2_math.h>
#include <str/include/gfl2_Str.h> //ノード名比較

#include <model/include/gfl2_BaseModel.h>
#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_DressUpModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include <DemoLib/Object/include/Object_Chara.h>
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>

#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <System/include/DressUp.h>

#include <fs/include/gfl2_BinLinkerAccessor.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

bool Object_Chara::IsMale_Battle(void)
{
  if(mCharaId==CHARA_MODEL_ID_P1_BASE) return(true);
  return(false);
}

bool Object_Chara::IsFemale_Battle(void)
{
  if(mCharaId==CHARA_MODEL_ID_P2_BASE) return(true);
  return(false);
}

int Object_Chara::DressUpBattle_HairType(void)
{
  int hairType = mDressParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE];

  if(IsMale())
  {
    // Medium Only Check:
    if(hairType>=DRESSUP_ITEM_ID_P1_HAIR_MIDIUM_START && hairType<=DRESSUP_ITEM_ID_P1_HAIR_MIDIUM_END) return(DRESSUP_ANI_MALE_HAIR_MEDIUM);

    // No Male Hair Animation:
    return(DRESSUP_ANI_NONE);    
  }

  // Must be FEMALE:
  if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_SHORT_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_SHORT_END) return(DRESSUP_ANI_FEMALE_HAIR_SHORT);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_SHORT_MAE_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_SHORT_MAE_END) return(DRESSUP_ANI_FEMALE_HAIR_SHORT);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_SHORT_PATTUN_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_SHORT_PATTUN_END) return(DRESSUP_ANI_FEMALE_HAIR_SHORT);

  // MEDIUM:
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_MIDIUM_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_MIDIUM_END) return(DRESSUP_ANI_FEMALE_HAIR_MEDIUM);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_MIDIUM_MAE_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_MIDIUM_MAE_END) return(DRESSUP_ANI_FEMALE_HAIR_MEDIUM);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_MIDIUM_PATTUN_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_MIDIUM_PATTUN_END) return(DRESSUP_ANI_FEMALE_HAIR_MEDIUM);
  
  // BOB:
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_BOB_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_BOB_END) return(DRESSUP_ANI_FEMALE_HAIR_BOB);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_BOB_MAE_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_BOB_MAE_END) return(DRESSUP_ANI_FEMALE_HAIR_BOB);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_BOB_PATTUN_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_BOB_PATTUN_END) return(DRESSUP_ANI_FEMALE_HAIR_BOB);
  

  // TWIN:
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_TWIN_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_TWIN_END) return(DRESSUP_ANI_FEMALE_HAIR_TWIN);  
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_TWIN_MAE_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_TWIN_MAE_END) return(DRESSUP_ANI_FEMALE_HAIR_TWIN);  
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_TWIN_PATTUN_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_TWIN_PATTUN_END) return(DRESSUP_ANI_FEMALE_HAIR_TWIN);  

  // LONG:
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_LONG_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_LONG_END) return(DRESSUP_ANI_FEMALE_HAIR_LONG);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_LONG_MAE_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_LONG_MAE_END) return(DRESSUP_ANI_FEMALE_HAIR_LONG);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_LONG_PATTUN_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_LONG_PATTUN_END) return(DRESSUP_ANI_FEMALE_HAIR_LONG);

  // MAKI:
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_MAKI_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_MAKI_END) return(DRESSUP_ANI_FEMALE_HAIR_MAKI);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_MAKI_MAE_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_MAKI_MAE_END) return(DRESSUP_ANI_FEMALE_HAIR_MAKI);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_MAKI_PATTUN_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_MAKI_PATTUN_END) return(DRESSUP_ANI_FEMALE_HAIR_MAKI);

  // MATOME:
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_MATOME_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_MATOME_END) return(DRESSUP_ANI_FEMALE_HAIR_MATOME);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_MATOME_MAE_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_MATOME_MAE_END) return(DRESSUP_ANI_FEMALE_HAIR_MATOME);
  else if(hairType>=DRESSUP_ITEM_ID_P2_HAIR_MATOME_PATTUN_START && hairType<=DRESSUP_ITEM_ID_P2_HAIR_MATOME_PATTUN_END) return(DRESSUP_ANI_FEMALE_HAIR_MATOME);

  //momiji追加
  // SPONY:
  else if (hairType >= DRESSUP_ITEM_ID_P2_HAIR_MMJ_SPONY_START && hairType <= DRESSUP_ITEM_ID_P2_HAIR_MMJ_SPONY_END) return(DRESSUP_ANI_FEMALE_HAIR_MMJ_SPONY);
  else if (hairType >= DRESSUP_ITEM_ID_P2_HAIR_MMJ_SPONY_MAE_START && hairType <= DRESSUP_ITEM_ID_P2_HAIR_MMJ_SPONY_MAE_END) return(DRESSUP_ANI_FEMALE_HAIR_MMJ_SPONY);
  else if (hairType >= DRESSUP_ITEM_ID_P2_HAIR_MMJ_SPONY_PATTUN_START && hairType <= DRESSUP_ITEM_ID_P2_HAIR_MMJ_SPONY_PATTUN_END) return(DRESSUP_ANI_FEMALE_HAIR_MMJ_SPONY);



  return(DRESSUP_ANI_NONE);
}

int Object_Chara::DressUpBattle_TopType(void)
{
  int topType = mDressParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS];

  if(IsMale())
  {   
    // No Male Top Animation:
    return(DRESSUP_ANI_NONE);    
  }

  // FEMALE:
  if(topType>=DRESSUP_ITEM_ID_P2_TOPS_DCUT01_START && topType<=DRESSUP_ITEM_ID_P2_TOPS_DCUT01_END) return(DRESSUP_ANI_FEMALE_TOPS_DCUT);

  // No animations for now:
  return(DRESSUP_ANI_NONE);
}

int Object_Chara::DressUpBattle_BottomType(void)
{
  int bottomType = mDressParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS];

  if(IsMale())
  {   
    // No Male Bottom Animation:
    return(DRESSUP_ANI_NONE);    
  }

  // Must be FEMALE:
  if(bottomType>=DRESSUP_ITEM_ID_P2_BOTTOMS_FLARE_FLARE01_START && bottomType<=DRESSUP_ITEM_ID_P2_BOTTOMS_FLARE_FLARE01_END) return(DRESSUP_ANI_FEMALE_BOTTOMS_FLARE);
  
  if(bottomType>=DRESSUP_ITEM_ID_P2_BOTTOMS_PLEAT01_START && bottomType<=DRESSUP_ITEM_ID_P2_BOTTOMS_PLEAT01_END) return(DRESSUP_ANI_FEMALE_BOTTOMS_FLARE);
  
  if(bottomType>=DRESSUP_ITEM_ID_P2_BOTTOMS_CPLEAT01_START && bottomType<=DRESSUP_ITEM_ID_P2_BOTTOMS_CPLEAT01_END) return(DRESSUP_ANI_FEMALE_BOTTOMS_FLARE);
  
  if(bottomType>=DRESSUP_ITEM_ID_P2_BOTTOMS_FLARE_HFLARE01_START && bottomType<=DRESSUP_ITEM_ID_P2_BOTTOMS_FLARE_HFLARE01_END) return(DRESSUP_ANI_FEMALE_BOTTOMS_FLARE);
  
  //momiji追加分
  if (bottomType >= DRESSUP_ITEM_ID_P2_BOTTOMS_MMJ_IDOL01_START && bottomType <= DRESSUP_ITEM_ID_P2_BOTTOMS_MMJ_IDOL01_END) return(DRESSUP_ANI_FEMALE_BOTTOMS_FLARE);

  if (bottomType >= DRESSUP_ITEM_ID_P2_BOTTOMS_MMJ_PC01_START && bottomType <= DRESSUP_ITEM_ID_P2_BOTTOMS_MMJ_PC01_END) return(DRESSUP_ANI_FEMALE_BOTTOMS_FLARE);


  return(DRESSUP_ANI_NONE);
}

int Object_Chara::DressUpBattle_HatType(void)
{
  int hatType = mDressParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT];

  if(IsMale())
  {   
    // No Male Hat Animation:
    return(DRESSUP_ANI_NONE);    
  }

  // Must be FEMALE:
  if(hatType>=DRESSUP_ITEM_ID_P2_HAT_DHAT01_START && hatType<=DRESSUP_ITEM_ID_P2_HAT_DHAT01_END) return(DRESSUP_ANI_FEMALE_HAT_DHAT);
  else if(hatType>=DRESSUP_ITEM_ID_P2_HAT_RESORT01_START && hatType<=DRESSUP_ITEM_ID_P2_HAT_RESORT01_END) return(DRESSUP_ANI_FEMALE_HAT_RESORT);

  //momiji追加
  if (hatType >= DRESSUP_ITEM_ID_P2_HAT_MMJ_DHAT01_START && hatType <= DRESSUP_ITEM_ID_P2_HAT_MMJ_DHAT01_END) return(DRESSUP_ANI_FEMALE_HAT_MMJ_DHAT);

  return(DRESSUP_ANI_NONE);
}

void Object_Chara::ChangeAnimationResourceDressUp_Battle( void *buf, gfl2::gfx::IGLAllocator *allocator  )
{
  gfl2::fs::BinLinkerAccessor mBinData;

  FreeDressUpAnimationNodes();

  mBinData.Initialize(buf);
  
  /*
  GFL_PRINT("BinLink Max: %i\n",mBinData.GetDataMax());
  // Print BinLinker Debug info:
  for(int n=0;n<MAX_DRESSUP_ANI_TYPE;n++)
  {
    GFL_PRINT("Blink(%i) Size:%i  Valid:%s\n",n,mBinData.GetDataSize(n),mBinData.GetData(n)!=NULL?"TRUE":"FALSE");
  }
  */

  // Base Animation:
  {  
    char* data;
    
    if(IsFemale()) data = static_cast<c8*>(mBinData.GetData(DRESSUP_ANI_FEMALE_BASE));
    else data = static_cast<c8*>(mBinData.GetData(DRESSUP_ANI_MALE_BASE));

    if(buf==NULL) return; // Base Animation Doesn't Exist!

    // Remove All Previous Animations:    
    ClearDressUpAnimation();
    
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
    anmRes.SetMotionData((c8*)data);

    anmResNodeDressUp[DRESSUP_ANISLOT_BASE] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator,&anmRes );

    mDressModel->ChangeAnimationByResourceNode(anmResNodeDressUp[DRESSUP_ANISLOT_BASE],DRESSUP_ANISLOT_BASE);
  }

   // Face Animation:
  {  
    char* data;
    int aniID = DRESSUP_ANI_FEMALE_FACE;
    
    if(IsFemale()) data = static_cast<c8*>(mBinData.GetData(DRESSUP_ANI_FEMALE_FACE));
    else
    {
      data = static_cast<c8*>(mBinData.GetData(DRESSUP_ANI_MALE_FACE));
      aniID = DRESSUP_ANI_MALE_FACE;
    }

    // Face Animation Exists:
    if(data!=NULL && mBinData.GetDataSize(aniID)>0) 
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
      anmRes.SetMotionData((c8*)data);

      anmResNodeDressUp[DRESSUP_ANISLOT_FACE] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator,&anmRes );

      if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_FACE)!=NULL)
      {
        if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_FACE)->GetAnimationController() != NULL)
        {
          mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_FACE)->ChangeAnimationByResourceNode(anmResNodeDressUp[DRESSUP_ANISLOT_FACE]);      
        }
      }
    }
  }

  // Hair
  int aniNum = DressUpBattle_HairType();

  if(aniNum!=DRESSUP_ANI_NONE)
  { 
    char* data = static_cast<c8*>(mBinData.GetData(aniNum));

    if(data!=NULL && mBinData.GetDataSize(aniNum)>0)
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
      anmRes.SetMotionData((c8*)data);

      anmResNodeDressUp[DRESSUP_ANISLOT_HAIR] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator,&anmRes );

      if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAIR)!=NULL)
      {
        if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAIR)->GetAnimationController() != NULL)
        {
          mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAIR)->ChangeAnimationByResourceNode(anmResNodeDressUp[DRESSUP_ANISLOT_HAIR]);      
        }
      }
    }
  }

  // Tops
  aniNum = DressUpBattle_TopType();

  if(aniNum!=DRESSUP_ANI_NONE)
  { 
    char* data = static_cast<c8*>(mBinData.GetData(aniNum));

    if(data!=NULL && mBinData.GetDataSize(aniNum)>0)
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
      anmRes.SetMotionData((c8*)data);

      anmResNodeDressUp[DRESSUP_ANISLOT_TOP] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator,&anmRes );

      if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_TOPS)!=NULL)
      {
        if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_TOPS)->GetAnimationController() != NULL)
        {
          mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_TOPS)->ChangeAnimationByResourceNode(anmResNodeDressUp[DRESSUP_ANISLOT_TOP]);
        }
      }
    }
  }

  // Bottoms
  aniNum = DressUpBattle_BottomType();

  if(aniNum!=DRESSUP_ANI_NONE)
  { 
    char* data = static_cast<c8*>(mBinData.GetData(aniNum));

    if(data!=NULL && mBinData.GetDataSize(aniNum)>0)
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
      anmRes.SetMotionData((c8*)data);

      anmResNodeDressUp[DRESSUP_ANISLOT_BOTTOM] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator,&anmRes );
      
      if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_BOTTOMS)!=NULL)
      {
        if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_BOTTOMS)->GetAnimationController() != NULL)
        {
          mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_BOTTOMS)->ChangeAnimationByResourceNode(anmResNodeDressUp[DRESSUP_ANISLOT_BOTTOM]);
        }
      }

    }
  }

  // Hat
  aniNum = DressUpBattle_HatType();

  if(aniNum!=DRESSUP_ANI_NONE)
  { 
    char* data = static_cast<c8*>(mBinData.GetData(aniNum));

    if(data!=NULL && mBinData.GetDataSize(aniNum)>0)
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
      anmRes.SetMotionData((c8*)data);

      anmResNodeDressUp[DRESSUP_ANISLOT_HAT] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator,&anmRes );

      if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAT)!=NULL)
      {
        if(mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAT)->GetAnimationController() != NULL)
        {
          mDressModel->GetPartsModel(poke_3d::model::DressUpModel::PARTS_MODEL_HAT)->ChangeAnimationByResourceNode(anmResNodeDressUp[DRESSUP_ANISLOT_HAT]);
        }
      }
    }
  }

}

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
