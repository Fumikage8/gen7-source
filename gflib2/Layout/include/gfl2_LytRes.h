//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file    gfl2_LytRes.h
 *  @brief   レイアウトリソースクラス
 *  @author  tomoya takahashi
 *  @date    2010.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( GFL2_LYTRES_H_INCLUDED )
#define GFL2_LYTRES_H_INCLUDED

#pragma once

#if defined(GF_PLATFORM_CTR)
#include <nn.h>
#include <nw/lyt2.h>
#elif defined(GF_PLATFORM_WIN32)
#include <nw/lyt.h>
#endif

#include <fs/include/gfl2_Fs.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Font.h>

#include <layout/include/gfl2_GraphicsType.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)

//-------------------------------------
///  前方宣言
//=====================================
class LytResManager;
class LytSys;
class LytSysBase;
class LytWkBase;
class LytWk;


//-----------------------------------------------------------------------------
/**
 *          定数宣言
 */
//-----------------------------------------------------------------------------
enum
{
  LYTRES_PANENAME_ERROR = 0xffff,  // ペイン名ない

  LYTRES_DEFAULT_FONT_MAX = 8,  // デフォルトフォント登録最大数
};
  
//-----------------------------------------------------------------------------
/**
 *          構造体宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          gfl::lyt::g2d::LytBinary構造
 */
//-----------------------------------------------------------------------------
typedef struct _LytBinaryHeader LytBinaryHeader;

//-----------------------------------------------------------------------------
/**
 *          gfl::lyt::g2d::LytPaneBinary構造
 */
//-----------------------------------------------------------------------------
typedef struct _LytPaneBinaryHeader LytPaneBinaryHeader;


//-----------------------------------------------------------------------------
/**
 *    リソースAccessor
 */
//-----------------------------------------------------------------------------
typedef nw::lyt::MultiArcResourceAccessor LytResourceAccessor;

//-----------------------------------------------------------------------------
/**
 *    テクスチャ情報
 */
//-----------------------------------------------------------------------------
typedef nw::lyt::TextureInfo LytResTextureInfo;

