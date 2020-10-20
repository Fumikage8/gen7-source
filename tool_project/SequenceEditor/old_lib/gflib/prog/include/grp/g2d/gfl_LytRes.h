#ifndef __GFL_LYTRES_H__
#define __GFL_LYTRES_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_LytRes.h
 *	@brief  レイアウトリソースクラス
 *	@author	tomoya takahashi
 *	@date		2010.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>
#include <nw/lyt.h>

#include <gfl_Fs.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_Str.h>

#include <grp/gfl_GraphicsType.h>

namespace gfl {
namespace grp {
namespace g2d {

//-------------------------------------
///	前方宣言
//=====================================
class LytResManager;
class LytSys;
class LytSysBase;
class LytWkBase;
class LytWk;


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
enum
{
  LYTRES_PANENAME_ERROR = 0xffff,  // ペイン名ない

  LYTRES_DEFAULT_FONT_MAX = 8,  // デフォルトフォント登録最大数
};
  
//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          gfl::grp::g2d::LytBinary構造
 */
//-----------------------------------------------------------------------------
typedef struct _LytBinaryHeader LytBinaryHeader;

//-----------------------------------------------------------------------------
/**
 *          gfl::grp::g2d::LytPaneBinary構造
 */
//-----------------------------------------------------------------------------
typedef struct _LytPaneBinaryHeader LytPaneBinaryHeader;


//-----------------------------------------------------------------------------
/**
 *    リソースAccessor
 */
//-----------------------------------------------------------------------------
typedef nw::lyt::ArcResourceAccessor LytResourceAccessor;

//-----------------------------------------------------------------------------
/**
 *    テクスチャ情報
 */
//-----------------------------------------------------------------------------
typedef nw::lyt::TextureInfo LytResTextureInfo;
typedef nw::lyt::TexResource LytResTexture;
  
//-----------------------------------------------------------------------------
/**
 *					レイアウトリソースクラス宣言
*/
//-----------------------------------------------------------------------------
class LytRes
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(LytRes);

  friend class LytResManager;
  friend class LytSys;
  friend class LytSysBase;
  friend class LytWkBase;
  friend class LytWk;
  
public:

