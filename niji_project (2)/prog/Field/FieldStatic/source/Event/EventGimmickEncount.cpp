//======================================================================
/**
 * @file EventGimmickEncount.cpp
 * @date 2015/09/02 16:07:23
 * @author saita_kazuki
 * @brief ギミックエンカウントイベント
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldStatic/include/Event/EventGimmickEncount.h"

// gflib
#include <util/include/gfl2_std_string.h>
#include <gfx/include/gfl2_GFGL.h>
#include <Fade/include/gfl2_FadeTypes.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <math/include/gfl2_MathCommon.h>

// poketool
#include "PokeTool/include/PokeModelSystem.h"
#include "PokeTool\source\PokeModelLocal.h"

// gamesys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// conv_header
#include <niji_conv_header/poke_lib/item/itemsym.h>
#include <niji_conv_header/field/flagwork/FlagDefine.h>
#include <niji_conv_header/field/static_model/static_model_id.h>

// reference_files
#include <niji_reference_files/script/FieldPawnTypes.h>

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/FieldRenderingPipeLineManager.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"
#include "Field/FieldRo/include/TrialModel/FieldTrialModelFactory.h"
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDummyPolygon.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectGimmickEncount.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
#include "Field/FieldRo/include/FieldEnvManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// system
#include "System/include/HeapDefine.h"
#include "System/include/nijiAllocator.h"

// gimmick_encount
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDataAccessor.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountLocalWork.h"

// placement
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldGimmickEncountAccessor.h"

// encount
#include "Field/FieldRo/include/Encount/FieldEncount.h"
#include "Field/FieldStatic/include/Encount/FieldEncountUtil.h"
#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"
#include "Field/FieldStatic/include/Encount/FieldEncountPokeSet.h"

// event
#include "Field/FieldStatic/include/Event/EventBattleCall.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"

// drop_item
#include "Field/FieldRo/include/DropItem/FieldDropItemCreator.h"
#include "Field/FieldRo/include/DropItem/FieldDropItemDataAccessor.h"

// savedata
#include "Savedata/include/EventWork.h"

// script
#include "FieldScript/include/EventScriptCall.h"

#ifdef DEBUG_ONLY_FOR_saita_kazuki
#if defined(GF_PLATFORM_WIN32)
#include  <gfx/include/gfl2_PCDebugUtil.h>
#endif // GF_PLATFORM_WIN32
#endif // DEBUG_ONLY_FOR_saita_kazuki

#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugGimmickEncount.h"
#endif

GFL_NAMESPACE_BEGIN( Field )

//======================================================================
// ギミックエンカウントシーケンス更新インターファース
//======================================================================

/**
* @brief コンストラクタ
*/
EventGimmickEncount::IEventGimmickEncountUpdate::IEventGimmickEncountUpdate()
  : m_desc()
  , m_seq( 0)
{
}

/**
* @brief デストラクタ
*/
EventGimmickEncount::IEventGimmickEncountUpdate::~IEventGimmickEncountUpdate()
{
}

//======================================================================
// ギミックエンカウント通常シーケンス更新
//======================================================================

/**
* @brief コンストラクタ
*/
EventGimmickEncount::EventGimmickEncountNormalUpdate::EventGimmickEncountNormalUpdate()
  : EventGimmickEncount::IEventGimmickEncountUpdate()
{
}

/**
* @brief デストラクタ
*/
EventGimmickEncount::EventGimmickEncountNormalUpdate::~EventGimmickEncountNormalUpdate()
{
}

/**
* @brief セットアップ
* @param desc セットアップ情報
*/
void EventGimmickEncount::EventGimmickEncountNormalUpdate::Setup( const Description& desc)
{
  m_desc = desc;
}

/**
* @brief 更新
* @retval true 完了
* @retval false 実行中
*/
bool EventGimmickEncount::EventGimmickEncountNormalUpdate::Update()
{
  enum Seq
  {
    SEQ_ICON_INITIALIZE,
    SEQ_ICON_WAIT,
    SEQ_END,
  };

  switch( m_seq)
  {
  case SEQ_ICON_INITIALIZE:
    {
      Effect::EffectManager* pEffectManager = m_desc.p_gman->GetFieldmap()->GetEffectManager();
      MoveModel::FieldMoveModel*  pMoveModel = m_desc.pEvent->m_pMoveModelForIcon;
      // [！]生成
      m_desc.pEvent->initializeIcon( pEffectManager, pMoveModel);
      m_seq++;
    }
    break;

  case SEQ_ICON_WAIT:
    {
      if( m_desc.pEvent->updateIcon())
      {
        Effect::EffectManager* pEffectManager = m_desc.p_gman->GetFieldmap()->GetEffectManager();
        // [！]削除
        m_desc.pEvent->terminateIcon( pEffectManager);
        m_seq++;
      }
    }
    break;

  case SEQ_END:
    m_seq = 0;
    return true;
  }

  return false;
}

/**
* @brief バトル呼び出しまでの更新
* @retval true 完了
* @retval false 実行中
*/
bool EventGimmickEncount::EventGimmickEncountNormalUpdate::UpdateBattleCall()
{
  enum Seq
  {
    SEQ_PLAY_ENCOUNT_EFFECT,
    SEQ_WAIT_ENCOUNT_EFFECT,
    SEQ_SETUP_ENCOUNT_EFFECT,
    SEQ_COMMON_TERMINATE_PRE,
    SEQ_COMMON_TERMINATE,
    END,
  };

  switch( m_seq)
  {
  case SEQ_PLAY_ENCOUNT_EFFECT:
    {
      Encount::EncountWork* pEncountWork = m_desc.p_gman->GetGameData()->GetEncountWork();
      Encount::EncountEffect* pEncountEffect  = pEncountWork->GetEncountEffect();
      Encount::PokeSet& pokeSet = m_desc.p_gman->GetGameData()->GetEncountWork()->GetPokeSet();

      gfl2::heap::HeapBase* pHeap      = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
      gfl2::heap::HeapBase* pHeapDLL   = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );

      // バトルエフェクトID取得
      BATTLE_SETUP_PARAM battle_setup_param;
      BATTLE_PARAM_SetBattleEffect( &battle_setup_param, pokeSet.GetBattleEffectId() );

      // DLL読み込み
      pEncountWork->LoadEncountEffectDll( pHeapDLL );

      // 初期化
      Encount::EncountEffect::Description desc;
      desc.effectKind = static_cast<Encount::EncountEffect::EffectKind>( battle_setup_param.btlEffData.enceff_fld.eff_kind );
      pEncountEffect->Initialize( pHeap, desc );

      // 再生待機モードに設定
      pEncountEffect->SetSequenceStartupWaitMode( true );

      // 再生開始
      pEncountEffect->Start();

      m_seq++;
    }
    break;

  case SEQ_WAIT_ENCOUNT_EFFECT:
    {
      Encount::EncountWork* pEncountWork = m_desc.p_gman->GetGameData()->GetEncountWork();
      Encount::EncountEffect* pEncountEffect  = pEncountWork->GetEncountEffect();

      // 再生準備ができたかチェック
      if( pEncountEffect->IsStart() )
      {
        // フィールドキャプチャーが必要ならリクエストを出す
        if( pEncountEffect->IsNeedFieldCapture() )
        {
          pEncountEffect->RequestFieldCapture();
        }

        m_seq++;
      }
    }
    break;

  case SEQ_SETUP_ENCOUNT_EFFECT:
    {
      Encount::EncountWork* pEncountWork = m_desc.p_gman->GetGameData()->GetEncountWork();
      Encount::EncountEffect* pEncountEffect  = pEncountWork->GetEncountEffect();

      // シーケンス再生前の準備
      pEncountEffect->Setup();

      // 再生待機モードの解除
      pEncountEffect->SetSequenceStartupWaitMode( false );

      m_seq++;
    }
    break;

  case SEQ_COMMON_TERMINATE_PRE:
    {
      // シンボル破棄事前処理
      m_desc.pEvent->preTerminateSymbol( m_desc.p_gman);
      m_seq++;
    }
    break;

  case SEQ_COMMON_TERMINATE:
    {
      // シンボル破棄
      m_desc.pEvent->terminateSymbol( m_desc.p_gman);
      m_seq++;
    }
    break;

  case END:
    {
      m_seq = 0;
    }
    return true;
    //break;

  default:
    GFL_ASSERT_MSG( 0, "Sequence is invalid.");
    m_seq = 0;
    break;
  }

  return false;
}

//======================================================================
// ギミックエンカウント飛び出し演出ありシーケンス更新
//======================================================================

/**
* @brief コンストラクタ
*/
EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::EventGimmickEncountJumpMoveEffectUpdate()
  : EventGimmickEncount::IEventGimmickEncountUpdate()
  , m_pCapturePolygon( NULL)
  , m_pPokeModel( NULL)
  , m_jumpMoveVec( gfl2::math::Vector3::GetZero())
  , m_startPosition( gfl2::math::Vector3::GetZero())
  , m_goalPosition( gfl2::math::Vector3::GetZero())
  , m_goalLength( 0.0f)
  , m_pokeWaistJointIndex( -1)
  , m_pTextureResourceNode( NULL)
  , m_pEffect( NULL)
  , m_pAllocator( NULL)
  , m_pFieldmap( NULL)
  , m_pPipeLineManager( NULL)
  , m_pPipeLine( NULL)
  , m_pPokeModelFactory( NULL)
  , m_pEffectManager( NULL)
  , m_pStaticModelManager( NULL )
  , m_time( 0.0f)
  , m_timeAdd( 0.0f)
  , m_radian( 0.0f)
  , m_radianAdd( 0.0f)
  , m_pokeBaseScale( 0.57f)
{
#if PM_DEBUG
  if( Debug::DebugTypes::m_isJumpMoveEnable)
  {
    m_pokeBaseScale = Debug::DebugTypes::m_pokeBaseScale;
  }
#endif // PM_DEBUG
}

/**
* @brief デストラクタ
*/
EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::~EventGimmickEncountJumpMoveEffectUpdate()
{
}

