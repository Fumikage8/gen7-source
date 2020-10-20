//===================================================================
/**
 * @file    FieldEffectManager.cpp
 * @brief   フィールドエフェクトシステム管理
 * @author  saita_kazuki
 * @date    2015.03.12
 */
//===================================================================

#include "test/FieldTest/include/FieldEffectManager.h"
#include "test/FieldTest/include/debug/FieldDebugInstanceMemoryCollection.h"


// デバッグ
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// メモリ
#include <gfx/include/gfl2_GLMemory.h>

// field_util
#include "test/FieldTest/include/FieldTestUtility.h"

// renderengine
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )


// ptclリソースパステーブル
const c16* Manager::PTCL_RESOURCE_PATH_TABLE[ RESOURCE_ID_MAX ] =
{
#if defined( GF_PLATFORM_CTR)
  L"rom:/data/test/field_test/effect/weather/effect_weather.ctr.ptcl",
  L"rom:/data/test/field_test/effect/foot_spray/effect_foot_spray.ctr.ptcl",
#elif defined( GF_PLATFORM_WIN32)
  L"R:/home/niji/exec/data_win32/data/test/field_test/effect/weather/effect_weather.wingl.ptcl",
  L"R:/home/niji/exec/data_win32/data/test/field_test/effect/foot_spray/effect_foot_spray.wingl.ptcl",
#endif
};

// modelリソースパステーブル
const c16* Manager::MODEL_RESOURCE_PATH_TABLE[ RESOURCE_ID_MAX ] =
{
#if defined(GF_PLATFORM_CTR)
    L"rom:/data/test/field_test/effect/weather/we_rain_spray.ctr.gfbmdlp",
    L"rom:/data/test/field_test/effect/foot_spray/water_surface.ctr.gfbmdlp",
#elif defined(GF_PLATFORM_WIN32)
    L"R:/home/niji/exec/data_win32/data/test/field_test/effect/weather/we_rain_spray.wingl.gfbmdlp",
    L"R:/home/niji/exec/data_win32/data/test/field_test/effect/foot_spray/water_surface.wingl.gfbmdlp",
#endif
};

// motionリソースパステーブル
const c16* Manager::MOTION_RESOURCE_PATH_TABLE[ RESOURCE_ID_MAX ] =
{
#if defined(GF_PLATFORM_CTR)
    L"rom:/data/test/field_test/effect/weather/we_rain_spray.gfbmot",
    L"rom:/data/test/field_test/effect/foot_spray/water_surface.gfbmot",
#elif defined(GF_PLATFORM_WIN32)
    L"R:/home/niji/exec/data_win32/data/test/field_test/effect/weather/we_rain_spray.gfbmot",
    L"R:/home/niji/exec/data_win32/data/test/field_test/effect/foot_spray/water_surface.wingl.gfbmot",
#endif
};

/**
 * @brief   コンストラクタ
 */
Manager::Manager() :
  m_pSystem( NULL),
  m_pHeap( NULL),
  m_pGroupIDList( NULL),
  m_frameRate( 1.0f),
  m_seq( 0),
  m_pGLAllocator( NULL),
  m_pResourceBin(),
  m_pModelPackBin(),
  m_pModelResourceRootNode(),
  m_pMotionBin(),
  m_pMotionResourceRootNode()
{
}

/**
 * @brief   デストラクタ
 */
Manager::~Manager()
{
}

/**
 * @brief   初期化
 * @ param  pHeap エフェクトヒープに使用するヒープメモリ
 * @ param  pGLAllocator リソース確保に使用するアロケータ
 */
b8 Manager::Initialize( gfl2::heap::HeapBase* pHeap, gfl2::gfx::IGLAllocator* pGLAllocator)
{
  m_pGLAllocator = pGLAllocator;

  // ヒープ生成
  m_pHeap = GFL_NEW( pHeap) gfl2::Effect::GFHeap( pHeap);

  // 描画グループ作成
  m_pGroupIDList = GFL_NEW( pHeap) gfl2::util::List<u32>( pHeap, GROUP_ID_MAX);
  for( u32 i = 0; i < GROUP_ID_MAX; ++i)
  {
    m_pGroupIDList->PushFront( i);
  }

  // システム生成
  gfl2::Effect::Config config;
  config.SetEffectHeap( m_pHeap);

#if 1
  config.SetEmitterNum( 16);
  config.SetParticleNum( 256);
  config.SetEmitterSetNum( 8);
  config.SetResourceNum( 4);
  config.SetStripeNum( 16);
#endif

  m_pSystem = GFL_NEW( pHeap) gfl2::Effect::System( config);

  return true;
}

