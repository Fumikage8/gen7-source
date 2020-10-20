/**
* @brief  フィールドの地形リソースの生成を担うクラス
* @file   FieldTerrainFactory.cpp
* @author ikuta_junya
* @data   2015.04.19
*/

// gfl2
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

// RenderingEngine
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>

// niji
#include "arc_def_index/arc_def.h"
#include "System/include/HeapDefine.h"
#include "System/include/nijiAllocator.h"

// field
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainFactory.h"
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainLayoutData.h"
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainTypes.h"
#include "ViewerProc/include/FieldRo/Terrain/Block/FieldTerrainBlockTypes.h"
#include "ViewerProc/include/FieldRo/FieldEnvManager.h"
///#include "Field/FieldRo/include/KusaModel/FieldKusaModel.h"
#include "ViewerProc/include/FieldRo/MyRenderingPipeLine.h"


#include "ViewerProc/include/FieldStatic/Collision/CollisionSceneNone.h"
#include "ViewerProc/include/FieldStatic/Collision/StaticActor.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

// ブロックデータ内部の構成
struct ModelDataIndex
{
  enum Value
  {
    HIGH = 0,           // highモデルデータ(gfbmdl型式)
    ENCOUNTGRASS_1,     // エンカウント草 1
    ENCOUNTGRASS_2,     // エンカウント草 2
    ENCOUNTGRASS_3,     // エンカウント草 3
    ENCOUNTGRASS_4,     // エンカウント草 4
    ENCOUNTGRASS_5,     // エンカウント草 5
    HEIGHT_COLLISION,   // 高さコリジョンデータ(gfbcol型式)
    WALL_COLLISION,     // 壁コリジョンデータ(gfbcol型式)
    MAX,

    ENCOUNTGRASS_NUM = HEIGHT_COLLISION - ENCOUNTGRASS_1 // エンカウント草数
  };
};

class TerrainFactory::HighModelUnit
{
public:
  
  struct UnitState
  {
    enum Value
    {
      NOT_RUNNING = 0,    // 動いていない
      LOADING,            // ロード中
      COMPLETION,
      MAX
    };
  };


  HighModelUnit( void ) : 
    m_UnitState( UnitState::NOT_RUNNING )
    ,m_IsPlanMode( false )
    ,m_X( 0 )
    ,m_Z( 0 )
    ,m_UnitID(0)
    ,m_pFileManager( NULL )
    ,m_pBinary( NULL )
    ,m_pModelResource( NULL )
    ,m_pHeightCollisionModelResource( NULL )
    ,m_pWallCollisionModelResource( NULL )
    ,m_pLocalDeviceHeap( NULL )
    ,m_pLocalDeviceHeapAllocator( NULL )
  {
    for(int i=0; i<ModelDataIndex::ENCOUNTGRASS_NUM; ++i){
      m_pEncountGrassModelResource[i] = NULL;
    }
  }

  virtual ~HighModelUnit( void )
  {
    Terminate();
  }

  void Initialize( gfl2::heap::HeapBase* pDeviceHeap, gfl2::fs::AsyncFileManager* pFileManager, s32 unitID, b32 isPlanMode = false )
  {
    m_pFileManager = pFileManager;
    m_UnitID = unitID;
    m_UnitState = UnitState::NOT_RUNNING;
    m_IsPlanMode = isPlanMode;

#if defined(GF_PLATFORM_WIN32)
    if( m_IsPlanMode )
    {
      // 設計マップモードではWIN32専用ヒープから作成する
      gfl2::heap::HeapBase* pWin32WorkHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_WIN32_WORK );
      m_pLocalDeviceHeap = GFL_CREATE_LOCAL_HEAP( pWin32WorkHeap, 1024*1024*32, gfl2::heap::HEAP_TYPE_EXP, false );
    }
    else
    {
      // @todo とりあえず3MB
      // @todo 3MB足りなくなったので
      gfl2::heap::HeapBase* pWin32WorkHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_WIN32_WORK );
      m_pLocalDeviceHeap = GFL_CREATE_LOCAL_HEAP( pWin32WorkHeap, 1024*1024*8, gfl2::heap::HEAP_TYPE_EXP, false );
      // m_pLocalDeviceHeap = GFL_CREATE_LOCAL_HEAP( pDeviceHeap, 0x300000, gfl2::heap::HEAP_TYPE_EXP, false );
    }

#else
    // @todo とりあえず1.5MB
    m_pLocalDeviceHeap = GFL_CREATE_LOCAL_HEAP( pDeviceHeap, 0x150000, gfl2::heap::HEAP_TYPE_EXP, false );
