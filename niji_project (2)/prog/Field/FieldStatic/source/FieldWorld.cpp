//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   FieldWorld.cpp
 *  @brief  ワールド管理クラス
 *  @author tomoya takahashi
 *  @date   2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <debug/include/gfl2_DebugPrint.h>
#include <System/include/HeapDefine.h>

#include "Field/FieldStatic/include/FieldWorld.h"
#include "Field/FieldStatic/include/Zone/ZoneCollision.h"

#include <arc_def_index/arc_def.h>

#include <niji_conv_header/field/world/world_id.h>

GFL_NAMESPACE_BEGIN(Field);


//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------
const gfl2::fs::ArcFile::ARCID WORLD_ARC_ID = ARCID_FIELD_WOLRD;
//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 */
World::World( gfl2::heap::HEAPID heapID ) : 
  m_WorldId(WORLD_ERROR),
  m_pHeap(NULL),
  m_pFileManager(NULL),
  m_pBuffer(NULL),
  m_Accessor(),
  m_pWorldData(NULL),
  m_pWorldInfo(NULL),
  m_pZoneInfoList(NULL),
  m_ZoneInfoNum(0),
  m_pAreaList(NULL),
  m_AreaNum(0),
  m_pZoneAreaData(NULL),
  m_pZoneAreaLocalHeap(NULL),
  m_pZoneCollisionDataList(NULL),
  m_pZoneAreaVertexArray(NULL)
{
  gfl2::heap::HeapBase *pResident = gfl2::heap::Manager::GetHeapByHeapId( heapID );

  m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME( pResident, 0x7000, gfl2::heap::HEAP_TYPE_EXP, false, "World" );
}

World::World( gfl2::heap::HeapBase* heap ) : 
  m_WorldId(WORLD_ERROR),
  m_pHeap(NULL),
  m_pFileManager(NULL),
  m_pBuffer(NULL),
  m_Accessor(),
  m_pWorldData(NULL),
  m_pWorldInfo(NULL),
  m_pZoneInfoList(NULL),
  m_ZoneInfoNum(0),
  m_pAreaList(NULL),
  m_AreaNum(0),
  m_pZoneAreaData(NULL),
  m_pZoneAreaLocalHeap(NULL),
  m_pZoneCollisionDataList(NULL),
  m_pZoneAreaVertexArray(NULL)
{
  m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME( heap, 0x7000, gfl2::heap::HEAP_TYPE_EXP, false, "World" );
}

/**
 *  @brief  デストラクタ
 */
World::~World()
{
  Terminate();
  GFL_DELETE_HEAP( m_pHeap );
}

/**
 *  @brief  初期化
 *  @param  worldId       ワールドID
 *  @param  p_heapBase    ヒープベース
 *  @param  p_fileManager ファイルマネージャー
 */
void World::Initialize( WorldID worldId, gfl2::fs::AsyncFileManager* p_fileManager )
{
  GFL_ASSERT( m_pFileManager == NULL ); // 多重初期化のチェック
  GFL_ASSERT_STOP( worldId != WORLD_ERROR );

  m_WorldId = worldId;
  m_pFileManager  = p_fileManager;

  gfl2::heap::HeapBase *pCompHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD );

  //! ファイル読み込み
  //---------------------------------------------------
  // ARCファイルオープン
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = WORLD_ARC_ID;
    openReq.heapForFile = pCompHeap;
    openReq.heapForArcSrc = pCompHeap->GetLowerHandle();
    openReq.heapForReq = pCompHeap->GetLowerHandle();
    m_pFileManager->AddArcFileOpenReq( openReq );
  }
  //---------------------------------------------------
  // ファイル読み込みリクエスト
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    loadReq.arcId = WORLD_ARC_ID;
    loadReq.datId = m_WorldId;
    loadReq.ppBuf = &m_pBuffer;
    loadReq.heapForBuf = pCompHeap;
    loadReq.heapForReq = pCompHeap->GetLowerHandle();
    loadReq.heapForCompressed = pCompHeap->GetLowerHandle();
    loadReq.pBufSize = NULL;
    loadReq.align = 4;
    m_pFileManager->AddArcFileLoadDataReq( loadReq );
  }

  //---------------------------------------------------
  // ZoneArea用のローカルヒープ生成
  //---------------------------------------------------
  m_pZoneAreaLocalHeap = GFL_CREATE_LOCAL_HEAP_NAME(
    m_pHeap,
    ZONEAREA_LOCAL_HEAP_SIZE,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    "WorldZoneArea"
    );

}

