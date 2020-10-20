//==============================================================================
/**
 * @file	BattleInstModel.cpp
 * @brief	バトル施設モデル
 * @author	onoue_masayuki
 * @date	2016/01/27
 */
// =============================================================================

#include <util/include/gfl2_std_string.h>
#include <base/include/gfl2_Singleton.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// system
#include "System/include/GflUse.h"
#include "System/include/DressUp.h"

// フィールドマップ
#include "Field/FieldRo/include/Fieldmap.h"
// Terrain
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
// フィールドレジデント
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
// コリジョンシステム
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"
#include "Field/FieldRo/include/PlacementData\accessor/FieldCollisionAccessor.h"

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>

// 動作モデル
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "niji_conv_header/field/FieldPlacementMoveCode.h"

// スクリプト
//#include "FieldScript/include/FieldScriptSystem.h"
//#include "FieldScript/include/ScriptWork.h"
//#include "Savedata/include/EventWork.h"
//#include "Savedata/include/EventWorkDef.h"
#include "Savedata/include/MyStatus.h"
#include "niji_reference_files/script/FieldPawnTypes.h"
#include <niji_conv_header/field/chara/p2_base_fi.h>

// mine
#include "Field/FieldRos/FieldGimmickBattleInst/include/BattleInst/BattleInstModel.h"
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"

GFL_NAMESPACE_BEGIN(BattleInst)

//! 衝突判定の距離
static const f32 PLAYER_HIT_LENGTH = 90.0f;


// コンストラクタ
BattleInstModel::BattleInstModel( gfl2::heap::HeapBase* pHeap ) :
 m_pRequestCharaModel(NULL)
 ,m_pAllocator(NULL)
 ,m_setupSeq(0)
 ,m_deleteSeq(0)
 ,m_subobjectId(0)
 ,m_IsSetuped(false)
 ,m_IsLoad(false) 
{
  mp_Heap = pHeap;
  // 着せ替え用アロケータ作成
  m_pAllocator = GFL_NEW( pHeap ) System::nijiAllocator( pHeap, System::nijiAllocator::APPLICATION );
  // 着せ替えリソースマネージャ初期化
  m_dressUpModelResourceManager.Initialize( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),
                                            pHeap, System::DressUp::GetDressUpArcIdListField() );
}

// デストラクタ
BattleInstModel::~BattleInstModel()
{
  m_dressUpModelResourceManager.Finalize();
  GFL_SAFE_DELETE( m_pAllocator );
}


//-----------------------------------------------------------------------------
/**
*  @brief   モデルセットアップ
*/
//-----------------------------------------------------------------------------
void BattleInstModel::Setup( BattleInst::MoveModelData* p_data )
{
  GFL_ASSERT(m_pRequestCharaModel == NULL);

  m_pRequestData = p_data;
  if( p_data->isDressup )
  {
    // 着せ替えパラメータ保存
    SetupMoveModelData( m_pRequestData->index, m_pRequestData->pos, m_pRequestData->rot, m_pRequestData->pMyStatus );
  }else{
    // 動作モデルデータセットアップ
    SetupMoveModelData( m_pRequestData->index, m_pRequestData->pos, m_pRequestData->rot, m_pRequestData->charaID );
  }

  //　この後のセットアップは 着せ替えパーツの読み込みが必要
  m_setupSeq = 0;
  m_deleteSeq = 0;
}

//-----------------------------------------------------------------------------
/**
*  @brief   モデルセットアップ完了チェック
*  @retval  true:完了
*  @retval  false:未完了
*/
//-----------------------------------------------------------------------------
bool BattleInstModel::IsSetupEnd()
{
  return m_IsSetuped;
}