#endif

    m_pLocalDeviceHeapAllocator = GFL_NEW( m_pLocalDeviceHeap ) System::nijiAllocator( m_pLocalDeviceHeap, System::nijiAllocator::APPLICATION );
  }

  void Terminate( void )
  {
    if( m_pLocalDeviceHeap )
    {
      GFL_DELETE( m_pLocalDeviceHeapAllocator );
      GFL_DELETE_HEAP( m_pLocalDeviceHeap );
      m_pLocalDeviceHeap = NULL;
    }
  }

  void Update( void )
  {
    if( m_UnitState == UnitState::LOADING )
    {
      if( m_pFileManager->IsArcFileLoadDataFinished((void**)&m_pBinary) )
      {
        // リソース作成
        gfl2::fs::BinLinkerAccessor dataAccessor( m_pBinary );

        // highモデル
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData data;
          data.SetModelData( static_cast<c8*>( dataAccessor.GetData( ModelDataIndex::HIGH )) );
          m_pModelResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( m_pLocalDeviceHeapAllocator, &data );
        }

        // エンカウント草モデル
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData data;
          for( int i=0; i<ModelDataIndex::ENCOUNTGRASS_NUM; ++i ){
            if( dataAccessor.GetDataSize( ModelDataIndex::ENCOUNTGRASS_1 + i ) > 0 ){
              data.SetModelData( static_cast<c8*>( dataAccessor.GetData( ModelDataIndex::ENCOUNTGRASS_1 + i )) );
              m_pEncountGrassModelResource[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( m_pLocalDeviceHeapAllocator, &data );
            }
            else
            {
              break;  //生成終わり。
            }
          }
        }

        // 高さコリジョンモデル
        {
          s32 size = 0; // @todo
          gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
          resourceData.SetModelData( static_cast<c8*>( dataAccessor.GetData( ModelDataIndex::HEIGHT_COLLISION )), size );
          m_pHeightCollisionModelResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( m_pLocalDeviceHeapAllocator, &resourceData );
        }

        // 壁コリジョンモデル
        {
          s32 size = 0; // @todo
          gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
          resourceData.SetModelData( static_cast<c8*>( dataAccessor.GetData( ModelDataIndex::WALL_COLLISION )), size );
          m_pWallCollisionModelResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( m_pLocalDeviceHeapAllocator, &resourceData );
        }

        m_UnitState = UnitState::COMPLETION;
      }
    }
  }

  b32 CheckDataValid( u32 arcId, u32 datId, gfl2::heap::HeapBase* pTempFileHeap ) const
  {
    const gfl2::fs::ArcFile* pFile = m_pFileManager->GetArcFile( arcId );

    if( pFile == NULL )
    {
      return false;
    }

    size_t dataSize;
    gfl2::fs::Result fsResult;
    fsResult = pFile->GetRealDataSize( &dataSize, datId, pTempFileHeap );

    if( fsResult.IsSuccess() == false || dataSize == 0 )
    {
      return false;
    }

    return true;
  }


  /**
  * @brief 非同期読み込み開始
  *
  * @param x             x軸番号
  * @param z             z軸番号
  * @param arcId         アーカイブ番号
  * @param datId         データ番号
  */
  void LoadAsync( u32 x, u32 z, u32 arcId, u32 datId )
  {
    if( IsRunning() )
    {
      return;
    }

    // リクエスト生成
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId      = arcId;
    req.datId      = datId;
    req.ppBuf      = &m_pBinary;
    req.heapForBuf = m_pLocalDeviceHeap;
    req.align      = 128; // テクスチャは128バイトアラインメント
    req.heapForReq = m_pLocalDeviceHeap->GetLowerHandle();
    req.heapForCompressed = NULL;

    m_pFileManager->AddArcFileLoadDataReq( req );
    m_UnitState = UnitState::LOADING;
    m_X = x;
    m_Z = z;
  }


  void UnLoad( void )
  {

    switch( m_UnitState )
    {
    case UnitState::NOT_RUNNING:
      {
        return;
      }

    case UnitState::LOADING:
      {
        // @todo
        return;
      }
    }

    // リソースノードとバッファを開放
    GFL_SAFE_DELETE( m_pModelResource );
    GFL_SAFE_DELETE( m_pWallCollisionModelResource );
    GFL_SAFE_DELETE( m_pHeightCollisionModelResource );
    for(int i=0; i<ModelDataIndex::ENCOUNTGRASS_NUM; ++i){
      GFL_SAFE_DELETE( m_pEncountGrassModelResource[i] );
    }
    GflHeapSafeFreeMemory( m_pBinary );

    // 稼動停止
    m_UnitState = UnitState::NOT_RUNNING;
  }

  b32 IsRunning( void ) const
  {
    return ( m_UnitState != UnitState::NOT_RUNNING );
  }
  
  b32 IsLoading( void ) const
  {
    return ( m_UnitState == UnitState::LOADING );
  }

  b32 IsCompletion( void ) const
  {
    return ( m_UnitState == UnitState::COMPLETION );
  }

  s32 GetUnitID( void ) const 
  {
    return m_UnitID;
  }

  b32 CheckBlockNumber( u32 x, u32 z ) const
  {
    if( this->IsRunning() == false )
    {
      return false;
    }

    if( m_X == x && m_Z == z )
    {
      return true;
    }

    return false;
  }

  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetModelResourceNode( void )
  {
    return m_pModelResource;
  }

  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetHeightCollisionModelResourceNode( void )
  {
    return m_pHeightCollisionModelResource;
  }

  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetWallCollisionModelResourceNode( void )
  {
    return m_pWallCollisionModelResource;
  }

  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetEncountGrassResourceNode( u32 index )
  {
    GFL_ASSERT( index < ModelDataIndex::ENCOUNTGRASS_NUM );
    return m_pEncountGrassModelResource[index];
  }

  System::nijiAllocator* GetAllocator( void )
  {
    return m_pLocalDeviceHeapAllocator;
  }

  gfl2::heap::HeapBase* GetDeviceHeap( void )
  {
    return m_pLocalDeviceHeap;
  }

