//==============================================================================
/**
 * @file	JoinFestaFacilitiesMoveModel.cpp
 * @brief	ジョインフェスタ施設の店員の動作モデル
 * @author	ichiraku_katsuhiko
 * @date	2015/11/14 15:59:28
 */
// =============================================================================
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesMoveModel.h"

#include <util/include/gfl2_std_string.h>
#include <base/include/gfl2_Singleton.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// system
#include "System/include/GflUse.h"

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

// アトラクションマネージャ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

#include "niji_conv_header/field/FieldPlacementMoveCode.h"
// スクリプト
//#include "FieldScript/include/FieldScriptSystem.h"
//#include "FieldScript/include/ScriptWork.h"
//#include "Savedata/include/EventWork.h"
//#include "Savedata/include/EventWorkDef.h"
#include "niji_reference_files/script/FieldPawnTypes.h"
#include <niji_conv_header/field/script/inc/join_festa_facilities.inc>
#include <niji_conv_header/field/chara/p2_base_fi.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

// コンストラクタ
JoinFestaFacilitiesMoveModel::JoinFestaFacilitiesMoveModel( gfl2::heap::HeapBase* pHeap ) :
  m_moveModelActor()
  ,m_moveModelDataArray()
  ,m_pRequestCharaModel(NULL)
  ,m_fieldPlacementDataMoveModel()
  ,m_setupSeq(0)
  ,m_deleteSeq(0)
{
}

// デストラクタ
JoinFestaFacilitiesMoveModel::~JoinFestaFacilitiesMoveModel()
{
}


