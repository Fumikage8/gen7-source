//======================================================================
/**
 * @file	Object_GfMdl.cpp
 * @brief	オブジェクト GfMdl
 * @author  ariizumi
 * @data  15/03/16
 */
//======================================================================

#include <math/include/gfl2_math.h>
#include <str/include/gfl2_Str.h> //ノード名比較

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNode.h>

#include <model/include/gfl2_BaseModel.h>

#include <DemoLib/Object/include/Object_GfMdl.h>
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>


GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

Object_GfMdl::Object_GfMdl(ObjectRenderPipeLine *pipe)
:Object()
,mRenderingPipeLine(pipe)
,mIsAddRenderPath(false)
{
  mIsLoadAnm = false;
#if PM_DEBUG
  mIsDebugLoad = false;
#endif
  mSceneOpt = 0;
  aniSlot1Loaded = false;
}

Object_GfMdl::~Object_GfMdl()
{
}

void Object_GfMdl::UpdateAnimation(int step)
{
  UpdateAlways(step);  
}

void Object_GfMdl::UpdateAlways(int step)
{
  //ビルボード対応
  if( mRenderingPipeLine->GetViewMatrix() )
  {
    mModel->GetModelInstanceNode()->SetBillboardRotation(*mRenderingPipeLine->GetViewMatrix());
  }
  


  if(step!=1)
  {
    {    
      f32 curFrame = mModel->GetAnimationFrame();
      f32 frame = curFrame + (step-1);
      if(frame>curFrame || !mModel->IsAnimationLoop()) mModel->SetAnimationFrame(frame);
    }
    if(aniSlot1Loaded)
    {    
      f32 curFrame = mModel->GetAnimationFrame(1);
      f32 frame = curFrame + (step-1);
      if(frame>curFrame || !mModel->IsAnimationLoop(1)) mModel->SetAnimationFrame(frame,1);
    }
  }

  {
    f32 curFrame = mModel->GetAnimationFrame();
    //GFL_PRINT("Cur Aniframe:%f\n",curFrame);
  }
}

void Object_GfMdl::UpdatePosition(void)
{
  UpdateCore(0);
}

void Object_GfMdl::UpdateCore(int step)
{
  gfl2::math::Vector3 pos;
  gfl2::math::Vector3 scale;
  gfl2::math::Vector3 rot;
  GetCalcPosition(&pos);
  GetCalcRotate(&rot);
  GetCalcScale(&scale);

  gfl2::math::Quaternion rotQ;
  rotQ.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(rot.x),gfl2::math::ConvDegToRad(rot.y),gfl2::math::ConvDegToRad(rot.z));

  mModel->SetPosition(pos);
  //ビルボード対応
  if( GetModelNode()->GetBillboardType() == gfl2::renderingengine::BillboardType::None )
  {    
    mModel->SetRotation(gfl2::math::ConvDegToRad(rot.x),gfl2::math::ConvDegToRad(rot.y),gfl2::math::ConvDegToRad(rot.z),GetRotationOrder());
  }
  mModel->SetScale(scale);
  
}

//こっちはUpdateCoreの後に呼ばれる。座標反映後の更新はこちらで。
void Object_GfMdl::UpdateAlwaysAfterCore(int step)
{
  mModel->UpdateAnimation();
}

//モデル生成
void Object_GfMdl::StartLoad(gfl2::fs::AsyncFileManager *fileMng, const LOAD_PARAM &param)
{
  mFileManager = fileMng;

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = param.arcId;
  req.datId = param.mdlDataId;
  req.align = 128;
  req.heapForBuf = param.dataHeap;
  req.heapForReq = param.workHeap;
  req.heapForCompressed = (param.isComp)?(param.workHeap):(NULL);

  req.ppBuf = &mMdlRes;

  mFileManager->AddArcFileLoadDataReq(req);

  if( param.anmDataId != gfl2::fs::ArcFile::ARCDATID_NULL )
  {
    req.datId = param.anmDataId;
    req.ppBuf = &mAnmRes;
    mFileManager->AddArcFileLoadDataReq(req);
    mIsLoadAnm = true;
  }
  else
  {
    mIsLoadAnm = false;
  }

#if PM_DEBUG
  mIsDebugLoad = false;
#endif
}