private:
  UnitState::Value m_UnitState;
  b32 m_IsPlanMode;
  u32 m_X;
  u32 m_Z;
  s32 m_UnitID;

  gfl2::fs::AsyncFileManager*                                 m_pFileManager;
  void*                                                       m_pBinary;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pModelResource;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pHeightCollisionModelResource;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pWallCollisionModelResource;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pEncountGrassModelResource[ModelDataIndex::ENCOUNTGRASS_NUM];

  gfl2::heap::HeapBase* m_pLocalDeviceHeap;                   // 専用のヒープ、寿命も管理する　
  System::nijiAllocator* m_pLocalDeviceHeapAllocator;      // 専用のアロケーター、寿命も管理する　
};


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TerrainFactory::TerrainFactory( void ) :

  // 制御
  m_pTerrainLayoutData( NULL )

  // オブジェクト
  ,m_pHighModelUnitTable( NULL )
  ,m_HighModelUnitNum( 0 )
  ,m_HighModelUnitRunningNum( 0 )

  // リソースデータ
  ,m_pTextureResourceRootNode( NULL )
  ,m_pShaderResourceRootNode( NULL )
  ,m_pHightModelResourceTable( NULL )
  ,m_ppMiddleModelResourceTable( NULL )
  ,m_MiddleModelResourceNum( 0 )
  ,m_ppDistantViewModelResourceTable( NULL )
  ,m_DistantViewModelResourceNum( 0 )
  ,m_pLocalAnimationRootNode( NULL )
  ,m_pOneDayAnimationRootNode( NULL )

  // 外部オブジェクト
  ///,m_pFileManager( NULL )
  ,m_pEnvManager( NULL )
{

}

/**
* @brief デストラクタ
*
* @return 無し
*/
TerrainFactory::~TerrainFactory( void )
{
  this->Terminate();
}

/**
* @brief 初期化処理
*
* @param  rInitDesc   初期化設定
*
* @return 無し
*/
void TerrainFactory::Initialize( const InitializeDescription& rInitDesc, BaseCollisionScene* pCollisionScene)
{
  m_pHighModelDeviceHeap = rInitDesc.m_pHighModelDeviceHeap;
  ///m_pFileManager = rInitDesc.m_pFileManager;
  m_pAllocator = rInitDesc.m_pAllocator;
  m_pTerrainLayoutData = rInitDesc.m_pTerrainLayoutData;
  m_pEnvManager = rInitDesc.m_pEnvManager;
  m_pRenderingPipeLine = rInitDesc.m_pRenderingPipeLine;

	m_pCollisionScene = pCollisionScene;

	m_ResourceAllocator.init(m_pHighModelDeviceHeap);

	//highモデルの生成
	{
	  gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinModelRootNode = NULL;
	  // パックされたデータからリソースノードを作成
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
	  c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/map.wingl.gfbmdlp", &m_ResourceAllocator );
	  resourceData.SetModelData( pBinData );
	  pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
	  m_pBinaryDataArray[0] = pBinData;
	  m_pHightModelResource = pBinModelRootNode;
	}

	// 高さコリジョンモデル
  {
		s32 size = 0; // @todo
    gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
		c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/map.gfbcol", &m_ResourceAllocator );
		m_pBinaryDataArray[1] = pBinData;
		resourceData.SetModelData( pBinData, size );
		m_pHeightCollisionModelResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
  }

	//コリジョン
	m_pCollisionModel = CreateHeightCollisionModel();
	//コリジョン生成・登録
  {
		if( m_pCollisionModel )
    {
			m_pStaticActor = m_pCollisionScene->CreateStaticActor();
      m_pStaticActor->CreateMesh( gfl2::math::Vector3(0.0f,0.0f,0.0f), m_pCollisionModel );
    }
		if( m_pStaticActor )
    {
			m_pCollisionScene->RegistStaticActor( m_pStaticActor );//@todo登録するとうまくいかない
    }
	}

	// ローカルループアニメーション生成
	{
		void* pData = ReadData( "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/map_local.gfbmot", &m_ResourceAllocator );
	  if (pData != NULL)
	  {
		  ///gfl2::fs::BinLinkerAccessor dataAccessor( pData );
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData data;
      data.SetMotionData( static_cast<c8*>( /*dataAccessor.GetData( 0 )*/pData ) );  // @todo 0
		  m_pLocalAnimationRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &data );
			m_pBinaryDataArray[2] = (c8*)pData; 
	  }
	}

	// 1日同期アニメーション
	{
		void* pData = ReadData( "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/map_one_day.gfbmot", &m_ResourceAllocator );
    if (pData != NULL)
	  {
		  ///gfl2::fs::BinLinkerAccessor dataAccessor( pData );
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData data;
		  data.SetMotionData( static_cast<c8*>( /*dataAccessor.GetData( 0 )*/pData ) );  // @todo 0
		  m_pOneDayAnimationRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &data );
    }
		m_pBinaryDataArray[3] = (c8*)pData; 
	}

	m_pTerrainModel = CreateHighModel();

