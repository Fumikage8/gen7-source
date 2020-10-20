//======================================================================
/**
 * @file FieldEventCheckGimmickEncount.h
 * @date 2015/09/02 12:03:05
 * @author saita_kazuki
 * @brief ギミックエンカウントのコリジョンチェック
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EVENT_CHECK_GIMMICK_ENCOUNT_H_INCLUDED__
#define __FIELD_EVENT_CHECK_GIMMICK_ENCOUNT_H_INCLUDED__
#pragma once

// gflib
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// placement
#include "Field/FieldRo/include/PlacementData/accessor/FieldGimmickEncountAccessor.h"

// 前方宣言
class BaseActor;
class DynamicActor;
namespace Field {
  class IFieldGimmickEncountActorBase;
  namespace MoveModel {
    class FieldMoveModel;
    class FieldMoveModelManager;
  }
  namespace Encount {
    class Encount;
  }
}
namespace GameSys {
  class GameManager;
}

GFL_NAMESPACE_BEGIN( Field )

/**
 *  @class ギミックエンカウントのコリジョンチェック
 */
class EventCheckGimmickEncount
{
public:

  /**
   *  @brief コリジョンチェック
   *  @param pGameManager ゲームマネージャー
   *  @param pPlayer プレイヤーの動作モデル
   *  @retval true エンカウント発生
   *  @retval false エンカウントなし
   */
  static bool Check( GameSys::GameManager* pGameManager, MoveModel::FieldMoveModel* pPlayer);

private:

  static IFieldGimmickEncountActorBase* CheckEncount( GameSys::GameManager* pGameManager, MoveModel::FieldMoveModel* pPlayer);
  static void CheckAction( GameSys::GameManager* pGameManager, MoveModel::FieldMoveModel* pPlayer);

};

GFL_NAMESPACE_END( Field )

#endif // __FIELD_EVENT_CHECK_GIMMICK_ENCOUNT_H_INCLUDED__
