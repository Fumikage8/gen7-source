//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		EventPokemonRide.h
 *	@brief  イベントポケモンライド
 *	@author	tomoya takahashi
 *	@date		2015.06.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <Thread/include/gfl2_ThreadStatic.h>


#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

#include "GameSys/include/GameData.h"

#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include  "Field/FieldStatic/include/FieldAttribute.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRideLight.h"

//下画面
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"
#include "App/FieldMenu/include/FieldMenuFrame.h"
#include "App/FieldMenu/include/FieldMenuSubWindowProc.h"


#include "Field/FieldRo/include/Collision/BaseShape.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

// ゾーン
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/FieldWorld.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include <niji_conv_header/field/zone/zone_id.h>

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>

// モーションのインデックス
#include <niji_conv_header/field/chara/p1_base_fi.h>

// ライドメッセージ
#include <niji_conv_header/message/msg_ride_pokelist.h>

// ScrID
#include <niji_conv_header/field/script/inc/field_event.inc>

// カメラ
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"

#include "niji_reference_files/script/FieldPokemonFureaiScriptTypes.h"

GFL_NAMESPACE_BEGIN(Field);


//--------------------------------------------------------------
// 演出クラス
//--------------------------------------------------------------

/**
 *  @class ライドONOFFのエフェクトのController
 */
class RideOnOffCameraController
{
  GFL_FORBID_COPY_AND_ASSIGN(RideOnOffCameraController); //コピーコンストラクタ＋代入禁止

public:

  struct Type
  {
    enum Value
    {
      TYPE_SEA_ON,    // 陸->海
      TYPE_SEA_OFF,   // 海->陸
      TYPE_MAX
    };
  };
  
  struct State
  {
    enum Value
    {
      NONE,           // 未開始
      EV_CAMETA_MOVE, // イベントカメラ制御による移動中
      END             // イベントカメラ制御による移動、完了(陸->海の場合はフィールドカメラへの補間で動いていることがある)
    };
  };

  RideOnOffCameraController() :
    m_Type( Type::TYPE_MAX )
    ,m_State( State::NONE )
    ,m_MoveFrameCounter( 0 )
    ,m_pController( NULL )
    ,m_pCameraManager( NULL )
  {

  }

  virtual ~RideOnOffCameraController()
  {
    Terminate();
  }

  void Initialize( GameSys::GameManager* pGameManager, gfl2::heap::HeapBase* pHeap )
  {
    m_pCameraManager = pGameManager->GetFieldmap()->GetCameraManager();

    Camera::Controller::ControllerTypeEventScript::InitializeDescription initDesc;
    initDesc.dummy = 0; // 引数はない
    m_pController = GFL_NEW( pHeap )Camera::Controller::ControllerTypeEventScript( initDesc );
  }

  void Terminate()
  {
    m_pCameraManager = NULL;
    GFL_SAFE_DELETE( m_pController );   // ここまで残っていたら使われていない
  }

  /**
  *  @brief カメラ移動を開始する
  *
  *  @param rStartPos 開始座標
  *  @param rEndPos   終了座標
  */
  void Start( const gfl2::math::Vector3& rStartPos, const gfl2::math::Vector3& rEndPos, Type::Value type )
  {
    m_MoveFrameCounter = 0;
    m_Type = type;
    m_State = State::EV_CAMETA_MOVE;

    // 陸->海
    if( m_Type == Type::TYPE_SEA_ON )
    {
      //
      // 現在のカメラで着地点まで移動し、その後着地点のカメラに補間する
      //


      // @fix MMcat[434]:フォーカス時になみのりを解除すると、カメラがカクつく対処。「陸～海間のライド切り替え演出」の冒頭で、TRフォーカス処理をリセットするように修正
      m_pCameraManager->SetSupportAction(Camera::SupportAction::STOP);
      if (m_pCameraManager->GetFocus())
      {
        m_pCameraManager->GetFocus()->EventStop(Camera::Area::Focus::EventStopBit::FOCUS | Camera::Area::Focus::EventStopBit::SE);
        m_pCameraManager->GetFocus()->ResetForRideEvent();
      }


      // FieldUnitの基本パラメーターをEventUnitにコピー
      m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT )->CopyBaseParam(
        *m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD )
        );

      // フィールドカメラをジャンプ先に上書きする
      m_pCameraManager->GetTarget()->OverWrite( rEndPos );
      m_pCameraManager->GetTarget()->Fix();
      m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD )->Fix();

      // 使用カメラをField->Eventに変更( Fieldはスリープ状態になる )
      m_pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::EVENT );

      // 操作コントローラーをイベントスクリプト用コントローラに変更
      m_pCameraManager->GetMainGamePlayCamera()->ChangeController( m_pController );

      // フィールドカメラをジャンプ先に上書きする
      gfl2::math::Vector3 targetToCameraOffset;
      gfl2::math::Vector3 playerToCameraTargetOffset;
      gfl2::math::Vector3 targetPosition;
      gfl2::math::Vector3 cameraPosition;
      m_pCameraManager->GetMainGamePlayCamera()->GetCameraAndTargetPosition( &targetPosition, &cameraPosition );
      targetToCameraOffset = cameraPosition - targetPosition;
      playerToCameraTargetOffset = targetPosition - rStartPos;

      f32 fovY = Camera::Controller::ControllerTypeEventScript::DEF_FOVY_RAD;
      if (m_pCameraManager->GetCameraByUnitNo(Camera::UnitNo::FIELD)->GetBaseCamera())
      {
        fovY = gfl2::math::ConvRadToDeg(m_pCameraManager->GetCameraByUnitNo(Camera::UnitNo::FIELD)->GetBaseCamera()->GetFovy());
      }

      // 移動設定
      const EventPokemonRideTool::CAMERA_SETTING_RAND_TO_SEA* cpCameraSetting = &EventPokemonRideTool::s_CameraMoveSettingRandToSea;
      gfl2::math::Vector3 newTargetPosition;
      newTargetPosition = rEndPos + playerToCameraTargetOffset;
      m_pController->Move(
        newTargetPosition + targetToCameraOffset, 
        newTargetPosition,
        cpCameraSetting->moveFrame,
        cpCameraSetting->moveEaseFunc,
        Camera::Controller::ControllerTypeEventScript::DEF_BANK_DEG,
        fovY,
        cpCameraSetting->startFrame
        );
    }
    // 海->陸
    else
    {
      //
      // 現在のカメラから着地点のカメラに補間する
      //

      // @fix MMcat[434]:フォーカス時になみのりを解除すると、カメラがカクつく対処。「陸～海間のライド切り替え演出」の冒頭で、TRフォーカス処理をリセットするように修正
      m_pCameraManager->SetSupportAction(Camera::SupportAction::STOP);
      if (m_pCameraManager->GetFocus())
      {
        m_pCameraManager->GetFocus()->EventStop(Camera::Area::Focus::EventStopBit::FOCUS | Camera::Area::Focus::EventStopBit::SE);
        m_pCameraManager->GetFocus()->ResetForRideEvent();
      }

      // FieldUnitの基本パラメーターをEventUnitにコピー
      m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT )->CopyBaseParam(
        *m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD )
        );

      // フィールドカメラをジャンプ先に上書きする
      m_pCameraManager->GetTarget()->OverWrite( rEndPos );
      m_pCameraManager->GetTarget()->Fix();
      m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD )->Fix();
      
      // ジャンプ先の状態を取得
      gfl2::math::Vector3 targetPosition;
      gfl2::math::Vector3 cameraPosition;
      m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD )->GetCameraAndTargetPosition( &targetPosition, &cameraPosition );
      f32 fovY = Camera::Controller::ControllerTypeEventScript::DEF_FOVY_RAD;
      if (m_pCameraManager->GetCameraByUnitNo(Camera::UnitNo::FIELD)->GetBaseCamera())
      {
        fovY = gfl2::math::ConvRadToDeg( m_pCameraManager->GetCameraByUnitNo(Camera::UnitNo::FIELD)->GetBaseCamera()->GetFovy() );
      }

      // 使用カメラをField->Eventに変更( Fieldはスリープ状態になる )
      m_pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::EVENT );

      // 操作コントローラーをイベントスクリプト用コントローラに変更
      m_pCameraManager->GetMainGamePlayCamera()->ChangeController( m_pController );

      // 移動設定
      const EventPokemonRideTool::CAMERA_SETTING_SEA_TO_RAND* cpCameraSetting = &EventPokemonRideTool::s_CameraMoveSettingSeaToRand;
      m_pController->Move(
        cameraPosition, 
        targetPosition,
        cpCameraSetting->moveFrame,
        cpCameraSetting->moveEaseFunc,
        Camera::Controller::ControllerTypeEventScript::DEF_BANK_DEG,
        fovY,
        cpCameraSetting->startFrame
        );
    }
  }


  /**
  *  @brief カメラを更新する
  */
  void Update( void )
  {
    switch( m_State )
    {
    case State::EV_CAMETA_MOVE:
      {
        // 陸->海
        // @note 海とのカメラ補間を自機操作開始までに終了するフローに変更。「移動中に大幅な補間が走ると不親切」「カメラが2段階変化しており違和感がある」「移動開始時に奥行きが見えない」を解消する為。
        // @note 移動時に補間してしまわないのは、「狭い->広い」への遷移で情報変化量が多すぎるため
        if( m_Type == Type::TYPE_SEA_ON )
        {
          // 指定フレーム立ったら、フィールドカメラへの補間に切り替える
          if( m_MoveFrameCounter >= EventPokemonRideTool::s_CameraMoveSettingRandToSea.toFieldStartFrame )
          {
            EvCameraMoveEnd();
          }
        }
        // 海->陸
        else
        {
          if( !m_pController->IsMoving() )
          {
            // 移動完了
            EvCameraMoveEnd();
          }
        }
        
        m_MoveFrameCounter++;
        break;
      }
    }
  }

  /**
   *  @brief  自機の座標更新が終わった段階で行う処理
   *  @note   後始末、カメラ移動が完了した後に呼ばれる事もある
   */
  void PlayerMoveEnd()
  {
    // 座標上書きをクリア
    m_pCameraManager->GetTarget()->ClearOverWrite();
  }
  
  /**
   *  @brief カメラ移動が終了しているか?
   *  @retval true 終了
   */
  b32 IsEnd() const
  {
    return ( m_State == State::END );
  }

private:

  /**
  *  @brief カメラ移動を終了する
  */
  void EvCameraMoveEnd( void )
  {
    if( m_State != State::EV_CAMETA_MOVE )
    {
      return;
    }

    m_State = State::END;
    m_MoveFrameCounter = 0;

    // イベントコントローラー破棄＆フィールドカメラ復帰
    m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT )->ChangeController( NULL );  // ここでメモリ開放される
    m_pController = NULL;
    m_pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::FIELD );

    // 陸->海の場合はフィールドへの補間設定
    if( m_Type == Type::TYPE_SEA_ON )
    {
      const EventPokemonRideTool::CAMERA_SETTING_RAND_TO_SEA* cpCameraSetting = &EventPokemonRideTool::s_CameraMoveSettingRandToSea;
      
      // 補間設定
      if( cpCameraSetting->toFieldIpoFrame > 0 )
      {
        Camera::CameraUnit* pCameraUnitEvent = m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT );
        Camera::CameraUnit* pCameraUnitField = m_pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD );

        // 補間設定
        gfl2::math::Vector3 targetPosition;
        gfl2::math::Vector3 cameraPosition;
        targetPosition = pCameraUnitEvent->GetBaseCamera()->GetTargetPosition();
        cameraPosition = pCameraUnitEvent->GetBaseCamera()->GetPosition();
        pCameraUnitField->SetInterpolation( targetPosition, cameraPosition, cpCameraSetting->toFieldIpoFrame, cpCameraSetting->toFieldIpoEaseFunc );
      }
    }
  }

  // 内部
  Type::Value                                           m_Type;             // 移動形式
  State::Value                                          m_State;            // 状態制御
  Field::Camera::Controller::ControllerTypeEventScript* m_pController;
  u32                                                   m_MoveFrameCounter; // 移動時間計測

  // 外部
  Field::Camera::CameraManager*                         m_pCameraManager;
};
  
  /**
 *  @class ライドONOFFのエフェクトのController
 */
class RideOnOffEffectController
{
  GFL_FORBID_COPY_AND_ASSIGN(RideOnOffEffectController); //コピーコンストラクタ＋代入禁止
public:

  enum Type
  {
    TYPE_LAND,
    TYPE_SEA,
    TYPE_MAX
  };

public:

  RideOnOffEffectController() : 
    m_pEffectManager(NULL),
    m_pEffect(NULL)
  {
  }

  virtual ~RideOnOffEffectController()
  {
    // 念のため
    this->EndEffect();
    this->Terminate();
  }

  /**
   *  @brief 初期化
   */
  void Initialize( GameSys::GameManager* pGameManager )
  {
    m_pEffectManager = pGameManager->GetFieldmap()->GetEffectManager();
    m_pEffect = NULL;

    m_pEffectManager->LoadDynamicEffectResource( Effect::EFFECT_TYPE_RIDE_APPER_LAND, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ), true );
    m_pEffectManager->SetupDynamicEffectResource( Effect::EFFECT_TYPE_RIDE_APPER_LAND, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );
    m_pEffectManager->LoadDynamicEffectResource( Effect::EFFECT_TYPE_RIDE_APPER_SEA, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ), true );
    m_pEffectManager->SetupDynamicEffectResource( Effect::EFFECT_TYPE_RIDE_APPER_SEA, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );
  }

  void Terminate()
  {
    if( m_pEffectManager )
    {
      m_pEffectManager->UnLoadDynamicEffectResource( Effect::EFFECT_TYPE_RIDE_APPER_LAND, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );
      m_pEffectManager->UnLoadDynamicEffectResource( Effect::EFFECT_TYPE_RIDE_APPER_SEA, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );
      m_pEffectManager = NULL;
    }
  }

  /** 
   *  @brief エフェクト開始
   */
  void StartEffect( Type type, const gfl2::math::Vector3& pos )
  {
    Sound::PlaySE( SEQ_SE_FLD_RIDE_COMMON_FLASHBALL );

    static const Effect::Type sc_EffectType[ TYPE_MAX ] = 
    {
      Effect::EFFECT_TYPE_RIDE_APPER_LAND,
      Effect::EFFECT_TYPE_RIDE_APPER_SEA
    };

    if( type < TYPE_MAX )
    {

      m_pEffect = reinterpret_cast<Effect::IEffectRideBase*>(m_pEffectManager->CreateEffect( sc_EffectType[type], pos ));
    }
    else
    {
      GFL_ASSERT( type < TYPE_MAX ); // タイプ不正
      m_pEffect = NULL;
    }

  }


  /** 
   *  @brief エフェクトにより、プレイヤーが見えなくなるのを待つ
   */
  b32 WaitHide() const
  {
    // エフェクト生成に失敗していたら問答無用に次へ
    if( !m_pEffect )
    {
      return true;
    }

    // 決まったフレーム進んだら隠れている
    static u32 sc_Frame = 4;

    if( m_pEffect->GetFrame() >= sc_Frame )
    {
      return true;
    }

    return false;
  }

  /**
   *  @brief エフェクトの終了を待つ
   */
  b32 WaitEnd() const
  {
    // エフェクト生成に失敗していたら問答無用に終了
    if( !m_pEffect )
    {
      return true;
    }

    return !m_pEffect->IsAlive();
  }

  /**
   *  @brief エフェクトの破棄
   */
  void EndEffect()
  {
    if( m_pEffect )
    {
      m_pEffectManager->DeleteEffect( m_pEffect );
      m_pEffect = NULL;

    }
  }

