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
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>

// niji
#include "arc_def_index/arc_def.h"
#include "System/include/HeapDefine.h"
#include "System/include/nijiAllocator.h"

// field
#include "Field/FieldRo/include/Terrain/FieldTerrainFactory.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainLayoutData.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainTypes.h"
#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockTypes.h"
#include "Field/FieldRo/include/FieldEnvManager.h"
#include "Field/FieldRo/include/KusaModel/FieldKusaModel.h"

#include "Field/FieldRo/include/MyRenderingPipeLine.h"

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

#if PM_DEBUG

// データサイズを出力
static const c8* sc_BinNameTable[ ModelDataIndex::MAX ] =
{
  "HIGH            ",
  "ENCOUNTGRASS_1  ",
  "ENCOUNTGRASS_2  ",
  "ENCOUNTGRASS_3  ",
  "ENCOUNTGRASS_4  ",
  "ENCOUNTGRASS_5  ",
  "HEIGHT_COLLISION",
  "WALL_COLLISION  "
};

#endif // #if PM_DEBUG

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
    ,m_pLocalDeviceHeap( NULL )
    ,m_pLocalDeviceHeapAllocator( NULL )
  {
    for(int i=0; i<ModelDataIndex::ENCOUNTGRASS_NUM; ++i){
      m_pEncountGrassModelResource[i] = NULL;
    }

    for( u32 i = 0; i < WallBinIndex::MAX; i++ )
    {
      m_pWallCollisionModelResourceTable[ i ] = NULL;
    }

#if PM_DEBUG
    m_BinaryCompressSize = 0;
    m_BinarySize = 0;
#endif
  }

  virtual ~HighModelUnit( void )
  {
    Terminate();
  }

  void Initialize( gfl2::heap::HeapBase* pDeviceHeap, gfl2::fs::AsyncFileManager* pFileManager, s32 unitID, MemoryMode::Value memoryMode, b32 isPlanMode = false )
  {
#if PM_DEBUG
    for( u32 i = 0; i < ModelDataIndex::MAX; i++ )
    {
      INSTANCE_DATA* pData = &m_DebugInstaceDataTable[ i ];

      pData->polygonNum = 0;
      pData->size = 0;
      pData->vertexNum = 0;
      pData->materialNum = 0;
      pData->meshNum = 0;
      pData->drawTagNum = 0;
    }
    m_DebugAnimeInstanceSize = 0;
#endif // #if PM_DEBUG

    m_pFileManager = pFileManager;
    m_UnitID = unitID;
    m_UnitState = UnitState::NOT_RUNNING;
    m_IsPlanMode = isPlanMode;

    gfl2::heap::HeapBase* pParentHeap = pDeviceHeap;
    u32 heapSize = 0;


#if defined(GF_PLATFORM_WIN32)
    // Win32版ではWin32用ヒープから多めに確保する
    pParentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_WIN32_WORK );

    if( m_IsPlanMode )
    {
      heapSize = 1024*1024*64*2;  // 128MB ※外マップを丸ごと表示する為、増強
    }
    else
    {
      heapSize = 1024*1024*12;    // 12MB
    }

    // @fix GFCHECK[8230]:ワーニング対処。else->elif。
#elif defined(GF_PLATFORM_CTR)

    s32 normalSize = 0x150000 - (250 * 1024); // 1.3125MB - 250kb

    switch( memoryMode )
    {
    case MemoryMode::LOW:
      {
        heapSize = 1024 * 200;     // 200kb
        break;
      }
    case MemoryMode::NORMAL_2X:
      {
        heapSize = normalSize * 2;   // * 2
        break;
      }
    case MemoryMode::NORMAL_4X:
      {
        heapSize = normalSize * 4;   // * 4
        break;
      }
    case MemoryMode::NORMAL:
    default:
      {
        heapSize = normalSize;
        break;
      }
    }

#endif

    m_pLocalDeviceHeap = GFL_CREATE_LOCAL_HEAP_NAME( pParentHeap, heapSize, gfl2::heap::HEAP_TYPE_EXP, false, "FieldTerrainHighBlock" );
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

#if PM_DEBUG
          m_HeapFreeSizeForLoadEnd = m_pLocalDeviceHeap->GetTotalFreeSize();
#endif // #if PM_DEBUG

          m_pModelResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pLocalDeviceHeapAllocator, &data );
        }

        // エンカウント草モデル
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData data;
          for( int i=0; i<ModelDataIndex::ENCOUNTGRASS_NUM; ++i ){
            if( dataAccessor.GetDataSize( ModelDataIndex::ENCOUNTGRASS_1 + i ) > 0 ){
              data.SetModelData( static_cast<c8*>( dataAccessor.GetData( ModelDataIndex::ENCOUNTGRASS_1 + i )) );
              m_pEncountGrassModelResource[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pLocalDeviceHeapAllocator, &data );
            }
            else
            {
              break;  //生成終わり。
            }
          }
        }

        // 高さコリジョンモデル
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
          resourceData.SetModelData( static_cast<c8*>( dataAccessor.GetData( ModelDataIndex::HEIGHT_COLLISION )), dataAccessor.GetDataSize( ModelDataIndex::HEIGHT_COLLISION ) ); 
          m_pHeightCollisionModelResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pLocalDeviceHeapAllocator, &resourceData );
        }

        // 壁コリジョンモデル
        {
          gfl2::fs::BinLinkerAccessor dataAccessorWall( dataAccessor.GetData( ModelDataIndex::WALL_COLLISION ) );
          u16 dataMax = dataAccessorWall.GetDataMax();

          if( dataMax > WallBinIndex::MAX )
          {
            // 開発中はありえるのでコメントアウト
            // GFL_ASSERT( 0 ); 
            dataMax = WallBinIndex::MAX;
          }

          for( u32 i = 0; i < dataMax; i++ )
          {
            gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
            resourceData.SetModelData( static_cast<c8*>( dataAccessorWall.GetData( i ) ), dataAccessorWall.GetDataSize( i ) );
            m_pWallCollisionModelResourceTable[ i ] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pLocalDeviceHeapAllocator, &resourceData );
          }
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

#if PM_DEBUG
    size_t datSize;
    if( m_pFileManager->GetArcFile( arcId )->GetRealDataSize( &datSize, datId, m_pLocalDeviceHeap ).IsSuccess() )
    {
      GFL_RELEASE_PRINT( "[FLD] Block Load Start x=%d z=%d datSize=%08.3f (KB) \n", x, z, datSize / 1024.0f );
      m_BinaryCompressSize = datSize;
    }
