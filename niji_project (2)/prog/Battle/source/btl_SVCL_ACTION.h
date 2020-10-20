//=============================================================================
/**
 * @file    btl_SVCL_ACTION.h
 * @brief   クライアントアクション構造体
 * @author  obata_toshihiro
 * @date    2015.05.28
 */
//=============================================================================
#if !defined( BTL_SVCL_ACTION_H_INCLUDED )
#define BTL_SVCL_ACTION_H_INCLUDED

#include "./btl_action.h"


namespace btl {


  /**
   * @brief 全クライアントのアクションを格納する構造体
   */
  typedef struct
  {
    BTL_ACTION_PARAM  param[ BTL_CLIENT_MAX ][ BTL_CLIENT_ACTION_PER_TURN_MAX ];
    u8                count[ BTL_CLIENT_MAX ];
  } SVCL_ACTION;




  /**
   * @brief 指定したクライアントのアクション数を取得する
   * @param clientAction   全クライアントのアクション配列
   * @param clientID       クライアントID
   */
  inline u8 SVCL_ACTION_GetNumAction( const SVCL_ACTION* clientAction, u8 clientID )
  {
    GFL_ASSERT(clientID<BTL_CLIENT_MAX);
    return clientAction->count[ clientID ];
  }

  /**
   * @brief 指定したアクションのパラメータを取得する
   * @param clientAction   全クライアントのアクション配列
   * @param clientID       クライアントID
   * @param posIdx         アクションのインデックス
   */
  inline BTL_ACTION_PARAM  SVCL_ACTION_Get( const SVCL_ACTION* clientAction, u8 clientID, u8 posIdx )
  {
    GFL_ASSERT(clientID<BTL_CLIENT_MAX);
    GFL_ASSERT(posIdx<BTL_CLIENT_ACTION_PER_TURN_MAX);
    return clientAction->param[ clientID ][ posIdx ];
  }


} // namespace btl

#endif // BTL_SVCL_ACTION_H_INCLUDED