//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEventCheckTrainerEye.h
 *  @brief  トレーナー視線エンカウントチェック
 *  @author saita_kazuki
 *  @date   2015.06.02
 */
//=============================================================================

#if !defined( FIELD_EVENT_CHECK_TRAINER_EYE_H_INCLUDED )
#define FIELD_EVENT_CHECK_TRAINER_EYE_H_INCLUDED
#pragma once

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// script
#include "FieldScript/include/ScriptWork.h"

// placement
#include "Field/FieldRo/include/PlacementData/accessor/FieldTrainerAccessor.h"

// 前方宣言
namespace Field {
  namespace MoveModel {
    class FieldMoveModel;
    class FieldMoveModelManager;
  }
  class FieldTrainerActor;
}
namespace GameSys {
  class GameManager;
}

GFL_NAMESPACE_BEGIN( Field )

/**
 *  @class 視線エンカウントチェック
 */
class EventCheckTrainerEye
{
  GFL_FORBID_COPY_AND_ASSIGN( EventCheckTrainerEye);

public:

  /**
   *  @brief 視線エンカウントチェック
   *  @param pGameManager ゲームマネージャー。プレイヤーのパーティーなどの取得に使用
   *  @param pPlayer プレイヤーの動作モデル
   *  @retval true 視線エンカウントバトルイベント発生
   *  @retval false  視線エンカウントなし
   */
  static bool CheckTrainerEye( GameSys::GameManager* pGameManager, MoveModel::FieldMoveModel* pPlayer);

  /*
   * @brief 視線エンカウントコール
  */  
  static bool CallTrainerScript( GameSys::GameManager* pGameManager, FieldTrainerActor* pTrainerActor, bool is_eye  );

private:


  /**
   * @brief 相方のアクターを見つける
   * @param pGameManager
   * @param pTrainerActor Hitしたトレーナーアクター
   * @param pIndex 相方が見つかった際にデータIndex(Enemy0かEnemy1か?)を0,1で返す
   * @return 相方のアクターがない場合はNULL
   */
  static FieldTrainerActor* SearchPairTrainerActor( GameSys::GameManager* pGameManager, FieldTrainerActor* pTrainerActor, FieldScript::Work::TRAINER_EYE_HIT* pIndex );

  /**
   * @brief スクリプトワークにエンカウントトレーナー情報を格納
   * @param pScriptWork スクリプトワーク
   * @param pModel モデルワーク
   * @param pTrainerData トレーナーデータ
   * @param data_idx データインデックス
   * @param move_type 移動タイプ
   */
  static void SetEncountTrainerData(
    FieldScript::Work* pScriptWork,
    const FieldTrainerAccessor::TrainerData* pTrainerData,
    FieldScript::Work::TRAINER_EYE_HIT data_idx,
    u16 move_type
    );

  /**
   * @brief エンカウント時の動作モデルに対するステータス変更処理
   */
  static void SetTrainerModelStatus( MoveModel::FieldMoveModel* pMoveModel );

};

GFL_NAMESPACE_END( Field )

#endif  // FIELD_EVENT_CHECK_TRAINER_EYE_H_INCLUDED

