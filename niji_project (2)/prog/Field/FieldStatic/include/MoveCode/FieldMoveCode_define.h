#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCode_define.h
*  @brief  フィールド用 動作コードの定義
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================

namespace Field{ namespace MoveModel {

  class FieldMoveModel;

  //----------------------------------------------------------------------------------------
  //! @brief 動作コード用のID
  //----------------------------------------------------------------------------------------
  enum FIELD_MOVE_CODE_ID
  {
    FIELD_MOVE_CODE_NONE,                   ///< 何も動かない動作コード
    FIELD_MOVE_CODE_PLAYER,                 ///< プレイヤー動作コード
    FIELD_MOVE_CODE_NPC_RETURN,             ///< NPC挙動   注目[無] 回転[無] 初期配置戻り[有]
    FIELD_MOVE_CODE_TEMPLATE,               ///< 量産用のテンプレート動作コード
    FIELD_MOVE_CODE_TRAINER_EYE,            ///< 視線トレーナー(パスなし)動作コード
    FIELD_MOVE_CODE_RIDE_KENTAROS,          ///< ケンタロスライド動作コード
    FIELD_MOVE_CODE_RIDE_SAMEHADER,         ///< サメハダー動作コード
    FIELD_MOVE_CODE_TRAFFIC,                ///< 往来NPC専用動作コード
    FIELD_MOVE_CODE_TRAINER_PATH,           ///< 視線トレーナー(パスあり)動作コード
    FIELD_MOVE_CODE_GIMMICK_ENCOUNT_CHASE,  ///< ギミックエンカウント接近型動作コード
    FIELD_MOVE_CODE_GIMMICK_ENCOUNT_PATROL, ///< ギミックエンカウント巡回型動作コード
    FIELD_MOVE_CODE_GIMMICK_ENCOUNT_ESCAPE, ///< ギミックエンカウント逃走型動作コード
    FIELD_MOVE_CODE_RIDE_MUURAND,           ///< ムーランド動作コード
    FIELD_MOVE_CODE_RIDE_ROBA2,             ///< 地面馬動作コード
    FIELD_MOVE_CODE_RIDE_KAIRIKY,           ///< カイリキー動作コード
    FIELD_MOVE_CODE_RIDE_LAPLACE,           ///< ラプラス動作コード

    FIELD_MOVE_CODE_NPC_MEET,               ///< NPC挙動   注目[有] 回転[有] 初期配置戻り[無]
    FIELD_MOVE_CODE_NPC_MEET_RETURN,        ///< NPC挙動   注目[有] 回転[有] 初期配置戻り[有]
    FIELD_MOVE_CODE_NPC_LOOK,               ///< NPC挙動   注目[有] 回転[無] 初期配置戻り[無]
    FIELD_MOVE_CODE_NPC_LOOK_RETURN,        ///< NPC挙動   注目[有] 回転[無] 初期配置戻り[無]

    FIELD_MOVE_CODE_NPC_MEET_SHORT,         ///< NPC挙動   注目[有] 回転[有] 初期配置戻り[無] 注目距離縮小
    FIELD_MOVE_CODE_NPC_MEET_SHORT_RETURN,  ///< NPC挙動   注目[有] 回転[有] 初期配置戻り[有] 注目距離縮小
    FIELD_MOVE_CODE_NPC_LOOK_SHORT,         ///< NPC挙動   注目[有] 回転[無] 初期配置戻り[無] 注目距離縮小
    FIELD_MOVE_CODE_NPC_LOOK_SHORT_RETURN,  ///< NPC挙動   注目[有] 回転[無] 初期配置戻り[無] 注目距離縮小
    FIELD_MOVE_CODE_NPC_AZUKARIYA,          ///< NPC挙動   (預かり屋 ※育て屋前のNPC)
    FIELD_MOVE_CODE_FUREAI_A,               ///< ふれあい挙動Aタイプ
    
    FIELD_MOVE_CODE_NUM                     ///< 動作コード最大数
  };

  //----------------------------------------------------------------------------------------
  //! @brief 動作モデルの各インスタンスが保持する動作コードのワークサイズ
  //----------------------------------------------------------------------------------------
  enum
  {
    FIELD_MOVE_CODE_WORK_SIZE = 96
  };

  //----------------------------------------------------------------------------------------
  //! @brief 動作コードの共通ワーク
  //----------------------------------------------------------------------------------------
  struct FieldMoveCodeWork
  {
    FieldMoveModel*   pModel;                             ///< 動作モデルのワーク
    u32               moveCodeId;                         ///< 動作の種類
    u32               work[FIELD_MOVE_CODE_WORK_SIZE/4];  ///< フリーに使える共有ワーク
    u32               extendWorkSize;                     ///< 特定動作コード用の拡張ワークのサイズ
    u32*              pExtendWork;                        ///< 特定動作コード用の拡張ワーク
    b32               isSetupWorkFromSaveData;            ///< work配列がセーブデータによって初期化されたかどうか
  };

}; //end of namespace MoveModel
}; //end of namespace Field
