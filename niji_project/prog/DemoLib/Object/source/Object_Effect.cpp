//======================================================================
/**
 * @file	Object_Effect.cpp
 * @brief	オブジェクト Effect
 * @author  ariizumi
 * @data  15/03/25
 */
//======================================================================

#include <math/include/gfl2_math.h>
#include <math/include/gfl2_EulerRotation.h>
#include <math/include/gfl2_SRT.h>

#include <DemoLib/Object/include/Object_Effect.h>
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>


GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

Object_Effect::Object_Effect(gfl2::Effect::System *effSys)
:Object()
,mEffectSystem(effSys)
,mIsAddRenderPath(false)
{
#if PM_DEBUG
  mIsDebugLoad = false;
#endif
}

Object_Effect::~Object_Effect()
{
}

void Object_Effect::ForceUpdate(int step)
{
  if( IsEnable() )
  {
    for( int i=0;i<mEffNum;i++ )
    {
      if( mEffectHandle[i]!=NULL &&
          mEffectHandle[i]->GetEmitterSet())
      {
        mEffectHandle[i]->GetEmitterSet()->ForceCalc(step);
      }
    }
  }
}

void Object_Effect::UpdateAlways(int step)
{
}

void Object_Effect::UpdatePosition(void)
{
  UpdateCore(0);
}

void Object_Effect::UpdateCore(int step)
{
  gfl2::math::Vector3 posTemp;
  gfl2::math::Vector3 scaleTemp;
  gfl2::math::Vector3 rotTemp;
  GetCalcPosition(&posTemp);
  GetCalcRotate(&rotTemp);
  GetCalcScale(&scaleTemp);

  gfl2::math::VEC3 pos(posTemp.x,posTemp.y,posTemp.z);
  gfl2::math::VEC3 rot(gfl2::math::ConvDegToRad(rotTemp.x),gfl2::math::ConvDegToRad(rotTemp.y),gfl2::math::ConvDegToRad(rotTemp.z));
  gfl2::math::VEC3 scale(scaleTemp.x,scaleTemp.y,scaleTemp.z);

  gfl2::math::Vector3 oneVec(1,1,1);  //スケールは別処理なので１
  // SRT計算用
  gfl2::math::EulerRotation eulerRot(gfl2::math::ConvDegToRad(rotTemp.x),gfl2::math::ConvDegToRad(rotTemp.y),gfl2::math::ConvDegToRad(rotTemp.z),GetRotationOrder());

  gfl2::math::SRT srt(oneVec,eulerRot.ToQuaternion(),posTemp);
  gfl2::math::Matrix34 mat = srt.ToMatrix();

  nw::math::MTX34 nwSrt;
  for( int x=0;x<3;x++ )
  {
    for( int y=0;y<4;y++ )
    {
      nwSrt.m[x][y] = mat[x][y];
    }
  }

#if 0
  //比較確認用
  nw::math::MTX34 testSrt;
#if defined(GF_PLATFORM_CTR)
  nw::eft::MTX34MakeSRT(&testSrt,gfl2::math::VEC3(1,1,1),rot,pos);
#elif defined(GF_PLATFORM_WIN32)
  nw::math::MTX34MakeSRT(&testSrt,gfl2::math::VEC3(1,1,1),rot,pos);
#endif  //プラットフォーム分岐
  {
    ARIIZUMI_PRINT("NwMat");
    for( int x=0;x<3;x++ )
    {
      for( int y=0;y<4;y++ )
      {
        ARIIZUMI_PRINT("[%9.3f]",nwSrt.m[x][y]);
      }
    }
    ARIIZUMI_PRINT("\nGfMat");
    for( int x=0;x<3;x++ )
    {
      for( int y=0;y<4;y++ )
      {
        ARIIZUMI_PRINT("[%9.3f]",testSrt.m[x][y]);
      }
    }
    ARIIZUMI_PRINT("\n");
  }
#endif

  if( mEffNum > 0 )
  {
    for( int i=0;i<mEffNum;i++ )
    {
      if( mEffectHandle[i] &&
          mEffectHandle[i]->GetEmitterSet() )
      {
        mEffectHandle[i]->GetEmitterSet()->SetMtxFast(nwSrt,scale);
        //mEffectHandle[i]->GetEmitterSet()->SetMtxFast(nwSrt,scale);
      }
    }
  }

  //gfl2::math::Quaternion rotQ;
  //rotQ.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(rot.x),gfl2::math::ConvDegToRad(rot.y),gfl2::math::ConvDegToRad(rot.z));

  //mMdlNode->SetLocalTranslate(pos);
  //mMdlNode->SetLocalRotation(rotQ);
  //mMdlNode->SetLocalScale(scale);
}

bool Object_Effect::IsAlive(void)
{
  for( int i=0;i<mEffNum;i++ )
  {
    if( mEffectHandle[i] && mEffectHandle[i]->GetEmitterSet()!=NULL && mEffectHandle[i]->GetEmitterSet()->IsAlive()) return(true);
  }
  return(false);
}