#endif

    // リクエスト生成
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId      = arcId;
    req.datId      = datId;
    req.ppBuf      = &m_pBinary;
    req.heapForBuf = m_pLocalDeviceHeap;
    req.align      = 128; // テクスチャは128バイトアラインメント
    req.heapForReq = m_pLocalDeviceHeap->GetLowerHandle();
    req.heapForCompressed = m_pLocalDeviceHeap->GetLowerHandle();

#if PM_DEBUG
    req.pRealReadSize = &m_BinarySize;
#endif

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
        GFL_ASSERT_STOP( 0 ); // ロード中にアンロードはケアのしようが無い不具合
        return;
      }
    }

    // リソースノードとバッファを開放
    GFL_SAFE_DELETE( m_pModelResource );

    for( u32 i = 0; i < WallBinIndex::MAX; i++ )
    {
      GFL_SAFE_DELETE( m_pWallCollisionModelResourceTable[ i ] );
    }

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

  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetWallCollisionModelResourceNode( WallBinIndex::Value index )
  {
    return m_pWallCollisionModelResourceTable[ index ];
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

#if PM_DEBUG

  void DebugDumpBinSize( void )
  {
    gfl2::fs::BinLinkerAccessor dataAccessor( m_pBinary );

    s32 binDataSizeTable[ ModelDataIndex::MAX ];
    s32 binDataSizeTotal = 0;
    for( u32 i = 0; i < ModelDataIndex::MAX; i++ )
    {
      binDataSizeTable[i] = dataAccessor.GetDataSize( i );
      binDataSizeTotal += binDataSizeTable[i];
    }

    GFL_RELEASE_PRINT( "----------------------------------\n" );
    GFL_RELEASE_PRINT( "[FLD] Block \"Bin\" Size Dump ( x=%2d z=%2d )\n", m_X, m_Z );
    GFL_RELEASE_PRINT( " TotalSize = %08.3f (KB) \n", binDataSizeTotal / 1024.0f );
    GFL_RELEASE_PRINT( " HeapSize = %08.3f / %08.3f (KB) \n", ( m_pLocalDeviceHeap->GetTotalSize() - m_HeapFreeSizeForLoadEnd ) / 1024.0f, m_pLocalDeviceHeap->GetTotalSize() / 1024.0f );
    for( u32 i = 0; i < ModelDataIndex::MAX; i++ )
    {
      GFL_RELEASE_PRINT( " %s = %08.3f (KB) \n", sc_BinNameTable[i], binDataSizeTable[i] / 1024.0f );
    }
    GFL_RELEASE_PRINT( "----------------------------------\n" );
  }

  void DebugDumpInstanceSize( void )
  {
    size_t totalSize = 0;
    for( u32 i = 0; i < ModelDataIndex::MAX; i++ )
    {
      totalSize += m_DebugInstaceDataTable[i].size;
    }

    GFL_RELEASE_PRINT( "----------------------------------\n" );
    GFL_RELEASE_PRINT( "[FLD] Block \"Instace\" Dump ( x=%2d z=%2d )\n", m_X, m_Z );
    GFL_RELEASE_PRINT( " TotalSize = %08.3f (KB) \n", totalSize / 1024.0f );
    GFL_RELEASE_PRINT( " HeapSize = %08.3f / %08.3f (KB) \n", ( m_pLocalDeviceHeap->GetTotalSize() - m_pLocalDeviceHeap->GetTotalFreeSize()) / 1024.0f, m_pLocalDeviceHeap->GetTotalSize() / 1024.0f );
    for( u32 i = 0; i < ModelDataIndex::MAX; i++ )
    {
      INSTANCE_DATA* pData = &m_DebugInstaceDataTable[ i ];
      GFL_RELEASE_PRINT( " %s size = %08.3f (KB) poly = %05d vetex = %05d material = %03d mesh = %03d drawTag = %03d \n", 
        sc_BinNameTable[i], pData->size / 1024.0f, pData->polygonNum, pData->vertexNum, pData->materialNum, pData->meshNum, pData->drawTagNum
        );
    }
    
    GFL_RELEASE_PRINT( " ANIM            size = %08.3f (KB) \n", m_DebugAnimeInstanceSize / 1024.0f );
    GFL_RELEASE_PRINT( "----------------------------------\n" );
  }

  void DebugDumpInstanceSize( u32 dataIndex )
  {
    INSTANCE_DATA* pData = &m_DebugInstaceDataTable[ dataIndex ];
    GFL_RELEASE_PRINT( "[%02d][%02d] %s size = %08.3f (KB) poly = %05d vetex = %05d material = %03d mesh = %03d drawTag = %03d \n",
      m_X, m_Z,
      sc_BinNameTable[dataIndex], pData->size / 1024.0f, pData->polygonNum, pData->vertexNum, pData->materialNum, pData->meshNum, pData->drawTagNum
      );
  }

  void DebugDumpForIkuta( void )
  {
    gfl2::fs::BinLinkerAccessor dataAccessor( m_pBinary );

    s32 binDataSizeTable[ ModelDataIndex::MAX ];
    s32 binDataSizeTotal = 0;
    for( u32 i = 0; i < ModelDataIndex::MAX; i++ )
    {
      binDataSizeTable[i] = dataAccessor.GetDataSize( i );
      binDataSizeTotal += binDataSizeTable[i];
    }

    GFL_RELEASE_PRINT( "Block,%02d,%02d,%d,",
      m_X,
      m_Z,
      m_pLocalDeviceHeap->GetTotalSize()
      );

    // bin
    GFL_RELEASE_PRINT( "%d,", binDataSizeTotal );
    for( u32 i = 0; i < ModelDataIndex::MAX; i++ )
    {
      GFL_RELEASE_PRINT( "%d,", binDataSizeTable[i] );
    }

    // ins
    {
      size_t totalSize = 0;
      for( u32 i = 0; i < ModelDataIndex::MAX; i++ )
      {
        totalSize += m_DebugInstaceDataTable[i].size;
      }
      
      GFL_RELEASE_PRINT( "%d,", totalSize );
      for( u32 i = 0; i < ModelDataIndex::MAX; i++ )
      {
        GFL_RELEASE_PRINT( "%d,", m_DebugInstaceDataTable[i].size );
      }

      // High
      INSTANCE_DATA* pData = &m_DebugInstaceDataTable[ ModelDataIndex::HIGH ];
      GFL_RELEASE_PRINT( "%d,%d,%d,%d,", m_DebugAnimeInstanceSize, pData->polygonNum, pData->vertexNum, pData->materialNum );
    }

    GFL_RELEASE_PRINT("\n");
  }

  void DebugDumpAnimeInstanceSize( void )
  {
    GFL_RELEASE_PRINT( "[%02d][%02d] ANIM            size = %08.3f (KB) \n", 
      m_X, m_Z,
      m_DebugAnimeInstanceSize / 1024.0f );
  }

  void DebugAnimeInstanceSizeStart( void )
  {
    m_DebugAnimeInstanceSize = m_pLocalDeviceHeap->GetTotalFreeSize();
  }
  void DebugAnimeInstanceSizeEnd( void )
  {
    m_DebugAnimeInstanceSize = m_DebugAnimeInstanceSize - m_pLocalDeviceHeap->GetTotalFreeSize();
  }

  void DebugInstanceSizeStart( u32 dataIndex )
  {
    m_DebugInstaceDataTable[ dataIndex ].size = m_pLocalDeviceHeap->GetTotalFreeSize();
  }
  void DebugInstanceSizeEnd( u32 dataIndex )
  {
    m_DebugInstaceDataTable[ dataIndex ].size = m_DebugInstaceDataTable[ dataIndex ].size - m_pLocalDeviceHeap->GetTotalFreeSize();
  }

  void DebugSetPolygonNum( u32 dataIndex, s32 polygonNum )
  {
    m_DebugInstaceDataTable[ dataIndex ].polygonNum = polygonNum;
  }

  void DebugSetVertexNum( u32 dataIndex, s32 vertexNum )
  {
    m_DebugInstaceDataTable[ dataIndex ].vertexNum = vertexNum;
  }

  void DebugSetMaterialNum( u32 dataIndex, s32 materialNum )
  {
    m_DebugInstaceDataTable[ dataIndex ].materialNum = materialNum;
  }

  void DebugSetMeshNum( u32 dataIndex, s32 meshNum )
  {
    m_DebugInstaceDataTable[ dataIndex ].meshNum = meshNum;
  }

  void DebugSetDrawTagNum( u32 dataIndex, s32 drawTagNum )
  {
    m_DebugInstaceDataTable[ dataIndex ].drawTagNum = drawTagNum;
  }

  void DebugGetInfo( DEBUG_INFO* pOut ) const
  {
    pOut->heapFreeSize = m_pLocalDeviceHeap->GetTotalFreeSize();
    pOut->heapSize = m_pLocalDeviceHeap->GetTotalSize();
    pOut->binarySize = m_BinarySize;
    pOut->binaryCompressSize = m_BinaryCompressSize;
  }

#endif // #if PM_DEBUG

private:
  UnitState::Value m_UnitState;
  b32 m_IsPlanMode;
  u32 m_X;
  u32 m_Z;
  s32 m_UnitID;

#if PM_DEBUG
  s32 m_HeapFreeSizeForLoadEnd;

  struct INSTANCE_DATA
  {
    size_t size;
    u32 polygonNum;
    u32 vertexNum;
    u32 materialNum;
    u32 meshNum;
    u32 drawTagNum;
  };

  INSTANCE_DATA m_DebugInstaceDataTable[ ModelDataIndex::MAX ];
  size_t        m_DebugAnimeInstanceSize;

  size_t        m_BinaryCompressSize;   // バイナリ(圧縮時)
  size_t        m_BinarySize;           // バイナリ(解凍時

#endif // #if PM_DEBUG

  gfl2::fs::AsyncFileManager*                                 m_pFileManager;
  void*                                                       m_pBinary;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pModelResource;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pHeightCollisionModelResource;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pWallCollisionModelResourceTable[ WallBinIndex::MAX ];
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pEncountGrassModelResource[ModelDataIndex::ENCOUNTGRASS_NUM];

  gfl2::heap::HeapBase* m_pLocalDeviceHeap;                   // 専用のヒープ、寿命も管理する　
  System::nijiAllocator* m_pLocalDeviceHeapAllocator;      // 専用のアロケーター、寿命も管理する　
};