private:

  Effect::EffectManager* m_pEffectManager;
  Effect::IEffectRideBase * m_pEffect;

};

/**
 *  @class ライドONOFFのライドモデルのController
 */
class RideOnOffRideModelController
{
  GFL_FORBID_COPY_AND_ASSIGN(RideOnOffRideModelController); //コピーコンストラクタ＋代入禁止
public:

  RideOnOffRideModelController( Field::MoveModel::FieldMoveModelManager* pCharaModelManager, Field::MoveModel::FieldMoveModelPlayer * pPlayer ) : 
     m_pFieldCharaModelManager(pCharaModelManager)
    ,m_pPlayer(pPlayer)
  {
  }

  virtual ~RideOnOffRideModelController()
  {
  }

  /**
   *  @brief ライドモデルを非表示にする
   */
  void HideModel( bool is_ride2ride )
  {
    Field::MoveModel::FieldMoveModel* pRideModel = m_pPlayer->GetOnMoveModel();
    if( pRideModel )
    {
      pRideModel->SetVisible(false);
    }

    if( is_ride2ride == false ){
      // BGMを元に戻す RideToRideの場合は元に戻さない
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldSound()->ChangeFieldBGMByPlayerWalk();
    }
  }

  /**
   *  @brief ライドモデルを削除する
   */
  void DeleteModel()
  {
    RIDE_POKEMON_ID pokemonId = EventPokemonRideTool::GetPokemonRideOnID(m_pPlayer); 

    // プレイヤーをおろす
    m_pPlayer->RideOffMoveModel();

    // 破棄
    m_pFieldCharaModelManager->TerminateMoveModelWorkResource ( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON );

    // リソース破棄
    m_pFieldCharaModelManager->Unload( EventPokemonRideTool::GetRIDE_POKEMON_ID_to_CharactorId(pokemonId)  );
  }

  /**
   *  @brief ライドモデルを読み込む
   */
  void Load( RIDE_POKEMON_ID pokemonId )
  {
    // ライドポケモンの非同期ロード開始
    m_pFieldCharaModelManager->LoadDynamicAsync( Field::MoveModel::FieldMoveModelManager::DYNAMIC_RES_LOAD_INDEX_RIDE, EventPokemonRideTool::GetRIDE_POKEMON_ID_to_CharactorId(pokemonId) );
  }

  /**
   *  @brief 読み込み待ち
   */
  b32 WaitLoad( RIDE_POKEMON_ID pokemonId ) const
  {
    return m_pFieldCharaModelManager->IsLoaded( EventPokemonRideTool::GetRIDE_POKEMON_ID_to_CharactorId(pokemonId) );
  }

  /**
   *  @brief ライドを生成してプレイヤーを乗せる
   */
  void CreateAndOn(RIDE_POKEMON_ID pokemonId)
  {
    u32 charactor_id = EventPokemonRideTool::GetRIDE_POKEMON_ID_to_CharactorId(pokemonId);
    // ライドポケモンの生成
    {
      m_pFieldCharaModelManager->SetupModel( charactor_id );

      Field::MoveModel::FieldMoveModelHeaderWork     ride_headerWork;
      Field::MoveModel::FieldMoveModelHeaderResource ride_headerResource;

      EventPokemonRideTool::CreatePokemonRideWorld( pokemonId, &ride_headerWork, &ride_headerResource );

      m_pFieldCharaModelManager->TerminateMoveModelWorkResource ( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON );
      m_pFieldCharaModelManager->InitializeMoveModelWork        ( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON , &ride_headerWork     );
      m_pFieldCharaModelManager->InitializeMoveModelResource    ( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON , &ride_headerResource );
    }
    // プレイヤーを乗せる
    Field::MoveModel::FieldMoveModel* pCharaModel = m_pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON );
    m_pPlayer->RideOnMoveModel( pCharaModel );


    // SEとBGMの再生
    PlayRidePokeSE(pokemonId);
    PlayBGMPokeRide(pokemonId);
  }



  /**
   *  @brief 乗る際のポケモンの泣き声再生
   */
  static void PlayRidePokeSE( RIDE_POKEMON_ID pokemonID )
  {
    switch (pokemonID)
    {
    case Field::RIDE_KENTAROSU:
      {
        // SE再生
        Sound::PlaySE( SEQ_SE_FLD_RIDE_KENTAUROS_PV_RIDEON );
      }
      break;
    case Field::RIDE_MUURANDO:
      {
        // SE再生
        Sound::PlaySE( SEQ_SE_FLD_RIDE_MOOLAND_PV_RIDEON );
      }
      break;
    case Field::RIDE_SAMEHADA:
      {
        // SE再生
        Sound::PlaySE( SEQ_SE_FLD_RIDE_SAMEHADAR_PV_RIDEON );
      }
      break;
    case Field::RIDE_LAPLACE:
      {
        // SE再生
        Sound::PlaySE( SEQ_SE_FLD_RIDE_RAPLACE_PV_RIDEON );
      }
      break;

    case Field::RIDE_KAIRIKY:
      {
        // SE再生
        Sound::PlaySE( SEQ_SE_FLD_RIDE_KAIRIKY_PV_RIDEON );
      }
      break;
    
    case Field::RIDE_ROBA2:
      {
        // SE再生
        Sound::PlaySE( SEQ_SE_FLD_RIDE_BANBADRO_PV_RIDEON );
      }
      break;

    default:
      break;
    }
  }

  /**
   *  @brief ライドBGMの再生
   */
  static void PlayBGMPokeRide( RIDE_POKEMON_ID pokemonID )
  {
    FieldSound* p_sound = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldSound();

    switch (pokemonID)
    {
    case Field::RIDE_KENTAROSU:
      {
        p_sound->ChangeFieldBGMByPokeRideLand();
      }
      break;

    case Field::RIDE_MUURANDO:
      {
        p_sound->ChangeFieldBGMByPokeRideLand();
      }
      break;

    case Field::RIDE_SAMEHADA:
      {
        p_sound->ChangeFieldBGMByPokeRideSea();
      }
      break;

    case Field::RIDE_LAPLACE:
      {
        p_sound->ChangeFieldBGMByPokeRideSea();
      }
      break;

    case Field::RIDE_KAIRIKY:
      {
        p_sound->ChangeFieldBGMByPokeRideLand();
      }
      break;

    case Field::RIDE_ROBA2:
      {
        p_sound->ChangeFieldBGMByPokeRideLand();
      }
      break;

    default:
      break;
    }
  }



private:
  Field::MoveModel::FieldMoveModelManager*      m_pFieldCharaModelManager;
  Field::MoveModel::FieldMoveModelPlayer*       m_pPlayer;
};

/**
 *  @class ライドONOFFのプレイヤーモデルのController
 */
class RideOnOffPlayerModelController
{
  GFL_FORBID_COPY_AND_ASSIGN(RideOnOffPlayerModelController); //コピーコンストラクタ＋代入禁止
public:

  RideOnOffPlayerModelController() : 
     m_pGameManager(NULL)
    ,m_pFieldCharaModelManager(NULL)
    ,m_pPlayer(NULL)
    ,m_IsAnimationStart(false)
    ,m_cpDressUpParam(NULL)
    ,m_JumpPosStart(0,0,0)
    ,m_JumpPosEnd(0,0,0)
    ,m_JumpPosEndAdjustGround(0,0,0)
  {
  }

  virtual ~RideOnOffPlayerModelController()
  {
    // 念のため
    this->EndJump();
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

    //
    m_IsAnimationStart = false;
  }

  /**
   *  @brief 波乗り壁関連の対応作業
   */
  void SetupNaminoriWall( b32 is_on )
  {
    // ジャンプ開始座標の保存
    // (水中から陸に戻るときのみ使用)
    GetNaminoriWallPushBack( m_pPlayer, &m_JumpPosStart );

    // ジャンプ後座標の保存
    if( !GetNaminoriWallJump( m_pPlayer, &m_JumpPosEnd, is_on ) )
    {
      GFL_ASSERT_STOP( 0 ); // 乗れない状態でここにきた。
    }

    // ジャンプ後接地済み座標の保存
    if( !GetNaminoriWallJumpAdjustGround( m_pPlayer, m_JumpPosEnd, &m_JumpPosEndAdjustGround ) )
    {
      GFL_ASSERT( 0 );
      m_JumpPosEndAdjustGround = m_JumpPosEnd;
    }
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
    // 持っているサブオブジェクトを外す
    for( s32 i=0 ; i<Field::MoveModel::FIELD_MOVE_MODEL_SUB_OBJECT_CNT ; i++ )
    {
      m_pPlayer->SetSubobject(i,NULL,NULL);
    }


    // 見た目の破棄
    m_pFieldCharaModelManager->TerminateMoveModelResource( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );

    // 着せ替えパーツリソースノードの削除
    m_pFieldCharaModelManager->UnloadDressUpParts(*m_cpDressUpParam);
  }

  /**
   *  @brief ジャンプモーション開始
   */
  void PlayJump( b32 is_on )
  {
    gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();

    // 陸に上るときのみ。
    if(is_on == false)
    {
      // 目の前の壁の基準点にまずは移動。
      srt.translate = m_JumpPosStart;
      m_pPlayer->SetLocalSRT(srt);
    }


    // 現座標
    srt = m_pPlayer->GetLocalSRT();

    // 着地点座標取得
    gfl2::math::Vector3 jumpPos = m_JumpPosEnd - srt.translate;

    // 壁の押し返しを無視
    m_pPlayer->GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_NAMINORI )->SetActive(false);

