//======================================================================
/**
 * @file FieldEffectDashSmoke.cpp
 * @date 2015/08/28 16:33:11
 * @author miyachi_soichi
 * @brief ケンタロスのけむり
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectDashSmoke.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
// model
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
// field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectDashSmoke
 *  @brief  ケンタロスのけむり
 */
/**
 *  @brief  コンストラクタ
 */
EffectDashSmoke::EffectDashSmoke( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
, m_pJointNode( NULL )
, m_pEffectHandle( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectDashSmoke::~EffectDashSmoke( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectDashSmoke::Initialize( SetupData &setupData )
{
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pFieldRootNode = setupData.pRootNode;
  m_pRenderingPipeLine = setupData.pPipeLine;

  {
    Field::MoveModel::FieldMoveModel *pMoveModel = setupData.pParentObject->SafeCast<Field::MoveModel::FieldMoveModel>();
    m_pParent = pMoveModel->GetCharaDrawInstance();
  }

  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );
  // モデルを作る
  m_pModel = GFL_NEW( setupData.pHeap ) poke_3d::model::BaseModel();
  void *pModelData = binLinkerAccessor.GetData( 0 );
  createModel( pModelData );
  m_pModel->Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode );
  // モーションを作る
  void *pMotionData = binLinkerAccessor.GetData( 1 );
  createMotion( pMotionData );
  m_pModel->ChangeAnimationByResourceNode( m_pMotionResourceRootNode );
  // 描画登録
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );

  m_modelDefaultConstantColor = m_pModel->GetMaterialColorConstant(0,0);
  m_modelConstantAlpha        = 0.0f;
  m_modelConstantAlphaOffset  = 0.0f;
  m_isAddEdeRenderTarget      = false;
}

/**
 *  @brief  破棄
 */
bool EffectDashSmoke::Terminate( void )
{
  Delete();

  m_pFieldRootNode->RemoveAfterTransChild( m_pModelInstanceNode );
  GFL_SAFE_DELETE( m_pModel );
  m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode );
  GFL_SAFE_DELETE( m_pModelResourceRootNode );
  m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode );
  GFL_SAFE_DELETE( m_pMotionResourceRootNode );
  GFL_SAFE_DELETE( m_pAllocator );
  GFL_SAFE_DELETE( m_pEffectHandle );

  if( m_pEffectSystem && m_pEffectHeap )
  {
    m_pEffectSystem = NULL;
    m_pEffectHeap = NULL;
  }
  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectDashSmoke::Delete( void )
{
  // bugfix MCat[388]
  if(m_isAddEdeRenderTarget)
  {
    m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
    m_isAddEdeRenderTarget     = false;
  }

  // 再生終了しているなら削除
  if( m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
}

/**
 *  @brief  土煙
 */
void EffectDashSmoke::CreateSmoke( const gfl2::math::Vector3 &pos )
{
  // 再生終了しているなら削除
  if( m_pEffectHandle->GetEmitterSet()
  && !m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }

  // 再生可能状態か
  bool isAlive = true;
  if( !m_pEffectHandle->GetEmitterSet() )
  {
    isAlive = false;
  }
  else if( !m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    isAlive = false;
  }

  // 再生できる
  if( !isAlive )
  {
    gfl2::math::Vector3 pos = m_pParent->GetPosition();
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, pos, 0, EFFECT_RESOURCE_DASH_SMOKE );
  }
}

/**
 *  @brief  土煙終了
 */
void EffectDashSmoke::DeleteSmoke( void )
{
  // 再生終了しているなら削除
  if( m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->Fade();
  }
}

/**
 *  @brief  ラッシュ切り替え
 */
void EffectDashSmoke::SetVisibleRush( bool flag )
{
  if     ( !m_isAddEdeRenderTarget &&  flag )
  {
    m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
    m_isAddEdeRenderTarget     = true;
    m_modelConstantAlphaOffset = 0.15f;
  }
  else if(  m_isAddEdeRenderTarget && !flag )
  {
    m_modelConstantAlphaOffset = -0.15f;
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  モデルの生成
 */
void EffectDashSmoke::createModel( void *pModelBinary )
{
  // モデルリソース作成
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( pModelBinary ) );
  m_pModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pModelResourceRootNode );

  // モデルインスタンス作成
  m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pAllocator, m_pModelResourceRootNode );
}

/**
 *  @brief  モーションの生成
 */
void EffectDashSmoke::createMotion( void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode );
}

/**
 *  @brief  更新処理
 */
void EffectDashSmoke::updateNormal( void )
{
  // 座標更新
  updatePosition();
  m_pModel->UpdateAnimation();

  // モデルのαを更新
  m_modelConstantAlpha += m_modelConstantAlphaOffset;

  // モデルを表示を消すタイミング
  if( m_isAddEdeRenderTarget && m_modelConstantAlpha < 0.0f )
  {
    m_isAddEdeRenderTarget     = false;
    m_modelConstantAlpha       = 0.0f;
    m_modelConstantAlphaOffset = 0.0f;
    m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
  }

  // αが最大値を以上になったタイミング
  if( m_modelConstantAlpha > m_modelDefaultConstantColor.w )
  {
    m_modelConstantAlpha       = m_modelDefaultConstantColor.w;
    m_modelConstantAlphaOffset = 0.0f;
  }

  m_pModel->SetMaterialColorConstant(0,0,
                                     m_modelDefaultConstantColor.x,
                                     m_modelDefaultConstantColor.y,
                                     m_modelDefaultConstantColor.z,
                                     m_modelConstantAlpha);
}

/**
 *  @brief  座標更新
 */
void EffectDashSmoke::updatePosition( void )
{
  // モデル
  gfl2::math::Vector3 vPos = m_pParent->GetPosition() + m_offsetPosition;
  m_pModel->SetPosition( vPos );
  gfl2::math::Quaternion quat = m_pParent->GetRotationQuat();
  m_pModel->SetRotationQuat( quat );

  // エミッター
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    // trans
    gfl2::math::Vector3 glPos( m_pParent->GetPosition()  );
    nw::math::VEC3      nwPos( glPos.x, glPos.y, glPos.z );

    // rotate
    f32 x, y, z;
    quat.QuaternionToRadian( &x, &y, &z );
    nw::math::VEC3 nwRot  ( x, y, z );

    // scale
    gfl2::math::VEC3 nwScale(1.0f,1.0f,1.0f);

    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#endif  //プラットフォーム分岐
    m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
  }
}

/**
 *  @brief  座標オフセットの設定
 */
void EffectDashSmoke::setOffsetPosition( gfl2::math::Vector3 offsetPos )
{
  m_offsetPosition = offsetPos;
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
