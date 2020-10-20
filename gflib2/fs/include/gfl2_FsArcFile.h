#if !defined(GFL2_FSARCFILE_H_INCLUDED)
#define GFL2_FSARCFILE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsArcFile.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   garcファイル
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// fsのインクルード
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>
#include <fs/include/gfl2_FsResult.h>
#include <fs/include/internal/gfl2_FsFileBase.h>


#if GFL_FS_GASC_ENABLE
// 前方宣言
namespace arcsrc
{
class Accessor;
}
#endif


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


// 前方宣言
class DeviceBase;
class IArcFileAccessor;


//==============================================================================
/**
 @class     ArcFile
 @brief     garcファイル
 */
//==============================================================================
class ArcFile : public FileBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ArcFile);

  friend class FileThread;
  friend class AsyncFileManager;

public:
  //! @brief  型
  typedef u32 ARCID;       //!< arc_tool.lstファイル内に書かれているID(*.garcファイル自体のID(ARCID_*))
  typedef u32 ARCDATID;    //!< *.gaixファイル内に書かれているID(GARC_*)
  
  //! @brief  定数
  static const u32 ARCID_NULL    = 0xffffffff;    //!< ARCIDのNULL値
  static const u32 ARCDATID_NULL = 0xffffffff;    //!< ARCDATIDのNULL値

  //! @brief  *.garcファイル内のデータにアクセスするための情報(対象言語のみ)
  struct AccessInfo 
  {
    u32 dataSize;             //!< データサイズ(パディングを含む)
    u32 realDataSize;         //!< データサイズ(パディングを含まない)
    u32 offsetFromArchiveTop; //!< *.garcファイルの先頭からのオフセット
  };


private:
  //! @brief  最新バージョン
  static const u8 LATEST_MAJOR_VERSION;


private:
  //! @name  friendからアクセスする
  //@{

  //! @brief  コンストラクタ
  ArcFile(void);
  //! @brief  デストラクタ
  virtual ~ArcFile();

  //@}


private:
  //! @name  friendからアクセスする
  //@{
  
  //! @name  *.garcファイル(arc)に対する操作
  //@{

  //! @brief  ファイルを開き、索引テーブルを読み込む
  //! @param[in] heapForFile  ヒープ。ファイル名の記憶、ファイル核の生成に使用。索引テーブルの作成にも使用。Closeするまで左記のものが保持される。
  //! @param[in] heapForTemp  索引テーブルの作成のために一時的に使用するヒープ。使用したメモリはこの関数内で解放しています。
  //! @param[in] device       デバイス。
  //! @param[in] fileName     ファイル名。終端は'\0'になっていること。
  //! @attention  ファイルのアクセス位置が移動する
  Result Open(gfl2::heap::HeapBase* heapForFile, gfl2::heap::HeapBase* heapForTemp, const DeviceBase* device, const char* fileName);

  //! @brief  ファイルを閉じる
  virtual Result Close(void);

  //@}

  //@}


  //! @name  *.garcファイル内のデータ(arcdat)に対する操作
  //@{