class TerrainFactory::BgPartsUnit
{
public:

  struct UnitState
  {
    enum Value
    {
      // NOT_RUNNING = 0,    // 動いていない
      LOADING = 0,            // ロード中
      COMPLETION,
      MAX
    };
  };

  BgPartsUnit( gfl2::heap::HeapBase* m_pDeviceHeap, System::nijiAllocator* pDeviceHeapAllocator, u32 arcId, u32 no, gfl2::fs::AsyncFileManager* pFileManager ) :
    m_UnitState( UnitState::LOADING )
    ,m_No( no )
    ,m_pBinary( NULL )
    ,m_pModelResource( NULL )
    ,m_pFileManager( pFileManager )
    ,m_pDeviceHeapAllocator( )
  {
    // リクエスト生成
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId      = arcId;
    req.datId      = no;
    req.ppBuf      = &m_pBinary;
    req.heapForBuf = m_pDeviceHeap;
    req.align      = 128; // テクスチャは128バイトアラインメント
    req.heapForReq = m_pDeviceHeap->GetLowerHandle();
    req.heapForCompressed = NULL;

    m_pFileManager->AddArcFileLoadDataReq( req );
    m_UnitState = UnitState::LOADING;
    m_pDeviceHeapAllocator = pDeviceHeapAllocator;
  }

  ~BgPartsUnit( void )
  {
    GFL_SAFE_DELETE( m_pModelResource );
    GflHeapSafeFreeMemory( m_pBinary );
  }

  void Update( void )
  {
    switch( m_UnitState )
    {
    case UnitState::LOADING:
      {

        if( m_pFileManager->IsArcFileLoadDataFinished((void**)&m_pBinary) )
        {
          // リソース作成
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack data;
          data.SetModelData( static_cast<c8*>( m_pBinary ) );
          m_pModelResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pDeviceHeapAllocator, &data );

          m_UnitState = UnitState::COMPLETION;
        }

        break;
      }
    }
  }

  u32 GetNo( void )
  {
    return m_No;
  }

  b32 IsLoading( void ) const
  {
    return ( m_UnitState == UnitState::LOADING );
  }

  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetModelResourceNode( void )
  {
    return m_pModelResource;
  }


