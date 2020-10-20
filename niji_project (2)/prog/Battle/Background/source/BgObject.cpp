//======================================================================
/**
 * @file	BgObject.cpp
 * @brief	戦闘背景 パーツ
 * @author	ariizumi
 * @data	15/04/10
 */
//======================================================================

#include <Debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>

#include "BgObject.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/background.gaix>


GFL_NAMESPACE_BEGIN(btl)

const int BgObject::ANM_SLOT_ARR[BgSystem::ANM_MAX] = 
{
  ANM_SLOT_LOOP,//    ANM_LOOP,
  ANM_SLOT_WHEATHER,//    ANM_RAIN_IN,
  ANM_SLOT_WHEATHER,//    ANM_RAIN_LOOP,
  ANM_SLOT_WHEATHER,//    ANM_RAIN_OUT,
  ANM_SLOT_WHEATHER,//    ANM_SUNLIGHT_IN,
  ANM_SLOT_WHEATHER,//    ANM_SUNLIGHT_LOOP,
  ANM_SLOT_WHEATHER,//    ANM_SUNLIGHT_OUT,
  ANM_SLOT_WHEATHER,//    ANM_CLOUD_IN,
  ANM_SLOT_WHEATHER,//    ANM_CLOUD_LOOP,
  ANM_SLOT_WHEATHER,//    ANM_CLOUD_OUT,
  ANM_SLOT_WHEATHER,//    ANM_WIND_IN,
  ANM_SLOT_WHEATHER,//    ANM_WIND_LOOP,
  ANM_SLOT_WHEATHER,//    ANM_WIND_OUT,
  ANM_SLOT_WHEATHER,//    ANM_SNOW_IN,
  ANM_SLOT_WHEATHER,//    ANM_SNOW_LOOP,
  ANM_SLOT_WHEATHER,//    ANM_SNOW_OUT,
  ANM_SLOT_DAY,//    ANM_1DAY,
  ANM_SLOT_TIME,//    ANM_MORNING_IN,
  ANM_SLOT_TIME,//    ANM_MORNING_LOOP,
  ANM_SLOT_TIME,//    ANM_MORNING_OUT,
  ANM_SLOT_TIME,//    ANM_NOON_IN,
  ANM_SLOT_TIME,//    ANM_NOON_LOOP,
  ANM_SLOT_TIME,//    ANM_NOON_OUT,
  ANM_SLOT_TIME,//    ANM_EVENING_IN,
  ANM_SLOT_TIME,//    ANM_EVENING_LOOP,
  ANM_SLOT_TIME,//    ANM_EVENING_OUT,
  ANM_SLOT_TIME,//    ANM_NIGHT_IN,
  ANM_SLOT_TIME,//    ANM_NIGHT_LOOP,
  ANM_SLOT_TIME,//    ANM_NIGHT_OUT,
};



const bool BgObject::ANM_SLOT_LOOP_ONOFF[BgSystem::ANM_MAX] = 
{
  true,//    ANM_LOOP,
  false,//    ANM_RAIN_IN,
  true,//    ANM_RAIN_LOOP,
  false,//    ANM_RAIN_OUT,
  false,//    ANM_SUNLIGHT_IN,
  true,//    ANM_SUNLIGHT_LOOP,
  false,//    ANM_SUNLIGHT_OUT,
  false,//    ANM_CLOUD_IN,
  true,//    ANM_CLOUD_LOOP,
  false,//    ANM_CLOUD_OUT,
  false,//    ANM_WIND_IN,
  true,//    ANM_WIND_LOOP,
  false,//    ANM_WIND_OUT,
  false,//    ANM_SNOW_IN,
  true,//    ANM_SNOW_LOOP,
  false,//    ANM_SNOW_OUT,
  true,//    ANM_1DAY,
  false,//    ANM_MORNING_IN,
  true,//    ANM_MORNING_LOOP,
  false,//    ANM_MORNING_OUT,
  false,//    ANM_NOON_IN,
  true,//    ANM_NOON_LOOP,
  false,//    ANM_NOON_OUT,
  false,//    ANM_EVENING_IN,
  true,//    ANM_EVENING_LOOP,
  false,//    ANM_EVENING_OUT,
  false,//    ANM_NIGHT_IN,
  true,//    ANM_NIGHT_LOOP,
  false,//    ANM_NIGHT_OUT,
};


