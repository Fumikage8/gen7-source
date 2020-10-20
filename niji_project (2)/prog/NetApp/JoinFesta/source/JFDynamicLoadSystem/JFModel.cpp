//==============================================================================
/**
 * @file	JFModel.cpp
 * @brief	ジョインフェスタモデル
 * @author	ichiraku_katsuhiko
 * @date	2015/05/28 15:59:28
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
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeNpc.h"

#include "niji_conv_header/field/FieldPlacementMoveCode.h"

// スクリプト
//#include "FieldScript/include/FieldScriptSystem.h"
//#include "FieldScript/include/ScriptWork.h"
//#include "Savedata/include/EventWork.h"
//#include "Savedata/include/EventWorkDef.h"
#include "niji_reference_files/script/FieldPawnTypes.h"
#include <niji_conv_header/field/script/inc/join_festa_field.inc>
#include <niji_conv_header/field/script/inc/join_festa_tutorial.inc>
#include <niji_conv_header/field/chara/p2_base_fi.h>
#include "Savedata/include/EventWork.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

// JoinFesta
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFPoint.h"

// JFパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"

// mine
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModel.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

//! 衝突判定の距離
static const f32 PLAYER_HIT_LENGTH = 100.0f;
static const f32 RIDE_HIT_LENGTH = 145.0f;


// コンストラクタ
JFModel::JFModel( gfl2::heap::HeapBase* pDressUpHeap, gfl2::heap::HeapBase* pDummyActorHeap, JoinFestaEffectSystem* pJoinFestaEffectSystem )
 :JoinFestaEffectSystem::IEventHandler()
 ,m_pJoinFestaEffectSystem(pJoinFestaEffectSystem)
 ,m_pJFPersonalData(NULL)
 ,m_moveModelActor()
 ,m_moveModelDataArray()
 ,m_pRequestCharaModel(NULL)
 ,m_fieldPlacementDataMoveModel()
 ,m_dressUpParam()
 ,m_dressUpModelResourceManager()
 ,m_pDressUpAllocator(NULL)
 ,m_pDummyActorHeap(pDummyActorHeap)
 ,m_pDummyActorAllocator(NULL)
 ,m_pStaticActor(NULL)
 ,m_jfPointIndex(JFPoint::INDEX_NULL)
 ,m_setupSeq(UPDATE_SEQ_SAFE_POS_CHECK)
 ,m_deleteSeq(0)
{
  // 着せ替え用アロケータ作成
  m_pDressUpAllocator = GFL_NEW( pDressUpHeap ) System::nijiAllocator( pDressUpHeap, System::nijiAllocator::APPLICATION );
  // 着せ替えリソースマネージャ初期化
  m_dressUpModelResourceManager.Initialize( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),
                                            pDressUpHeap, System::DressUp::GetDressUpArcIdListField() );

  // 衝突判定用アロケータ作成
  m_pDummyActorAllocator = GFL_NEW( m_pDummyActorHeap ) System::nijiAllocator( m_pDummyActorHeap, System::nijiAllocator::APPLICATION );
}

// デストラクタ
JFModel::~JFModel()
{
  // @fix NMCat[1353]：エフェクト中に削除されるとハンドラで吹っ飛ぶ
  m_pJoinFestaEffectSystem->SetEventHandler( NULL );

  GFL_SAFE_DELETE( m_pStaticActor ); // 念の為
  m_dressUpModelResourceManager.Finalize();
  GFL_SAFE_DELETE( m_pDressUpAllocator );
  GFL_SAFE_DELETE( m_pDummyActorAllocator );
}


//-----------------------------------------------------------------------------
/**
*  @brief   モデルセットアップ
*  @param   [in]pJFPersonalData ジョインフェスタパーソナルデータ
*  @param   [in]jfPointIndex    JFPointのインデックス値
*  @param   [in]pos  表示位置
*/
//-----------------------------------------------------------------------------
void JFModel::Setup( NetAppLib::JoinFesta::JoinFestaPersonalData* pJFPersonalData, const u32 jfPointIndex, const gfl2::math::Vector3 pos )
{
  GFL_ASSERT(m_pRequestCharaModel == NULL);

  // JFパーソナルデータ
  m_pJFPersonalData = pJFPersonalData;

  // JFポイントのインデックス
  m_jfPointIndex = jfPointIndex;

  // 着せ替えパラメータ保存
  ::System::DressUpParamSaveData::ConvertToDressUpParam( &m_dressUpParam, *(m_pJFPersonalData->GetDressUpParam()) );

  // @fix MMCat[337]nijiの場合は、バングルをnijiのに上書きする
  if( m_pJFPersonalData->IsDressUpParamRomVersionNiji() )
  {
    System::DressUpParamSaveData::OverrideNijiBngl( &m_dressUpParam );
  }

  // 動作モデルデータセットアップ
  SetupMoveModelData( jfPointIndex, pos, m_pJFPersonalData->GetSex() );

  //　この後のセットアップは 着せ替えパーツの読み込みが必要
  m_setupSeq = UPDATE_SEQ_SAFE_POS_CHECK;
  m_deleteSeq = 0;

  // 出現エフェクトのイベントハンドラ設定
  // @fix NMCat[1352]：原因：アトラクションの人物入れ替え時に、前の人物でのエフェクトが残っていて、IsSetupのStartWarpEffectを通る前に
  //                   エフェクトリスナーからEVENT_Warp_Endが返ってくる事で出現がスキップされる不具合の対処
  //                   対処方法：リスナーのセットタイミングをStartWarpEffect呼び出しと同時にする
  m_pJoinFestaEffectSystem->SetEventHandler( NULL );
}

