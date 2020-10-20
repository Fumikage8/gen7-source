//=============================================================================
/**
 * @file   btl_PartyAllDeadRecorder.h
 * @date   2015/09/12 13:42:00
 * @author obata_toshihiro
 * @brief  パーティが全滅した順番の記録
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_PARTYALLDEADRECORDER_H_INCLUDED
#define BTL_PARTYALLDEADRECORDER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"

GFL_NAMESPACE_BEGIN( btl )


  /**
   * @class PartyAllDeadRecorder
   * @brief パーティが全滅した順番の記録
   */
  class PartyAllDeadRecorder
  {
    GFL_FORBID_COPY_AND_ASSIGN( PartyAllDeadRecorder );

  public:

    /**
     * @brief まだ全滅していない場合の、全滅した順番値
     */
    static const u8 DEAD_ORDER_NONE = BTL_CLIENT_NUM;

    /**
     * @brief コンストラクタ
     */
    PartyAllDeadRecorder( void );

    /**
     * @brief デストラクタ
     */
    virtual ~PartyAllDeadRecorder();

    /**
     * @brief 初期化する
     */
    void Clear( void );

    /**
     * @brief パーティが全滅したことを記録する
     * @param clientID  全滅したクライアントのID
     */
    void RecordPartyAllDead( u8 clientID );

    /**
     * @brief パーティが全滅した順番を取得する
     * @param clientID 取得対象クライアントのID
     * @retval 0origin, まだ全滅していないなら PartyAllDeadRecorder::DEAD_ORDER_NONE
     */
    u8 GetAllDeadOrder( u8 clientID ) const;



  private:

    u8 m_partyAllDeadOrder[ BTL_CLIENT_NUM ]; ///< 全滅した順番( 0 origin, DEAD_ORDER_NONE：全滅していない )

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_PARTYALLDEADRECORDER_H_INCLUDED