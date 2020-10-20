//=============================================================================
/**
 * @file    ItemIcon.cpp
 * @brief   アイテムアイコン関連
 * @author  Hiroyuki Nakamura
 * @date    12.01.17
 */
//=============================================================================
#include <AppLib/include/Tool/ItemIcon.h>

#include <base/include/gfl2_Singleton.h>

#include <GameSys/include/GameManager.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/ItemIcon.gaix>
#include "niji_conv_header/poke_lib/item/itemicon_tbl.cdat"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

const bool ItemIcon::IS_COMP = true;   // 圧縮されているか


//-----------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 *
 * @param		heap		ヒープ
 * @param		devHeap	デバイスヒープ
 * @param		max			テクスチャ登録最大数
 */
//-----------------------------------------------------------------------------
ItemIcon::ItemIcon( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * devHeap, u32 max )
  : m_pTexSys( NULL )
  , m_pWorkHeap( heap )
  , m_pBuffHeap( devHeap )
  , m_isFileOpen( false )
{
	m_pTexSys = GFL_NEW( heap ) app::tool::LytTexReplaceSystem( heap, devHeap, TEX_SIZE, max );
}

//-----------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//-----------------------------------------------------------------------------
ItemIcon::~ItemIcon()
{
  if( m_isFileOpen != false )
  {
    GFL_ASSERT( m_isFileOpen );
    FileCloseSync();
  }
	GFL_DELETE m_pTexSys;
}

//-----------------------------------------------------------------------------
/**
 * @func    FileOpenSync
 * @brief   アークファイルを開く（同期版）
 * @date    2015.02.26
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void ItemIcon::FileOpenSync( gfl2::heap::HeapBase * heap )
{
  app::util::FileAccessor::FileOpenSync( ARCID_ITEMICON, heap );
  m_isFileOpen = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    FileOpen
 * @brief   アークファイルを開く（非同期版）
 * @date    2015.02.26
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void ItemIcon::FileOpen( gfl2::heap::HeapBase * heap )
{
  app::util::FileAccessor::FileOpen( ARCID_ITEMICON, heap );
  m_isFileOpen = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFileOpen
 * @brief   アークファイルオープンチェック
 * @date    2015.02.26
 *
 * @retval  false = オープン中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemIcon::IsFileOpen(void)
{
  return app::util::FileAccessor::IsFileOpen( ARCID_ITEMICON );
}

//-----------------------------------------------------------------------------
/**
 * @func    FileCloseSync
 * @brief   アークファイルを閉じる（同期版）
 * @date    2015.02.26
 */
