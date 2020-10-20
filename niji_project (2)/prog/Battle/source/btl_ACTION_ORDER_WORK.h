//=============================================================================
/**
 * @file    btl_ACTION_ORDER_WORK.h
 * @brief   アクション優先順記録構造体
 * @author  obata_toshihiro
 * @date    2015.05.28
 */
//=============================================================================
#if !defined( BTL_ACTION_ORDER_WORK_H_INCLUDED )
#define BTL_ACTION_ORDER_WORK_H_INCLUDED

#include "./btl_action.h"
#include "./btl_server_const.h"


namespace btl {

  class BTL_POKEPARAM;


  /**
   * @brief アクションの詳細情報
   */
  struct ActionDesc
  {
    u32 isOiutiInterruptAction :  1;  // 技「おいうち」による割り込みアクション
    u32 isYokodoriRobAction    :  1;  // 技「よこどり」による乗っ取りアクション
    u32 isMagicCoatReaction    :  1;  // 技「マジックコート」、特性「マジックガード」による反射アクション
    u32 isOdorikoReaction      :  1;  // 特性「おどりこ」による追加アクション
    u32 isSaihaiReaction       :  1;  // ワザ「さいはい」による追加アクション
    u32 insertInfoID           :  5;  // 割り込み情報ID
    u32 pad                    : 22;
  };

  //---------------------------------------------------------------------------
  /**
   * @brief アクションの詳細情報をクリアする
   */
  //---------------------------------------------------------------------------
  extern void ActionDesc_Clear( ActionDesc* desc );


  /**
   * @brief アクション優先順記録構造体
   */
  typedef struct 
  {
    BTL_POKEPARAM      *bpp;
    BTL_ACTION_PARAM   action;
    ActionDesc         actionDesc;
    u32                priority;
    u8                 clientID;        ///< クライアントID
    u8                 fDone;           ///< 処理終了フラグ
    u8                 fIntrCheck;      ///< 割り込みチェック中フラグ
    u8                 fRecalcPriority; ///< 優先度再計算フラグ
  }ACTION_ORDER_WORK;


  //---------------------------------------------------------------------------
  /**
   * @brief 使用する技の技No.を取得する
   * @retval WAZANO_NULL  技を使用しない場合
   */
  //---------------------------------------------------------------------------
  extern WazaID ActOrder_GetWazaID( const ACTION_ORDER_WORK* wk );


} // namespace btl

#endif // BTL_ACTION_ORDER_WORK_H_INCLUDED