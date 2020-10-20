//======================================================================
/**
 * @file FieldTrialModel.cpp
 * @date 2015/09/08 18:56:42
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// PokeTool
#include "PokeTool/include/PokeModel.h"
// field
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
// self
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"
#include "Field/FieldRo/include/TrialModel/FieldTrialModelFactory.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

/**
 * @class FieldTrialModel
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
FieldTrialModel::FieldTrialModel( PokeTool::PokeModel *pModel, Field::MyRenderingPipeLine *pPipe )
: m_pModel( pModel )
, m_pRenderingPipeLine( pPipe )
, m_pMotPack( NULL )
, m_pShadowData( NULL )
, m_AnimationPackList()
, m_bShadowVisible( true )
, m_pDrawEnvNode( NULL )
, m_nOffsetIndex( 0 )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
void FieldTrialModel::CreateModel( void )
{
  m_pModel->Create();
  m_pModel->SetPosition( 0.0f, 0.0f, 0.0f );
  m_pModel->SetVisible( false );
  m_pModel->SetAdjustHeight( m_pModel->GetBaseHeight() );

  m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModel->GetModelInstanceNode() );

  // ライト設定
  if( m_pDrawEnvNode )
  {
    m_pModel->SetLightSetNo( TrialModelFactory::LIGHT_SET_NO + m_nOffsetIndex );
    u32 cnt = m_pModel->GetMaterialCount();
    for( u32 i = 0; i < cnt; ++ i )
    {
      m_pModel->SetMaterialLightSetIndex( i, TrialModelFactory::LIGHT_SET_NO + m_nOffsetIndex );
    }
    for( u32 i = 0; i < gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax; ++ i )
    {
      gfl2::renderingengine::scenegraph::instance::LightSetNode *lightSet = m_pModel->GetDrawEnvNode()->GetLightSet( i );
      if( lightSet )
      {
        m_pDrawEnvNode->AddLightSet( lightSet );
      }
    }
  }
}

/**
 *  @brief  モデル破棄
 */
void FieldTrialModel::DeleteModel( void )
{
  if( m_pDrawEnvNode )
  {
    for( u32 i = 0; i < gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax; ++ i )
    {
      gfl2::renderingengine::scenegraph::instance::LightSetNode *lightSet = m_pModel->GetDrawEnvNode()->GetLightSet( i );
      if( lightSet )
      {
        m_pDrawEnvNode->RemoveLightSet( lightSet );
        m_pModel->GetDrawEnvNode()->AddLightSet( lightSet );
      }
    }
  }
}

bool FieldTrialModel::IsReady( void )
{
  if( !m_pModel->IsFinishLoad() ) { return false; }

  return true;
}
bool FieldTrialModel::IsEnable( void )
{
  if( !m_pModel->IsEnable() ) { return false; }
  return true;
}

/**
 *  @brief  表示設定
 */
bool FieldTrialModel::IsDisp( void ) const
{
  return GFL_BOOL_CAST( m_pModel->IsVisible() );
}

/**
 *  @brief  表示設定
 */
void FieldTrialModel::SetDisp( bool flag )
{
  m_pModel->SetVisible( flag );
}

/**
 *  @brief  モーションLoad
 */
void FieldTrialModel::LoadMotion( gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase *pHeap, void *pMotPack )
{
  m_AnimationPackList.Initialize( pHeap, 1 );
  m_AnimationPackList.LoadData( 0, pGfxAllocator, pHeap, (c8*)pMotPack );
}

/**
 *  @brief  モーションUnLoad
 */
void FieldTrialModel::UnLoadMotion( void )
{
  m_AnimationPackList.UnloadAll();
  m_AnimationPackList.Finalize();
}

void FieldTrialModel::ChangeAnimation( u32 idx )
{
  gfl2::renderingengine::scenegraph::resource::ResourceNode *pResNode = m_AnimationPackList.GetResourceNode( 0, idx );
  m_pModel->ChangeAnimationResource( pResNode );
}

/**
 *  @brief  モーション待ち
 */
bool FieldTrialModel::IsAnimationLastFrame( void )
{
  return GFL_BOOL_CAST( m_pModel->IsAnimationLastFrame() );
}