//-----------------------------------------------------------------------------
/**
*  @brief   モデルセットアップ完了チェック
*  @param   isCheckHitPlayer    主人公との衝突判定をするかどうか(trueでする）
*  @retval  true:完了
*  @retval  false:未完了
*/
//-----------------------------------------------------------------------------
bool BattleInstModel::SetupUpdate( const bool isCheckHitPlayer )
{
  enum
  {
    INIT_MAIN_MODEL_START,
    SETUP_MAIN_MODEL,

    INIT_SUB_MODEL_START,
    LOAD_SUB_MODEL,
    SETUP_SUB_MODEL,

    SETUP_ACTOR,
    WAIT_MAIN_MODEL_LOAD,
    END,
  };
  const u32 characterId = m_moveModelDataArray.characterId;
  Field::Fieldmap* fieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
  Field::MoveModel::FieldMoveModelManager*  pFieldCharaModelManager = fieldmap->GetMoveModelManager();

  // 着せ替えパーツの読み込み用
  switch( m_setupSeq ){

  case INIT_MAIN_MODEL_START:
    // 着せ替えパーツの非同期読み込み開始
    if( !m_pRequestData->isDressup && IsLoaded(pFieldCharaModelManager, characterId) )
    {
      m_IsLoad = false;
      ONOUE_PRINT("動的生成動作モデル読み込みスキップ\n");
    }else{
      LoadStart(pFieldCharaModelManager, characterId);
      m_IsLoad = true;
      ONOUE_PRINT("動的生成動作モデル読み込み\n");
    }
    m_setupSeq=SETUP_MAIN_MODEL;
    break;

  case SETUP_MAIN_MODEL:

    if( IsLoaded(pFieldCharaModelManager, characterId) )
    {
      SetupModel(pFieldCharaModelManager, characterId);


      if( !m_pRequestData->isDressup )
      {
        // NPCのアプリケーションデータを取得
        mp_CharaData = (Field::MoveModel::ApplicationDataForNPC*)( pFieldCharaModelManager->GetApplicationData( characterId ) );
        if( mp_CharaData )
        {
          m_subobjectId = mp_CharaData->subobjectId;
          m_setupSeq = INIT_SUB_MODEL_START;
        }else{
          m_setupSeq = SETUP_ACTOR;
        }
      }else{
        m_setupSeq = SETUP_ACTOR;
      }

    }
    break;

  case INIT_SUB_MODEL_START:
    // 対象のバイナリロード
    pFieldCharaModelManager->LoadStaticAsync(mp_Heap,m_subobjectId);
    m_setupSeq = LOAD_SUB_MODEL;
    break;

  case LOAD_SUB_MODEL:
    if( pFieldCharaModelManager->IsLoaded(m_subobjectId)) 
    {
      m_setupSeq = SETUP_SUB_MODEL;
    }
    break;

  case SETUP_SUB_MODEL:
    {
      pFieldCharaModelManager->SetupModel(m_subobjectId);
      m_setupSeq = SETUP_ACTOR;
    }
    break;

  case SETUP_ACTOR:
    {
      // 主人公と衝突判定をするかどうか（フィールド終了時はしない）
      if( m_pRequestData->isDressup && isCheckHitPlayer == true )
      {
        // 主人公と衝突していたらSkip
        if( IsHitPlayer() == true )
        {
          break;
        }
      }

      // 動作モデルアクターセットアップ
      SetupMoveModelActor();

      // 動作モデル取得しておく
      m_pRequestCharaModel = mp_moveModelActor->GetMoveModel();
      m_pRequestCharaModel->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE);
      // @fix NMCat[670] バトルツリー内セーブで、動作モデルが復帰時に不正な状態で復帰してしまう問題を修正
      m_pRequestCharaModel->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_NOSERIALIZE);

      // 地形コリジョンを設定
      m_pRequestCharaModel->SetCollisionSceneContainerForGround( fieldmap->GetTerrainManager()->GetCollsionScene() );

      // 最初は非表示
      m_pRequestCharaModel->SetVisible( false );

      m_setupSeq=WAIT_MAIN_MODEL_LOAD;
    }
    break;

  case WAIT_MAIN_MODEL_LOAD:
    {
      // 一定時間待つ
      if( m_pRequestCharaModel->IsSetup() )
      {
        // 表示
        m_pRequestCharaModel->SetVisible( true );

        m_setupSeq = END;
      }
    }
    break;

  case END:
    m_IsSetuped = true;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief   モデルをセットアップする