/**
 *  @brief  初期化処理
 */
bool World::InitializeWait()
{
  // 読み込み完了待ち
  if( !m_pFileManager->IsArcFileOpenFinished( WORLD_ARC_ID ) ){
    return false;
  }
  if( !m_pFileManager->IsArcFileLoadDataFinished( &m_pBuffer ) ){
    return false;
  }

  // アクセサーに登録
  m_Accessor.Initialize( m_pBuffer );

  // ワールドデータセットアップ
  m_pWorldData = static_cast<WorldDataHeader*>(m_Accessor.GetData(BIN_IDX_WORLD));

  // ゾーン領域データセットアップ
  m_pZoneAreaData = static_cast<ZoneAreaHeader*>(m_Accessor.GetData(BIN_IDX_ZONEAREA));

  // WorldInfoの初期化
  SetupWorldInfo();

  // ZoneInfoの初期化
  SetupZoneInfoList();

  // AreaListの初期化
  SetupAreaList();

  // ZoneAreaListの生成
  CreateZoneAreaList();

  return true;

}

/**
 *  @brief  ワールド情報の破棄
 */
void World::Terminate( void )
{
  if(m_pBuffer == NULL){ return; }

  // ZoneAreaList破棄
  TerminateZoneAreaList();

  // ローカルヒープ削除
  GFL_DELETE_HEAP( m_pZoneAreaLocalHeap );
  m_pZoneAreaLocalHeap = NULL;

  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = WORLD_ARC_ID;
    closeReq.heapForReq = m_pHeap->GetLowerHandle();
    m_pFileManager->SyncArcFileClose( closeReq );
  }

  GflHeapFreeMemory( m_pBuffer );
  m_pBuffer = NULL;


  m_WorldId = WORLD_ERROR;
  m_pFileManager = NULL;
}


/**
 *  @brief 現在のワールドIDを取得
 */
WorldID World::GetWorldID() const 
{
  return m_WorldId;
}

/**
 *  @brief  ワールドインフォを取得する
 */
const World::WorldInfo& World::GetWorldInfo() const
{
  return *m_pWorldInfo;
}

/**
 *  @brief  ゾーン数を取得する
 */
u32 World::GetZoneNum() const
{
  return m_ZoneInfoNum;
}

/**
 *  @brief  ゾーン情報を取得
 *  @param  idx インデックス
 */
const World::ZoneInfo& World::GetZoneInfo( u32 idx ) const
{
  if( idx < m_ZoneInfoNum ){
    return m_pZoneInfoList[idx];
  }
  GFL_ASSERT( idx < m_ZoneInfoNum );
  return m_pZoneInfoList[0];
}

/**
 *  @brief  ゾーン情報を取得
 *  @param  zoneID  ゾーンID
 */
const World::ZoneInfo& World::GetIDZoneInfo( ZoneID zoneID ) const
{
  for( u32 i=0; i<m_ZoneInfoNum; ++i )
  {
    if( m_pZoneInfoList[i].m_ZoneID == zoneID )
    {
      return m_pZoneInfoList[i];
    }
  }

  GFL_ASSERT( 0 );// Zoneが存在していない。
  return m_pZoneInfoList[0];
}

/**
 *  @brief ゾーンIDのZoneInfoテーブルインデックスを取得
 */
u32 World::GetZoneIDIndex( ZoneID zoneID ) const
{
  for( u32 i=0; i<m_ZoneInfoNum; ++i )
  {
    if( m_pZoneInfoList[i].m_ZoneID == zoneID )
    {
      return i;
    }
  }

  GFL_ASSERT( 0 );// Zoneが存在していない。
  return m_ZoneInfoNum;
}

