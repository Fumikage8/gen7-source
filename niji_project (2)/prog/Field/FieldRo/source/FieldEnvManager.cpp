//===================================================================
/**
 * @file    FieldEnvManager.cpp
 * @brief   フィールドEnv管理クラス
 * @author  saita_kazuki
 * @date    2015.05.12
 */
//===================================================================

#include "Field/FieldRo/include/FieldEnvManager.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

#include "System/include/nijiAllocator.h"
#include "System/include/Skybox/Skybox.h"

#include <fs/include/gfl2_BinLinkerAccessor.h>

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>

#include <util/include/gfl2_std_string.h>
#include <math/include/gfl2_MathCommon.h>

#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

#include "Field/FieldRo/include/Skybox/FieldSkyboxDataManager.h"

GFL_NAMESPACE_BEGIN( Field )

#define SMOOTH_FRAME (10)

/**
 * @brief コンストラクタ
 */
EnvManager::EnvManager()
  :  m_pHeap( NULL)
  ,  m_pGLAllocator( NULL)
  ,  m_EnvAccessor()
  ,  m_pSkybox( NULL)
  ,  m_pSkyboxDataManager( NULL)
  ,  m_pDrawEnvNode( NULL)
  ,  m_pDrawEnvResourceRootNode( NULL)
  ,  m_pDrawEnvResourceAlphaMask( NULL)
  ,  m_reqUpdateState()
  ,  m_bloomStrength()
  ,  m_bloomIntensity()
  ,  m_changeFrameCnt( 0)
  ,  m_changeFrame( 0)
{
}

/**
 * @brief デストラクタ
 */
EnvManager::~EnvManager()
{
}

/**
 * @brief 初期化
 * @param 天球インスタンスのポインタ
 */
void EnvManager::Initialize( gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pGLAllocator, MyRenderingPipeLine* pRenderingPipeLine, void* pEnvPackData, System::Skybox::Skybox* pSkybox, Skybox::DataManager* pSkyboxDataManager)
{
  m_pHeap = pHeap;
  m_pGLAllocator = pGLAllocator;

  m_pSkybox = pSkybox;
#if PM_DEBUG
  FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::VISIBLE_SKYBOX, static_cast<s32>( m_pSkybox->IsEnable()));
#endif // PM_DEBUG

  m_pSkyboxDataManager = pSkyboxDataManager;

  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( pEnvPackData);

  // EnvNode作成
  {
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_ENV));

    gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData resourceData;
    resourceData.SetBinaryEnvData( pBinaryData);

    m_pDrawEnvResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);
    m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode( m_pGLAllocator, m_pDrawEnvResourceRootNode);
  }

  m_EnvAccessor.Initialize( m_pHeap, m_pDrawEnvNode);
#if 0
  // Envモーションノード作成
  if( binLinkerAccessor.GetDataSize(PACK_INDEX_MOTION_LIGHT) != 0 )
  {
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_MOTION_LIGHT));
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData);

    m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_LIGHT] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);

    // Lightアニメーション開始
    m_EnvAccessor.ChangeAnimation( m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_LIGHT], 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
    m_EnvAccessor.SetStepFrame( 0.0f, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
    m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);

  }

  // Envモーションノード作成(FOG)
  if( binLinkerAccessor.GetDataSize(PACK_INDEX_MOTION_LIGHT) != 0 )
  {
    // バイナリデータを取得
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_MOTION_LIGHT) );
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData );

    // リソースノードをバイナリデータから作成
    m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_FOG] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);

    // Fogアニメーション開始
    m_EnvAccessor.ChangeAnimation( m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_FOG  ], 0, System::EnvAccessor::ANIMATION_BIT_FOG  );
    m_EnvAccessor.SetStepFrame( 0.0f, 0, System::EnvAccessor::ANIMATION_BIT_FOG);
    m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_FOG );
  }

  // Envモーションノード作成(ブルーム)
  if( binLinkerAccessor.GetDataSize(PACK_INDEX_MOTION_LIGHT) != 0 )
  {
    // バイナリデータを取得
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_MOTION_LIGHT) );
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData );

    // リソースノードををバイナリデータから作成
    m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_BLOOM] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);

    // ブルームアニメーション開始
    m_EnvAccessor.ChangeAnimation( m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_BLOOM], 0, System::EnvAccessor::ANIMATION_BIT_BLOOM);
    // @fix 1日アニメーションはUpdateで時間から求めたフレームをセットするため、ステップフレームは0.0fに設定
    m_EnvAccessor.SetStepFrame( 0.0f, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM);
    m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM );
  }
