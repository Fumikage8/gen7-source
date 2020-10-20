//=============================================================================
/**
 * @file   btl_SendDataContainer.h
 * @date   2015/08/31 15:55:16
 * @author obata_toshihiro
 * @brief  サーバー・クライアント間で送受信されたデータを保持するコンテナ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_SENDDATACONTAINER_H_INCLUDED
#define BTL_SENDDATACONTAINER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"
#include "./btl_SendData.h"

GFL_NAMESPACE_BEGIN( btl )


  /**
   * @class SendDataContainer
   * @brief サーバー・クライアント間で送受信されたデータを保持するコンテナ
   */
  class SendDataContainer
  {
    GFL_FORBID_COPY_AND_ASSIGN( SendDataContainer );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heapForBuffer  バッファの確保に使用するヒープ
     * @param bufferNum      バッファの数
     */
    //-------------------------------------------------------------------------
    SendDataContainer( HeapHandle heapForBuffer, u32 bufferNum );

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    virtual ~SendDataContainer();

    //-------------------------------------------------------------------------
    /**
     * @brief データをコピーする
     * @param srcContainer  コピー元データのコンテナ
     */
    //-------------------------------------------------------------------------
    void CopyFrom( const SendDataContainer& srcContainer );

    //-------------------------------------------------------------------------
    /**
     * @brief データを追加する
     * @param dataBuffer  追加するデータ
     */
    //-------------------------------------------------------------------------
    void AddData( const SendData& dataBuffer );

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したデータが存在するか
     * @param dataSerialNumber  データの通し番号
     * @retval true   指定したデータが存在する
     * @retval false  指定したデータが存在しない
     */
    //-------------------------------------------------------------------------
    bool IsDataExist( u32 dataSerialNumber ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したデータを取得する
     * @param dataSerialNumber  データの通し番号
     * @retval NULL  指定したデータが存在しない場合
     */
    //-------------------------------------------------------------------------
    const SendData* GetData( u32 dataSerialNumber ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 通し番号が最も大きいデータを取得する
     * @retval NULL  データが存在しない場合
     */
    //-------------------------------------------------------------------------
    const SendData* GetLatestData( void ) const;



  private:

    void ClearBuffer( void );
    void IncAddIndex( void );

    SendData* m_buffer;
    u32       m_bufferNum;
    u32       m_addIndex;

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_SENDDATACONTAINER_H_INCLUDED