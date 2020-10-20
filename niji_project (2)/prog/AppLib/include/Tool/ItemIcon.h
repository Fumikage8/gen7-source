//=============================================================================
/**
 * @file    ItemIcon.h
 * @brief   アイテムアイコン関連
 * @author  Hiroyuki Nakamura
 * @date    12.01.17
 */
//=============================================================================
#if !defined( ITEM_ICON_H_INCLUDED )
#define ITEM_ICON_H_INCLUDED

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <layout/include/gfl2_Layout.h>

#include <pml/include/item/item.h>

#include <AppLib/include/Tool/app_tool_LytTexReplaceSystem.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//=============================================================================
/**
 * @class ItemIcon
 * @brief	アイテムアイコン
 * @date  12.01.17
 */
//=============================================================================
class ItemIcon
{
	GFL_FORBID_COPY_AND_ASSIGN( ItemIcon );

public:
	static const u32 TEX_WIDTH = 32;		//!< テクスチャドット幅
	static const u32 TEX_HEIGHT = 32;		//!< テクスチャドット高さ

#if defined(GF_PLATFORM_CTR)
  static const u32 TEX_SIZE   = TEX_WIDTH*TEX_HEIGHT*2+LytTexReplaceSystem::BCLIM_FOOTER_SIZE;  //!< テクスチャサイズ
#elif defined(GF_PLATFORM_WIN32)
	// @note PC版のサイズは適当です。３倍あれば足りるはず。
  static const u32 TEX_SIZE   = (TEX_WIDTH*TEX_HEIGHT*2)*3+LytTexReplaceSystem::BCLIM_FOOTER_SIZE;  //!< テクスチャサイズ
#endif

  static const bool IS_COMP;


public:
	//-----------------------------------------------------------------------------
	/**
	 * @brief		コンストラクタ
	 *
	 * @param		heap		ヒープ
	 * @param		devHeap	デバイスヒープ
	 * @param		max			テクスチャ登録最大数
	 */
	//-----------------------------------------------------------------------------
  ItemIcon( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * devHeap, u32 max );

	//-----------------------------------------------------------------------------
	/**
	 * @brief		デストラクタ
	 */
	//-----------------------------------------------------------------------------
  virtual ~ItemIcon();

  //-----------------------------------------------------------------------------
  /**
   * @func    FileOpenSync
   * @brief   アークファイルを開く（同期版）
   * @date    2015.02.26
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void FileOpenSync( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @func    FileOpen
   * @brief   アークファイルを開く（非同期版）
   * @date    2015.02.26
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void FileOpen( gfl2::heap::HeapBase * heap );

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
  bool IsFileOpen(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    FileCloseSync
   * @brief   アークファイルを閉じる（同期版）
   * @date    2015.02.26
   */
  //-----------------------------------------------------------------------------
  void FileCloseSync(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    FileClose
   * @brief   アークファイルを閉じる（非同期版）
   * @date    2015.02.26
   */
  //-----------------------------------------------------------------------------
  void FileClose( gfl2::heap::HeapBase * heap );

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
  bool IsFileClose(void);

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
	void AllocTextureBuffer( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @brief		テクスチャを読み込んでペインを変更
   *
   * @param		id				読み込む場所のID
   * @param		item			アイテム番号
   * @param		material	マテリアルのインデックス
   * @param		texture		テクスチャのインデックス
   *
   * @return	none
   */
  //-----------------------------------------------------------------------------
  void ReplacePaneTexture( u32 id, u32 item, gfl2::lyt::LytPicture * pane, u32 material=0, u32 texture=0 );

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
	void ReplacePaneTexture( u32 id, gfl2::lyt::LytPicture * pane, u32 material=0, u32 texture=0 );

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
	static gfl2::fs::ArcFile::ARCDATID GetResourceIndex( u32 item );

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
	void LoadRequest( u32 id, u32 item );

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
	bool IsLoadFinished( u32 id );

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
	bool CancelLoadTextureRequest( u32 id );

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
  void ReplaceReadTexture( u32 id, gfl2::lyt::LytPicture * pane, u32 material=0, u32 texture=0 );

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
	bool IsModuleFree(void);

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
	void ReplacePaneTexture( u32 id, item::BALL_ID ball, gfl2::lyt::LytPicture * pane, u32 material=0, u32 texture=0 );

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
	gfl2::fs::ArcFile::ARCDATID GetResourceIndex( item::BALL_ID ball );

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
	void LoadRequest( u32 id, item::BALL_ID ball );


private:
  app::tool::LytTexReplaceSystem * m_pTexSys;  //!< テクスチャシステム
  gfl2::heap::HeapBase * m_pWorkHeap;          //!< 作業用ヒープ
  gfl2::heap::HeapBase * m_pBuffHeap;          //!< データ生成用ヒープ（デバイスヒープ）
  bool m_isFileOpen;                           //!< ファイルを開いたか
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // ITEM_ICON_H_INCLUDED