#if 0

  // Highモデル用Garcを開く
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId				= ARCID_FIELD_TERRAIN_HIGH_MODEL;
    req.heapForFile	= rInitDesc.m_pSystemHeap;
    req.heapForReq	= rInitDesc.m_pSystemHeap;
    m_pFileManager->SyncArcFileOpen( req );
  }

  // Highモデル用生成Unitを生成
  {
    m_pHighModelUnitTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) HighModelUnit[ rInitDesc.m_HighModleWorkNum ];
    m_HighModelUnitNum = rInitDesc.m_HighModleWorkNum;
    m_HighModelUnitRunningNum = 0;

    HighModelUnit* pTarget = m_pHighModelUnitTable;
    for( u32 i = 0; i < m_HighModelUnitNum; i++, pTarget++ )
    {
      pTarget->Initialize(
        rInitDesc.m_pHighModelDeviceHeap,
        rInitDesc.m_pFileManager,
        i,   // 添え字演算子でアクセスするため、順番にIDを割り振る
        m_pTerrainLayoutData->IsPlanMode()
        );
    }
  }

  // Middleモデルリソース生成(ない時は0バイト)
  {
    if( rInitDesc.m_pAreaResourceDataAccessor->GetDataSize( AreaResourceDataIndex::MODEL_MIDDLE ) > 0 )
    {
      void* pData = rInitDesc.m_pAreaResourceDataAccessor->GetData( AreaResourceDataIndex::MODEL_MIDDLE );
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );

      m_MiddleModelResourceNum = dataAccessor.GetDataMax();

      // このif文にはじかれる事はないはず
      if( m_MiddleModelResourceNum > 0 )
      {
        m_ppMiddleModelResourceTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [ m_MiddleModelResourceNum ];
        for( u32 i = 0; i < m_MiddleModelResourceNum; i++ )
        {
          // データサイズが0の場合はdummyが埋め込まれている、製品ではありえないが開発中は許容
          if( dataAccessor.GetDataSize( i ) == 0 )
          {
            m_ppMiddleModelResourceTable[ i ] = NULL;
          }
          else
          {
            gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
            resourceData.SetModelData( static_cast<c8*>( dataAccessor.GetData( i ) ));
            m_ppMiddleModelResourceTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( rInitDesc.m_pAllocator, &resourceData );
          }
        }
      }
    }
  } // Middleモデルリソース生成

  // 遠景モデルリソース生成(ない時は0バイト)
  {
    if( rInitDesc.m_pAreaResourceDataAccessor->GetDataSize( AreaResourceDataIndex::MODEL_DISTANT_VIEW ) > 0 )
    {
      void* pData = rInitDesc.m_pAreaResourceDataAccessor->GetData( AreaResourceDataIndex::MODEL_DISTANT_VIEW );
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );

      m_DistantViewModelResourceNum = dataAccessor.GetDataMax();

      // このif文にはじかれる事はないはず
      if( m_DistantViewModelResourceNum > 0 )
      {
        m_ppDistantViewModelResourceTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [ m_DistantViewModelResourceNum ];
        for( u32 i = 0; i < m_DistantViewModelResourceNum; i++ )
        {
          // データサイズが0の場合はdummyが埋め込まれている、製品ではありえないが開発中は許容
          if( dataAccessor.GetDataSize( i ) == 0 )
          {
            m_ppDistantViewModelResourceTable[ i ] = NULL;
          }
          else
          {
            gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
            resourceData.SetModelData( static_cast<c8*>( dataAccessor.GetData( i ) ));
            m_ppDistantViewModelResourceTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( rInitDesc.m_pAllocator, &resourceData );
          }
        }
      }
    }
  } // 遠景モデルリソース生

  // テクスチャリソース生成
  {
    // @todo 仮
    m_pTextureResourceRootNode = FieldResourceNode::CreateNode();
    gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pTextureResourceRootNode );
    
    if( rInitDesc.m_pAreaResourceDataAccessor->GetDataSize( AreaResourceDataIndex::TEXTURE ) > 0 )
    {
      void* pData = rInitDesc.m_pAreaResourceDataAccessor->GetData( AreaResourceDataIndex::TEXTURE );
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );

      for( u32 i = 0; i < dataAccessor.GetDataMax(); i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	data;
        data.SetTextureData( static_cast<c8*>( dataAccessor.GetData( i ) ) );
        m_pTextureResourceRootNode->AddChild( gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( rInitDesc.m_pAllocator, &data ) );
      }
    }
  }

  // シェーダーリソース生成
  {
    // @todo ResourceNodeを別々で渡せるようになっていない
    //m_pShaderResourceRootNode = FieldResourceNode::CreateNode();
    //gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pShaderResourceRootNode );
    
    if( rInitDesc.m_pAreaResourceDataAccessor->GetDataSize( AreaResourceDataIndex::SHADER ) > 0 )
    {
      void* pData = rInitDesc.m_pAreaResourceDataAccessor->GetData( AreaResourceDataIndex::SHADER );
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );

      for( u32 i = 0; i < dataAccessor.GetDataMax(); i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryShdData data;
        data.SetShaderData( static_cast<c8*>( dataAccessor.GetData( i ) ) );
        // m_pShaderResourceRootNode->AddChild( gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &data ) );
        m_pTextureResourceRootNode->AddChild( gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( rInitDesc.m_pAllocator, &data ) );
      }
    }
  }

  // ローカルループアニメーション生成
  {
    if( rInitDesc.m_pAreaResourceDataAccessor->GetDataSize( AreaResourceDataIndex::ANIME_LOCAL ) > 0 )
    {
      void* pData = rInitDesc.m_pAreaResourceDataAccessor->GetData( AreaResourceDataIndex::ANIME_LOCAL );
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );
    

      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData data;
      data.SetMotionData( static_cast<c8*>( dataAccessor.GetData( 0 ) ) );  // @todo 0
      m_pLocalAnimationRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( rInitDesc.m_pAllocator, &data );
    }
  }

  // 1日同期アニメーション
  {
    if( rInitDesc.m_pAreaResourceDataAccessor->GetDataSize( AreaResourceDataIndex::ANIME_ONE_DAY ) > 0 )
    {
      void* pData = rInitDesc.m_pAreaResourceDataAccessor->GetData( AreaResourceDataIndex::ANIME_ONE_DAY );
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );
    

      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData data;
      data.SetMotionData( static_cast<c8*>( dataAccessor.GetData( 0 ) ) );  // @todo 0
      m_pOneDayAnimationRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( rInitDesc.m_pAllocator, &data );
    }
  }