    // 平面の変更量にする。
    jumpPos.SetY(0.0f);
    // 
    if( jumpPos.LengthSq() )
    {
      jumpPos = jumpPos.Normalize();

      gfl2::math::Vector3 z(0,0,1);
      gfl2::math::Vector3 axis(0,0,0);
      f32 acos = z.Dot( jumpPos );
      axis     = z.Cross( jumpPos );

      if( gfl2::math::IsSimilar( acos, 1.0f ) )
      {
        axis = gfl2::math::Vector3(0,1,0);
        acos = 1.0f;
      }
      else if( gfl2::math::IsSimilar( acos, -1.0f ) )
      {
        axis = gfl2::math::Vector3(0,1,0);
        acos = -1.0f;
      }

      srt.rotation = gfl2::math::Quaternion( axis, gfl2::math::AcosRad( acos ) );
      m_pPlayer->SetLocalSRT(srt);

      // ジャンプ開始
      if(is_on)
      {
        m_pPlayer->GetCharaDrawInstance()->ChangeAnimation( P2_BASE_FI120_SEARIDEJUMP_START );
      }
      else
      {
        m_pPlayer->GetCharaDrawInstance()->ChangeAnimation( P2_BASE_FI121_SEARIDEJUMP_END );
      }

      m_IsAnimationStart = true;
    }
    else
    {
      GFL_ASSERT(0);  // すでにジャンプ先にいる？？？

      m_IsAnimationStart = false;
    }
  }

  /**
   *  @brief ジャンプモーション終了待ち
   */
  b32 WaitJump() const
  {
    // アニメーション再生していないのであれば常にtrue
    if( !m_IsAnimationStart )
    {
      return true;
    }


    // アニメーションが終了フレームに到達するのを待つ
    if( m_pPlayer->GetCharaDrawInstance()->IsAnimationLastFrame() )
    {
      return true;
    }
    return false;
  }

  /**
   *  @breif ジャンプ終了座標を設定する。
   *          後始末をする。
   */
  void EndJump()
  {
    if(m_IsAnimationStart)
    {
      gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();

      // 目の前の壁の基準点にまずは移動。
      srt.translate = m_JumpPosEnd;
      m_pPlayer->SetLocalSRT(srt);

      TOMOYA_PRINT( "JumpPos AllEnd [%f,%f,%f]\n", srt.translate.GetX(), srt.translate.GetY(), srt.translate.GetZ() );

      // 波乗り壁の押し返し有効
      m_pPlayer->GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_NAMINORI )->SetActive(true);

      m_IsAnimationStart = false; 
    }
  }

  /**
   *  @brief ジャンプの開始座標の取得
   */
  const gfl2::math::Vector3& GetJumpStartPos()
  {
    return m_JumpPosStart;
  }
  
  /**
   *  @brief ジャンプの終了座標の取得
   */
  const gfl2::math::Vector3& GetJumpEndPos()
  {
    return m_JumpPosEnd;
  }

  /**
   *  @brief ジャンプの終了座標(接地済み)の取得
   */
  const gfl2::math::Vector3& GetJumpEndPosAdjustGround()
  {
    return m_JumpPosEndAdjustGround;
  }

  /**
   *  @brief ジャンプ中のエフェクト開始待ち
   */
  b32 WaitJumpMidstreamEffect()
  {
    static const f32 sc_LEN = 150.0f * 150.0f;

    // 着地基準座標の一定の距離範囲に入ったらエフェクト開始
    gfl2::math::Vector3 length = m_pPlayer->GetCharaDrawInstance()->GetPosition() - m_JumpPosEnd;
    length.SetY(0.0f);

    TOMOYA_PRINT( "player Pos (%f,%f,%f) EndPos(%f,%f,%f) Length (%f)\n", 
      m_pPlayer->GetCharaDrawInstance()->GetPosition().GetX(),
      m_pPlayer->GetCharaDrawInstance()->GetPosition().GetY(),
      m_pPlayer->GetCharaDrawInstance()->GetPosition().GetZ(),
      m_JumpPosEnd.GetX(),
      m_JumpPosEnd.GetY(),
      m_JumpPosEnd.GetZ(),
      length.Length()
      );

    if( length.LengthSq() < sc_LEN )
    {
      return true;
    }

    return false;
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



  /**
   *  @brief 海壁の押し返し　座標設定処理
   */
  static void GetNaminoriWallPushBack( Field::MoveModel::FieldMoveModelPlayer* pPlayer, gfl2::math::Vector3* p_out_jump_pos )
  {
    // 必ずその場開始。
    gfl2::math::Vector3 playerPos        = pPlayer->GetLocalSRT().translate;
    gfl2::math::Vector3 adjustVec        = playerPos - pPlayer->GetDynamicActor(MoveModel::COLLISION_TYPE_EVENT_NAMINORI)->GetPosition();
    gfl2::math::Vector3 adjustVecNormal  = adjustVec.Normalize();
    f32                 adjustLen        = adjustVec.Length();
    f32                 distance         = (static_cast<SphereShape*>( pPlayer->GetDynamicActor(MoveModel::COLLISION_TYPE_EVENT_NAMINORI)->GetShape() )->m_r) - adjustLen;
    gfl2::math::Vector3 addPos           = ( adjustVecNormal * distance );
    *p_out_jump_pos = playerPos + addPos;
  }

  /**
   *  @brief 壁の向こうに放り出す。　座標設定処理
   */
  static b32 GetNaminoriWallJump( Field::MoveModel::FieldMoveModelPlayer* pPlayer, gfl2::math::Vector3* p_out_adjustVec, b32 is_on_ride )
  {
    gfl2::math::Vector3 playerPos        = pPlayer->GetLocalSRT().translate;
    gfl2::math::Vector3 adjustVec        = playerPos - pPlayer->GetDynamicActor(MoveModel::COLLISION_TYPE_EVENT_NAMINORI)->GetPosition();
    gfl2::math::Vector3 adjustVecNormal  = adjustVec.Normalize();
    f32                 adjustLen        = adjustVec.Length();
    f32                 distance         = (static_cast<SphereShape*>( pPlayer->GetDynamicActor(MoveModel::COLLISION_TYPE_EVENT_NAMINORI)->GetShape() )->m_r) - adjustLen;
    // 波乗り壁基準面から150.0fの場所にジャンプ
    gfl2::math::Vector3 calc_adjustVec = playerPos + ( adjustVecNormal * distance ) + ( adjustVecNormal * 150.0f );

    // @fix NMCat[865]   壁に接触している状態で水上ライドを行った際、ライド後に壁から離れた位置へ瞬間的に移動する
    // 壁に押し出された座標を求める。
    if( pPlayer->IsEnableGroundRide(calc_adjustVec, p_out_adjustVec, is_on_ride) ) //@fix GFNMCat[2704] 波乗りを起動できるが、降りられない場所がある 波乗りから降りるときは通常のコリジョン幅でチェック
    {
      // 波乗りジャンプは押し返しで座標が変わることを許容しない。 @fix NMCat[1277] ポニのげんや：地上にいるNPC近くに水上から復帰すると、自機が強制的に移動する NMCat[865]のエンバグ
      gfl2::math::Vector3 subVector = (calc_adjustVec - *p_out_adjustVec);
      if( subVector.LengthSq() < EPSILON )
      {
        // 乗れるパターン
        return true;
      }
      else
      {
        // 座標が変わっているため乗れない。
        *p_out_adjustVec = calc_adjustVec; // 元の座標に戻しておく。
        return false;
      }
    }
    else
    {
      // 乗れないパターン
      *p_out_adjustVec = calc_adjustVec; // 元の座標に戻しておく。
      return false;
    }
  }



  static b32 GetNaminoriWallJumpAdjustGround( Field::MoveModel::FieldMoveModelPlayer* pPlayer, const gfl2::math::Vector3& rJumpEndPos, gfl2::math::Vector3* p_out_adjustVec )
  {
    // 接地を考慮した移動後の座標に変換する
    if( pPlayer->GetCollisionSceneContainerForGround() )
    {
      // 遥か上空から地下深くまでレイキャストをするように
      f32 startEndOffset = 1000000.0f;

      // 地面の高さを取得
      RaycastCustomCallback::HIT_DATA hitData;
      gfl2::math::Vector4 start( rJumpEndPos.x, rJumpEndPos.y+startEndOffset, rJumpEndPos.z,0.0f);
      gfl2::math::Vector4   end( rJumpEndPos.x, rJumpEndPos.y-startEndOffset, rJumpEndPos.z,0.0f);
      if( pPlayer->GetCollisionSceneContainerForGround()->RaycastFromStaticActorsMesh(start,end,&hitData) )
      {
        *p_out_adjustVec = gfl2::math::Vector3(hitData.intersection.x,hitData.intersection.y,hitData.intersection.z);
        p_out_adjustVec->y += Field::MoveModel::FieldMoveModel::GROUND_OFFSET;
        return true;
      }
    }
    *p_out_adjustVec = gfl2::math::Vector3::GetZero();
    return false;
  }


private:

  GameSys::GameManager* m_pGameManager;
  Field::MoveModel::FieldMoveModelManager*      m_pFieldCharaModelManager;
  Field::MoveModel::FieldMoveModelPlayer*       m_pPlayer;
  b8                                            m_IsAnimationStart;
  u8                                            pad[3];
  const poke_3d::model::DressUpParam * m_cpDressUpParam;
  gfl2::math::Vector3                           m_JumpPosStart;
  gfl2::math::Vector3                           m_JumpPosEnd;
  gfl2::math::Vector3                           m_JumpPosEndAdjustGround;
};



/**
 *  @blass ポケモン乗り降り演出クラス
 */
class PokemonRideOnOffDirection
{
  GFL_FORBID_COPY_AND_ASSIGN(PokemonRideOnOffDirection); //コピーコンストラクタ＋代入禁止

public:

  enum Type
  {
    TYPE_RIDE_ON,       ///< 通常状態から陸のポケモンに乗る
    TYPE_RIDE_OFF,      ///< 陸ライドポケモンから通常状態に降りる
    TYPE_RIDE_TO_RIDE,  ///< 海なら海、陸なら陸の別のポケモンに乗り移る
    TYPE_RIDE_SEA_ON,   ///< 通常状態から海のポケモンに乗る
    TYPE_RIDE_SEA_OFF,  ///< 海のポケモンから陸に降り立つ（通常状態で終わる）

    TYPE_RIDE_SEA_OFF_TO_RIDE_ON, ///< 海から陸に降りてから陸ポケモンに乗る。
    TYPE_RIDE_OFF_TO_RIDE_SEA_ON, ///< 陸ポケモンから降りてから海ポケモンに乗る。

    // 特殊タイプ
    TYPE_RIDE_FORCE_OFF,  ///< 各種チェックをせずに、その場におろします。

    TYPE_MAX,           ///< タイプ最大数
  };


public:

  /**
   *  @brief  コンストラクタ
   */
  PokemonRideOnOffDirection() :
     m_pGameManager(NULL)
    ,m_pDressUpParam(NULL)
    ,m_pFieldCharaModelManager(NULL)
    ,m_pPlayer(NULL)
    ,m_pEffectController(NULL)
    ,m_pRideModelController(NULL)
    ,m_pPlayerModelController(NULL)
    ,m_pCameraController(NULL)
    ,m_IsNaminoriWallHit(false)
    ,m_IsForceFlag(false)
    ,m_MagnetPos(0,0,0)
    ,m_MagnetBeforeRotate(0,0,0,1)
    ,m_IsMagnetMove(false)
    ,m_MagnetWaitCounter(0)
  {
  }


  /**
   *  @brief デストラクタ
   */
  virtual ~PokemonRideOnOffDirection()
  {
  }

