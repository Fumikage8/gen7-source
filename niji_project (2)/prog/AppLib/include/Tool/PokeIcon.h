//=============================================================================
/**
 * @file    PokeIcon.h
 * @brief   ポケモンアイコン関連
 * @author  Hiroyuki Nakamura
 * @date    12.01.17
 */
//=============================================================================
#if !defined( POKE_ICON_H_INCLUDED )
#define POKE_ICON_H_INCLUDED

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <layout/include/gfl2_Layout.h>

#include <pml/include/pmlib.h>

#include <AppLib/include/Tool/app_tool_LytTexReplaceSystem.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(PokeTool)
struct SimpleParam;
GFL_NAMESPACE_END(PokeTool)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

// 前方宣言
class LytTexData;


//=============================================================================
/**
 * @class PokeIcon
 * @brief	ポケモンアイコン
 * @date  12.01.17
 */
//=============================================================================
class PokeIcon
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeIcon );

public:
  static const u32 TEX_WIDTH  = 64;		//!< テクスチャドット幅
  static const u32 TEX_HEIGHT = 32;		//!< テクスチャドット高さ

#if defined(GF_PLATFORM_CTR)
  static const u32 TEX_SIZE   = TEX_WIDTH*TEX_HEIGHT*2+LytTexReplaceSystem::BCLIM_FOOTER_SIZE;  //!< テクスチャサイズ
#elif defined(GF_PLATFORM_WIN32)
	// @note PC版のサイズは適当です。３倍あれば足りるはず。
  static const u32 TEX_SIZE   = (TEX_WIDTH*TEX_HEIGHT*2)*3+LytTexReplaceSystem::BCLIM_FOOTER_SIZE;  //!< テクスチャサイズ