#endif
}

/**
* @brief 終了処理
*
* @return 無し
*/
void TerrainFactory::Terminate( void )
{
  // 1日同期アニメーション破棄
  {
    GFL_SAFE_DELETE( m_pOneDayAnimationRootNode );
  }

  // ローカルループアニメーション破棄
  {
    GFL_SAFE_DELETE( m_pLocalAnimationRootNode );
  }

  // 遠景モデルリソース破棄
  if( m_ppDistantViewModelResourceTable )
  {
    for( u32 i = 0; i < m_DistantViewModelResourceNum; i++ )
    {
      GFL_SAFE_DELETE( m_ppDistantViewModelResourceTable[i] );
    }

    GFL_SAFE_DELETE_ARRAY( m_ppDistantViewModelResourceTable );
    m_DistantViewModelResourceNum = 0;
  }

  // Highモデル用生成Unitを破棄
  {
    GFL_SAFE_DELETE_ARRAY(m_pHighModelUnitTable);
  }

  // シェーダーリソース破棄
  {
    GFL_SAFE_DELETE( m_pShaderResourceRootNode );
  }

  // テクスチャリソース破棄
  {
    GFL_SAFE_DELETE( m_pTextureResourceRootNode );
  }

  // Middleモデルリソース破棄
  if( m_ppMiddleModelResourceTable  )
  {
    for( u32 i = 0; i < m_MiddleModelResourceNum; i++ )
    {
      GFL_SAFE_DELETE( m_ppMiddleModelResourceTable[i] );
    }

    GFL_SAFE_DELETE_ARRAY( m_ppMiddleModelResourceTable );
    m_MiddleModelResourceNum = 0;
  }
#if 0
  // Highモデル用Garcを閉じる
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId				= ARCID_FIELD_TERRAIN_HIGH_MODEL;
    m_pFileManager->SyncArcFileClose( req );
  }
#endif //@saito_del
	for( u32 i=0 ;i<BIN_DATA_CNT ; i++ )
  {
    if (m_pBinaryDataArray[i] !=NULL)
    {
      m_ResourceAllocator.SystemMemoryFree(m_pBinaryDataArray[i]);
    }
  }
	GFL_DELETE( m_pCollisionModel );

	GFL_DELETE( m_pTerrainModel );

	GFL_SAFE_DELETE(m_pHightModelResource);
	GFL_SAFE_DELETE( m_pHeightCollisionModelResource );



  // 外部オブジェクトクリア
  m_pEnvManager = NULL;
}