/**
 *  @brief  指定したゾーンのエリア内Indexを取得
 *  @param  zoneID  ゾーンID
 *  @retval エリア内Index
 *
 *  ゾーン毎のデータをエリア単位でパックした場合に
 *  各ゾーンのデータにアクセスするにはエリア内Indexが必要。
 *  ただし、この関数で取得したIndexでエリアデータのBinLinkerにアクセスするには
 *  データのパック時にfield_convert\world\world\converter\GetAreaZoneList.rbが行うのと
 *  同じアルゴリズムでエリア内ゾーンリストを取得し、その順番でパックする必要がある。
*/
u32 World::GetZoneIndexInArea( ZoneID zoneID ) const
{
  s32 areaID = -1;
  u32 index = 0;

  for( u32 i=0; i<m_ZoneInfoNum; ++i, ++index )
  {
    if( m_pZoneInfoList[i].m_AreaID != areaID )
    {
      index = 0; //エリアが変わったらIndexをリセット
      areaID = m_pZoneInfoList[i].m_AreaID;
    }
    if( m_pZoneInfoList[i].m_ZoneID == zoneID )
    {
      return index;
    }
  }

  GFL_ASSERT( 0 );// Zoneが存在していない。
  return 0;
}

/**
 *  @brief  エリア数を取得
 */
u32 World::GetAreaNum() const
{
  return m_AreaNum;
}

/**
 *  @brief  エリアIDリストの取得
 */
const World::AreaInfo* World::GetAreaArray() const
{
  return m_pAreaList;
}

/**
 *  @brief エリアIDのm_IsDisplayBattlePokemonUsingAreaHeapを取得
 */
b32 World::IsAreaDisplayBattlePokemonUsingAreaHeap( AreaID areaID ) const
{
  u32 areaNum = GetAreaNum();
  const AreaInfo* cpInfo = GetAreaArray();

  for( u32 i=0; i<areaNum; ++i )
  {
    if( cpInfo[i].m_AreaID == areaID )
    {
      return (cpInfo[i].m_AreaMemType == AREA_MEM_LOW);
    }
  }

  GFL_ASSERT_MSG( 0, "areaID not exits [%d]\n", areaID );
  return false;
}

/**
 *  @brief エリアIDのメモリ設定を取得
 */
b32 World::IsAreaDisplayShopUIUsingAreaHeap( AreaID areaID ) const
{
  u32 areaNum = GetAreaNum();
  const AreaInfo* cpInfo = GetAreaArray();

  for( u32 i=0; i<areaNum; ++i )
  {
    if( cpInfo[i].m_AreaID == areaID )
    {
      return (cpInfo[i].m_AreaMemType == AREA_MEM_MIDDLE);
    }
  }

  GFL_ASSERT_MSG( 0, "areaID not exits [%d]\n", areaID );
  return false;
}

/**
 *  @brief  ゾーンヒットチェック
 */
const World::ZoneInfo* World::IsHitZoneInfo( const gfl2::math::Vector3& pos, b32 is_asseert_check ) const
{
  gfl2::math::Vector start, end;
  start.Set( pos.GetX(), 1000.0f, pos.GetZ() );
  end.Set( pos.GetX(), -1000.0f, pos.GetZ() );

  s32 zoneIndex = ZoneCollision::Check( start, end, m_pZoneCollisionDataList );
  if( (zoneIndex >= 0) && (zoneIndex < static_cast<s32>(m_ZoneInfoNum)) ) 
  {
    return &GetZoneInfo( zoneIndex );
  }
  else
  {
    if( is_asseert_check )
    {
      GFL_ASSERT( 0 );
    }
  }

  return NULL;
}