public:
  //! @brief アーカイブに含まれるデータの数を取得する
  Result GetDataCount(u32* pDataNum) const;    // *.garcファイル内のデータ数(言語違いのデータがあるときは言語違いをまとめて1個というふうにカウントしている)

  //! @brief *.garcファイル内のデータ1件分の( パディングを含む )サイズを取得する。ArcSrcを使うときはファイルサイズを返すだけ。
  //! @param datId        アーカイブファイル内データID
  //! @param heapForTemp  ArcSrcを使うときだけ使用(使わないときはNULLでよい)。ファイルオープンのために一時的に使用するヒープ。使用したメモリはこの関数内で解放しています。
  Result GetDataSize(size_t* pDataSize, ARCDATID datId, gfl2::heap::HeapBase* heapForTemp) const; 
  Result GetDataSize(size_t* pDataSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForTemp) const; 

  //! @brief *.garcファイル内のデータ1件分の( パディングを含まない )サイズを取得する。ArcSrcを使うときはファイルサイズを返すだけ。
  //! @param datId        アーカイブファイル内データID
  //! @param heapForTemp  ArcSrcを使うときだけ使用(使わないときはNULLでよい)。ファイルオープンのために一時的に使用するヒープ。使用したメモリはこの関数内で解放しています。
  //! @note データを読み込むために必要なバッファサイズは, パディングを含んだサイズです。
  //!       データを読み込むためのデータサイズは GetDataSize() で取得してください。
  Result GetRealDataSize(size_t* pRealDataSize, ARCDATID datId, gfl2::heap::HeapBase* heapForTemp) const; 
  Result GetRealDataSize(size_t* pRealDataSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForTemp) const; 

  //! @note  ArcSrcを使うときでもArcSrcは使わず、*.garcを使います。
  Result GetOffsetFromArchiveTop(size_t* pOffsetFromArchiveTop, ARCDATID datId) const;
  Result GetOffsetFromArchiveTop(size_t* pOffsetFromArchiveTop, ARCDATID datId, ToolDefine::ArcLangType::Tag lang) const;

  //! @brief *.garcファイルに含まれるデータのうち, 最も大きなデータのサイズを取得する( パディングを含む )
  //! @note  ArcSrcを使うときでもArcSrcは使わず、*.garcを使います。
  //! @attention  このメソッドは, ver.5.0 以降のアーカイブファイルでのみ有効となります。
  //!             ver.4.x までのアーカイブファイルでは必ず0が返ります。
  Result GetMaxDataSize(size_t* pDataSize) const;          // *.garcファイル内のデータのうちで最も大きなもののサイズ(言語違いのデータがあるときは全ての言語を通して最も大きいデータサイズ)

  //! @brief *.garcファイルに含まれるデータのうち, 最も大きなデータのサイズを取得する( パディングを含まない )
  //! @note  ArcSrcを使うときでもArcSrcは使わず、*.garcを使います。
  Result GetMaxRealDataSize(size_t* pRealDataSize) const;  // *.garcファイル内のデータのうちで最も大きなもののサイズ(言語違いのデータがあるときは全ての言語を通して最も大きいデータサイズ)

