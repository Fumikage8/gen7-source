//======================================================================
/**
 * @file	btlv_local_def.h
 * @brief	バトル描画ローカル定義
 * @author	ariizumi
 * @data	11/04/21
 */
//======================================================================

#if !defined __BTLV_LOCAL_DEF_H__
#define __BTLV_LOCAL_DEF_H__
#pragma once

#include <math/include/gfl2_math.h>
#include <heap/include/gfl2_Heap.h>

#include "../btl_common.h"
#include "Demolib/Object/include/ObjectManager.h"

GFL_NAMESPACE_BEGIN(btl)

//技エフェクト用
#define BTLV_EFF_PARTICLE_NUM (32)
#define BTLV_EFF_MODEL_NUM (32)
#define BTLV_EFF_SOUND_NUM (6)
#define BTLV_EFF_SOUND_ACTOR_NUM (6)
#define BTLV_EFF_CLUSTER_NUM (5)
#define BTLV_EFF_LAYOUT_NUM (5)

#define BTLV_TARGET_TRAINER_NUM (2)

//ライト番号
#define LIGHTNO_MODEL (0)
#define LIGHTNO_CHARA_TOP (2) //2,3,4,5
#define LIGHTNO_POKEMON_TOP (6) //6,7,8,9

//導入演出時の高さ
static const f32 POKE_INTOR_HEIGHT = 200.0f;

//着地演出
typedef enum
{
  BTLV_LAND_HEAVY,        //200kg以上
  BTLV_LAND_LIGHT_HEAVY,  //100kg以上
  BTLV_LAND_MIDDLE,       //50kg以上
  BTLV_LAND_LIGHT,        //50kg未満
  BTLV_LAND_NONE,         //ふうせん持ちか着地モーションがない
}BTLV_LAND_TYPE;

//計算関数
inline void BtlvRot_Deg( f32 &x , f32 &z , const f32 deg )
{
  f32 sin,cos;
  const f32 wx = x;
  const f32 wz = z;
  gfl2::math::SinCosDeg( &sin,&cos,deg );
  x = wx*cos - wz*sin;
  z = wx*sin + wz*cos;
}

inline void BtlvRot_Rad( f32 &x , f32 &z , const f32 rad )
{
  f32 sin,cos;
  const f32 wx = x;
  const f32 wz = z;
  gfl2::math::SinCosRad( &sin,&cos,rad );
  x = wx*cos - wz*sin;
  z = wx*sin + wz*cos;
}

//コリジョンのヒットチェック
//カメラとの当たり判定用
inline bool CheckHitCollision(const gfl2::math::Vector3 &pos,f32 ofsSize,DemoLib::Obj::Object *obj,const gfl2::math::AABB &aabb)
{
  gfl2::math::AABB box = aabb;
  //BOXは原点基準になっている。
  //回転を加味する場合、矩形をまわして判定はメンドイ。
  //まずカメラの点を原点基準にする。
  //後、ポケモンと同じ回転値を点の方にかけてチェックを行う。

  gfl2::math::Vector3 tempPos;
  obj->GetCalcPosition(&tempPos);
  tempPos = pos-tempPos;
  BtlvRot_Deg( tempPos.x,tempPos.z,obj->GetRotateY() );

  gfl2::math::Vector4 maxVec = box.GetMax();
  gfl2::math::Vector4 minVec = box.GetMin();
  maxVec.x += ofsSize;
  maxVec.y += ofsSize;
  maxVec.z += ofsSize;
  minVec.x -= ofsSize;
  minVec.y -= ofsSize;
  minVec.z -= ofsSize;
  box.SetMax(maxVec);
  box.SetMin(minVec);

  if( box.IsContain(tempPos) )
  {
    return true;
  }
  return false;
}

inline void CalcBoxMaxSize(gfl2::math::AABB &aabb)
{
  //AABBのMAXは必ずプラス、Minはマイナスなチェック
  f32 tempMax = gfl2::math::max(aabb.GetMax().x,aabb.GetMax().z);
  f32 tempMin = gfl2::math::min(aabb.GetMin().x,aabb.GetMin().z);
  tempMin *= -1;
  tempMax = gfl2::math::max(tempMax,tempMin);
  gfl2::math::Vector4 max = aabb.GetMax();
  gfl2::math::Vector4 min = aabb.GetMin();
  max.x = tempMax;
  max.z = tempMax;
  min.x = -tempMax;
  min.z = -tempMax;
  aabb.SetMax(max);
  aabb.SetMin(min);
}

