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
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Quaternion.h>
#include "../MoveCode/FieldMoveCode_define.h"
#include "../Zone/ZoneDataLoaderTypes.h"

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
    FIELD_MOVE_MODEL_PLAYER = 0,                    ///< プレイヤー用動作モデル
    FIELD_MOVE_MODEL_RIDE_POKEMON,                  ///< ライドポケモン用動作モデル
    FIELD_MOVE_MODEL_NPC_START,                     ///< プレイヤー以外のワーク
    FIELD_MOVE_MODEL_NPC_END = 33,                  ///< プレイヤー以外のワーク終了値
    FIELD_MOVE_MODEL_EVENT_START = 34,              ///< イベント用ワーク   @fix GFNMcat[1393] Rotomイベント用にMoveModelワークを予約
    FIELD_MOVE_MODEL_EVENT_END = 34,                ///< イベント用ワーク終了値
    FIELD_MOVE_MODEL_MAX,                           ///< 最大数は32、弄らないでください
  };

  //-----------------------------------------------------------------------------
  //! @brief サブオブジェクト用のID
  //-----------------------------------------------------------------------------
  enum FIELD_SUB_OBJECT_ID
  {
    FIELD_SUB_OBJECT_MANUAL,          ///< プレイヤーの釣竿といった必ず表示させたいサブオブジェクト用のワークID
    FIELD_SUB_OBJECT_AUTO,            ///< 必ず表示させなくともよいサブオブジェクト用のワークID
    FIELD_SUB_OBJECT_MAX = 10,        ///< 最大数は10、弄らないでください
  };

  enum
  {
    FIELD_MOVE_MODEL_SUB_OBJECT_STATIC,   ///< NPCはMoveModelManagerで使用する。PlayerはMoveModel外で使用可能
    FIELD_MOVE_MODEL_SUB_OBJECT_DYNAMIC,  ///< MoveModelManager以外での使用禁止
    FIELD_MOVE_MODEL_SUB_OBJECT_CNT,
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用のセーブされない状態  動作ビット
  //-----------------------------------------------------------------------------
  enum FieldMoveModelStateBit
  {
    FIELD_MOVE_MODEL_STATEBIT_NON                           = (0),      ///< 無し
    FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE           = (1<<1),   ///< モーション移動量を無視する
    FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE  = (1<<2),   ///< コリジョンの押し返しを無視する
    FIELD_MOVE_MODEL_STATEBIT_NOSERIALIZE                   = (1<<3),   ///< シリアライズ対象外にする(プレイヤー・ライドには無効)
    FIELD_MOVE_MODEL_DISABLE_DEFAULTMOITON_TO_SUSPEND       = (1<<4),   ///< 動作モデルの停止時にデフォルトアニメーションを再生しない(プレイヤー・ライドのみに有効)
    FIELD_MOVE_MODEL_DISABLE_STEP_CNT                       = (1<<5),   ///< 動作モデルの歩数カウントを無効化
    FIELD_MOVE_MODEL_ATTACH_SHADOW_TO_WAIST                 = (1<<6),   ///< 影をWaistジョイントにアタッチするかどうか
    FIELD_MOVE_MODEL_DISP_CNT_CULLING_DISABLE               = (1<<7),   ///< 描画数カリング対処から除外する(カメラカリングは行われる)
    FIELD_MOVE_MODEL_APP_CULLING_DISABLE                    = (1<<8),   ///< アプリケーションカリング対処から除外する(カメラカリングは行われる)
    FIELD_MOVE_MODEL_ANIM_STOP_ACTOR_UNREGIST               = (1<<9),   ///< イベントアクターのアンレジストに起因するアニメーションの停止(対象となるイベントデータは一部に限られる)
    FIELD_MOVE_MODEL_STATEBIT_SHADOW_OFF                    = (1 << 10),   //< 影を表示するかどうか
    FIELD_MOVE_MODEL_STATEBIT_RESIST_CHECK_INVALID          = (1 << 11),   //< レジスト状態チェックを無効化するか？
    FIELD_MOVE_MODEL_STATEBIT_CULLING_AABB_MINIMUM          = (1 << 12),   //< AABBを最小にするか？
    FIELD_MOVE_MODEL_STATEBIT_CULLING_LENGTH_OPT_DISABLE    = (1 << 13),   //< 距離カリングの距離最適化を無効化
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用のコリジョンの種類
  //-----------------------------------------------------------------------------
  enum FieldMoveModelCollisionType
  {
    //------ 押し返しを受ける ------
    COLLISION_TYPE_TERRAIN,                   ///< 地面との衝突検出用
    COLLISION_TYPE_STATIC_MODEL,              ///< 配置モデルとの衝突検出用
    COLLISION_TYPE_TRAFFIC_MODEL,             ///< 往来モデルとの衝突検出用
    COLLISION_TYPE_OSHIKAESHI_NAMINORI,       ///< 波乗りフィルタとの衝突検出用
    COLLISION_TYPE_OSHIKAESHI_ROBA2,          ///< 地面馬フィルタとの衝突検出用
    COLLISION_TYPE_OSHIKAESHI_RIDE,           ///< ライド進入禁止フィルタとの衝突検出用

    //------ 押し返しを受けない ------
    COLLISION_TYPE_EVENT_POS,                 ///< イベントとの衝突検出用(押し返しを受けない)
    COLLISION_TYPE_EVENT_INTRPOS,             ///< イベントとの衝突検出用(押し返しを受けない)
    COLLISION_TYPE_EVENT_TALK,                ///< イベントとの衝突検出用(押し返しを受けない)
    COLLISION_TYPE_EVENT_PUSH,                ///< イベントとの衝突検出用(押し返しを受けない)
    COLLISION_TYPE_ENCOUNT,                   ///< エンカウント領域との衝突検出用(押し返しを受けない)
    COLLISION_TYPE_TRAINER_EYE,               ///< トレーナー視線領域との衝突検出用(押し返しを受けない)
    COLLISION_TYPE_STATIC_MODEL_BB_ANIMATION, ///< 配置モデルバウンディングボックスアニメーション領域との衝突検出用(押し返しを受けない)
    COLLISION_TYPE_EVENT_NAMINORI,            ///< 波乗りとの衝突検出量(押し返しを受けない)
    COLLISION_TYPE_GIMMICK_ENCOUNT,           ///< ギミックエンカウントのエンカウント開始範囲との衝突検出用(押し返しを受けない)
    COLLISION_TYPE_GIMMICK_ENCOUNT_ACTION,    ///< ギミックエンカウントのアクション実行範囲との衝突検出用(押し返しを受けない)
    COLLISION_TYPE_EVENT_FINDER,              ///< ファインダー
    COLLISION_TYPE_FISHING_POS,               ///< 釣り警戒範囲用
    COLLISION_TYPE_MAX,
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル 対 動作モデル用のStaticActor
  //-----------------------------------------------------------------------------
  enum FieldMoveModelStaticCollisionType
  {
    COLLISION_STATIC_TYPE_STATIC_MODEL,           ///< 動作モデル(NPC以外)  対 動作モデル用(NPC)
    COLLISION_STATIC_TYPE_TRAFFIC_MODEL,          ///< 動作モデル(往来以外) 対 動作モデル用(往来)
    COLLISION_STATIC_TYPE_EVENT_TALK,             ///< 会話判定用
    COLLISION_STATIC_TYPE_TRAINER_EYE,            ///< トレーナー視線用(削除予定)
    COLLISION_STATIC_TYPE_TRAINER_EYE_NEW,        ///< トレーナー視線用
    COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT,        ///< ギミックエンカウントのエンカウント開始判定
    COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT_ACTION, ///< ギミックエンカウントのアクション実行判定
    COLLISION_STATIC_TYPE_EVENT_FUREAI_TALK,      ///< 会話判定用

    COLLISION_STATIC_TYPE_MAX,
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用の作成リクエストデータ
  //-----------------------------------------------------------------------------
  struct FieldMoveModelHeaderWork
  {
    //------------------------------------------------------------------
    /**
    * @brief   コンストラクタ
    */
    //------------------------------------------------------------------
    FieldMoveModelHeaderWork()
    {
      Init();
    }

    //------------------------------------------------------------------
    /**
    * @brief   初期化
    */
    //------------------------------------------------------------------
    inline void Init(void)
    {
      this->eventId      = 0;
      this->moveCodeId   = FIELD_MOVE_CODE_NONE;
      this->posForReturn = gfl2::math::Vector3(0.0f,0.0f,0.0f);
      this->quaForReturn = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);
      this->zoneId       = ZONEID_ERROR;
    }

    //------------------------------------------------------------------
    /**
    * @brief   代入
    */
    //------------------------------------------------------------------
    inline FieldMoveModelHeaderWork&  operator = ( const FieldMoveModelHeaderWork&  rhs )
    {
      this->eventId      = rhs.eventId;
      this->moveCodeId   = rhs.moveCodeId;
      this->posForReturn = rhs.posForReturn;
      this->quaForReturn = rhs.quaForReturn;
      this->zoneId       = rhs.zoneId;
      return *this;
    }

    u32                    eventId;     // 識別用のユニークな値
    FIELD_MOVE_CODE_ID     moveCodeId;
    gfl2::math::Vector3    posForReturn;
    gfl2::math::Quaternion quaForReturn;
    u16                    zoneId;
  };
  struct FieldMoveModelHeaderResource
  {
    //------------------------------------------------------------------
    /**
    * @brief   コンストラクタ
    */
    //------------------------------------------------------------------
    FieldMoveModelHeaderResource()
    {
      Init();
    }

    //------------------------------------------------------------------
    /**
    * @brief   初期化
    */
    //------------------------------------------------------------------
    inline void Init(void)
    {
      this->pos                                       = gfl2::math::Vector3   (0.0f,0.0f,0.0f);
      this->qua                                       = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);
      this->characterId                               = 0;
      this->pDressUpParam                             = 0;
      this->pCollisionSceneForTerrain                 = 0;
      this->pCollisionSceneForStaticModel             = 0;
      this->pCollisionSceneForTrafficModel            = 0;
      this->pCollisionSceneForEventPos                = 0;
      this->pCollisionSceneForEventIntrPos            = 0;
      this->pCollisionSceneForEventTalk               = 0;
      this->pCollisionSceneForEventPush               = 0;
      this->pCollisionSceneForEncount                 = 0;
      this->pCollisionSceneForTrainerEye              = 0;
      this->pCollisionSceneForStaticModelBBAnimation  = 0;
      this->pCollisionSceneForOshikaeshiNaminori      = 0;
      this->pCollisionSceneForEventNaminori           = 0;
      this->pCollisionSceneForGimmickEncount          = 0;
      this->pCollisionSceneForGimmickEncountAction    = 0;
      this->pCollisionSceneForEventFinder             = 0;
      this->pCollisionSceneForOshikaeshiRoba2         = 0;
      this->pCollisionSceneForOshikaeshiRide          = 0;
      this->pCollisionSceneForFishingPos              = 0;
    }

    //------------------------------------------------------------------
    /**
    * @brief   代入
    */
    //------------------------------------------------------------------
    inline FieldMoveModelHeaderResource&  operator = ( const FieldMoveModelHeaderResource&  rhs )
    {
      this->pos                                       = rhs.pos;
      this->qua                                       = rhs.qua;
      this->characterId                               = rhs.characterId;
      this->pDressUpParam                             = rhs.pDressUpParam;
      this->pCollisionSceneForTerrain                 = rhs.pCollisionSceneForTerrain;
      this->pCollisionSceneForStaticModel             = rhs.pCollisionSceneForStaticModel;
      this->pCollisionSceneForTrafficModel            = rhs.pCollisionSceneForTrafficModel;
      this->pCollisionSceneForEventPos                = rhs.pCollisionSceneForEventPos;
      this->pCollisionSceneForEventIntrPos            = rhs.pCollisionSceneForEventIntrPos;
      this->pCollisionSceneForEventTalk               = rhs.pCollisionSceneForEventTalk;
      this->pCollisionSceneForEventPush               = rhs.pCollisionSceneForEventPush;
      this->pCollisionSceneForEncount                 = rhs.pCollisionSceneForEncount;
      this->pCollisionSceneForTrainerEye              = rhs.pCollisionSceneForTrainerEye;
      this->pCollisionSceneForStaticModelBBAnimation  = rhs.pCollisionSceneForStaticModelBBAnimation;
      this->pCollisionSceneForOshikaeshiNaminori      = rhs.pCollisionSceneForOshikaeshiNaminori;
      this->pCollisionSceneForEventNaminori           = rhs.pCollisionSceneForEventNaminori;
      this->pCollisionSceneForGimmickEncount          = rhs.pCollisionSceneForGimmickEncount;
      this->pCollisionSceneForGimmickEncountAction    = rhs.pCollisionSceneForGimmickEncountAction;
      this->pCollisionSceneForEventFinder             = rhs.pCollisionSceneForEventFinder;
      this->pCollisionSceneForOshikaeshiRoba2         = rhs.pCollisionSceneForOshikaeshiRoba2;
      this->pCollisionSceneForOshikaeshiRide          = rhs.pCollisionSceneForOshikaeshiRide;
      this->pCollisionSceneForFishingPos              = rhs.pCollisionSceneForFishingPos;
      return *this;
    }

    gfl2::math::Vector3           pos;
    gfl2::math::Quaternion        qua;
    u32                           characterId;
    poke_3d::model::DressUpParam* pDressUpParam;
    BaseCollisionScene*           pCollisionSceneForTerrain;
    BaseCollisionScene*           pCollisionSceneForStaticModel;
    BaseCollisionScene*           pCollisionSceneForTrafficModel;
    BaseCollisionScene*           pCollisionSceneForEventPos;
    BaseCollisionScene*           pCollisionSceneForEventIntrPos;
    BaseCollisionScene*           pCollisionSceneForEventTalk;
    BaseCollisionScene*           pCollisionSceneForEventPush;
    BaseCollisionScene*           pCollisionSceneForEncount;
    BaseCollisionScene*           pCollisionSceneForTrainerEye;
    BaseCollisionScene*           pCollisionSceneForStaticModelBBAnimation;
    BaseCollisionScene*           pCollisionSceneForOshikaeshiNaminori;
    BaseCollisionScene*           pCollisionSceneForEventNaminori;
    BaseCollisionScene*           pCollisionSceneForGimmickEncount;
    BaseCollisionScene*           pCollisionSceneForGimmickEncountAction;
    BaseCollisionScene*           pCollisionSceneForEventFinder;
    BaseCollisionScene*           pCollisionSceneForOshikaeshiRoba2;
    BaseCollisionScene*           pCollisionSceneForOshikaeshiRide;
    BaseCollisionScene*           pCollisionSceneForFishingPos;
  };

  //-----------------------------------------------------------------------------
  //! @brief サブオブジェクト用の作成リクエストデータ
  //-----------------------------------------------------------------------------
  struct FieldSubobjectHeader
  {
    //------------------------------------------------------------------
    /**
    * @brief   コンストラクタ
    */
    //------------------------------------------------------------------
    FieldSubobjectHeader()
    {
      Init();
    }

    //------------------------------------------------------------------
    /**
    * @brief   初期化
    */
    //------------------------------------------------------------------
    inline void Init(void)
    {
      this->characterId = 0;
    }

    //------------------------------------------------------------------
    /**
    * @brief   代入
    */
    //------------------------------------------------------------------
    inline FieldSubobjectHeader&  operator = ( const FieldSubobjectHeader&  rhs )
    {
      this->characterId = rhs.characterId;
      return *this;
    }

    u32 characterId;
  };

  //-----------------------------------------------------------------------------
  //! @brief CHARA.garcのアプリケーションデータマウント用構造体
  //-----------------------------------------------------------------------------
  enum
  {
    TYPE_PLAYER,
    TYPE_NPC,
    TYPE_POKEMON,
    TYPE_SUBOBJECT
  };
  struct ApplicationDataForCommon
  {
    u32 type;       // palyer=0・npc=1・pokemon=2・subobject=3
  };
  struct ApplicationDataForPlayer
  {
    u32 type;       // palyer=0・npc=1・pokemon=2・subobject=3
    u32 shadowFootEnable;
    u32 shadowFootScale;
    u32 shadowFootOffsetX;
    u32 shadowFootOffsetZ;
    u32 shadowType;
    u32 shadowScale;
    s32 shadowOffsetX;
    s32 shadowOffsetZ;
    u32 offsetX;
    u32 offsetY;
    u32 offsetZ;
    u32 collisionR;
    u32 collisionHeight;
    u32 interpStickDefault;
    u32 interpStickRun;
    u32 runTurnSpeed;
    u32 runTurnDegree;
    u32 collisioFilterR;
    u32 neutralRange;
    u32 piyoriMaxValue;
    u32 piyoriAngle;
  };
  struct ApplicationDataForNPC
  {
    u32 type;        // palyer=0・npc=1・pokemon=2・subobject=3
    u32 subobjectId; // 0なら無効値
    u32 isLanding;
    u32 edgeIdType;
    u32 shadowType;
    u32 shadowScale;
    s32 shadowOffsetX;
    s32 shadowOffsetZ;
    u32 isSittingTalk;
    u32 iconOffsetX;
    u32 iconOffsetY;
    u32 iconOffsetZ;
    u32 collisionOshikaeshiType;
    u32 collisionOshikaeshiOffsetX;
    u32 collisionOshikaeshiOffsetY;
    u32 collisionOshikaeshiOffsetZ;
    u32 collisionOshikaeshiCircleR;
    u32 collisionOshikaeshiCircleHeight;
    u32 collisionOshikaeshiBoxHeight;
    u32 collisionOshikaeshiBoxWidth;
    u32 collisionOshikaeshiBoxDepth;
    u32 collisionOshikaeshiYDegree;
    u32 collisionTalkType;
    u32 collisionTalkOffsetX;
    u32 collisionTalkOffsetY;
    u32 collisionTalkOffsetZ;
    u32 collisionTalkCircleR;
    u32 collisionTalkCircleHeight;
    u32 collisionTalkBoxHeight;
    u32 collisionTalkBoxWidth;
    u32 collisionTalkBoxDepth;
    u32 collisionTalkYDegree;
  };
  struct ApplicationDataForPokemon
  {
    u32 type;       // palyer=0・npc=1・pokemon=2・subobject=3
    u32 isLanding;
    u32 edgeIdType;
    u32 shadowType;
    u32 shadowScale;
    s32 shadowOffsetX;
    s32 shadowOffsetZ;
    u32 isSittingTalk;
    u32 scalePercent;
    u32 iconOffsetX;
    u32 iconOffsetY;
    u32 iconOffsetZ;
    u32 collisionOshikaeshiType;
    u32 collisionOshikaeshiOffsetX;
    u32 collisionOshikaeshiOffsetY;
    u32 collisionOshikaeshiOffsetZ;
    u32 collisionOshikaeshiCircleR;
    u32 collisionOshikaeshiCircleHeight;
    u32 collisionOshikaeshiBoxHeight;
    u32 collisionOshikaeshiBoxWidth;
    u32 collisionOshikaeshiBoxDepth;
    u32 collisionOshikaeshiYDegree;
    u32 collisionTalkType;
    u32 collisionTalkOffsetX;
    u32 collisionTalkOffsetY;
    u32 collisionTalkOffsetZ;
    u32 collisionTalkCircleR;
    u32 collisionTalkCircleHeight;
    u32 collisionTalkBoxHeight;
    u32 collisionTalkBoxWidth;
    u32 collisionTalkBoxDepth;
    u32 collisionTalkYDegree;
    u32 monsNo;
  };

  static const u32 SUBOBJECT_JOINT_NAME_SIZE = 32;
  struct ApplicationDataForSubobject
  {
    u32 type;         // palyer=0・npc=1・pokemon=2・subobject=3
    u32 jointName[SUBOBJECT_JOINT_NAME_SIZE];
  };

  //--------------------------------------------------------------------
  /**
   *  utf16->sjis 変換
   *
   *  @param[in]    code   変換元のUTF-16コード
   *  @param[out]   dst    変換後のsjisコードを格納するバッファ
   */
  //--------------------------------------------------------------------
  inline void utf16_to_ascii( wchar_t code, char* dst ){ *dst = '\0'; if( ((code & 0xffffff00) == 0) && ((code & 0x000000ff) < 0x0000007f) ){ *dst = code & 0x000000ff;} }

}; //end of namespace MoveModel
}; //end of namespace Field
