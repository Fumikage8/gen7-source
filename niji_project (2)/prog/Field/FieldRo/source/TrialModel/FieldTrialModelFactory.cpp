//======================================================================
/**
 * @file FieldTrialModelFactory.cpp
 * @date 2015/09/08 14:16:18
 * @author miyachi_soichi
 * @brief TrialModel生成
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gfl2
#include <debug/include/gfl2_Assert.h>
// GameSys
#include "GameSys/include/GameManager.h"
// System
#include "System/include/Skybox/Skybox.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/FieldEnvManager.h"
// camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
// effect
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectPokeShadow.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectShadow.h"
// self
#include "Field/FieldRo/include/TrialModel/FieldTrialModelFactory.h"
// model
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"
#include "Field/FieldRo/include/TrialModel/FieldFinderModel.h"
// debug
#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

inline void CalcRad( f32 &x, f32 &z, const f32 rad )
{
  f32 sin,cos;
  const f32 wx = x;
  const f32 wz = z;
  gfl2::math::SinCosRad( &sin, &cos, rad );
  x = wx*cos - wz*sin;
  z = wx*sin + wz*cos;
}

/**
 * @class TrialModelFactory
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
TrialModelFactory::TrialModelFactory( void )
: m_pPokeModelSystem( NULL )
, m_pFieldmap( NULL )
, m_pRenderingPipeLine( NULL )
, m_pEffectManager( NULL )
, m_pCollisionScene( NULL )
, m_pTrialShadowData( NULL )
, m_pFinderShadowData( NULL )
, m_pAllocator( NULL )
, m_pCameraManager( NULL )
, m_pDrawEnvNode( NULL )
, m_nPokeModelNum( 0 )
, m_ppModelArray( NULL )
, m_IsCreatePokeHeap( false )
, m_IsOutside( true )
, m_IsOverWriteAmbientColor( false )                   //!< 環境光設定をデフォルトから書き換えるか
, m_OverWriteAmbientColor( 0.0f, 0.0f, 0.0f, 0.0f )      //!< 環境光上書き設定
{
  m_fValDegPoke = 60.0f;
  m_fHolDegPoke = 0.0f;
  m_fTrial_x = 0.0f;
  m_fTrial_y = 0.0f;
  m_fTrial_z = 0.0f;
  m_DirectionalUpdate = false;
  m_IsFinderMode = false;

  for( u32 i = 0; i < SHADOW_MAXI; ++ i )
  {
    m_pEffectShadow[i] = NULL;
  }
}

/**
 *  @brief  デストラクタ
 */