//------------------------------------------------------------------------------------------------------------
//サウンド関係
//------------------------------------------------------------------------------------------------------------
//数値変換用(kujira移植)
#define ADJUST_SE_PITCH(val) (val<0 ? (val+127)*0.5f/127.0f+0.5f : val/127.0f+1.0f)
#define ADJUST_SE_PAN(val) (val/127.0f)
#define ADJUST_SE_TEMPO(val) (val/127.0f)
#define ADJUST_SE_VOL(val) (val/127.0f)

//オブジェクト管理
template<typename T> class BattleObjectManager
{
public:
  BattleObjectManager(gfl2::heap::HeapBase *heap,int objectNum)
  {
    mObjBufNum = objectNum;
    mObjArr = GFL_NEW_ARRAY(heap)T**[mObjBufNum];
    mObjNum = GFL_NEW_ARRAY(heap)int[mObjBufNum];
    mObjId = GFL_NEW_ARRAY(heap)int[mObjBufNum];
  }
  virtual ~BattleObjectManager()
  {
    GFL_SAFE_DELETE_ARRAY(mObjId);
    GFL_SAFE_DELETE_ARRAY(mObjNum);
    GFL_SAFE_DELETE_ARRAY(mObjArr);
  }

  void SetObjectArr( T **objArr , int objNum , int objId )
  {
    for( int i=0;i<mObjBufNum;i++ )
    {
      if( mObjArr[i] == NULL )
      {
        mObjArr[i] = objArr;
        mObjNum[i] = objNum;
        mObjId[i] = objId;
        return;
      }
    }
  }

  T** GetObjectArr( int objId , int *num )const
  {
    const int idx = GetObjectIndex(objId);
    if( idx != -1 )
    {
      if( num != NULL )
      {
        *num = mObjNum[idx];
      }
      return mObjArr[idx];
    }

    if( num != NULL )
    {
      *num = 0;
    }
    return NULL;
  }

  T** GetObjectArrIdx( int idx , int *num )const
  {
    if( idx < mObjBufNum )
    {
      if( num != NULL )
      {
        *num = mObjNum[idx];
      }
      return mObjArr[idx];
    }
    else
    {
      if( num != NULL )
      {
        *num = 0;
      }
      return NULL;
    }
  }

  void ClearBuffer(DemoLib::Obj::ObjectManager *objMng)
  {
    for( int i=0;i<mObjBufNum;i++ )
    {
      if( mObjArr[i] != NULL )
      {
        bool isEnable = false;
        for( int ii=0;ii<mObjNum[i];ii++ )
        {
          if( mObjArr[i][ii]->IsEnable() == false )
          {
            objMng->RemoveObject(mObjArr[i][ii]);
            GFL_SAFE_DELETE(mObjArr[i][ii]);
          }
          else
          {
            isEnable = true;
          }
        }
        if( isEnable == false )
        {
          GFL_SAFE_DELETE_ARRAY( mObjArr[i] );
        }
      }
    }
  }

  void ClearBufferId(DemoLib::Obj::ObjectManager *objMng,int objId)
  {
    const int idx = GetObjectIndex(objId);
    if( idx != -1 )
    {
      if( mObjArr[idx] != NULL )
      {
        bool isEnable = false;
        for( int ii=0;ii<mObjNum[idx];ii++ )
        {
          if( mObjArr[idx][ii]->IsEnable() == false )
          {
            objMng->RemoveObject(mObjArr[idx][ii]);
            GFL_SAFE_DELETE(mObjArr[idx][ii]);
          }
          else
          {
            isEnable = true;
          }
        }
        if( isEnable == false )
        {
          GFL_SAFE_DELETE_ARRAY( mObjArr[idx] );
        }
      }
    }
  }

  //エフェクトのIDに使う
  int GetObjectIndex( int objId )const
  {
    for( int i=0;i<mObjBufNum;i++ )
    {
      if( mObjArr[i] != NULL &&
        mObjId[i] == objId )
      {
        return i;
      }
    }
    return -1;
  }

private:
  //ポインタの配列を格納する配列・・・
  T*** mObjArr;
  int* mObjNum;
  int* mObjId;

  int mObjBufNum;
};

//ポケモン相対移動用オプションセット
typedef struct 
{
  BtlvPos vpos;     //エフェクト対象
  BtlvPos vposSub;  //エフェクトの関係者
  int     node;

  gfl2::math::Vector3 offset;
  int rate;

  int adjustFlip;
  int adjustScale;
  int autoRotateModel;
  int autoRotatePos;
  bool forceUpdate;

}BTLV_RELARIVE_POKE_OPTION;

GFL_NAMESPACE_END(btl)


#endif //__BTLV_LOCAL_DEF_H__
