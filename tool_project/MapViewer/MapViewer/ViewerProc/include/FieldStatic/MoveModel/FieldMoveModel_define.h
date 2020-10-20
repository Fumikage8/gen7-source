#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModel_define.h
*  @brief  �t�B�[���h�p ���샂�f���̒�`
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <types/include/gfl2_Typedef.h>
#include "../MoveCode/FieldMoveCode_define.h"

class BaseCollisionScene;

namespace poke_3d{ namespace model {
  struct DressUpParam;
}}

namespace Field{ namespace MoveModel {

  //-----------------------------------------------------------------------------
  //! @brief ���샂�f���p��ID
  //-----------------------------------------------------------------------------
  enum FIELD_MOVE_MODEL_ID
  {
    FIELD_MOVE_MODEL_PLAYER,        ///< �v���C���[�p���샂�f��
    FIELD_MOVE_MODEL_NPC_START,     ///< �v���C���[�ȊO�̃��[�N
    FIELD_MOVE_MODEL_MAX = 64,      ///< �ő吔��64�A�M��Ȃ��ł�������
  };

  //-----------------------------------------------------------------------------
  //! @brief ���샂�f���p�̃Z�[�u����Ȃ����  ����r�b�g
  //-----------------------------------------------------------------------------
  enum FieldMoveModelStateBit
  {
    FIELD_MOVE_MODEL_STATEBIT_NON                           = (0),      ///< ����
    FIELD_MOVE_MODEL_STATEBIT_ACMD                          = (1<<1),   ///< �A�N�V�����R�}���h��
    FIELD_MOVE_MODEL_STATEBIT_ACMD_END                      = (1<<2),   ///< �A�N�V�����R�}���h���I�������t���[�����ǂ���
    FIELD_MOVE_MODEL_STATEBIT_EVENT                         = (1<<3),   ///< �C�x���g��(�A�N�V�����R�}���h�A����R�[�h�����ɓ������Ȃ�)
    FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE           = (1<<4),   ///< ���[�V�����ړ��ʂ𖳎�����
    FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE  = (1<<5),   ///< �R���W�����̉����Ԃ��𖳎�����
  };

  //-----------------------------------------------------------------------------
  //! @brief ���샂�f���p�̃R���W�����̎��
  //-----------------------------------------------------------------------------
  enum FieldMoveModelCollisionType
  {
    //------ �����Ԃ����󂯂� ------
    COLLISION_TYPE_TERRAIN,               ///< �n�ʂƂ̏Փˌ��o�p
    COLLISION_TYPE_STATIC_MODEL,          ///< �z�u���f���Ƃ̏Փˌ��o�p

    //------ �����Ԃ����󂯂Ȃ� ------
    COLLISION_TYPE_EVENT_POS,             ///< �C�x���g�Ƃ̏Փˌ��o�p(�����Ԃ����󂯂Ȃ�)
    COLLISION_TYPE_EVENT_TALK,            ///< �C�x���g�Ƃ̏Փˌ��o�p(�����Ԃ����󂯂Ȃ�)
    COLLISION_TYPE_EVENT_PUSH,            ///< �C�x���g�Ƃ̏Փˌ��o�p(�����Ԃ����󂯂Ȃ�)
    COLLISION_TYPE_ENCOUNT,               ///< �G���J�E���g�̈�Ƃ̏Փˌ��o�p(�����Ԃ����󂯂Ȃ�)
    COLLISION_TYPE_MAX,
  };

  //-----------------------------------------------------------------------------
  //! @brief ���샂�f�� �� ���샂�f���p��StaticActor
  //-----------------------------------------------------------------------------
  enum FieldMoveModelStaticCollisionType
  {
    COLLISION_STATIC_TYPE_STATIC_MODEL,   ///< ���샂�f�� �� ���샂�f���p
    COLLISION_STATIC_TYPE_EVENT_TALK,     ///< ��b����p

    COLLISION_STATIC_TYPE_MAX,
  };

  //-----------------------------------------------------------------------------
  //! @brief ���샂�f�������p����C�x���g�p�A�N�^�[�����̃f�[�^�\��
  //-----------------------------------------------------------------------------
  class EventDataForCollision
  {
  public:

    EventDataForCollision() : m_isInit(false),m_isEnable(false) {}

    void Init(u32 eventId)
    {
      if( !m_isInit )
      {
        m_isInit    = true;
        m_isEnable  = true;
        m_eventId   = eventId;
      }
    }

    b32 isEnable(void){ return m_isEnable; }

    void setEnable(b32 isEnable){ m_isEnable = isEnable; }

  private:

    b32 m_isInit;         ///< �������ς݂��ǂ���
    b32 m_isEnable;       ///< �C�x���g�����s�ł��邩�ǂ���
    u32 m_eventId;        ///< �C�x���gID
  };

  //-----------------------------------------------------------------------------
  //! @brief ���샂�f���p�̍쐬���N�G�X�g�f�[�^
  //  @note  ���샂�f���p�w�b�_�[�ɂ�Create�ɕK�v�Ȓl�̂ݒ�`����
  //-----------------------------------------------------------------------------
  struct FieldMoveModelHeader
  {
    //------------------------------------------------------------------
    /**
    * @brief   �R���X�g���N�^
    */
    //------------------------------------------------------------------
    FieldMoveModelHeader()
    {
      this->moveCodeId                    = FIELD_MOVE_CODE_NONE;
      this->characterId                   = 0;
      this->pDressUpParam                 = 0;
      this->eventId                       = 0;
      this->pCollisionSceneForTerrain     = 0;
      this->pCollisionSceneForStaticModel = 0;
      this->pCollisionSceneForEventPos    = 0;
      this->pCollisionSceneForEventTalk   = 0;
      this->pCollisionSceneForEventPush   = 0;
      this->pCollisionSceneForEncount     = 0;
    }

    //------------------------------------------------------------------
    /**
    * @brief   ���
    */
    //------------------------------------------------------------------
    inline FieldMoveModelHeader&  operator = ( const FieldMoveModelHeader&  rhs )
    {
      this->moveCodeId                    = rhs.moveCodeId;
      this->characterId                   = rhs.characterId;
      this->pDressUpParam                 = rhs.pDressUpParam;
      this->eventId                       = rhs.eventId;
      this->pCollisionSceneForTerrain     = rhs.pCollisionSceneForTerrain;
      this->pCollisionSceneForStaticModel = rhs.pCollisionSceneForStaticModel;
      this->pCollisionSceneForEventPos    = rhs.pCollisionSceneForEventPos;
      this->pCollisionSceneForEventTalk   = rhs.pCollisionSceneForEventTalk;
      this->pCollisionSceneForEventPush   = rhs.pCollisionSceneForEventPush;
      this->pCollisionSceneForEncount     = rhs.pCollisionSceneForEncount;
      return *this;
    }

    //------------------------------------------------------------------
    /**
    * @brief   ��r
    */
    //------------------------------------------------------------------
    inline bool operator==(const FieldMoveModelHeader& rhs )
    {
      if( this->moveCodeId                    == rhs.moveCodeId
       && this->characterId                   == rhs.characterId
       && this->pDressUpParam                 == rhs.pDressUpParam
       && this->eventId                       == rhs.eventId
       && this->pCollisionSceneForTerrain     == rhs.pCollisionSceneForTerrain
       && this->pCollisionSceneForStaticModel == rhs.pCollisionSceneForStaticModel
       && this->pCollisionSceneForEventPos    == rhs.pCollisionSceneForEventPos
       && this->pCollisionSceneForEventTalk   == rhs.pCollisionSceneForEventTalk
       && this->pCollisionSceneForEventPush   == rhs.pCollisionSceneForEventPush
       && this->pCollisionSceneForEncount     == rhs.pCollisionSceneForEncount )
      {
        return true;
      }
      return false;
    }

    //------------------------------------------------------------------
    /**
    * @brief   ��r
    */
    //------------------------------------------------------------------
    inline bool operator!=(const FieldMoveModelHeader& rhs )
    {
      if( this->moveCodeId                    == rhs.moveCodeId
       && this->characterId                   == rhs.characterId
       && this->pDressUpParam                 == rhs.pDressUpParam
       && this->eventId                       == rhs.eventId
       && this->pCollisionSceneForTerrain     == rhs.pCollisionSceneForTerrain
       && this->pCollisionSceneForStaticModel == rhs.pCollisionSceneForStaticModel
       && this->pCollisionSceneForEventPos    == rhs.pCollisionSceneForEventPos
       && this->pCollisionSceneForEventTalk   == rhs.pCollisionSceneForEventTalk
       && this->pCollisionSceneForEventPush   == rhs.pCollisionSceneForEventPush
       && this->pCollisionSceneForEncount     == rhs.pCollisionSceneForEncount )
      {
        return false;
      }
      return true;
    }

    FIELD_MOVE_CODE_ID            moveCodeId;
    u32                           characterId;
    poke_3d::model::DressUpParam* pDressUpParam;
    u32                           eventId;                      //���ʗp�̃��j�[�N�Ȓl
    BaseCollisionScene*           pCollisionSceneForTerrain;
    BaseCollisionScene*           pCollisionSceneForStaticModel;
    BaseCollisionScene*           pCollisionSceneForEventPos;
    BaseCollisionScene*           pCollisionSceneForEventTalk;
    BaseCollisionScene*           pCollisionSceneForEventPush;
    BaseCollisionScene*           pCollisionSceneForEncount;
  };

}; //end of namespace MoveModel
}; //end of namespace Field