*/
//-----------------------------------------------------------------------------
void BattleInstModel::SetupModel(Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager, const u32 characterId)
{
  if( m_pRequestData->isDressup )
  {
    // ドレスアップリソースをセットアップする
    if( m_dressUpModelResourceManager.SetupDressUpParts( m_pAllocator, m_pRequestData->pMyStatus->GetDressup() ) == false )
    {// 失敗
      GFL_ASSERT_STOP(0);
    }
  }else{
    pFieldCharaModelManager->SetupModel(characterId);
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief   ロード開始
*/
//-----------------------------------------------------------------------------
void BattleInstModel::LoadStart(Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager, const u32 characterId)
{
  if( m_pRequestData->isDressup )
  {
    m_dressUpModelResourceManager.LoadDressUpPartsAsync( m_pRequestData->pMyStatus->GetDressup() );
  }else{
    pFieldCharaModelManager->LoadStaticAsync(mp_Heap,characterId);
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief   ロード完了待ち
*/
//-----------------------------------------------------------------------------
bool BattleInstModel::IsLoaded(Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager, const u32 characterId)
{
  if( m_pRequestData->isDressup )
  {
    return m_dressUpModelResourceManager.IsDressUpPartsLoaded( m_pRequestData->pMyStatus->GetDressup() );
  }else{
    return pFieldCharaModelManager->IsLoaded(characterId);
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief   モデル破棄
*  @return  trueで破棄完了
*/
//-----------------------------------------------------------------------------
bool BattleInstModel::Delete()
{
  if( m_pRequestCharaModel != NULL )
  {
    // 着せ替え破棄＆１フレ遅れ対応
    switch( m_deleteSeq ){
    case 0:
      mp_moveModelActor->UnregistActor();
      m_deleteSeq++;
      return false;
    case 1:
      mp_moveModelActor->Terminate();
      m_deleteSeq++;
      return false;
    case 2:
      {
        if( m_pRequestData->isDressup )
        {
          // 着せ替えを開放出来るか
          if( m_dressUpModelResourceManager.CanUnloadDressUpParts( m_pRequestData->pMyStatus->GetDressup() ) )
          {
            // 着せ替えパーツの削除
            m_dressUpModelResourceManager.UnloadDressUpParts( m_pRequestData->pMyStatus->GetDressup() );
          }
        }else{
          // @fix NMcat[1114] マップに配置済みのトレーナーを破棄してしまう問題を修正
          if( m_IsLoad )
          {
            Field::Fieldmap*      fieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
            Field::FieldResident* fieldResident = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident();
            Field::MoveModel::FieldMoveModelManager*  pFieldCharaModelManager = fieldmap->GetMoveModelManager();
            const u32 characterId = m_moveModelDataArray.characterId;
            if( m_subobjectId != 0 )
            {
              pFieldCharaModelManager->Unload(m_subobjectId);
            }
            pFieldCharaModelManager->Unload(characterId);
          }
        }

        // 初期化
        GFL_SAFE_DELETE(mp_moveModelActor);
        m_pRequestCharaModel = NULL;
        m_deleteSeq = 0;
      }
      return false;
    }
  }
  return true;
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
*  @brief   動作モデルデータセットアップ
*  @param   [in]pos             表示位置
*  @param   [in]sex             性別
*/
//-----------------------------------------------------------------------------
void BattleInstModel::SetupMoveModelData( const u32 index, const gfl2::math::Vector3 pos, const gfl2::math::Quaternion rot, const u32 characterId )
{
  SetupMoveModelDataDefault(rot, pos, index);

  // キャラクターIDの指定は必要
  m_moveModelDataArray.characterId = characterId;

  // デフォルトアニメーション指定
  m_moveModelDataArray.defaultAnimeData.defaultMotionID = 0;        // 待機
  m_moveModelDataArray.defaultAnimeData.defaultMotionIDNum = 0;

}

//-----------------------------------------------------------------------------
/**
*  @brief   動作モデルデータセットアップ
*  @param   [in]pos             表示位置
*  @param   [in]sex             性別
*/
//-----------------------------------------------------------------------------
void BattleInstModel::SetupMoveModelData( const u32 index, const gfl2::math::Vector3 pos, const gfl2::math::Quaternion rot, const Savedata::MyStatus* pMyStatus )
{
  SetupMoveModelDataDefault(rot, pos, index);
  
  // キャラクターIDの指定は必要
  m_moveModelDataArray.characterId = (pMyStatus->GetSex() == PM_MALE ? CHARA_MODEL_ID_PC0001_00 : CHARA_MODEL_ID_PC0002_00 );

  // デフォルトアニメーション指定
  m_moveModelDataArray.defaultAnimeData.defaultMotionID = P2_BASE_FI001_WAIT01;        // 待機
  m_moveModelDataArray.defaultAnimeData.defaultMotionIDNum = 0;

}

//-----------------------------------------------------------------------------
/**
*  @brief   基本パラメータ設定
*/
//-----------------------------------------------------------------------------
void BattleInstModel::SetupMoveModelDataDefault(const gfl2::math::Quaternion rot, const gfl2::math::Vector3 &pos, const u32 index)
{
  m_moveModelDataArray.position[0] = pos.x;
  m_moveModelDataArray.position[1] = pos.y;
  m_moveModelDataArray.position[2] = pos.z;
  m_moveModelDataArray.quaternion[0]= rot.x;
  m_moveModelDataArray.quaternion[1]= rot.y;
  m_moveModelDataArray.quaternion[2]= rot.z;
  m_moveModelDataArray.quaternion[3]= rot.w;
  m_moveModelDataArray.romVersion = 0;
  m_moveModelDataArray.flagwork   = 0;
  m_moveModelDataArray.flagwork_num= 0;
  m_moveModelDataArray.eventID     = BattleInstDefine::MODEL_EVENT_ID_OFFSET + index;
  m_moveModelDataArray.moveCode    = static_cast<u32>(Field::PLACEMENT_MOVE_CODE_NONE);
  m_moveModelDataArray.scriptID    = Field::FieldScript::SCRID_NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief   動作モデルアクターセットアップ
*/
//-----------------------------------------------------------------------------
void BattleInstModel::SetupMoveModelActor()
{
  Field::Fieldmap*      fieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
  Field::FieldResident* fieldResident = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident();
  Field::MoveModel::FieldMoveModelManager*  pFieldCharaModelManager = fieldmap->GetMoveModelManager();
  //Field::MoveModel::FieldMoveModel* pPlayer = fieldmap->GetPlayerCharacter();
  BaseCollisionScene*   pCollisionSceneForTerrain = fieldResident->GetCollisionScene( Field::FieldResident::COLLISION_SCENE_TERRAIN );
  BaseCollisionScene*   pCollisionSceneForStaticModel = fieldResident->GetCollisionScene( Field::FieldResident::COLLISION_SCENE_STATIC_MODEL );
  BaseCollisionScene*   pCollisionSceneForEventTalk = fieldResident->GetCollisionScene( Field::FieldResident::COLLISION_SCENE_EVENT_TALK );
  //Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();

  // 動作モデルアクターセットアップデータ
  Field::FieldMoveModelActor::MoveModelSetupData setupData;
  {
    setupData.pMoveModelManager               = pFieldCharaModelManager;
    setupData.pCollisionSceneForTerrain       = pCollisionSceneForTerrain;
    setupData.pCollisionSceneForStaticModel   = pCollisionSceneForStaticModel;
    setupData.pCollisionSceneForEventPos      = NULL;
    setupData.pCollisionSceneForEventTalk     = pCollisionSceneForEventTalk;
    setupData.pCollisionSceneForEventPush     = NULL;
    setupData.pMoveModelData                  = &m_moveModelDataArray;
    setupData.pContactPokemonData             = NULL;
    setupData.zoneId                          = fieldmap->GetZoneID();
    setupData.dataZoneId                      = fieldmap->GetZoneID();
  }

  mp_moveModelActor = GFL_NEW(mp_Heap) Field::FieldMoveModelActor();
  mp_moveModelActor->Initialize( setupData );

  if( m_pRequestData->isDressup )
  {
    // [fix]GFCHECK[8230] テンポラリのアドレスを参照していた問題を修正
    // 着せ替えパラメータ設定
    m_dressUpParam = m_pRequestData->pMyStatus->GetDressup();
    m_fieldPlacementDataMoveModel.SetDressUpParam( &m_dressUpParam );
    // 着せ替えリソースマネージャ設定
    m_fieldPlacementDataMoveModel.SetDressUpModelResourceManager( &m_dressUpModelResourceManager );
  }

  // リソースとコリジョン設定
  {
    Field::FieldMoveModelActor::MoveModelCreateResourceData createData;
    createData.pAccessor  = &m_fieldPlacementDataMoveModel;
    createData.pRenderingPipeLine = fieldmap->GetUpperRenderingPipeLine();
    createData.pTerrainManager    = fieldmap->GetTerrainManager();
    createData.cpEventWork        = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();
    // アクセサー生成
    mp_moveModelActor->CreateResource( createData );
    mp_moveModelActor->RegistActor();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief   主人公と衝突しているかチェック
*  @retun   trueで衝突している
*/
//-----------------------------------------------------------------------------
bool BattleInstModel::IsHitPlayer() const
{
  gfl2::math::Vector3 playerPos = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetPlayerPosition(); 
  gfl2::math::Vector3 npcPos( m_moveModelDataArray.position[0], m_moveModelDataArray.position[1], m_moveModelDataArray.position[2] );
  gfl2::math::Vector3 len = playerPos - npcPos;

  // 衝突判定
  if( len.Length() <= PLAYER_HIT_LENGTH )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief   モデルデータを更新する
*/
//-----------------------------------------------------------------------------
void BattleInstModel::UpdateModelData( BattleInst::MoveModelData* p_data )
{
  poke_3d::model::BaseModel* model =m_pRequestCharaModel->GetCharaDrawInstance();
  p_data->pos.x = model->GetPosition().x;
  p_data->pos.y = model->GetPosition().y;
  p_data->pos.z = model->GetPosition().z;

  // @fix GFNMCat[1191] バトル施設のパートナーが正しく回転を保存できていなかった問題を修正
  p_data->rot = model->GetRotationQuat();
}

GFL_NAMESPACE_END(BattleInst)

