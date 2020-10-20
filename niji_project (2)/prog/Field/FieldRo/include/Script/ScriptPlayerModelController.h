//==============================================================================
/**
 * @file        ScriptPlayerModelController.cpp
 * @brief       スクリプト用　動作モデルコントローラークラス 
    
 * @author      Sho Ohhira 
 * @date        2015/04/13(月) 18:52:48
 */
//==============================================================================

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/SaveData.h"
#include <arc_def_index/arc_def.h>

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"

#include "Field/FieldRo/include/Window/FieldYesNoWindow.h"
#include "Field/FieldRo/include/Window/FieldTalkWindow.h"
#include "Field/FieldRo/include/Window/FieldListMenuWindow.h"

#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

#include <arc_index/message.gaix>
#include <arc_index/script_message.gaix>

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )


/**
 *  @class プレイヤーモデルのController（現在は髪型の変更のみを想定）
 */
class PlayerModelControllerForScript
{
  GFL_FORBID_COPY_AND_ASSIGN(PlayerModelControllerForScript); //コピーコンストラクタ＋代入禁止
public:

  PlayerModelControllerForScript() : 
     m_pGameManager(NULL)
    ,m_pFieldCharaModelManager(NULL)
    ,m_pPlayer(NULL)
    ,m_cpDressUpParam(NULL)
  {
  }

  virtual ~PlayerModelControllerForScript()
  {
  }

  /**
   *  @brief 初期化
   */
  void Initialize( GameSys::GameManager* pGameManager , Field::MoveModel::FieldMoveModelManager* pCharaModelManager, Field::MoveModel::FieldMoveModelPlayer* pPlayer, const poke_3d::model::DressUpParam* cpDressUpParam )
  {
    m_pGameManager            = pGameManager;
    m_pFieldCharaModelManager = pCharaModelManager;
    m_pPlayer                 = pPlayer;
    m_cpDressUpParam          = cpDressUpParam;
  }

  /**
   *  @brief モデルを表示にする
   */
  void OnModel()
  {
    m_pPlayer->SetVisible(true);
  }

  /**
   *  @brief モデルを非表示にする
   */
  void HideModel()
  {
    m_pPlayer->SetVisible(false);
  }

  /**
   *  @brief モデルを削除する
   */
  void DeleteModel()
  {
    //モデルデータがGPUのスタックに乗ってる状態で削除してはダメなので、まずはHideModelして１フレーム後に実行する事

    // 持っているサブオブジェクトを外す
    for( s32 i=0 ; i<Field::MoveModel::FIELD_MOVE_MODEL_SUB_OBJECT_CNT ; i++ )
    {
      m_pPlayer->SetSubobject(i,NULL,NULL);
    }

    // 見た目の破棄
    m_pFieldCharaModelManager->TerminateMoveModelResource( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );

    // 着せ替えパーツリソースノードの削除
    m_pFieldCharaModelManager->UnloadDressUpPartsAll();
  }

  /**
   *  @brief モデルを読み込む
   */
  void Load( const poke_3d::model::DressUpParam* cp_param )
  {
    //ファイル読み込み
    m_pFieldCharaModelManager->LoadDressUpPartsAsync(*m_cpDressUpParam);
  }

  /**
   *  @brief 読み込み待ち
   */
  b32 WaitLoad() const
  {
    return m_pFieldCharaModelManager->IsDressUpPartsLoaded(*m_cpDressUpParam);
  }

  /**
   *  @brief 生成
   */
  void Create()
  {
    // ドレスアップリソースの読み込み
    m_pFieldCharaModelManager->SetupDressUpParts(m_cpDressUpParam);

    // 見た目の再生性
    m_pGameManager->GetFieldmap()->CreatePlayer(false);
  }


private:

  GameSys::GameManager*                         m_pGameManager;
  Field::MoveModel::FieldMoveModelManager*      m_pFieldCharaModelManager;
  Field::MoveModel::FieldMoveModelPlayer*       m_pPlayer;
  const poke_3d::model::DressUpParam*           m_cpDressUpParam;
};


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