/**
* @brief 更新処理(ロード状況を監視、リソースの生成)
*
* @return 無し
*/
void TerrainFactory::Update( void )
{
  HighModelUnit* pTarget = m_pHighModelUnitTable;
  for( u32 i = 0; i < m_HighModelUnitNum; i++, pTarget++ )
  {
    pTarget->Update();
  }

	m_pTerrainModel->UpdateAnimation();
}
#if 0
/**
* @brief 指定ブロック番号のhighモデルデータが有効かチェックする
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return true 有効   false 無効
*/
b32 TerrainFactory::CheckHighModelDataVailed( u32 x, u32 z ) const
{
  gfl2::fs::ArcFile::ARCID arcId = ARCID_FIELD_TERRAIN_HIGH_MODEL;
  u32 datId = m_pTerrainLayoutData->GetHighModelDataIndex( x, z );
  const gfl2::fs::ArcFile* pFile = m_pFileManager->GetArcFile( arcId );

  if( pFile == NULL )
  {
    return false;
  }

  size_t dataSize;
  gfl2::fs::Result fsResult;
  fsResult = pFile->GetRealDataSize( &dataSize, datId, m_pHighModelDeviceHeap );

  // データサイズ0はダミーデータが登録されている
  if( fsResult.IsSuccess() == false || dataSize == 0 )
  {
    return false;
  }

  return true;
}
#endif
/**
* @brief 指定ブロック番号のhighモデルデータの非同期読み込み開始
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 生成Unit番号(高速にアクセスできる、使わなくても良い)
*/
s32 TerrainFactory::LoadHighModelAsync( u32 x, u32 z )
{
  HighModelUnit* pUnit = GetNotRunningHighModelUnit();
  if( pUnit )
  {
    // @todo 引数が適当
    pUnit->LoadAsync( x, z, ARCID_FIELD_TERRAIN_HIGH_MODEL, m_pTerrainLayoutData->GetHighModelDataIndex( x, z ) );
    m_HighModelUnitRunningNum++;
    return pUnit->GetUnitID();
  }

  return -1;
}

/**
* @brief 指定ブロック番号のhighモデルデータの読み込み状態をチェックする
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 生成ユニットの状態
*/
TerrainFactory::UnitState::Value TerrainFactory::CheckHighModelUnitState( u32 x, u32 z ) const
{
  const HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit )
  {
    if( pUnit->IsCompletion() )
    {
      return UnitState::LOAD_COMPLETION;
    }
    else
    {
      return UnitState::LOADING;
    }
  }
  else
  {
    return UnitState::NOT_EXIST;
  }
}

/**
* @brief 指定ブロック番号のhighモデルデータを破棄する
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 無し
*/
void TerrainFactory::UnLoadHighModel( u32 x, u32 z )
{
  HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit && pUnit->IsCompletion() )
  {
    pUnit->UnLoad();
    m_HighModelUnitRunningNum--;
  }

  // @todo
}

/**
* @brief 稼動していないHighモデル用生成Unit数を取得
*
* @return ユニット数
*/
u32 TerrainFactory::GetHighModelUnitUnRunnigNum( void ) const
{
  return m_HighModelUnitNum - m_HighModelUnitRunningNum;
}

/**
* @brief 指定ブロック番号のhighモデルを生成する
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 生成したインスタンス(不正な場合はNULLを返す)
*/
gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* TerrainFactory::CreateHighModelNode( u32 x, u32 z )
{
  HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit && pUnit->IsCompletion() )
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = NULL;
    pModelInstance = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(
      pUnit->GetAllocator(),
      pUnit->GetModelResourceNode(),
      m_pTextureResourceRootNode
      );

    // Fog適応
    if( pModelInstance && m_pEnvManager )
    {
      m_pEnvManager->ApplyFogParam( pModelInstance );
    }

    return pModelInstance;
  }

  return NULL;
}

poke_3d::model::BaseModel* TerrainFactory::CreateHighModel( void )
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance =
		gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pHightModelResource );
  if( pModelInstance )
  {
    // @todo
    poke_3d::model::BaseModel* pModel = GFL_NEW( m_pHighModelDeviceHeap ) poke_3d::model::BaseModel;
    pModel->Create( m_pAllocator, m_pHighModelDeviceHeap, pModelInstance );

    // ローカルループアニメーション適応
    if( m_pLocalAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pLocalAnimationRootNode, Block::AnimationSlotNo::LOCAL );
      pModel->SetAnimationLoop( true, Block::AnimationSlotNo::LOCAL );
    }

    // 1日同期アニメーション適応
    if( m_pOneDayAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pOneDayAnimationRootNode, Block::AnimationSlotNo::ONE_DAY );
      pModel->SetAnimationLoop( true, Block::AnimationSlotNo::ONE_DAY );
    }
  	
  	m_pRenderingPipeLine->AddEdgeRenderingTarget( pModel->GetModelInstanceNode() );
    
    return pModel;
  }

  return NULL;
}
#if 0
/**
* @brief 指定ブロック番号のhighモデルを生成する
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 生成したインスタンス(不正な場合はNULLを返す)
*/
poke_3d::model::BaseModel* TerrainFactory::CreateHighModel( u32 x, u32 z )
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = CreateHighModelNode( x, z );
  if( pModelInstance )
  {
    // @todo
    poke_3d::model::BaseModel* pModel = GFL_NEW( m_pHighModelDeviceHeap ) poke_3d::model::BaseModel;
    pModel->Create( m_pAllocator, m_pHighModelDeviceHeap, pModelInstance );

    // ローカルループアニメーション適応
    if( m_pLocalAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pLocalAnimationRootNode, Block::AnimationSlotNo::LOCAL );
      pModel->SetAnimationLoop( true, Block::AnimationSlotNo::LOCAL );
    }

    // 1日同期アニメーション適応
    if( m_pOneDayAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pOneDayAnimationRootNode, Block::AnimationSlotNo::ONE_DAY );
      pModel->SetAnimationLoop( true, Block::AnimationSlotNo::ONE_DAY );
    }
    
    return pModel;
  }

  return NULL;
}
#endif //@saito_del