BgObject::BgObject(void)
{
  mbRead=false;
  mMdlRes=NULL;
}

BgObject::~BgObject()
{
}


bool BgObject::Create(gfl2::util::GLHeapAllocator *allocator)
{
  if(!mbRead){
    return false;
  }

  for( int i = 0 ; i < ANM_SLOT_NUM; i++ )
  {
    mbAnimPlaying[i] = false;
  }
  //mAnmType=0;
  mBinData.Initialize(mBuf);

  for( int i=0;i<mBinData.GetDataMax();i++ )
  {
    //GFL_PRINT("Size[%d][%d]\n",i,mBinData.GetDataSize(i));
  }

  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>(mBinData.GetData( DATA_MDL_POS ) ) );
  mMdlRes = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator,&resourceData );
  mMdlNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( allocator,mMdlRes );

  for( int i=0;i<BgSystem::ANM_MAX;i++ )
  {

    
    if( mBinData.GetDataSize( DATA_ANIM_START_POS + i ) != 0 )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmResData;
      anmResData.SetMotionData(static_cast<c8*>(mBinData.GetData(DATA_ANIM_START_POS+i)));
      mAnmNode[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator,&anmResData );		
    }
    else
    {
      mAnmNode[i] = NULL;
    }
    OHNO_PRINT("アニメCreate %d %x\n",i,mAnmNode[i]);
  }
  mAnmController.Initialize( allocator->GetHeap() , mMdlNode);


  return true;
  
  //PlayAnimeCore(mAnmType);  //アニメは必ず外指定
}

void BgObject::Delete(void)
{
  if(mMdlRes)
  {
    mAnmController.Finalize();
    for( int i=0;i<BgSystem::ANM_MAX;i++ )
    {
      GFL_SAFE_DELETE(mAnmNode[i]);
    }
    GFL_SAFE_DELETE(m_envNode);
    GFL_SAFE_DELETE(m_envResourceNode);
    GFL_SAFE_DELETE(mMdlNode);
    GFL_SAFE_DELETE(mMdlRes);
    GflHeapFreeMemory(mBuf);
    OHNO_PRINT("アニメdelete\n");
  }
  mbRead=false;
  mMdlRes=NULL;
}

void BgObject::Update(void)
{


  if(mMdlRes){
    mAnmController.Update();
  }
}

void BgObject::SetVisible(bool flg)
{
  if(mMdlRes){
    mMdlNode->SetVisible(flg);
  }
}

void BgObject::SetRead(bool flg)
{
  mbRead  = flg;
}

bool BgObject::GetRead(void)
{
  return mbRead;
}



bool BgObject::IsAnimeEnd(int anmType)
{
  if(mMdlRes==NULL){
    return true;
  }
  //代表としてマテリアルでGET
  return mAnmController.GetMaterialSlot(ANM_SLOT_ARR[anmType])->IsLastFrame();
}


