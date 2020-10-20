#if !defined( __BTL_REC_H__ )
#define __BTL_REC_H__
//=============================================================================================
/**
 * @file  btl_rec.h
 * @brief ポケモンXY バトルシステム クライアント操作記録データ生成
 * @author  taya
 *
 * @date  2010.12.29  作成
 */
//=============================================================================================
#pragma once

#include "btl_common.h"
#include "btl_action.h"



GFL_NAMESPACE_BEGIN(btl)
namespace rec {



typedef enum {
  BTL_RECFIELD_NULL = 0,
  BTL_RECFIELD_ACTION,          ///< 通常行動選択
  BTL_RECFIELD_BTLSTART,        ///< バトル開始
  BTL_RECFIELD_TIMEOVER,        ///< 時間制限が来たので終了
  BTL_RECFIELD_SIZEOVER,        ///< これ以上コマンド記憶できないので終了
}FieldType;

/**
 *  コマンド生成タイミング
 */
typedef enum {
  #if 0
  BTL_RECTIMING_None = 0,
  BTL_RECTIMING_StartTurn,
  BTL_RECTIMING_PokeInCover,
  BTL_RECTIMING_PokeInChange,
  BTL_RECTIMING_BtlIn,
  #else
  RECTIMING_None = 0,
  RECTIMING_StartTurn,
  RECTIMING_PokeInCover,
  RECTIMING_PokeInChange,
  RECTIMING_BtlIn,
  #endif
}Timing;




/*===========================================================================================*/
/*                                                                                           */
/* クライアント用                                                                            */
/*                                                                                           */
/*===========================================================================================*/

//--------------------------------------------------------------------------
/**
 *  サーバーから送られてきたデータを格納
 */
//--------------------------------------------------------------------------
//_ BTL_REC
class Data {
  GFL_FORBID_COPY_AND_ASSIGN( Data );

public:
  Data( void );
  ~Data();

  void        Write( const void* data, u32 size );
  Timing      GetTimingCode( const void* data );
  bool        IsCorrect( void ) const;
  const void* GetDataPtr( u32* size ) const;

private:
  u16          m_writePtr;
  u8           m_fSizeOver;
  u8           dmy;


#if 1
  u8           m_buf[ BTLREC_OPERATION_BUFFER_SIZE ];
#else
  // ２ターンでバッファを一杯にするテスト措置用
  u8           m_buf[ 24 ];
#endif


};  // class Data:

//--------------------------------------------------------------------------
/**
 *  格納したデータを読み取り
 */
//--------------------------------------------------------------------------
class Reader {
  GFL_FORBID_COPY_AND_ASSIGN( Reader );

public:
  Reader( void );
  ~Reader();

  void                     Init(  const void* recordData, u32 dataSize );
  void                     Reset( void );
  bool                     CheckBtlInCmd( u8 clientID );
  const BTL_ACTION_PARAM*  ReadAction( u8 clientID, u8 *numAction, u8* fChapter );
  u32                      GetTurnCount( void ) const;
  bool                     IsReadComplete( u8 clientID ) const;

#if PM_DEBUG
  void                     ResetError( void ){ m_fReadOver = false; }
  void                     SetDataSize( u32 size ){ m_dataSize = size; }
#endif

private:
  const u8*   m_recordData;
  u32         m_dataSize  : 31;
  u32         m_fReadOver :  1;
  u32         m_readPtr[ BTL_CLIENT_MAX ];
  u8          m_readBuf[ BTL_CLIENT_MAX ][64];


}; // class Reader



/*===========================================================================================*/
/*                                                                                           */
/* サーバー用                                                                                */
/*                                                                                           */
/* クライアントに送信するデータを生成するためのクラス                                        */
/*                                                                                           */
/*===========================================================================================*/

class SendTool {
  GFL_FORBID_COPY_AND_ASSIGN( SendTool );

public:
  SendTool(void) {} ;
  ~SendTool() {} ;

  void    Init( bool fChapter );
  void    PutSelActionData( u8 clientID, const BTL_ACTION_PARAM* action, u8 numAction );
  void*   PutBtlInTiming( u32* dataSize );
  void*   PutBtlInChapter( u32* dataSize, bool fChapter );
  void*   FixSelActionData( Timing timingCode, u32* dataSize );
  void*   PutTimeOverData( u32* dataSize );
  void    RestoreStart( const void* data, u32 dataSize );
  bool    RestoreFwd( u32* rp, u8* clientID, u8* posIdx, BTL_ACTION_PARAM* action );


private:

  enum {
    BUFSIZE = 60,
  };

  u8   m_writePtr;      ///< 書き込みポインタ
  u8   m_clientBit;     ///< クライアント毎の書き込み完了ビット
  u8   m_numClients;    ///< 書き込んだクライアント数
  u8   m_type;          ///< 書き込み中のデータタイプ
  bool m_fChapter;      ///< チャプターフラグ
  bool m_fError;        ///< エラー発生フラグ

  u8   m_buffer[ BUFSIZE ];   ///< 書き込みバッファ

};  // class SendTool




}  // end of namespace 'rec'
GFL_NAMESPACE_END(btl)

#endif // __BTL_REC_H__