gfl2::collision::CollisionModel* TerrainFactory::CreateHeightCollisionModel()
{
	gfl2::renderingengine::scenegraph::resource::ResourceNode* pResource( m_pHeightCollisionModelResource );
	if( pResource )
  {
		gfl2::collision::CollisionModel* pClollisionModel = GFL_NEW( m_pHighModelDeviceHeap ) gfl2::collision::CollisionModel();
    pClollisionModel->Initialize( pResource );
    return pClollisionModel;
  }

  // @todo 仮
  return NULL;
}

/**
* @brief 指定ブロック番号の高さコリジョンモデルを生成する
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 生成したインスタンス(不正な場合はNULLを返す)
*/
gfl2::collision::CollisionModel* TerrainFactory::CreateHeightCollisionModel( u32 x, u32 z )
{
  HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit && pUnit->IsCompletion() )
  {
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pResource( pUnit->GetHeightCollisionModelResourceNode() );

    if( pResource )
    {
      gfl2::collision::CollisionModel* pClollisionModel = GFL_NEW( pUnit->GetDeviceHeap() ) gfl2::collision::CollisionModel();
      pClollisionModel->Initialize( pResource );
      return pClollisionModel;
    }
  }

  // @todo 仮
  return NULL;
}

/**
* @brief 指定ブロック番号の壁コリジョンモデルを生成する
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 生成したインスタンス(不正な場合はNULLを返す)
*/
gfl2::collision::CollisionModel* TerrainFactory::CreateWallCollisionModel( u32 x, u32 z )
{
  HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit && pUnit->IsCompletion() )
  {
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pResource( pUnit->GetWallCollisionModelResourceNode() );

    if( pResource )
    {
      gfl2::collision::CollisionModel* pClollisionModel = GFL_NEW( pUnit->GetDeviceHeap() ) gfl2::collision::CollisionModel();
      pClollisionModel->Initialize( pResource );
      return pClollisionModel;
    }
  }

  // @todo 仮
  return NULL;
}

/**
* @brief 指定ブロック番号のMiddleモデルを生成する
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 生成したインスタンス(不正な場合はNULLを返す)
*/
gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* TerrainFactory::CreateMiddleModelNode( u32 x, u32 z )
{
  u32 dataId = m_pTerrainLayoutData->GetMiddleModelDataIndex( x, z );
  if( dataId == DATA_ID_INVALID_VALUE || dataId >= m_MiddleModelResourceNum )
  {
    return NULL;
  }

  // map以下にデータがコミットされていない場合はリソースデータが無い、製品ではありえないが開発中は許容
  if( m_ppMiddleModelResourceTable[ dataId ] == NULL )
  {
    return NULL;
  }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = NULL;
  pModelInstance = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(
    m_pAllocator,
    m_ppMiddleModelResourceTable[ dataId ],
    m_pTextureResourceRootNode
    );

  // Fog適応
  if( pModelInstance && m_pEnvManager )
  {
    m_pEnvManager->ApplyFogParam( pModelInstance );
  }

  return pModelInstance;
}

/**
* @brief 指定ブロック番号のMiddleモデルを生成する
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 生成したインスタンス(不正な場合はNULLを返す)
*/
poke_3d::model::BaseModel* TerrainFactory::CreateMiddleModel( u32 x, u32 z )
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = CreateMiddleModelNode( x, z );
  if( pModelInstance )
  {
    // @todo
    poke_3d::model::BaseModel* pModel = GFL_NEW( m_pHighModelDeviceHeap ) poke_3d::model::BaseModel;
    pModel->Create( m_pAllocator, m_pHighModelDeviceHeap, pModelInstance );

    // ローカルループアニメーション適応
    if( m_pLocalAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pLocalAnimationRootNode, Block::AnimationSlotNo::LOCAL );
      pModel->SetAnimationLoop( true, Block::AnimationSlotNo::LOCAL );
    }

    // 1日同期アニメーション適応
    if( m_pOneDayAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pOneDayAnimationRootNode, Block::AnimationSlotNo::ONE_DAY );
      pModel->SetAnimationLoop( true, Block::AnimationSlotNo::ONE_DAY );
    }

    return pModel;
  }

  return NULL;
}
#if 0
/**
* @brief 指定ブロック番号の草モデルを生成する
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 生成したインスタンス(不正な場合はNULLを返す)
*/
KusaModel::FieldKusaModel** TerrainFactory::CreateEncountGrassModelTable( u32 x, u32 z )
{
  KusaModel::FieldKusaModel** ppKusaModelTable = NULL;
  HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit && pUnit->IsCompletion() )
  {
    ppKusaModelTable = GFL_NEW_ARRAY( pUnit->GetDeviceHeap() ) KusaModel::FieldKusaModel*[ModelDataIndex::ENCOUNTGRASS_NUM];

    for( int i=0; i<ModelDataIndex::ENCOUNTGRASS_NUM; ++i ){
      gfl2::renderingengine::scenegraph::resource::ResourceNode * pEncountGrassResourece = pUnit->GetEncountGrassResourceNode( i );
      //if(pEncountGrassResourece){
      if(0){
        ppKusaModelTable[i] = Field::KusaModel::FieldKusaModel::CreateKusaModelInstanceNode( pUnit->GetAllocator(), pEncountGrassResourece );
        ppKusaModelTable[i]->InitKusaShader( pUnit->GetAllocator() );

        // Fog適応
        if( ppKusaModelTable[i] && m_pEnvManager )
        {
          m_pEnvManager->ApplyFogParam( ppKusaModelTable[i] );
        }
      }
      else{
        ppKusaModelTable[i] = NULL;
      }
    }
  }

  return ppKusaModelTable;
}