TrialModelFactory::~TrialModelFactory( void )
{
  GFL_ASSERT_MSG( !m_pPokeModelSystem, "TrialModelFactory PokeModelSystem is Alive !!\n" );
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  setup
 */
void TrialModelFactory::Setup( const SetupParam &prm )
{
  m_pFieldmap           = prm.pFieldmap;
  m_pRenderingPipeLine  = prm.pPipeLine;
  m_pEffectManager      = prm.pEffectManager;
  m_pCollisionScene     = prm.pColScene;
  m_pCameraManager      = prm.pCameraManager;
  if( prm.pTrialShadow )
  {
    m_pTrialShadowData = static_cast<ShadowDataHeader*>( prm.pTrialShadow );
  }
  if( prm.pFinderShadow )
  {
    m_pFinderShadowData = static_cast<ShadowDataHeader*>( prm.pFinderShadow );
  }
}

/**
 *  @brief  初期化
 */
void TrialModelFactory::InitializeSystem( gfl2::heap::HeapBase *pSysHeap, gfl2::heap::HeapBase *pDevHeap, s32 poke_num, bool create_shadow )
{
  m_pDrawEnvNode = m_pFieldmap->GetFieldEnvManager()->GetDrawEnvNode();
  // 管理配列作成
  m_nPokeModelNum = poke_num;
  m_ppModelArray = GFL_NEW( pSysHeap ) FieldTrialModel* [poke_num];
  m_pAllocator = GFL_NEW( pSysHeap ) System::nijiAllocator( pSysHeap );

  m_pPokeModelSystem = GFL_NEW( pDevHeap ) PokeTool::PokeModelSystem();
  m_pPokeModelSystem->InitSystemAsync( 
    pSysHeap, pDevHeap,
    GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(),
    poke_num
    );
  // 影を生成
  if( create_shadow )
  {
    m_pEffectShadow[SHADOW_CIRCLE] = static_cast<Effect::EffectShadow*>( m_pEffectManager->CreateEffect( Effect::EFFECT_TYPE_SHADOW_CIRCLE, NULL ) );
    m_pEffectShadow[SHADOW_LONG  ] = static_cast<Effect::EffectShadow*>( m_pEffectManager->CreateEffect( Effect::EFFECT_TYPE_SHADOW_DEPTH , NULL ) );
    m_pEffectShadow[SHADOW_WIDE  ] = static_cast<Effect::EffectShadow*>( m_pEffectManager->CreateEffect( Effect::EFFECT_TYPE_SHADOW_WIDTH , NULL ) );
  }
  System::Skybox::Skybox *pSkyBox = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetSkybox();
  if( pSkyBox )
  {
    // 屋内外判定
    m_IsOutside = pSkyBox->IsEnable();
  }
}

/**
 *  @brief  初期化待ち
 */
bool TrialModelFactory::IsInitialize( void )
{
  if( !m_pPokeModelSystem ){ return false; }

  return m_pPokeModelSystem->IsFinishInitSystemAsync();
}

/**
 *  @brief  作成
 */
void TrialModelFactory::CreateSystem( gfl2::heap::HeapBase *pDevHeap, gfl2::gfx::IGLAllocator *pAllocator, PokeTool::PokeModelSystem::HeapOption* pHeapOption, bool create_heap_array )
{
  PokeTool::PokeModelSystem::HeapOption opt;
  if( pHeapOption)
  {
    // 指定
    opt.animeType = pHeapOption->animeType;
    opt.useIdModel = pHeapOption->useIdModel;
    opt.useShadow = pHeapOption->useShadow;
    opt.useColorShader = pHeapOption->useColorShader;
  }
  else
  {
    // デフォルト
    opt.animeType = PokeTool::MODEL_ANIMETYPE_BATTLE;
    opt.useIdModel      = false;     // タッチしない
    opt.useShadow       = false;     // 影は別でつける
    opt.useColorShader  = false;     // カラーシェーダー使用しない
  }

  m_pPokeModelSystem->CreateSystem( opt.useColorShader, pAllocator );
  if( create_heap_array )
  {
    m_pPokeModelSystem->CreatePokeModelHeap( pDevHeap, &opt );
    m_IsCreatePokeHeap = true;
  }
  // 法線マップOn
  if( m_pRenderingPipeLine )
  {
    m_pRenderingPipeLine->SetEdgeNormalMapEnable( true );
  }

  m_DirectionalUpdate = true;
}

/**
 *  @brief  破棄
 */
void TrialModelFactory::DeleteSystem()
{
  // 法線マップOff
  if( m_pRenderingPipeLine )
  {
    m_pRenderingPipeLine->SetEdgeNormalMapEnable( false );
  }

  if( m_IsCreatePokeHeap )
  {
    m_pPokeModelSystem->DeletePokeModelHeap();
  }
  m_pPokeModelSystem->TermSystemAsync();
  for( u32 i = 0; i < SHADOW_MAXI; ++ i )
  {
    if( !m_pEffectShadow[i] ){ continue; }

    m_pEffectManager->DeleteEffect( m_pEffectShadow[i] );
    m_pEffectShadow[i] = NULL;
  }
}

/**
 *  @brief  破棄待ち
 */
bool TrialModelFactory::IsDelete()
{
  if( !m_pPokeModelSystem ){ return true; }

  return m_pPokeModelSystem->IsFinishTermSystemAsync();
}

/**
 *  @brief  破棄
 */
void TrialModelFactory::FinalizeSystem()
{
  GFL_SAFE_DELETE( m_pPokeModelSystem );
  GFL_SAFE_DELETE_ARRAY( m_ppModelArray );
  GFL_SAFE_DELETE( m_pAllocator );
}

/**
 *  @brief  更新
 */
void TrialModelFactory::Update( void )
{
  if( m_pPokeModelSystem )
  {
    // アニメーション更新前のやっておくこと
    UpdateBeforeAnimation();
    // アニメーション更新はシステムに任せる
    m_pPokeModelSystem->Update();
    // アニメーション更新後のやっておくこと
    UpdateAfterAnimation();
  }
}

/**
 *  @brief  更新
 *  @note   アニメーション更新前に行う
 */
void TrialModelFactory::UpdateBeforeAnimation( void )
{
  if( !m_ppModelArray ){ return; }

  for( u32 i = 0; i < m_nPokeModelNum; ++ i )
  {
    if( !m_ppModelArray[i] ){ continue; }
    if( m_ppModelArray[i]->IsEnable() )
    {
      m_ppModelArray[i]->UpdateBeforeAnimation();
    }
  }
}

/**
 *  @brief  更新
 *  @note   アニメーション更新後に行う
 */
void TrialModelFactory::UpdateAfterAnimation( void )
{
  if( !m_ppModelArray ){ return; }

  for( u32 i = 0; i < m_nPokeModelNum; ++ i )
  {
    if( !m_ppModelArray[i] ){ continue; }
    if( m_ppModelArray[i]->IsEnable() )
    {
      m_ppModelArray[i]->UpdateAfterAnimation();
    }
  }
}

/**
 *  @brief  更新：トラバース後の処理
 */
void TrialModelFactory::UpdateAfterTraverse( void )
{
  if( !m_ppModelArray ){ return; }

  for( u32 i = 0; i < m_nPokeModelNum; ++ i )
  {
    if( !m_ppModelArray[i] ){ continue; }
    if( m_ppModelArray[i]->IsEnable() )
    {
      m_ppModelArray[i]->UpdateAfterTraverse();
    }
  }

  updateShadow();
  updateLight();
}

/**
 *  @brief  TrialModelの生成
 */
FieldTrialModel *TrialModelFactory::CreateTrialModel( gfl2::heap::HeapBase *pHeap, const PokeTool::SimpleParam *param, const PokeTool::PokeModel::SetupOption &opt )
{
  if( !m_pPokeModelSystem ){ return NULL; }
  // 空き枠がない
  s32 idx = getEmptyIndex();
  if( idx < 0 ){ return NULL; }
  // PokeModel作成
  PokeTool::PokeModel *pPokeModel = m_pPokeModelSystem->CreateModelSimple( param, opt );
  FieldTrialModel *pTrialModel = GLNew( m_pAllocator) FieldTrialModel( pPokeModel, m_pRenderingPipeLine );
  pTrialModel->InitLight( m_pDrawEnvNode, idx );
  m_ppModelArray[ idx ] = pTrialModel;
  // 影データ設定
  if( m_pTrialShadowData )
  {
    u32 maxi = m_pTrialShadowData->num;
    for( u32 i = 0; i < maxi; ++ i )
    {
      ShadowData *pData = &(m_pTrialShadowData->data[i]);
      if( param->monsNo != pData->monsNo ){ continue; }
      if( pData->form != 65535 && param->formNo != pData->form ){ continue; }

      pTrialModel->SetShadowData( pData );
      break;
    }
  }

  return pTrialModel;
}

/**
 *  @brief  TrialModelの破棄
 */
void TrialModelFactory::DeleteTrialModel( FieldTrialModel *mdl )
{
  for( u32 i = 0; i < m_nPokeModelNum; ++ i )
  {
    if( m_ppModelArray[i] == mdl )
    {
      m_ppModelArray[i] = NULL;
    }
  }
  // モデル破棄
  mdl->DeleteModel();
  PokeTool::PokeModel *pModel = mdl->GetPokeModel();
  m_pRenderingPipeLine->RemoveEdgeRenderingTarget( pModel->GetModelInstanceNode() );
  mdl->UnLoadMotion();
  m_pPokeModelSystem->DeleteModel( pModel );
  GFL_SAFE_DELETE( mdl );
}

/**
 *  @brief  FinderModelの生成
 */
FinderModel *TrialModelFactory::CreateFinderModel( const FinderModelSetupParam &prm )
{
  if( !m_pPokeModelSystem ){ return NULL; }
  // 空き枠がない
  s32 idx = getEmptyIndex();
  if( idx < 0 ){ return NULL; }
  // PokeModel作成
  PokeTool::PokeModel *pPokeModel = m_pPokeModelSystem->CreateModelSimple( prm.param, *prm.option );
  // FinderModel作成
  FinderModel *pFinderModel = GLNew( m_pAllocator) FinderModel( pPokeModel, m_pRenderingPipeLine, prm.model, idx );
  pFinderModel->InitLight( m_pDrawEnvNode, idx );
  m_ppModelArray[idx] = pFinderModel;
  // 影データ設定
  if( m_pFinderShadowData )
  {
    u32 maxi = m_pFinderShadowData->num;
    for( u32 i = 0; i < maxi; ++ i )
    {
      ShadowData *pData = &(m_pFinderShadowData->data[i]);
      if( prm.param->monsNo != pData->monsNo ){ continue; }
      if( pData->form != 65535 && prm.param->formNo != pData->form ){ continue; }

      pFinderModel->SetShadowData( pData );
      break;
    }
  }

  return pFinderModel;
}

/**
 *  @brief  FinderModelの破棄
 */
void TrialModelFactory::DeleteFinderModel( FinderModel *mdl )
{
  for( u32 i = 0; i < m_nPokeModelNum; ++ i )
  {
    if( m_ppModelArray[i] == mdl )
    {
      m_ppModelArray[i] = NULL;
    }
  }
  // PokeModel破棄
  mdl->DeleteModel();
  PokeTool::PokeModel *pModel = mdl->GetPokeModel();
  m_pRenderingPipeLine->RemoveEdgeRenderingTarget( pModel->GetModelInstanceNode() );
  m_pPokeModelSystem->DeleteModel( pModel );
  GFL_SAFE_DELETE( mdl );
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  影エフェクトの更新
 */
void TrialModelFactory::updateShadow( void )
{
  if( !m_ppModelArray ){ return; }
  if( !m_pEffectShadow[0] || !m_pEffectShadow[1] || !m_pEffectShadow[2] ){ return; }

  // 丸影を初期化
  for( u32 i = 0; i < SHADOW_MAXI; ++ i )
  {
    m_pEffectShadow[i]->setSRT( 0.0f, gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f), gfl2::math::Vector3(0.0f,0.0f,0.0f) );
  }

  u32 shadow_index[SHADOW_MAXI] = {1,1,1}; // 影操作は1から
  // 丸影を更新
  for( u32 i = 0; i < m_nPokeModelNum; ++ i )
  {
    FieldTrialModel *pTrialModel = m_ppModelArray[ i ];
    // モデルが作成されていない
    if( !pTrialModel ){ continue; }
    if( !pTrialModel->IsEnable() ){ continue; }
    // モデルが描画されていない
    if( !pTrialModel->IsDisp() ){ continue; }
    // 影描画設定が無効
    if( !pTrialModel->GetShadowVisible() ){ continue; }

    u32                     shadow_type   = pTrialModel->GetShadowType();
    // 影指定がおかしい
    if( shadow_type >= SHADOW_MAXI ){ continue; }

    PokeTool::PokeModel *pCharaModel = pTrialModel->GetPokeModel();

    const f32               GROUND_OFFSET = 3.0f;
    const u32               JOINT_INDEX   = pCharaModel->GetJointIndex( "Origin" );
    gfl2::math::Vector3     joint_pos     = pCharaModel->GetJointWorldMatrix(JOINT_INDEX).GetElemPosition();
    f32                     ground_height = 0.0f;
    gfl2::math::Vector3     ground_normal = gfl2::math::Vector3( 0.0f, 1.0f, 0.0f );
    bool                    bOnGround     = true;
    // 地面の高さと法線を取得
    if( m_pCollisionScene )
    {
      f32 start_offset = 100.0f;
      f32 end_offset = 10000.0f;
      RaycastCustomCallback::HIT_DATA hitData;
      gfl2::math::Vector4 startVec( joint_pos.x, joint_pos.y+start_offset, joint_pos.z, 0.0f );
      gfl2::math::Vector4 endVec(   joint_pos.x, joint_pos.y-  end_offset, joint_pos.z, 0.0f );
      // レイが地面にあたったか
      if( m_pCollisionScene->RaycastFromStaticActorsMesh( startVec, endVec, &hitData ) )
      {
        ground_height = hitData.intersection.y;
        gfl2::math::Vector4 v0 = hitData.pTriangle->m_Positions[1] - hitData.pTriangle->m_Positions[0];
        gfl2::math::Vector4 v1 = hitData.pTriangle->m_Positions[2] - hitData.pTriangle->m_Positions[0];
        gfl2::math::Vector4 v2 = v0.Cross(v1).Normalize3();
        ground_normal = gfl2::math::Vector3( v2.x,v2.y,v2.z );
        if( ground_normal.y < 0.0f ){ ground_normal *= -1.0f; }
      }
      else
      {
        bOnGround = false;
      }
    }

    gfl2::math::Vector3     up          = gfl2::math::Vector3( 0.0f, 1.0f, 0.0f );
    gfl2::math::Vector3     axis        = up.Cross( ground_normal ).Normalize();
    gfl2::math::Vector3     def         = ground_normal - up;
    f32                     dot         = gfl2::math::Clamp( ground_normal.Dot(up), -1.0f, 1.0f );
    f32                     angle       = acos( dot );
    f32                     cos_angle   = cos( angle * 0.5f );
    f32                     sin_angle   = sin( angle * 0.5f );
    gfl2::math::Quaternion  ground_qua  = gfl2::math::Quaternion( sin_angle * axis.x, sin_angle * axis.y, sin_angle * axis.z, cos_angle );
    ground_qua.Normalize();
    if( dot >= 0.99f )
    {
      ground_qua = gfl2::math::Quaternion( 0.0f, 0.0f, 0.0f, 1.0f );
    }

    gfl2::math::Quaternion parent_qua( 0.0f, 0.0f, 0.0f, 1.0f );
    parent_qua.MatrixToQuaternion( pCharaModel->GetJointWorldMatrix( JOINT_INDEX ) );

    gfl2::math::Vector3     pos           = gfl2::math::Vector3( joint_pos.x, ground_height+GROUND_OFFSET, joint_pos.z );
    gfl2::math::Quaternion  rot           = gfl2::math::Quaternion( 0.0f, 0.0f, 0.0f, 1.0f );
    f32                     scale         = pTrialModel->GetShadowScale();
    gfl2::math::Vector3     offset        = pTrialModel->GetShadowOffset();
    // 空中キャラクタは影をけす
    if( !bOnGround && m_IsFinderMode )
    {
      scale = 0.0f;
    }

    {
      gfl2::math::Matrix34 mtx;
      gfl2::math::Vector3 parent_rot = pCharaModel->GetRotation();
      mtx = gfl2::math::Matrix34::GetRotationZYX( 0.0f, parent_rot.y, 0.0f );
      pos += mtx * offset;
    }

    rot = ground_qua * parent_qua;
    gfl2::math::Quaternion y_qua( 0.0f, 0.0f, 0.0f, 1.0f );
    y_qua.RadianYToQuaternion( PI/2.0f );
    rot = rot * y_qua;

    m_pEffectShadow[shadow_type]->setSRT( shadow_index[shadow_type], scale, rot, pos );
    shadow_index[shadow_type] += 1;

#if PM_DEBUG
    if( Field::Debug::DebugTypes::s_DebugFinderParam.bShadowCheck != 0 )
    {
      // 1単位距離以上影がめり込むなら
      if( pos.y > joint_pos.y + 1.0f )
      {
        GFL_ASSERT_MSG( false, "Shadow Pos Invalid Pos=%f,Org=%f \n", pos.y, joint_pos.y );
        // 連続したアサートを回避
        Field::Debug::DebugTypes::s_DebugFinderParam.bShadowCheck = 0;
      }
    }
#endif
  }
}

/**
 *  @brief  空き枠を取得
 */
s32 TrialModelFactory::getEmptyIndex( void )
{
  s32 idx = -1;
  // 未初期化対応
  if( !m_ppModelArray ){ return idx; }

  for( u32 i = 0; i < m_nPokeModelNum; ++ i )
  {
    if( !m_ppModelArray[i] )
    {
      idx = static_cast<s32>( i );
      break;
    }
  }

  return idx;
}

/**
 *  @brief  ライト更新
 */
void TrialModelFactory::updateLight( void )
{
  if( m_pCameraManager == NULL ){ return; }
  if( !m_IsFinderMode )
  {
    // 試練用更新
    updateLight_TrialMode();
  }
  else
  {
    // ファインダー用更新
    updateLight_FinderMode();
  }

  // 環境光
  gfl2::math::Vector4 color( 1.0f, 1.0f, 1.0f, 1.0f );
  if( m_IsOutside )
  {
    color.Set( 1.0f, 1.0f, 1.0f, 1.0f );
  }
  else
  {
    // 屋内処理
    color.Set( 0.7f, 0.7f, 0.7f, 1.0f );
  }

  // momiji追加、環境光の上書き
  if (m_IsOverWriteAmbientColor)
  {
    color = m_OverWriteAmbientColor;
  }

  if( m_ppModelArray )
  {
    for( u32 i = 0; i < m_nPokeModelNum; ++ i )
    {
      if( !m_ppModelArray[i] ){ continue; }
      if( !m_ppModelArray[i]->IsEnableAmbientColor() ){ continue; }

      m_ppModelArray[i]->SetAmbientColor( color );
    }
  }
}

/**
 *  @brief  ライト更新：試練用
 */
void TrialModelFactory::updateLight_TrialMode( void )
{
  if( !m_DirectionalUpdate ){ return; }

  f32 rotX = m_fTrial_x;
  f32 rotY = m_fTrial_y;
  f32 rotZ = m_fTrial_z;
  gfl2::math::Vector3 dir( rotX, rotY, rotZ );

  for( u32 i = 0; i < m_nPokeModelNum; ++ i )
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode *pLightSetNode = m_pDrawEnvNode->GetLightSet( TrialModelFactory::LIGHT_SET_NO + i );
    if( !pLightSetNode ){ continue; }

    gfl2::renderingengine::scenegraph::instance::LightNode *pLightNode = pLightSetNode->GetLightNode(0);
    if( pLightNode )
    {
      pLightNode->SetDirection( dir );
      m_DirectionalUpdate = false;
    }
  }
}