private:
  //! @name  friendからアクセスする
  //@{

  //! @brief *.garcファイル内のデータを1件分読み込む
  //! @param[out] pRealReadSize  実際に読み込んだサイズ。パディングを含まないサイズです。
  //! @param[out] pBuf           呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。
  //! @param[in]  bufSize        bufのサイズ。データサイズ(パディングを含む)以上にして下さい。
  //! @param[in]  datId          アーカイブファイル内データID
  //! @param heapForTemp         ArcSrcを使うときだけ使用(使わないときはNULLでよい)。ファイルオープンのために一時的に使用するヒープ。使用したメモリはこの関数内で解放しています。
  //! @attention  ファイルのアクセス位置が移動する
  Result LoadData(size_t* pRealReadSize, void* pBuf, size_t bufSize, ARCDATID datId, gfl2::heap::HeapBase* heapForTemp);
  Result LoadData(size_t* pRealReadSize, void* pBuf, size_t bufSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForTemp);
 
  //! @brief *.garcファイル内のデータを1件分読み込む
  //! @param[out] ppBuf          heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。
  //! @param[out] pBufSize       heapForBufから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。パディングを含むサイズ確保します。
  //! @param[out] pRealReadSize  実際に読み込んだサイズ。パディングを含まないサイズです。
  //! @param[in]  datId          アーカイブファイル内データID
  //! @param[in]  heapForBuf     ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
  //! @param[in]  align          アラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。
  //! @param heapForTemp         ArcSrcを使うときだけ使用(使わないときはNULLでよい)。ファイルオープンのために一時的に使用するヒープ。使用したメモリはこの関数内で解放しています。
  //! @attention  ファイルのアクセス位置が移動する
  Result LoadData(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, ARCDATID datId, gfl2::heap::HeapBase* heapForBuf, u32 align, gfl2::heap::HeapBase* heapForTemp);
  Result LoadData(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForBuf, u32 align, gfl2::heap::HeapBase* heapForTemp);

  //! @brief *.garcファイル内のデータ1件の一部を読み込む
  //! @param[out] pRealReadSize   実際に読み込んだバイト数
  //! @param[out] pBuf            呼び出し元であらかじめメモリを確保しておいた 読み込んだデータの格納先( 4 byte alignment )
  //! @param[in]  bufSize         pBufのサイズ。readSize以上にして下さい。
  //! @param[in]  datId           アーカイブファイル内データID
  //! @param[in]  offsetFromData  読み込み開始オフセット(byte)(datIdデータ先頭からのオフセット)
  //! @param[in]  readSize        読み込みサイズ(byte)
  //! @attention  ファイルのアクセス位置が移動する
  //! @note  ArcSrcを使うときでもArcSrcは使わず、*.garcを使います。。
  Result LoadDataPiece(size_t* pRealReadSize, void* pBuf, size_t bufSize, ARCDATID datId, size_t offsetFromData, size_t readSize ); 
  Result LoadDataPiece(size_t* pRealReadSize, void* pBuf, size_t bufSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, size_t offsetFromData, size_t readSize ); 

  //! @brief *.garcファイル内のデータ1件の読み込みポインタをシークする( 取り扱い注意！)
  //! @param[in] datId           アーカイブファイル内データID
  //! @param[in] offsetFromData  シークサイズ(byte)(datIdデータ先頭からのオフセット)
  //! @attention  ファイルのアクセス位置が移動する
  //! @note  ArcSrcを使うときでもArcSrcは使わず、*.garcを使います。。
  Result SeekDataPtr( ARCDATID datId, size_t offsetFromData );
  Result SeekDataPtr( ARCDATID datId, ToolDefine::ArcLangType::Tag lang, size_t offsetFromData );

  //! @brief SeekDataPtr()でシークしたポインタからデータ読み込む( 取り扱い注意！)
  //! @param[out] pRealReadSize  実際に読み込んだバイト数
  //! @param[out] pBuf           呼び出し元であらかじめメモリを確保しておいた 読み込んだデータの格納先( 4 byte alignment )
  //! @param[in]  bufSize        pBufのサイズ。readSize以上にして下さい。
  //! @param[in]  readSize       読み込みサイズ
  //! @attention  ファイルのアクセス位置が移動する
  //! @note  ArcSrcを使うときでもArcSrcは使わず、*.garcを使います。。
  Result LoadDataPtr( size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize );

  //@}

  //@}


private:
  //! @name  friendからアクセスする
  //@{

  //! @name  *.gascファイル(ArcSrc)に対する操作
  //@{

  //! @brief  ArcSrcアクセッサを生成する(ArcSrcファイルのオープン、読み込み、クローズを行う)
  //! @param[in] heapForArcSrc  ヒープ。ArcSrcのアクセッサインスタンスの作成とファイル丸ごとからつくるソーステーブルの作成に使用。Destroyするまで左記のものが保持される。
  //! @param[in] heapForTemp    ArcSrcファイルの操作で一時的に使用するヒープ。使用したメモリはこの関数内で解放しています。
  //! @param[in] device         デバイス。
  //! @param[in] fileName       ArcSrcファイル名。終端は'\0'になっていること。
  Result CreateArcSrcAccessor(gfl2::heap::HeapBase* heapForArcSrc, gfl2::heap::HeapBase* heapForTemp, const DeviceBase* device, const char* arcSrcFileName);

  //! @brief  ArcSrcアクセッサを破棄する
  virtual Result DestroyArcSrcAccessor(void);

  //! @brief  ArcSrcを使うか否か
  //! @param[in]  useFlag  使う場合true、使わない場合false
  void SetArcSrcUseFlag(bool useFlag);
  bool GetArcSrcUseFlag(void) const;

  //@}

  //@}


private:
  //! @name  friendからアクセスする
  //@{
  //! @brief  ARCIDを設定する
  void SetArcId(ARCID arcId) { m_arcId = arcId; }
  //! @brief  ARCIDを取得する
  ARCID GetArcId(void) const { return m_arcId; }
  //! @brief  対象言語を設定する
  void SetArcLang(ToolDefine::ArcLangType::Tag arcLang) { m_arcLang = arcLang; }  // 索引テーブルを作成したときのものから変更できない。
  //@}