/**
 *  @brief  座標の取得
 */
gfl2::math::Vector3 FieldTrialModel::GetPosition( void )
{
#if 0
  gfl2::math::Matrix34 mtx;
  gfl2::math::Vector3 pos;

  mtx =  m_pModel->GetJointWorldMatrix(m_pModel->GetJointIndex( "Origin" ) );
  pos = mtx.GetElemPosition();

  GFL_PRINT("#BOSS Origin %f,%f,%f\n", pos.x, pos.y, pos.z );

  mtx =  m_pModel->GetJointWorldMatrix(m_pModel->GetJointIndex( "Waist" ) );
  pos = mtx.GetElemPosition();

  GFL_PRINT("#BOSS Waist %f,%f,%f\n", pos.x, pos.y, pos.z );
#endif
  return m_pModel->GetPosition();
}

/**
 *  @brief  影タイプ取得
 */
u32 FieldTrialModel::GetShadowType( void )
{
  if( m_pShadowData )
  {
    return m_pShadowData->type;
  }

  // エラー対応
  return SHADOW_MAXI;
}

/**
 *  @brief  影サイズ取得
 */
f32 FieldTrialModel::GetShadowScale( void )
{
  if( m_pShadowData )
  {
    return m_pShadowData->scale;
  }
  // エラー対応
  return 1.0f;
}

/**
 *  @brief  影オフセットの取得
 */
gfl2::math::Vector3 FieldTrialModel::GetShadowOffset( void )
{
  gfl2::math::Vector3 offset( 0.0f, 0.0f, 0.0f );
  if( m_pShadowData )
  {
    offset.x = m_pShadowData->offset_x;
    offset.z = m_pShadowData->offset_z;
  }

  return offset;
}

/**
 *  @brief  環境高設定をアクティブ化
 */
void FieldTrialModel::SetEnableAmbientColor( bool enable )
{
  if( m_pModel )
  {
    m_pModel->SetEnabledAmbientColor( enable );
  }
}
/**
 *  @brief  環境光の設定の有効/無効の取得
 */
bool FieldTrialModel::IsEnableAmbientColor( void )
{
  if( m_pModel )
  {
    return m_pModel->IsEnabledAmbientColor();
  }
  return false;
}
/**
 *  @brief  色の設定
 */
void FieldTrialModel::SetAmbientColor( const gfl2::math::Vector4 &color )
{
  if( m_pModel )
  {
    m_pModel->SetAmbientColor( color );
  }
}

/**
 *  @brief  FieldAdjustHeightを適応する
 */
void FieldTrialModel::SetFieldAdjustHeight( void )
{
  // FieldAdjustHeightを設定
  m_pModel->SetAdjustHeight( m_pModel->GetSettingData()->fieldAdjustHeight * 0.01f );
}

// 3DObject派生用
void FieldTrialModel::SetLocalSRT( const gfl2::math::SRT &srt )
{
  m_pModel->GetModelInstanceNode()->SetLocalSRT( srt );
}
const gfl2::math::SRT &FieldTrialModel::GetLocalSRT( void ) const
{
  return m_pModel->GetModelInstanceNode()->GetLocalSRT();
}
u32 FieldTrialModel::GetModelInstanceNum( void ) const
{
  return 1;
}
const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *FieldTrialModel::GetModelInstance( int index /*= 0*/) const
{
  return m_pModel->GetModelInstanceNode();
}
void FieldTrialModel::ExecuteTerminateRequest( void )
{
}
bool FieldTrialModel::IsCanTerminate( void )
{
  return false;
}

#if PM_DEBUG
/**
 *  @brief  デバッグ用：影サイズ調整
 */
void FieldTrialModel::DebugFunc_SetShadowScale( f32 scale )
{
  if( m_pShadowData )
  {
    m_pShadowData->scale = scale;
  }
}
void FieldTrialModel::DebugFunc_SetShadowOffset( f32 x, f32 z )
{
  if( m_pShadowData )
  {
    m_pShadowData->offset_x = x;
    m_pShadowData->offset_z = z;
  }
}
#endif
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)