/**
* @brief セットアップ
* @param desc セットアップ情報
*/
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::Setup( const Description& desc)
{
  m_desc              = desc;
  m_pFieldmap         = desc.p_gman->GetFieldmap();
  m_pPipeLineManager  = m_pFieldmap->GetFieldRenderingPipeLineManager();
  m_pPokeModelFactory = m_pFieldmap->GetTrialModelFactory();
  m_pEffectManager    = m_pFieldmap->GetEffectManager();
  m_pStaticModelManager = m_pFieldmap->GetStaticModelManager();
}

/**
* @brief 更新
* @retval true 完了
* @retval false 実行中
*/
bool EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::Update()
{
  enum Seq
  {
    SEQ_ICON_INITIALIZE,
    SEQ_ICON_WAIT,
    SEQ_PLAYER_ROTATE,
    SEQ_PLAYER_ROTATE_WAIT,
    SEQ_SYMBOL_JUMP_MOVE_ANIMATION,
    SEQ_SYMBOL_JUMP_MOVE_ANIMATION_WAIT,
    SEQ_CAPTURE_INITIALIZE,
    SEQ_CAPTURE_INITIALIZE_WAIT,
    SEQ_POKEMODEL_SYSTEM_INITIALIZE,
    SEQ_POKEMODEL_SYSTEM_INITIALIZE_WAIT,
    SEQ_POKEMODEL_INITIALIZE,
    SEQ_POKEMODEL_INITIALIZE_WAIT,
    SEQ_JUMP_EVENT_INITIALIZE,
    SEQ_JUMP_EVENT_INITIALIZE_WAIT,
    SEQ_JUMP_EVENT_UPDATE,
    SEQ_FADE_OUT,
    SEQ_FADE_OUT_WAIT,
    SEQ_JUMP_EVENT_TERMINATE_PRE,
    SEQ_JUMP_EVENT_TERMINATE,
    SEQ_JUMP_EVENT_TERMINATE_WAIT,
    SEQ_END,
  };

  switch( m_seq)
  {
  case SEQ_ICON_INITIALIZE:
    {
      // アロケーター作成
      this->initializeAllocator( m_desc.pHeap);

      // [！]生成
      m_desc.pEvent->initializeIcon( m_pEffectManager, m_desc.pEvent->m_pMoveModelForIcon);
      m_seq++;
    }
    break;

  case SEQ_ICON_WAIT:
    {
      if( m_desc.pEvent->updateIcon())
      {
        // [！]削除
        m_desc.pEvent->terminateIcon( m_pEffectManager);
        m_seq++;
      }
    }
    break;

  case SEQ_PLAYER_ROTATE:
    {
      // ポケモン初期位置をここで求めておく
      m_startPosition = this->CalculateFirstPosition( m_desc.pEvent->m_pokeMoveType);
      gfl2::math::Vector3 pokePos = m_startPosition;

      // プレイヤー位置取得
      MoveModel::FieldMoveModel* pPlayer = m_pFieldmap->GetPlayerCharacter();
      gfl2::math::Vector3 playerPos = pPlayer->GetCharaDrawInstance()->GetPosition();

      // Y座標は無視
      pokePos.y = 0.0f;
      playerPos.y = 0.0f;

      gfl2::math::Vector3 v0( 0.0f, 0.0f, 1.0f);
      gfl2::math::Vector3 v1( 0.0f, 0.0f, 1.0f);

      gfl2::math::Vector3 vec = pokePos - playerPos;
      if( gfl2::math::IsAlmostZero( vec.Length() ) == false )
      {
        v1 = vec;
      }

      f32 angleDeg = gfl2::math::ConvRadToDeg( gfl2::math::FACos( v0.Dot( v1) / ( v0.Length() * v1.Length())) );
      if( v1.x < 0.0f )
      {
        angleDeg = (180.0f - angleDeg) + 180.0f; // 反転
      }

      pPlayer->SetActionCommandRotation( MoveModel::FIELD_ACTION_COMMAND_ROTATION, angleDeg, 0.0f, 0.0f, 8);

      m_seq++;
    }
    break;

  case SEQ_PLAYER_ROTATE_WAIT:
    {
      MoveModel::FieldMoveModel* pPlayer = m_pFieldmap->GetPlayerCharacter();
      if( pPlayer->IsExecuteActionCommand() == false)
      {
        pPlayer->EnableInterest( m_startPosition);
        m_seq++;
      }
    }
    break;

  case SEQ_SYMBOL_JUMP_MOVE_ANIMATION:
    {
      // 待機型以外は何もしないで抜ける
      if( m_desc.pEvent->m_desc.pActor->GetLocalWork()->GetGimmickType() != GimmickEncount::GIMMICK_TYPE_STANDBY)
      {
        m_seq = SEQ_CAPTURE_INITIALIZE;
        break;
      }

      // 配置モデル飛び出し時アニメーション再生
      u32 eventID = m_desc.pEvent->m_desc.pActor->GetGimmickEncountData()->eventID;
      StaticModel::StaticModel* pStaticModel = m_pStaticModelManager->GetEventIDStaticModel( eventID);
      StaticModel::StaticModel::Handle_StartEventAnimation handle( 1);
      pStaticModel->Set( &handle);
      m_seq++;

      // @fix GFNMCat[1321] 使用用途を変更。飛び出し時になる音から飛び出し前の音になりました
      // 飛び出し時のSE
      if( m_desc.pEvent->m_jumpSE != SMID_NULL)
      {
        Sound::PlaySE( m_desc.pEvent->m_jumpSE);
      }
    }
    break;

  case SEQ_SYMBOL_JUMP_MOVE_ANIMATION_WAIT:
    {
      // 配置モデルのアニメーション完了を待つ
      // 待機型以外はここは通らない
      u32 eventID = m_desc.pEvent->m_desc.pActor->GetGimmickEncountData()->eventID;
      StaticModel::StaticModel* pStaticModel = m_pStaticModelManager->GetEventIDStaticModel( eventID);
      StaticModel::StaticModel::Handle_IsEndEventAnimation handle( 1);
      if( pStaticModel->IsEnd( handle))
      {
        m_seq++;
      }
    }
    break;

  case SEQ_CAPTURE_INITIALIZE:
    {
      // テクスチャ生成初期化
      this->initializeTexture();
      m_seq++;
    }
    break;

  case SEQ_CAPTURE_INITIALIZE_WAIT:
    {
      // テクスチャ生成完了を待つ
      if( this->initializeWaitTexture())
      {
        // キャプチャ貼り付け用モデル生成
        this->initializeCapturePolygon();

        // テクスチャ差し替え
        if( m_pCapturePolygon)
        {
          poke_3d::model::BaseModel* pCapturePolygonModel = m_pCapturePolygon->GetBaseModel();
          gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode = pCapturePolygonModel->GetModelInstanceNode();
          gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialNode = pModelNode->GetMaterialInstanceNode( 0);
          pMaterialNode->SetTextureResourceNode( 0, m_pTextureResourceNode);

#if defined(GF_PLATFORM_CTR)
          // CTRのみ、UV値の回転と平行移動
          pCapturePolygonModel->SetMaterialTextureCoordinatorRotateUV(0, 0, GFL_MATH_DEG_TO_RAD(-90));
          gfl2::math::Vector2 trans( 0.0f, 0.218750f );
          pCapturePolygonModel->SetMaterialTextureCoordinatorTranslateUV(0, 0, trans);
#endif // GF_PLATFORM_CTR
        }


        // キャプチャーパイプライン生成
        FieldRenderingPipeLineManager* m_pPipeLineManager = m_pFieldmap->GetFieldRenderingPipeLineManager();
        FieldRenderingPipeLineManager::CapturePipeLineCreateDescription desc;
        desc.pHeap        = m_desc.pHeap;
        desc.pAllocator   = m_pAllocator;
        desc.pDrawEnvNode = m_pFieldmap->GetFieldEnvManager()->GetDrawEnvNode();
        m_pPipeLineManager->CreateCaptureRenderingPipeLine( desc);

        // アクセスしやすいよう取得しておく
        m_pPipeLine = m_pPipeLineManager->GetCaptureRenderingPipeLine();

        // キャプチャ表示ON
        if( m_pCapturePolygon)
        {
          m_pCapturePolygon->GetBaseModel()->SetVisible( true);
          // キャプチャモデルを描画パイプラインに登録
          {
            gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode = m_pCapturePolygon->GetBaseModel()->GetModelInstanceNode();
            m_pPipeLineManager->GetCaptureRenderingPipeLine()->GetOrthoRenderPath()->AddRenderingTarget( pModelNode);
          }
        }

        // 描画パイプライン変更
        m_pPipeLineManager->ChangeExecuteType( FieldRenderingPipeLineManager::EXECUTE_TYPE_CAPTURE);

      }
      m_seq++;
    }
    break;

  case SEQ_POKEMODEL_SYSTEM_INITIALIZE:
    {
      // ポケモンモデルシステム初期化
      this->initializePokeModelSystem();
      m_seq++;
    }
    break;

  case SEQ_POKEMODEL_SYSTEM_INITIALIZE_WAIT:
    {
      // ポケモンシステムの初期化を待つ
      if( this->initializeWaitPokeModelSystem())
      {
        m_seq++;
      }
    }
    break;

  case SEQ_POKEMODEL_INITIALIZE:
    {
      // ポケモン生成リクエスト
      this->initializePokeModel();
      m_seq++;
    }
    break;

  case SEQ_POKEMODEL_INITIALIZE_WAIT:
    {
      if( this->initializeWaitPokeModel())
      {
        m_seq++;
      }
    }
    break;

  case SEQ_JUMP_EVENT_INITIALIZE:
    {
      FieldRenderingPipeLineManager* m_pPipeLineManager = m_pFieldmap->GetFieldRenderingPipeLineManager();


      // プレイヤーコリジョンデータ取得
      gfl2::math::Vector3 playerPos = m_pFieldmap->GetPlayerCharacter()->GetCharaDrawInstance()->GetPosition();
      u8 sex = m_desc.p_gman->GetGameData()->GetPlayerStatusConst()->GetSex();
      u32 loadCharacterID = (PM_MALE == sex) ? CHARA_MODEL_ID_PC0001_00 : CHARA_MODEL_ID_PC0002_00;
      Field::MoveModel::ApplicationDataForPlayer* pPlayerData = (Field::MoveModel::ApplicationDataForPlayer*)m_pFieldmap->GetMoveModelManager()->GetApplicationData( loadCharacterID );

      // 目標位置作成
      m_goalPosition = this->CalculateGoalPosition( m_desc.pEvent->m_pokeMoveType, pPlayerData->collisionHeight);

      // @fix 開始位置と目標位置が同じなら開始位置を上にずらして動作を担保する
      gfl2::math::Vector3 checkVec = m_goalPosition - m_startPosition;
      if( gfl2::math::IsAlmostZero( checkVec.Length() ) )
      {
        m_startPosition.y += 100.0f;
      }

      // ゴールとする差を作成
      m_goalLength = this->CalculateGoalLength( m_desc.pEvent->m_pokeMoveType, pPlayerData->collisionR, pPlayerData->collisionHeight);


      // 移動ベクトル作成
      m_jumpMoveVec = this->CalculateMoveVector( m_desc.pEvent->m_pokeMoveType);


      // カーブに使用するパラメーターを初期化
      f32 rate = ((m_goalPosition - m_startPosition).Length()) / m_jumpMoveVec.Length();
      m_radian = 0.0f;
      m_radianAdd = PI2 / rate;

      m_time = 0.0f;
      m_timeAdd = 1.0f / rate;


      // ジョイント番号取得
      PokeTool::PokeModel* pPokeModel = m_pPokeModel->GetPokeModel();
      m_pokeWaistJointIndex = pPokeModel->GetJointIndex( "Waist");

      // ポケモン回転初期化
      // @fix GFNMCat[534] ギミックエンカウントの飛び出すポケモンの向きが自機の方向を向いていない
      // こちらは初期化忘れ。1フレームだけ初期化時の方向を向いていた
      gfl2::math::Quaternion rotQt = this->CalculateRotation( pPokeModel->GetPosition());
      pPokeModel->SetRotationQuat( rotQt);


      // ポケモンモデル表示ON
      m_pPokeModel->SetDisp( true);
      // ポケモンモデルを描画パイプラインに登録
      {
        gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode = m_pPokeModel->GetPokeModel()->GetModelInstanceNode();
        m_pPipeLineManager->GetCaptureRenderingPipeLine()->AddEdgeRenderingTarget( pModelNode);
      }



      // 飛び出し移動時のSE
      if( m_desc.pEvent->m_jumpMoveSE != SMID_NULL)
      {
        Sound::PlaySE( m_desc.pEvent->m_jumpMoveSE);
      }


      // エフェクト生成
      if( m_desc.pEvent->m_jumpEffectType != Effect::EffectGimmickEncount::EFFECT_TYPE_NONE)
      {
        this->initializeEffect( m_desc.pEvent->m_symbolPos);
      }


      // カラーシェーダーをキャプチャパイプラインに登録
      this->initializeColorShaderPath();


      // モーションブラーパスの初期化
      m_pPipeLineManager->GetCaptureRenderingPipeLine()->InitializeMotionBlurPath(
        m_pAllocator,
        m_desc.pHeap,
        m_desc.p_gman->GetAsyncFileManager(),
        gfl2::Fade::FadeManager::COLOR_BUFFER_LOWER
        );

      m_seq++;
    }
    break;

  case SEQ_JUMP_EVENT_INITIALIZE_WAIT:
    {
      // モーションブラー初期化待ち
      if( m_pPipeLineManager->GetCaptureRenderingPipeLine()->WaitInitializeMotionBlurPath())
      {
        // 開始
        System::MotionBlur::BlendType blendType = System::MotionBlur::BLEND_PLANE;
        System::MotionBlur::AnimeType animeType = System::MotionBlur::ANIM_SCALE;
#if PM_DEBUG
        if( Debug::DebugGimmickEncount::m_isDebugMotionBlurEnable)
        {
          blendType = static_cast<System::MotionBlur::BlendType>( Debug::DebugGimmickEncount::m_motionBlurBlendType);
          animeType = static_cast<System::MotionBlur::AnimeType>( Debug::DebugGimmickEncount::m_motionBlurAnimeType);
        }
#endif
        m_pPipeLineManager->GetCaptureRenderingPipeLine()->GetMotionBlurPath()->RenderStart(
          blendType,
          animeType,
          System::GflUse::GetRenderTargetColor( gfl2::gfx::CtrDisplayNo::LEFT ),
          true
          );

        // ブレンド比コントロール
        System::MotionBlur::Param paramBlend;
        paramBlend.type = System::MotionBlur::TYPE_SPLINE;
        paramBlend.end = 0.0f;
        paramBlend.frame = 25;
#if PM_DEBUG
        if( Debug::DebugGimmickEncount::m_isDebugMotionBlurBlendInterpEnable)
        {
          paramBlend.type = static_cast<System::MotionBlur::Type>( Debug::DebugGimmickEncount::m_motionBlurBlendInterpType);
          paramBlend.end = Debug::DebugGimmickEncount::m_motionBlurBlendEnd;
          paramBlend.frame = Debug::DebugGimmickEncount::m_motionBlurBlendFrame;
        }
#endif
        m_pPipeLineManager->GetCaptureRenderingPipeLine()->GetMotionBlurPath()->SetBlendControl( paramBlend);

        // カラーフィルターコントロール
        System::MotionBlur::ColorParam paramColor;
        paramColor.type = System::MotionBlur::TYPE_SET;
        paramColor.end = gfl2::math::Vector3::GetZero();
        paramColor.frame = 0;
#if PM_DEBUG
        if( Debug::DebugGimmickEncount::m_isDebugMotionBlurColorInterpEnable)
        {
          paramColor.type = static_cast<System::MotionBlur::Type>( Debug::DebugGimmickEncount::m_isDebugMotionBlurColorInterpEnable);
          paramColor.end = Debug::DebugGimmickEncount::m_motionBlurColorEnd;
          paramColor.frame = Debug::DebugGimmickEncount::m_motionBlurColorFrame;
        }
#endif
        m_pPipeLineManager->GetCaptureRenderingPipeLine()->GetMotionBlurPath()->SetColorFilter( paramColor);

        m_seq++;
      }
    }
    break;

  case SEQ_JUMP_EVENT_UPDATE:
    {
      // ポケモデル更新
      this->updatePokeModel();

      // ポケモデルシステム更新
      this->updatePokeModelSystem();

      // 補間値更新
      m_radian += m_radianAdd;
      if( m_radian > PI2)
      {
        m_radian = PI2;
      }

      m_time += m_timeAdd;
      if( m_time > 1.0f)
      {
        m_time = 1.0f;
      }

      // 終了判定
      // @fix Easing処理に変更
      {
        gfl2::math::Vector3 afterPos = m_pPokeModel->GetPosition();
        f32 nowDiff = (m_goalPosition - afterPos).Length();
        if( nowDiff < m_goalLength || m_time >= 1.0f)
        {
          m_seq++;
        }
      }

      // ポケモンアニメーションフレームループ
      if( m_pPokeModel->IsAnimationLastFrame())
      {
        m_pPokeModel->GetPokeModel()->SetAnimationFrame( m_desc.pEvent->m_pPokeData->motionStartFrame);
      }
    }
    break;

  case SEQ_FADE_OUT:
    {
      // フェードリクエスト
      gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f);
      gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f);
      // @fix NMCat[1473] フェードアウト時間を調整
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->RequestOut( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col, 3 );

      m_seq++;
    }
    break;

  case SEQ_FADE_OUT_WAIT:
    {
      // フェードを待つ
      if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->IsEnd( gfl2::Fade::DISP_DOUBLE))
      {
        m_seq++;
      }
    }
    break;

  case SEQ_JUMP_EVENT_TERMINATE_PRE:
    {
      // パイプラインを戻す
      m_pPipeLineManager->ChangeExecuteType( FieldRenderingPipeLineManager::EXECUTE_TYPE_NORMAL);

      // カラーシェーダーをパイプラインから削除
      this->terminateColorShaderPath();

      // ポケモデル破棄事前処理
      this->preTerminatePokeModel();

      // キャプチャ破棄事前処理
      this->preTerminateCapturePolygon();

      // エフェクト破棄
      this->terminateEffect();

      // モーションブラーOFF
      m_pPipeLineManager->GetCaptureRenderingPipeLine()->GetMotionBlurPath()->RenderOff();

      m_seq++;
    }
    break;

  case SEQ_JUMP_EVENT_TERMINATE:
    {
      // キャプチャ破棄
      this->terminateCapturePolygon();

      // ポケモンモデル破棄
      this->terminatePokeModel();

      // ポケモンシステム削除
      this->terminatePokeModelSystem();

      m_seq++;
    }
    break;

  case SEQ_JUMP_EVENT_TERMINATE_WAIT:
    {
      if( this->terminateWaitPokeModelSystem())
      {
        FieldRenderingPipeLineManager* m_pPipeLineManager = m_pFieldmap->GetFieldRenderingPipeLineManager();

        // モーションブラーパス破棄
        m_pPipeLineManager->GetCaptureRenderingPipeLine()->TerminateMotionBlurPath();

        // キャプチャーパイプライン削除
        m_pPipeLineManager->DeleteCaptureRenderingPipeLine();

        // テクスチャ削除
        this->terminateTexture();

        // アロケーター破棄
        this->terminateAllocator();

        m_seq++;
      }
    }
    break;

  case SEQ_END:
    m_seq = 0;
    return true;
  }

  return false;
}