#endif

  // ブルーム描画パスの初期化
  {
    poke_3d::renderer::BloomRenderPath* bloomRenderPath = pRenderingPipeLine->GetBloomRenderPath();
    bloomRenderPath->SetDrawEnvNode( m_pDrawEnvNode );

    if( binLinkerAccessor.GetDataSize(PACK_INDEX_TEXTURE_BLOOM) != 0 )
    {
      // リソースノード
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
        texData.SetTextureData(static_cast<c8*>(binLinkerAccessor.GetData( PACK_INDEX_TEXTURE_BLOOM)));
        m_pDrawEnvResourceAlphaMask = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pGLAllocator, &texData);
      }
      // ブルーム描画パスに渡す
      {
        gfl2::renderingengine::scenegraph::resource::TextureResourceNode* texResNode = poke_3d::renderer::BloomRenderPath::GetTextureResourceNode(m_pDrawEnvResourceAlphaMask);
        bloomRenderPath->SetAlphaMaskTexture(texResNode->GetTexture());
      }
    }
  }


  // 外部操作用パラメータ初期化
  f32 nowStrength = m_pDrawEnvNode->GetGlareStrength();
  m_bloomStrength.now = nowStrength;
  m_bloomStrength.goal = nowStrength;
  m_bloomStrength.save = nowStrength;

  f32 nowIntensity = m_pDrawEnvNode->GetGlareIntensity();
  m_bloomIntensity.now = nowIntensity;
  m_bloomIntensity.goal = nowIntensity;
  m_bloomIntensity.save = nowIntensity;
}

/**
 * @brief 画面表示前に行う処理
 * @param マップ管理座標
 * @param hour 時間
 * @param minute 分
 * @param second 秒
 * @param projMatrix プロジェクションマトリクス
 * @param pRenderingPipeLine 描画パイプライン
 */
void EnvManager::Ready( const gfl2::math::Vector3& controlPos, s32 hour, s32 minute, s32 second, gfl2::math::Matrix44& projMatrix, MyRenderingPipeLine* pRenderingPipeLine)
{
  // 天球
  gfl2::math::Vector3 controlPosFix = controlPos;
  if( m_pSkyboxDataManager->IsExistPosData())
  {
    controlPosFix = m_pSkyboxDataManager->ConvertPosition( controlPos);
  }
  m_pSkybox->Update( controlPosFix, hour, minute, second);

  // Envアニメーション
  m_EnvAccessor.SetFrame( hour, minute, second, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
  m_EnvAccessor.SetFrame( hour, minute, second, 0, System::EnvAccessor::ANIMATION_BIT_FOG);
  m_EnvAccessor.SetFrame( hour, minute, second, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM);
  m_EnvAccessor.Update();

  // フォグ
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogOuterParam param;
  param.projMatrix = projMatrix;
  m_pDrawEnvNode->UpdateFog(param);

  // ブルーム描画パス
  poke_3d::renderer::BloomRenderPath* bloomRenderPath = pRenderingPipeLine->GetBloomRenderPath();
  if( bloomRenderPath )
  {
    bloomRenderPath->Update();
  }
}

/**
 * @brief アニメーションのセット
 * @param　アニメーションリソースノードポインタ
 */
void EnvManager::SetAnime(gfl2::renderingengine::scenegraph::resource::ResourceNode* anime_node)
{
  // Lightアニメーション開始
  m_EnvAccessor.ChangeAnimation( anime_node, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT );
  m_EnvAccessor.SetStepFrame( 0.0f, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
  m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
  // Fogアニメーション開始
  m_EnvAccessor.ChangeAnimation( anime_node, 0, System::EnvAccessor::ANIMATION_BIT_FOG );
  m_EnvAccessor.SetStepFrame( 0.0f, 0, System::EnvAccessor::ANIMATION_BIT_FOG);
  m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_FOG );
  // ブルームアニメーション開始
  m_EnvAccessor.ChangeAnimation( anime_node, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM);
  m_EnvAccessor.SetStepFrame( 0.0f, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM);
  m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM );
}

/**
 * @brief アニメーションのセット
 * @param　アニメーションリソースノードポインタ
 */
