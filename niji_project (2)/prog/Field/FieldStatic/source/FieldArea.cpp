//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldArea.cpp
 *	@brief  エリアデータ管理
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldStatic/include/FieldArea.h"
#include <arc_def_index/arc_def.h>

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#include "System/include/HeapSize.h"
#include "System/include/HeapDefine.h"
#include "System/include/nijiAllocator.h"
#include <niji_conv_header/field/area/field_area.h>

GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
const gfl2::fs::ArcFile::ARCID AREA_ARC_ID = ARCID_FIELD_AREA;  ///! エリアのアーカイブID
const u32 Area::AREA_DATA_BATTLE_POKEMON_USING_MAX = Area::AREA_DATA_SIZE_MAX - System::HeapSize::HEAP_FIELD_BATTLE_MODEL_HEAP_SIZE;  // バトルモデル表示にエリアヒープを使用する場合のOKサイズ
const u32 Area::AREA_DATA_JOINFESTA_USING_MAX = Area::AREA_DATA_SIZE_MAX - System::HeapSize::HEAP_FIELD_JOINFESTA_HEAP_SIZE;  // 
const u32 Area::AREA_DATA_MIDDLE_USING_MAX = Area::AREA_DATA_SIZE_MAX - System::HeapSize::HEAP_FIELD_MIDDLE_HEAP_SIZE;  // 


#if PM_DEBUG
// エリア各種リソースの利用制限基準値
const u32 Area::AREA_DATA_SIZE_RULE[AREA_DATA_IDX_NUM] =
{
  0x8000,       //AREA_DATA_IDX_PLACEMENTDATA = 0,          //! 配置情報
  0x500000,     //AREA_DATA_IDX_TERRAIN_RESOURCE,           //! 地形リソース
  0,            //AREA_DATA_IDX_TERRAIN_LAYOUT_RESOURCE,    //! 地形配置データリソース
  0x600000,     //AREA_DATA_IDX_CHARACTOR_RESOURCE,         //! キャラクターリソース
  0x000000,     //AREA_DATA_IDX_STATICMODEL_RESOURCE,       //! 配置モデルリソース
  0,            //AREA_DATA_IDX_EFFECT_RESOURCE,            //! エフェクトリソース 
  0,            //AREA_DATA_IDX_ENVIRONMENT_RESOURCE,       //! 環境データリソース
  0,            //AREA_DATA_IDX_ZONE_SCRIPT_RESOURCE,       //! ゾーンスクリプトリソース
  0,            //AREA_DATA_IDX_INIT_SCRIPT_RESOURCE,       //! 初期化スクリプトリソース
  0,            //AREA_DATA_IDX_ENCOUNT_DATA,               //! エンカウントデータ
  0,            //AREA_DATA_IDX_CAMERA_AREA_DATA,           //! カメラエリアデータ
};


// エリア種別リソースの名称
const char* Area::AREA_DATA_NAME[AREA_DATA_IDX_NUM] = 
{
  "AREA_DATA_IDX_PLACEMENTDATA",          //! 配置情報
  "AREA_DATA_IDX_TERRAIN_RESOURCE",           //! 地形リソース
  "AREA_DATA_IDX_TERRAIN_LAYOUT_RESOURCE",    //! 地形配置データリソース
  "AREA_DATA_IDX_CHARACTOR_RESOURCE",         //! キャラクターリソース
  "AREA_DATA_IDX_STATICMODEL_RESOURCE",       //! 配置モデルリソース
  "AREA_DATA_IDX_EFFECT_RESOURCE",            //! エフェクトリソース 
  "AREA_DATA_IDX_ENVIRONMENT_RESOURCE",       //! 環境データリソース
  "AREA_DATA_IDX_ZONE_SCRIPT_RESOURCE",       //! ゾーンスクリプトリソース
  "AREA_DATA_IDX_INIT_SCRIPT_RESOURCE",       //! 初期化スクリプトリソース
  "AREA_DATA_IDX_ENCOUNT_DATA",               //! エンカウントデータ
  "AREA_DATA_IDX_CAMERA_AREA_DATA",           //! カメラエリアデータ
};

#endif // PM_DEBUG



//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 */
Area::Area(gfl2::heap::HeapBase * pArcOpenHeapParentHeap) : m_AreaID(AREAID_ERROR)
  , m_pHeap(NULL)
  , m_pFieldExtHeap(NULL)
  , m_pFieldExtHeapAllocator(NULL)
  , m_pFileManager(NULL)
{
  for( int i=0; i<AREA_DATA_IDX_NUM; ++i ){
    m_pBuffer[i] = NULL;
    m_DataSize[i] = 0;
  }

	m_pArcFileOpenHeap = GFL_CREATE_LOCAL_HEAP_NAME(pArcOpenHeapParentHeap, AREA_ARC_OPEN_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "AreaArcOpen");

  m_AreaDataSizeMax = 0;
}
/**
 *  @brief  デストラクタ
 */
