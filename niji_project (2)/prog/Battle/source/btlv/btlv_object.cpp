//======================================================================
/**
 * @file	btlv_object.cpp
 * @brief	バトル描画 モデル等の規定クラス
 * @author	ariizumi
 * @data	14/11/13
 */
//======================================================================

#include "System/include/GflUse.h"
#include "sound/include/Sound.h"

#include "btlv_core.h"
#include "btlv_object.h"
#include "btlv_seq_com_define.h"

GFL_NAMESPACE_BEGIN(btl)

void UpdateAutoRotateFunc( gfl2::math::Vector3 &nowPos , gfl2::math::Vector3 &befPos , gfl2::math::Vector3 *retRot )
{
  const gfl2::math::Vector3 ofs = nowPos - befPos;
  const f32 len =  gfl2::math::Vector2(ofs.x,ofs.z).Length();
    
  retRot->x = gfl2::math::Atan2Deg( len , ofs.y );
  retRot->y = gfl2::math::Atan2Deg(ofs.x , ofs.z );
  retRot->z = 0;
}

//コンストラクタ
BtlvObject_Gfmdl::BtlvObject_Gfmdl(BattleViewSystem *btlvCore,DemoLib::Obj::ObjectRenderPipeLine *pipe)
:DemoLib::Obj::Object_GfMdl(pipe)
,mBtlvCore(btlvCore)
{
  mDeleteCnt = 0;
  mWaitCreate = false;
  mType = 0;
}

//デストラクタ
BtlvObject_Gfmdl::~BtlvObject_Gfmdl()
{
}

void BtlvObject_Gfmdl::Update(int step)
{
  if( mAutoRotate )
  {
    gfl2::math::Vector3 nowPos;
    gfl2::math::Vector3 retRot;
    GetCalcPosition(&nowPos);
    UpdateAutoRotateFunc(nowPos,mBefPos,&retRot);
    mBefPos = nowPos;
    SetRotate(retRot);
  }
  DemoLib::Obj::Object_GfMdl::Update(step);

  //生成待ち
  if( mWaitCreate && IsFinishLoad() )
  {
    Create(mBtlvCore->GetTempHeap(),mBtlvCore->GetTempGLAllocator());
    AddRenderPath();
    SetIsUpdate();
    mWaitCreate = false;
  }
  //破棄待ち
  if( mDeleteCnt > 0 )
  {
    mDeleteCnt--;
    if( mDeleteCnt == 0 )
    {
      Delete();
    }
  }
}

//独自
void BtlvObject_Gfmdl::ReserveCreate(void)
{

  mWaitCreate = true;
}
void BtlvObject_Gfmdl::ReserveDelete(void)
{
  mDeleteCnt = 2;
}
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

//コンストラクタ
BtlvObject_Effect::BtlvObject_Effect(BattleViewSystem *btlvCore)
:DemoLib::Obj::Object_Effect(btlvCore->GetEffectSystem())
,mBtlvCore(btlvCore)
{
  mDeleteCnt = 0;
  mWaitCreate = false;
  mType = 0;
}

//デストラクタ
BtlvObject_Effect::~BtlvObject_Effect()
{
}

void BtlvObject_Effect::Update(int step)
{
  if( mAutoRotate )
  {
    gfl2::math::Vector3 nowPos;
    gfl2::math::Vector3 retRot;
    GetCalcPosition(&nowPos);
    UpdateAutoRotateFunc(nowPos,mBefPos,&retRot);
    mBefPos = nowPos;
    SetRotate(retRot);
  }

  DemoLib::Obj::Object_Effect::Update(step);

  //生成待ち
  if( mWaitCreate && IsFinishLoad() )
  {
    Create(mBtlvCore->GetEffectHeap(),0);
    AddRenderPath();
    SetIsUpdate();
    mWaitCreate = false;
  }
  //破棄待ち
  if( mDeleteCnt > 0 )
  {
    mDeleteCnt--;
    if( mDeleteCnt == 0 )
    {
      Delete();
    }
  }
}

//独自
void BtlvObject_Effect::ReserveCreate(void)
{

  mWaitCreate = true;
}
void BtlvObject_Effect::ReserveDelete(void)
{
  mDeleteCnt = 2;
}


//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

void BtlvObject_Sound::SetSoundId(int id)
{
  //ついでにリセットしてしまう。
  SetFollowObject(NULL);
  SetFollowPoke(NULL);

  mSoundId = id;
  mTargetGrp = -1;
  if( id != -1 )
  {
    SetIsEnable(true);
  }
  else
  {
    SetIsEnable(false);
  }
}

void BtlvObject_Sound::SetTargetGrp(int grp,int type)
{
  //追従先のObjが死ぬのを監視するため
  mTargetGrp = grp;
  mTargetType = type;
}

//こっちはUpdate直後の追従処理の前に呼ばれる
void BtlvObject_Sound::UpdateFirst(int step)
{
  if( mTargetGrp != -1 && mFollowObject )
  {
    bool isExist = false;
    if( mTargetType == SEQ_DEF_FOLLOW_PARTICLE )
    {
      if( mBtlvCore->GetEffectObjManager()->GetObjectArr(mTargetGrp,NULL) != NULL )
      {
        isExist = true;
      }
    }
    else
    if( mTargetType == SEQ_DEF_FOLLOW_MODEL )
    {
      if( mBtlvCore->GetModelObjManager()->GetObjectArr(mTargetGrp,NULL) != NULL )
      {
        isExist = true;
      }
    }
    if( isExist == false )
    {
      SetFollowObject(NULL);
      mTargetGrp = -1;
    }
  }
}

void BtlvObject_Sound::UpdateAlways(int step)
{
  if( mIs3DSound )
  {
    if( mIsBefUpdate )
    {
      gfl2::math::Vector3 pos;
      GetCalcPosition( &pos );
      gfl2::math::Vector3 diff = pos - mBefPos;
      gfl2::math::VEC3 nwPos(diff.x,diff.y,diff.z);
      Sound::Set3DActorVelocity( mIndex , nwPos );
      mIsBefUpdate = false;
    }
    else
    {
      gfl2::math::VEC3 zero(0,0,0);
      Sound::Set3DActorVelocity( mIndex , zero );
    }
  }
}
void BtlvObject_Sound::UpdateCore(int step)
{
  if( mIs3DSound )
  {
    gfl2::math::Vector3 pos;
    GetCalcPosition( &pos );

    gfl2::math::VEC3 nwPos(pos.x,pos.y,pos.z);
    Sound::Set3DActorPos( mIndex , nwPos );
    mBefPos = pos;
    mIsBefUpdate = true;
  }
}

GFL_NAMESPACE_END(btl)