/**
 * @brief   破棄
 */
b8 Manager::Terminate()
{
  // @todo EndFuncがシーケンス対応したらここもする

  enum
  {
    KILL_EMITTER,
    CLEAR_RESOURCE,
    DELETE_BINARY,
    DELETE_INSTANCE,
  };

  switch( m_seq)
  {
  case KILL_EMITTER:
    m_pSystem->KillAllEmitter();
    m_seq++;
    //break;

  case CLEAR_RESOURCE:
    for( u32 i = 0; i < RESOURCE_ID_MAX; ++i)
    {
      if( m_pResourceBin[i] != NULL)
      {
        m_pSystem->ClearResource( m_pHeap, i);
      }
    }
    m_seq++;
    //break;

  case DELETE_BINARY:
    for( u32 i = 0; i < RESOURCE_ID_MAX; ++i)
    {
      if( m_pResourceBin[i] != NULL)
      {
        m_pGLAllocator->SystemMemoryFree( m_pResourceBin[i]);
        m_pResourceBin[i] = NULL;
      }
    }
    m_seq++;
    //break;

  case DELETE_INSTANCE:
    GFL_SAFE_DELETE( m_pSystem);
    GFL_SAFE_DELETE( m_pGroupIDList);
    GFL_SAFE_DELETE( m_pHeap);
    return true;
    //break;
  }

  return false;
}

/**
 * @brief   更新
 */
void Manager::Update()
{
  m_pSystem->BeginFrame();

  gfl2::util::List<u32>::Iterator iter = m_pGroupIDList->Begin();
  for( ; iter != m_pGroupIDList->End(); ++iter )
  {
    m_pSystem->Calc( *iter);
  }
}

/**
 * @brief   リソース作成
 * @param   resId リソースID。このクラスで定義
 * @retval  true 完了
 * @retval  false 実行中
 * @note    今は1フレームで完了する
 */
b8 Manager::CreateResource( ResourceID resId)
{
  if( m_pResourceBin[resId] == NULL)
  {
    m_pResourceBin[resId] = Test::FieldTest::Utility::ReadData( PTCL_RESOURCE_PATH_TABLE[resId], 128);

    m_pSystem->EntryResource( m_pHeap, m_pResourceBin[resId], resId);
  }
  return true;
}

/**
 * @brief   リソース破棄
 * @param   resId リソースID。このクラスで定義
 * @retval  true 完了
 * @retval  false 実行中
 * @note    リソース破棄→バイナリ破棄を数フレームかけて行う
 */
b8 Manager::DestroyResource( ResourceID resId)
{
  enum
  {
    CLEAR_RESOURCE,
    DELETE_BINARY,
  };

  if( m_pResourceBin[resId] == NULL)
  {
    return true;
  }

  switch( m_seq)
  {
  case CLEAR_RESOURCE:
    m_pSystem->ClearResource( m_pHeap, resId);
    m_seq++;
    //break;

  case DELETE_BINARY:
    m_pGLAllocator->SystemMemoryFree( m_pResourceBin[resId]);
    m_pResourceBin[resId] = NULL;
    m_seq = 0;
    return true;
    //break;
  }

  return false;
}

/**
 * @brief   リソース作成
 * @param   resId リソースID。このクラスで定義
 * @retval  true 完了
 * @retval  false 実行中
 * @note    今は1フレームで完了する
 */