Area::~Area()
{
  Terminate();

	if (m_pArcFileOpenHeap)
	{
		GFL_DELETE_HEAP(m_pArcFileOpenHeap);
		m_pArcFileOpenHeap = NULL;
	}
}

/**
 *  @biref エリア情報の読み込み開始 
 *  @param  areaID ロードエリアID
 *  @param  p_heapBase ヒープベース
 *  @param  p_fileManager ファイルマネージャ
 */
void Area::Initialize( AreaID areaID, gfl2::heap::HeapBase* p_heapBase, gfl2::heap::HeapBase * p_tmpHeap, gfl2::fs::AsyncFileManager* p_fileManager, Area::Mode mode )
{

  GFL_ASSERT( m_pFileManager == NULL ); // 多重初期化のチェック
  m_AreaID       = areaID;
  m_pHeap        =GFL_CREATE_LOCAL_HEAP_NAME( p_heapBase, System::HeapSize::AREA_DATA_SIZE_AND_BUFFER_MAX, gfl2::heap::HEAP_TYPE_EXP, false, "Area" );
  m_pFileManager = p_fileManager;

  switch( mode )
  {
  case MODE_NORMAL:
    m_pFieldExtHeap = NULL;
    m_pFieldExtHeapAllocator = NULL;
    m_AreaDataSizeMax = AREA_DATA_SIZE_MAX;
    break;

  case MODE_BATTLEMODEL:  ///< エリアヒープをポケモンモデル表示に使用
    m_pFieldExtHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pHeap, System::HeapSize::HEAP_FIELD_BATTLE_MODEL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "BattlePokemon" );
    m_pFieldExtHeapAllocator = GFL_NEW(m_pHeap) System::nijiAllocator( m_pFieldExtHeap, System::nijiAllocator::APPLICATION );
    m_AreaDataSizeMax = AREA_DATA_BATTLE_POKEMON_USING_MAX;
    break;

  case MODE_JF:  ///< エリアヒープをJFモデル表示に使用
    m_pFieldExtHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pHeap, System::HeapSize::HEAP_FIELD_JOINFESTA_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "JoinFesta" );
    m_pFieldExtHeapAllocator = GFL_NEW(m_pHeap) System::nijiAllocator( m_pFieldExtHeap, System::nijiAllocator::APPLICATION );
    m_AreaDataSizeMax = AREA_DATA_JOINFESTA_USING_MAX;
    break;

  case MODE_MIDDLE:  ///< ショップUIをしようするところよう設定
    m_pFieldExtHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pHeap, System::HeapSize::HEAP_FIELD_MIDDLE_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "ShopApp" );
    m_pFieldExtHeapAllocator = GFL_NEW(m_pHeap) System::nijiAllocator( m_pFieldExtHeap, System::nijiAllocator::APPLICATION );
    m_AreaDataSizeMax = AREA_DATA_MIDDLE_USING_MAX;
    break;

  default:
    GFL_ASSERT_STOP(mode < MODE_MAX);
    break;
  }

  //---------------------------------------------------
  // ARCファイルオープン
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = AREA_ARC_ID;
		openReq.heapForFile = m_pArcFileOpenHeap;
		openReq.heapForArcSrc = m_pArcFileOpenHeap;
    openReq.heapForReq = p_tmpHeap->GetLowerHandle();
    m_pFileManager->SyncArcFileOpen( openReq );
  }
  //---------------------------------------------------
  // ファイル読み込みリクエスト
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    for( int i= 0; i< AREA_DATA_IDX_NUM; ++i ){
      loadReq.arcId = AREA_ARC_ID;
      loadReq.datId = (areaID * AREA_DATA_IDX_NUM) + i;
      loadReq.ppBuf = &m_pBuffer[i];
      loadReq.heapForBuf = m_pHeap;
      loadReq.heapForReq = p_tmpHeap->GetLowerHandle();
      loadReq.heapForCompressed = p_tmpHeap->GetLowerHandle();
      loadReq.pBufSize = &m_DataSize[i];
      loadReq.align = 128;
      m_pFileManager->AddArcFileLoadDataReq( loadReq );
    }
  }
}

/**
 *  @brief  初期化処理 
 *  @retval true    初期化完了
 *  @retval false   初期化途中
 */
