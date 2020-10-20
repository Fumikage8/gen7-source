//===========================================================================
/**
 * @file  gfl2_MsgData.h
 * @brief GAMEFREAK Library  String Print System ( メッセージデータバイナリ取り扱いクラス )
 * @author  GAMEFREAK inc.
 * @date    2010.10.27
 */
//===========================================================================
#if !defined( GFL2_MSGDATA_H_INCLUDED )
#define GFL2_MSGDATA_H_INCLUDED
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_StrBuf.h>
#include <fs/include/gfl2_FsArcFile.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
class AsyncFileManager;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

#ifdef GF_PLATFORM_WIN32
//Win32の場合はWarning抑止のpragmaで囲む
#pragma warning(push)
#pragma warning(disable:4200) //構造体定義中のsize0の配列を許容する
#endif


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)


class MsgData {
  // コピーコンストラクタ＆代入演算子の禁止
  GFL_FORBID_COPY_AND_ASSIGN( MsgData );

public:
  //------------------------------------------------------
  /**
   *  データロード形式指定
   */
  //------------------------------------------------------
  enum LoadType {
    LOAD_PART,    ///< 要求された部分だけを逐次メモリに読み込む
    LOAD_FULL     ///< 最初に一括して全てメモリ上に読み込む
  };

  //----------------------------------------------------------------------
  /**
   * コンストラクタ＆デストラクタ
   */
  //----------------------------------------------------------------------
  MsgData( const char* arcFilePath, gfl2::fs::ArcFile::ARCDATID datID, gfl2::heap::HeapBase* pHeap, LoadType loadType = LOAD_PART );
  MsgData( gfl2::fs::ArcFile::ARCID arcID, gfl2::fs::ArcFile::ARCDATID datID, gfl2::heap::HeapBase* pHeap, LoadType loadType = LOAD_PART );
  MsgData( const void* dataAdress, gfl2::heap::HeapBase* pHeap );
  virtual ~MsgData();

  //----------------------------------------------------------------------
  /**
   * @brief   指定文字列をバッファに読み込む
   *
   * @param[in]    strID   文字列ID
   * @param[out]   dst     読み込み先文字列バッファ
   *
   * @retval  u16    ユーザー設定パラメータ
   */
  //----------------------------------------------------------------------
  u16 GetString( u32 strID, StrBuf& dst );

  //----------------------------------------------------------------------
  /**
   * @brief   一時的に言語を切り替えて指定文字列をバッファに読み込む
   *
   * @param[in]    strID   文字列ID
   * @param[in]    langID  言語
   * @param[out]   dst     読み込み先文字列バッファ
   *
   * @retval  u16    ユーザー設定パラメータ
   */
  //----------------------------------------------------------------------
  u16 GetStringByLanguage( u32 strID, u32 langID, StrBuf& dst );

  //----------------------------------------------------------------------
  /**
   * @brief   データに含まれる文字列数を取得
   *
   * @retval  u32    文字列数
   */
  //----------------------------------------------------------------------
  u32 GetStrCount( void ) const;

  //----------------------------------------------------------------------
  /**
   * @brief   ユーザー設定パラメータを取得
   *
   * @param[in]    strID   文字列ID
   *
   * @return  u16      ユーザー設定パラメータ
   */
  //----------------------------------------------------------------------
  u16 GetUserParam( u32 strID );


private:

  /**
   *  暗号状況
   */
  enum Coded {
    DATA_CODED = 0,
    DATA_NO_CODED = 1,
  };

  /**
   *  コンストラクタ処理下請け
   */
  void construct_common( gfl2::fs::ArcFile::ARCDATID datID, LoadType loadType );

  /*------------------------------------------------------------*/
  /** メッセージデータヘッダ                                    */
  /*------------------------------------------------------------*/
  typedef struct {
    u16   numLangs;         // 格納言語種類数（かな＆漢字なら２）
    u16   numStrings;       // １言語辺りの文字列数
    u32   maxLangBlockSize; // 最も大きい言語ブロックのバイトサイズ
    Coded reserved;         // 予約領域（暗号化とかに使うかも）
    u32   ofsLangBlocks[0]; // 言語ごとのブロックオフセット値（ファイル先頭から，バイト単位）
  }MSG_HEADER_BLOCK;

  /*------------------------------------------------------------*/
  /** 文字列パラメータ                                          */
  /*------------------------------------------------------------*/
  typedef struct {
    u32   offset;     ///< 言語ブロック先頭からのバイトオフセット
    u16   len;        ///< 文字数（EOMを含む）
    u16   userParam;  ///< ユーザー指定パラメータ
  }MSG_STRPARAM_BLOCK;

  /*------------------------------------------------------------*/
  /** 言語ブロックヘッダ                                        */
  /*------------------------------------------------------------*/
  typedef struct {
    u32                     size;       // 当ブロックのバイトサイズ
    MSG_STRPARAM_BLOCK      params[0];  // 文字列パラメータ配列
  }MSG_LANG_BLOCK;

  static  u32           s_CurrentLangID;

