﻿//============================================================================================
/**
 * @file		item_manager.h
 * @brief		アイテムデータマネージャ処理
 * @author	Hiroyuki Nakamura
 * @date		11.06.30
 *
 * class		itemman::ITEM_MANAGER::
 */
//============================================================================================
#ifndef __PML_ITEM_MANAGER_H__
#define __PML_ITEM_MANAGER_H__
#pragma	once

#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_MsgData.h>
#include "item.h"


//---------------------------------------------------------------------
/**
 * @namespace itemman
 */
//---------------------------------------------------------------------
namespace itemman {

//--------------------------------------------------------------------------------------------
/**
 * @brief アイテムマネージャー
 */
//--------------------------------------------------------------------------------------------
class ITEM_MANAGER
{
public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   cache_size  キャッシュサイズ（個数）
   * @param   is_open     ファイルを開くか（true = 同期）　※デフォルト = true
   *
   * @li  非同期の場合は自前で開いてください
   */
  //--------------------------------------------------------------------------------------------
  ITEM_MANAGER( gfl2::heap::HeapBase * heap, int cache_size, bool is_open = true );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		デストラクタ
	 */
	//--------------------------------------------------------------------------------------------
	virtual ~ITEM_MANAGER();

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
	void GetData( int itemID, item::ITEM_DATA & dat );

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
	int GetParam( int itemID, item::ITEM_DATA::ITEM_PRM_ID prmID );

  //-----------------------------------------------------------------------------
  /**
   * @func    FileOpen
   * @brief   アークファイルを開く（非同期版）
   * @date    2015.06.24
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void FileOpen( gfl2::heap::HeapBase * heap );

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
  bool IsFileOpen(void);

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
  void FileClose( gfl2::heap::HeapBase * heap );

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
  bool IsFileClose(void);


private:
  item::ITEM_DATA * m_pItemData;  // アイテムデータ
  int m_cacheMax;                 // キャッシュ最大数
  bool m_syncOpen;                // 同期読み込み時 = true
};

}	// itemman

#endif // __PML_ITEM_MANAGER_H__