/**
* @brief バトル呼び出しまでの更新
* @retval true 完了
* @retval false 実行中
*/
bool EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::UpdateBattleCall()
{
  enum Seq
  {
    SEQ_COMMON_TERMINATE_PRE,
    SEQ_COMMON_TERMINATE,
    END,
  };

  switch( m_seq)
  {
  case SEQ_COMMON_TERMINATE_PRE:
    {
      // シンボル破棄事前処理
      m_desc.pEvent->preTerminateSymbol( m_desc.p_gman);
      m_seq++;
    }
    break;

  case SEQ_COMMON_TERMINATE:
    {
      // シンボル破棄
      m_desc.pEvent->terminateSymbol( m_desc.p_gman);
      m_seq++;
    }
    break;

  case END:
    {
      m_seq = 0;
    }
    return true;
    //break;

  default:
    GFL_ASSERT_MSG( 0, "Sequence is invalid.");
    m_seq = 0;
    break;
  }

  return false;
}

/**
 * @brief アロケーター初期化
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::initializeAllocator( gfl2::heap::HeapBase* pHeap)
{
  m_pAllocator = GFL_NEW( pHeap) System::nijiAllocator( pHeap);
}

/**
 * @brief アロケーター破棄
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::terminateAllocator()
{
  GFL_SAFE_DELETE( m_pAllocator);
}

/**
 * @brief ポケモンモデルシステム初期化
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::initializePokeModelSystem()
{
  // ポケモンモデルシステム初期化
  m_pPokeModelFactory->InitializeSystem( m_desc.pHeap, m_desc.pTemporaryHeap, 1, false ); // 1体のみ表示
}

/**
 * @brief ポケモンモデルシステム初期化待ち
 */