private:
#if GFL_DEBUG
  //! @name  *.garcファイルが正しいかチェック
  //@{
  Result checkSignatureAndVersion(const char* fileName);
          static Result checkSignature(const char* fileName, const u8* buf);
          static Result checkVersion(u8* a_majorVersion, u8* a_minorVersion, const char* fileName, const u8* buf);
  //@}
#endif

  //! @name  索引テーブル、ArcFileAccessorの生成
  //@{

  Result setupForFastMode(gfl2::heap::HeapBase* heapForFile, gfl2::heap::HeapBase* heapForTemp);

          /**
           * @brief アーカイブファイルへのアクセッサを生成する
           * @param heap アクセッサの確保に使用するヒープ
           * @return 生成したアクセッサ
           */
          static IArcFileAccessor* createArcFileAccessor(gfl2::heap::HeapBase* heap, u8 majorVersion);
          
          Result createBlockData(void** pBlockData, gfl2::heap::HeapBase* heap, const IArcFileAccessor* accessor);
          
          /**
           * @brief データブロックの合計サイズを読み込む
           * @return 自身のデータブロックの合計サイズ
           */
          Result readTotalBlockSize(size_t* totalBlockSize, const IArcFileAccessor* accessor);
          
          /**
           * @brief アーカイブ内データへのアクセステーブルを生成する
           * @param heap      テーブルの生成に使用するヒープ
           * @param accessor  アーカイブファイルのアクセッサ
           * @return 生成したアクセステーブル
           */
          static AccessInfo* createDataAccessTable(gfl2::heap::HeapBase* heap, const IArcFileAccessor* accessor, ToolDefine::ArcLangType::Tag arcLang);

  //@}


#if GFL_FS_GASC_ENABLE
  //! @name  *.garcファイル内のデータ(arcdat)に対する操作を、*.gascファイル(ArcSrc)を使ってソースファイルに対する操作に変える
  //@{
  
  bool doesUseArcSrc(void) const;

  Result getArcSrcDataSize(size_t* pDataSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForTemp) const; 
  Result loadArcSrcData(size_t* pRealReadSize, void* pBuf, size_t bufSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForTemp);
  Result loadArcSrcData(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, ARCDATID datId, ToolDefine::ArcLangType::Tag lang, gfl2::heap::HeapBase* heapForBuf, u32 align, gfl2::heap::HeapBase* heapForTemp);

  //@}
#endif


private:
  ARCID                         m_arcId;
  ToolDefine::ArcLangType::Tag  m_arcLang;          // 対象言語
 
#if GFL_DEBUG
  u8                           m_majorVersion;
  u8                           m_minorVersion;
#endif

  // m_arcLang==ToolDefine::ArcLangType::ALL_LANGのとき
  IArcFileAccessor*            m_accessor;         // ArcFileAccessor
  void*                        m_blockData;        // ブロックデータ

  // m_arcLang!=ToolDefine::ArcLangType::ALL_LANGのとき
  AccessInfo*                  m_fastAccessTable;  // 索引テーブル
  
  u32                          m_dataNum;          // *.garcファイル内のデータ数(言語違いのデータがあるときは言語違いをまとめて1個というふうにカウントしている)  // dataCount
  u32                          m_maxDataSize;      // *.garcファイル内のデータのうちで最も大きなもののサイズ(言語違いのデータがあるときは全ての言語を通して最も大きいデータサイズ)(パディングを含むサイズ)
  u32                          m_maxRealDataSize;  // *.garcファイル内のデータのうちで最も大きなもののサイズ(言語違いのデータがあるときは全ての言語を通して最も大きいデータサイズ)(パディングを含んでいないサイズ)

#if GFL_FS_GASC_ENABLE
  // 「garcのソースを記録したファイル」に関する変数
  void*                        m_arcSrcBuf;
  arcsrc::Accessor*            m_arcSrcAccessor;  // NULLのときはArcSrcを使えない。
  bool                         m_arcSrcUseFlag;
#endif
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSARCFILE_H_INCLUDED

