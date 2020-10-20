//======================================================================
/**
 * @file Object_Cluster.cpp
 * @date 2015/08/15 13:52:42
 * @author ariizumi_nobuhiko
 * @brief オブジェクト 群体制御
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>

#include "System/include/GflUse.h"

#include "DemoLib/Object/include/ObjectManager.h"
#include "DemoLib/Object/include/Object_GfMdl.h"
#include "DemoLib/Object/include/Object_Effect.h"
#include "DemoLib/Object/include/Object_Cluster.h"

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

//コンストラクタ
Object_Cluster::Object_Cluster()
{
  mIsCallDelete = false;
}

//デストラクタ
Object_Cluster::~Object_Cluster()
{
}
  
//モデルを使って生成
void Object_Cluster::CreateModel(const SetupParam &param , const ModelParam &modelParam)
{
  mModelParam = modelParam;
  mObjType = OBJECT_TYPE_MODEL;

  CreateCommon(param);
}

//エフェクトを使って生成
void Object_Cluster::CreateEffect(const SetupParam &param , const EffectParam &effParam)
{
  mEffectParam = effParam;
  mObjType = OBJECT_TYPE_EFFECT;

  CreateCommon(param);
}

//共通部分の生成
void Object_Cluster::CreateCommon(const SetupParam &param)
{
  mSetupParam = param;

  mSpawnCnt = 0;

  //Objectの管理領域の作成
  mSpawnObjArr = GFL_NEW_ARRAY(mSetupParam.heap)SpawnObjectData[mSetupParam.objectMax];
  for( int i=0;i<mSetupParam.objectMax;i++ )
  {
    mSpawnObjArr[i].isEnable = false;
  }

  SetIsEnable(true);
}

//破棄
bool Object_Cluster::Delete(void)
{
  bool isFinish = true;
  mIsCallDelete = true;

  //オブジェクトの更新処理
  if( mSpawnObjArr )
  {
    for( int i=0;i<mSetupParam.objectMax;i++ )
    {
      if( mSpawnObjArr[i].isEnable )
      {
        DeleteObject(&mSpawnObjArr[i]);
        isFinish = false;
      }
    }
  }
  if( isFinish )
  {
    GFL_SAFE_DELETE_ARRAY(mSpawnObjArr);
    SetIsEnable(false);
  }
  return isFinish;
}

//IsEnableはチェック済み。
//毎フレーム呼ばれる
void Object_Cluster::UpdateAlways(int step)
{
  if( mIsCallDelete )return;

  for( int s=0;s<step;s++ )
  {
    if( GetSpawnType() == SPAWN_PARENT )
    {
      CheckSpawn_Time(mSpawnCnt,this);
    }
    mSpawnCnt++;

    //オブジェクトの更新処理
    if( mSpawnObjArr )
    {
      for( int i=0;i<mSetupParam.objectMax;i++ )
      {
        if( mSpawnObjArr[i].isEnable )
        {
          UpdateObject( &mSpawnObjArr[i] );
        }
      }
    }
  }
}

//IsUpdateが立っていたら来る関数。
//IsEnableはチェック済みなので座標更新はこちらで。
void Object_Cluster::UpdateCore(int step)
{
}

//時間生成のチェック(親と子の判定があるので、関数化)
void Object_Cluster::CheckSpawn_Time(int cnt,Object *parent)
{
  //生成判定
  if( (cnt % mSetupParam.spawnInterval == 0 &&
      cnt < mSetupParam.spawnTime )||
      mSetupParam.spawnTime == -1)  //無限
  {
    SpawnObject(parent);
  }
}

//Objectの作成
void Object_Cluster::SpawnObject(Object *parent)
{
  //生成個数分まわす
  for( int i=0;i<mSetupParam.spawnNum;i++ )
  {
    if( mSetupParam.spawnRate == 100 ||
        mSetupParam.spawnRate > static_cast<int>(System::GflUse::GetPublicRand(100)) )
    {
      //空きを探す
      for( int ii=0;ii<mSetupParam.objectMax;ii++ )
      {
        if( mSpawnObjArr[ii].isEnable == false )
        {
          SpawnCore(&mSpawnObjArr[ii],parent);
          break;
        }
      }
    }
  }
}

//生成処理
void Object_Cluster::SpawnCore(SpawnObjectData *spawnObj,Object *parent)
{
  //中身をクリア
  gfl2::std::MemClear( spawnObj, sizeof(SpawnObjectData) );

  //Objectの作成
  if( mObjType == OBJECT_TYPE_MODEL )
  {
    SpawnCore_Model(spawnObj);
  }
  else
  {
    SpawnCore_Effect(spawnObj);
  }
  spawnObj->parent = parent;
  spawnObj->isEnable = true;
  spawnObj->randScale.Set(1,1,1);
  //初期位置の決定
  SpawnCore_SetStartPosition(spawnObj);
  //速度系の設定
  SpawnCore_SetStartSpeed(spawnObj);
  //その他の設定(回転・拡縮)
  SpawnCore_SetStartParam(spawnObj);
  //親より低い優先度にしておく
  mSetupParam.objManager->AddObject(spawnObj->obj,parent->GetPriority()+1);
}

//モデルObjの生成
void Object_Cluster::SpawnCore_Model(SpawnObjectData *spawnObj)
{

  Object_GfMdl *mdl;
  mdl = GFL_NEW(mSetupParam.heap)Object_GfMdl(mModelParam.pipe);

  mdl->CreateBuffer(mModelParam.modelBuf,mModelParam.heap,mModelParam.allocator);
  mdl->AddRenderPath(mModelParam.drawOpt);
  mdl->SetRotationOrder(static_cast<gfl2::math::EulerRotation::RotationOrder>(mModelParam.rotOrder));
  if( mModelParam.anmBuf )
  {
    mdl->SetAnimation( mModelParam.allocator,mModelParam.anmBuf );
    mdl->SetAnimationIsLoop(true);
    mdl->SetAnimationSpeed(1.0f);
  }

  spawnObj->obj = mdl;
  spawnObj->objMdl = mdl;
  spawnObj->objEff = NULL;
}

//エフェクトObjの生成
void Object_Cluster::SpawnCore_Effect(SpawnObjectData *spawnObj)
{

  Object_Effect *eff;
  eff = GFL_NEW(mSetupParam.heap)Object_Effect(mEffectParam.effSystem);

  eff->CreateResIdx(mEffectParam.effHeap,mEffectParam.effResIdx);
  eff->AddRenderPath(mEffectParam.drawOpt);
  eff->SetRotationOrder(static_cast<gfl2::math::EulerRotation::RotationOrder>(mEffectParam.rotOrder));

  spawnObj->obj = eff;
  spawnObj->objMdl = NULL;
  spawnObj->objEff = eff;
}

//計算関数 座標の回転(Deg
inline void ObjectRot_Deg( f32 &x , f32 &z , const f32 deg )
{
  f32 sin,cos;
  const f32 wx = x;
  const f32 wz = z;
  gfl2::math::SinCosDeg( &sin,&cos,deg );
  x = wx*cos - wz*sin;
  z = wx*sin + wz*cos;
}
//計算関数 Vectorの回転
inline void ObjectRotVector3_Rad( gfl2::math::Vector3 &pos , const gfl2::math::Vector3 &rot)
{
  //回す。(移植のためNintendoWareの関数をそのまま使用。そのため変換してる)
  gfl2::math::VEC3 rotVec(rot.x,rot.y,rot.z);
  gfl2::math::VEC3 ofsVec3(pos.x,pos.y,pos.z);

  gfl2::math::MTX34 rotMtx;
#if defined(GF_PLATFORM_CTR)
  rotMtx.SetupRotateXyz( rotVec );
#elif defined(GF_PLATFORM_WIN32)
  rotMtx.SetRotateXyz( rotVec );
#endif  //プラットフォーム分岐
  gfl2::math::VEC3Transform( &ofsVec3, &rotMtx, &ofsVec3 );

  //型を戻して適用
  pos.Set(ofsVec3.x,ofsVec3.y,ofsVec3.z);
}
inline void ObjectRotVector3_Deg( gfl2::math::Vector3 &pos , const gfl2::math::Vector3 &rot)
{
  gfl2::math::Vector3 radRot(gfl2::math::ConvDegToRad(rot.x),
                             gfl2::math::ConvDegToRad(rot.y),
                             gfl2::math::ConvDegToRad(rot.z));
  ObjectRotVector3_Rad(pos,radRot);
}

//初期位置
void Object_Cluster::SpawnCore_SetStartPosition(SpawnObjectData *spawnObj)
{
  //位置
  
  //自身の位置を基点に
  gfl2::math::Vector3 pos = spawnObj->parent->GetPosition();
  gfl2::math::Vector3 ofs(0,0,0);
  gfl2::math::Vector3 scl;
  spawnObj->parent->GetCalcScale(&scl);
  
  scl = scl.MulPerElem( mSetupParam.emitterSize );
  scl.x = gfl2::math::Abs(scl.x);
  scl.y = gfl2::math::Abs(scl.y);
  scl.z = gfl2::math::Abs(scl.z);

  //中心からの空洞率(%)
  const f32 lenRate = 1.0f - (f32)(System::GflUse::GetPublicRand(100-mSetupParam.emitterCavityRate))*0.01f;
  
  switch(mSetupParam.emitterType)
  {
  case EMITTER_POINT: //点
    //no_func
    break;
  case EMITTER_SPHERE:  //球
    {
      //孤になる方の角度
      const s32 dec1 = System::GflUse::GetPublicRand( mSetupParam.emitterEndAngle - mSetupParam.emitterStartAngle ) + mSetupParam.emitterStartAngle;
      //孤にならない方の角度
      const s32 dec2 = System::GflUse::GetPublicRand(180)-90;
      
      switch( mSetupParam.emitterAxis )
      {
      case POS_AXIS_X: //X軸
        ofs.Set(0,0,1.0f);
        ObjectRot_Deg( ofs.z , ofs.y , dec1 );
        ObjectRot_Deg( ofs.x , ofs.z , dec2 );
        break;
      case POS_AXIS_Y: //Y軸
        ofs.Set(1.0f,0,0);
        ObjectRot_Deg( ofs.x , ofs.z , dec1 );
        ObjectRot_Deg( ofs.y , ofs.x , dec2 );
        break;
      case POS_AXIS_Z: //Z軸
        ofs.Set(0,1.0f,0);
        ObjectRot_Deg( ofs.y , ofs.x , dec1 );
        ObjectRot_Deg( ofs.z , ofs.y , dec2 );
        break;
      }
      ofs *= lenRate;
      ofs = ofs.MulPerElem( scl );
      
    }
    break;
  case EMITTER_CYLINDER1: //円柱(基準は底面
  case EMITTER_CYLINDER2: //円柱(基準は中央
    {
      //孤になる方の角度
      const s32 dec1 = System::GflUse::GetPublicRand( mSetupParam.emitterEndAngle - mSetupParam.emitterStartAngle ) + mSetupParam.emitterStartAngle;

      switch( mSetupParam.emitterAxis )
      {
      case POS_AXIS_X: //X軸
        ofs.Set(0,0,1.0f);
        ObjectRot_Deg( ofs.z , ofs.y , dec1 );
        break;
      case POS_AXIS_Y: //Y軸
        ofs.Set(1.0f,0,0);
        ObjectRot_Deg( ofs.x , ofs.z , dec1 );
        break;
      case POS_AXIS_Z: //Z軸
        ofs.Set(0,1.0f,0);
        ObjectRot_Deg( ofs.y , ofs.x , dec1 );
        break;
      }
      ofs *= lenRate;
      ofs = ofs.MulPerElem( scl );
      
      //柱の分の補正
      if( mSetupParam.emitterType == EMITTER_CYLINDER1 )
      {
        //円柱(基準は底面
        switch( mSetupParam.emitterAxis )
        {
        case POS_AXIS_X: //X軸
          ofs.x = System::GflUse::GetPublicRand( (u32)(scl.x*100.0f) )*0.01f;
          break;
        case POS_AXIS_Y: //Y軸
          ofs.y = System::GflUse::GetPublicRand( (u32)(scl.y*100.0f) )*0.01f;
          break;
        case POS_AXIS_Z: //Z軸
          ofs.z = System::GflUse::GetPublicRand( (u32)(scl.z*100.0f) )*0.01f;
          break;
        }
      }
      else
      {
        //円柱(基準は中央
        switch( mSetupParam.emitterAxis )
        {
        case POS_AXIS_X: //X軸
          ofs.x = System::GflUse::GetPublicRand( (u32)(scl.x*100.0f) )*0.01f - scl.x/2;
          break;
        case POS_AXIS_Y: //Y軸
          ofs.y = System::GflUse::GetPublicRand( (u32)(scl.y*100.0f) )*0.01f - scl.y/2;
          break;
        case POS_AXIS_Z: //Z軸
          ofs.z = System::GflUse::GetPublicRand( (u32)(scl.z*100.0f) )*0.01f - scl.z/2;
          break;
        }
      }
    }
    break;
  case EMITTER_BOX: //箱
    {
      const u32 axisRand = System::GflUse::GetPublicRand((u32)(scl.x+scl.y+scl.z));
      if( axisRand < scl.x )
      {
        ofs.x = scl.x/2 * lenRate;
        ofs.y = System::GflUse::GetPublicRand((u32)scl.y*100.0f/2)*0.01f;
        ofs.z = System::GflUse::GetPublicRand((u32)scl.z*100.0f/2)*0.01f;
      }
      else
      if( axisRand < scl.x+scl.y )
      {
        ofs.x = System::GflUse::GetPublicRand((u32)scl.x*100.0f/2)*0.01f;
        ofs.y = scl.y/2 * lenRate;
        ofs.z = System::GflUse::GetPublicRand((u32)scl.z*100.0f/2)*0.01f;
      }
      else
      {
        ofs.x = System::GflUse::GetPublicRand((u32)scl.x*100.0f/2)*0.01f;
        ofs.y = System::GflUse::GetPublicRand((u32)scl.y*100.0f/2)*0.01f;
        ofs.z = scl.z/2 * lenRate;
      }
      //中心からどっち側に出るか？
      if( System::GflUse::GetPublicRand(2) == 0 )
      {
        ofs.x *= -1;
      }
      if( System::GflUse::GetPublicRand(2) == 0 )
      {
        ofs.y *= -1;
      }
      if( System::GflUse::GetPublicRand(2) == 0 )
      {
        ofs.z *= -1;
      }
    }
    break;
  }
  
  //回す。
  ObjectRotVector3_Deg(ofs,spawnObj->parent->GetRotate());

  pos += ofs;
  spawnObj->obj->SetPosition( pos );
}


void Object_Cluster::SpawnCore_SetStartSpeed(SpawnObjectData *spawnObj)
{
  switch(mSetupParam.spdType)
  {
  case SPEED_NONE:
    //none func
    break;
  case SPEED_DIFFUSE:
    {
      gfl2::math::Vector3 basePos = spawnObj->parent->GetPosition() + mSetupParam.spdVec;
      gfl2::math::Vector3 mdlPos = spawnObj->obj->GetPosition();
      gfl2::math::Vector3 ofsPos = mdlPos - basePos;
      //基準軸(面)で不要な速度を切る
      switch( mSetupParam.spdAxis )
      {
      case POS_ORIGIN:
        //none func
        break;
      case POS_AXIS_X:
        ofsPos.x = 0;
        break;
      case POS_AXIS_Y:
        ofsPos.y = 0;
        break;
      case POS_AXIS_Z:
        ofsPos.z = 0;
        break;
      case POS_PLANE_X:
        ofsPos.y = 0;
        ofsPos.z = 0;
        break;
      case POS_PLANE_Y:
        ofsPos.x = 0;
        ofsPos.z = 0;
        break;
      case POS_PLANE_Z:
        ofsPos.x = 0;
        ofsPos.y = 0;
        break;
      }
      //正規化
      if( ofsPos != gfl2::math::Vector3::GetZero() )
      {
        spawnObj->dir = ofsPos.Normalize();
      }
      else
      {
        spawnObj->dir = gfl2::math::Vector3::GetZero();
      }
    }
    break;
  case SPEED_DIR:
    {
      //正規化
      if( mSetupParam.spdVec != gfl2::math::Vector3::GetZero() )
      {
        spawnObj->dir = mSetupParam.spdVec.Normalize();
      }
      else
      {
        spawnObj->dir = gfl2::math::Vector3::GetZero();
      }
      //回す。
      ObjectRotVector3_Deg(spawnObj->dir,spawnObj->parent->GetRotate());

      //方向指定
      if( mSetupParam.spdDecRand != 0 )
      {
        f32 rotX = (f32)System::GflUse::GetPublicRand(mSetupParam.spdDecRand*20+1)-mSetupParam.spdDecRand*10;
        f32 rotY = (f32)System::GflUse::GetPublicRand(mSetupParam.spdDecRand*20+1)-mSetupParam.spdDecRand*10;
        f32 rotZ = (f32)System::GflUse::GetPublicRand(mSetupParam.spdDecRand*20+1)-mSetupParam.spdDecRand*10;
        
        gfl2::math::Vector3 baseDir(0,0,0);

        baseDir.x = gfl2::math::SinDeg( rotX*0.1f );
        baseDir.y = gfl2::math::SinDeg( rotY*0.1f );
        baseDir.z = gfl2::math::SinDeg( rotZ*0.1f );

        //回す。
        ObjectRotVector3_Rad(spawnObj->dir,baseDir);
      }
    }
    break;
  }

  spawnObj->spd = mSetupParam.spdSpd;
  spawnObj->accelSpd = mSetupParam.spdAcc;
}

//初期設定その他(回転・拡縮)
inline f32 SpauwModelCalcFunc( f32 min , f32 max )
{
  //ランダム幅の計算関数
  const f32 ofs = max - min;
  if( ofs )
  {
    return System::GflUse::GetPublicRand((u32)(ofs*100))*0.01f + min;
  }
  else
  {
    return min;
  }
}
void Object_Cluster::SpawnCore_SetStartParam(SpawnObjectData *spawnObj)
{
  gfl2::math::Vector3 scl,rot;
  const gfl2::math::Vector3 diffScl = mSetupParam.initSclMax - mSetupParam.initSclMin;
  const gfl2::math::Vector3 diffRot = mSetupParam.initRotMax - mSetupParam.initRotMin;
  
  scl.x = SpauwModelCalcFunc( mSetupParam.initSclMin.x , mSetupParam.initSclMax.x );
  if( mSetupParam.syncInitScl )
  {
    scl.y = scl.x;
    scl.z = scl.x;
  }
  else
  {
    scl.y = SpauwModelCalcFunc( mSetupParam.initSclMin.y , mSetupParam.initSclMax.y );
    scl.z = SpauwModelCalcFunc( mSetupParam.initSclMin.z , mSetupParam.initSclMax.z );
  }
                                                      
  rot.x = SpauwModelCalcFunc( mSetupParam.initRotMin.x , mSetupParam.initRotMax.x );
  rot.y = SpauwModelCalcFunc( mSetupParam.initRotMin.y , mSetupParam.initRotMax.y );
  rot.z = SpauwModelCalcFunc( mSetupParam.initRotMin.z , mSetupParam.initRotMax.z );
 

  rot += spawnObj->parent->GetRotate();
  //以前はAdjustに入れていたが、子に引き継がれてしまうので、別で取っておく
  spawnObj->randScale = scl;
  spawnObj->randRotate = rot;

  spawnObj->obj->SetRotate( spawnObj->randRotate );
  spawnObj->obj->SetScale( spawnObj->randScale );
}

//オブジェクト更新処理
void Object_Cluster::UpdateObject(SpawnObjectData *spawnObj)
{
  if( mSetupParam.isFollowParent &&
      spawnObj->parent )
  {
    //追従の場合は移動処理を行わない
    spawnObj->obj->SetPosition(spawnObj->parent->GetPosition());
  }
  else
  {
    //追従しない場合
    //速度の更新
    gfl2::math::Vector3 pos = spawnObj->obj->GetPosition();
    gfl2::math::Vector3 befPos = pos;
    gfl2::math::Vector3 movePos;

    movePos = spawnObj->dir * spawnObj->spd;
  
    pos += movePos;
    spawnObj->obj->SetPosition(pos);
  
    //加速処理
    switch( mSetupParam.accelType )
    {
    case ACCEL_NONE:    //なし
      //no func
      break;
    case ACCEL_GRAVITY: //重力
      {
        gfl2::math::Vector3 basePos = mSetupParam.accelPos;
        if( mSetupParam.accelIsAbsolute == false )
        {
          if( spawnObj->parent ) // @fix cov_ctr[10131]spawnObj->parentはNULLになる可能性があるため
          {
            //相対
            basePos += spawnObj->parent->GetPosition();
          }
        }
        //加速量の計算
        gfl2::math::Vector3 mdlPos = spawnObj->obj->GetPosition();
        gfl2::math::Vector3 ofsPos = basePos - mdlPos;
        gfl2::math::Vector3 nowSpd = movePos;
        switch( mSetupParam.accelAxis )
        {
        case POS_ORIGIN:
          //none func
          break;
        case POS_AXIS_X:
          ofsPos.x = 0;
          break;
        case POS_AXIS_Y:
          ofsPos.y = 0;
          break;
        case POS_AXIS_Z:
          ofsPos.z = 0;
          break;
        case POS_PLANE_X:
          ofsPos.y = 0;
          ofsPos.z = 0;
          break;
        case POS_PLANE_Y:
          ofsPos.x = 0;
          ofsPos.z = 0;
          break;
        case POS_PLANE_Z:
          ofsPos.x = 0;
          ofsPos.y = 0;
          break;
        }
        //正規化
        if( ofsPos != gfl2::math::Vector3::GetZero() )
        {
          ofsPos = ofsPos.Normalize();
        }
        else
        {
          ofsPos = gfl2::math::Vector3::GetZero();
        }

        ofsPos *= mSetupParam.accelRate;
      
        nowSpd += ofsPos;
        spawnObj->spd = nowSpd.Length();

        //正規化
        if( nowSpd != gfl2::math::Vector3::GetZero() )
        {
          spawnObj->dir = nowSpd.Normalize();
        }
        else
        {
          spawnObj->dir = gfl2::math::Vector3::GetZero();
        }
      }
    
      break;
    }
  
    //加速度追加
    if( spawnObj->spd != 0 )
    {
      spawnObj->spd *= spawnObj->accelSpd;
    }
  
    //反射処理
    UpdateObject_Refrect( spawnObj , befPos );
  }
  
  //スケールアニメ
  gfl2::math::Vector3 scl;
  bool retS = ValueAnimeFunc( mSetupParam.animScaleFrame , mSetupParam.animScaleValue , spawnObj->cnt , &scl );
  if( retS )
  {
    scl = scl.MulPerElem( spawnObj->randScale );
    spawnObj->obj->SetScale( scl );
  }
  //回転アニメ
  gfl2::math::Vector3 rot;
  bool retR = ValueAnimeFunc( mSetupParam.animRotateFrame , mSetupParam.animRotateValue , spawnObj->cnt , &rot );
  if( retR )
  {
    spawnObj->obj->SetRotate( rot + spawnObj->randRotate );
  }
    
  //子の生成判定
  for( int i=0;i<CHILD_CLUSTER_NUM;i++ )
  {
    if( mChildClusterArr[i] &&
        mChildClusterArr[i]->GetSpawnType() == SPAWN_TIME )
    {
      mChildClusterArr[i]->CheckSpawn_Time(spawnObj->cnt,spawnObj->obj);
    }
  }

  spawnObj->cnt++;
  if( spawnObj->cnt > mSetupParam.childLifeTime )
  {
    DeleteObject(spawnObj);
  }
}

bool Object_Cluster::ValueAnimeFunc( int *frameArr , gfl2::math::Vector3 *vecArr , int cnt, gfl2::math::Vector3 *ret )
{
  for( int i=0;i<ANIME_PARAM_NUM-1;i++ )
  {
    if( frameArr[i] == -1 )
    {
      return false;
    }
    else
    if( frameArr[i+1] == -1 )
    {
      *ret = vecArr[i];
      return true;
    }
    else
    if( frameArr[i] <= cnt &&
        frameArr[i+1] > cnt )
    {
      f32 rate = (f32)(cnt-frameArr[i])/(f32)(frameArr[i+1]-frameArr[i]);
      gfl2::math::Vector3 ofsVec = (vecArr[i+1])-(vecArr[i]);
      ofsVec *= rate;
      *ret = ofsVec + (vecArr[i]);
      return true;
    }
  }
  if( frameArr[ANIME_PARAM_NUM-1] < cnt )
  {
    *ret = vecArr[ANIME_PARAM_NUM-1];
    return true;
  }
  return false;
}

//オブジェクト更新処理（反射処理
void Object_Cluster::UpdateObject_Refrect(SpawnObjectData *spawnObj , gfl2::math::Vector3 &befPos )
{
  if( mRefrectParam.type != REFRECT_NONE )
  {
    gfl2::math::Vector3 pos = spawnObj->obj->GetPosition();
    f32 *befY;
    f32 *nowY;
    
    f32 planePos = mRefrectParam.pos;
    //反射面計算用に値を入れ替える
    switch( mRefrectParam.axis )
    {
    case POS_PLANE_X:
      befY = &befPos.x;
      nowY = &pos.x;
      if( mRefrectParam.isAbs == false )
      {
        if( spawnObj->parent ) // @fix cov_ctr[10131]spawnObj->parentはNULLになる可能性があるため
        {
          planePos += spawnObj->parent->GetPosition().x;
        }
      }
      break;
    default:  //KlockWork対応念のため
    case POS_PLANE_Y:
      befY = &befPos.y;
      nowY = &pos.y;
      if( mRefrectParam.isAbs == false )
      {
        planePos += spawnObj->parent->GetPosition().y;
      }
      break;
    case POS_PLANE_Z:
      befY = &befPos.z;
      nowY = &pos.z;
      if( mRefrectParam.isAbs == false )
      {
        planePos += spawnObj->parent->GetPosition().z;
      }
      break;
    }
    
    //反射判定
    if( (*befY < planePos && *nowY >= planePos )||
        (*befY > planePos && *nowY <= planePos ) )
    {
      const f32 ofs = planePos - *nowY;
      *nowY  = planePos + ofs;
      
      switch( mRefrectParam.axis )
      {
      case POS_PLANE_X:
        spawnObj->dir.x *= -1;
        break;
      case POS_PLANE_Y:
        spawnObj->dir.y *= -1;
        break;
      case POS_PLANE_Z:
        spawnObj->dir.z *= -1;
        break;
      }

      //反射時挙動
      switch( mRefrectParam.type )
      {
      case REFRECT_NORMAL:  //反射
        {
          f32 tempSpd = spawnObj->spd * mRefrectParam.brake;
          spawnObj->spd -= tempSpd;
        }
        break;
      case REFRECT_STOP:    //停止
        spawnObj->spd = 0;
        //位置を面に合わせておく
        switch( mRefrectParam.axis )
        {
        case POS_PLANE_X:
          pos.x = planePos;
          break;
        default:  //KlockWork対応念のため
        case POS_PLANE_Y:
          pos.y = planePos;
          break;
        case POS_PLANE_Z:
          pos.z = planePos;
          break;
        }
        break;
      case REFRECT_DEATH:   //死亡
        spawnObj->cnt = mSetupParam.childLifeTime+1;
        spawnObj->spd = 0;
        spawnObj->accelSpd = 0;
        //位置を面に合わせておく
        switch( mRefrectParam.axis )
        {
        case POS_PLANE_X:
          pos.x = planePos;
          break;
        default:  //KlockWork対応念のため
        case POS_PLANE_Y:
          pos.y = planePos;
          break;
        case POS_PLANE_Z:
          pos.z = planePos;
          break;
        }
        break;
      }

      spawnObj->obj->SetPosition(pos);

      //子の生成判定
      for( int i=0;i<CHILD_CLUSTER_NUM;i++ )
      {
        if( mChildClusterArr[i] &&
            mChildClusterArr[i]->GetSpawnType() == SPAWN_REFRECT )
        {
          mChildClusterArr[i]->SpawnObject(spawnObj->obj);
        }
      }
      
    }
  }
}

//オブジェクト破棄処理
void Object_Cluster::DeleteObject(SpawnObjectData *spawnObj)
{
  if( spawnObj->isCreateChild == false )
  {
    spawnObj->isCreateChild = true;
    //子の生成判定
    for( int i=0;i<CHILD_CLUSTER_NUM;i++ )
    {
      if( mChildClusterArr[i] )
      {
        if( mChildClusterArr[i]->GetSpawnType() == SPAWN_DEATH )
        {
          mChildClusterArr[i]->SpawnObject(spawnObj->obj);
        }
        //追従の親になっていないか確認する
        mChildClusterArr[i]->CheckFollowChild(spawnObj->obj);
      }
    }
  }


  if( mObjType == OBJECT_TYPE_MODEL )
  {
    DeleteObject_Model(spawnObj);
  }
  else
  {
    DeleteObject_Effect(spawnObj);
  }
}


//オブジェクト破棄処理(モデル
void Object_Cluster::DeleteObject_Model(SpawnObjectData *spawnObj)
{
  if( spawnObj->objMdl->IsAddRenderPath() )
  {
    spawnObj->objMdl->RemoveRenderPath();
  }
  else
  {
    mSetupParam.objManager->RemoveObject(spawnObj->obj);

    spawnObj->objMdl->Delete();
    GFL_SAFE_DELETE(spawnObj->objMdl);
    spawnObj->obj = NULL;
    spawnObj->isEnable = false;
  }
}

//オブジェクト破棄処理(エフェクト
void Object_Cluster::DeleteObject_Effect(SpawnObjectData *spawnObj)
{
  if( spawnObj->objEff->IsAddRenderPath() )
  {
    spawnObj->objEff->RemoveRenderPath();
  }
  else
  {
    mSetupParam.objManager->RemoveObject(spawnObj->obj);

    spawnObj->objEff->Delete();
    GFL_SAFE_DELETE(spawnObj->objEff);
    spawnObj->obj = NULL;
    spawnObj->isEnable = false;
  }
}

//親子の設定
void Object_Cluster::SetParent(Object_Cluster *obj)
{
  mParentCluster = obj;
}
void Object_Cluster::SetChild(Object_Cluster *obj)
{
  for( int i=0;i<CHILD_CLUSTER_NUM;i++ )
  {
    if( mChildClusterArr[i] == NULL )
    {
      mChildClusterArr[i] = obj;
      return;
    }
  }
  GFL_ASSERT_MSG(0,"Object_Cluster この設定に失敗しました");
}

//追従の親が死ぬ時呼ばれて、親をはずす
void Object_Cluster::CheckFollowChild(Object *obj)
{
  if( mSetupParam.isFollowParent )
  {
    for( int i=0;i<mSetupParam.objectMax;i++ )
    {
      if( mSpawnObjArr[i].isEnable )
      {
        if( mSpawnObjArr[i].parent == obj )
        {
          mSpawnObjArr[i].parent = NULL;
        }
      }
    }
  }
}


GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)