/**
*  @brief  マップジャンプできる？
*
*  @return  true できる false 出来ない
*/
b32 World::CanMapJump( void ) const
{
  if( m_pWorldInfo == NULL )
  {
    return false;
  }

  //最終段階でテストデータにはジャンプできないようにする。
  //@note テストワールドにはジャンプできないようにする。
  // @todo momiji開発開始のため、テストワールドを復帰。
#if 0
  switch( m_WorldId )
  {
  case W_TESTPL1701:
  case W_TESTPL1801:
  case W_TESTPL1901:
  case W_TESTPL2001:
  case W_TESTPL2101:
  case W_TESTPL2201:
  case W_TESTSP0101:
  case W_TESTSP0201:
  case W_TESTBG0101:
  case W_TESTBG0201:
  case W_TESTBG0301:
    return false;
  }
#endif

#if defined(GF_PLATFORM_CTR)
  if( m_pWorldInfo->m_TerrainMode == World::TerrainMode::PLAN_ONLY_WIN32 )
  {
    return false;
  }
#endif

  return true;
}

/**
 *  @brief  ワールドインフォのセットアップ
 */
void World::SetupWorldInfo()
{
  u32 addr = reinterpret_cast<u32>( m_pWorldData );
  m_pWorldInfo = reinterpret_cast<WorldInfo*>( addr + m_pWorldData->worldDataOffset );
}

/**
 *  @brief  ゾーンインフォリストのセットアップ
 */
void World::SetupZoneInfoList()
{
  u32 addr = reinterpret_cast<u32>( m_pWorldData );
  m_pZoneInfoList = reinterpret_cast<ZoneInfo*>( addr + m_pWorldData->zoneListOffset );
  m_ZoneInfoNum = (m_pWorldData->endOffset - m_pWorldData->zoneListOffset) / sizeof(ZoneInfo);

#if PM_DEBUG

  for( u32 i=0; i<m_ZoneInfoNum; ++i ){
    TOMOYA_PRINT( "AreaID[%d] ZoneID[%d]\n", m_pZoneInfoList[i].m_AreaID, m_pZoneInfoList[i].m_ZoneID );
  }
#endif
}

/**
 *  @brief  エリアリストのセットアップ
 */
void World::SetupAreaList()
{
  u32 addr = reinterpret_cast<u32>( m_pWorldData );
  m_pAreaList = reinterpret_cast<AreaInfo*>( addr + m_pWorldData->areaListOffset );
  m_AreaNum = (m_pWorldData->zoneListOffset - m_pWorldData->areaListOffset) / sizeof(AreaInfo);
}


/**
 *  @brief  ゾーン領域リストの生成
 */
void World::CreateZoneAreaList()
{
  // ゾーン数が一致するかチェック
  GFL_ASSERT( m_pZoneAreaData->zoneNum == this->GetZoneNum() );

  // ゾーンコリジョンデータリストを生成する
  {
    TOMOYA_PRINT( "=============== World ZoneArea ==================\n" );
    TOMOYA_PRINT( "VertexArray \n" );
    m_pZoneAreaVertexArray = GFL_NEW_ARRAY( m_pZoneAreaLocalHeap ) gfl2::math::Vector[m_pZoneAreaData->vertexNum];
    const f32 * cpVertexData = this->GetZoneAreaVertexArray();
    for( u32 i=0; i<m_pZoneAreaData->vertexNum; ++i )
    {
      u32 count = i*3;
      TOMOYA_PRINT( "index %d [%f,%f,%f]\n", i, cpVertexData[count+0], cpVertexData[count+1], cpVertexData[count+2] );
      m_pZoneAreaVertexArray[i].Set( cpVertexData[count+0], cpVertexData[count+1], cpVertexData[count+2] );
    }
  }
  
  // ゾーンコリジョンデータリスト生成
  {
    m_pZoneCollisionDataList = GFL_NEW_ARRAY( m_pZoneAreaLocalHeap ) gfl2::util::List<ZoneCollisionData*>( m_pZoneAreaLocalHeap, m_pZoneAreaData->zoneNum );
    for( u32 i=0; i<m_pZoneAreaData->zoneNum; ++i )
    {

      u32 indexListSize = this->GetZoneAreaZoneIndexArraySize(i);
      u32 triangleNum = indexListSize / 3;  // すべて3角形ポリ

      ZoneCollisionData* pZoneCollisionData = GFL_NEW( m_pZoneAreaLocalHeap ) ZoneCollisionData();
      pZoneCollisionData->m_pVertex = m_pZoneAreaVertexArray;
      pZoneCollisionData->m_pIndex  = this->GetZoneAreaZoneIndexArray(i);
      pZoneCollisionData->m_triangleCount = triangleNum;
      pZoneCollisionData->m_id      = i; // zoneIndexを入れている　ZoneIDではない。


#if PM_DEBUG
#if defined( DEBUG_ONLY_FOR_takahashi_tomoya )
      TOMOYA_PRINT( "Index %d ZoneIndexList size %d\n", i, indexListSize );
      for( u32 j=0; j<triangleNum; ++j ){
        s32 count = j*3;
        TOMOYA_PRINT( "[%d, %d, %d]\n", pZoneCollisionData->m_pIndex[ count + 0], pZoneCollisionData->m_pIndex[ count + 1], pZoneCollisionData->m_pIndex[ count + 2] );
      }
#endif
#endif // PM_DEBUG

      // 各ゾーンのコリジョンデータリスト生成
      m_pZoneCollisionDataList->PushBack(pZoneCollisionData);
    }
  }
}