void EnvManager::SetAnimeSmooth(gfl2::renderingengine::scenegraph::resource::ResourceNode* anime_node)
{
  u32 frame = SMOOTH_FRAME;
  // Lightアニメーション開始
  ///m_EnvAccessor.ChangeAnimation( anime_node, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT );
  m_EnvAccessor.ChangeAnimationSmooth( anime_node, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT, frame );
  m_EnvAccessor.SetStepFrame( 0.0f, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
  m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
  // Fogアニメーション開始
  ///m_EnvAccessor.ChangeAnimation( anime_node, 0, System::EnvAccessor::ANIMATION_BIT_FOG );
  m_EnvAccessor.ChangeAnimationSmooth( anime_node, 0, System::EnvAccessor::ANIMATION_BIT_FOG, frame );
  m_EnvAccessor.SetStepFrame( 0.0f, 0, System::EnvAccessor::ANIMATION_BIT_FOG);
  m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_FOG );
  // ブルームアニメーション開始
  ///m_EnvAccessor.ChangeAnimation( anime_node, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM);
  m_EnvAccessor.ChangeAnimationSmooth( anime_node, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM, frame );
  m_EnvAccessor.SetStepFrame( 0.0f, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM);
  m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM );
}

/**
 * @brief アニメーショの変更が終了したか
 * @return true で終了
 */
bool EnvManager::IsAnimeChangeFinished(void)
{
  if ( m_EnvAccessor.IsAnimationSmooth(System::EnvAccessor::ANIMATION_BIT_LIGHT) )
  {
    return false;
  }
  if ( m_EnvAccessor.IsAnimationSmooth(System::EnvAccessor::ANIMATION_BIT_FOG) )
  {
    return false;
  }
  if ( m_EnvAccessor.IsAnimationSmooth(System::EnvAccessor::ANIMATION_BIT_BLOOM) )
  {
    return false;
  }

  return true;
}

/**
 * @brief 終了
 */
void EnvManager::Terminate()
{
  m_EnvAccessor.Terminate();
#if 0
  for( u32 i = 0; i < ANIMATION_INDEX_MAX; ++i)
  {
    GFL_SAFE_DELETE( m_pDrawEnvMotionResourceRootNode[i]);
  }
#endif
  GFL_SAFE_DELETE( m_pDrawEnvResourceRootNode);
  GFL_SAFE_DELETE( m_pDrawEnvNode );
  GFL_SAFE_DELETE( m_pDrawEnvResourceAlphaMask );
}

/**
 * @brief 更新
 * @param マップ管理座標
 * @param hour 時間
 * @param minute 分
 * @param second 秒
 */
void EnvManager::Update( const gfl2::math::Vector3& controlPos, s32 hour, s32 minute, s32 second, gfl2::math::Matrix44& projMatrix, MyRenderingPipeLine* pRenderingPipeLine)
{
  // 天球座標設定データがあれば合わせる
  gfl2::math::Vector3 controlPosFix = controlPos;

  if( m_pSkyboxDataManager->IsFixPosition() )
  {
    controlPosFix = m_pSkyboxDataManager->GetFixPosition();
  }
  else if( m_pSkyboxDataManager->IsExistPosData())
  {
    controlPosFix = m_pSkyboxDataManager->ConvertPosition( controlPos);
  }

  // 天球更新
#if PM_DEBUG
  bool isDebugSkyboxVisible = static_cast<bool>( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_SKYBOX));
  if( m_pSkybox->IsEnable() != isDebugSkyboxVisible )
  {
    m_pSkybox->SetEnable( isDebugSkyboxVisible);
  }
#endif // PM_DEBUG
  m_pSkybox->Update( controlPosFix, hour, minute, second);

  m_EnvAccessor.SetFrame( hour, minute, second, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
  m_EnvAccessor.SetFrame( hour, minute, second, 0, System::EnvAccessor::ANIMATION_BIT_FOG);
  // @fix ブルームの1日アニメーションに対応
  m_EnvAccessor.SetFrame( hour, minute, second, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM);
  m_EnvAccessor.Update();

  // リクエストを受けたパラメータの更新
  this->UpdateRequest();

  // フォグ
  {
    // 他は変更していないので
    //   ・Zバッファ
    //   ・Zバッファなのでカメラのニアクリップ距離、ファークリップ距離は使わない
    //   ・深度バッファの範囲はデフォルトから変更しない
    // となっている。

    gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogOuterParam param;
    param.projMatrix = projMatrix;
    m_pDrawEnvNode->UpdateFog(param);
  }

  // ブルーム描画パス
  {
    poke_3d::renderer::BloomRenderPath* bloomRenderPath = pRenderingPipeLine->GetBloomRenderPath();
    if( bloomRenderPath )
    {
      bloomRenderPath->Update();
    }
  }
}

/**
 * @brief DrawEnvNode取得
 */
gfl2::renderingengine::scenegraph::instance::DrawEnvNode* EnvManager::GetDrawEnvNode() const
{
  return m_pDrawEnvNode;
}

/**
 * @brief ブルーム(グレア)の輝度を指定の割合に変化させる
 * @param rate 輝度の割合
 * @param frame 変化させるフレーム
 */