void BgObject::PlayAnimeCore(int anmType)
{
  if(mMdlRes==NULL){
    return;
  }

  if( mAnmNode[anmType] )
  {
    mAnmController.GetJointSlot(ANM_SLOT_ARR[anmType])->SetAnimationResource(mAnmNode[anmType]);
    mAnmController.GetJointSlot(ANM_SLOT_ARR[anmType])->SetLoop(ANM_SLOT_LOOP_ONOFF[anmType]);
    mAnmController.GetMaterialSlot(ANM_SLOT_ARR[anmType])->SetAnimationResource(mAnmNode[anmType]);
    mAnmController.GetMaterialSlot(ANM_SLOT_ARR[anmType])->SetLoop(ANM_SLOT_LOOP_ONOFF[anmType]);
    mAnmController.GetVisibilitySlot(ANM_SLOT_ARR[anmType])->SetAnimationResource(mAnmNode[anmType]);
    mAnmController.GetVisibilitySlot(ANM_SLOT_ARR[anmType])->SetLoop(ANM_SLOT_LOOP_ONOFF[anmType]);
    mbAnimPlaying[ANM_SLOT_ARR[anmType]] = true;
  }

#if GFL_DEBUG_PRINT
  if( mAnmNode[anmType] )
  {
    OHNO_PRINT("アニメ再生 %d\n",anmType);
  }
  else{
    OHNO_PRINT("アニメ再生してない %d\n",anmType);
  }
#endif//GFL_PRINT
}

void BgObject::StopAnimeCore(int anmType)
{
  if(mMdlRes==NULL){
    return;
  }
  
  mbAnimPlaying[ANM_SLOT_ARR[anmType]] = false;
  mAnmController.GetJointSlot(ANM_SLOT_ARR[anmType])->SetAnimationResource(NULL);
  mAnmController.GetMaterialSlot(ANM_SLOT_ARR[anmType])->SetAnimationResource(NULL);
  mAnmController.GetVisibilitySlot(ANM_SLOT_ARR[anmType])->SetAnimationResource(NULL);
}

bool BgObject::IsAnimPlaying(int anmType)
{
  return mbAnimPlaying[ANM_SLOT_ARR[anmType]];
}


void BgObject::SetAnimationFrame(int anmType, float frame)
{
  if(mMdlRes==NULL){
    return;
  }
  mAnmController.GetJointSlot(ANM_SLOT_ARR[anmType])->SetFrame(frame);
  mAnmController.GetMaterialSlot(ANM_SLOT_ARR[anmType])->SetFrame(frame);
  mAnmController.GetVisibilitySlot(ANM_SLOT_ARR[anmType])->SetFrame(frame);
}

int BgObject::GetAnimationFrame(int anmType)
{
  int frame = -1;

  if(mMdlRes==NULL)
  {
    return frame;
  }

  if(anmType >= 0 && anmType < BgSystem::ANM_MAX)
  {
    //代表としてマテリアルでGET
    frame = mAnmController.GetMaterialSlot(ANM_SLOT_ARR[anmType])->GetFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0,"AnmSlot Array Over!\n");
  }

  return frame;
}

int BgObject::GetAnimationLastFrame(int anmType)
{
  int frame = -1;

  if(mMdlRes==NULL)
  {
    return frame;
  }

  if(anmType >= 0 && anmType < BgSystem::ANM_MAX)
  {
    //代表としてマテリアルでGET
    frame = mAnmController.GetMaterialSlot(ANM_SLOT_ARR[anmType])->GetLoopEndFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0,"AnmSlot Array Over!\n");
  }

  return frame;
}

int BgObject::GetAnimationStepFrame(int anmType)
{
  int stepFrame = -1;

  if(mMdlRes==NULL)
  {
    return stepFrame;
  }

  if(anmType >= 0 && anmType < BgSystem::ANM_MAX)
  {
    //代表としてマテリアルでGET
    stepFrame = mAnmController.GetMaterialSlot(ANM_SLOT_ARR[anmType])->GetStepFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0,"AnmSlot Array Over!\n");
  }

  return stepFrame;
}

//バトルでロイヤル背景をまわす対応
void BgObject::RotateY_Deg(const gfl2::math::Quaternion &quat)
{
  if(mMdlRes==NULL)
  {
    return;
  }
  mMdlNode->SetLocalRotation(quat);
}

// アニメーションの更新のオンオフを切り替える
void BgObject::SetAnimationUpdateFrameEnable(bool isUpdateAnimation)
{
  if(mMdlRes==NULL)
  {
    return;
  }
  mAnmController.SetUpdateFrameEnable(isUpdateAnimation);
}

GFL_NAMESPACE_END(btl)