/**
 *  @brief  ライト更新：ファインダー用
 */
void TrialModelFactory::updateLight_FinderMode( void )
{
  if( !m_DirectionalUpdate ){ return; }

  Field::Camera::CameraUnit *pUnit = m_pCameraManager->GetMainGamePlayCamera();
  gfl2::math::Vector3 cam_target;
  gfl2::math::Vector3 cam_position;
  pUnit->GetCameraAndTargetPosition( &cam_target, &cam_position );
  gfl2::math::Vector3 cam_dir = cam_target - cam_position;

  f32 rotY = atan2( cam_dir.x, cam_dir.z ) + gfl2::math::ConvDegToRad( m_fHolDegPoke );
  f32 rotX = gfl2::math::ConvDegToRad( m_fValDegPoke );

  gfl2::math::Vector3 lightVecPoke( 0.0f, 0.0f, 1.0f );
  gfl2::math::Matrix34 mtx; mtx = gfl2::math::Matrix34::GetRotationZYX( 0.0f, rotY, rotX );
  gfl2::math::Vector3 dir = mtx * lightVecPoke.Normalize();
  for( u32 i = 0; i < m_nPokeModelNum; ++ i )
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode *pLightSetNode = m_pDrawEnvNode->GetLightSet( TrialModelFactory::LIGHT_SET_NO + i );
    if( !pLightSetNode ){ continue; }

    gfl2::renderingengine::scenegraph::instance::LightNode *pLightNode = pLightSetNode->GetLightNode(0);
    if( pLightNode )
    {
      pLightNode->SetDirection( dir );
      m_DirectionalUpdate = false;
    }
  }
}

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)