bool EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::initializeWaitPokeModelSystem()
{
  if( m_pPokeModelFactory->IsInitialize())
  {
    // ポケモンモデルシステム生成
    PokeTool::PokeModelSystem::HeapOption opt;
    opt.animeType       = static_cast<PokeTool::MODEL_ANIMETYPE>( m_desc.pEvent->m_pPokeData->motionType);
    opt.useIdModel      = false;
    opt.useShadow       = false;
    opt.useColorShader  = true;
    m_pPokeModelFactory->CreateSystem( m_desc.pHeap, m_pAllocator, &opt);
    return true;
  }
  return false;
}

/**
 * @brief ポケモンモデルシステム更新
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::updatePokeModelSystem()
{
  m_pPokeModelFactory->Update();
}

/**
 * @brief ポケモンモデルシステム破棄
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::terminatePokeModelSystem()
{
  m_pPokeModelFactory->DeleteSystem();
}

/**
 * @brief ポケモンモデルシステム破棄待ち
 */
bool EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::terminateWaitPokeModelSystem()
{
  if( m_pPokeModelFactory->IsDelete())
  {
    m_pPokeModelFactory->FinalizeSystem();
    return true;
  }
  return false;
}

/**
 * @brief キャプチャ貼り付け用板ポリゴン初期化
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::initializeCapturePolygon()
{
  m_pCapturePolygon = reinterpret_cast<Effect::EffectDummyPolygon*>( m_pEffectManager->CreateEffect( Effect::EFFECT_TYPE_DUMMY_POLYGON, gfl2::math::Vector3( 0.0f, 0.0f, 0.0f)));
  if( m_pCapturePolygon)
  {
    m_pCapturePolygon->GetBaseModel()->SetVisible( false);
    m_pPipeLineManager->GetMyRenderingPipeLine()->RemoveEdgeRenderingTarget( m_pCapturePolygon->GetBaseModel()->GetModelInstanceNode());
  }
}

/**
 * @brief キャプチャ貼り付け用板ポリゴン破棄事前処理
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::preTerminateCapturePolygon()
{
  if( m_pCapturePolygon)
  {
    m_pCapturePolygon->GetBaseModel()->SetVisible( false);
    m_pPipeLineManager->GetCaptureRenderingPipeLine()->GetOrthoRenderPath()->RemoveRenderingTarget( m_pCapturePolygon->GetBaseModel()->GetModelInstanceNode());
  }
}

/**
 * @brief キャプチャ貼り付け用板ポリゴン破棄
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::terminateCapturePolygon()
{
  if( m_pCapturePolygon)
  {
    m_pEffectManager->DeleteEffect( m_pCapturePolygon);
  }
}

/**
 * @brief ポケモンモデル初期化
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::initializePokeModel()
{
  PokeTool::SimpleParam simpleParam;
  simpleParam.monsNo = static_cast<MonsNo>( m_desc.pEvent->m_pEncPokeParam->monsNo);
  simpleParam.formNo = static_cast<pml::FormNo>( m_desc.pEvent->m_pEncPokeParam->form);
  simpleParam.sex = static_cast<pml::Sex>( m_desc.pEvent->m_pEncPokeParam->sex);

  PokeTool::PokeModel::SetupOption setupOption;
  setupOption.dataHeap = m_desc.pHeap;
  setupOption.workHeap = m_desc.pTemporaryHeap;
  setupOption.animeType = static_cast<PokeTool::MODEL_ANIMETYPE>( m_desc.pEvent->m_pPokeData->motionType);

  m_pPokeModel = m_pPokeModelFactory->CreateTrialModel( m_desc.pHeap, &simpleParam, setupOption);
}

/**
 * @brief ポケモンモデル初期化待ち
 */
bool EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::initializeWaitPokeModel()
{
  if( m_pPokeModel->IsReady())
  {
    // ポケモン生成
    m_pPokeModel->CreateModel();
    m_pPokeModel->SetDisp( false);

    // カラーシェーダーセットアップ
    PokeTool::PokeModel* pPokeModel = m_pPokeModel->GetPokeModel();
    pPokeModel->SetupColorShader();
    pPokeModel->SetShaderColor( gfl2::math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f));

    // ポケモンアニメーション設定
    pPokeModel->ChangeAnimation( static_cast<PokeTool::MODEL_ANIME>( m_desc.pEvent->m_pPokeData->motionID), true);
    pPokeModel->SetAnimationLoop( false); // 手動で開始フレームを設定するため
    pPokeModel->SetAnimationStepFrame( m_desc.pEvent->m_pPokeData->motionStepFrame);
    pPokeModel->SetAnimationFrame( m_desc.pEvent->m_pPokeData->motionStartFrame);

    // ポケモン位置変更
    pPokeModel->SetPosition( m_startPosition);

    // スケール変更
    f32 scale = this->CalculateScale();
    pPokeModel->SetScale( gfl2::math::Vector3( scale, scale, scale));

    f32 fieldAdjustHeight = pPokeModel->GetSettingData()->fieldAdjustHeight * 0.01f;
    pPokeModel->SetAdjustHeight( fieldAdjustHeight);

    m_pPipeLineManager->GetMyRenderingPipeLine()->RemoveEdgeRenderingTarget( m_pPokeModel->GetPokeModel()->GetModelInstanceNode());

    return true;
  }

  return false;
}

/**
 * @brief ポケモンモデル更新
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::updatePokeModel()
{
  // 位置更新
  PokeTool::PokeModel* pPokeModel = m_pPokeModel->GetPokeModel();
  gfl2::math::Vector3 afterPos;

  // @fix Easing処理に変更
  {
    afterPos.x = gfl2::math::Easing::GetValue( m_desc.pEvent->m_easingFunc, m_startPosition.x, m_goalPosition.x, m_time);
    afterPos.y = gfl2::math::Easing::GetValue( m_desc.pEvent->m_easingFunc, m_startPosition.y, m_goalPosition.y, m_time);
    afterPos.z = gfl2::math::Easing::GetValue( m_desc.pEvent->m_easingFunc, m_startPosition.z, m_goalPosition.z, m_time);

    // Y座標変更してカーブさせる
    f32 rate = gfl2::math::SinRad( m_radian);
    afterPos.y += (rate * m_desc.pEvent->m_pPokeData->moveCurve);
  }

  pPokeModel->SetPosition( afterPos);


  // 向き更新
  // @fix GFNMCat[534] ギミックエンカウントの飛び出すポケモンの向きが自機の方向を向いていない
  // カメラ用のGetLookAt関数を使用していたため。クォータニオン作成関数を作ってそれを使用するように修正
  gfl2::math::Quaternion rotQt = this->CalculateRotation( afterPos);
  pPokeModel->SetRotationQuat( rotQt);

  // スケール更新
  f32 scale = this->CalculateScale();
  pPokeModel->SetScale( gfl2::math::Vector3( scale, scale, scale));
}

/**
 * @brief ポケモンモデル破棄事前処理
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::preTerminatePokeModel()
{
  // ポケモン描画OFF
  m_pPokeModel->SetDisp( false);
  m_pPipeLineManager->GetCaptureRenderingPipeLine()->RemoveEdgeRenderingTarget( m_pPokeModel->GetPokeModel()->GetModelInstanceNode());

  // カラーシェーダークリーン
  m_pPokeModel->GetPokeModel()->CleanColorShader();
}

/**
 * @brief ポケモンモデル破棄
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::terminatePokeModel()
{
  // ポケモン削除
  m_pPokeModelFactory->DeleteTrialModel( m_pPokeModel);
}

/**
 * @brief キャプチャテクスチャ初期化
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::initializeTexture()
{
  // キャプチャリクエスト
  gfl2::gfx::Texture* pTexture = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->GetColorBufferTexture( gfl2::Fade::FadeManager::COLOR_BUFFER_UPPER);
  GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetFieldmap()->GetFieldRenderingPipeLineManager()->RequestTransferColorBuffer( pTexture);
}

/**
 * @brief キャプチャテクスチャ初期化待ち
 */
bool EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::initializeWaitTexture()
{
  gfl2::gfx::Texture* pTexture = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->GetColorBufferTexture( gfl2::Fade::FadeManager::COLOR_BUFFER_UPPER);

  const c8* pFactoryName;
  pFactoryName = gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory::GetFactoryTypeName();
  gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory;
  pTextureResourceNodeFactory = reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>( gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory( gfl2::renderingengine::scenegraph::resource::IResourceData::Type( pFactoryName)));

  m_pTextureResourceNode = reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode*>(pTextureResourceNodeFactory->CreateResourceNode( m_pAllocator, "capture", pTexture, false));

#ifdef DEBUG_ONLY_FOR_saita_kazuki
#if defined(GF_PLATFORM_WIN32)
  // Win32のみキャプチャを画像として保存しておく(確認用)
  void * addr = pTexture->Lock();
  gfl2::gfx::PCDebugUtil::PrintBlockImage( addr, "screenshot/colorbuffer_block.bmp", 512, 256, 4, m_desc.pHeap );
  gfl2::gfx::PCDebugUtil::PrintImage( addr, "screenshot/colorbuffer_liner.bmp", 512, 256, 4, m_desc.pHeap );
  pTexture->UnLock();
#endif // GF_PLATFORM_WIN32
#endif // DEBUG_ONLY_FOR_saita_kazuki

  return true;
}