//モデル生成
void Object_Effect::StartLoad(gfl2::fs::AsyncFileManager *fileMng, const LOAD_PARAM &param)
{
  mFileManager = fileMng;

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = param.arcId;
  req.datId = param.dataId;
  req.align = 128;
  req.heapForBuf = param.dataHeap;
  req.heapForReq = param.workHeap;
  req.heapForCompressed = (param.isComp)?(param.workHeap):(NULL);

  req.ppBuf = &mEffRes;

  mFileManager->AddArcFileLoadDataReq(req);

#if PM_DEBUG
  mIsDebugLoad = false;
#endif
}

#if PM_DEBUG
void Object_Effect::StartLoad_Debug(gfl2::fs::AsyncFileManager *fileMng, const LOAD_PARAM &param)
{
  mFileManager = fileMng;

  gfl2::fs::AsyncFileManager::FileEasyReadReq req;
  req.fileName = param.dataFilePath;
  req.align = 128;
  req.heapForBuf = param.dataHeap;
  req.heapForReq = param.workHeap;
  req.heapForCompressed = (param.isComp)?(param.workHeap):(NULL);

  req.ppBuf = &mEffRes;

  mFileManager->AddFileEasyReadReq(req);

  mIsDebugLoad = true;
}
#endif

bool Object_Effect::IsFinishLoad(void)
{
#if PM_DEBUG
  if( mIsDebugLoad )
  {
    if( mFileManager->IsFileEasyReadFinished( &mEffRes ) )
    {
      return true;
    }
  }
  else
#endif
  {
    if( mFileManager->IsArcFileLoadDataFinished( &mEffRes ) )
    {
      return true;
    }
  }
  return false;
}
void Object_Effect::Create( gfl2::Effect::GFHeap *effHeap , int resIdx )
{
  CreateBuffer( mEffRes ,effHeap , resIdx );
}

void Object_Effect::CreateBuffer(void *buf,gfl2::Effect::GFHeap *effHeap , int resIdx )
{
  mEffectHeap = effHeap;
  mEffectSystem->EntryResource( mEffectHeap , buf , resIdx );
  CreateResIdx(mEffectHeap,resIdx);
  mIsEntryResource = true;
}

//外でEffectSystemにEntryResourceしてから作成
void Object_Effect::CreateResIdx(gfl2::Effect::GFHeap *effHeap , int resIdx )
{
  mResIdx = resIdx;
  mEffectHeap = effHeap;

  mEffNum = mEffectSystem->GetResource(mResIdx)->GetNumEmitterSet();

  mEffectHandle = GFL_NEW_ARRAY(effHeap->GetGFHeapBase())gfl2::Effect::Handle*[mEffNum];
  for( int i=0;i<mEffNum;i++ )
  {
    mEffectHandle[i] = NULL;
  }

  SetIsEnable(true);
  mIsEntryResource = false;
}

void Object_Effect::Delete(void)
{
  if( IsEnable() )
  {
    SetIsEnable(false);
    if( mIsEntryResource )
    {
      mEffectSystem->ClearResource( mEffectHeap, mResIdx);
    }
    if( mEffNum > 0 )
    {
      for( int i=0;i<mEffNum;i++ )
      {
        GFL_SAFE_DELETE( mEffectHandle[i] );
      }
    }
    GFL_SAFE_DELETE_ARRAY(mEffectHandle);
    GflHeapFreeMemory(mEffRes);
  }
}

//ObjectRenderPipeLine経由で追加する
void Object_Effect::AddRenderPath(int groupId)
{
  if( IsEnable() && IsAddRenderPath() == false )
  {
    for( int i=0;i<mEffNum;i++ )
    {
      mEffectHandle[i] = GFL_NEW(mEffectHeap->GetGFHeapBase())gfl2::Effect::Handle();
      mEffectSystem->CreateEmitterSetID( mEffectHandle[i] , GetPosition() , i , mResIdx , groupId );
    }
    mIsAddRenderPath = true;
    mGroupId = groupId;
  }
}

void Object_Effect::RemoveRenderPath(void)
{
  if( IsEnable() && IsAddRenderPath() )
  {
    for( int i=0;i<mEffNum;i++ )
    {
      if(mEffectHandle[i]!=NULL && mEffectHandle[i]->GetEmitterSet()!=NULL)
        mEffectHandle[i]->GetEmitterSet()->Kill();
      GFL_SAFE_DELETE( mEffectHandle[i] );
    }
    mIsAddRenderPath = false;
  }
}

//継承
void Object_Effect::SetVisible(const bool flg)
{
  if( IsEnable() )
  {
    for( int i=0;i<mEffNum;i++ )
    {
      if( mEffectHandle[i]!=NULL &&
        mEffectHandle[i]->GetEmitterSet())
      {
        int num = mEffectHandle[i]->GetEmitterSet()->GetNumCreatedEmitter();
        for( int j=0;j<num;j++ )
        {
          mEffectHandle[i]->GetEmitterSet()->GetEmitterController(j)->SetVisible(flg);
        }
      }
    }
    Object::SetVisible(flg);
  }
}

void Object_Effect::StopParticle(void)
{
  if( IsEnable() )
  {
    for( int i=0;i<mEffNum;i++ )
    {
      if(mEffectHandle[i]!=NULL && mEffectHandle[i]->GetEmitterSet()!=NULL)
        mEffectHandle[i]->GetEmitterSet()->Fade();      
    }
  }
}

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