/**
* @brief エンカウント草モデルテーブルの要素数
* @return エンカウント草モデルテーブルの要素数
*/
u32 TerrainFactory::GetEncountGrassModelTableNum() const
{
  return ModelDataIndex::ENCOUNTGRASS_NUM;
}

/**
* @brief 指定番号の遠景モデルを生成する
*
* @param  no    番号
*
* @return 生成したインスタンス(不正な場合はNULLを返す)
*/
gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* TerrainFactory::CreateDistantViewModel( u32 no )
{
  if( no >= m_DistantViewModelResourceNum || m_ppDistantViewModelResourceTable[ no ] == NULL )
  {
    return NULL;
  }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = NULL;
  pModelInstance = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(
    m_ppDistantViewModelResourceTable[ no ],
    m_pTextureResourceRootNode
    );

  // Fog適応
  if( pModelInstance && m_pEnvManager )
  {
    m_pEnvManager->ApplyFogParam( pModelInstance );
  }

  return pModelInstance;
}

/**
* @brief 遠景モデル数を取得 @todo 本来はlayoutが持つ機能
*
* @return 遠景モデル数
*/
u32 TerrainFactory::GetDistantViewModelNum( void ) const
{
  return m_DistantViewModelResourceNum;
}
#endif //@saito_del
//
// private
//

/**
* @brief 指定ブロック番号のHighモデル用生成Unitを取得
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return ユニット(不正な場合はNULLを返す)
*/
TerrainFactory::HighModelUnit* TerrainFactory::GetHighModelUnit( u32 x, u32 z )
{
  HighModelUnit* pTarget = m_pHighModelUnitTable;
  for( u32 i = 0; i < m_HighModelUnitNum; i++, pTarget++ )
  {
    if( pTarget->IsRunning() == false )
    {
      continue;
    }

    if( pTarget->CheckBlockNumber( x, z ) )
    {
      return pTarget;
    }
  }

  return NULL;
}

/**
* @brief 指定ブロック番号のHighモデル用生成Unitを取得(const)
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return ユニット(不正な場合はNULLを返す)
*/
const TerrainFactory::HighModelUnit* TerrainFactory::GetHighModelUnit( u32 x, u32 z ) const
{
  const HighModelUnit* pTarget = m_pHighModelUnitTable;
  for( u32 i = 0; i < m_HighModelUnitNum; i++, pTarget++ )
  {
    if( pTarget->IsRunning() == false )
    {
      continue;
    }

    if( pTarget->CheckBlockNumber( x, z ) )
    {
      return pTarget;
    }
  }

  return NULL;
}


/**
* @brief 未稼働のHighモデル用生成Unitを取得
*
* @return ユニット(未稼働Unitが無い場合はNULLを返す)
*/
TerrainFactory::HighModelUnit* TerrainFactory::GetNotRunningHighModelUnit( void )
{
  HighModelUnit* pTarget = m_pHighModelUnitTable;
  for( u32 i = 0; i < m_HighModelUnitNum; i++, pTarget++ )
  {
    if( pTarget->IsRunning() == false )
    {
      return pTarget;
    }
  }

  return NULL;
}

#if 0
//----------------------------------------------------------------------------
/**
*	@brief  ファイル読み込み
*	@param  利用するアロケータ
*	@param  ファイルパス
*	@return Rawデータ
*/
//-----------------------------------------------------------------------------
void* TerrainFactory::ReadData( const char* pFilePath )
{
#if defined(GF_PLATFORM_CTR)
  nn::fs::FileReader file( pFilePath );
  u32 fileSize = file.GetSize();
  void* pShaderBinary = m_ResourceAllocator.SystemMemoryMalloc(fileSize, 128);
  file.Read(pShaderBinary, fileSize);
  return pShaderBinary;
#else
  FILE		*hFile;
  int			file_size;
  char		*read_buff;

  fopen_s( &hFile, pFilePath, "rb" );

  if ( hFile == NULL )
  {
    assert(0);
    return NULL;
  }

  fseek( hFile, 0, SEEK_END );
  file_size = ftell( hFile );
  fseek( hFile, 0, SEEK_SET );

  u32		align = 16;
  file_size += align;//必ずNULLが入るようにしたい。
  u32 buf_size = file_size + ((-file_size) & (align-1));

  read_buff = reinterpret_cast<c8*>( m_ResourceAllocator.SystemMemoryMalloc(buf_size,128) );
  memset( read_buff, 0, buf_size );
  fread( read_buff, file_size, 1, hFile );
  fclose( hFile );

  return read_buff;
#endif
}
#endif

GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );
