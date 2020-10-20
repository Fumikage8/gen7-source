//=============================================================================
/**
 * @file   btl_ClientCommunicationObserver.h
 * @date   2015/08/19 12:07:08
 * @author obata_toshihiro
 * @brief  リモートクライアントとの通信接続を監視する
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_CLIENTCOMMUNICATIONOBSERVER_H_INCLUDED
#define BTL_CLIENTCOMMUNICATIONOBSERVER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_SetupParam.h>

GFL_NAMESPACE_BEGIN( btl )


  /**
   * @class ClientCommunicationObserver
   * @brief リモートクライアントとの通信接続を監視する
   */
  class ClientCommunicationObserver
  {
    GFL_FORBID_COPY_AND_ASSIGN( ClientCommunicationObserver );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    ClientCommunicationObserver( void );

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    virtual ~ClientCommunicationObserver();



    //-------------------------------------------------------------------------
    /**
     * @brief 全クライアントとの通信接続状況を監視する
     */
    //-------------------------------------------------------------------------
    void ObserveClinetCommunication( void );

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したクライアントとの通信が接続されているか？
     * @param clientId  チェック対象クライアントのID
     * @retval true   接続されている
     * @retval false  接続されていない
     */
    //-------------------------------------------------------------------------
    bool IsClientCommunicationExist( BTL_CLIENT_ID clientId ) const;



  private:

    bool m_isClientCommunicationExist[ BTL_CLIENT_NUM ];

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_CLIENTCOMMUNICATIONOBSERVER_H_INCLUDED