  /**
   *  @brief 初期化
   *  @param  p_gman    ゲームマネージャー
   *  @param  p_heap    ヒープ
   */
  void Initialize( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* p_heap , b32 isUseEffect, b32 isForceFlag )
  {
    m_pGameManager    = p_gman;
    m_pDressUpParam   = m_pGameManager->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

    m_pFieldCharaModelManager   = m_pGameManager->GetGameData()->GetFieldCharaModelManager();
    m_pPlayer                   = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( m_pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

    m_IsForceFlag = isForceFlag;

    if( isUseEffect )
    {
      m_pEffectController     = GFL_NEW(p_heap) RideOnOffEffectController();
    }
    m_pRideModelController    = GFL_NEW(p_heap) RideOnOffRideModelController( m_pFieldCharaModelManager, m_pPlayer );
    m_pPlayerModelController  = GFL_NEW(p_heap) RideOnOffPlayerModelController();
    m_pCameraController       = GFL_NEW(p_heap) RideOnOffCameraController();

    // 初期化
    {
      m_pPlayerModelController->Initialize( m_pGameManager, m_pFieldCharaModelManager, m_pPlayer, m_pDressUpParam );
      if( m_pEffectController )
      {
        m_pEffectController->Initialize( m_pGameManager );
      }

      m_pCameraController->Initialize( m_pGameManager, p_heap );
    }
  }

  /**
   *  @brief 破棄
   */
  void Terminate()
  {
    m_pGameManager             = NULL;
    m_pDressUpParam           = NULL;
    m_pFieldCharaModelManager = NULL;
    m_pPlayer                 = NULL;

    GFL_SAFE_DELETE(m_pEffectController);
    GFL_SAFE_DELETE(m_pRideModelController);
    GFL_SAFE_DELETE(m_pPlayerModelController);
    GFL_SAFE_DELETE(m_pCameraController);
  }

  /**
   *  @brief  演出スタート
   *  @param  pokemonID   次のポケモンID
   */
  void StartDirection( RIDE_POKEMON_ID pokemonID )
  {
    m_Type        = getType( pokemonID );
    m_Seq         = 0;
    m_DirectToDirectControl  = 0;
    m_PokemonID   = pokemonID;

    // 波乗り壁を乗り越えるための座標を取得
    // Playerインスタンス削除前のこのタイミングで保持する。
    switch( m_Type )
    {
    case TYPE_RIDE_SEA_ON:   ///< 通常状態から海のポケモンに乗る
    case TYPE_RIDE_SEA_OFF:  ///< 海のポケモンから陸に降り立つ（通常状態で終わる）
    case TYPE_RIDE_SEA_OFF_TO_RIDE_ON: ///< 海から陸に降りてから陸ポケモンに乗る。
    case TYPE_RIDE_OFF_TO_RIDE_SEA_ON: ///< 陸ポケモンから降りてから海ポケモンに乗る。
      if( m_pPlayer->IsHitNaminoriWall() )
      {
        b32 is_on = (m_Type == TYPE_RIDE_SEA_OFF) ? false : true;

        m_pPlayerModelController->SetupNaminoriWall( is_on );
        m_IsNaminoriWallHit = true;
      }
      else
      {
        m_IsNaminoriWallHit = false;
        GFL_ASSERT(0);
      }
      break;
    }

    startDirectionLocalParam();
  }

  /** 
   *  @brief  演出更新
   *  @retval true 終了
   *  @retval false 途中
   */
  b32 UpdateDirection()
  {
    b32 result = true;

    switch( m_Type )
    {
    case TYPE_RIDE_ON:       ///< 通常状態から陸のポケモンに乗る
      result = updateRideOn();
      break;
    case TYPE_RIDE_OFF:      ///< 陸ライドポケモンから通常状態に降りる
      result = updateRideOff( false, true );
      break;
    case TYPE_RIDE_TO_RIDE:  ///< 海なら海、陸なら陸の別のポケモンに乗り移る
      result = updateRideToRide();
      break;
    case TYPE_RIDE_SEA_ON:   ///< 通常状態から海のポケモンに乗る
      result = updateRideSeaOn();
      break;
    case TYPE_RIDE_SEA_OFF:  ///< 海のポケモンから陸に降り立つ（通常状態で終わる）
      result = updateRideSeaOff( false );
      break;
    case TYPE_RIDE_SEA_OFF_TO_RIDE_ON: ///< 海から陸に降りてから陸ポケモンに乗る。
      result = updateRideSeaOffToRideOn();
      break;
    case TYPE_RIDE_OFF_TO_RIDE_SEA_ON: ///< 陸ポケモンから降りてから海ポケモンに乗る。
      result = updateRideOffToRideSeaOn();
      break;


    case TYPE_RIDE_FORCE_OFF:///< 強制その場下ろし
      result = updateRideOff( false, false );
      break;

    default:
      GFL_ASSERT(0);
      break;
    }

    return result;
  }


private:

  /**
   *  @brief  演出スタート　ローカルパラメータ更新
   */
  void startDirectionLocalParam( void )
  {
    // 初期化
    m_pPlayerModelController->Initialize( m_pGameManager, m_pFieldCharaModelManager, m_pPlayer, m_pDressUpParam );
  }

  /**
   *  @brief TYPE_RIDE_ON 通常状態から陸のポケモンに乗る
   */
  b32 updateRideOn()
  {
    enum
    {
      ST_CHECK,
      ST_MAGNET,
      ST_MAGNET_WAIT,
      ST_EFFECT_START_RIDEMODEL_LOAD_START,  ///< エフェクト開始
      ST_WAIT_COUNT,                            ///<  全体が隠れるのをまつ。
      ST_PLAYER_OFF,                            ///< プレイヤー表示OFF
      ST_PLAYER_DELETE_AND_LOAD_START,          ///< プレイヤー削除 AND 読み込み開始
      ST_ALL_LOAD_WAIT_AND_CREATE_RIDEON_PLAYER,///< 全読み込みを待ち、ライドON状態のプレイヤー作成
      ST_EFFECT_WAIT,                           ///< エフェクトの終了待ち
      ST_END,
    };

    switch( m_Seq )
    {
    case ST_CHECK:
      {
        if( !checkRideOn() ) { m_Seq = ST_END; break; } // 状態のチェック

        // ライドに乗る際に吸着が必要ではないかチェック
        m_MagnetPos = m_pPlayer->GetCharaDrawInstance()->GetPosition();
        m_MagnetBeforeRotate = m_pPlayer->GetLocalSRT().rotation;

        // 通常状態から、陸ライドするときのみ、リカバーの可能性の高いチェックを採用する。
        if( !m_pPlayer->IsEnableGroundRideBetween(m_pPlayer->GetCharaDrawInstance()->GetPosition(), &m_MagnetPos,true, m_PokemonID==RIDE_ROBA2?false:true ,true) )
        {
          GFL_ASSERT(0);  // ここにくることはない
          m_Seq = ST_END; break;
        }

        m_Seq = ST_MAGNET;
      }
      // break;

    case ST_MAGNET:
      {
        gfl2::math::Vector3 diff = m_MagnetPos - m_pPlayer->GetCharaDrawInstance()->GetPosition();

        if(diff.Length() > 0.0f)
        {
          m_IsMagnetMove = true;

          const u32 WAIT_NUM = 10;
          m_MagnetWaitCounter = WAIT_NUM;

          // 2Dの移動方向を求める。
          m_pPlayer->SetActionCommandTargetMove( Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE, false, m_MagnetPos, m_pPlayer->GetLocalSRT().rotation, false, false, false, 180.0f );
        }
        else
        {
          m_MagnetWaitCounter = 0;
        }
      }

      m_Seq = ST_MAGNET_WAIT;
      // break;

    case ST_MAGNET_WAIT:
      if( m_MagnetWaitCounter > 0 )
      {
        m_MagnetWaitCounter --;
        break;
      }

      m_Seq = ST_EFFECT_START_RIDEMODEL_LOAD_START;
      // break;

    case ST_EFFECT_START_RIDEMODEL_LOAD_START:  ///< エフェクト開始

      // エフェクト開始
      if( m_pEffectController )
      {
        m_pEffectController->StartEffect( RideOnOffEffectController::TYPE_LAND, m_pPlayer->GetCharaDrawInstance()->GetPosition() );
      }

      // ライドモデルの読み込み開始
      m_pRideModelController->Load( m_PokemonID );

      {
        // 足音の読み込み開始
        PlayerForm form = EventPokemonRideTool::GetRIDE_POKEMON_ID_to_PlayerForm(m_PokemonID);
        m_pGameManager->GetGameData()->GetFieldSound()->LoadFootSoundData( form, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );
      }

      m_Seq = ST_WAIT_COUNT;
      break;

    case ST_WAIT_COUNT:                            ///<  全体が隠れるのをまつ。
      if( (m_pEffectController && !m_pEffectController->WaitHide()) ) // @fix GFNMCat[1121] ライド乗り降りの見た目がきえるまでの間が長い
      {
        break;
      }
      m_Seq = ST_PLAYER_OFF;
      // break;through

    case ST_PLAYER_OFF:                            ///< プレイヤー表示OFF
      m_pPlayerModelController->HideModel();
      m_Seq = ST_PLAYER_DELETE_AND_LOAD_START;
      break;

    case ST_PLAYER_DELETE_AND_LOAD_START:          ///< プレイヤー削除 AND 読み込み開始
      {
        // 吸着動作を中断
        if( m_IsMagnetMove )
        {
          m_pPlayer->ClearActionCommand(false);
        }
        m_pPlayerModelController->DeleteModel();

        // 着せ替え用のドレスアップデータ生成
        u32 sex = m_pPlayer->IsMale() ? PM_MALE : PM_FEMALE;
        GetPokemonRideDressUpParam( sex, !IsNaminoriRidePokemon(m_PokemonID), m_pDressUpParam );  

        m_pPlayerModelController->Load( m_pDressUpParam );
        m_Seq = ST_ALL_LOAD_WAIT_AND_CREATE_RIDEON_PLAYER;
      }
      break;

    case ST_ALL_LOAD_WAIT_AND_CREATE_RIDEON_PLAYER:///< 全読み込みを待ち、ライドON状態のプレイヤー作成
      {
        b32 result_footsound = m_pGameManager->GetGameData()->GetFieldSound()->IsLoadedFootSoundData(); // @fix GFNMCat[1121] ライド乗り降りの見た目がきえるまでの間が長い
        b32 result_ridemodel = m_pRideModelController->WaitLoad(m_PokemonID);
        b32 result_player = m_pPlayerModelController->WaitLoad(); 
        if( result_ridemodel && result_player && result_footsound )
        {
          m_pPlayerModelController->Create();
          m_pRideModelController->CreateAndOn(m_PokemonID);
          // 吸着した場合は、向きを設定
          if( m_IsMagnetMove )
          {
            gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();
            srt.translate = m_MagnetPos;
            srt.rotation  = m_MagnetBeforeRotate;
            m_pPlayer->SetLocalSRT(srt);
          }

          m_Seq = ST_EFFECT_WAIT;
        }
      }
      break;

    case ST_EFFECT_WAIT:                           ///< エフェクトの終了待ち
      if( !m_pEffectController || m_pEffectController->WaitEnd() )
      {
        if( m_pEffectController )
        {
          m_pEffectController->EndEffect();
        }
        
        m_Seq = ST_END;
      }
      break;

    case ST_END:
      return true;
    };

    return false;
  }

  /**
   *  @brief　乗る処理　状態チェック
   */
  b32 checkRideOn() const
  {
    // プレイヤーは通常状態？
    if( m_pPlayer->IsRideMoveModel() )
    {
      return false;
    }

    // 乗るポケモンは陸用？
    if( IsNaminoriRidePokemon( m_PokemonID ) )
    {
      return false;
    }

    return true;
  }

  /**
   *  @brief TYPE_RIDE_OFF 陸ライドポケモンから通常状態に降りる
   *  @param  is_error_check  エラーチェックを行なうか？ falseにすると波乗りポケモンでもその場で降りれる。
   */
  b32 updateRideOff( b32 is_ride2ride, b32 is_error_check = true )
  {
    enum
    {
      ST_CHECK_AND_EFFECT_START,                ///< エフェクト開始
      ST_WAIT_COUNT,                            ///<  全体が隠れるのをまつ。
      ST_PLAYER_OFF,                            ///< プレイヤー表示OFF
      ST_PLAYER_DELETE_AND_LOAD_START,          ///< プレイヤー削除
      ST_CREATE_PLAYER,                         ///< 通常状態のプレイヤー作成
      ST_EFFECT_WAIT,                            ///< エフェクトが終了するのを待つ
      ST_END,
    };

    switch( m_Seq )
    {
    case ST_CHECK_AND_EFFECT_START:                ///< エフェクト開始
      if( is_error_check )
      {
        if( !checkRideOff() ) { m_Seq = ST_END; break; } // 状態のチェック
      }

      if( m_pEffectController )
      {
        m_pEffectController->StartEffect(RideOnOffEffectController::TYPE_LAND, m_pPlayer->GetCharaDrawInstance()->GetPosition()  );
      }
      {
        // 足音の読み込み開始
        m_pGameManager->GetGameData()->GetFieldSound()->LoadFootSoundData( PLAYER_FORM_NORMAL, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );
      }
      m_Seq = ST_WAIT_COUNT;

      break;

    case ST_WAIT_COUNT:                            ///<  全体が隠れるのをまつ。
      if( m_pEffectController && !m_pEffectController->WaitHide() ) // @fix GFNMCat[1121] ライド乗り降りの見た目がきえるまでの間が長い
      {
        break;
      }
      m_Seq = ST_PLAYER_OFF;
      // break;through

    case ST_PLAYER_OFF:                            ///< プレイヤー表示OFF
      m_pRideModelController->HideModel( is_ride2ride );
      m_pPlayerModelController->HideModel();
      m_Seq = ST_PLAYER_DELETE_AND_LOAD_START;
      break;

    case ST_PLAYER_DELETE_AND_LOAD_START:          ///< プレイヤー削除
      m_pRideModelController->DeleteModel();
      m_pPlayerModelController->DeleteModel();

      // プレイヤーのドレスアップを差し替える。
      *m_pDressUpParam = m_pGameManager->GetGameData()->GetPlayerStatusConst()->GetDressup();

      m_pPlayerModelController->Load( m_pDressUpParam );
      m_Seq = ST_CREATE_PLAYER;
      break;

    case ST_CREATE_PLAYER:                         ///< 通常状態のプレイヤー作成
      {
        b32 result_sound = m_pGameManager->GetGameData()->GetFieldSound()->IsLoadedFootSoundData(); // @fix GFNMCat[1121] ライド乗り降りの見た目がきえるまでの間が長い
        b32 result_player = m_pPlayerModelController->WaitLoad(); 
        if( result_player && result_sound )
        {
          m_pPlayerModelController->Create();

          m_Seq = ST_EFFECT_WAIT;
        }
      }
      break;
      
    case ST_EFFECT_WAIT:                            ///< エフェクトが終了するのを待つ
      if( !m_pEffectController || m_pEffectController->WaitEnd() )
      {
        if( m_pEffectController )
        {
          m_pEffectController->EndEffect();
        }

        m_Seq = ST_END;
      }
      break;

    case ST_END:
      return true;

    };

    return false;
  }

  /** 
   *  @brief 降りる処理　状態チェック
   */
  b32 checkRideOff() const
  {
    // プレイヤーライドにのっているか？
    if( !m_pPlayer->IsRideMoveModel() )
    {
      return false;
    }

    // 乗っているポケモンは陸用？
    if( IsNaminoriRidePokemon( EventPokemonRideTool::GetPokemonRideOnID(m_pPlayer) ) )
    {
      return false;
    }

    return true;
  }

  /**
   *  @brief TYPE_RIDE_TO_RIDE 海なら海、陸なら陸の別のポケモンに乗り移る
   */
  b32 updateRideToRide()
  {
    enum
    {
      ST_CHECK_AND_EFFECT_START,                ///< エフェクト開始
      ST_WAIT_COUNT,                            ///<  全体が隠れるのをまつ。
      ST_PLAYER_OFF,                            ///< プレイヤー表示OFF
      ST_PLAYER_DELETE_AND_LOAD_START,          ///< ライドポケモン削除 AND ライドポケモン読み込み開始
      ST_ALL_LOAD_WAIT_AND_CREATE_RIDEON_PLAYER,///< 全読み込みを待ち、ライドポケモンに乗ったのプレイヤー作成
      ST_EFFECT_WAIT,                           ///< エフェクトの完了待ち
      ST_END,
    };


    switch(m_Seq)
    {
    case ST_CHECK_AND_EFFECT_START:                ///< エフェクト開始
      if( !checkRideToRide() ) { m_Seq = ST_END; break; } // 状態のチェック

      if( m_pEffectController )
      {
        if( !IsNaminoriRidePokemon( m_PokemonID ) )
        {
          m_pEffectController->StartEffect(RideOnOffEffectController::TYPE_LAND, m_pPlayer->GetCharaDrawInstance()->GetPosition() );
        }
        else
        {
          m_pEffectController->StartEffect(RideOnOffEffectController::TYPE_SEA, m_pPlayer->GetCharaDrawInstance()->GetPosition());
        }

      }
      {
        // 足音の読み込み開始
        PlayerForm form = EventPokemonRideTool::GetRIDE_POKEMON_ID_to_PlayerForm(m_PokemonID);
        m_pGameManager->GetGameData()->GetFieldSound()->LoadFootSoundData( form, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );
      }
      m_Seq = ST_WAIT_COUNT;
      break;

    case ST_WAIT_COUNT:                            ///<  全体が隠れるのをまつ。
      if( m_pEffectController && !m_pEffectController->WaitHide() ) // @fix GFNMCat[1121] ライド乗り降りの見た目がきえるまでの間が長い
      {
        break;
      }
      m_Seq = ST_PLAYER_OFF;
      // break;through

    case ST_PLAYER_OFF:                            ///< プレイヤー表示OFF
      m_pRideModelController->HideModel( true );
      m_pPlayerModelController->HideModel();
      m_Seq = ST_PLAYER_DELETE_AND_LOAD_START;
      break;

    case ST_PLAYER_DELETE_AND_LOAD_START:          ///< ライドポケモン削除 AND ライドポケモン読み込み開始
      m_pRideModelController->DeleteModel();
      m_pRideModelController->Load( m_PokemonID );
      m_RideModelRemakeCount = 0;
      m_Seq = ST_ALL_LOAD_WAIT_AND_CREATE_RIDEON_PLAYER;
      break;

    case ST_ALL_LOAD_WAIT_AND_CREATE_RIDEON_PLAYER:///< 全読み込みを待ち、ライドポケモンに乗ったのプレイヤー作成
      {
        b32 result_sound = m_pGameManager->GetGameData()->GetFieldSound()->IsLoadedFootSoundData(); // @fix GFNMCat[1121] ライド乗り降りの見た目がきえるまでの間が長い
        b32 result_ridemodel = m_pRideModelController->WaitLoad(m_PokemonID);
        m_RideModelRemakeCount ++;
        if( result_ridemodel && (m_RideModelRemakeCount >= 2) && result_sound )
        {
          m_pPlayerModelController->OnModel();
          m_pRideModelController->CreateAndOn(m_PokemonID);
          m_Seq = ST_EFFECT_WAIT;
        }
      }
      break;

    case ST_EFFECT_WAIT:                           ///< エフェクトの完了待ち
      if( !m_pEffectController || m_pEffectController->WaitEnd() )
      {
        if( m_pEffectController )
        {
          m_pEffectController->EndEffect();
        }
        m_Seq = ST_END;
      }
      break;


    case ST_END:
      return true;

    }

    return false;
  }

  /** 
   *  @brief ライドからライド　状態チェック
   */
  b32 checkRideToRide() const
  {
    // 現状のライドと次のライドは、海なら海、陸なら陸どうしか？

    // プレイヤーライドにのっているか？
    if( !m_pPlayer->IsRideMoveModel() )
    {
      return false;
    }

    // 乗っているポケモンは陸用？
    b32 before_naminori = IsNaminoriRidePokemon( EventPokemonRideTool::GetPokemonRideOnID(m_pPlayer) );
    b32 after_naminori  = IsNaminoriRidePokemon( m_PokemonID );
    if( before_naminori != after_naminori  )
    {
      return false;
    }

    return true;
  }

  /**
   *  @breif TYPE_RIDE_SEA_ON 通常状態から海のポケモンに乗る
   */
  b32 updateRideSeaOn()
  {
    enum
    {
      ST_CHECK_AND_JUMP_START_AND_RIDE_LOAD_START,///< ジャンプ開始 と ライドモデル読み込み開始
      ST_WAIT_COUNT_AND_EFFECT_START,             ///< ちょっと待って、エフェクト開始
      ST_PLAYER_OFF,                              ///< プレイヤー表示OFF
      ST_PLAYER_DELETE_AND_LOAD_START,            ///< ライドポケモン削除 AND 着せ替え読み込み開始
      ST_ALL_LOAD_WAIT_AND_CREATE_RIDEON_PLAYER,  ///< 全読み込みを待ち、ライドポケモンに乗ったのプレイヤー作成
      ST_EFFECT_WAIT,                           ///< エフェクトの完了待ち
      ST_CAMERA_WAIT,                           ///< カメラの完了待ち
      ST_END,
    };

    switch( m_Seq )
    {
    case ST_CHECK_AND_JUMP_START_AND_RIDE_LOAD_START:///< ジャンプ開始 と ライドモデル読み込み開始
      if( !checkRideSeaOn() ) { m_Seq = ST_END; break; } // 状態のチェック

      // @fix GFNMCat[1547]水上ライドポケモンから降りる瞬間カメラがガク付く、対処。フィールドカメラのままだったため、ジャンプ中に自機が削除された際に座標がワープしていた。ジャンプ中のカメラ移動をイベントカメラに変更。
      // @fix GFNMCat[1547]追加対処、JumpStartPosは壁による加工が入っている
      m_pCameraController->Start( m_pPlayer->GetLocalSRT().translate, m_pPlayerModelController->GetJumpEndPosAdjustGround(), RideOnOffCameraController::Type::TYPE_SEA_ON );
      m_pPlayerModelController->PlayJump(true);
      m_pRideModelController->Load( m_PokemonID );

      m_Seq = ST_WAIT_COUNT_AND_EFFECT_START;
      break;

    case ST_WAIT_COUNT_AND_EFFECT_START:             ///< ちょっと待って、エフェクト開始

      m_pCameraController->Update();

      if( m_pPlayerModelController->WaitJumpMidstreamEffect() )
      {
        if( m_pEffectController )
        {
          m_pEffectController->StartEffect(RideOnOffEffectController::TYPE_SEA, m_pPlayerModelController->GetJumpEndPos());
        }
        {
          // 足音の読み込み開始
          PlayerForm form = EventPokemonRideTool::GetRIDE_POKEMON_ID_to_PlayerForm(m_PokemonID);
          m_pGameManager->GetGameData()->GetFieldSound()->LoadFootSoundData( form, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );
        }
        m_Seq  = ST_PLAYER_OFF;
      }
      break;

    case ST_PLAYER_OFF:                              ///< プレイヤー表示OFF
      m_pCameraController->Update();
      if( m_pEffectController && !m_pEffectController->WaitHide() ) // @fix GFNMCat[1121] ライド乗り降りの見た目がきえるまでの間が長い
      {
        break;
      }
      m_pPlayerModelController->HideModel();
      m_Seq = ST_PLAYER_DELETE_AND_LOAD_START;
      break;

    case ST_PLAYER_DELETE_AND_LOAD_START:            ///< ライドポケモン削除 AND 着せ替え読み込み開始
      {
        m_pCameraController->Update();
        m_pPlayerModelController->DeleteModel();

        // 着せ替え用のドレスアップデータ生成
        u32 sex = m_pPlayer->IsMale() ? PM_MALE : PM_FEMALE;
        GetPokemonRideDressUpParam( sex, !IsNaminoriRidePokemon(m_PokemonID), m_pDressUpParam );  

        m_pPlayerModelController->Load( m_pDressUpParam );
        m_Seq = ST_ALL_LOAD_WAIT_AND_CREATE_RIDEON_PLAYER;
      }
      break;

    case ST_ALL_LOAD_WAIT_AND_CREATE_RIDEON_PLAYER:  ///< 全読み込みを待ち、ライドポケモンに乗ったのプレイヤー作成
      {
        m_pCameraController->Update();
        
        b32 result_sound = m_pGameManager->GetGameData()->GetFieldSound()->IsLoadedFootSoundData(); // @fix GFNMCat[1121] ライド乗り降りの見た目がきえるまでの間が長い
        b32 result_ridemodel = m_pRideModelController->WaitLoad(m_PokemonID);
        b32 result_player = m_pPlayerModelController->WaitLoad(); 
        if( result_ridemodel && result_player && result_sound )
        {
          m_pPlayerModelController->Create();
          m_pRideModelController->CreateAndOn(m_PokemonID);

          m_pPlayerModelController->EndJump();

          // 着水音の再生
          Sound::PlaySE( SEQ_SE_FLD_RIDE_COMMON_CHAKUSUI );

          m_Seq = ST_EFFECT_WAIT;
        }
      }
      break;

    case ST_EFFECT_WAIT:                           ///< エフェクトの終了待ち
      
      m_pCameraController->Update();
      
      if( !m_pEffectController || m_pEffectController->WaitEnd() )
      {
        if( m_pEffectController )
        {
          m_pEffectController->EndEffect();
        }

        m_Seq = ST_CAMERA_WAIT;
      }
      break;


    case ST_CAMERA_WAIT:
      {
        m_pCameraController->Update();
        
        if( m_pCameraController->IsEnd() )
        {
          m_pCameraController->PlayerMoveEnd(); // 自機の移動も終わっている
          m_Seq = ST_END;
        }
        else
        {
          break;
        }
      }

    case ST_END:
      return true;

    }

    return false;
  }

  /**
   *  @brief 海ライドに乗る
   */
  b32 checkRideSeaOn() const
  {
    // プレイヤーは通常状態か？
    if( m_pPlayer->IsRideMoveModel() )
    {
      return false;
    }
    
    // 波乗り壁が目の前になるか？
    if( !m_IsNaminoriWallHit )
    {
      return false;
    }

    // 次に乗るポケモンは海か？
    if( !IsNaminoriRidePokemon( m_PokemonID ) )
    {
      return false;
    }

    // ジャンプ先が安全かチェック
    // m_JumpPosEnd;
    
    return true;
  }

  /**
   *  @breif TYPE_RIDE_SEA_OFF 海のポケモンから陸に降り立つ（通常状態で終わる）
   */
  b32 updateRideSeaOff( bool is_ride2ride )
  {
    enum
    {
      ST_CHECK_AND_EFFECT_START,                ///< エフェクト開始
      ST_WAIT_COUNT,                            ///<  全体が隠れるのをまつ。
      ST_PLAYER_OFF,                            ///< プレイヤー表示OFF
      ST_PLAYER_DELETE,                         ///< ライドポケモンも、プレイヤーも破棄
      ST_CREATE_PLAYER_AND_JUMP_START,          ///<プレイヤー作成 と　ジャンプ開始
      ST_EFFECT_WAIT,                           ///< エフェクトの完了待ち
      ST_CAMERA_WAIT,                           ///< カメラの完了待ち
      ST_END,
    };

    switch(m_Seq)
    {
    case ST_CHECK_AND_EFFECT_START:                ///< エフェクト開始
      if( !checkRideSeaOff() ) { m_Seq = ST_END; break; } // 状態のチェック

      if( m_pEffectController )
      {
        m_pEffectController->StartEffect(RideOnOffEffectController::TYPE_SEA, m_pPlayer->GetCharaDrawInstance()->GetPosition());
      }
      {
        // 足音の読み込み開始
        m_pGameManager->GetGameData()->GetFieldSound()->LoadFootSoundData( PLAYER_FORM_NORMAL, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );
      }
      m_Seq = ST_WAIT_COUNT;
      break;

    case ST_WAIT_COUNT:                            ///<  全体が隠れるのをまつ。
      if( m_pEffectController && !m_pEffectController->WaitHide() ) // @fix GFNMCat[1121] ライド乗り降りの見た目がきえるまでの間が長い
      {
        break;
      }
      m_Seq = ST_PLAYER_OFF;
      // break; through

    case ST_PLAYER_OFF:                            ///< プレイヤー表示OFF
      m_pRideModelController->HideModel( is_ride2ride );
      m_pPlayerModelController->HideModel();

      m_Seq = ST_PLAYER_DELETE;
      break;

    case ST_PLAYER_DELETE:                         ///< ライドポケモンも、プレイヤーも破棄
      m_pRideModelController->DeleteModel();
      m_pPlayerModelController->DeleteModel();

      // プレイヤーのドレスアップを差し替える。
      *m_pDressUpParam = m_pGameManager->GetGameData()->GetPlayerStatusConst()->GetDressup();

      m_pPlayerModelController->Load( m_pDressUpParam );
      m_Seq = ST_CREATE_PLAYER_AND_JUMP_START;
      break;

    case ST_CREATE_PLAYER_AND_JUMP_START:                         ///< 通常状態のプレイヤー作成
      {
        b32 result_sound = m_pGameManager->GetGameData()->GetFieldSound()->IsLoadedFootSoundData(); // @fix GFNMCat[1121] ライド乗り降りの見た目がきえるまでの間が長い
        b32 result_player = m_pPlayerModelController->WaitLoad(); 
        if( result_player && result_sound )
        {
          // @fix GFNMCat[1547]水上ライドポケモンから降りる瞬間カメラがガク付く、対処。海→陸ではカク付きは発生しないが、カメラエリアの遷移タイミングを合わせるためにジャンプ中のカメラ移動をイベントカメラに変更。
          // @fix GFNMCat[1547]追加対処、JumpStartPosは壁による加工が入っている
          m_pCameraController->Start( m_pPlayer->GetLocalSRT().translate, m_pPlayerModelController->GetJumpEndPosAdjustGround(), RideOnOffCameraController::Type::TYPE_SEA_OFF );
          
          m_pPlayerModelController->Create();
          m_pPlayerModelController->PlayJump(false);
      
          m_Seq = ST_EFFECT_WAIT;
        }
      }
      break;
      
    case ST_EFFECT_WAIT:                            ///< エフェクトが終了するのを待つ
      {
        m_pCameraController->Update();
        
        b32 result_jump = m_pPlayerModelController->WaitJump();
        b32 result_effect = true;
        if( m_pEffectController )
        {
          result_effect = m_pEffectController->WaitEnd();
        }

        if( result_jump && result_effect )
        {
          if( m_pEffectController )
          {
            m_pEffectController->EndEffect();
          }

          // 座標設定
          m_pPlayerModelController->EndJump();

          m_Seq = ST_CAMERA_WAIT;
        }
      }
      break;


    case ST_CAMERA_WAIT:
      {
        m_pCameraController->Update();

        if( m_pCameraController->IsEnd() )
        {
          m_pCameraController->PlayerMoveEnd(); // 自機の移動も終わっている
          m_Seq = ST_END;
          // break
        }
        else
        {
          break;
        }
      }

    case ST_END:
      return true;

    };

    return false;
  }

  /**
   *  @brief 海ライドから降りる
   */
  b32 checkRideSeaOff() const
  {
    // プレイヤーは通常状態か？
    if( !m_pPlayer->IsRideMoveModel() )
    {
      return false;
    }

    // 今乗っているポケモンは海か？
    if( !IsNaminoriRidePokemon( EventPokemonRideTool::GetPokemonRideOnID(m_pPlayer) ) )
    {
      return false;
    }

    // 目の前に海壁はあるか？
    if( !m_IsNaminoriWallHit )
    {
      return false;
    }
    
    return true;
  }


  /**
   *  @brief TYPE_RIDE_SEA_OFF_TO_RIDE_ON 海から陸に降りてから陸ポケモンに乗る。
   */
  b32 updateRideSeaOffToRideOn()
  {
    enum
    {
      SQ_RIDE_SEA_OFF,
      SQ_RIDE_ON,
      SQ_END,
    };

    switch( m_DirectToDirectControl )
    {
    case SQ_RIDE_SEA_OFF:
      if( updateRideSeaOff( true ) )
      {
        m_Seq = 0;
        m_DirectToDirectControl = SQ_RIDE_ON;
        startDirectionLocalParam();
      }
      break;

    case SQ_RIDE_ON:
      if( updateRideOn() )
      {
        m_DirectToDirectControl = SQ_END;
        m_Seq = 0;
        return true;
      }
      break;
    case SQ_END:
      return true;
    }

    return false;
  }

  /**
   *  @brief TYPE_RIDE_OFF_TO_RIDE_SEA_ON 陸ポケモンから降りてから海ポケモンに乗る。
   */
  b32 updateRideOffToRideSeaOn()
  {
    enum
    {
      SQ_RIDE_OFF,
      SQ_RIDE_SEA_ON,
      SQ_END,
    };

    switch( m_DirectToDirectControl )
    {
    case SQ_RIDE_OFF:
      if( updateRideOff( true, true ) )
      {
        m_Seq = 0;
        m_DirectToDirectControl = SQ_RIDE_SEA_ON;
        startDirectionLocalParam();
      }
      break;

    case SQ_RIDE_SEA_ON:
      if( updateRideSeaOn() )
      {
        m_DirectToDirectControl = SQ_END;
        m_Seq = 0;
        return true;
      }
      break;
    case SQ_END:
      return true;
    }

    return false;

  }

  /**
   *  @brief 演出タイプの取得
   */
  Type getType( RIDE_POKEMON_ID pokemonID )
  {
    RIDE_POKEMON_ID now = EventPokemonRideTool::GetPokemonRideOnID( m_pPlayer );
    //--------------------------------------
    // 降りる場合
    if( pokemonID == RIDE_NO_POKEMON )
    {
      // 強制降り状態
      if( m_IsForceFlag )
      {
        return TYPE_RIDE_FORCE_OFF;
      }

      if( IsNaminoriRidePokemon(now) )
      {
        return TYPE_RIDE_SEA_OFF;
      }
      else
      {
        return TYPE_RIDE_OFF;
      }

    }
    //--------------------------------------
    //--------------------------------------

    //--------------------------------------
    // 乗る場合

    // 今乗っていない。
    if( now == RIDE_NO_POKEMON )
    {
      if( IsNaminoriRidePokemon(pokemonID) )
      {
        return TYPE_RIDE_SEA_ON;
      }
      else
      {
        return TYPE_RIDE_ON;
      }
    }
    else
    {
      b32 now_nami = IsNaminoriRidePokemon(now);
      b32 next_nami = IsNaminoriRidePokemon(pokemonID);

      // 海から海 or 陸から陸
      if( now_nami == next_nami )
      {
        return TYPE_RIDE_TO_RIDE;
      }
      // 海から陸
      else if( (now_nami == true) && (next_nami == false) )
      {
        return TYPE_RIDE_SEA_OFF_TO_RIDE_ON;
      }
      // 陸から海
      else if( (now_nami == false) && (next_nami == true) )
      {
        return TYPE_RIDE_OFF_TO_RIDE_SEA_ON;
      }
      else
      {
        GFL_ASSERT_STOP(0); ///< 存在しない
      }
    }
    //--------------------------------------

    return TYPE_MAX;
  }



public:




  /**
   *  @brief 波乗りで使用するライドポケモンかチェック
   */
  static b32 IsNaminoriRidePokemon( RIDE_POKEMON_ID pokemonID ) 
  {
    switch(pokemonID)
    {
    case RIDE_KENTAROSU:
    case RIDE_MUURANDO:
    case RIDE_KAIRIKY:
    case RIDE_ROBA2:
      return false;

    case RIDE_SAMEHADA:
    case RIDE_LAPLACE:
      return true;
    }
    return false;
  }




  //--------------------------------------------------------------
  /**
    * @brief   ライド用ドレスアップパラメータの取得
    * @param    sex             性別 PM_MALE PM_FEMALE
    * @param    is_land         陸上か、海か
    * @param    p_dressupParam  着せ替えパラメータ
    */
  //--------------------------------------------------------------
  static void GetPokemonRideDressUpParam( u32 sex, bool is_land, poke_3d::model::DressUpParam* p_dressupParam )
  {
    // @fix GFNMCat[1105] Zリング入手後、ライドスーツ着用時Zリングが主人公についていない
    // ライドの着せ替え情報を取得
    System::DressUp::OverwrideDressUpParamRide( sex, is_land, p_dressupParam );
  }

  //--------------------------------------------------------------
  /**
    * @brief   宇宙服用ドレスアップパラメータの取得
    * @param    sex             性別 PM_MALE PM_FEMALE
    * @param    p_dressupParam  着せ替えパラメータ
    */
  //--------------------------------------------------------------
  static void GetSpaceSuitDressUpParam( u32 sex, poke_3d::model::DressUpParam* p_dressupParam )
  {
    // @fix GFNMCat[1105] Zリング入手後、スーツ着用時Zリングが主人公についていない
    // ライドの着せ替え情報を取得
    System::DressUp::OverwrideDressUpParamSpace( sex, p_dressupParam );
  }

private:

  GameSys::GameManager*         m_pGameManager;
  poke_3d::model::DressUpParam* m_pDressUpParam;  ///< プレイヤーのドレスアップパラメータ

  Field::MoveModel::FieldMoveModelManager*      m_pFieldCharaModelManager;
  Field::MoveModel::FieldMoveModelPlayer*       m_pPlayer;
  
  Type m_Type;        ///< 演出タイプ
  u32  m_Seq;         ///< シーケンス
  RIDE_POKEMON_ID m_PokemonID;    ///< 次に乗るポケモン 
  u32  m_DirectToDirectControl;  ///< 演出から演出に遷移する場合のコントロール


  b32 m_IsNaminoriWallHit;  ///< 波乗り壁とのヒット

  s32 m_RideModelRemakeCount; ///< ライドモデルの再生成は2フレーム後じゃないといけない

  b32 m_IsForceFlag; ///< モード

  gfl2::math::Vector3     m_MagnetPos;
  gfl2::math::Quaternion  m_MagnetBeforeRotate;
  b32                     m_IsMagnetMove;
  u32                     m_MagnetWaitCounter;

  //-------------------
  // localParam Start
  //-------------------

  RideOnOffEffectController*        m_pEffectController;
  RideOnOffRideModelController*     m_pRideModelController;
  RideOnOffPlayerModelController*   m_pPlayerModelController;
  RideOnOffCameraController*        m_pCameraController;

  //-------------------
  // localParam End
  //-------------------
};




//--------------------------------------------------------------
// 演出クラス
//--------------------------------------------------------------


//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventPokemonRideOn::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventPokemonRideOn::InitFunc(GameSys::GameManager* gmgr)
{
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  m_pDirection = GFL_NEW( pHeap ) PokemonRideOnOffDirection();
  m_pDirection->Initialize( gmgr, pHeap, m_isUseEffect, false );
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventPokemonRideOn::MainFunc(GameSys::GameManager* gmgr)
{
  switch( this->GetSeqNo() )
  {
  case EVENT_RIDE_START :
    {
      m_pDirection->StartDirection( m_pokemonID );
      this->AddSeqNo();
    }
    break;

  case EVENT_RIDE_WAIT :
    if( m_pDirection->UpdateDirection() )
    {
      this->AddSeqNo();
    }
    break;

  case EVENT_RIDE_END :

    // レコード(ライド回数)
    Savedata::IncRecord( Savedata::Record::RECID_RIDE_POKE );

    // ダイアリー(ライドを選んだとき)
    {
      const ZoneDataLoader* pZoneLoader = gmgr->GetGameData()->GetFieldZoneDataLoader();
      ZoneDataAccessor zoneAccessor( pZoneLoader->GetZoneData( gmgr->GetGameData()->GetNowZoneID() ) );
      PokeDiary::Set( PDID_COUNT_RIDE , m_pokemonID, zoneAccessor.GetPlaceLabelID() );
    }

    // ふれあいリアクション通知
    {
      Field::FieldResident* pFieldResident = gmgr->GetFieldResident();
      Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();
			pPlacementDataManager->RequestEventPlayerActionForFureaiActor( PokemonFureai::FPF_PLAYER_ACITON_RIDE_ON );
    }

    return GameSys::GMEVENT_RES_FINISH;
  default :
    {
      GFL_ASSERT_STOP(0);
      return GameSys::GMEVENT_RES_FINISH;
    }
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventPokemonRideOn::EndFunc(GameSys::GameManager* gmgr)
{
  m_pDirection->Terminate();
  GFL_SAFE_DELETE(m_pDirection);
}



//--------------------------------------------------------------
/**
  * @brief   乗るポケモンを設定
  * @param   pokemonID    乗るポケモン
  */
//--------------------------------------------------------------
void EventPokemonRideOn::SetPokemonRideOnID( GameSys::GameManager* pGameMng, RIDE_POKEMON_ID pokemonID )
{
  GFL_ASSERT( IsPokemonRideOnEnable( pGameMng,  pokemonID, false ) == RCR_OK );

  m_pokemonID = pokemonID;
}


//--------------------------------------------------------------
/**
  * @brief   ポケモンに乗れるかどうかチェック
  * @param   pokeID       ポケモンライドID(RIDE_POKEMON_ID参照)
  */
//--------------------------------------------------------------
RIDE_CHECK_RESULT EventPokemonRideOn::IsPokemonRideOnEnable( GameSys::GameManager* pGameMng, RIDE_POKEMON_ID pokeID, b32 isNaminoriAngleCheck )
{
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameMng->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
  const Field::EventWork* cpEventWork = pGameMng->GetGameData()->GetEventWork();

  // -------------------------
  // チェックパターン
  enum CheckType
  {
    CHECK_TYPE_LAND,
    CHECK_TYPE_SEA,
    CHECK_TYPE_LAND_TO_LAND,
    CHECK_TYPE_SEA_TO_SEA,
    CHECK_TYPE_LAND_TO_SEA,
    CHECK_TYPE_SEA_TO_LAND,
    CHECK_TYPE_MAX,
  };
  /*
    CHECK_TYPE_LAND,              その場で終わる
    CHECK_TYPE_SEA,               ジャンプ先で終わる
    CHECK_TYPE_LAND_TO_LAND,      その場
    CHECK_TYPE_SEA_TO_SEA,        その場
    CHECK_TYPE_LAND_TO_SEA,       ジャンプ先
    CHECK_TYPE_SEA_TO_LAND,       ジャンプ先

Landライドにのるで発生しうるタイプ
    CHECK_TYPE_LAND,              その場
    CHECK_TYPE_LAND_TO_LAND,      その場
    CHECK_TYPE_SEA_TO_LAND,       ジャンプ先

SEAライドにのるで発生しうるタイプ
    CHECK_TYPE_SEA,               ジャンプ先
    CHECK_TYPE_SEA_TO_SEA,        その場
    CHECK_TYPE_LAND_TO_SEA,       ジャンプ先

  */

  const World*  pWorld = pGameMng->GetGameData()->GetFieldWorld();
  if( !pWorld )
  {
    return RCR_ERROR_OTHERS;
  }

  // ライドフラグを確認
  if( !CheckRideFlag( pGameMng, pokeID ) )
  {
    return RCR_ERROR_OTHERS;
  }


  // プレイヤーの描画インスタンスが不在なら乗れない。
  if( pPlayer->GetCharaDrawInstance() == NULL )
  {
    return RCR_ERROR_OTHERS;
  }


  // 今乗っているポケモンを取得
  RIDE_POKEMON_ID now_ride_id = EventPokemonRideTool::GetPokemonRideOnID(pPlayer); 

  // チェックタイプを求める。
  CheckType type = CHECK_TYPE_MAX;
  b32 is_now_naminori   = PokemonRideOnOffDirection::IsNaminoriRidePokemon( now_ride_id );
  b32 is_next_naminori  = PokemonRideOnOffDirection::IsNaminoriRidePokemon( pokeID );
  gfl2::math::Vector3 OnRidePos( pPlayer->GetLocalSRT().translate ); // ライドにのる座標
  b32 is_on_ride_pos_enable = true;
  if( now_ride_id == RIDE_NO_POKEMON )
  {
    if( !is_next_naminori )
    {
      type = CHECK_TYPE_LAND;
      is_on_ride_pos_enable = pPlayer->IsEnableGroundRide( pPlayer->GetLocalSRT().translate, &OnRidePos );
    }
    else
    {
      //ジャンプ後座標の取得 
      is_on_ride_pos_enable = RideOnOffPlayerModelController::GetNaminoriWallJump( pPlayer, &OnRidePos, true ); 
      type = CHECK_TYPE_SEA;
    }
  }
  else if( !is_now_naminori )
  {
    if( !is_next_naminori )
    {
      type = CHECK_TYPE_LAND_TO_LAND;
    }
    else
    {
      //ジャンプ後座標の取得 
      is_on_ride_pos_enable = RideOnOffPlayerModelController::GetNaminoriWallJump( pPlayer, &OnRidePos, true ); 
      type = CHECK_TYPE_LAND_TO_SEA;
    }
  }
  else
  {
    if( !is_next_naminori )
    {
      //ジャンプ後座標の取得 
      is_on_ride_pos_enable = RideOnOffPlayerModelController::GetNaminoriWallJump( pPlayer, &OnRidePos, true ); 
      type = CHECK_TYPE_SEA_TO_LAND;
    }
    else
    {
      type = CHECK_TYPE_SEA_TO_SEA;
    }
  }
  GFL_ASSERT( type != CHECK_TYPE_MAX );


  switch ( pokeID )
  {
  case RIDE_MUURANDO : // ムーランド
  case RIDE_KENTAROSU : // ケンタロス
  case RIDE_KAIRIKY : // カイリキー
  case RIDE_ROBA2: // 地面馬
    {
      // @attention 最終的に条件の確認が必要

      // 走るポケモンに乗れるゾーンかどうか
      Fieldmap* pMap = pGameMng->GetFieldmap();
      if( pMap )
      {
        void* pZoneData = pGameMng->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( pMap->GetZoneID() );
        if( pZoneData )
        {
          ZoneDataAccessor zoneDataAccessor( pZoneData );
          if( !zoneDataAccessor.IsPokeRideRunEnable() )
          {
            return RCR_ERROR_ZONE;
          }
        }
      }


      if(type != CHECK_TYPE_SEA_TO_LAND)
      {
        u32 checkAttr = pPlayer->GetGroundAttribute();
        
        // 陸地でないと陸上ライドには乗れない。
        if( Attribute::IsWater( checkAttr ) )
        {
          return RCR_ERROR_GROUND_ATTRIBUTE;
        }

        // 荒地は地面馬しか降り立てない
        if( pokeID != RIDE_ROBA2 && ( Attribute::IsRoba2Ground( checkAttr ) || pPlayer->IsHitRoba2WallGroundRide() ))
        {
          return RCR_ERROR_GROUND_ATTRIBUTE;
        }
      }
      else
      {
        // ジャンプした先のアトリビュートをチェックする。
        u32 checkAttr = GetPositionAttr( pGameMng, OnRidePos );
        if( Attribute::IsWater( checkAttr ) )
        {
          return RCR_ERROR_NAMINORI_JUMP;
        }

        // @note いきなり荒地に乗り込むことは考慮していません
      }

      // 海から陸に行く場合は、壁があるかチェックを行なう.
      if( type == CHECK_TYPE_SEA_TO_LAND )
      {
        // 波乗り壁に触れているか？
        if( !pPlayer->IsHitNaminoriWall() )
        {
          return RCR_ERROR_NAMINORI_JUMP;
        }

        // @fix 谷くん要望: Yボタンメニュー ショートカットキーからの波乗り開始判定にも角度を入れる。
        // 波乗りフィルターの方を向いているか？
        // note: "自身の座標( GetCharaDrawInstance()->GetPosition() ) == 波乗りイベント用アクターの補正される前の座標"なので↓の処理がなりたつ。
        //       この時点では波乗りイベント用アクターは補正された後の座標です
        {
          // 自身の進行方向を取得
          gfl2::math::Vector3 vecZ(0.0f,0.0f,1.0f);
          vecZ = pPlayer->GetCharaDrawInstance()->GetRotationQuat().Transform( vecZ );

          // 自身に触れているフィルターの法線
          gfl2::math::Vector3 adjustVec       ( pPlayer->GetCharaDrawInstance()->GetPosition() - pPlayer->GetDynamicActor(MoveModel::COLLISION_TYPE_EVENT_NAMINORI)->GetPosition() );
          gfl2::math::Vector3 adjustVecNormal ( adjustVec.Normalize() );

          // 陸地に向かって移動したら下ろす
          const f32 THRESHOLD = 75.0f;
          f32 dot       = gfl2::math::Clamp( adjustVecNormal.Dot(vecZ),-1.0f,1.0f );
          f32 angle     = gfl2::math::ConvRadToDeg( acosf( dot ) );

          if( angle >= THRESHOLD )
          {
            return RCR_ERROR_NAMINORI_JUMP;
          }
        }
      }

      // 陸上ポケモンに乗れる地形かどうか
      if(type != CHECK_TYPE_SEA_TO_LAND)
      {
        if( type == CHECK_TYPE_LAND )
        {
          // 通常状態から、陸ライドするときのみ、リカバーの可能性の高いチェックを採用する。
          gfl2::math::Vector3 recover_point;
          if( !pPlayer->IsEnableGroundRideBetween(pPlayer->GetCharaDrawInstance()->GetPosition(), &recover_point,true, pokeID==RIDE_ROBA2?false:true ,true) )
          {
            return RCR_ERROR_SPACE_SMALE;
          }
        }
        // CHECK_TYPE_SEA_TO_LANDとCHECK_TYPE_LAND以外のケース
        else
        {
          if( !is_on_ride_pos_enable )
          {
            return RCR_ERROR_SPACE_SMALE;
          }
        }
      }
      else
      {
        // ジャンプ先でライドに乗れない状態をチェック
        if( !is_on_ride_pos_enable )
        {
          return RCR_ERROR_SPACE_SMALE;
        }

        // 陸上にジャンプする先で乗れるかどうかをチェックする。
        if( !EventPokemonRideOn::IsEnableNaminoriWallJumpGroundRide( pPlayer, OnRidePos ) )
        {
          return RCR_ERROR_SPACE_SMALE;
        }


        // GFNMCat[5598] サメハダーに乗って陸地に降り立とうとする直前にアサートが出ます
        // ジャンプ先に床があるかチェック
        {
          gfl2::math::Vector3 adjustGround;
          if( !RideOnOffPlayerModelController::GetNaminoriWallJumpAdjustGround( pPlayer, OnRidePos, &adjustGround ) )
          {
            // ここでは降りられないを表示
            return RCR_ERROR_NAMINORI_JUMP;
          }
        }

        TOMOYA_PRINT( "Jump Ground Pos[%f,%f,%f]\n", OnRidePos.GetX(), OnRidePos.GetY(), OnRidePos.GetZ() );
      }

    }
    return ( RCR_OK );

  case RIDE_SAMEHADA : // サメハダー
  case RIDE_LAPLACE  : // ラプラス
    {
      // @attention 最終的に条件の確認が必要
      // 走るポケモンに乗れるゾーンかどうか
      Fieldmap* pMap = pGameMng->GetFieldmap();
      if( pMap )
      {
        void* pZoneData = pGameMng->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( pMap->GetZoneID() );
        if( pZoneData )
        {
          ZoneDataAccessor zoneDataAccessor( pZoneData );
          if( !zoneDataAccessor.IsPokeRideRunEnable() )
          {
            return RCR_ERROR_ZONE;
          }
        }
      }

      if( type != CHECK_TYPE_SEA_TO_SEA )
      {
        // 波乗りポケモンに乗れる地形かどうか
        if( !pPlayer->IsHitNaminoriWall() )
        {
          return RCR_ERROR_NAMINORI_JUMP;
        }

        // ジャンプ先でライドに乗れるか？
        if( !is_on_ride_pos_enable )
        {
          return RCR_ERROR_SPACE_SMALE;
        }
        if( !EventPokemonRideOn::IsEnableNaminoriWallJumpGroundRide( pPlayer, OnRidePos ) )
        {
          return RCR_ERROR_SPACE_SMALE;
        }
        TOMOYA_PRINT( "Jump Ground Pos[%f,%f,%f]\n", OnRidePos.GetX(), OnRidePos.GetY(), OnRidePos.GetZ() );

        // ジャンプ先が水アトリビュートでないかチェックする。
        if( !Attribute::IsWater( GetPositionAttr( pGameMng, OnRidePos ) ) )
        {
          return RCR_ERROR_NAMINORI_JUMP;
        }

        // @fix 谷くん要望: Yボタンメニュー ショートカットキーからの波乗り開始判定にも角度を入れる。
        // 波乗りフィルターの方を向いているか？
        // note: "自身の座標( GetCharaDrawInstance()->GetPosition() ) == 波乗りイベント用アクターの補正される前の座標"なので↓の処理がなりたつ。
        //       この時点では波乗りイベント用アクターは補正された後の座標です
        {
          // 自身の進行方向を取得
          gfl2::math::Vector3 vecZ(0.0f,0.0f,1.0f);
          vecZ = pPlayer->GetCharaDrawInstance()->GetRotationQuat().Transform( vecZ );

          // 自身に触れているフィルターの法線
          gfl2::math::Vector3 adjustVec       ( pPlayer->GetCharaDrawInstance()->GetPosition() - pPlayer->GetDynamicActor(MoveModel::COLLISION_TYPE_EVENT_NAMINORI)->GetPosition() );
          gfl2::math::Vector3 adjustVecNormal ( adjustVec.Normalize() );

          // 陸地に向かって移動したら下ろす
          const f32 THRESHOLD = 75.0f;
          f32 dot       = gfl2::math::Clamp( adjustVecNormal.Dot(vecZ),-1.0f,1.0f );
          f32 angle     = gfl2::math::ConvRadToDeg( acosf( dot ) );

          if( angle >= THRESHOLD )
          {
            return RCR_ERROR_NAMINORI_JUMP;
          }
        }
      }

    }
    return RCR_OK;

  case RIDE_RIZAADON : // リザードン
    {
      // @attention 最終的に条件の確認が必要
      // 空を飛ぶポケモンに乗れるゾーンかどうか
      Fieldmap* pMap = pGameMng->GetFieldmap();
      if( pMap )
      {
        void* pZoneData = pGameMng->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( pMap->GetZoneID() );
        if( pZoneData )
        {
          ZoneDataAccessor zoneDataAccessor( pZoneData );
          if( !zoneDataAccessor.IsPokeRideFlyEnable() )
          {
            return RCR_ERROR_ZONE;
          }
        }
      }

      // システムフラグで空を飛ぶを禁止している場合。
      if( cpEventWork->CheckEventFlag( SYS_FLAG_CAN_NOT_FLY ) )
      {
        return RCR_ERROR_CAN_NOT_NOW;
      }

    }
    return RCR_OK;

  case RIDE_NO_POKEMON :
    return RCR_ERROR_OTHERS;

  default:  // 入ってないポケモン？
    {
      GFL_ASSERT( 0 );
    }
    break;
  }
  return RCR_ERROR_OTHERS;
}


//--------------------------------------------------------------
/**
  * @brief   ポケモンに乗れるかチェックの結果からNGメッセージを取得する。
  */
//--------------------------------------------------------------
u32 EventPokemonRideOn::GetRcrMessageId( RIDE_CHECK_RESULT result )
{
  static const u32 sc_MessageId[] = 
  {
    0,                    // RCR_OK,                       ///< ライドに乗れる/降りれる
    ride_pokelist_01_04,  //RCR_ERROR_ZONE,               ///< ゾーンの設定がライド無効
    ride_pokelist_01_05,  // RCR_ERROR_SPACE_SMALE,        ///< 障害物があり、乗れない/降りれない
    ride_pokelist_01_04,  // RCR_ERROR_GROUND_ATTRIBUTE,   ///< 地形の状況により乗れない/降りれない
    ride_pokelist_01_04,  // RCR_ERROR_NAMINORI_JUMP,      ///< 波乗り/降りジャンプの不正
    ride_pokelist_01_09,  // RCR_ERROR_CAN_NOT_NOW,        ///< 今はつかえない
    ride_pokelist_01_04,  // RCR_ERROR_OTHERS,             ///< その他の理由で乗れない/降りれない
  };

  GFL_ASSERT_STOP( RCR_MAX == GFL_NELEMS(sc_MessageId) );

  if( result == RCR_OK )
  {
    GFL_ASSERT_STOP( 0 );
    return sc_MessageId[result];
  }

  if( result < RCR_MAX )
  {
    return sc_MessageId[result];
  }

  GFL_ASSERT_STOP( result < RCR_MAX );
  return 0;
}



//--------------------------------------------------------------
/**
  * @brief   ポケモンに乗れるかチェックの結果からスクリプトIdを取得する。
  */
//--------------------------------------------------------------
u32 EventPokemonRideOn::GetRcrScriptId( RIDE_CHECK_RESULT result )
{
  static const u32 sc_ScriptId[] = 
  {
    0,                    // RCR_OK,                       ///< ライドに乗れる/降りれる
    SCRID_FLD_EV_RIDE_ERROR_ON_01,  //RCR_ERROR_ZONE,               ///< ゾーンの設定がライド無効
    SCRID_FLD_EV_RIDE_ERROR_ON_02,  // RCR_ERROR_SPACE_SMALE,        ///< 障害物があり、乗れない/降りれない
    SCRID_FLD_EV_RIDE_ERROR_ON_01,  // RCR_ERROR_GROUND_ATTRIBUTE,   ///< 地形の状況により乗れない/降りれない
    SCRID_FLD_EV_RIDE_ERROR_ON_01,  // RCR_ERROR_NAMINORI_JUMP,      ///< 波乗り/降りジャンプの不正
    SCRID_FLD_EV_RIDE_ERROR_ON_03,  // RCR_ERROR_CAN_NOT_NOW,        ///< 今はつかえない
    SCRID_FLD_EV_RIDE_ERROR_ON_01,  // RCR_ERROR_OTHERS,             ///< その他の理由で乗れない/降りれない
  };

  GFL_ASSERT_STOP( RCR_MAX == GFL_NELEMS(sc_ScriptId) );

  if( result == RCR_OK )
  {
    GFL_ASSERT_STOP( 0 );
    return sc_ScriptId[result];
  }

  if( result < RCR_MAX )
  {
    return sc_ScriptId[result];
  }

  GFL_ASSERT_STOP( result < RCR_MAX );
  return 0;
}


/**
 *  @brief ライド用のドレスアップパラメータを設定
 */
u32 EventPokemonRideOn::GetPositionAttr( GameSys::GameManager* pGameMng, const gfl2::math::Vector3& pos )
{
  BaseCollisionScene* pScene = pGameMng->GetFieldmap()->GetTerrainManager()->GetCollsionScene();
  if( pScene )
  {
    f32 startEndOffset = 200.0f;

    // 地面の高さを取得
    RaycastCustomCallback::HIT_DATA hitData;
    gfl2::math::Vector3             startVec3(pos + gfl2::math::Vector3(0.0f,startEndOffset,0.0f));
    gfl2::math::Vector3             endVec3  (pos - gfl2::math::Vector3(0.0f,startEndOffset,0.0f));
    gfl2::math::Vector4              startVec (startVec3.x,startVec3.y,startVec3.z,0.0f);
    gfl2::math::Vector4              endVec   (  endVec3.x,  endVec3.y,  endVec3.z,0.0f);

    // レイが地面に当たったかどうか
    if( pScene->RaycastFromStaticActorsMesh( startVec,endVec,&hitData ) )
    {
      // 地面情報の確保
      s32 hitAttribute = hitData.pTriangle->m_GroundAttribute;

      // リソース側のエラー値は-1なのでフィールド側のエラー値に合わせる
      if( hitAttribute == -1 )
      {
        hitAttribute = ATTRIBUTE_ERROR;
      }
      return hitAttribute;
    }
  }
  
  return ATTR_ERROR;
}

#if PM_DEBUG
//--------------------------------------------------------------
/**
  * @brief   デバッグ用 乗るポケモンを自動で設定
  * @return   乗るポケモンのID
  */
//--------------------------------------------------------------
Field::RIDE_POKEMON_ID EventPokemonRideOn::DebugAutoSelectRidePokemon( GameSys::GameManager* pGameMng )
{
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameMng->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  // 動作モデルマネージャーを取得
  if( pPlayer->GetOnMoveModel() )
  {
    return RIDE_NO_POKEMON;
  }

  if     ( pPlayer->IsHitNaminoriWall() )
  {
    return RIDE_SAMEHADA;
  }
  else if( pPlayer->IsEnableGroundRide(pPlayer->GetCharaDrawInstance()->GetPosition(),NULL) )
  {
    return RIDE_MUURANDO;
  }
  else
  {
    return RIDE_NO_POKEMON;
  }
}

#endif



/**
 *  @brief ライド機能が解放されているかチェック
 */
bool EventPokemonRideOn::CheckRideFlag( const GameSys::GameManager* cpGameMng, RIDE_POKEMON_ID pokeID )
{
  static const u32 sc_Flag[RIDE_POKEMON_NUM] = 
  {
    SYS_FLAG_KENTAROSU_RIDE_OPEN,
    SYS_FLAG_SAMEHADAA_RIDE_OPEN,
    SYS_FLAG_MUURANDO_RIDE_OPEN,
    SYS_FLAG_RAPURASU_RIDE_OPEN,
    SYS_FLAG_KAIRIKII_RIDE_OPEN,
    SYS_FLAG_RIZAADON_RIDE_OPEN,
    SYS_FLAG_ROBA2_RIDE_OPEN,
  };

  // そもそもライドがオープンしているか？
  if( !cpGameMng->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_POKE_RIDE_OPEN ) )
  {
    return false;
  }

  if( pokeID < RIDE_POKEMON_NUM )
  {
    return cpGameMng->GetGameData()->GetEventWork()->CheckEventFlag( sc_Flag[pokeID] );
  }

  return false;
}


/**
 *  @brief ライド用のドレスアップパラメータを設定
 // @fix NMCat[181] 水上ライド中にバッグ画面等を開閉すると、フィールド上のライド衣装が陸上用のものになる
 */
void EventPokemonRideOn::GetPokemonRideDressUpParam( GameSys::GameManager* pGameManager, poke_3d::model::DressUpParam* p_dressupParam, b32 is_fieldmap_none )
{
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  RIDE_POKEMON_ID now_ride_id = EventPokemonRideTool::GetPokemonRideOnID( pPlayer, is_fieldmap_none ); 
  b32 is_naminori  = PokemonRideOnOffDirection::IsNaminoriRidePokemon( now_ride_id );

  PokemonRideOnOffDirection::GetPokemonRideDressUpParam( pGameManager->GetGameData()->GetPlayerStatusConst()->GetSex(), !is_naminori, p_dressupParam );
}

/**
 *  @brief 宇宙服用のドレスアップパラメータを設定
 */
void EventPokemonRideOn::GetSpaceSuitDressUpParam( GameSys::GameManager* pGameManager, poke_3d::model::DressUpParam* p_dressupParam )
{
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  PokemonRideOnOffDirection::GetSpaceSuitDressUpParam( pGameManager->GetGameData()->GetPlayerStatusConst()->GetSex(), p_dressupParam );
}



/**
 *  @brief 波乗りジャンプ先に壁にぶつからずに進むことが出来るかチェックする。
 */
b32 EventPokemonRideOn::IsEnableNaminoriWallJumpGroundRide( Field::MoveModel::FieldMoveModelPlayer* pPlayer, const gfl2::math::Vector3& jumpEndPos )
{
#if 0
  // @fix NMCat[865] 波乗りジャンプ先を求める際に、EnableGroundRideはチェックするように変更が入ったためこちらは無効化
  // 降り立つ場所でライドに乗れるか？
  if( !pPlayer->IsEnableGroundRide(jumpEndPos) )
  {
    return false;
  }
#endif

  // @fix GFNMCat[887] 配置モデルアクターとの押し返しにも対応
  gfl2::math::Vector3 nowPos( pPlayer->GetLocalSRT().translate );
  return pPlayer->GetHitCntFromOshikaeshiScene( nowPos, jumpEndPos , 0, false,true,true );  
}


//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventPokemonRideOff::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventPokemonRideOff::InitFunc(GameSys::GameManager* gmgr)
{
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  m_pDirection = GFL_NEW( pHeap ) PokemonRideOnOffDirection();
  m_pDirection->Initialize( gmgr, pHeap, m_isUseEffect, m_isForce );
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventPokemonRideOff::MainFunc(GameSys::GameManager* gmgr)
{
  switch( this->GetSeqNo() )
  {
  case EVENT_RIDE_START :
    {
      m_pDirection->StartDirection( RIDE_NO_POKEMON );
      this->AddSeqNo();
    }
    break;

  case EVENT_RIDE_WAIT:
    if(m_pDirection->UpdateDirection())
    {
      this->AddSeqNo();
    }
    break;

  case EVENT_RIDE_END:

    // ふれあいリアクション通知
    {
      Field::FieldResident* pFieldResident = gmgr->GetFieldResident();
      Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();
			pPlacementDataManager->RequestEventPlayerActionForFureaiActor( PokemonFureai::FPF_PLAYER_ACITON_RIDE_OFF);
    }

    return GameSys::GMEVENT_RES_FINISH;

  default :
    GFL_ASSERT(0);
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventPokemonRideOff::EndFunc(GameSys::GameManager* gmgr)
{
  m_pDirection->Terminate();
  GFL_SAFE_DELETE(m_pDirection);
}

//--------------------------------------------------------------
/**
  * @brief   ポケモンから降りられるかどうかチェック
  */
//--------------------------------------------------------------
RIDE_CHECK_RESULT EventPokemonRideOff::IsPokemonRideOffEnable( GameSys::GameManager* pGameMng )
{
  // 動作モデルマネージャーを取得
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameMng->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer                 = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  // 一応乗っているかチェック
  Field::MoveModel::FieldMoveModel* pMoveModel = pPlayer->GetOnMoveModel();
  if( !pMoveModel )
  {
    return RCR_ERROR_OTHERS;
  }

  // プレイヤーの描画インスタンスが不在なら乗れない。
  if( pPlayer->GetCharaDrawInstance() == NULL )
  {
    return RCR_ERROR_OTHERS;
  }

  // 各ライドポケモンごとに条件チェック
  u32 modelID = pMoveModel->GetCharacterId();
  switch ( modelID )
  {
  case RIDE_MODEL_MUURANDO : 
  case RIDE_MODEL_KENTAROSU : 
  case RIDE_MODEL_KAIRIKY :
  case RIDE_MODEL_ROBA2:
    {
      // @attention 最終的に条件の確認が必要
      // 陸地でないとケンタロスからは降りれない。
      if( Attribute::IsWater( pPlayer->GetGroundAttribute() ) )
      {
        return RCR_ERROR_GROUND_ATTRIBUTE;
      }

      // 荒地は地面馬のみ走行可能なので、降りられない
      if( Attribute::IsRoba2Ground( pPlayer->GetGroundAttribute() ) || pPlayer->IsHitRoba2WallRideOff() )
      {
        return RCR_ERROR_GROUND_ATTRIBUTE;
      }
    }
    return RCR_OK;

  case RIDE_MODEL_SAMEHADA : 
  case RIDE_MODEL_LAPLACE : 
    {
      // @attention 最終的に条件の確認が必要
      // 水でさらに壁に当たっているときにサメハダーから降りれる。
      if( !Attribute::IsWater( pPlayer->GetGroundAttribute() ) )
      {
        return RCR_ERROR_GROUND_ATTRIBUTE;
      }

      if( !pPlayer->IsHitNaminoriWall() )  // 波際でのみ降りられる
      {
        return RCR_ERROR_NAMINORI_JUMP;
      }

      // 降りる先の座標が安全かチェック
      gfl2::math::Vector3 jumpEndPos;
      if( !RideOnOffPlayerModelController::GetNaminoriWallJump( pPlayer, &jumpEndPos, false ) )
      {
        return RCR_ERROR_SPACE_SMALE;
      }

      TOMOYA_PRINT( "Jump Ground Pos[%f,%f,%f]\n", jumpEndPos.GetX(), jumpEndPos.GetY(), jumpEndPos.GetZ() );
      if( !EventPokemonRideOn::IsEnableNaminoriWallJumpGroundRide(pPlayer, jumpEndPos) )
      {
        return RCR_ERROR_SPACE_SMALE;
      }

      // GFNMCat[5598] サメハダーに乗って陸地に降り立とうとする直前にアサートが出ます
      // 降りる先に床があるかチェック
      {
        gfl2::math::Vector3 adjustGround;
        if( !RideOnOffPlayerModelController::GetNaminoriWallJumpAdjustGround( pPlayer, jumpEndPos, &adjustGround ) )
        {
          // ここでは降りられないを表示
          return RCR_ERROR_NAMINORI_JUMP;
        }
      }

      // @fix 谷くん要望: Yボタンメニュー ショートカットキーからの波乗り開始判定にも角度を入れる。
      // 波乗りフィルターの方を向いているか？
      // note: "自身の座標( GetCharaDrawInstance()->GetPosition() ) == 波乗りイベント用アクターの補正される前の座標"なので↓の処理がなりたつ。
      //       この時点では波乗りイベント用アクターは補正された後の座標です
      {
        // 自身の進行方向を取得
        gfl2::math::Vector3 vecZ(0.0f,0.0f,1.0f);
        vecZ = pPlayer->GetCharaDrawInstance()->GetRotationQuat().Transform( vecZ );

        // 自身に触れているフィルターの法線
        gfl2::math::Vector3 adjustVec       ( pPlayer->GetCharaDrawInstance()->GetPosition() - pPlayer->GetDynamicActor(MoveModel::COLLISION_TYPE_EVENT_NAMINORI)->GetPosition() );
        gfl2::math::Vector3 adjustVecNormal ( adjustVec.Normalize() );

        // 陸地に向かって移動したら下ろす
        const f32 THRESHOLD = 75.0f;
        f32 dot       = gfl2::math::Clamp( adjustVecNormal.Dot(vecZ),-1.0f,1.0f );
        f32 angle     = gfl2::math::ConvRadToDeg( acosf( dot ) );

        if( angle >= THRESHOLD )
        {
          return RCR_ERROR_NAMINORI_JUMP;
        }
      }

    }
    return RCR_OK;

  default:
    {
      GFL_ASSERT(0);
      return RCR_ERROR_OTHERS;
    }
  }
}

//--------------------------------------------------------------
/**
  * @brief   ポケモンに降りれるかチェックの結果からNGメッセージを取得する。
  */
//--------------------------------------------------------------
u32 EventPokemonRideOff::GetRcrMessageId( RIDE_CHECK_RESULT result )
{
  static const u32 sc_MessageId[] = 
  {
    0,                    // RCR_OK,                       ///< ライドに乗れる/降りれる
    ride_pokelist_01_06,  //RCR_ERROR_ZONE,               ///< ゾーンの設定がライド無効
    ride_pokelist_01_07,  // RCR_ERROR_SPACE_SMALE,        ///< 障害物があり、乗れない/降りれない
    ride_pokelist_01_06,  // RCR_ERROR_GROUND_ATTRIBUTE,   ///< 地形の状況により乗れない/降りれない
    ride_pokelist_01_06,  // RCR_ERROR_NAMINORI_JUMP,      ///< 波乗り/降りジャンプの不正
    0,                    // RCR_ERROR_CAN_NOT_NOW,        ///< 今はつかえない
    ride_pokelist_01_06,  // RCR_ERROR_OTHERS,             ///< その他の理由で乗れない/降りれない
  };

  GFL_ASSERT_STOP( RCR_MAX == GFL_NELEMS(sc_MessageId) );

  if( result == RCR_OK )
  {
    GFL_ASSERT_STOP( 0 );
    return sc_MessageId[result];
  }

  if( result < RCR_MAX )
  {
    return sc_MessageId[result];
  }

  GFL_ASSERT_STOP( result < RCR_MAX );
  return 0;
}


//--------------------------------------------------------------
/**
  * @brief   ポケモンに降りれるかチェックの結果からスクリプトIdを取得する。
  */
//--------------------------------------------------------------
u32 EventPokemonRideOff::GetRcrScriptId( RIDE_CHECK_RESULT result )
{
  static const u32 sc_ScriptId[] = 
  {
    0,                    // RCR_OK,                       ///< ライドに乗れる/降りれる
    SCRID_FLD_EV_RIDE_ERROR_OFF_01,  //RCR_ERROR_ZONE,               ///< ゾーンの設定がライド無効
    SCRID_FLD_EV_RIDE_ERROR_OFF_02,  // RCR_ERROR_SPACE_SMALE,        ///< 障害物があり、乗れない/降りれない
    SCRID_FLD_EV_RIDE_ERROR_OFF_01,  // RCR_ERROR_GROUND_ATTRIBUTE,   ///< 地形の状況により乗れない/降りれない
    SCRID_FLD_EV_RIDE_ERROR_OFF_01,  // RCR_ERROR_NAMINORI_JUMP,      ///< 波乗り/降りジャンプの不正
    0,                                // RCR_ERROR_CAN_NOT_NOW,        ///< 今はつかえない @fix GFNMCat[843] 降りられない場所でライドポケモンから降りようとするとアサート
    SCRID_FLD_EV_RIDE_ERROR_OFF_01,  // RCR_ERROR_OTHERS,             ///< その他の理由で乗れない/降りれない
  };

  GFL_ASSERT_STOP( RCR_MAX == GFL_NELEMS(sc_ScriptId) );

  if( result == RCR_OK )
  {
    GFL_ASSERT_STOP( 0 );
    return sc_ScriptId[result];
  }

  if( result < RCR_MAX )
  {
    return sc_ScriptId[result];
  }

  GFL_ASSERT_STOP( result < RCR_MAX );
  return 0;
}


#if 0
//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventPokemonRideOnBeforeMagnetPos::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventPokemonRideOnBeforeMagnetPos::InitFunc(GameSys::GameManager* gmgr)
{
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventPokemonRideOnBeforeMagnetPos::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_MAGNET_POS,
    SEQ_WAIT,
    SEQ_CHANGE_EVENT,
    SEQ_END,
  };


  MoveModel::FieldMoveModelPlayer* pPlayer = gmgr->GetFieldmap()->GetPlayerCharacter();

  switch( this->GetSeqNo() )
  {
  case SEQ_MAGNET_POS:
    {
      // 色々な状態での歩きモーションを取得する。 各ライドの歩きに切り替える処理は、field_data/ride_motion_convert_dataに任せる。
      u32 motionID = P2_BASE_FI010_WALK01;

      pPlayer->SetActionCommandTargetMoveWithOptionalMotion(Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE_WITH_OPTIONAL_MOTION, motionID, m_magnetPos, pPlayer->GetLocalSRT().rotation );
    }
    this->AddSeqNo();
    break;

  case SEQ_WAIT:

    // 吸着中
    if( pPlayer->IsExecuteActionCommand() )
    {
      break;
    }
    this->AddSeqNo();
    //break; through

  case SEQ_CHANGE_EVENT:
    {
      Field::EventPokemonRideOn* pEvent = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, Field::EventPokemonRideOn>( gmgr->GetGameEventManager() );
      pEvent->SetPokemonRideOnID( gmgr, m_pokemonID );
      this->AddSeqNo();
    }
    return GameSys::GMEVENT_RES_FINISH;

  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventPokemonRideOnBeforeMagnetPos::EndFunc(GameSys::GameManager* gmgr)
{
}


//--------------------------------------------------------------
/**
  * @brief   乗るポケモンを設定
  * @param   pokemonID    乗るポケモン
  */
//--------------------------------------------------------------
void EventPokemonRideOnBeforeMagnetPos::SetPokemonRideOnID( GameSys::GameManager* /*pGameMng*/, RIDE_POKEMON_ID pokemonID, const gfl2::math::Vector3& magnet_pos )
{
  m_magnetPos = magnet_pos;
  m_pokemonID = pokemonID;
}

#endif

GFL_NAMESPACE_END(Field);