//-----------------------------------------------------------------------------
/**
 *          レイアウトリソースクラス宣言
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


private:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  LytRes(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  ~LytRes();


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースが読み込み済みかチェック
   *
   * @retval  true    読み込み済み
   * @retval  false   読み込まれていない
   */
  //-----------------------------------------------------------------------------
  bool IsDataIn(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   リソースアクセサ初期設定
   *
   * @param   heap            ヒープ
   * @param   multi_res_max   アクセサが保持できるアーカイブの数
   */
  //-----------------------------------------------------------------------------
  void InitMultiResource( gfl2::heap::HeapBase * heap, u32 multi_res_max );

  //-----------------------------------------------------------------------------
  /**
   * @brief   リソースアクセサ削除
   */
  //-----------------------------------------------------------------------------
  void ReleaseMultiResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   リソースアクセサ取得
   */
  //-----------------------------------------------------------------------------
  LytResourceAccessor * GetResourceAccessor(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースの読み込み（データ指定版）
   *
   * @param   allocator     ヒープアロケータ
   * @param   cp_buff       リソースデータ
   * @param   addTxtureNum  追加登録するテクスチャの数
   *
   * @return  リソース登録番号
   */
  //-----------------------------------------------------------------------------
  u32 Load( gfl2::heap::NwAllocator * allocator, const void * cp_buff, u32 addTextureNum );

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースの読み込み
   *
   * @param   pAsyncFileMan ファイルマネージャ
   * @param   allocator     ヒープアロケータ
   * @param   arcID         アーカイブIDarc_tool.lstファイル内に書かれているID(*.garcファイル自体のID(ARCID_*))
   * @param   dataID        アーカイブデータID(*.gaixファイル内に書かれているID(GARC_*))
   * @param   comp          圧縮フラグ ( true = 圧縮データ )
   * @param   addTextureNum 追加登録するテクスチャの数
   *
   * @return  リソース登録番号
   */
  //-----------------------------------------------------------------------------
  u32 Load( gfl2::fs::AsyncFileManager* pAsncFileManager, gfl2::heap::NwAllocator * allocator, gfl2::fs::ArcFile::ARCDATID arcID, gfl2::fs::ArcFile::ARCDATID dataID, bool comp, u32 addTextureNum );

  //-----------------------------------------------------------------------------
  /**
   * @brief   リソース登録数を取得
   *
   * @return  登録数
   */
  //-----------------------------------------------------------------------------
  u32 GetMultiResNum(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   テクスチャをすべてメモリに配置する
   *
   * @li  レイアウトに設定されたメモリを使用する
   */
  //-----------------------------------------------------------------------------
  void TransferAllTexture(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   テクスチャを追加する
   *
   * @param   dst_multi_id  追加先のリソース登録ID
   * @param   src_res       追加元のリソースデータ
   * @param   src_multi_id  追加元のリソース登録ID
   *
   * @li  レイアウトに設定されたメモリを使用する
   */
  //-----------------------------------------------------------------------------
  void RegistTexture( u32 dst_multi_id, const LytRes * src_res, u32 src_multi_id );

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースを取得
   *
   * @param   lytDataID   データID
   * @param   multi_id    リソース登録ID
   *
   * @return  レイアウトリソース ( ない場合はNULL )
   */
  //-----------------------------------------------------------------------------
  void * GetLayoutResource( u32 lytDataID, u32 multi_id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   アニメリソースを取得
   *
   * @param   lytDataID   データID
   * @param   multi_id    リソース登録ID
   *
   * @return  アニメリソース ( ない場合はNULL )
   */
  //-----------------------------------------------------------------------------
  void * GetAnimeResource( u32 lytDataID, u32 multi_id ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  テクスチャリソースを取得
   *
   * @param   lytDataID   データID
   * @param   size        テクスチャサイズ格納場所 ( 不要の場合はNULL )
   * @param   multi_id    リソース登録ID
   *
   * @return  テクスチャリソース ( ない場合はNULL )
   *
   * @note    使用例
   *          u32 size;
   *          void* buffer = LytRes::GetTextureRes( lytDataID, &size, multi_id );
   *          nw::lyt::TexResource tex_resource(buffer, *size);
   */
  //-----------------------------------------------------------------------------
  void * GetTextureResource( u32 lytDataID, u32 * size, u32 multi_id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   テクスチャ情報を取得
   *
   * @param   lytDataID   データID
   * @param   multi_id    リソース登録ID
   *
   * @return  テクスチャ情報
   */
  //-----------------------------------------------------------------------------
  const LytResTextureInfo * GetTexture( u32 lytDataID, u32 multi_id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   ペイン名の取得
   *
   * @param   paneEnum    ペイン番号
   * @param   multi_id    リソース登録ID
   *
   * @return  ペイン名
   */
  //-----------------------------------------------------------------------------
  const char * GetPaneName( u32 paneEnum, u32 multi_id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   ペイン名インデックス数を取得
   *
   * @param   multi_id  リソース登録ID
   *
   * @return  ペイン名インデックス数
   */
  //-----------------------------------------------------------------------------
  u32 GetPaneNum( u32 multi_id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief  名前のインデックスを取得
   *
   * @param   name      名前
   * @param   multi_id  リソース登録ID
   *
   * @retval  インデックス
   * @retval  LYTRES_PANENAME_ERROR そんな名前ない
   */
  //-----------------------------------------------------------------------------
  u32 GetPaneNameIndex( const char * name, u32 multi_id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   Defaultフォントを設定
   *
   * @param   font    フォント
   * @param   name    名称
   * @param   index   インデックス ( 0 〜 LYTRES_DEFAULT_FONT_MAX )
   */
  //-----------------------------------------------------------------------------
  void SetDefaultFont( gfl2::str::Font * font, const char * name, u32 index = 0 );


private:
  //-----------------------------------------------------------------------------
  /**
   * @brief   リソースセット
   *
   * @param   allocator     ヒープアロケータ
   * @param   cp_buff       リソースデータ
   * @param   addTxtureNum  追加登録するテクスチャの数
   */
  //-----------------------------------------------------------------------------
  void SetResource( gfl2::heap::NwAllocator * allocator, const void * cp_buff, u32 addTextureNum );

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースの読み込み（非圧縮データ）
   *
   * @param   pAsyncFileMan ファイルマネージャ
   * @param   allocator     ヒープアロケータ
   * @param   arcID         アーカイブID(arc_tool.lstファイル内に書かれているID(*.garcファイル自体のID(ARCID_*)))
   * @param   dataID        アーカイブデータID(*.gaixファイル内に書かれているID(GARC_*))
   * @param   addTextureNum 追加登録するテクスチャの数
   *
   * @note    Open,Closeは使用する側で行ってください
   */
  //-----------------------------------------------------------------------------
  void LoadNormal( gfl2::fs::AsyncFileManager* pAsyncFileMan, gfl2::heap::NwAllocator * allocator, gfl2::fs::ArcFile::ARCID arcID, gfl2::fs::ArcFile::ARCDATID dataID, u32 addTextureNum );

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースの読み込み（圧縮データ）
   *
   * @param   pAsyncFileMan ファイルマネージャ
   * @param   allocator     ヒープアロケータ
   * @param   arcID         アーカイブID(arc_tool.lstファイル内に書かれているID(*.garcファイル自体のID(ARCID_*)))
   * @param   dataID        アーカイブデータID(*.gaixファイル内に書かれているID(GARC_*))
   * @param   addTextureNum 追加登録するテクスチャの数
   *
   * @note    Open,Closeは使用する側で行ってください
   */
  //-----------------------------------------------------------------------------
  void LoadComp( gfl2::fs::AsyncFileManager* pAsyncFileMan, gfl2::heap::NwAllocator * allocator, gfl2::fs::ArcFile::ARCID arcID, gfl2::fs::ArcFile::ARCDATID dataID, u32 addTextureNum );

  //-----------------------------------------------------------------------------
  /**
   * @brief   テクスチャをメモリに配置する
   *
   * @param   multi_id    リソース登録ID
   * @param   texArcIndex テクスチャレイアウトデータID
   *
   * @li  レイアウトに設定されたメモリを使用する
   */
  //-----------------------------------------------------------------------------
  void TransferIndexTexture( u32 multi_id, u32 texArcIndex );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル数取得
   *
   * @param   multi_id  リソース登録ID
   *
   * @return  ファイル数
   */
  //-----------------------------------------------------------------------------
  u32 GetFileNum( u32 multi_id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル名取得
   *
   * @param   lytDataID   データID
   * @param   multi_idx   リソース登録ID
   *
   * @return  ファイル名
   */
  //-----------------------------------------------------------------------------
  const char * GetFileName( u32 lytDataID, u32 multi_id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   テクスチャーをアンロードする
   * @param   LytTextureInfo*    テクスチャー情報のポインタ
 */
  //-----------------------------------------------------------------------------
  void UnLoad(LytResTextureInfo* pInfo);

private:
  LytResourceAccessor * m_resAccessor;    //!< リソースアクセサ

  //! リソースデータ
  struct MultiResource
  {
    void * memory;                              //!< 読み込んだデータ ( 外部指定時はNULL )
    const LytBinaryHeader * buff;               //!< 生データ
    const char * nameTbl;                       //!< ファイル名リスト
    LytPaneBinaryHeader * paneData;             //!< ペイン名リスト
    nw::lyt::ArcResourceLink * link;            //!< アーカイブ情報を保持するクラス

    LytResTextureInfo ** textureInfoArray;      //!< テクスチャ情報
    LytResTextureInfo ** addTextureInfoArray;   //!< 追加登録テクスチャ情報
    u16 addTextureMax;                          //!< 追加登録できるテクスチャ数
    u16 addTextureNum;                          //!< 追加登録したテクスチャ数
  };

  MultiResource * m_multiResource;  //!< リソースデータ
  u16 m_multiResMax;                //!< 登録できるリソースデータ数
  u16 m_multiResNum;                //!< 登録したリソースデータ数
  
  // Defaultフォント情報
  bool m_useDefaultFont[LYTRES_DEFAULT_FONT_MAX];
  nw::lyt::FontKey m_fontKey[LYTRES_DEFAULT_FONT_MAX];
};


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

#endif  // GFL2_LYTRES_H_INCLUDED
