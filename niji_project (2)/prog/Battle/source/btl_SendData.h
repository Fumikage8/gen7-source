//=============================================================================
/**
 * @file   btl_SendData.h
 * @date   2015/08/31 15:55:16
 * @author obata_toshihiro
 * @brief  サーバー・クライアント間で送受信されるデータ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_SENDDATA_H_INCLUDED
#define BTL_SENDDATA_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_server_const.h"

GFL_NAMESPACE_BEGIN( btl )


  /**
   *  定数群
   */
  enum {
    SEND_BUFFER_SIZE = BTL_SERVERCMD_QUE_SIZE + 4,
  };

  typedef u16 SendDataSerialNumber;                                    // 送信データの通し番号を扱う型
  static const SendDataSerialNumber SEND_DATA_SERIAL_NUMBER_NULL = 0;  // 送信データの通し番号の無効値
  static const SendDataSerialNumber SEND_DATA_SERIAL_NUMBER_MIN  = 1;  // 送信データの通し番号の最小値



  //---------------------------------------------------------------------------
  /**
   * @brief データ送信用バッファ
   */
  //---------------------------------------------------------------------------
  struct SEND_DATA_BUFFER
  {

    struct {
      u16 serialNumber;
      u16 cmd;
      u16 size;
      u16 serverSeq;
    } header;

    u8 data[ SEND_BUFFER_SIZE ];

  };

  /**
   * @brief 送信データバッファをクリアする
   * @param buf  クリアするバッファ
   */
  extern void SEND_DATA_BUFFER_Clear( SEND_DATA_BUFFER* buf );

  /**
   * @brief 送信データバッファをコピーする
   * @param[out] dest  コピー先バッファ
   * @param      src   コピー元バッファ
   */
  extern void SEND_DATA_BUFFER_Copy( SEND_DATA_BUFFER* dest, const SEND_DATA_BUFFER& src );

  /**
   * @brief 送信データをバッファに格納する
   * @param[out] buf             送信データの格納先
   * @param      serialNumber    送信データの通し番号
   * @param      serverSeq       送信時のサーバーシーケンス
   * @param      adapterCommand  送信するコマンド
   * @param      data            送信データ本体
   * @param      dataSize        送信データ本体のサイズ
   */
  extern void SEND_DATA_BUFFER_Store( SEND_DATA_BUFFER* buf, SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand adapterCommand, const void* data, u32 dataSize );

  /**
   * @brief 送信データから、通し番号を取得する
   * @param buf  送信データ
   */
  extern SendDataSerialNumber SEND_DATA_BUFFER_GetSerialNumber( const SEND_DATA_BUFFER* buf );

  /**
   * @brief 送信データから、送信時のサーバーのシーケンス番号を取得する
   * @param buf  送信データ
   */
  extern ServerSequence SEND_DATA_BUFFER_GetServerSequence( const SEND_DATA_BUFFER* buf );

  /**
   * @brief 送信データから、コマンドを取得する
   * @param buf  送信データ
   */
  extern AdapterCommand SEND_DATA_BUFFER_GetAdapterCommand( const SEND_DATA_BUFFER* buf );

  /**
   * @brief 送信データから、データ本体を取得する
   */
  extern const void* SEND_DATA_BUFFER_GetData( const SEND_DATA_BUFFER* buf );

  /**
   * @brief 送信データから、データ本体のサイズを取得する
   */
  extern u32 SEND_DATA_BUFFER_GetDataSize( const SEND_DATA_BUFFER* buf );

  /**
   * @brief 送信データ全体のサイズを取得する
   * @param buf  送信データ
   */
  extern u32 SEND_DATA_BUFFER_GetTotalSize( const SEND_DATA_BUFFER* buf );



  //---------------------------------------------------------------------------
  /**
   * @class SendData
   * @brief 送信データを扱うクラス
   */
  //---------------------------------------------------------------------------
  class SendData
  {
  public:
    /**
     * @brief コンストラクタ
     */
    SendData( void );

    /**
     * @brief コンストラクタ
     * @param src コピー元のデータ
     */
    SendData( const SEND_DATA_BUFFER& src );

    /**
     * @brief コンストラクタ
     * @param src コピー元のデータ
     */
    SendData( const SendData& src );

    /**
     * @brief デストラクタ
     */
    virtual ~SendData();

    /**
     * @brief 送信データのバッファを取得する
     */
    const SEND_DATA_BUFFER& GetBuffer( void ) const;

    /**
     * @brief 送信データバッファをクリアする
     */
    void Clear( void );

    /**
     * @brief 送信データバッファをコピーする
     * @param src   コピー元のデータ
     */
    void CopyFrom( const SendData& src );

    /**
     * @brief 送信データをバッファに格納する
     * @param serialNumber    送信データの通し番号
     * @param serverSeq       送信時のサーバーシーケンス
     * @param adapterCommand  送信するコマンド
     * @param data            送信データ本体
     * @param dataSize        送信データ本体のサイズ
     */
    void Store( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand adapterCommand, const void* data, u32 dataSize );

    /**
     * @brief 送信データをバッファに格納する
     * @param buffer  格納するデータ
     */
    void Store( const SEND_DATA_BUFFER& buffer );

    /**
     * @brief 送信データから、通し番号を取得する
     */
    SendDataSerialNumber GetSerialNumber( void ) const;

    /**
     * @brief 送信データから、送信時のサーバーのシーケンス番号を取得する
     * @param buf  送信データ
     */
    ServerSequence GetServerSequence( void ) const;

    /**
     * @brief 送信データから、コマンドを取得する
     */
    AdapterCommand GetAdapterCommand( void ) const;

    /**
     * @brief 送信データから、データ本体を取得する
     */
    const void* GetData( void ) const;

    /**
     * @brief 送信データから、データ本体のサイズを取得する
     */
    u32 GetDataSize( void ) const;

    /**
     * @brief 送信データ全体のサイズを取得する
     */
    u32 GetTotalSize( void ) const;



  private:

    SEND_DATA_BUFFER m_buffer;

  };



GFL_NAMESPACE_END( btl )

#endif // BTL_SENDDATA_H_INCLUDED