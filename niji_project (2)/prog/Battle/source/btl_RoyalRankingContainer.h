//=============================================================================
/**
 * @file   btl_RoyalRankingContainer.h
 * @date   2015/09/11 17:38:56
 * @author obata_toshihiro
 * @brief  バトルロイヤルの現在順位を保持するコンテナ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_ROYALRANKINGCONTAINER_H_INCLUDED
#define BTL_ROYALRANKINGCONTAINER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"


GFL_NAMESPACE_BEGIN( btl )


  /**
   * @class RoyalRankingContainer
   * @brief バトルロイヤルの現在順位を保持するコンテナ
   *
   * @note 戦闘中、サーバー用、クライアント用の２つが存在し、
   *       サーバーは、現在順位を更新したら、クライアントにも更新するようサーバーコマンドを発行する。
   *       クライアントは、サーバーコマンドを受信したら、現在順位を更新する。
   */
  class RoyalRankingContainer
  {
    GFL_FORBID_COPY_AND_ASSIGN( RoyalRankingContainer );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    RoyalRankingContainer( void );

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    virtual ~RoyalRankingContainer();

    //-------------------------------------------------------------------------
    /**
     * @brief 初期化する
     */
    //-------------------------------------------------------------------------
    void Clear( void );

    //-------------------------------------------------------------------------
    /**
     * @brief 指定クライアントの現在順位を取得する
     * @para clientId  取得対象クライアントのID
     * @return 順位( 0 origin )
     */
    //-------------------------------------------------------------------------
    u8 GetClientRanking( u8 clientId ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 指定クライアントの現在順位を設定する
     * @param clientId  設定対象クライアントのID
     * @param ranking   設定する順位( 0 origin )
     */
    //-------------------------------------------------------------------------
    void SetClientRanking( u8 clientId, u8 ranking );



  private:

    u8 m_clientRanking[ BTL_CLIENT_NUM ];

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_ROYALRANKINGCONTAINER_H_INCLUDED