private:

  // 内部制御
  UnitState::Value    m_UnitState;
  u32                 m_No;

  void*                                                       m_pBinary;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pModelResource;


  // 外部オブジェクト
  gfl2::fs::AsyncFileManager* m_pFileManager;
  System::nijiAllocator*      m_pDeviceHeapAllocator;      // アロケーター

};


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TerrainFactory::TerrainFactory( void ) :

  // 制御
  m_pTerrainLayoutData( NULL )
  ,m_LocalAnimationFrameSize( 0 )
  ,m_OneDayAnimationFrameSize( 0 )


  // オブジェクト
  ,m_pHighModelUnitTable( NULL )
  ,m_HighModelUnitNum( 0 )
  ,m_HighModelUnitRunningNum( 0 )
  ,m_pBgPartsUnitList( NULL )

  // リソースデータ
  ,m_pTextureShaderResourceRootNode( NULL )
  ,m_pHightModelResourceTable( NULL )
  ,m_ppMiddleModelResourceTable( NULL )
  ,m_MiddleModelResourceNum( 0 )
  ,m_ppDistantViewModelResourceTable( NULL )
  ,m_DistantViewModelResourceNum( 0 )
  ,m_pLocalAnimationRootNode( NULL )
  ,m_pOneDayAnimationRootNode( NULL )

  // 外部オブジェクト
  ,m_pFileManager( NULL )
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
void TerrainFactory::Initialize( const InitializeDescription& rInitDesc )
{
  m_pFileManager = rInitDesc.m_pFileManager;
  m_pDeviceHeap = rInitDesc.m_pDeviceHeap;
  m_pAllocator = rInitDesc.m_pAllocator;
  m_pTerrainLayoutData = rInitDesc.m_pTerrainLayoutData;
  m_pEnvManager = rInitDesc.m_pEnvManager;
  m_pRenderingPipeLine = rInitDesc.m_pRenderingPipeLine;


  // BgParts用Garcを開く @note 重いならスクリプトに移動したい
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId				= ARCID_FIELD_BG_PARTS;
    req.heapForFile	= rInitDesc.m_pSystemHeap;
    req.heapForReq	= rInitDesc.m_pSystemHeap->GetLowerHandle();
    m_pFileManager->AddArcFileOpenReq( req );
  }

  // Highモデル用Garcを開く
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId				= ARCID_FIELD_TERRAIN_HIGH_MODEL;
    req.heapForFile	= rInitDesc.m_pSystemHeap;
    req.heapForReq	= rInitDesc.m_pSystemHeap->GetLowerHandle();
    m_pFileManager->AddArcFileOpenReq( req );
  }

  // BgParts管理リストを生成
  m_pBgPartsUnitList = GFL_NEW( rInitDesc.m_pSystemHeap ) BgPartsUnitList( rInitDesc.m_pSystemHeap );

  // Highモデル用生成Unitを生成
  {
    m_HighModelUnitNum = rInitDesc.m_HighModleWorkNum;
    m_HighModelUnitRunningNum = 0;
    m_pHighModelUnitTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) HighModelUnit[ m_HighModelUnitNum ];

    HighModelUnit* pTarget = m_pHighModelUnitTable;
    for( u32 i = 0; i < m_HighModelUnitNum; i++, pTarget++ )
    {
      pTarget->Initialize(
        rInitDesc.m_pHighModelDeviceHeap,
        rInitDesc.m_pFileManager,
        i,   // 添え字演算子でアクセスするため、順番にIDを割り振る
        rInitDesc.m_MemoryMode,
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
            m_ppMiddleModelResourceTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( rInitDesc.m_pAllocator, &resourceData );
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
            m_ppDistantViewModelResourceTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( rInitDesc.m_pAllocator, &resourceData );
          }
        }
      }
    }
  } // 遠景モデルリソース生


  //
  // 共用リソース生成
  //
  m_pTextureShaderResourceRootNode = ResourceTopNode::CreateNode( m_pAllocator );
  gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pTextureShaderResourceRootNode );

  // テクスチャリソース生成
  {
    if( rInitDesc.m_pAreaResourceDataAccessor->GetDataSize( AreaResourceDataIndex::TEXTURE ) > 0 )
    {
      void* pData = rInitDesc.m_pAreaResourceDataAccessor->GetData( AreaResourceDataIndex::TEXTURE );
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );

      for( u32 i = 0; i < dataAccessor.GetDataMax(); i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	data;
        data.SetTextureData( static_cast<c8*>( dataAccessor.GetData( i ) ) );
        //m_pTextureShaderResourceRootNodeの直下にデータを繋げる
        gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( rInitDesc.m_pAllocator, &data, m_pTextureShaderResourceRootNode );
      }
    }
  }

  // シェーダーリソース生成
  {
    if( rInitDesc.m_pAreaResourceDataAccessor->GetDataSize( AreaResourceDataIndex::SHADER ) > 0 )
    {
      void* pData = rInitDesc.m_pAreaResourceDataAccessor->GetData( AreaResourceDataIndex::SHADER );
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );

      for( u32 i = 0; i < dataAccessor.GetDataMax(); i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryShdData data;
        data.SetShaderData( static_cast<c8*>( dataAccessor.GetData( i ) ) );
        //m_pTextureShaderResourceRootNodeの直下にデータを繋げる
        gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( rInitDesc.m_pAllocator, &data, m_pTextureShaderResourceRootNode );
      }
    }
  }

  m_pTextureShaderResourceRootNode->Freeze(rInitDesc.m_pAllocator);

  // ローカルループアニメーション生成
  {
    if( rInitDesc.m_pAreaResourceDataAccessor->GetDataSize( AreaResourceDataIndex::ANIME_LOCAL ) > 0 )
    {
      void* pData = rInitDesc.m_pAreaResourceDataAccessor->GetData( AreaResourceDataIndex::ANIME_LOCAL );
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );
    

      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData data;
      data.SetMotionData( static_cast<c8*>( dataAccessor.GetData( 0 ) ) );  // nijiではエリアに必ず1個、アニメが無い場合はANIME_LOCAL自体が0バイトになっている
      m_pLocalAnimationRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( rInitDesc.m_pAllocator, &data );

      // アニメーションフレーム数取得
      gfl2::renderingengine::scenegraph::DagNode* pNode = m_pLocalAnimationRootNode->GetChild();
      if( pNode )
      {
        gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pAnimationResourceNode = pNode->SafeCast<gfl2::renderingengine::scenegraph::resource::AnimationResourceNode>();
        if( pAnimationResourceNode )
        {
          m_LocalAnimationFrameSize = pAnimationResourceNode->GetFrameSize();
        }
      }
    }
  }

  // 1日同期アニメーション
  {
    if( rInitDesc.m_pAreaResourceDataAccessor->GetDataSize( AreaResourceDataIndex::ANIME_ONE_DAY ) > 0 )
    {
      void* pData = rInitDesc.m_pAreaResourceDataAccessor->GetData( AreaResourceDataIndex::ANIME_ONE_DAY );
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );
    

      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData data;
      data.SetMotionData( static_cast<c8*>( dataAccessor.GetData( 0 ) ) );  // nijiではエリアに必ず1個、アニメが無い場合はANIME_ONE_DAY自体が0バイトになっている
      m_pOneDayAnimationRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( rInitDesc.m_pAllocator, &data );
      
      // アニメーションフレーム数取得
      gfl2::renderingengine::scenegraph::DagNode* pNode = m_pOneDayAnimationRootNode->GetChild();
      if( pNode )
      {
        gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pAnimationResourceNode = pNode->SafeCast<gfl2::renderingengine::scenegraph::resource::AnimationResourceNode>();
        if( pAnimationResourceNode )
        {
          m_OneDayAnimationFrameSize = pAnimationResourceNode->GetFrameSize();
        }
      }
    }
  }

}