void EnvManager::ChangeBloom( f32 rate, s32 frame)
{
  if( gfl2::math::IsSimilar( 1.0f, rate))
  {
    // 戻すとき
    m_bloomStrength.now = m_bloomStrength.goal;
    m_bloomStrength.goal = m_bloomStrength.save;
    m_bloomStrength.save = 0.0f;

    m_bloomIntensity.now = m_bloomIntensity.goal;
    m_bloomIntensity.goal =m_bloomIntensity.save;
    m_bloomIntensity.save = 0.0f;

    m_reqUpdateState.AddState( RequestUpdateState::STATE_BLOOM_STRENGTH_RETURN);
  }
  else
  {
    f32 nowStrength = m_pDrawEnvNode->GetGlareStrength();

    m_bloomStrength.now = nowStrength;
    m_bloomStrength.goal = nowStrength * rate;
    m_bloomStrength.save = nowStrength;

    f32 nowIntensity = m_pDrawEnvNode->GetGlareIntensity();

    m_bloomIntensity.now = nowIntensity;
    m_bloomIntensity.goal = nowIntensity * rate;
    m_bloomIntensity.save = nowIntensity;

    m_reqUpdateState.AddState( RequestUpdateState::STATE_BLOOM_STRENGTH);
  }

  f32 subStrength = m_bloomStrength.goal - m_bloomStrength.now;
  f32 subIntensity = m_bloomIntensity.goal - m_bloomIntensity.now;
  if( frame == 0)
  {
    m_bloomStrength.add = subStrength;
    m_bloomIntensity.add = subIntensity;
  }
  else
  {
    m_bloomStrength.add = subStrength / static_cast<f32>( frame);
    m_bloomIntensity.add = subIntensity / static_cast<f32>( frame);
  }

  m_changeFrameCnt = frame;
  m_changeFrame = frame;
}

/**
 * @brief リクエストを受けたパラメータの更新
 */
void EnvManager::UpdateRequest()
{
  this->UpdateBloom();
}

/**
 * @brief ブルーム(グレア)の輝度の更新
 */
void EnvManager::UpdateBloom()
{
  if( m_reqUpdateState.IsState( RequestUpdateState::STATE_BLOOM_STRENGTH) ||
      m_reqUpdateState.IsState( RequestUpdateState::STATE_BLOOM_STRENGTH_RETURN)
    )
  {
    f32 nextStrength = m_bloomStrength.goal;
    f32 nextIntensity = m_bloomIntensity.goal;
    if( m_changeFrameCnt > 0)
    {
      s32 cnt = (m_changeFrame - m_changeFrameCnt) + 1;

      nextStrength = m_bloomStrength.now + (m_bloomStrength.add * cnt);
      nextIntensity = m_bloomIntensity.now + (m_bloomIntensity.add * cnt);
      SAITA_PRINT( "on  / next = %f\n", nextStrength);

      m_changeFrameCnt--;

      if( m_changeFrameCnt < 0 &&
          m_reqUpdateState.IsState( RequestUpdateState::STATE_BLOOM_STRENGTH_RETURN)
        )
      {
        u32 del = RequestUpdateState::STATE_BLOOM_STRENGTH_RETURN | RequestUpdateState::STATE_BLOOM_STRENGTH;
        m_reqUpdateState.DeleteState( del);
        SAITA_PRINT( "off\n");
      }
    }
    m_pDrawEnvNode->SetGlareStrength( nextStrength);
    m_pDrawEnvNode->SetGlareIntensity( nextIntensity);
  }
}

/**
 * @brief ライトセットのアンビエントカラーから輝度を受け取る
 */
b32 EnvManager::GetLuminanceFromLightSetAmbient( u32 lightSetIndex, f32* pOutLuminance ) const
{
  *pOutLuminance = 0.0f;

  for( u32 i=0 ; i<m_pDrawEnvNode->GetLightSet(lightSetIndex)->GetLightNum() ; i++ )
  {
    gfl2::renderingengine::scenegraph::instance::LightNode* pLightNode = m_pDrawEnvNode->GetLightSet(lightSetIndex)->GetLightNode(i);
    if( pLightNode && pLightNode->GetLightType() == gfl2::renderingengine::LightType::Ambient )
    {
      gfl2::math::Vector4 color    ( m_pDrawEnvNode->GetLightSet(0)->GetLightNode(0)->GetColor() );
      f32                 luminance( 0.298912f * color.x + 0.586611f * color.y + 0.114478f * color.z );
      *pOutLuminance = gfl2::math::Clamp( luminance, 0.0f, 1.0f );
      return true;
    }
  }
  return false;
}

GFL_NAMESPACE_END( Field )