bool Area::InitializeWait()
{
  // 読み込み完了待ち
  if( !m_pFileManager->IsArcFileOpenFinished( AREA_ARC_ID ) ){
    return false;
  }
  for( int i=0; i<AREA_DATA_IDX_NUM; ++i ){
    if( !m_pFileManager->IsArcFileLoadDataFinished( &m_pBuffer[i] ) ){
      return false;
    }
  }

  // リソースをBinLinkerに渡す。
  for( int i=0; i<AREA_DATA_IDX_NUM; ++i ){
    m_Accessor[i].Initialize( m_pBuffer[i] );
  }

  return true;
}

/**
 *  @brief  エリア情報の破棄
 */
void Area::Terminate( void )
{
  if(m_pFileManager)
  {
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = AREA_ARC_ID;
      closeReq.heapForReq = m_pHeap->GetLowerHandle();
      m_pFileManager->SyncArcFileClose( closeReq );
    }

    for( int i= 0; i< AREA_DATA_IDX_NUM; ++i ){
      GflHeapFreeMemory( m_pBuffer[i] );
      m_pBuffer[i] = NULL;
      m_DataSize[i] = 0;
    }

    m_AreaID = AREAID_ERROR;


    if(m_pFieldExtHeap)
    {
      GFL_DELETE(m_pFieldExtHeapAllocator);
      GFL_DELETE_HEAP(m_pFieldExtHeap);
      m_pFieldExtHeap = NULL;
      m_pFieldExtHeapAllocator = NULL;
    }

    GFL_DELETE_HEAP(m_pHeap);
    m_pHeap = NULL;

    m_pFileManager = NULL;

  }
}

/**
 *  @brief アクセサーの取得
 */
gfl2::fs::BinLinkerAccessor* Area::GetAccessor( Area::DataIdx idx )
{ 
  if( idx < AREA_DATA_IDX_NUM ){
    return &m_Accessor[idx]; 
  }
  GFL_ASSERT_STOP( idx < AREA_DATA_IDX_NUM );
  return NULL;
}


/**
*  @brief 地形メモリモードを取得
*/
Terrain::MemoryMode::Value Area::GetTerrainMemoryMode(AreaID areaID)
{
  switch( areaID )
  {
    // JF外周部
  case A_F0601JF0101_01:
    {
      return Terrain::MemoryMode::LOW;
    }

  default:
    {
      return Terrain::MemoryMode::NORMAL;
    }
  }
}

#if PM_DEBUG
// リソースサイズをチェックする。
void Area::DebugCheckResourceSize()
{
  u32 dataSizeTotal = 0;

  f32 size_rule_par = static_cast<f32>(m_AreaDataSizeMax) / static_cast<f32>(AREA_DATA_SIZE_MAX);
  f32 area_data_size_rule;

  // Log出力
  GFL_PRINT( "[][][][][][][][][AREA RESOURCE SIZE][][][][][][][][][]\n" );
  GFL_PRINT( "USE SIZE:       使用量  /  最大使用量の基準\n" );
  for( u32 i=0; i<AREA_DATA_IDX_NUM; ++i )
  {
    area_data_size_rule = static_cast<f32>(AREA_DATA_SIZE_RULE[i]) * size_rule_par; // 実際にエリアで使用できるサイズの割合を掛け合わせる。

    GFL_PRINT( "IDX:           %s\n",   AREA_DATA_NAME[i] );
    GFL_PRINT( "USE SIZE:      0x%x(%dkb)/0x%x(%dkb)\n", m_DataSize[i], m_DataSize[i] / 1024, static_cast<u32>(area_data_size_rule), static_cast<u32>(area_data_size_rule) / 1024 );

    dataSizeTotal += m_DataSize[i];
  }

  // データサイズトータルがオーバーしていないか？
  if( dataSizeTotal >= m_AreaDataSizeMax )
  {
    GFL_PRINT( "!!!!AreaDataSizeのトータルがリミットを越えています。リーダーと相談して納まるように調整してください。!!!!!\n" );
    GFL_PRINT( "AreaDataSizeLimit[0x%x] <= AreaDataSizeTotal[0x%x]\n",m_AreaDataSizeMax, dataSizeTotal );
    GFL_PRINT( "このエラーはXY同時押しで抜けることが可能です。\n" );
    GFL_ASSERT( dataSizeTotal < m_AreaDataSizeMax );
  }
  else
  {
    GFL_PRINT( "AREA FreeSize 0x%x\n", DebugGetResourceFreeSize() );
  }
  GFL_PRINT( "[][][][][][][][][AREA RESOURCE SIZE][][][][][][][][][]\n" );
}

u32 Area::DebugGetResourceFreeSize()
{
  u32 dataSizeTotal = 0;

  // Log出力
  for( u32 i=0; i<AREA_DATA_IDX_NUM; ++i )
  {
    dataSizeTotal += m_DataSize[i];
  }

  return m_AreaDataSizeMax - dataSizeTotal;
}

#endif // PM_DEBUG

GFL_NAMESPACE_END(Field);