/**
 * @brief キャプチャテクスチャ破棄
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::terminateTexture()
{
  GFL_SAFE_DELETE( m_pTextureResourceNode);
}

/**
 * @brief カラーシェーダー初期化
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::initializeColorShaderPath()
{
  PokeTool::PokeModelSystem* pPokeModelSystem = m_pPokeModelFactory->GetPokeModelSystem();
  s32 pokeShaderPathNum = pPokeModelSystem->GetPokeShaderPathNum();
  for( s32 i = 0; i < pokeShaderPathNum; ++i)
  {
    m_pPipeLine->AddPokeShaderPath( pPokeModelSystem->GetPokeShaderPath( i));
  }
}

/**
 * @brief カラーシェーダー破棄
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::terminateColorShaderPath()
{
  // カラーシェーダーをパイプラインから削除
  PokeTool::PokeModelSystem* pPokeModelSystem = m_pPokeModelFactory->GetPokeModelSystem();
  s32 pokeShaderPathNum = pPokeModelSystem->GetPokeShaderPathNum();
  for( s32 i = 0; i < pokeShaderPathNum; ++i)
  {
    m_pPipeLine->RemovePokeShaderPath( pPokeModelSystem->GetPokeShaderPath( i));
  }
}

/**
 * @brief エフェクト初期化
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::initializeEffect( const gfl2::math::Vector3& pos)
{
  if( m_pEffect == NULL)
  {
    m_pEffect =  m_pEffectManager->CreateEffect(
      Effect::EFFECT_TYPE_GIMMICK_ENCOUNT,
      pos,
      false
      );

    Effect::EffectGimmickEncount* pEffect = reinterpret_cast<Effect::EffectGimmickEncount*>( m_pEffect);
    if( pEffect)
    {
      pEffect->Create( static_cast<Effect::EffectGimmickEncount::EffectType>( m_desc.pEvent->m_jumpEffectType));
    }
  }
}

/**
 * @brief エフェクト破棄
 */
void EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::terminateEffect()
{
  if( m_pEffect)
  {
    m_pEffectManager->DeleteEffect( m_pEffect);
    m_pEffect = NULL;
  }
}

/**
 * @brief ポケモン初期位置計算
 */
