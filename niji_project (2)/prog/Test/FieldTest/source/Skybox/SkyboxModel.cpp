//===================================================================
/**
 * @file    SkyboxModel.cpp
 * @brief   天球リソース
 * @author  saita_kazuki
 * @date    2015.02.19
 */
//===================================================================

#include "Test/FieldTest/include/Skybox/SkyboxModel.h"

#include "test/FieldTest/include/Debug/FieldDebugInstanceMemoryCollection.h"
#include "test/FieldTest/include/FieldTestUtility.h"


// リソース
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// デバッグ
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// メモリ
#include <gfx/include/gfl2_GLMemory.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @brief   コンストラクタ
 * @param   pResourceAllocator アロケーター
 */
SkyboxModel::SkyboxModel() :
  m_seq( 0),
  m_pModelPackBinary( NULL),
  m_pResourceRootNode( NULL),
  m_pModelInstanceNode( NULL),
  m_pGLAllocator( NULL)
{
}

///< デストラクタ
SkyboxModel::~SkyboxModel()
{
}

/**
 * @brief   生成
 * @param   desc 設定情報
 */
void SkyboxModel::Create( const Description& desc)
{
  // @todo 今は非同期読み込み

  m_pGLAllocator = desc.pGLAllocator;

  gfl2::fs::Result fsResult;

  // @note GARCのオープン、クローズはこれより上層でやる

  // 読み込み
  {
    std::size_t buffsize;
    desc.pAsyncFileManager->GetArcFile( desc.arcId)->GetDataSize( &buffsize, desc.datId, NULL);

    static int create_binary_count = 0; char strbuff[128] = {0};
    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "SkyboxBinary[%d]",  create_binary_count ++ );
    int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    m_pModelPackBinary = reinterpret_cast<c8*>( m_pGLAllocator->SystemMemoryMalloc( buffsize, 128));
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
    fsResult.Clear();

    size_t realSize;

    gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
    req.arcId         = desc.arcId;
    req.datId         = desc.datId; 
    req.pBuf          = m_pModelPackBinary;
    req.bufSize       = buffsize;
    req.pRealReadSize = &realSize;
    req.result        = &fsResult;

    desc.pAsyncFileManager->SyncArcFileLoadDataBuf( req);

    if( fsResult.IsSuccess() == false)
    {
      GFL_PRINT("!! ERROR !! LoadDataReq\n");
      fsResult.Print();
      return;
    }
  }

  // リソースノード作成
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
    resourceData.SetModelData( m_pModelPackBinary);

    static int create_indtance_count = 0; char strbuff[128] = {0};
    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "SkyboxResource[%d]",  create_indtance_count ++ );
    int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    m_pResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData);
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
  }

  // インスタンス生成
  {
    static int create_indtance_count = 0; char strbuff[128] = {0};
    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "SkyboxModel[%d]",  create_indtance_count ++ );
    int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
	  m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pResourceRootNode);
    m_pModelInstanceNode->SetNodeName(strbuff);
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
  }
}

/**
 * @brief   破棄
 * @retval  true  破棄完了
 * @retval  false 破棄実行中
 */
bool SkyboxModel::Destroy()
{
  enum
  {
    DELETE_NODE,
    DELETE_BINARY,
  };

  switch( m_seq)
  {
  case DELETE_NODE:
    GFL_SAFE_DELETE( m_pModelInstanceNode);
    GFL_SAFE_DELETE( m_pResourceRootNode);
    m_seq++;
    //break;

  // @todo テストProcの破棄関数がシーケンス制御に対応したら、以下は1フレーム待ってから解放をする
  case DELETE_BINARY:
    m_pGLAllocator->SystemMemoryFree( reinterpret_cast<void*>( m_pModelPackBinary));
    return true;
    //break;

  default:
    GFL_ASSERT( 0);
    break;
  }

  return false;
}

/**
 * @brief   表示フラグ切り替え
 * @param   isEnable trueで有効、falseで無効
 */
void SkyboxModel::SetVisible( b32 isEnable)
{
  if( this->IsVisible() != isEnable)
  {
    // フラグが切り替わったらシーンへの登録、解除を行う
    if( isEnable)
    {
      gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pModelInstanceNode);
    }
    else
    {
      gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode);
    }
  }

  m_pModelInstanceNode->SetVisible( isEnable);
}

/**
 * @brief   表示フラグ取得
 * @retval  true 有効
 * @retval  false 無効
 */
b32 SkyboxModel::IsVisible() const 
{
  return m_pModelInstanceNode->IsVisible();
}

/**
 * @brief   リソースルートノード取得
 * @return   リソースルートノードインスタンス
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* SkyboxModel::GetResourceRootNode() const 
{
  return m_pResourceRootNode;
}


/**
 * @brief   インスタンスノード取得
 * @return   インスタンスノード
 */
gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* SkyboxModel::GetModelInstanceNode() const 
{
  return m_pModelInstanceNode;
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )
