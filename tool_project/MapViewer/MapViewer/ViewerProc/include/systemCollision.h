#pragma once
//=================================================================================================
/**
 *  @file  systemCollision.h
 *  @brief  コリジョンシステム
 *  @author  yy-ikeuchi@gamefreak.co.jp
 *  @date  2015.02.17
 *
 */
//=================================================================================================
#include "../include/chara_define.h"
#include "../include/SimpleCollisionCallbackable.h"
#include <types/include/gfl2_Typedef.h>

namespace gfl2{ 
  namespace collision { class CollisionModel;  }
}

namespace fld{ namespace mmodel {
  class MoveModel;
}}

//----------------------------------------------------------------------------------------
//! @brief コリジョンシステム
//----------------------------------------------------------------------------------------
class systemCollision
{
public:

  //----------------------------------------------------------------------------
  /**
  *  @brief  シングルトンインスタンスの取得
  *    
  *  @return インスタンス
  */
  //-----------------------------------------------------------------------------
  static systemCollision* systemCollision::getInstance() 
  {
    static systemCollision instance;
    return &instance;
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  systemCollision();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  ~systemCollision();

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突されるアクターの追加
  *
  *  @param  pActor  コリジョンシステムに登録したいアクター
  */
  //-----------------------------------------------------------------------------
  b32 registStaticActor(fld::mmodel::MoveModel *pActor,
                        void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*)
                       );

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突される地形の追加
  *
  *  @param  pCollisionModel  地形コリジョン
  */
  //-----------------------------------------------------------------------------
  b32 registTerrain(gfl2::collision::CollisionModel *pCollisionModel,
                        void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*)
                       );

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突するアクターの追加
  *
  *  @param  pActor  コリジョンシステムに登録したいアクター
  */
  //-----------------------------------------------------------------------------
  b32 registDynamicActor(fld::mmodel::MoveModel *pActor,
                         void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*)
                        );

  //----------------------------------------------------------------------------
  /**
  *  @brief  全アクターのリセット(毎フレームよばれます)
  */
  //-----------------------------------------------------------------------------
  void resetActor(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  コリジョン判定
  */
  //-----------------------------------------------------------------------------
  void update(void);

private:

  struct collisionData
  {
    collisionData()
    : pActor(NULL)
    , pCollisionModel(NULL)
    , pRayCallbackFunc(NULL)
    {
    }

    b32 isFree(void)
    {
      return pActor ? false:true;
    }

    fld::mmodel::MoveModel          *pActor;
    gfl2::collision::CollisionModel *pCollisionModel;
    void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*);
  };

  struct terrainData
  {
    terrainData()
    : pCollisionModel(NULL)
    , pRayCallbackFunc(NULL)
    {
    }

    b32 isFree(void)
    {
      return pCollisionModel ? false:true;
    }

    gfl2::collision::CollisionModel *pCollisionModel;
    void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*);
  };
  static const int MAX_TERRAIN_DATA_CNT  = 4;
  static const int MAX_STATIC_DATA_CNT  = 8;
  static const int MAX_DYNAMIC_DATA_CNT = 32;

  terrainData    m_pTerrainData [ MAX_TERRAIN_DATA_CNT ]; 
  collisionData  m_pStaticData [MAX_STATIC_DATA_CNT];   //衝突される側
  collisionData  m_pDynamicData[MAX_DYNAMIC_DATA_CNT];  //衝突する側
};