/**
* @brief 初期化待ち処理
*
* @return true 完了 false 未完了
*/
b32 TerrainFactory::InitializeWait( void )
{
  // ARCファイルオープン待ち
  if( !m_pFileManager->IsArcFileOpenFinished( ARCID_FIELD_BG_PARTS ) )
  {
    return false;
  }
  if( !m_pFileManager->IsArcFileOpenFinished( ARCID_FIELD_TERRAIN_HIGH_MODEL ) )
  {
    return false;
  }

  return true;
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

  // テクスチャ、シェーダーリソース破棄
  {
    GFL_SAFE_DELETE( m_pTextureShaderResourceRootNode );
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

  // BgParts管理リストを破棄
  GFL_SAFE_DELETE( m_pBgPartsUnitList );

  // Highモデル用Garcを閉じる
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId				= ARCID_FIELD_TERRAIN_HIGH_MODEL;
    m_pFileManager->SyncArcFileClose( req );
  }

  // BgParts用Garcを閉じる
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId				= ARCID_FIELD_BG_PARTS;
    m_pFileManager->SyncArcFileClose( req );
  }

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
  // HighModelUnit更新
  HighModelUnit* pTarget = m_pHighModelUnitTable;
  for( u32 i = 0; i < m_HighModelUnitNum; i++, pTarget++ )
  {
    pTarget->Update();
  }

  // BgPartsUnit更新
  BgPartsUnitListIterator it( m_pBgPartsUnitList->Begin() );
  BgPartsUnitListIterator end( m_pBgPartsUnitList->End() );
  while( it != end )
  {
    (*it)->Update();
    it++;
  }
}

/**
* @brief 指定ブロック番号のhighモデルデータが有効かチェックする
*
* @param  x      x軸番号
* @param  z      z軸番号
* @param  replaceNo 置き換え番号
*
* @return true 有効   false 無効
*/
b32 TerrainFactory::CheckHighModelDataVailed( u32 x, u32 z, u16 replaceNo ) const
{
  // エリアに含まれているか？
  u32 datId = m_pTerrainLayoutData->GetHighModelDataIndex( x, z, replaceNo );
  if( datId == DATA_ID_INVALID_VALUE )
  {
    return false;
  }

  // データが0バイト(ダミー)になっていないか？ (製品版ではありえない)
  gfl2::fs::ArcFile::ARCID arcId = ARCID_FIELD_TERRAIN_HIGH_MODEL;
  const gfl2::fs::ArcFile* pFile = m_pFileManager->GetArcFile( arcId );

  if( pFile == NULL )
  {
    return false;
  }

  size_t dataSize;
  gfl2::fs::Result fsResult;
  fsResult = pFile->GetRealDataSize( &dataSize, datId, m_pDeviceHeap );

  // データサイズ0はダミーデータが登録されている
  if( fsResult.IsSuccess() == false || dataSize == 0 )
  {
    return false;
  }

  return true;
}

/**
* @brief 指定ブロック番号のhighモデルデータの非同期読み込み開始
*
* @param  x      x軸番号
* @param  z      z軸番号
* @param  replaceNo 置き換え番号
*
* @return 生成Unit番号(高速にアクセスできる、使わなくても良い)
*/
s32 TerrainFactory::LoadHighModelAsync( u32 x, u32 z, u16 replaceNo )
{
  HighModelUnit* pUnit = GetNotRunningHighModelUnit();
  if( pUnit )
  {
    u32 datId = m_pTerrainLayoutData->GetHighModelDataIndex( x, z, replaceNo );
    GFL_ASSERT_STOP( datId != DATA_ID_INVALID_VALUE );  // 事前にCheckHighModelDataVailedでチェックしている、ケアできないシーケンス破綻なのでここで止める
    pUnit->LoadAsync( x, z, ARCID_FIELD_TERRAIN_HIGH_MODEL, datId );
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
* @brief モデルを破棄する
*
* @param  ppModel 対象(削除後、NULLクリアされる)
*
* @return 無し
*/
void TerrainFactory::DeleteModel( poke_3d::model::BaseModel** ppModel )
{
  if( ppModel == NULL || (*ppModel) == NULL )
  {
    return;
  }

  // エッジ対象からはずす
  if( m_pRenderingPipeLine )
  {
    m_pRenderingPipeLine->RemoveEdgeRenderingTarget( (*ppModel)->GetModelInstanceNode() );
  }

  GFL_SAFE_DELETE( (*ppModel ) );
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
      m_pTextureShaderResourceRootNode
      );

    return pModelInstance;
  }

  return NULL;
}

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
  HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit && pUnit->IsCompletion() )
  {
#if PM_DEBUG
    pUnit->DebugInstanceSizeStart( ModelDataIndex::HIGH );
#endif // PM_DEBUG

    // モデルインスタンスノード生成
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = NULL;
    pModelInstance = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(
      pUnit->GetAllocator(),
      pUnit->GetModelResourceNode(),
      m_pTextureShaderResourceRootNode
      );


    // モデル生成
    poke_3d::model::BaseModel* pModel = GFL_NEW( pUnit->GetDeviceHeap() ) poke_3d::model::BaseModel;
    pModel->Create( pUnit->GetAllocator(), pUnit->GetDeviceHeap(), pModelInstance );

#if PM_DEBUG
    pUnit->DebugInstanceSizeEnd( ModelDataIndex::HIGH );
    pUnit->DebugSetVertexNum( ModelDataIndex::HIGH, pModelInstance->GetTotalVertexNum() );
    pUnit->DebugSetPolygonNum( ModelDataIndex::HIGH, pModelInstance->GetTotalPolygonNum() );
    pUnit->DebugSetMaterialNum( ModelDataIndex::HIGH, pModelInstance->GetMaterialNum() );
    pUnit->DebugSetMeshNum( ModelDataIndex::HIGH, pModelInstance->GetMeshNum() );
    pUnit->DebugSetDrawTagNum( ModelDataIndex::HIGH, pModelInstance->GetDrawTagNum() );
    
    pUnit->DebugDumpInstanceSize( ModelDataIndex::HIGH );
    pUnit->DebugAnimeInstanceSizeStart();
#endif // PM_DEBUG

    // ローカルループアニメーション適応
    if( m_pLocalAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pLocalAnimationRootNode, Block::AnimationSlotNo::LOCAL );
      pModel->SetAnimationLoop( false, Block::AnimationSlotNo::LOCAL );
      pModel->SetAnimationStepFrame( 0.0f, Block::AnimationSlotNo::LOCAL );
    }

    // 1日同期アニメーション適応
    if( m_pOneDayAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pOneDayAnimationRootNode, Block::AnimationSlotNo::ONE_DAY );
      pModel->SetAnimationLoop( false, Block::AnimationSlotNo::ONE_DAY );
      pModel->SetAnimationStepFrame( 0.0f, Block::AnimationSlotNo::ONE_DAY );
    }

