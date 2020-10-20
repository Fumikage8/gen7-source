//============================================================================================
/**
 * @file		item_manager.cpp
 * @brief		アイテムデータマネージャ処理
 * @author	Hiroyuki Nakamura
 * @date		11.06.30
 *
 * class		itemman::ITEM_MANAGER::
 */
//============================================================================================
#include <debug/include/gfl2_Assert.h>

#include "pml/include/system/pml_Library.h"
#include "pml/include/item/item_manager.h"


namespace itemman {


//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   cache_size  キャッシュサイズ（個数）
 * @param   is_open     ファイルを開くか（同期）
 *
 * @li  非同期の場合は自前で開いてください
 */
//--------------------------------------------------------------------------------------------
ITEM_MANAGER::ITEM_MANAGER( gfl2::heap::HeapBase * heap, int cache_size, bool is_open )
  : m_cacheMax( cache_size )
  , m_syncOpen( is_open )
{
  m_pItemData = GFL_NEW_ARRAY( heap ) item::ITEM_DATA[m_cacheMax];

  if( m_syncOpen != false )
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq arcReq;
    arcReq.fileName = pml::Library::m_setting->garcPath_ItemData;
    arcReq.heapForFile = heap;
    arcReq.heapForReq = heap->GetLowerHandle();
    /*
      2016/08/25  Yu Muto
      GF_DEVELOP_HIO_RESOURCE!=0の場合は、
      全てのリソースをHIO経由で読み込むモードへの対応のためデフォルト設定（マネージャ設定に依存）とします
    */
#if GF_DEVELOP_HIO_RESOURCE
#else
    arcReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;
#endif
    pml::Library::m_pmlFileManager->SyncArcFileOpen(arcReq);
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
ITEM_MANAGER::~ITEM_MANAGER()
{
  GFL_DELETE_ARRAY( m_pItemData );

  if( m_syncOpen != false )
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.fileName = pml::Library::m_setting->garcPath_ItemData;
    pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムデータ取得
 *
 * @param		itemID	アイテム番号
 * @param		dat			取得場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ITEM_MANAGER::GetData( int itemID, item::ITEM_DATA & dat )
{
	item::ITEM_DATA * buf;
	int	i;
	
	buf = NULL;

	for( i=0; i<m_cacheMax; i++ ){
		int prm = m_pItemData[i].GetParam( item::ITEM_DATA::PRM_ID_ITEMNUMBER );
		// 確保済み
		if( prm == itemID ){
			dat = m_pItemData[i];
//			GFL_PRINT( "CACHE :\n" );
			return;
		}
		// 空きあり
		if( prm == ITEM_DUMMY_DATA ){
			buf = &m_pItemData[i];
//			GFL_PRINT( "NEW :\n" );
			break;
		}
	}

	// 空きなし
	if( buf == NULL ){
		// 一番古いデータを削除
		for( i=0; i<m_cacheMax-1; i++ ){
			m_pItemData[i] = m_pItemData[i+1];
		}
		buf = &m_pItemData[m_cacheMax-1];
//		GFL_PRINT( "SORT :\n" );
	}

	// 読み込み
	buf->Create( itemID , pml::Library::m_setting->garcPath_ItemData );
	dat = *buf;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムパラメータ取得
 *
 * @param		itemID	アイテム番号
 * @param		prmID		パラメータID
 *
 * @return	指定パラメータ
 */
//--------------------------------------------------------------------------------------------
int ITEM_MANAGER::GetParam( int itemID, item::ITEM_DATA::ITEM_PRM_ID prmID )
{
	if( itemID == ITEM_DUMMY_DATA ){
		return 0;
	}

	item::ITEM_DATA	dat;

	GetData( itemID, dat );
	return dat.GetParam( prmID );
}

//-----------------------------------------------------------------------------
/**
 * @func    FileOpen
 * @brief   アークファイルを開く（非同期版）
 * @date    2015.06.24
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void ITEM_MANAGER::FileOpen( gfl2::heap::HeapBase * heap )
{
  if( m_syncOpen != false )
  {
    GFL_ASSERT( 0 );
    return;
  }
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.fileName      = pml::Library::m_setting->garcPath_ItemData;
  req.heapForFile   = heap;
  req.heapForReq    = heap->GetLowerHandle();
  /*
    2016/08/25  Yu Muto
    GF_DEVELOP_HIO_RESOURCE!=0の場合は、
    全てのリソースをHIO経由で読み込むモードへの対応のためデフォルト設定（マネージャ設定に依存）とします
  */
#if GF_DEVELOP_HIO_RESOURCE
#else
  req.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;
#endif
  pml::Library::m_pmlFileManager->AddArcFileOpenReq( req );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFileOpen
 * @brief   アークファイルオープンチェック
 * @date    2015.06.24
 *
 * @retval  false = オープン中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool ITEM_MANAGER::IsFileOpen(void)
{
  if( m_syncOpen != false )
  {
    GFL_ASSERT( 0 );
    return true;
  }
  return pml::Library::m_pmlFileManager->IsArcFileOpenFinished( pml::Library::m_setting->garcPath_ItemData );
}

//-----------------------------------------------------------------------------
/**
 * @func    FileClose
 * @brief   アークファイルを閉じる（非同期版）
 * @date    2015.06.24
 *
 * @param   arc_id  アークID
 * @param   heap    ヒープ
 */
//-----------------------------------------------------------------------------
void ITEM_MANAGER::FileClose( gfl2::heap::HeapBase * heap )
{
  if( m_syncOpen != false )
  {
    GFL_ASSERT( 0 );
    return;
  }
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.fileName   = pml::Library::m_setting->garcPath_ItemData;
  req.heapForReq = heap->GetLowerHandle();
  pml::Library::m_pmlFileManager->AddArcFileCloseReq( req );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFileClose
 * @brief   アークファイルクローズチェック
 * @date    2015.06.24
 *
 * @retval  false = クローズ中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool ITEM_MANAGER::IsFileClose(void)
{
  if( m_syncOpen != false )
  {
    GFL_ASSERT( 0 );
    return true;
  }
  return pml::Library::m_pmlFileManager->IsArcFileCloseFinished( pml::Library::m_setting->garcPath_ItemData );
}


}	// itemman