gfl2::math::Vector3 EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::CalculateFirstPosition( EventGimmickEncount::PokeMoveType type)
{
  gfl2::math::Vector3 afterPos( 0.0f, 0.0f, 0.0f);

  switch( type)
  {
  case EventGimmickEncount::POKE_MOVE_TYPE_FALL:
    {
      const poke_3d::model::BaseCamera* pCamera = m_pFieldmap->GetCameraManager()->GetMainViewCamera()->GetBaseCamera();

      gfl2::math::Vector3 symbolPos = m_desc.pEvent->m_symbolPos;
      gfl2::math::Vector3 camPos = pCamera->GetPosition();
      gfl2::math::Vector3 targetPos = pCamera->GetTargetPosition();
      f32 fov = pCamera->GetFovy();

      gfl2::math::Vector3 camNor = (targetPos - camPos).Normalize();
      gfl2::math::Quaternion qt( gfl2::math::Vector3( 1.0f, 0.0f, 0.0f), fov * 0.5f);

      camNor = qt.Transform( camNor);
      gfl2::math::Vector3 symbolVec = symbolPos - camPos;
      afterPos = camPos + (camNor * (symbolVec.Dot(camNor)));

      afterPos.SetX( symbolPos.x);
      afterPos.SetZ( symbolPos.z);

      // @attention オニドリル、ウォーグルのみ専用の値を反映しています
      // @fix 通常は100加算、特定ポケモンのみ専用の値を用意
      f32 addY = 100.0f;
      switch( m_desc.pEvent->m_pEncPokeParam->monsNo )
      {
      case MONSNO_ONIDORIRU:
      case MONSNO_WHOOGURU:
        addY = 200.0f;
        break;
      }

#if PM_DEBUG
      if( Debug::DebugTypes::m_isPokeStartPosYEnable)
      {
        addY = Debug::DebugTypes::m_pokeStartPosYOffset;
      }
#endif // PM_DEBUG

      afterPos.y += addY;
    }
    break;

  case EventGimmickEncount::POKE_MOVE_TYPE_COME:
    {
      afterPos = m_desc.pEvent->m_symbolPos;
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }

  return afterPos;
}

/**
 * @brief ポケモン目標位置計算
 */
gfl2::math::Vector3 EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::CalculateGoalPosition( EventGimmickEncount::PokeMoveType type, f32 heightOffset)
{
  gfl2::math::Vector3 pos = m_startPosition;

  switch( type)
  {
  case EventGimmickEncount::POKE_MOVE_TYPE_FALL:
    {
      gfl2::math::AABB pokeAABB = m_pPokeModel->GetPokeModel()->GetModelInstanceNode()->GetAABB();
      // @fix AABBにスケールを適用していなかった
      f32 length = (pokeAABB.GetMax().y - pokeAABB.GetCenter().y) * m_pokeBaseScale;

      pos = m_desc.pEvent->m_symbolPos;
      pos.y += length;
    }
    break;

  case EventGimmickEncount::POKE_MOVE_TYPE_COME:
    {
      gfl2::math::AABB pokeAABB = m_pPokeModel->GetPokeModel()->GetModelInstanceNode()->GetAABB();
      gfl2::math::Vector4 aabbVec4 = pokeAABB.GetMax() - pokeAABB.GetCenter();
      gfl2::math::Vector aabbVec( aabbVec4.x, aabbVec4.y, aabbVec4.z);
      // @fix AABBにスケールを適用していなかった
      f32 aabbLength = aabbVec.Length() * m_pokeBaseScale;

      gfl2::math::Vector3 playerPos = m_pFieldmap->GetPlayerCharacter()->GetCharaDrawInstance()->GetPosition();
      playerPos.y += (heightOffset * 0.5f);

      // @fix プレイヤー位置と開始位置が同じなら
      gfl2::math::Vector3 vec = playerPos - m_startPosition;
      f32 length = vec.Length();
      if( gfl2::math::IsAlmostZero( length ) == false )
      {
        gfl2::math::Vector3 nor = vec.Normalize();
        length -= aabbLength;

        pos = m_startPosition + (nor * length);
      }
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }

  return pos;
}

/**
 * @brief ポケモン移動ベクトル計算
 */
gfl2::math::Vector3 EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::CalculateMoveVector( EventGimmickEncount::PokeMoveType type)
{
  const f32 MOVE_RATE = 30.0f;
  gfl2::math::Vector3 vec( 0.0f, 0.0f, 0.0f);

  // @fix
  // PokeModelからpokePosを取得していたが、この前に必ずstartPositionが入っているので、関係性をわかりやすくするため、startPositionを使用するように変更
  // 降りてくるタイプがギミックエンカウントモデルからsymbolPosを取得していたが、それだとAABBが考慮されていない位置なのでgoalPositionを使用するように変更
  // 上記の修正により、演出タイプで処理を分ける必要がなくなったためswitch文での分岐を削除
  gfl2::math::Vector3 pokePos = m_startPosition;
  gfl2::math::Vector3 symbolPos = m_goalPosition;

  gfl2::math::Vector3 nor = (symbolPos - pokePos).Normalize();
  vec = nor * MOVE_RATE;

#if PM_DEBUG
  if( Debug::DebugTypes::m_isJumpMoveEnable)
  {
    if( gfl2::math::IsAlmostZero( Debug::DebugTypes::m_jumpMoveSpeed ) == false)
    {
      vec = nor * Debug::DebugTypes::m_jumpMoveSpeed;
    }
  }
#endif

  return vec;
}

/**
 * @brief ポケモン目標長さ計算
 */
f32 EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::CalculateGoalLength( EventGimmickEncount::PokeMoveType type, f32 offsetR, f32 offsetHeight)
{
  f32 offset = 0.0f;

  switch( type)
  {
  case EventGimmickEncount::POKE_MOVE_TYPE_FALL:
    {
      offset = 0.0f;
    }
    break;

  case EventGimmickEncount::POKE_MOVE_TYPE_COME:
    {
      offset = offsetR;
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }

  f32 length = 0.0f;
  gfl2::math::AABB pokeAABB = m_pPokeModel->GetPokeModel()->GetModelInstanceNode()->GetAABB();
  // @fix AABBにスケールを適用していなかった
  f32 aabbLength = (pokeAABB.GetMax() - pokeAABB.GetCenter()).Length() * m_pokeBaseScale;
  length = aabbLength + offset;

  return length;
}

/**
 * @brief ポケモンスケール計算
 */
f32 EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::CalculateScale()
{
  const poke_3d::model::BaseCamera* pCamera = m_pFieldmap->GetCameraManager()->GetMainViewCamera()->GetBaseCamera();
  PokeTool::PokeModel* pPokeModel = m_pPokeModel->GetPokeModel();

  f32 fovyHalf = pCamera->GetFovy() * 0.5f;
  f32 z0 = 120.0f / fovyHalf;

  gfl2::math::Vector3 cameraPos = pCamera->GetPosition();
  gfl2::math::Vector3 pokePos   = pPokeModel->GetPosition();
  f32 z1 = gfl2::math::FAbs( pokePos.z - cameraPos.z);

  f32 scale = z1 / z0;

  return m_pokeBaseScale * scale;
}

/**
 * @brief 回転計算共通処理
 */
gfl2::math::Quaternion EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::CalculateRotationCore( const gfl2::math::Vector3& pos, const gfl2::math::Vector3& target)
{
  // @fix targetとposが同座標なら回転なしクォータニオンを返す
  f32 length = (target - pos).Length();
  if( gfl2::math::IsAlmostZero( length ) )
  {
    return gfl2::math::Quaternion( 0.0f, 0.0f, 0.0f, 1.0f);
  }

  gfl2::math::Vector3 v1( 0.0f, 0.0f, 1.0f);
  gfl2::math::Vector3 v2 = (target - pos).Normalize();

  gfl2::math::Vector3 axis = v1.Cross( v2);
  f32 cos = v1.Dot( v2);
  f32 radian = gfl2::math::FACos( cos);


  gfl2::math::Quaternion qt;

  // @fix GFNMCat[1705]ポケモンセンターからの出入りを素早く行おうとすると、扉を開ける際、自機が非表示になる類似
  if( !gfl2::math::IsAlmostZero( axis.LengthSq() ) )
  {
    axis = axis.Normalize();
    qt = gfl2::math::Quaternion( axis, radian);
  }
  else
  {
    qt = gfl2::math::Quaternion( gfl2::math::Vector3(0,1,0), radian);
  }

  return qt;
}

/**
 * @brief 回転計算
 */
gfl2::math::Quaternion EventGimmickEncount::EventGimmickEncountJumpMoveEffectUpdate::CalculateRotation( const gfl2::math::Vector3& pos)
{
  gfl2::math::Quaternion qt( 0.0f, 0.0f, 0.0f, 1.0f);

  switch( m_desc.pEvent->m_pokeMoveType)
  {
  case EventGimmickEncount::POKE_MOVE_TYPE_FALL:
    {
      gfl2::math::Vector3 playerPos = m_pFieldmap->GetPlayerCharacter()->GetCharaDrawInstance()->GetPosition();
      gfl2::math::Vector3 look( playerPos.x, pos.y, playerPos.z);

      qt = this->CalculateRotationCore( pos, look);
    }
    break;

  case EventGimmickEncount::POKE_MOVE_TYPE_COME:
    {
      qt = this->CalculateRotationCore( pos, m_goalPosition);
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }

  return qt;
}

//======================================================================
// ギミックエンカウントイベント
//======================================================================

/**
* @brief   コンストラクタ
*/
EventGimmickEncount::EventGimmickEncount( gfl2::heap::HeapBase* pHeap)
  : GameSys::GameEvent( pHeap)
  , m_playerLocation()
  , m_presetID( 0)
  , m_encountDataID( 0)
  , m_dropItemID( DropItem::DataAccessor::DROP_ITEM_INVALID_ID)
  , m_scriptID( FieldScript::SCRID_NULL)
  , m_wildBtlResult( BTL_RESULT_WILD_MAX)
  , m_isJumpEvent( false)
  , m_jumpSE( SMID_NULL)
  , m_jumpMoveSE( SMID_NULL)
  , m_flagwork( 0)
  , m_eventID( 0)
  , m_pokeMoveType( POKE_MOVE_TYPE_NONE)
  , m_jumpEffectType( 0)
  , m_pEncPokeParam( NULL)
  , m_pPokeData( NULL)
  , m_symbolPos( gfl2::math::Vector3::GetZero())
  , m_pUpdater( NULL)
  , m_pExclamationIcon( NULL)
  , m_pMoveModelForIcon( NULL)
  , m_easingFunc( gfl2::math::Easing::LINEAR)
  , m_battleCallOption( 0)
  , m_isSymbolEncount( false)
  , m_dropItemPos( gfl2::math::Vector3::GetZero())
  , m_dropItemType( FieldItemManager::TYPE_NORMAL )
  // 谷くん要望：吹き出しアイコンを優先順位高い版に変更
  , m_exclamationEffID( Effect::EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY )
{
  gfl2::std::MemClear( &m_desc, sizeof( m_desc));
}

/**
* @brief   デストラクタ
*/
EventGimmickEncount::~EventGimmickEncount()
{
}

/**
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @param   p_gman       ゲームマネージャークラス
* @retval  true  起動してよい
* @retval  false 起動しない
*/
bool EventGimmickEncount::BootChk(GameSys::GameManager* p_gman)
{
  return true;
}

/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   p_gman       ゲームマネージャークラス
*/
void EventGimmickEncount::InitFunc(GameSys::GameManager* p_gman)
{
  SAITA_PRINT( "Start EventGimmickEncount\n");

  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE);
  gfl2::heap::HeapBase* pTemporaryHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD);

  // イベントに必要なデータの初期化
  this->initializeEventData( p_gman);

  // ポケモン関連データ初期化
  this->initializePokeData( p_gman, pHeap);

  // 各型ごとの初期化
  this->initializeTypesData( p_gman);

  // 更新モジュール初期化
  this->initializeUpdater( p_gman, pHeap, pTemporaryHeap);

  // ギミックフラグOFF(このフラグはバニッシュフラグなのでOFFにする=フラグを立てる)
  GimmickEncount::LocalWork* pLocalWork = m_desc.pActor->GetLocalWork();
  m_desc.pEventWork->SetEventFlag( pLocalWork->GetGimmickFlagNo());

  // レコード
  this->RefrectRecode( m_desc.pActor->GetGimmickEncountData()->modelID, pLocalWork->GetGimmickType());

#if PM_DEBUG
  if( Debug::DebugTypes::m_isEasingEnable)
  {
    m_easingFunc = static_cast<gfl2::math::Easing::EaseFunc>( Debug::DebugTypes::m_easingFunc);
  }
#endif
}

/**
* @brief   呼び出される関数 純粋仮想関数
* @param   p_gman       ゲームマネージャークラス
* @return  GMEVENT_RESULT  イベント制御関数の戻り値
*/
GameSys::GMEVENT_RESULT EventGimmickEncount::MainFunc(GameSys::GameManager* p_gman)
{
  enum Seq
  {
    SEQ_EVENT_UPDATE,
    SEQ_EVENT_UPDATE_BATTLE_CALL,
    SEQ_CALL_ENCOUNT,
    SEQ_BATTLE_RESULT,
    SEQ_FIELD_OPEN,
    SEQ_CREATE_DROP_ITEM,
    SEQ_FADE_IN,
    SEQ_FADE_IN_WAIT,
    SEQ_CALL_SCRIPT,
    SEQ_END,
  };

  switch( this->GetSeqNo())
  {
  case SEQ_EVENT_UPDATE:
    {
      if( this->updateUpdater())
      {
        this->AddSeqNo();
      }
    }
    break;

  case SEQ_EVENT_UPDATE_BATTLE_CALL:
    {
      if( this->updateUpdaterBattleCall())
      {
        // 更新モジュール破棄
        this->terminateUpdater();

        this->AddSeqNo();
      }
    }
    break;

  case SEQ_CALL_ENCOUNT:
    {
      // オプション…非スクリプトからのコール、pokesetのアトリビュートを使う、フィールド生成シーケンススキップ、エンカウントエフェクトスキップ、SKIP_ENCEFFと合わせて使うとバトルBGMの再生のみ行う
      u32 call_option =
        EVBTL_CALLOPT_NOT_SCRIPT        |
        EVBTL_CALLOPT_ATTR_POKESET      |
        EVBTL_CALLOPT_SKIP_FIELD_OPEN   |
        EVBTL_CALLOPT_PLAY_BTL_BGM      |
        m_battleCallOption; // 演出の有り無しで変わる
      Encount::PokeSet& poke_set = p_gman->GetGameData()->GetEncountWork()->GetPokeSet();
      GameSys::GameEventManager* p_eventman = p_gman->GetGameEventManager();

      if( m_isSymbolEncount)
      {
        Encount::Encount::CallSymbolEncount( p_gman, m_encountDataID, call_option);
      }
      else
      {
        EventBattleCall::CallWild( p_eventman, p_gman, poke_set, call_option);
      }

      this->AddSeqNo();
    }
    break;

  case SEQ_BATTLE_RESULT:
    {
      BtlResult btlResult = p_gman->GetGameData()->GetLastBattleResult();
      m_wildBtlResult = EventBattleCall::GetWildBattleResult( btlResult);
      // 逃げたなら抜けて次のシーケンスへ
      if( m_wildBtlResult == BTL_RESULT_WILD_RUN)
      {
        this->AddSeqNo();
        break;
      }
      // 負けならイベント終了、
      // ゲームオーバーイベントでマップチェンジもするので何もしなくてOK
      if( m_wildBtlResult == BTL_RESULT_WILD_LOSE)
      {
        this->SetSeqNo( SEQ_END);
        break;
      }

      // 勝った場合フラグワークが設定されていたら立てる
      if( m_flagwork != WORK_NULL)
      {
        EventWork* pEventWork = p_gman->GetGameData()->GetEventWork();
        if( EventWork::CheckIDFlagRenge( m_flagwork))
        {
          pEventWork->SetEventFlag( m_flagwork);
        }
        else
        {
          // ギミックエンカウントはワーク設定は禁止
          GFL_ASSERT( 0);
        }
      }

      this->AddSeqNo();
    }
    break;

  case SEQ_FIELD_OPEN:
    {
      SAITA_PRINT( "Open Field\n");
      EventFieldmapOpen* p_event;

      FieldmapDesc desc;
      desc.openMode = FM_OPEN_MODE_APP_RECOVER;
      desc.playerLocation = m_playerLocation;

      // オープンイベント
      p_event = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, EventFieldmapOpen>( p_gman->GetGameEventManager());
      p_event->SetDesc( desc);
      p_event->SetFadeCallBack( NULL, NULL, NULL );

      this->AddSeqNo();
    }
    break;

  case SEQ_CREATE_DROP_ITEM:
    {
      SAITA_PRINT( "Create DropItem\n");

      //勝った、または、捕まえたことを条件にドロップアイテム生成
      if( m_wildBtlResult == BTL_RESULT_WILD_WIN || m_wildBtlResult  == BTL_RESULT_WILD_CAPTURE )
      {
        //ドロップアイテム生成処理
        Fieldmap* pFieldmap = p_gman->GetFieldmap();
        DropItem::Creator* pCreator = pFieldmap->GetDropItemCreator();

        bool isCreate = false;
        isCreate = pCreator->CreateLotteryDropItem( m_dropItemID, m_dropItemPos, static_cast<FieldItemManager::Type>( m_dropItemType));

        // @fix NMCat[58] 生成成功時のSE再生が抜けていたので追加
        if( isCreate)
        {
          Sound::PlaySE( SEQ_SE_FLD_DROPITEM);
        }
      }

      //負けた、もしくは、スクリプト未設定の場合はフェードインへ
      if( m_scriptID == FieldScript::SCRID_NULL ||
          m_wildBtlResult == BTL_RESULT_WILD_LOSE )
      {
        this->AddSeqNo();
        break;
      }
      //自分or相手が逃げた場合は、スクリプト起動条件がシンボルエンカウントかどうかで異なる
      if( !m_isSymbolEncount ){
        //非シンボルエンカウントで逃げの場合はスクリプトを起動しない
        if( m_wildBtlResult == BTL_RESULT_WILD_RUN || m_wildBtlResult == BTL_RESULT_WILD_RUN_ENEMY ){
          this->AddSeqNo();
          break;       
        }
      }
      // スクリプトが設定されていたらフェードインスキップ
      this->SetSeqNo( SEQ_CALL_SCRIPT);
    }
    break;

  case SEQ_FADE_IN:
    {
      // フェードリクエスト
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );

      this->AddSeqNo();
    }
    break;

  case SEQ_FADE_IN_WAIT:
    {
      // フェードを待つ
      if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->IsEnd( gfl2::Fade::DISP_DOUBLE))
      {
        this->SetSeqNo( SEQ_END);
      }
    }
    break;

  case SEQ_CALL_SCRIPT:
    {
      SAITA_PRINT( "Call Script\n");
      EventScriptCall::CallScript( p_gman, m_scriptID, NULL );
      this->AddSeqNo();
    }
    break;

  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
    //break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

/**
* @brief   呼び出される関数 純粋仮想関数
* @param   p_gman       ゲームマネージャークラス
*/
void EventGimmickEncount::EndFunc(GameSys::GameManager* p_gman)
{
#ifdef DEBUG_ONLY_FOR_saita_kazuki
  gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE)->Dump();
#endif

  SAITA_PRINT( "End EventGimmickEncount\n");
}

/**
 * @brief   セットアップ
 * @param   desc このイベントに必要な外部情報
 */
void EventGimmickEncount::Setup( const Description& desc)
{
  m_desc = desc;
}

/**
 * @brief   待機型の初期化
 * @param   pLocalWork ギミックエンカウントローカルワーク
 */
void EventGimmickEncount::InitFunc_Standby( GimmickEncount::LocalWork* pLocalWork, GameSys::GameManager* p_gman)
{
  GimmickEncount::StandbyData* pData = reinterpret_cast<GimmickEncount::StandbyData*>( pLocalWork->GetTypesData());

  m_scriptID = pData->scriptID;
  m_jumpSE = pData->jumpSEID;
  m_jumpMoveSE = pData->jumpMoveSEID;
  m_isJumpEvent = true;

  u32 modelID = pLocalWork->GetActor()->GetGimmickEncountData()->modelID;
  switch( modelID)
  {
  case STATIC_MODEL_ID_COM_BM_GE_TREE1:
    m_pokeMoveType = POKE_MOVE_TYPE_COME;
    m_jumpEffectType = Effect::EffectGimmickEncount::EFFECT_TYPE_SCATTER_GRASS_TREE_0;
    m_easingFunc = gfl2::math::Easing::LINEAR;
    m_dropItemPos = p_gman->GetFieldmap()->GetPlayerCharacter()->GetCharaDrawInstance()->GetPosition();
    // 大森さん承認：待機型木は「？」にする
    // 谷くん要望：吹き出しアイコンを優先順位高い版に変更
    m_exclamationEffID = Effect::EFFECT_TYPE_QUESTION_ICON_OVERLAY;
    break;
  case STATIC_MODEL_ID_COM_BM_GE_PLANT1:
    m_pokeMoveType = POKE_MOVE_TYPE_COME;
    m_jumpEffectType = Effect::EffectGimmickEncount::EFFECT_TYPE_SCATTER_GRASS_PLANT_0;
    m_easingFunc = gfl2::math::Easing::LINEAR;
    m_dropItemPos = p_gman->GetFieldmap()->GetPlayerCharacter()->GetCharaDrawInstance()->GetPosition();
    // 大森さん承認：待機型植物は「？」にする
    // 谷くん要望：吹き出しアイコンを優先順位高い版に変更
    m_exclamationEffID = Effect::EFFECT_TYPE_QUESTION_ICON_OVERLAY;
    break;
  case STATIC_MODEL_ID_COM_BM_GE_SHADOW1:
    m_pokeMoveType = POKE_MOVE_TYPE_FALL;
    m_jumpEffectType = Effect::EffectGimmickEncount::EFFECT_TYPE_NONE;
    m_easingFunc = gfl2::math::Easing::LINEAR;
    m_dropItemPos = m_symbolPos;
    break;
  default:
    GFL_ASSERT( 0);
    m_pokeMoveType = POKE_MOVE_TYPE_COME;
    m_jumpEffectType = Effect::EffectGimmickEncount::EFFECT_TYPE_SCATTER_GRASS_TREE_0;
    m_easingFunc = gfl2::math::Easing::LINEAR;
    m_dropItemPos = p_gman->GetFieldmap()->GetPlayerCharacter()->GetCharaDrawInstance()->GetPosition();
    break;
  }

  m_pMoveModelForIcon = p_gman->GetFieldmap()->GetPlayerCharacter();
}

/**
 * @brief   接近型の初期化
 * @param   pLocalWork ギミックエンカウントローカルワーク
 */
void EventGimmickEncount::InitFunc_Chase( GimmickEncount::LocalWork* pLocalWork, GameSys::GameManager* p_gman)
{
  GimmickEncount::ChaseData* pData = reinterpret_cast<GimmickEncount::ChaseData*>( pLocalWork->GetTypesData());

  m_scriptID = pData->scriptID;
  m_jumpSE = SMID_NULL;
  m_jumpMoveSE = pData->jumpMoveSEID;
  m_isJumpEvent = false;

  m_pokeMoveType = POKE_MOVE_TYPE_NONE;
  m_jumpEffectType = Effect::EffectGimmickEncount::EFFECT_TYPE_NONE;

  m_pMoveModelForIcon = p_gman->GetFieldmap()->GetPlayerCharacter();

  m_easingFunc = gfl2::math::Easing::LINEAR;

  m_dropItemPos = m_symbolPos;
}

/**
 * @brief   巡回型の初期化
 * @param   pLocalWork ギミックエンカウントローカルワーク
 */
void EventGimmickEncount::InitFunc_Patrol( GimmickEncount::LocalWork* pLocalWork, GameSys::GameManager* p_gman)
{
  GimmickEncount::PatrolData* pData = reinterpret_cast<GimmickEncount::PatrolData*>( pLocalWork->GetTypesData());

  m_scriptID = pData->scriptID;
  m_jumpSE = SMID_NULL;
  m_jumpMoveSE = pData->jumpMoveSEID;
  m_isJumpEvent = GFL_BOOL_CAST( pData->isJumpMove);

  if( m_isJumpEvent)
  {
    m_pokeMoveType = POKE_MOVE_TYPE_FALL;
    m_jumpEffectType = Effect::EffectGimmickEncount::EFFECT_TYPE_NONE;
    m_easingFunc = gfl2::math::Easing::INOUT_SINE;
  }
  else
  {
    m_pokeMoveType = POKE_MOVE_TYPE_NONE;
    m_jumpEffectType = Effect::EffectGimmickEncount::EFFECT_TYPE_NONE;
    m_easingFunc = gfl2::math::Easing::LINEAR;
  }

  m_pMoveModelForIcon = p_gman->GetFieldmap()->GetPlayerCharacter();

  m_dropItemPos = m_symbolPos;
}

/**
 * @brief   逃走型の初期化
 * @param   pLocalWork ギミックエンカウントローカルワーク
 */
void EventGimmickEncount::InitFunc_Escape( GimmickEncount::LocalWork* pLocalWork, GameSys::GameManager* p_gman)
{
  GimmickEncount::EscapeData* pData = reinterpret_cast<GimmickEncount::EscapeData*>( pLocalWork->GetTypesData());

  m_scriptID = pData->scriptID;
  m_jumpSE = SMID_NULL;
  m_jumpMoveSE = pData->jumpMoveSEID;
  m_isJumpEvent = false;
  m_pokeMoveType = POKE_MOVE_TYPE_FALL;
  m_jumpEffectType = Effect::EffectGimmickEncount::EFFECT_TYPE_NONE;

  // 逃走型はシンボルにアイコンを出す
  MoveModel::FieldMoveModelManager* pMoveModelManager = p_gman->GetFieldmap()->GetMoveModelManager();
  u32 modelIndex = pMoveModelManager->GetFieldMoveModelIndexFromEventId( m_eventID);
  m_pMoveModelForIcon = pMoveModelManager->GetFieldMoveModel( modelIndex);

  m_easingFunc = gfl2::math::Easing::LINEAR;

  m_dropItemPos = m_symbolPos;
}

/**
 * @brief イベントに必要なデータの初期化
 */
void EventGimmickEncount::initializeEventData( GameSys::GameManager* p_gman)
{
  // 必要なギミックエンカウントデータを取得しておく
  const FieldGimmickEncountAccessor::Data* pData = m_desc.pActor->GetGimmickEncountData();
  m_presetID        = pData->presetID;
  m_flagwork        = pData->flagwork;
  m_eventID         = pData->eventID;


  // エンカウントデータIDとエンカウント種類の決定
  GimmickEncount::DataAccessor gimmickEncountDataAccessor;
  gimmickEncountDataAccessor.SetData( p_gman->GetFieldResident()->GetResource( BL_IDX_FIELD_RESIDENT__GIMMICK_ENCOUNT_PACK));
  u32 symbolEncountIDOffset = gimmickEncountDataAccessor.GetSymbolEncountIDOffset();
  if( pData->encountDataID < symbolEncountIDOffset)
  {
    // 通常のエンカウントIDなのでそのまま
    m_encountDataID = pData->encountDataID;
    m_isSymbolEncount = false;
  }
  else
  {
    // シンボルエンカウントIDなのでオフセット分を引く
    m_encountDataID = pData->encountDataID - symbolEncountIDOffset;
    m_isSymbolEncount = true;
  }


  // シンボル位置を取得
  m_symbolPos = m_desc.pActor->GetLocalWork()->GetNowPosition();

  // 復帰時に設定するため、現在のロケーションを保持しておく
  m_playerLocation.worldId    = p_gman->GetFieldmap()->GetWorldID();
  m_playerLocation.zoneId     = p_gman->GetFieldmap()->GetZoneID();
  m_playerLocation.position   = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().translate;
  m_playerLocation.quaternion = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().rotation;
}

/**
 * @brief ポケモン関連データ初期化
 */
void EventGimmickEncount::initializePokeData( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* pHeap)
{
  if( m_isSymbolEncount)
  {
    // シンボルエンカウントはシンボルエンカウントデータを取得してPokeSet経由で変換する
    Encount::SymbolEncountParam symbolEncountParam;
    Encount::EncountUtil::SyncLoadSymbolEncountParam( pHeap, p_gman->GetAsyncFileManager(), m_encountDataID, &symbolEncountParam);

    Encount::PokeSet& poke_set = p_gman->GetGameData()->GetEncountWork()->GetPokeSet();
    poke_set.SetSymbolEncountData( &symbolEncountParam);
    m_pEncPokeParam = poke_set.GetTopMonsEncountParam();
  }
  else
  {
    // 通常エンカウントはポケモンの抽選を行っておく
    m_pEncPokeParam = m_desc.pEncount->LotteryEncountPoke( p_gman, m_encountDataID, false, BATTLE_EFFECT_WILD_GIMMICK);
    GFL_ASSERT_STOP( m_pEncPokeParam );
  }

  // ポケモンデータ取得
  GimmickEncount::DataAccessor gimmickEncountDataAccessor;
  gimmickEncountDataAccessor.SetData( p_gman->GetFieldResident()->GetResource( BL_IDX_FIELD_RESIDENT__GIMMICK_ENCOUNT_PACK));
  m_pPokeData = gimmickEncountDataAccessor.GetPokeData( m_pEncPokeParam->monsNo);

  // デバッグ効率向上のためデバッグ時はギミックエンカウントのポケモンテーブルとエンカウントテーブルに差異があっても進める
  if( m_pPokeData == NULL)
  {
    GFL_ASSERT( 0); // 開発中に気付くべき不具合。ギミックエンカウントのポケモンテーブルとエンカウントテーブルに差異がある
    for( u32 i = MONSNO_HUSIGIDANE; i < MONSNO_MAX; ++i)
    {
      // ギミックエンカウントのポケモンテーブルの中にいる一番Noが若いポケモンを出す
      m_pPokeData = gimmickEncountDataAccessor.GetPokeData( i);
      if( m_pPokeData != NULL)
      {
        break;
      }
    }
  }

  // ドロップアイテムID保存
  m_dropItemID = m_pPokeData->dropItemID;
}

/**
 * @brief 各型ごとのデータ初期化
 */
void EventGimmickEncount::initializeTypesData( GameSys::GameManager* p_gman)
{
  GimmickEncount::LocalWork* pLocalWork = m_desc.pActor->GetLocalWork();
  switch( pLocalWork->GetGimmickType())
  {
  case GimmickEncount::GIMMICK_TYPE_STANDBY:
    this->InitFunc_Standby( pLocalWork, p_gman);
    break;
  case GimmickEncount::GIMMICK_TYPE_CHASE:
    this->InitFunc_Chase( pLocalWork, p_gman);
    break;
  case GimmickEncount::GIMMICK_TYPE_PATROL:
    this->InitFunc_Patrol( pLocalWork, p_gman);
    break;
  case GimmickEncount::GIMMICK_TYPE_ESCAPE:
    this->InitFunc_Escape( pLocalWork, p_gman);
    break;
  default:
    GFL_ASSERT( 0);
    break;
  }

  m_dropItemType = this->GetDropItemType( m_desc.pActor->GetGimmickEncountData()->modelID, pLocalWork->GetGimmickType());
}

/**
 * @brief シンボル破棄事前処理
 */
void EventGimmickEncount::preTerminateSymbol( GameSys::GameManager* p_gman)
{
  // ギミックエンカウントUnRegist
  p_gman->GetFieldResident()->GetPlacementDataManager()->UnRegistZoneEventIDGimmickEncount( m_eventID);
}

/**
 * @brief シンボル破棄
 */
void EventGimmickEncount::terminateSymbol( GameSys::GameManager* p_gman)
{
  // ギミックエンカウント削除
  p_gman->GetFieldResident()->GetPlacementDataManager()->DeleteZoneEventIDGimmickEncount( m_eventID);
}

/**
 * @brief 更新モジュール初期化
 */
void EventGimmickEncount::initializeUpdater( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pTemporaryHeap)
{
  if( m_isJumpEvent)
  {
    m_pUpdater = GFL_NEW( pHeap) EventGimmickEncountJumpMoveEffectUpdate();

    // 演出有りのときは、戦闘導入演出スキップ
    m_battleCallOption = EVBTL_CALLOPT_SKIP_ENCEFF;
  }
  else
  {
    m_pUpdater = GFL_NEW( pHeap) EventGimmickEncountNormalUpdate();

    // 演出なしのときは、戦闘導入演出を外部再生(このイベント内で先に再生しておく)
    m_battleCallOption = EVBTL_CALLOPT_ENCEFF_EXTERNAL;
  }

  IEventGimmickEncountUpdate::Description desc;
  desc.p_gman         = p_gman;
  desc.pEvent         = this;
  desc.pHeap          = pHeap;
  desc.pTemporaryHeap = pTemporaryHeap;
  m_pUpdater->Setup( desc);
}

/**
 * @brief 更新モジュール更新
 */
bool EventGimmickEncount::updateUpdater()
{
  return m_pUpdater->Update();
}

/**
 * @brief 更新モジュールバトルコールまでの更新
 */
bool EventGimmickEncount::updateUpdaterBattleCall()
{
  return m_pUpdater->UpdateBattleCall();
}

/**
 * @brief 更新モジュール破棄
 */
void EventGimmickEncount::terminateUpdater()
{
  GFL_SAFE_DELETE( m_pUpdater);
}

/**
 * @brief 「！」アイコン初期化
 */
void EventGimmickEncount::initializeIcon( Effect::EffectManager* pEffectManager, MoveModel::FieldMoveModel* pMoveModel)
{
  // 大森さん承認：ギミックエンカウントの種類によってエフェクトIDを変えました
  m_pExclamationIcon =  pEffectManager->CreateEffect(
    m_exclamationEffID,
    pMoveModel,
    true
    );
}

/**
 * @brief 「！」アイコン更新
 */
bool EventGimmickEncount::updateIcon()
{
  if( m_pExclamationIcon)
  {
    return m_pExclamationIcon->IsAnimationLastFrame();
  }
  return true;
}

/**
 * @brief 「！」アイコン破棄
 */
void EventGimmickEncount::terminateIcon( Effect::EffectManager* pEffectManager)
{
  if( m_pExclamationIcon)
  {
    pEffectManager->DeleteEffect( m_pExclamationIcon);
    m_pExclamationIcon = NULL;
  }
}

/**
 * @brief ドロップアイテム種類の取得
 */
u32 EventGimmickEncount::GetDropItemType( u32 modelID, Field::GimmickEncount::GimmickType type) const
{
  u32 ret = 0;

  switch( type)
  {
  case GimmickEncount::GIMMICK_TYPE_STANDBY:
    ret = this->GetDropItemType_StaticModel( modelID);
    break;

  case GimmickEncount::GIMMICK_TYPE_CHASE:
  case GimmickEncount::GIMMICK_TYPE_PATROL:
  case GimmickEncount::GIMMICK_TYPE_ESCAPE:
    ret = this->GetDropItemType_MoveModel( modelID);
    break;
  }

  return ret;
}

/**
 * @brief ドロップアイテム種類の取得 配置モデル
 */
u32 EventGimmickEncount::GetDropItemType_StaticModel( u32 modelID) const
{
  return FieldItemManager::TYPE_NORMAL;
}

/**
 * @brief ドロップアイテム種類の取得 動作モデル
 */
u32 EventGimmickEncount::GetDropItemType_MoveModel( u32 modelID) const
{
  switch( modelID)
  {
  case CHARA_MODEL_ID_IT0001_00_GRASS:
    return FieldItemManager::TYPE_GRASS;

  case CHARA_MODEL_ID_IT0005_00_WATER:
    return FieldItemManager::TYPE_SEA;
  }

  return FieldItemManager::TYPE_NORMAL;
}

/**
 * @brief レコード反映
 */
void EventGimmickEncount::RefrectRecode( u32 modelID, Field::GimmickEncount::GimmickType type)
{
  switch( type)
  {
  case GimmickEncount::GIMMICK_TYPE_STANDBY:
    this->RefrectRecode_StaticModel( modelID);
    break;

  case GimmickEncount::GIMMICK_TYPE_CHASE:
  case GimmickEncount::GIMMICK_TYPE_PATROL:
  case GimmickEncount::GIMMICK_TYPE_ESCAPE:
    this->RefrectRecode_MoveModel( modelID);
    break;
  }
}

/**
 * @brief レコード反映 配置モデル
 */
void EventGimmickEncount::RefrectRecode_StaticModel( u32 modelID)
{
  switch( modelID)
  {
  case STATIC_MODEL_ID_COM_BM_GE_TREE1:
    Savedata::IncRecord(Savedata::Record::RECID_KIYURE);
    break;

  case STATIC_MODEL_ID_COM_BM_GE_PLANT1:
    Savedata::IncRecord(Savedata::Record::RECID_PLANT);
    break;

  case STATIC_MODEL_ID_COM_BM_GE_SHADOW1:
    Savedata::IncRecord(Savedata::Record::RECID_SHADOW);
    break;
  }
}

/**
 * @brief レコード反映 動作モデル
 */
void EventGimmickEncount::RefrectRecode_MoveModel( u32 modelID)
{
  switch( modelID)
  {
  case CHARA_MODEL_ID_IT0001_00_GRASS:
    Savedata::IncRecord(Savedata::Record::RECID_WEED);
    break;

  case CHARA_MODEL_ID_IT0002_00_SAND:
    Savedata::IncRecord(Savedata::Record::RECID_SABAKU_ENCOUNT);
    break;

  case CHARA_MODEL_ID_IT0003_00_SOIL:
    Savedata::IncRecord(Savedata::Record::RECID_GROUND);
    break;

  case CHARA_MODEL_ID_IT0004_00_SHADOW:
    Savedata::IncRecord(Savedata::Record::RECID_SHADOW);
    break;

  case CHARA_MODEL_ID_IT0005_00_WATER:
    Savedata::IncRecord(Savedata::Record::RECID_UMI_ENCOUNT);
    break;

  case CHARA_MODEL_ID_PM0866_00:
    Savedata::IncRecord(Savedata::Record::RECID_KOSOKU);
    break;
  }
}

GFL_NAMESPACE_END( Field )