#if PM_DEBUG
    pUnit->DebugAnimeInstanceSizeEnd();
    pUnit->DebugDumpAnimeInstanceSize();
#endif // PM_DEBUG
    
    // エッジ表示対象に登録
    if( m_pRenderingPipeLine )
    {
      m_pRenderingPipeLine->AddEdgeRenderingTarget( pModel->GetModelInstanceNode() );
    }

#if PM_DEBUG
    //pUnit->
    //DebugSetMaterialNum

#endif // PM_DEBUG

    return pModel;
  }

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
#if PM_DEBUG
      pUnit->DebugInstanceSizeStart( ModelDataIndex::HEIGHT_COLLISION );
#endif // PM_DEBUG
      gfl2::collision::CollisionModel* pClollisionModel = GFL_NEW( pUnit->GetDeviceHeap() ) gfl2::collision::CollisionModel();
      pClollisionModel->Initialize( pResource );

#if PM_DEBUG
      pUnit->DebugInstanceSizeEnd( ModelDataIndex::HEIGHT_COLLISION );
      pUnit->DebugSetPolygonNum( ModelDataIndex::HEIGHT_COLLISION, pClollisionModel->GetCollisionMeshNode()->GetCollisionTriangleCount() );
#endif // PM_DEBUG

      return pClollisionModel;
    }
  }

  return NULL;
}

/**
* @brief 指定ブロック番号の壁コリジョンモデルを生成する
*
* @param  x                 x軸番号
* @param  z                 z軸番号
* @param  ppCollisionModel  生成したインスタンス格納先[ WallBinIndex::MAX ] NULLが入ることもある
*
* @return ロードが行われていない場合はfalseを返す
*/
b32 TerrainFactory::CreateWallCollisionModel( u32 x, u32 z, gfl2::collision::CollisionModel** ppCollisionModel )
{
  HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit && pUnit->IsCompletion() )
  {
#if PM_DEBUG
    u32 totalTriangleCount = 0;
    pUnit->DebugInstanceSizeStart( ModelDataIndex::WALL_COLLISION );
#endif // PM_DEBUG

    for( u32 i = 0; i < WallBinIndex::MAX; i++ )
    {
      gfl2::renderingengine::scenegraph::resource::ResourceNode* pResource( pUnit->GetWallCollisionModelResourceNode( static_cast< WallBinIndex::Value > ( i ) ) );
      if( pResource )
      {
        gfl2::collision::CollisionModel* pClollisionModel = GFL_NEW( pUnit->GetDeviceHeap() ) gfl2::collision::CollisionModel();
        pClollisionModel->Initialize( pResource );

        ppCollisionModel[ i ] = pClollisionModel;

#if PM_DEBUG
        totalTriangleCount += pClollisionModel->GetCollisionMeshNode()->GetCollisionTriangleCount();
#endif // PM_DEBUG

      }
    }

#if PM_DEBUG
    pUnit->DebugInstanceSizeEnd( ModelDataIndex::WALL_COLLISION );
    pUnit->DebugSetPolygonNum( ModelDataIndex::WALL_COLLISION, totalTriangleCount );
#endif // PM_DEBUG
    return true;
  }
  else
  {
    return false;
  }
}