#endif


  static const u32 PANE_WIDTH  = 64;	//!< 1倍時のペイン幅
  static const u32 PANE_HEIGHT = 32;	//!< 1倍時のペイン高さ

  static const bool IS_COMP;


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief		コンストラクタ
   *
   * @param		heap		  ヒープ
   * @param		devHeap	  デバイスヒープ
   * @param		max			  テクスチャ登録最大数
   * @param		is_dummy  ？アイコン読み込み  ※デフォルト = false
   */
  //-----------------------------------------------------------------------------
  PokeIcon( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * devHeap, u32 max, bool is_dummy=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief		デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~PokeIcon();

  //-----------------------------------------------------------------------------
  /**
   * @func    FileOpenSync
   * @brief   アークファイルを開く（同期版）
   * @date    2015.03.02
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void FileOpenSync( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @func    FileOpen
   * @brief   アークファイルを開く（非同期版）
   * @date    2015.03.02
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void FileOpen( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFileOpen
   * @brief   アークファイルオープンチェック
   * @date    2015.03.02
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
   * @date    2015.03.02
   */
  //-----------------------------------------------------------------------------
  void FileCloseSync(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    FileClose
   * @brief   アークファイルを閉じる（非同期版）
   * @date    2015.03.02
   */
  //-----------------------------------------------------------------------------
  void FileClose( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFileClose
   * @brief   アークファイルクローズチェック
   * @date    2015.03.02
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
   * @brief		テクスチャを読み込んでペインを変更 ( CoreParam指定版 )
   *
   * @param		id				読み込む場所のID
   * @param		pp				CoreParam
   * @param		pane			ペイン
   * @param		material	マテリアルのインデックス　※デフォルト = 0
   * @param		texture		テクスチャのインデックス　※デフォルト = 0
   * @param		flip			反転フラグ　※デフォルト = false
   *
   * @return	none
   */
  //-----------------------------------------------------------------------------
  void ReplacePaneTexture(
			  u32 id, const pml::pokepara::CoreParam * pp,
			  gfl2::lyt::LytPicture * pane, u32 material=0, u32 texture=0, bool flip=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief		テクスチャを読み込んでペインを変更 ( SimpleParam指定版 )
   *
   * @param		id				読み込む場所のID
   * @param		simple_param  描画用ポケモン情報
   * @param		pane			ペイン
   * @param		material	マテリアルのインデックス　※デフォルト = 0
   * @param		texture		テクスチャのインデックス　※デフォルト = 0
   * @param		flip			反転フラグ　※デフォルト = false
   *
   * @return	none
   */
  //-----------------------------------------------------------------------------
  void ReplacePaneTexture(
			  u32 id, const PokeTool::SimpleParam * simple_param,
			  gfl2::lyt::LytPicture * pane, u32 material=0, u32 texture=0, bool flip=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief		ペインのテクスチャを変更（データ読み込みなし版）
   *
   * @param		id				読み込む場所のID
   * @param		pane			ペイン
   * @param		material	マテリアルのインデックス　※デフォルト = 0
   * @param		texture		テクスチャのインデックス　※デフォルト = 0
   *
   * @return	none
   *
   * @li  非同期読み込みの直後はReplaceReadTexture()を使用すること。
   */
  //-----------------------------------------------------------------------------
  void ReplacePaneTexture( u32 id, gfl2::lyt::LytPicture * pane, u32 material=0, u32 texture=0 );

  //-----------------------------------------------------------------------------
  /**
   * @brief		テクスチャリソースのインデックスを取得 ( CoreParam指定版 )
   *
   * @param		pp		CoreParam
   * @param		flip	反転フラグ　※デフォルト = false
   *
   * @return	テクスチャリソースのインデックス
   *
   * @li	アーカイブデータのインデックス取得です
   */
  //-----------------------------------------------------------------------------
  static gfl2::fs::ArcFile::ARCDATID GetResourceIndex( const pml::pokepara::CoreParam * pp, bool flip=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief		テクスチャリソースのインデックスを取得 ( SimpleParam指定版 )
   *
   * @param		simple_param  描画用ポケモン構造体
   * @param		flip					反転フラグ　※デフォルト = false
   *
   * @return	テクスチャリソースのインデックス
   *
   * @li	アーカイブデータのインデックス取得です
   */
  //-----------------------------------------------------------------------------
  static gfl2::fs::ArcFile::ARCDATID GetResourceIndex( const PokeTool::SimpleParam * simple_param, bool flip=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief		読み込みリクエスト ( CoreParam版 )
   *
   * @param		id		読み込む場所のID
   * @param		pp		CoreParam
   * @param		flip	反転フラグ　※デフォルト = false
   *
   * @retval	"true = リクエスト成功 "
   * @retval	"false = リクエスト失敗"
   */
  //-----------------------------------------------------------------------------
  bool ReadRequest( u32 id, const pml::pokepara::CoreParam * pp, bool flip=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief		読み込みリクエスト ( SimpleParam版 )
   *
   * @param		id						読み込む場所のID
   * @param		simple_param	SimpleParam
   * @param		flip					反転フラグ　※デフォルト = false
   *
   * @retval	"true = リクエスト成功 "
   * @retval	"false = リクエスト失敗"
   */
  //-----------------------------------------------------------------------------
  bool ReadRequest( u32 id, const PokeTool::SimpleParam * simple_param, bool flip=false );

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
   * @brief		ダミーテクスチャの読み込み
   *
   * @param		none
   *
   * @return	none
   */
  //-----------------------------------------------------------------------------
  void LoadDummyTexture(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief		ダミーテクスチャの読み込みリクエスト
   *
   * @param		none
   *
   * @return	none
   */
  //-----------------------------------------------------------------------------
  void LoadDummyTextureRequest(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief		ダミーテクスチャの読み込み終了チェック
   *
   * @param		none
   *
   * @retval	"true = 終了"
   * @retval	"false = それ以外"
   */
  //-----------------------------------------------------------------------------
  bool IsLoadDummyTextureFinished(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   ペインのテクスチャをダミーテクスチャに変更
   *
   * @param   pane      ペイン
   * @param   material  マテリアルのインデックス  ※デフォルト = 0
   * @param   texture   テクスチャのインデックス  ※デフォルト = 0
   *
   * @li  非同期読み込みの直後はReplaceReadDummyTexture()を使用すること。
   */
  //-----------------------------------------------------------------------------
  void ReplaceDummyTexture( gfl2::lyt::LytPicture * pane, u32 material=0, u32 texture=0 );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ペインのテクスチャをダミーテクスチャに変更
   *
   * @param   pane      ペイン
   * @param   material  マテリアルのインデックス  ※デフォルト = 0
   * @param   texture   テクスチャのインデックス  ※デフォルト = 0
   *
   * @li  非同期読み込み後、最初の変更はこれで。
   */
  //-----------------------------------------------------------------------------
  void ReplaceReadDummyTexture( gfl2::lyt::LytPicture * pane, u32 material=0, u32 texture=0 );

  //-----------------------------------------------------------------------------
  /**
   * @brief   テクスチャがロード済みかどうかを調べる
   *
   * @param   id  チェックするテクスチャの管理番号
   *
   * @retval  "true = ロード済み"
   * @retval  "false = ロードしていない"
   */
  //-----------------------------------------------------------------------------
  bool IsTextureLoaded( u32 id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   テクスチャデータを取得する
   *
   * @param   id  場所のID
   *
   * @return  テクスチャデータ
   */
  //-----------------------------------------------------------------------------
  app::tool::LytTexData * GetLytTexData( u32 id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   テクスチャデータをコピーする
   *
   * @param   src_id    コピー元ID
   * @param   dest_id   コピー先ID
   */
  //-----------------------------------------------------------------------------
  void CopyTextureBuffer( u32 src_id, u32 dest_id );

  //-----------------------------------------------------------------------------
  /**
   * @brief   テクスチャデータを入れ替える
   *
   * @param   id1     ID1
   * @param   id2     ID2
   * @param   tmp_id  テンポラリID
   */
  //-----------------------------------------------------------------------------
  void ChangeTextureBuffer( u32 id1, u32 id2, u32 tmp_id );


private:
  //-----------------------------------------------------------------------------
  /**
   * @brief		テクスチャを読み込んでペインを変更
   *
   * @param		id				読み込む場所のID
   * @param		dat_id		アーカイブデータID
   * @param		pane			ペイン
   * @param		material	マテリアルのインデックス
   * @param		texture		テクスチャのインデックス
   *
   * @return	none
   */
  //-----------------------------------------------------------------------------
  void ReplacePaneTextureCore( u32 id, u32 dat_id, gfl2::lyt::LytPicture * pane, u32 material, u32 texture );

  //-----------------------------------------------------------------------------
  /**
   * @brief		テクスチャリソースのインデックスを取得
   *
   * @param		mons	ポケモン番号
   * @param		frm		フォルム番号
   * @param		sex		性別
   * @param		egg		タマゴフラグ
   * @param		flip	反転フラグ　※デフォルト = false
   *
   * @return	テクスチャリソースのインデックス
   */
  //-----------------------------------------------------------------------------
  static gfl2::fs::ArcFile::ARCDATID GetResourceIndex( MonsNo mons, pml::FormNo frm, pml::Sex sex, bool egg, bool flip=false );


private:
  app::tool::LytTexReplaceSystem * m_pTexSys;   //!< テクスチャシステム
  gfl2::heap::HeapBase * m_pWorkHeap;           //!< 作業用ヒープ
  gfl2::heap::HeapBase * m_pBuffHeap;           //!< データ生成用ヒープ（デバイスヒープ）
  bool m_isFileOpen;                            //!< ファイルを開いたか
  u32	m_dummyTexID;                             //!< ？アイコンのテクスチャID
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // POKE_ICON_H_INCLUDED