//-----------------------------------------------------------------------------
void ItemIcon::FileCloseSync(void)
{
  // 開いていない
  if( m_isFileOpen == false )
  {
    GFL_ASSERT( 0 );
  }
  app::util::FileAccessor::FileCloseSync( ARCID_ITEMICON );
  m_isFileOpen = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    FileClose
 * @brief   アークファイルを閉じる（非同期版）
 * @date    2015.02.26
 */
//-----------------------------------------------------------------------------
void ItemIcon::FileClose( gfl2::heap::HeapBase * heap )
{
  // 開いていない
  if( m_isFileOpen == false )
  {
    GFL_ASSERT( 0 );
  }
  app::util::FileAccessor::FileClose( ARCID_ITEMICON, heap );
  m_isFileOpen = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFileClose
 * @brief   アークファイルクローズチェック
 * @date    2015.02.26
 *
 * @retval  false = オープン中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemIcon::IsFileClose(void)
{
  return app::util::FileAccessor::IsFileClose( ARCID_ITEMICON );
}

//-----------------------------------------------------------------------------
/**
 * @brief		メモリ確保
 *
 * @param		id	確保する場所のID
 *
 * @return	none
 *
 * @li	あらかじめ確保しておく場合に使用
 */
//-----------------------------------------------------------------------------
void ItemIcon::AllocTextureBuffer( u32 id )
{
  m_pTexSys->AllocTextureBuffer( id );
}

//-----------------------------------------------------------------------------
/**
 * @brief		テクスチャを読み込んでペインを変更
 *
 * @param		id				読み込む場所のID
 * @param		item			アイテム番号
 * @param		pane			ペイン
 * @param		material	マテリアルのインデックス
 * @param		texture		テクスチャのインデックス
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void ItemIcon::ReplacePaneTexture( u32 id, u32 item, gfl2::lyt::LytPicture * pane, u32 material, u32 texture )
{
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  u32 datID = GetResourceIndex( item );
  if( man->GetArcFile(ARCID_ITEMICON) == NULL )
  {
    m_pTexSys->LoadTexture( m_pWorkHeap, id, ARCID_ITEMICON, datID, IS_COMP );
  }
  else
  {
    m_pTexSys->LoadTextureCore( m_pWorkHeap, id, ARCID_ITEMICON, datID, IS_COMP );
  }
  m_pTexSys->ReplaceTexture( id, pane, material, texture );
}

//-----------------------------------------------------------------------------
/**
 * @brief		ペインのテクスチャを変更（データ読み込みなし版）
 *
 * @param		id				読み込む場所のID
 * @param		pane			ペイン
 * @param		material	マテリアルのインデックス
 * @param		texture		テクスチャのインデックス
 *
 * @return	none
 *
 * @li  非同期読み込みの直後はReplaceReadTexture()を使用すること。
 */
//-----------------------------------------------------------------------------
void ItemIcon::ReplacePaneTexture( u32 id, gfl2::lyt::LytPicture * pane, u32 material, u32 texture )
{
  m_pTexSys->ReplaceTexture( id, pane, material, texture );
}

//-----------------------------------------------------------------------------
/**
 * @brief		テクスチャリソースのインデックスを取得
 *
 * @param		item	アイテム番号
 *
 * @return	テクスチャリソースのインデックス
 *
 * @li	アーカイブデータのインデックス取得です
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID ItemIcon::GetResourceIndex( u32 item )
{
	return ItemIconIndexTable[item];
}

//-----------------------------------------------------------------------------
/**
 * @brief		読み込みリクエスト
 *
 * @param		id		読み込む場所のID
 * @param		item	アイテム番号
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void ItemIcon::LoadRequest( u32 id, u32 item )
{
  bool result = m_pTexSys->LoadTextureRequestCore(
                  m_pWorkHeap,
                  id,
                  ARCID_ITEMICON,
                  GetResourceIndex(item),
                  IS_COMP );
  GFL_ASSERT( result );
}

//-----------------------------------------------------------------------------
/**
 * @brief		読み込み終了チェック
 *
 * @param		id		読み込む場所のID
 *
 * @retval	"true = 終了"
 * @retval	"false = それ以外"
 */
//-----------------------------------------------------------------------------
bool ItemIcon::IsLoadFinished( u32 id )
{
	return m_pTexSys->IsLoadTextureFinished( id );
}

//-----------------------------------------------------------------------------
/**
 * @brief		読み込みリクエストをキャンセル
 *
 * @param		id		読み込む場所のID
 *
 * @retval	"true = キャンセル成功"
 * @retval	"false = キャンセル失敗"
 */
//-----------------------------------------------------------------------------
bool ItemIcon::CancelLoadTextureRequest( u32 id )
{
  return m_pTexSys->CancelLoadTextureRequest( id );
}

//-----------------------------------------------------------------------------
/**
 * @brief		ペインのテクスチャを変更（データ読み込みなし版）
 *
 * @param		id				読み込む場所のID
 * @param		pane			ペイン
 * @param		material	マテリアルのインデックス
 * @param		texture		テクスチャのインデックス
 *
 * @return	none
 *
 * @li	非同期読み込み後、最初の変更はこれで。
 */
//-----------------------------------------------------------------------------
void ItemIcon::ReplaceReadTexture( u32 id, gfl2::lyt::LytPicture * pane, u32 material, u32 texture )
{
  m_pTexSys->LoadTexture( id );
  m_pTexSys->ReplaceTexture( id, pane, material, texture );
}

//-----------------------------------------------------------------------------
/**
 * @brief		クラスを破棄できるか
 *
 * @param		none
 *
 * @retval	"true = 可"
 * @retval	"false = 不可"
 */
//-----------------------------------------------------------------------------
bool ItemIcon::IsModuleFree(void)
{
	return m_pTexSys->IsModuleFree();
}


//=============================================================================
//  ボール関連
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief		ペインのテクスチャを変更
 *
 * @param		id				読み込む場所のID
 * @param		ball			ボールID
 * @param		pane			ペイン
 * @param		material	マテリアルのインデックス
 * @param		texture		テクスチャのインデックス
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void ItemIcon::ReplacePaneTexture( u32 id, item::BALL_ID ball, gfl2::lyt::LytPicture * pane, u32 material, u32 texture )
{
	ReplacePaneTexture( id, item::ITEM_BallID2ItemID(ball), pane, material, texture );
}

//-----------------------------------------------------------------------------
/**
 * @brief		テクスチャリソースのインデックスを取得
 *
 * @param		ball	ボールID
 *
 * @return	テクスチャリソースのインデックス
 *
 * @li	アーカイブデータのインデックス取得です
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID ItemIcon::GetResourceIndex( item::BALL_ID ball )
{
	return GetResourceIndex( item::ITEM_BallID2ItemID(ball) );
}

//-----------------------------------------------------------------------------
/**
 * @brief		読み込みリクエスト
 *
 * @param		id		読み込む場所のID
 * @param		ball	ボールID
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void ItemIcon::LoadRequest( u32 id, item::BALL_ID ball )
{
	LoadRequest( id, item::ITEM_BallID2ItemID(ball) );
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