#if PM_DEBUG
void Object_GfMdl::StartLoad_Debug(gfl2::fs::AsyncFileManager *fileMng, const LOAD_PARAM &param)
{
  mFileManager = fileMng;

  gfl2::fs::AsyncFileManager::FileEasyReadReq req;
  req.fileName = param.mdlFilePath;
  req.align = 128;
  req.heapForBuf = param.dataHeap;
  req.heapForReq = param.workHeap;
  req.heapForCompressed = (param.isComp)?(param.workHeap):(NULL);
  req.ppBuf = &mMdlRes;

  mFileManager->SyncFileEasyRead(req);

  if( param.anmFilePath )
  {
    req.fileName = param.anmFilePath;
    req.ppBuf = &mAnmRes;
    mFileManager->SyncFileEasyRead(req);
    mIsLoadAnm = true;
  }
  else
  {
    mIsLoadAnm = false;
  }
  
  mIsDebugLoad = true;
}
#endif

bool Object_GfMdl::IsFinishLoad(void)
{
#if PM_DEBUG
  if( mIsDebugLoad )
  {
    if( mFileManager->IsFileEasyReadFinished( &mMdlRes ) &&
        (mIsLoadAnm==false || mFileManager->IsFileEasyReadFinished( &mAnmRes )) )
    {
      return true;
    }
  }
  else
#endif
  {
    if( mFileManager->IsArcFileLoadDataFinished( &mMdlRes ) &&
        (mIsLoadAnm==false || mFileManager->IsArcFileLoadDataFinished( &mAnmRes )) )
    {
      return true;
    }
  }
  return false;
}
void Object_GfMdl::Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator)
{
  CreateBuffer(mMdlRes,heap,allocator);
}

void Object_GfMdl::CreateBufferTexture(void *buf,gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator,gfl2::renderingengine::scenegraph::DagNode* pTextureShaderNodeRoot)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>(buf) );
  mMdlResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, &resourceData );
  mModel = GFL_NEW(heap)poke_3d::model::BaseModel();

  if(pTextureShaderNodeRoot!=NULL)
    mModel->Create(allocator,heap,mMdlResNode,pTextureShaderNodeRoot);
  else
    mModel->Create(allocator,heap,mMdlResNode);

  SetVisible(true);
  SetIsEnable(true);

  if( mIsLoadAnm )
  {
    SetAnimation(allocator,mAnmRes);
  }
}

void Object_GfMdl::CreateBuffer(void *buf,gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>(buf) );
  mMdlResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, &resourceData );
  mModel = GFL_NEW(heap)poke_3d::model::BaseModel();
  
  mModel->Create(allocator,heap,mMdlResNode);

  SetVisible(true);
  SetIsEnable(true);

  if( mIsLoadAnm )
  {
    SetAnimation(allocator,mAnmRes);
  }
}

void Object_GfMdl::Delete(void)
{
  if( IsEnable() )
  {
    SetIsEnable(false);

    GFL_SAFE_DELETE(mAnmResNode);
    GflHeapSafeFreeMemory(mAnmRes);

    GFL_SAFE_DELETE(mModel);
    GFL_SAFE_DELETE(mMdlResNode);
    GflHeapSafeFreeMemory(mMdlRes);
  }
}

//ObjectRenderPipeLine経由で追加する
void Object_GfMdl::AddRenderPath(int opt)
{
  if( IsEnable() && IsAddRenderPath() == false )
  {
    mSceneOpt = opt;  //自動削除タスクのため覚えておく
    mRenderingPipeLine->AddObject_GfMdl(mModel->GetModelInstanceNode(),mSceneOpt);
    mIsAddRenderPath = true;
  }
}

void Object_GfMdl::RemoveRenderPath(void)
{
  if( IsEnable() && IsAddRenderPath() )
  {
    mRenderingPipeLine->RemoveObject_GfMdl(mModel->GetModelInstanceNode(),mSceneOpt);
    mIsAddRenderPath = false;
  }
}

//継承
void Object_GfMdl::SetVisible(const bool flg)
{
  if( IsEnable() )
  {
    mModel->SetVisible(flg);
    Object::SetVisible(flg);
  }
}

gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* Object_GfMdl::GetModelNode(void)
{
  return mModel->GetModelInstanceNode();
}

poke_3d::model::BaseModel* Object_GfMdl::GetBaseModel(void)
{
  return mModel;
}