  //----------------------------------------------------------------------------
  /**
   *	@brief  レイアウトリソースが読み込み済みかチェック
   *
   *	@retval true    読み込み済み
   *	@retval false   読み込まれていない
   */
  //-----------------------------------------------------------------------------
  bool IsDataIn(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  ファイル数取得
   *
   *	@return ファイル数
   */
  //-----------------------------------------------------------------------------
  u32 GetFileNum( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  ファイル名取得
   *
   *	@param  lytDataID データID
   *
   *	@return ファイル名
   */
  //-----------------------------------------------------------------------------
  const char* GetFileName( u32 lytDataID ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  目的のリソースを取得
   *
   *  @param  lytDataID   レイアウトデータID
   * 
   *  @retval リソースポインタ  ない場合製品版ではNULLが戻る。
   */
  //-----------------------------------------------------------------------------
  void* GetResource( u32 lytDataID ) const;

  //-----------------------------------------------------------------------------
  /**
   *   @brief リソースブロックの取得
   *
   *   @param lytDataID   レイアウトデータID
   *
   *   @retval  アニメーションリソース
   *   @retval  NULL  データが無かった場合（製品版のみ）
   */
  //-----------------------------------------------------------------------------
  void* GetAnimeResource( u32 lytDataID ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  テクスチャをすべてメモリに配置する
   *
   *  @param  area    転送先メモリ　（FCRAM/VRAMA/VRAMB）
   */
  //-----------------------------------------------------------------------------
  void TransferAllTexture( gfl::grp::MemoryArea area );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  テクスチャをメモリに配置する
   *
   *  @param  texArcIndex   テクスチャレイアウトデータID
   *  @param  area          転送先メモリ　（FCRAM/VRAMA/VRAMB）
   */
  //-----------------------------------------------------------------------------
  void TransferIndexTexture( u32 texArcIndex, gfl::grp::MemoryArea area );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  テクスチャリソースを取得
   *
   *  @param  lytDataID   レイアウトデータID
   *
   *  @retval テクスチャインフォデータ
   */
  //-----------------------------------------------------------------------------
  const LytResTextureInfo GetTexture( u32 lytDataID ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  テクスチャリソースを取得
   *
   *  @param  size リソースのサイズを受け取る変数へのポインタです。
   *               サイズを受け取る必要がない場合は NULL を渡してください。
   *
   *  @retval テクスチャリソースの先頭アドレスへのポインタ
   *
   *  @note   使用例
   *          u32 size;
   *          void* buffer = LytRes::GetTextureRes( lytDataID, &size );
   *          nw::lyt::TexResource tex_resource(buffer, *size);
   */
  //-----------------------------------------------------------------------------
  void* GetTextureRes( u32 lytDataID, u32* size ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  リソースアクセサーの取得
   */
  //-----------------------------------------------------------------------------
  LytResourceAccessor* GetResourceAccessor(void) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  ペイン名の取得
   */
  //-----------------------------------------------------------------------------
  const char * GetPaneName( u32 paneEnum )const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  ペイン名インデックス数を取得
   *
   *	@return ペイン名インデックス数
   */
  //-----------------------------------------------------------------------------
  u32 GetPaneNum( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  名前のインデックスを取得
   *
   *	@return インデックス
   */
  //-----------------------------------------------------------------------------
  u32 GetPaneNameIndex( const char* name ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  Defaultフォントを設定
   *
   *	@param	font    フォント
   *	@param  name    名称
   *	@param  index   インデックス(0～LYTRES_DEFAULT_FONT_MAX)
   */
  //-----------------------------------------------------------------------------
  void SetDefaultFont( gfl::str::Font* font, const char* name, u32 index = 0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  テクスチャを追加登録
   *
   *	@param	res     リソース
   */
  //-----------------------------------------------------------------------------
  void RegistTexture( const LytRes* res, gfl::grp::MemoryArea area );


private:
  // コンストラクタ・デストラクタ
  LytRes(void);
  ~LytRes();


  //----------------------------------------------------------------------------
  /**
   *	@brief  レイアウトリソースの読み込み
   *
   *  @param  cp_buff          バッファポインタ
   *	@param  addTextureNum   追加登録　テクスチャ数
   */
  //-----------------------------------------------------------------------------
  void Load( heap::NwAllocator* allocator, const void * cp_buff, u32 addTextureNum=0 );

	//----------------------------------------------------------------------------
	/**
	 *	@brief  レイアウトリソースの読み込み（圧縮データ対応版）
	 *
	 *	@param  allocator アロケータ
	 *	@param  arcFile   アーカイブファイル
	 *	@param  dataID    データID
	 *	@param	comp			圧縮フラグ　true = 圧縮データ
	 *	@param  addTextureNum   追加登録　テクスチャ数
	 */
	//-----------------------------------------------------------------------------
	void Load( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, bool comp, u32 addTextureNum=0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  レイアウトリソースの読み込み（非圧縮データ）
   *
   *	@param  allocator       アロケータ
   *	@param  arcFile         アーカイブファイル
   *	@param  dataID          データID
   *	@param  addTextureNum   追加登録　テクスチャ数
   */
  //-----------------------------------------------------------------------------
  void Load( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, u32 addTextureNum=0 );

	//----------------------------------------------------------------------------
	/**
	 *	@brief  レイアウトリソースの読み込み（圧縮データ）
	 *
	 *	@param  allocator アロケータ
	 *	@param  arcFile   アーカイブファイル
	 *	@param  dataID    データID
	 *	@param  addTextureNum   追加登録　テクスチャ数
	 */
	//-----------------------------------------------------------------------------
	void LoadComp( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, u32 addTextureNum=0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  レイアウトリソースの破棄
   */
  //-----------------------------------------------------------------------------
  void Release( void );




private:
  void*                           m_memory;
  const LytBinaryHeader*          m_buff;
  LytPaneBinaryHeader*            m_paneData;
  const char*                     m_nameTbl;
  void*                           m_arcData;
  nw::lyt::ArcResourceAccessor*   m_resAccessor;
  
  // Defaultフォント情報
  bool                            m_useDefaultFont[LYTRES_DEFAULT_FONT_MAX];
  nw::lyt::FontKey                m_fontKey[LYTRES_DEFAULT_FONT_MAX];

  // 追加登録テクスチャキー
  u16                             m_textureKeyNum;
  u16                             m_addTextureNum;
  nw::lyt::TextureKey*            m_textureKeyArray;
};



} // g2d
} // grp
} // gfl

#endif // __GFL_LYTRES_H__