/**
* @brief 指定ブロック番号のMiddleモデルを生成する
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 生成したインスタンス(不正な場合はNULLを返す)
*/
gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* TerrainFactory::CreateMiddleModelNode( u32 x, u32 z, u16 replaceNo )
{
  u32 dataId = m_pTerrainLayoutData->GetMiddleModelDataIndex( x, z, replaceNo );
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
    m_pTextureShaderResourceRootNode
    );

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
poke_3d::model::BaseModel* TerrainFactory::CreateMiddleModel( u32 x, u32 z, u16 replaceNo )
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = CreateMiddleModelNode( x, z, replaceNo );
  if( pModelInstance )
  {
    poke_3d::model::BaseModel* pModel = GFL_NEW( m_pDeviceHeap ) poke_3d::model::BaseModel;
    pModel->Create( m_pAllocator, m_pDeviceHeap, pModelInstance );

    // ローカルループアニメーション適応
    if( m_pLocalAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pLocalAnimationRootNode, Block::AnimationSlotNo::LOCAL );
      pModel->SetAnimationLoop( false, Block::AnimationSlotNo::LOCAL );
      pModel->SetAnimationStepFrame( 0.0f, Block::AnimationSlotNo::LOCAL );
    }

    // 1日同期アニメーション適応
    if( m_pOneDayAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pOneDayAnimationRootNode, Block::AnimationSlotNo::ONE_DAY );
      pModel->SetAnimationLoop( false, Block::AnimationSlotNo::ONE_DAY );
      pModel->SetAnimationStepFrame( 0.0f, Block::AnimationSlotNo::ONE_DAY );
    }

    // エッジ表示対象に登録
    if( m_pRenderingPipeLine )
    {
      m_pRenderingPipeLine->AddEdgeRenderingTarget( pModel->GetModelInstanceNode() );
    }

    return pModel;
  }

  return NULL;
}

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

    for( int i=0; i<ModelDataIndex::ENCOUNTGRASS_NUM; ++i )
    {
#if PM_DEBUG
      pUnit->DebugInstanceSizeStart( ModelDataIndex::ENCOUNTGRASS_1 + i );
#endif // PM_DEBUG

      gfl2::renderingengine::scenegraph::resource::ResourceNode * pEncountGrassResourece = pUnit->GetEncountGrassResourceNode( i );
      if(pEncountGrassResourece)
      {
        ppKusaModelTable[i] = Field::KusaModel::FieldKusaModel::CreateKusaModelInstanceNode( pUnit->GetAllocator(), pEncountGrassResourece, m_pTextureShaderResourceRootNode );
        ppKusaModelTable[i]->InitKusaShader( pUnit->GetAllocator() );
      
        // エッジ表示対象に登録
        if( ppKusaModelTable[i] )
        {
          m_pRenderingPipeLine->AddEdgeRenderingTarget( ppKusaModelTable[i] );
        }

#if PM_DEBUG
        pUnit->DebugSetVertexNum( ModelDataIndex::ENCOUNTGRASS_1 + i, ppKusaModelTable[i]->GetTotalVertexNum() );
        pUnit->DebugSetPolygonNum( ModelDataIndex::ENCOUNTGRASS_1 + i, ppKusaModelTable[i]->GetTotalPolygonNum() );
#endif // PM_DEBUG
      }
      else
      {
        ppKusaModelTable[i] = NULL;
      }

#if PM_DEBUG
      pUnit->DebugInstanceSizeEnd( ModelDataIndex::ENCOUNTGRASS_1 + i );
#endif // PM_DEBUG

    }
  }

  return ppKusaModelTable;
}

/**
* @brief 指定ブロック番号の草モデルを削除する
*
* @param  pppModelTable 対象
*
* @return 無し
*/
void TerrainFactory::DeleteEncountGrassModelTable( KusaModel::FieldKusaModel*** pppModelTable )
{
  for( int i=0; i<ModelDataIndex::ENCOUNTGRASS_NUM; ++i )
  {
    if( (*pppModelTable)[i] )
    {
      // エッジ表示対象から解除
      if( m_pRenderingPipeLine )
      {
        m_pRenderingPipeLine->RemoveEdgeRenderingTarget( (*pppModelTable)[i] );
      }

      GFL_SAFE_DELETE( (*pppModelTable)[i] );
    }
  }

  GFL_SAFE_DELETE_ARRAY( (*pppModelTable) );
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
poke_3d::model::BaseModel* TerrainFactory::CreateDistantViewModel( u32 no )
{
  if( no >= m_DistantViewModelResourceNum || m_ppDistantViewModelResourceTable[ no ] == NULL )
  {
    return NULL;
  }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = NULL;
  pModelInstance = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pAllocator,
    m_ppDistantViewModelResourceTable[ no ],
    m_pTextureShaderResourceRootNode
    );
  
  poke_3d::model::BaseModel* pModel = GFL_NEW( m_pDeviceHeap ) poke_3d::model::BaseModel;
  pModel->Create( m_pAllocator, m_pDeviceHeap, pModelInstance );

  // ローカルループアニメーション適応
  if( m_pLocalAnimationRootNode )
  {
    pModel->ChangeAnimationByResourceNode( m_pLocalAnimationRootNode, Block::AnimationSlotNo::LOCAL );
    pModel->SetAnimationLoop( false, Block::AnimationSlotNo::LOCAL );
    pModel->SetAnimationStepFrame( 0.0f, Block::AnimationSlotNo::LOCAL );
  }

  // 1日同期アニメーション適応
  if( m_pOneDayAnimationRootNode )
  {
    pModel->ChangeAnimationByResourceNode( m_pOneDayAnimationRootNode, Block::AnimationSlotNo::ONE_DAY );
    pModel->SetAnimationLoop( false, Block::AnimationSlotNo::ONE_DAY );
    pModel->SetAnimationStepFrame( 0.0f, Block::AnimationSlotNo::ONE_DAY );
  }

  // エッジ表示対象に登録
  if( m_pRenderingPipeLine )
  {
    m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelInstance );
  }

  return pModel;
}

/**
* @brief 遠景モデルを削除する
*
* @param  ppNode 対象
*
* @return 無し
*/
void TerrainFactory::DeleteDistantViewModel( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode** ppNode )
{
  // エッジ表示対象から解除
  if( m_pRenderingPipeLine )
  {
    m_pRenderingPipeLine->RemoveEdgeRenderingTarget( (*ppNode) );
  }

  GFL_SAFE_DELETE( (*ppNode) );
}

/**
* @brief 遠景モデル数を取得
* @note アッパーでは配置情報に持たせたほうが良い
*
* @return 遠景モデル数
*/
u32 TerrainFactory::GetDistantViewModelNum( void ) const
{
  return m_DistantViewModelResourceNum;
}


/**
* @brief 生成ユニットが保持するデバイスヒープを取得
*
* @param  unitNo ユニット番号
*
* @return デバイスヒープ
*/
gfl2::heap::HeapBase* TerrainFactory::GetUnitDeviceHeap( s32 unitNo )
{
  if( unitNo < 0 || unitNo >= m_HighModelUnitNum )
  {
    return NULL;
  }

  return m_pHighModelUnitTable[ unitNo ].GetDeviceHeap();
}

System::nijiAllocator* TerrainFactory::GetUnitAllocator( s32 unitNo )
{
  if( unitNo < 0 || unitNo >= m_HighModelUnitNum )
  {
    return NULL;
  }

  return m_pHighModelUnitTable[ unitNo ].GetAllocator();
}


//
// BGパーツ
//