void Object_GfMdl::UpdateAnimationForce(void)
{
  //単体で更新してしまう
  //gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFastFromChild(GetModelNode());
  //@todo TraverseModelFastFromChildだと、親のnode自体は更新されないのでSetPositionの座標が反映されない
  //専用関数用意予定
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();
}

gfl2::renderingengine::scenegraph::instance::JointInstanceNode* Object_GfMdl::GetJointNode( char *name )
{
  u32 num = GetModelNode()->GetJointNum();
  for( u32 i=0;i<num;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* node = GetModelNode()->GetJointInstanceNode(i);
    if( gfl2::str::StrComp_J( name,node->GetName() ) )
    {
      return node;
    }
  }
  return NULL;
}

void Object_GfMdl::SetAnimation( gfl2::gfx::IGLAllocator *allocator , void*buf,int aniSlot )
{
  gfl2::renderingengine::scenegraph::resource::ResourceNode* anmNode;

  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmResData;
  anmResData.SetMotionData(static_cast<c8*>(buf));
  anmNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, &anmResData );		

  mModel->ChangeAnimationByResourceNode(anmNode,aniSlot);
  mModel->SetAnimationLoop(true,aniSlot);

  //@todo 補間アニメをする場合はココで削除するとまずいので、押し出し式でもう一個Nodeのバッファ作る
  GFL_SAFE_DELETE(mAnmResNode);
  mAnmResNode = anmNode;

  if(aniSlot==1 && buf!=NULL) aniSlot1Loaded = true;
}

void Object_GfMdl::RemoveAnimation(int aniSlot)
{
  if(aniSlot==1) aniSlot1Loaded = false;
  mModel->ChangeAnimationByResourceNode(NULL,aniSlot);
}

void Object_GfMdl::SetAnimationIsLoop( bool flg,int aniSlot )
{
  if( IsEnable() )
  {
    mModel->SetAnimationLoop(flg,aniSlot);
  }
}

void Object_GfMdl::SetAnimationSpeed(f32 speed,int aniSlot )
{
  if( IsEnable() )
  {
    mModel->SetAnimationStepFrame(speed,aniSlot);
  }
}

void Object_GfMdl::SetAnimationFrame(f32 frame,int aniSlot )
{
  if( IsEnable() )
  {
    mModel->SetAnimationFrame(frame,aniSlot);
  }
}

b32 Object_GfMdl::IsFinishAnimation(void)
{
  if( IsEnable() )
  {
    if( mModel->IsAnimationLoop() == true )
    {
      return false;
    }
    return mModel->IsAnimationLastFrame();
  }
  return true;
}

gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* Object_GfMdl::GetMaterialNode(const char* matName)
{
  int matNum = GetModelNode()->GetMaterialNum();
  for( int i=0;i<matNum;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode *matNode = GetModelNode()->GetMaterialInstanceNode(i);
    if( gfl2::str::StrComp_J( matName,matNode->GetName() ) )
    {
      return matNode;
    }
  }
  return NULL;
}

gfl2::math::Vector4 Object_GfMdl::GetConstantColor(const char* matName , const int colNo )
{
  gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode *matNode = GetMaterialNode(matName);
  if(matNode)
  {
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rAttributeParam = *matNode->GetAttributeParam();
    return rAttributeParam.m_ConstantColor[colNo].GetColor();
  }
  GFL_ASSERT_MSG(0,"マテリアル[%s]が見つかりませんでした。\n",matName);
  {
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rAttributeParam = *GetModelNode()->GetMaterialInstanceNode(0)->GetAttributeParam();
    return rAttributeParam.m_ConstantColor[colNo].GetColor();
   }
}
void Object_GfMdl::SetConstantColor(const char* matName , const int colNo , const gfl2::math::Vector4 &color )
{
  if( matName == NULL )
  {
    int matNum = GetModelNode()->GetMaterialNum();
    for( int i=0;i<matNum;i++ )
    {
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode *matNode = GetModelNode()->GetMaterialInstanceNode(i);
      gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rAttributeParam = *matNode->GetAttributeParam();
      rAttributeParam.m_ConstantColor[colNo]= color;
    }
  }
  else
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode *matNode = GetMaterialNode(matName);
    if(matNode)
    {
      gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rAttributeParam = *matNode->GetAttributeParam();
      rAttributeParam.m_ConstantColor[colNo]= color;
    }
  }
}

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