b8 Manager::CreateModelResource( ResourceID resId)
{
  if( m_pModelPackBin[resId] == NULL)
  {
    char strbuff[128] = {0};
    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Effect Binary[%d]",  resId );
    int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    m_pModelPackBin[resId] = reinterpret_cast<c8*>( Test::FieldTest::Utility::ReadData( MODEL_RESOURCE_PATH_TABLE[resId], 128));
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
    resourceData.SetModelData( m_pModelPackBin[resId]);

    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Effect ModelResource[%d]",  resId );
    _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    m_pModelResourceRootNode[resId] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData);
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
    
#if 1
    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Effect Model[%d]",  resId );
    _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode[resId] );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
    delete pModelInstanceNode;
#endif
  }

  if( m_pMotionBin[resId] == NULL)
  {
    char strbuff[128] = {0};
    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Effect ModelMotionBinary[%d]",  resId );
    int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    m_pMotionBin[resId] = reinterpret_cast<c8*>( Test::FieldTest::Utility::ReadData( MOTION_RESOURCE_PATH_TABLE[resId], 128));
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( m_pMotionBin[resId]);

    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Effect ModelMotionResource[%d]",  resId );
    _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    m_pMotionResourceRootNode[resId] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData);
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
     



  }
  return true;
}

/**
 * @brief   リソース破棄
 * @param   resId リソースID。このクラスで定義
 * @retval  true 完了
 * @retval  false 実行中
 * @note    リソース破棄→バイナリ破棄を数フレームかけて行う
 */
b8 Manager::DestroyModelResource( ResourceID resId)
{
  enum
  {
    CLEAR_RESOURCE,
    DELETE_BINARY,
  };

  if( m_pModelPackBin[resId] == NULL && m_pMotionBin[resId] == NULL)
  {
    return true;
  }

  switch( m_seq)
  {
  case CLEAR_RESOURCE:
    GFL_SAFE_DELETE( m_pModelResourceRootNode[resId]);
    GFL_SAFE_DELETE( m_pMotionResourceRootNode[resId]);
    m_seq++;
    //break;

  case DELETE_BINARY:
    this->SafeMemoryFree( m_pModelPackBin[resId]);
    m_pModelPackBin[resId] = NULL;
    this->SafeMemoryFree( m_pMotionBin[resId]);
    m_pMotionBin[resId] = NULL;
    m_seq = 0;
    return true;
    //break;
  }

  return false;
}

/**
 * @brief   リソースが生成されているか
 * @param   resId リソースID。このクラスで定義
 * @retval  true 生成済み
 * @retval  false 生成されていない
 */
b8 Manager::IsCreateResource( ResourceID resId)
{
  return ( m_pResourceBin[resId] != NULL );
}

/**
 * @brief   リソースが生成されているか
 * @param   resId リソースID。このクラスで定義
 * @retval  true 生成済み
 * @retval  false 生成されていない
 */
b8 Manager::IsCreateModelResource( ResourceID resId)
{
  return ( m_pModelPackBin[resId] != NULL && m_pMotionBin[resId] != NULL );
}

/**
 * @brief   エフェクト描画パスのコンフィグ情報取得
 * @param   エフェクト描画コンフィグ情報のポインタ
 */
void Manager::GetEffectRenderPathConfig( gfl2::Effect::EffectRenderPath::Config* pConfig)
{
  pConfig->m_pSystem        = m_pSystem;
  pConfig->m_cpGroupIDList  = m_pGroupIDList;
  pConfig->m_FrameRate      = m_frameRate;
}

/**
 * @brief   モデルリソースノード取得
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* Manager::GetModelResourceRootNode( ResourceID resId)
{
  GFL_ASSERT( resId < RESOURCE_ID_MAX);
  return m_pModelResourceRootNode[resId];
}

/**
 * @brief   モーションリソースノード取得
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* Manager::GetMotionResourceRootNode( ResourceID resId)
{
  GFL_ASSERT( resId < RESOURCE_ID_MAX);
  return m_pMotionResourceRootNode[resId];
}

/**
 * @brief   安全にメモリを解放
 */
void Manager::SafeMemoryFree( void* pAddress)
{
  if( pAddress != NULL)
  {
    m_pGLAllocator->SystemMemoryFree( reinterpret_cast<void*>( pAddress));
  }
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