  gfl2::heap::HeapBase*       m_pHeap;
  LoadType                    m_loadType;
  gfl2::fs::ArcFile::ARCDATID m_dataID;
  u32                         m_targetLangID;
  const MSG_HEADER_BLOCK*     m_pDataHeader;
  MSG_LANG_BLOCK*             m_pLangHeader;
  MSG_STRPARAM_BLOCK          m_strParamBody;
  gfl2::fs::AsyncFileManager * m_pFileManager;
  char * m_fileName;
  gfl2::fs::ArcFile::ARCID m_arcID;

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateFileName
   * @brief   ファイル名を保存
   * @date    2014.10.28
   *
   * @param   file  ファイル名
   */
  //-----------------------------------------------------------------------------
  void CreateFileName( const char * file );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteFileName
   * @brief   ファイル名を削除
   * @date    2014.10.28
   */
  //-----------------------------------------------------------------------------
  void DeleteFileName(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFileManager
   * @brief   ファイル読み込みマネージャ初期設定
   * @date    2014.10.28
   */
  //-----------------------------------------------------------------------------
  void InitFileManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFileManager
   * @brief   ファイル読み込みマネージャ終了設定
   * @date    2014.10.28
   */
  //-----------------------------------------------------------------------------
  void EndFileManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    FileSeek
   * @brief   ファイルシーク
   * @date    2014.10.28
   *
   * @param   dataID  アークデータID
   * @param   offset  データIDからのオフセット
   */
  //-----------------------------------------------------------------------------
  //void FileSeek( gfl2::fs::ArcFile::ARCDATID dataID, size_t offset );  // 【使用禁止】代わりにFileSeekLoadDataPtrBufを使って下さい！

  //-----------------------------------------------------------------------------
  /**
   * @func    FileLoadDataPtrBuf
   * @brief   シークしたポインタからデータ読み込む
   * @date    2014.10.28
   *
   * @param   buf         読み込み場所
   * @param   read_size   読み込みサイズ
   */
  //-----------------------------------------------------------------------------
  //void FileLoadDataPtrBuf( void * buf, size_t read_size );  // 【使用禁止】代わりにFileSeekLoadDataPtrBufを使って下さい！

  //-----------------------------------------------------------------------------
  /**
   * @func    FileSeekLoadDataPtrBuf
   * @brief   ファイルシーク＆シークしたポインタからデータ読み込む
   * @date    2016.05.23
   *
   * @param   dataID  アークデータID
   * @param   offset  データIDからのオフセット
   * @param   buf         読み込み場所
   * @param   read_size   読み込みサイズ
   */
  //-----------------------------------------------------------------------------
  void FileSeekLoadDataPtrBuf( gfl2::fs::ArcFile::ARCDATID dataID, size_t offset, void * buf, size_t read_size );


  /**
   *
   */
  MSG_HEADER_BLOCK* loadDataHeader( gfl2::fs::ArcFile::ARCDATID dataID );



  //--------------------------------------------------------------------------
  /**
   *
   */
  //--------------------------------------------------------------------------
  void loadLangHeader(
        MSG_LANG_BLOCK** ppLangHeader, const MSG_HEADER_BLOCK* pDataHeader, gfl2::fs::ArcFile::ARCDATID dataID, u32 langID );

  //--------------------------------------------------------------------------
  /**
   *  読み込むべき言語IDを取得する
   */
  //--------------------------------------------------------------------------
  u32 getCurrentLangID( const MSG_HEADER_BLOCK* dataHeader ) const;


  //--------------------------------------------------------------------------
  /**
   *  指定された言語IDのデータ内オフセット（byte）を取得する
   */
  //--------------------------------------------------------------------------
  u32 getTargetLangBlockOfs( const MSG_HEADER_BLOCK* dataHeader, u32 langID ) const;

  //--------------------------------------------------------------------------
  /**
   * 読み込み対象の言語IDが変更されているかどうかチェック
   * 変更されていれば、必要に応じて内部データを更新する。
   */
  //--------------------------------------------------------------------------
  void checkTargetLangChanged( void );

  //--------------------------------------------------------------------------
  /**
   * 指定文字列のパラメータ部分ポインタを返す
   */
  //--------------------------------------------------------------------------
  const MsgData::MSG_STRPARAM_BLOCK* getStrParamPointer( u32 strID );


  //--------------------------------------------------------------------------
  /**
   * 文字列データを、StrBufクラスにセットする（暗号済みデータ対応）
   */
  //--------------------------------------------------------------------------
  void putToStrBuf( const wchar_t* src, u32 strLen, u32 strID, StrBuf& dst ) const;


public:
  /*============================================================================================*/
  /*                                                                                            */
  /*  ロードする言語の切り替え処理                                                              */
  /*                                                                                            */
  /*  同一の文字列IDを使って、実際に読み出される文字列を切り替えるための仕組み。                */
  /*  日本語版の『ひらがな-漢字』切り替えとか、                                                */
  /*  欧州版の『英・仏・独・伊・西』５ヶ国語切り替えとかに使ってもらうことを想定。              */
  /*                                                                                            */
  /*============================================================================================*/

  //--------------------------------------------------------------------------------------
  /**
   * @brief  ロード対象となる言語IDを設定する。以降、MsgData クラスを介して取得できる文字列が切り替わる。
   */
  //--------------------------------------------------------------------------------------
  static void SetLangID( u32 langID );

  //--------------------------------------------------------------------------------------
  /**
   * @brief  ロード対象となっている言語IDを取得する。
   */
  //--------------------------------------------------------------------------------------
  static u32 GetLangID( void );

};

GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

#ifdef GF_PLATFORM_WIN32
#pragma warning(pop)
#endif

#endif // GFL2_MSGDATA_H_INCLUDED