//-----------------------------------------------------------------------------
/**
*  @brief   モデルセットアップ
*  @param   [in]facilitiesIndex   施設インデックス
*  @param   [in]facilitiesType    施設種類
*  @param   [in]characterId       キャラクターID
*  @param   [in]position          位置
*  @param   [in]rotationRadY      回転Ｙ
*/
//-----------------------------------------------------------------------------
void JoinFestaFacilitiesMoveModel::Setup( const JoinFestaScript::FacilitiesIndex facilitiesIndex, const JoinFestaScript::FacilitiesType facilitiesType, const u32 characterId )
{
  f32 rotationRadY = gfl2::math::ConvDegToRad( 45.0f * (facilitiesIndex+1) );
  // 位置計算
  gfl2::math::Vector3 position;
  {
    // ケンタロスライドのアトラクションのみ障害物として設置したい
    if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying() &&
        GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->GetPlayAttractionId() == JoinFestaScript::ATTRACTION_RIDE_KENTAROSU_1 )
    {
      static const gfl2::math::Vector3 rideAttractionPos[ JoinFestaScript::FACILITIES_INDEX_MAX ] = {
        gfl2::math::Vector3( 5543.0f,0.0f,4000.0f ),
        gfl2::math::Vector3( 4114.0f,0.0f,2897.0f ),
        gfl2::math::Vector3( 3827.0f,0.0f,2679.0f ),
        gfl2::math::Vector3( 2648.0f,0.0f,4049.0f ),
        gfl2::math::Vector3( 5393.0f,0.0f,3146.0f ),
        gfl2::math::Vector3( 2991.0f,0.0f,4551.0f ),
        gfl2::math::Vector3( 2949.0f,0.0f,4780.0f ),
      };

      position = rideAttractionPos[ static_cast<u8>(facilitiesIndex) ];
    }
    // 通常フィールド時
    else
    {
      gfl2::math::Vector3 centerPos( 4200.0f, 0.0f, 4200.0f );
      gfl2::math::Vector3 basePos( 0.0f, 0.0f, 0.0f );

      // 施設によって位置変える
      switch( facilitiesType ){
      case JoinFestaScript::FACILITIES_TYPE_LOTTERY:    //! くじ引き屋
        {
          basePos.SetZ( 1020.0f );

          // 話しかけコリジョンもずらす
          m_fieldPlacementDataMoveModel.SetFacilitiesTalkCollisionOffset( rotationRadY, 1150.0f );
        }
        break;
      case JoinFestaScript::FACILITIES_TYPE_STALL:      //! 屋台
        {
          basePos.SetZ( 1056.0f );

          // 話しかけコリジョンもずらす
          m_fieldPlacementDataMoveModel.SetFacilitiesTalkCollisionOffset( rotationRadY, 1130.0f );
        }
        break;
      case JoinFestaScript::FACILITIES_TYPE_DYEING:     //! 染物屋
        {
          basePos.SetZ( 1026.0f );

          // 話しかけコリジョンもずらす
          m_fieldPlacementDataMoveModel.SetFacilitiesTalkCollisionOffset( rotationRadY, 1130.0f );
        }
        break;
      case JoinFestaScript::FACILITIES_TYPE_AIR_TOY:    //! エア遊具
      case JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE:  //! ビックリハウス
      case JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP:  //! ギフトショップ
      case JoinFestaScript::FACILITIES_TYPE_FORTUNE:    //! 占い
      case JoinFestaScript::FACILITIES_TYPE_TRADE:      //! こうかんじょ(momiji追加)
        basePos.SetZ( 1200.0f );
        break;
      }

      gfl2::math::Matrix34 transMat( gfl2::math::Matrix34::GetTranslation( basePos ) );
      gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationY( rotationRadY ) );
      gfl2::math::Matrix34 mat = rotMat * transMat;
      //position += mat.GetElemPosition();
      position = centerPos + mat.GetElemPosition();
      ICHI_PRINT("  pos(%.f,%.f,%.f)\n", position.x, position.y, position.z );
      ICHI_PRINT("  rotY(%f)\n", rotationRadY );
    }
  }

  

  // 動作モデルデータセットアップ
  {
    gfl2::math::Quaternion qut;
    // 外側向かす
    qut.RadianYToQuaternion( rotationRadY );

    //m_moveModelDataArray[jfPointIndex].type        = 0;
    m_moveModelDataArray.position[0] = position.x;
    m_moveModelDataArray.position[1] = 1.0f;
    m_moveModelDataArray.position[2] = position.z;
    m_moveModelDataArray.quaternion[0]= qut.x;
    m_moveModelDataArray.quaternion[1]= qut.y;
    m_moveModelDataArray.quaternion[2]= qut.z;
    m_moveModelDataArray.quaternion[3]= qut.w;
    m_moveModelDataArray.romVersion = 0;
    m_moveModelDataArray.flagwork   = 0;
    m_moveModelDataArray.flagwork_num= 0;
    m_moveModelDataArray.eventID     = JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET + facilitiesIndex;

    // キャラクターIDの指定は必要
    m_moveModelDataArray.characterId = characterId;

    // 施設によってスクリプトID変える
    switch( facilitiesType ){
    case JoinFestaScript::FACILITIES_TYPE_LOTTERY:    //! くじ引き屋
      m_moveModelDataArray.scriptID    = SCRID_JOIN_FESTA_FACILITIES_LOTTERY;
      m_moveModelDataArray.moveCode    = static_cast<u32>(Field::PLACEMENT_MOVE_CODE_NPC_MEET_SHORT_RETURN);
      break;
    case JoinFestaScript::FACILITIES_TYPE_AIR_TOY:    //! エア遊具
      m_moveModelDataArray.scriptID    = SCRID_JOIN_FESTA_FACILITIES_AIR_TOY;
      m_moveModelDataArray.moveCode    = static_cast<u32>(Field::PLACEMENT_MOVE_CODE_NPC_MEET_SHORT_RETURN);
      break;
    case JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE:  //! ビックリハウス
      m_moveModelDataArray.scriptID    = SCRID_JOIN_FESTA_FACILITIES_FUN_HOUSE;
      m_moveModelDataArray.moveCode    = static_cast<u32>(Field::PLACEMENT_MOVE_CODE_NPC_MEET_SHORT_RETURN);
      break;
    case JoinFestaScript::FACILITIES_TYPE_STALL:      //! 屋台
      m_moveModelDataArray.scriptID    = SCRID_JOIN_FESTA_FACILITIES_STALL;
      m_moveModelDataArray.moveCode    = static_cast<u32>(Field::PLACEMENT_MOVE_CODE_NPC_MEET_SHORT_RETURN);
      break;
    case JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP:  //! ギフトショップ
      m_moveModelDataArray.scriptID    = SCRID_JOIN_FESTA_FACILITIES_GIFT_SHOP;
      m_moveModelDataArray.moveCode    = static_cast<u32>(Field::PLACEMENT_MOVE_CODE_NPC_MEET_SHORT_RETURN);
      break;
    case JoinFestaScript::FACILITIES_TYPE_FORTUNE:    //! 占い
      m_moveModelDataArray.scriptID    = SCRID_JOIN_FESTA_FACILITIES_FORTUNE;
      m_moveModelDataArray.moveCode    = static_cast<u32>(Field::PLACEMENT_MOVE_CODE_NPC_MEET_SHORT_RETURN);
      break;
    case JoinFestaScript::FACILITIES_TYPE_DYEING:     //! 染物屋
      m_moveModelDataArray.scriptID    = SCRID_JOIN_FESTA_FACILITIES_DYEING;
      m_moveModelDataArray.moveCode    = static_cast<u32>(Field::PLACEMENT_MOVE_CODE_NPC_MEET_SHORT_RETURN);
      break;
    case JoinFestaScript::FACILITIES_TYPE_TRADE:      //! こうかんじょ(momiji追加)
      m_moveModelDataArray.scriptID    = SCRID_JOIN_FESTA_FACILITIES_TRADE;
      m_moveModelDataArray.moveCode    = static_cast<u32>(Field::PLACEMENT_MOVE_CODE_NPC_MEET_SHORT_RETURN);
      break;
    }

    // デフォルトアニメーション指定
    m_moveModelDataArray.defaultAnimeData.defaultMotionID = 0;        // 待機
    m_moveModelDataArray.defaultAnimeData.defaultMotionIDNum = 0;

    //m_moveModelDataArray[ jfPointIndex].talkCollisionOffset
    //m_moveModelDataArray[ jfPointIndex].collisionOffset
  }

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
bool JoinFestaFacilitiesMoveModel::IsSetup()
{
  Field::Fieldmap* fieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
  Field::MoveModel::FieldMoveModelManager*  pFieldCharaModelManager = fieldmap->GetMoveModelManager();
  const u32 characterId = m_moveModelDataArray.characterId;

  // 着せ替えパーツの読み込み用
  switch( m_setupSeq ){
  case 0:
    // データの読み込み完了していなかったら読み込みリクエスト開始
    //if( pFieldCharaModelManager->IsLoaded(characterId) == false )
    pFieldCharaModelManager->LoadStaticAsync(characterId);
    m_setupSeq++;
    /* break through */

  case 1:
    if( !pFieldCharaModelManager->IsLoaded(characterId) )
    {
      break;
    }

    pFieldCharaModelManager->SetupModel(characterId);

    // 動作モデルアクターセットアップ
    SetupMoveModelActor();

    // 動作モデル取得しておく
    m_pRequestCharaModel = m_moveModelActor.GetMoveModel();

    // 地形コリジョンを設定
    m_pRequestCharaModel->SetCollisionSceneContainerForGround( fieldmap->GetTerrainManager()->GetCollsionScene() );

    // 最初は非表示
    m_pRequestCharaModel->SetVisible( false );
    m_setupSeq++;
    /* break through */

  case 2:
    // 一定時間待つ
    if( m_pRequestCharaModel->IsSetup() )
    {
      // 表示
      m_pRequestCharaModel->SetVisible( true );
      m_setupSeq++;
      return true;
    }
    break;
  case 3:
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief   モデル破棄（破棄完了が返った後でも呼んでもOK）
*  @retval  true:破棄完了
*  @retval  false:未完了
*/
//-----------------------------------------------------------------------------
bool JoinFestaFacilitiesMoveModel::Delete()
{
  if( m_pRequestCharaModel != NULL )
  {
    // 着せ替え破棄＆１フレ遅れ対応
    switch( m_deleteSeq ){
    case 0:
      m_moveModelActor.UnregistActor();
      m_deleteSeq++;
      return false;

    case 1:
      m_moveModelActor.Terminate();
      {
        // 初期化
        m_pRequestCharaModel = NULL;
        m_deleteSeq = 0;
      }
      break;
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
*  @brief   動作モデルアクターセットアップ
*/
//-----------------------------------------------------------------------------
void JoinFestaFacilitiesMoveModel::SetupMoveModelActor()
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

  m_moveModelActor.Initialize( setupData );

  // リソースとコリジョン設定
  {
    Field::FieldMoveModelActor::MoveModelCreateResourceData createData;
    createData.pAccessor  = &m_fieldPlacementDataMoveModel;
    createData.pRenderingPipeLine = fieldmap->GetUpperRenderingPipeLine();
    createData.pTerrainManager    = fieldmap->GetTerrainManager();
    createData.cpEventWork        = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();

    // アクセサー生成
    m_moveModelActor.CreateResource( createData );
    m_moveModelActor.RegistActor();
  }
}

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

