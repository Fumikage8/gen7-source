#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModel_define.h
*  @brief  フィールド用 動作モデルの定義
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
  //! @brief 動作モデル用のID
  //-----------------------------------------------------------------------------
  enum FIELD_MOVE_MODEL_ID
  {
    FIELD_MOVE_MODEL_PLAYER,        ///< プレイヤー用動作モデル
    FIELD_MOVE_MODEL_NPC_START,     ///< プレイヤー以外のワーク
    FIELD_MOVE_MODEL_MAX = 64,      ///< 最大数は64、弄らないでください
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用のセーブされない状態  動作ビット
  //-----------------------------------------------------------------------------
  enum FieldMoveModelStateBit
  {
    FIELD_MOVE_MODEL_STATEBIT_NON                           = (0),      ///< 無し
    FIELD_MOVE_MODEL_STATEBIT_ACMD                          = (1<<1),   ///< アクションコマンド中
    FIELD_MOVE_MODEL_STATEBIT_ACMD_END                      = (1<<2),   ///< アクションコマンドが終了したフレームかどうか
    FIELD_MOVE_MODEL_STATEBIT_EVENT                         = (1<<3),   ///< イベント中(アクションコマンド、動作コードを共に動かさない)
    FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE           = (1<<4),   ///< モーション移動量を無視する
    FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE  = (1<<5),   ///< コリジョンの押し返しを無視する
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用のコリジョンの種類
  //-----------------------------------------------------------------------------
  enum FieldMoveModelCollisionType
  {
    //------ 押し返しを受ける ------
    COLLISION_TYPE_TERRAIN,               ///< 地面との衝突検出用
    COLLISION_TYPE_STATIC_MODEL,          ///< 配置モデルとの衝突検出用

    //------ 押し返しを受けない ------
    COLLISION_TYPE_EVENT_POS,             ///< イベントとの衝突検出用(押し返しを受けない)
    COLLISION_TYPE_EVENT_TALK,            ///< イベントとの衝突検出用(押し返しを受けない)
    COLLISION_TYPE_EVENT_PUSH,            ///< イベントとの衝突検出用(押し返しを受けない)
    COLLISION_TYPE_ENCOUNT,               ///< エンカウント領域との衝突検出用(押し返しを受けない)
    COLLISION_TYPE_MAX,
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル 対 動作モデル用のStaticActor
  //-----------------------------------------------------------------------------
  enum FieldMoveModelStaticCollisionType
  {
    COLLISION_STATIC_TYPE_STATIC_MODEL,   ///< 動作モデル 対 動作モデル用
    COLLISION_STATIC_TYPE_EVENT_TALK,     ///< 会話判定用

    COLLISION_STATIC_TYPE_MAX,
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデルが利用するイベント用アクター向けのデータ構造
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

    b32 m_isInit;         ///< 初期化済みかどうか
    b32 m_isEnable;       ///< イベントを実行できるかどうか
    u32 m_eventId;        ///< イベントID
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用の作成リクエストデータ
  //  @note  動作モデル用ヘッダーにはCreateに必要な値のみ定義する
  //-----------------------------------------------------------------------------
  struct FieldMoveModelHeader
  {
    //------------------------------------------------------------------
    /**
    * @brief   コンストラクタ
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
    * @brief   代入
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
    * @brief   比較
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
    * @brief   比較
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
    u32                           eventId;                      //識別用のユニークな値
    BaseCollisionScene*           pCollisionSceneForTerrain;
    BaseCollisionScene*           pCollisionSceneForStaticModel;
    BaseCollisionScene*           pCollisionSceneForEventPos;
    BaseCollisionScene*           pCollisionSceneForEventTalk;
    BaseCollisionScene*           pCollisionSceneForEventPush;
    BaseCollisionScene*           pCollisionSceneForEncount;
  };

}; //end of namespace MoveModel
}; //end of namespace Field
