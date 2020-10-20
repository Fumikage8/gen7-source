#ifndef __GFL_MSGDATA_H__
#define __GFL_MSGDATA_H__
//===========================================================================
/**
 * @file  gfl_MsgData.h
 * @brief GAMEFREAK Library  String Print System ( メッセージデータバイナリ取り扱いクラス )
 * @author  GAMEFREAK inc.
 * @date    2010.10.27
 */
//===========================================================================
#pragma once

#include <gfl_Common.h>
#include <gfl_Heap.h>
#include <gfl_Fs.h>
#include <str/gfl_StrBuf.h>

namespace gfl {
namespace str {



class MsgData {
  // コピーコンストラクタ＆代入演算子の禁止
  GFL_FORBID_COPY_AND_ASSIGN( MsgData );

public:
  //------------------------------------------------------
  /**
   *  データロード形式指定
   */
  //------------------------------------------------------
  typedef enum {
    LOAD_PART,    ///< 要求された部分だけを逐次メモリに読み込む
    LOAD_FULL     ///< 最初に一括して全てメモリ上に読み込む
  }LoadType;

  //----------------------------------------------------------------------
  /**
   * コンストラクタ＆デストラクタ
   */
  //----------------------------------------------------------------------
  MsgData( const wchar_t* arcFilePath, u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType = LOAD_PART, u32 arc_init_flag = gfl::fs::ArcFile::OPEN_FASTMODE );
  MsgData( gfl::fs::ArcFile::ARCID arcID, u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType = LOAD_PART, u32 arc_init_flag = gfl::fs::ArcFile::OPEN_FASTMODE );
  MsgData( const void* dataAdress, gfl::heap::HeapBase* pHeap );
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
  enum {
    DATA_CODED = 0,
    DATA_NO_CODED = 1,
  };

  /**
   *  コンストラクタ処理下請け
   */
  void construct_common( u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType );

  /*------------------------------------------------------------*/
  /** メッセージデータヘッダ                                    */
  /*------------------------------------------------------------*/
  typedef struct {
    u16   numLangs;         // 格納言語種類数（かな＆漢字なら２）
    u16   numStrings;       // １言語辺りの文字列数
    u32   maxLangBlockSize; // 最も大きい言語ブロックのバイトサイズ
    u32   reserved;         // 予約領域（暗号化とかに使うかも）
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


  gfl::heap::HeapBase*        m_pHeap;
  gfl::fs::ArcFile*           m_pArc;
  LoadType                    m_loadType;
  u32                         m_dataID;
  u32                         m_targetLangID;
  const MSG_HEADER_BLOCK*     m_pDataHeader;
  MSG_LANG_BLOCK*             m_pLangHeader;
  MSG_STRPARAM_BLOCK          m_strParamBody;


  /**
   *
   */
  MSG_HEADER_BLOCK*
    loadDataHeader( gfl::fs::ArcFile* pArc, u32 dataID, gfl::heap::HeapBase* pHeap );



  //--------------------------------------------------------------------------
  /**
   *
   */
  //--------------------------------------------------------------------------
  void
    loadLangHeader( MSG_LANG_BLOCK** ppLangHeader, const MSG_HEADER_BLOCK* pDataHeader,
        gfl::fs::ArcFile* pArc, u32 dataID, u32 langID );

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
  /*  日本語版の『ひらがな－漢字』切り替えとか、                                                */
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

} /* end of namespace str */
} /* end of namespace gfl */



#endif // __GFL_MSGDATA_H__
