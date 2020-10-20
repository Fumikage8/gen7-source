

#include "GameSys/include/GameManager.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Script/ScriptEndCheck.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )


// 1フレームで終了するチェック
void CheckOntime( GameSys::GameManager* pGameManager )
{
#if PM_DEBUG
  // 動作モデルアクターのレジスト状態に、イベントデータとの差異がないかチェックする
  // ChrAdd,ChrDelとフラグワークに差異があるとアサート

  // @todo 10ROMで多発するので一旦無効化
  // pGameManager->GetFieldResident()->GetPlacementDataManager()->CheckMoveModelActorResistState();
#endif
}

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