/**
 *  @brief  ゾーン領域リストの破棄
 */
void World::TerminateZoneAreaList()
{
  if( m_pZoneCollisionDataList )
  {
    //子をすべて破棄
    gfl2::util::List<ZoneCollisionData*>::Iterator it = m_pZoneCollisionDataList->Begin();
    while( it != m_pZoneCollisionDataList->End() )
    {
      ZoneCollisionData* item = (*it);
      ++it;
      GFL_DELETE( item );
    }

  }
  GFL_SAFE_DELETE( m_pZoneCollisionDataList );
  GFL_SAFE_DELETE_ARRAY( m_pZoneAreaVertexArray );
}


/**
 *  @brief  ゾーン領域　頂点バッファポインタ取得
 */
const f32 * World::GetZoneAreaVertexArray( void ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pZoneAreaData);

  addr += m_pZoneAreaData->vertexArrayOffset;

  return reinterpret_cast<const f32*>( addr );
}

/**
 *  @brief  ゾーン領域　頂点インデックス数の取得
 *  @param  zoneIndex   ワールド内ゾーンインデックス
 *  @return 頂点インデックス配列の要素数
 */
u32 World::GetZoneAreaZoneIndexArraySize( u32 zoneIndex ) const
{
  if( zoneIndex < m_pZoneAreaData->zoneNum )
  {
    // オフセットからサイズを算出
    u32 start_offset  = m_pZoneAreaData->zoneIndexArrayOffset[zoneIndex];
    u32 end_offset = m_pZoneAreaData->fileEndOffset;
    if( zoneIndex + 1 < m_pZoneAreaData->zoneNum ){
      end_offset = m_pZoneAreaData->zoneIndexArrayOffset[zoneIndex + 1];
    }

    u32 datasize = end_offset - start_offset;
    TOMOYA_PRINT( "IndexList Size so %d eo %d size %d\n", start_offset, end_offset, datasize );
    return datasize / sizeof(u32);
  }
  else
  {
    GFL_ASSERT(0);
  }
  return 0;
}

/**
 *  @brief  ゾーン領域　頂点インデックス配列のアドレス取得
 *  @param  zoneIndex   ワールド内ゾーンインデックス
 *  @return 頂点インデックス配列のアドレス
 */
u32 * World::GetZoneAreaZoneIndexArray( u32 zoneIndex )
{
  uptr addr = reinterpret_cast<uptr>(m_pZoneAreaData);
  if( zoneIndex < m_pZoneAreaData->zoneNum )
  {
    addr  += m_pZoneAreaData->zoneIndexArrayOffset[zoneIndex];
    return reinterpret_cast<u32*>( addr );
  }
  else
  {
    GFL_ASSERT(0);
  }
  return NULL;
}


GFL_NAMESPACE_END(Field);
