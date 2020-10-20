﻿//=================================================================================================
/**
 *  @file  systemCollision.cpp
 *  @brief
 *  @author  yy-ikeuchi@gamefreak.co.jp
 *  @date  2015.02.17
 */
//=================================================================================================
#include "../include/movemodel.h"
#include "../include/field_transform.h"
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include "System/include/GflUse.h"
#include "../include/chara_rsc_manager.h"
#include "../include/chara_define.h"
#include "../include/SimpleCollisionCallbackable.h"

#include <Collision/include/gfl2_CollisionModel.h>

#include "../include/systemCollision.h"

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
systemCollision::systemCollision()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
systemCollision::~systemCollision()
{
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突されるアクターの追加
*
*  @param  pActor  コリジョンシステムに登録したいアクター
*/
//-----------------------------------------------------------------------------
b32 systemCollision::registStaticActor(fld::mmodel::MoveModel *pActor,void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*) )
{
  u32 index = 0;

  // 多重登録防止
  for( ; index<MAX_STATIC_DATA_CNT ; index++ )
  {
    if( m_pStaticData[index].pActor == pActor )
    {
      return false;
    }
  }

  index = 0;

  // 空きがあるかどうか
  for( ; index<MAX_STATIC_DATA_CNT ; index++ )
  {
    if( m_pStaticData[index].isFree() )
    {
      break;
    }
  }
  if( index == MAX_STATIC_DATA_CNT )
  {
    return false;
  }

  m_pStaticData[index].pActor           = pActor;
  m_pStaticData[index].pCollisionModel  = pActor->GetStaticCollision();
  m_pStaticData[index].pRayCallbackFunc = pRayCallbackFunc;
  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突される地形の追加
*
*  @param  pCollisionModel  地形コリジョン
*/
//-----------------------------------------------------------------------------
b32 systemCollision::registTerrain(gfl2::collision::CollisionModel *pCollisionModel,
                                   void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*)
                                   )
{
   u32 index = 0;

  // 多重登録防止
  for( ; index<MAX_TERRAIN_DATA_CNT ; index++ )
  {
    if( m_pTerrainData[index].pCollisionModel == pCollisionModel )
    {
      return false;
    }
  }

  index = 0;

  // 空きがあるかどうか
  for( ; index<MAX_TERRAIN_DATA_CNT ; index++ )
  {
    if( m_pTerrainData[index].isFree() )
    {
      break;
    }
  }
  if( index == MAX_TERRAIN_DATA_CNT )
  {
    return false;
  }

  m_pTerrainData[index].pCollisionModel  = pCollisionModel;
  m_pTerrainData[index].pRayCallbackFunc = pRayCallbackFunc;
  return true;
}


//----------------------------------------------------------------------------
/**
*  @brief  衝突するアクターの追加
*
*  @param  pActor  コリジョンシステムに登録したいアクター
*/
//-----------------------------------------------------------------------------
b32 systemCollision::registDynamicActor(fld::mmodel::MoveModel *pActor,void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*) )
{
  u32 index = 0;

  // 多重登録防止
  for( ; index<MAX_DYNAMIC_DATA_CNT ; index++ )
  {
    if( m_pDynamicData[index].pActor == pActor )
    {
      return false;
    }
  }

  index = 0;

  // 空きがあるかどうか
  for( ; index<MAX_DYNAMIC_DATA_CNT ; index++ )
  {
    if( m_pDynamicData[index].isFree() )
    {
      break;
    }
  }
  if( index == MAX_DYNAMIC_DATA_CNT )
  {
    return false;
  }

  m_pDynamicData[index].pActor           = pActor;
  m_pDynamicData[index].pCollisionModel  = NULL;
  m_pDynamicData[index].pRayCallbackFunc = pRayCallbackFunc;
  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  全アクターのリセット(毎フレームよばれます)
*/
//-----------------------------------------------------------------------------
void systemCollision::resetActor(void)
{
  for( u32 index=0 ; index<MAX_STATIC_DATA_CNT ; index++ )
  {
    m_pStaticData[index].pActor           = NULL;
    m_pStaticData[index].pCollisionModel  = NULL;
  }
  for( u32 index=0 ; index<MAX_DYNAMIC_DATA_CNT ; index++ )
  {
    m_pDynamicData[index].pActor          = NULL;
    m_pDynamicData[index].pCollisionModel = NULL;
  }
  for( u32 index=0 ; index<MAX_TERRAIN_DATA_CNT ; index++ )
  {
    m_pTerrainData[index].pCollisionModel = NULL;
  }

}

//----------------------------------------------------------------------------
/**
*  @brief  コリジョン判定
*/
//-----------------------------------------------------------------------------
void systemCollision::update(void)
{
  //---------------------------------------------------
  // レイキャストを利用した床判定
  //---------------------------------------------------

  for(u32 i=0 ; i<MAX_DYNAMIC_DATA_CNT ; i++ )
  {
    if( m_pDynamicData[i].isFree() )
    {
      continue;
    }

    // 当たり判定に使用する位置を取得
    const gfl2::math::Matrix& worldMatrix = m_pDynamicData[i].pActor->GetTransform()->GetWorldMatrix();
    gfl2::math::Vector pos = worldMatrix.GetElemPosition();

    // レイキャスト用コールバッククラスを作成
    gfl2::math::Vector start = pos + gfl2::math::Vector(0.0f,10000.0f,0.0f);
    gfl2::math::Vector end   = pos - gfl2::math::Vector(0.0f,10000.0f,0.0f);
    Test::FieldTest::customCollisionCallback_Intersection callbackIntersection;

    for(u32 j=0 ; j<MAX_STATIC_DATA_CNT ; j++ )
    {
      if( m_pStaticData[j].isFree() )
      {
        continue;
      }

      m_pStaticData[j].pCollisionModel->GetIntersectionEx(start,end,NULL,&callbackIntersection);
    }

    for(u32 j=0 ; j<MAX_TERRAIN_DATA_CNT ; j++ )
    {
      if( m_pTerrainData[j].isFree() )
      {
        continue;
      }

      m_pTerrainData[j].pCollisionModel->GetIntersectionEx(start,end,NULL,&callbackIntersection);
    }

    // 取得した一番近い接点から高さを取得します
    if( callbackIntersection.IsHit() )
    {
      Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA hitData = callbackIntersection.GetHitData();
      gfl2::math::Vector newPos = worldMatrix.GetElemPosition();
      newPos.y = hitData.intersection.y;
      m_pDynamicData[i].pActor->GetTransform()->SetTranslation(newPos);

      // 衝突したアクターのコールバック
      if( m_pDynamicData[i].pRayCallbackFunc )
      {
        (m_pDynamicData[i].pActor->*m_pDynamicData[i].pRayCallbackFunc)( &hitData );
      }
      // 衝突されるアクターのコールバック
      if( m_pStaticData[i].pRayCallbackFunc )
      {
        (m_pStaticData[i].pActor->*m_pStaticData[i].pRayCallbackFunc)( &hitData );
      }
    }
  }

  //---------------------------------------------------
  // 各アクターが持つシェイプを利用したあたり判定
  //---------------------------------------------------

}