//-----------------------------------------------------------------------------
/**
*  @brief   モデルセットアップ完了チェック
*  @param   JfModelSetupMode    セットアップモード
*  @retval  true:完了
*  @retval  false:未完了
*/
//-----------------------------------------------------------------------------
bool JFModel::IsSetup( const JfModelSetupMode mode )
{
  ApplyMoveModelToActor();

  // 着せ替えパーツの読み込み用
  switch( m_setupSeq ){
  case UPDATE_SEQ_SAFE_POS_CHECK:    //!< 配置しても安全な場所かチェック
    {
      // 主人公と衝突判定をするかどうか（フィールド終了時はしない）
      if( mode != JF_MODEL_SETUP_END )
      {
        // 主人公と衝突していたらSkip
        if( IsHitPlayer() == false )
        {
          // 予めコリジョン出しておく
          RegistDummyStaticActor();
          m_setupSeq = UPDATE_SEQ_DRESSUP_LOAD;
        }
      }
      else
      {
        m_setupSeq = UPDATE_SEQ_DRESSUP_LOAD;
      }
    }
    break;

  case UPDATE_SEQ_DRESSUP_LOAD:      //!< 着せ替えパーツの読み込み開始
    // 着せ替えパーツの非同期読み込み開始
    m_dressUpModelResourceManager.LoadDressUpPartsAsync( m_dressUpParam );
    m_setupSeq = UPDATE_SEQ_DRESSUP_LOAD_WAIT;
    break;

  case UPDATE_SEQ_DRESSUP_LOAD_WAIT: //!< 着せ替えパーツの読み込み完了待ち
    // 着せ替えパーツの非同期読み込み完了待ち
    if( m_dressUpModelResourceManager.IsDressUpPartsLoaded( m_dressUpParam ) )
    {
      // ドレスアップリソースをセットアップする
      if( m_dressUpModelResourceManager.SetupDressUpParts( m_pDressUpAllocator, m_dressUpParam ) == false )
      {// 失敗
        GFL_ASSERT(0);
        return true;
      }
      m_setupSeq = UPDATE_SEQ_DRESSUP_SETUP;
    }
    break;

  case UPDATE_SEQ_DRESSUP_SETUP:     //!< 動作モデルセットアップ
    {
      Field::Fieldmap* fieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();

      // 動作モデルアクターセットアップ
      SetupMoveModelActor();

      // 動作モデル取得しておく
      m_pRequestCharaModel = m_moveModelActor.GetMoveModel();

      // 地形コリジョンを設定
      m_pRequestCharaModel->SetCollisionSceneContainerForGround( fieldmap->GetTerrainManager()->GetCollsionScene() );

      // 最初は非表示
      m_pRequestCharaModel->SetVisible( false );
      m_setupSeq = UPDATE_SEQ_DRESSUP_SETUP_WAIT;
    }
    break;

  case UPDATE_SEQ_DRESSUP_SETUP_WAIT://!< 動作モデルセットアップ完了待ち
    // 一定時間待つ
    if( m_pRequestCharaModel->IsSetup() )
    {
      switch( mode ){
      case JF_MODEL_SETUP_NORMAL:    //! 通常時：エフェクト有り/主人公との当たり判定有り
        {
          // 出現出来るようになったのでエフェクト開始
          gfl2::math::Vector3 npcPos( m_moveModelDataArray.position[0], m_moveModelDataArray.position[1], m_moveModelDataArray.position[2] );

          // @fix NMCat[1352]：対処。詳細は別の「SetEventHandler」に記述。
          m_pJoinFestaEffectSystem->SetEventHandler( this );

          m_pJoinFestaEffectSystem->Start_Warp( npcPos );
          GFL_PRINT("JFワープエフェクト開始\n");
          m_setupSeq = UPDATE_SEQ_DRESSUP_EFFECT_WAIT;
        }
        break;
      case JF_MODEL_SETUP_NON_EFFECT://! エフェクトなし:エフェクト無し/主人公との当たり判定有り　動作モデルの表示はする
        {
          // エフェクトなしで即表示
          MoveModelVisibleOn();
          m_setupSeq = UPDATE_SEQ_END;
        }
        break;
      case JF_MODEL_SETUP_END:       //! 終了時用:エフェクト無し/主人公との当たり判定無し　動作モデルの表示はなし
        {
          // 終了時なのでエフェクトなしで即終了
          m_setupSeq = UPDATE_SEQ_END;
        }
        break;
      }
      
    }
    break;
  case UPDATE_SEQ_DRESSUP_EFFECT_WAIT://!< 出現エフェクト終了待ち
    // エフェクトイベントハンドラ通知待ち
    {
      if( mode == JF_MODEL_SETUP_END )
      {
        // 終了時なのでエフェクトなしで即終了
        m_setupSeq = UPDATE_SEQ_END;
        GFL_PRINT("JFワープエフェクトSkipして終了\n");
      }
    }
    break;
  case UPDATE_SEQ_END:               //!< 終了
    {
    }
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief   モデル破棄
*  @return  trueで破棄完了
*/
//-----------------------------------------------------------------------------
bool JFModel::Delete()
{
  GFL_PRINT("JFModel::Delete[%d]\n", m_deleteSeq);
  if( m_pRequestCharaModel != NULL )
  {
    // 着せ替え破棄＆１フレ遅れ対応
    switch( m_deleteSeq ){
    case 0:
      m_moveModelActor.UnregistActor();
      m_deleteSeq++;

      // @fix GFNMCat[5527]：原因：エフェクト開始直後にアトラクションの人物入れ替え時を行うと、
      //                   エフェクトリスナーからEVENT_AppearCharacterが通知されたタイミングでDelete完了してしまっている可能性がある。
      //                   そうすると、m_pRequestCharaModelがNULLハングする。
      //                   対処方法：破棄時にはエフェクトリスナー通知は必要ないのでNULLにする
      m_pJoinFestaEffectSystem->SetEventHandler( NULL );
      return false;
    case 1:
      m_moveModelActor.Terminate();
      m_deleteSeq++;
      return false;
    case 2:
      // 着せ替えを開放出来るか
      if( m_dressUpModelResourceManager.CanUnloadDressUpParts( m_dressUpParam ) )
      {
        // 着せ替えパーツの削除
        m_dressUpModelResourceManager.UnloadDressUpParts( m_dressUpParam );

        // 初期化
        m_pJFPersonalData = NULL;
        m_jfPointIndex = JFPoint::INDEX_NULL;
        m_pRequestCharaModel = NULL;
        m_deleteSeq = 0;
      }
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief   トラバース更新チェックをする必要があるかどうか
 *  @retval   true:チェック必要
 *  @retval   false:チェック不必要
 */
//-----------------------------------------------------------------------------
bool JFModel::IsTraverseCheckNeed()
{
  if( m_pRequestCharaModel == NULL )
  {
    return false;
  }

  // カリング有効時はカリング処理ないでやっているのでチェックする必要なし
  if( m_pRequestCharaModel->IsEnableCulilng() == true )
  {
    return false;
  }

  // 非表示時
  if( m_pRequestCharaModel->GetVisible() == false )
  {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief   強制でトラバース更新をするかどうか
 *  @retval   true:する
 *  @retval   false:しない
 */
//-----------------------------------------------------------------------------
bool JFModel::IsForceTraverseUpdate()
{
  if( m_pRequestCharaModel != NULL )
  {
    if( Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode( m_pRequestCharaModel->GetMoveCodeId() ) )
    {
      // NPC振り向き挙動中は強制更新
      if( !Field::MoveModel::FieldMoveCodeNpc::IsNoAction( m_pRequestCharaModel->GetMoveCodeWork() ) )
      {
        return true;
      }
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief   トラーバス更新の設定
 *  @param   isTraverseUpdate true:トラバース更新処理する false:トラバース更新処理しない
 */
//-----------------------------------------------------------------------------
void JFModel::SetTraverseUpdate( const bool isTraverseUpdate )
{
  if( m_pRequestCharaModel != NULL )
  {
    if( isTraverseUpdate == true )
    {
      m_pRequestCharaModel->ReSetParentNode();
    }
    else
    {
      m_pRequestCharaModel->RemoveParentNode();
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief   World座標の取得
 */
//-----------------------------------------------------------------------------
const gfl2::math::Vector3* JFModel::GetWorldPosition()
{
  GFL_ASSERT( m_pRequestCharaModel != NULL );
  if( m_pRequestCharaModel != NULL )
  {
    return &m_pRequestCharaModel->GetCharaDrawInstance()->GetPosition();
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief   WorldMatrixの取得
 */
//-----------------------------------------------------------------------------
const gfl2::math::Matrix34* JFModel::GetWorldMatrix()
{
  GFL_ASSERT( m_pRequestCharaModel != NULL );
  if( m_pRequestCharaModel != NULL )
  {
    return &m_pRequestCharaModel->GetCharaDrawInstance()->GetWorldMatrix();
    //return m_pRequestCharaModel->GetModelInstance(0)->GetWorldMatrixPtr();
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief   出現エフェクトのイベントハンドラ
 */
//-----------------------------------------------------------------------------
void JFModel::JoinFestaEffectSystem_OnEvent( JoinFestaEffectSystem* pCaller, const JoinFestaEffectSystem::IEventHandler::EventID eventID )
{
  GFL_PRINT("JoinFestaEffectSystem_OnEvent[%d]\n", eventID );
  switch( eventID ){
  case JoinFestaEffectSystem::IEventHandler::EVENT_Warp_AppearCharacter: /**<  warp:キャラを出現させるタイミング  */
    {
      // キャラ表示
      MoveModelVisibleOn();
    }
    break;
  case JoinFestaEffectSystem::IEventHandler::EVENT_Warp_End: /**<  warp:エフェクトが終了した          */
    {
      m_setupSeq = UPDATE_SEQ_END;
    }
    break;
  }
}
  

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
*  @brief   動作モデルデータセットアップ
*  @param   [in]jfPointIndex    JFPointのインデックス値
*  @param   [in]pos             表示位置
*  @param   [in]sex             性別
*/
//-----------------------------------------------------------------------------
void JFModel::SetupMoveModelData( const u32 jfPointIndex, const gfl2::math::Vector3 pos, const u32 sex )
{
  gfl2::math::Quaternion qut;

  {
    const gfl2::math::Vector3 centerPosition( 4200.0f, 0.0f, 4200.0f ); // マップの真ん中
    // 中心点からのベクトル
    gfl2::math::Vector3 vec = (pos - centerPosition).Normalize();
    f32 radian = gfl2::math::Atan2Rad(vec.x,vec.z);

    // 40度のゆらぎ
    s32 randomDeg = System::GflUse::GetPublicRand(80)-40;
    ICHI_PRINT(">ICHI ゆらぎ%d \n", randomDeg );
    f32 randomRad = gfl2::math::ConvDegToRad( static_cast<f32>(randomDeg) );
    radian += randomRad;

    ICHI_PRINT(">ICHI 動作モデルの角度 %f (ゆらぎ%f) \n", gfl2::math::ConvRadToDeg(radian), gfl2::math::ConvRadToDeg(randomRad) );
    qut.RadianYToQuaternion( radian );
  }

  //m_moveModelDataArray[jfPointIndex].type        = 0;
  m_moveModelDataArray.position[0] = pos.x;
  m_moveModelDataArray.position[1] = pos.y;// 城外、城内の床に合わせた高さが設定されている
  m_moveModelDataArray.position[2] = pos.z;
  m_moveModelDataArray.quaternion[0]= qut.x;
  m_moveModelDataArray.quaternion[1]= qut.y;
  m_moveModelDataArray.quaternion[2]= qut.z;
  m_moveModelDataArray.quaternion[3]= qut.w;
  m_moveModelDataArray.romVersion = 0;
  m_moveModelDataArray.flagwork   = 0;
  m_moveModelDataArray.flagwork_num= 0;
  m_moveModelDataArray.eventID     = JoinFestaDefine::MODEL_EVENT_ID_OFFSET + jfPointIndex;
  m_moveModelDataArray.moveCode    = static_cast<u32>(Field::PLACEMENT_MOVE_CODE_NPC_MEET_SHORT);

  // キャラクターIDの指定は必要
  m_moveModelDataArray.characterId = (sex == PM_MALE ? CHARA_MODEL_ID_PC0001_00 : CHARA_MODEL_ID_PC0002_00 );

  // チュートリアル中かどうか
  bool isTutorial = false;
  {
    Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
    if( *(pEventWork->GetEventWorkAdrs( WK_SYS_JF_TUTORIAL )) < 5 )
    {
      isTutorial = true;
    }
  }

  if( isTutorial == true )
  {
    // チュートリアルは全部で３人いてスクリプトIDも３つある
    m_moveModelDataArray.scriptID    = SCRID_JOIN_FESTA_TUTORIAL_NPC_1 + jfPointIndex;
  }
  else
  {// 通常時
    m_moveModelDataArray.scriptID    = SCRID_JOIN_FESTA_FIELD_PEOPLE;
  }

  // デフォルトアニメーション指定
  m_moveModelDataArray.defaultAnimeData.defaultMotionID = P2_BASE_FI001_WAIT01;        // 待機
  m_moveModelDataArray.defaultAnimeData.defaultMotionIDNum = 0;


  //m_moveModelDataArray[ jfPointIndex].talkCollisionOffset
  //m_moveModelDataArray[ jfPointIndex].collisionOffset
}

//-----------------------------------------------------------------------------
/**
*  @brief   動作モデルアクターセットアップ
*/
//-----------------------------------------------------------------------------
void JFModel::SetupMoveModelActor()
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

  // 着せ替えパラメータ設定
  m_fieldPlacementDataMoveModel.SetDressUpParam( &m_dressUpParam );
  // 着せ替えリソースマネージャ設定
  m_fieldPlacementDataMoveModel.SetDressUpModelResourceManager( &m_dressUpModelResourceManager );

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

//-----------------------------------------------------------------------------
/**
 *  @brief   動作モデル表示
*/
//-----------------------------------------------------------------------------
void JFModel::MoveModelVisibleOn()
{
  // 表示までのダミーコリジョンは破棄
  UnregistDummyStaticActor();

  // キャラ表示
  GFL_ASSERT( m_pRequestCharaModel );
  if( m_pRequestCharaModel != NULL )
  {
    m_pRequestCharaModel->SetVisible( true );
  }

#if PM_DEBUG
#if defined( DEBUG_ONLY_FOR_ichiraku_katsuhiko )
  // 着せ替え組み合わせチェック
  {
    bool ret = m_dressUpModelResourceManager.GetSetupDressUpModelResourceManagerCore()->CheckDressUpCombination( m_dressUpParam );
    GFL_ASSERT_MSG( ret, "着せ替え組み合わせチェックエラー");
  }
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 *  @brief   主人公との衝突判定するダミーアクターを追加
 */
//-----------------------------------------------------------------------------
void JFModel::RegistDummyStaticActor()
{
  // @fix NMcat[223]　自機と動作モデルが被る問題

  Field::FieldResident* fieldResident = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident();
  BaseCollisionScene*   pCollisionScene = fieldResident->GetCollisionScene( Field::FieldResident::COLLISION_SCENE_STATIC_MODEL );

  // アクターとアクターが持つ衝突形状を作成
  GFL_ASSERT(m_pStaticActor == NULL);
  if( m_pStaticActor == NULL )
  {
    m_pStaticActor = pCollisionScene->CreateStaticActor( m_pDummyActorAllocator, m_pDummyActorHeap );
  }
  m_pStaticActor->CreateCylinder( gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), 45.5f, 200.0f );


  // NPC vs Playerのシーンにアクターを登録
  pCollisionScene->RegistStaticActor( m_pStaticActor );
}

//-----------------------------------------------------------------------------
/**
 *  @brief   主人公との衝突判定するダミーアクターを削除
 */
//-----------------------------------------------------------------------------
void JFModel::UnregistDummyStaticActor()
{
  Field::FieldResident* fieldResident = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident();
  BaseCollisionScene*   pCollisionScene = fieldResident->GetCollisionScene( Field::FieldResident::COLLISION_SCENE_STATIC_MODEL );

  // NPC vs Playerのシーンにアクターを解除
  GFL_ASSERT( m_pStaticActor );
  if( m_pStaticActor != NULL )
  {
    pCollisionScene->UnRegistStaticActor( m_pStaticActor );
  }
  GFL_SAFE_DELETE( m_pStaticActor );
}

//-----------------------------------------------------------------------------
/**
 *  @brief   主人公との衝突判定するダミーアクターのコリジョンシステムにデータ適用
*/
//-----------------------------------------------------------------------------
void JFModel::ApplyMoveModelToActor()
{
  // コリジョン結果をクリア
  if( m_pStaticActor != NULL )
  {
    gfl2::math::Vector3   npcPos( m_moveModelDataArray.position[0], m_moveModelDataArray.position[1], m_moveModelDataArray.position[2] );
    m_pStaticActor->SetPosition( npcPos );
    m_pStaticActor->InitCollisionResult();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief   主人公と衝突しているかチェック
*  @retun   trueで衝突している
*/
//-----------------------------------------------------------------------------
bool JFModel::IsHitPlayer() const
{
  Field::Fieldmap* fieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
  gfl2::math::Vector3 playerPos = fieldmap->GetPlayerPosition(); 
  gfl2::math::Vector3 npcPos( m_moveModelDataArray.position[0], m_moveModelDataArray.position[1], m_moveModelDataArray.position[2] );
  gfl2::math::Vector3 len = playerPos - npcPos;


  // @fix GFNMcat[706]：カイリキーは主人公より大きいので判定も大きくする
  Field::MoveModel::FieldMoveModelPlayer* pFieldMoveModelPlayer = fieldmap->GetPlayerCharacter();
  const bool isRide = ( pFieldMoveModelPlayer != NULL ) ? pFieldMoveModelPlayer->IsRideMoveModel() : false ;

  // 衝突判定距離
  const f32 hitLength = isRide ? RIDE_HIT_LENGTH : PLAYER_HIT_LENGTH; 

  // 衝突判定
  if( len.Length() <= hitLength )
  {
    return true;
  }
  return false;
}

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

