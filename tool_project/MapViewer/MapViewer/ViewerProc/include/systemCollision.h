#pragma once
//=================================================================================================
/**
 *  @file  systemCollision.h
 *  @brief  �R���W�����V�X�e��
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
//! @brief �R���W�����V�X�e��
//----------------------------------------------------------------------------------------
class systemCollision
{
public:

  //----------------------------------------------------------------------------
  /**
  *  @brief  �V���O���g���C���X�^���X�̎擾
  *    
  *  @return �C���X�^���X
  */
  //-----------------------------------------------------------------------------
  static systemCollision* systemCollision::getInstance() 
  {
    static systemCollision instance;
    return &instance;
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �R���X�g���N�^
  */
  //-----------------------------------------------------------------------------
  systemCollision();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �f�X�g���N�^
  */
  //-----------------------------------------------------------------------------
  ~systemCollision();

  //----------------------------------------------------------------------------
  /**
  *  @brief  �Փ˂����A�N�^�[�̒ǉ�
  *
  *  @param  pActor  �R���W�����V�X�e���ɓo�^�������A�N�^�[
  */
  //-----------------------------------------------------------------------------
  b32 registStaticActor(fld::mmodel::MoveModel *pActor,
                        void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*)
                       );

  //----------------------------------------------------------------------------
  /**
  *  @brief  �Փ˂����n�`�̒ǉ�
  *
  *  @param  pCollisionModel  �n�`�R���W����
  */
  //-----------------------------------------------------------------------------
  b32 registTerrain(gfl2::collision::CollisionModel *pCollisionModel,
                        void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*)
                       );

  //----------------------------------------------------------------------------
  /**
  *  @brief  �Փ˂���A�N�^�[�̒ǉ�
  *
  *  @param  pActor  �R���W�����V�X�e���ɓo�^�������A�N�^�[
  */
  //-----------------------------------------------------------------------------
  b32 registDynamicActor(fld::mmodel::MoveModel *pActor,
                         void (fld::mmodel::MoveModel::*pRayCallbackFunc) (Test::FieldTest::customCollisionCallback_Intersection::HIT_DATA*)
                        );

  //----------------------------------------------------------------------------
  /**
  *  @brief  �S�A�N�^�[�̃��Z�b�g(���t���[����΂�܂�)
  */
  //-----------------------------------------------------------------------------
  void resetActor(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  �R���W��������
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
  collisionData  m_pStaticData [MAX_STATIC_DATA_CNT];   //�Փ˂���鑤
  collisionData  m_pDynamicData[MAX_DYNAMIC_DATA_CNT];  //�Փ˂��鑤
};