/**
* @brief 指定番号のBgPartsが有効かチェックする
*
* @param  no  番号
*
* @return true 有効   false 無効
*/
b32 TerrainFactory::CheckBgPartsNoVailed( u32 no ) const
{
  u32 dataMax;
  if( m_pFileManager->GetArcFile( ARCID_FIELD_BG_PARTS )->GetDataCount( &dataMax ).IsSuccess() )
  {
    if( no < dataMax )
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}


/**
* @brief 指定データ番号のBgPartsの非同期読み込み開始
*
* @param  no                データ番号
* @param  pDeviceHeap       読み込み及びインスタンス生成に使用するヒープ
* @param  pDeviceAllocator  インスタンス生成に使用するアロケータ
*
* @return 成否( データ番号が不正値、既に存在する場合はfalseを返し何もしない )
*/
b32 TerrainFactory::LoadBgPartsAsync( u32 no, gfl2::heap::HeapBase* pDeviceHeap, System::nijiAllocator* pDeviceAllocator )
{
  // 無効番号
  if( CheckBgPartsNoVailed( no ) == false )
  {
    return false;
  }

  // 既にある
  if( SearchBgPartsUnit( no ) != NULL )
  {
    return false;
  }

  // 生成ユニットを作成＆ロード開始
  BgPartsUnit* pUnit = GFL_NEW( pDeviceHeap ) BgPartsUnit( pDeviceHeap, pDeviceAllocator, ARCID_FIELD_BG_PARTS, no, m_pFileManager );
  m_pBgPartsUnitList->PushBack( pUnit, pDeviceHeap );
  return true;
}

/**
* @brief 指定番号のBgPartを破棄
*        開放チェックは呼び出し側で行ってください
*
* @param  no  番号
*
* @return 無し
*/
void TerrainFactory::UnLoadBgParts( u32 no )
{  
  BgPartsUnit* pUnit = SearchBgPartsUnit( no );
  if( pUnit == NULL )
  {
    return;
  }

  if( pUnit->IsLoading() )
  {
    GFL_ASSERT_STOP( 0 );
  }

  // リストから取り出し、破棄する
  m_pBgPartsUnitList->Remove( pUnit );
  GFL_DELETE( pUnit );
}

/**
* @brief 指定番号のBgPartを生成
*
* @param  no  番号
*
* @return 生成したインスタンス(不正な場合はNULLを返す)
*/
poke_3d::model::BaseModel* TerrainFactory::CreateBgParts( u32 no,  gfl2::heap::HeapBase* pDeviceHeap, System::nijiAllocator* pDeviceAllocator )
{
  BgPartsUnit* pUnit = SearchBgPartsUnit( no );
  if( pUnit && pUnit->IsLoading() == false )
  {
    // モデルインスタンスノード生成
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = NULL;
    pModelInstance = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(
      pDeviceAllocator,
      pUnit->GetModelResourceNode()
      );

    // モデルクラス生成
    poke_3d::model::BaseModel* pModel = GFL_NEW( pDeviceHeap ) poke_3d::model::BaseModel;
    pModel->Create( pDeviceAllocator, pDeviceHeap, pModelInstance );

    // ローカルループアニメーション適応
    if( m_pLocalAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pLocalAnimationRootNode, Block::AnimationSlotNo::LOCAL );
      pModel->SetAnimationLoop( false, Block::AnimationSlotNo::LOCAL );
      pModel->SetAnimationStepFrame( 0.0f, Block::AnimationSlotNo::LOCAL );
    }

    // 1日同期アニメーション適応
    if( m_pOneDayAnimationRootNode )
    {
      pModel->ChangeAnimationByResourceNode( m_pOneDayAnimationRootNode, Block::AnimationSlotNo::ONE_DAY );
      pModel->SetAnimationLoop( false, Block::AnimationSlotNo::ONE_DAY );
      pModel->SetAnimationStepFrame( 0.0f, Block::AnimationSlotNo::ONE_DAY );
    }
    
    // エッジ表示対象に登録
    if( m_pRenderingPipeLine )
    {
      m_pRenderingPipeLine->AddEdgeRenderingTarget( pModel->GetModelInstanceNode() );
    }

    return pModel;
  }

  return NULL;
}

/**
* @brief 指定番号のBgPartsの読み込み状態をチェックする
*
* @param  no  番号
*
* @return 生成ユニットの状態
*/
TerrainFactory::UnitState::Value TerrainFactory::CheckBgPartsUnitState( u32 no ) const
{
  const BgPartsUnit* pUnit = SearchBgPartsUnit( no );
  if( pUnit == NULL )
  {
    return UnitState::NOT_EXIST;
  }

  if( pUnit->IsLoading() )
  {
    return UnitState::LOADING;
  }
  else
  {
    return UnitState::LOAD_COMPLETION;
  }
}

#if PM_DEBUG
void TerrainFactory::DebugDumpBlockBinSize( u32 x, u32 z )
{
  HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit && pUnit->IsCompletion() )
  {
    pUnit->DebugDumpBinSize();
  }
}
void TerrainFactory::DebugDumpBlockInstanceSize( u32 x, u32 z )
{
  HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit && pUnit->IsCompletion() )
  {
    pUnit->DebugDumpInstanceSize();
  }
}

void TerrainFactory::DebugDumpBlockForScript( u32 x, u32 z )
{
  HighModelUnit* pUnit = GetHighModelUnit( x, z );
  if( pUnit && pUnit->IsCompletion() )
  {
    pUnit->DebugDumpForIkuta();
  }
}

b32 TerrainFactory::DebugGetInfo(u32 x, u32 z, DEBUG_INFO* pOut) const
{
  const HighModelUnit* pUnit = GetHighModelUnit(x, z);
  if (pUnit && pUnit->IsCompletion())
  {
    pUnit->DebugGetInfo(pOut);
    return true;
  }
  else
  {
    return false;
  }

}


#endif // #if PM_DEBUG



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


//
// BGパーツ
//

/**
* @brief 指定データ番号のBgParts生成ユニットを取得する
*
* @param  no        データ番号
*
* @retval NULL 無い
* @return BgParts生成ユニット
*/
const TerrainFactory::BgPartsUnit* TerrainFactory::SearchBgPartsUnit( u32 no ) const
{
  BgPartsUnitListIterator it( m_pBgPartsUnitList->Begin() );
  BgPartsUnitListIterator end( m_pBgPartsUnitList->End() );
  while( it != end )
  {
    if( (*it)->GetNo() == no )
    {
      return (*it);
    }
    it++;
  }

  return NULL;
}

/**
* @brief 指定データ番号のBgParts生成ユニットを取得する
*
* @param  no        データ番号
*
* @retval NULL 無い
* @return BgParts生成ユニット
*/
TerrainFactory::BgPartsUnit* TerrainFactory::SearchBgPartsUnit( u32 no )
{
  BgPartsUnitListIterator it( m_pBgPartsUnitList->Begin() );
  BgPartsUnitListIterator end( m_pBgPartsUnitList->End() );
  while( it != end )
  {
    if( (*it)->GetNo() == no )
    {
      return (*it);
    }
    it++;
  }

  return NULL;
}

GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );
