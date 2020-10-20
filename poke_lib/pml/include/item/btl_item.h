//=============================================================================
/**
 * @file btl_item.h
 * @brief バトルで使用するアイテム情報にアクセスする関数群
 * @author obata_toshihiro
 * @date 2011.09.26
 */
//=============================================================================
#ifndef  __BTLITEM_H__
#define  __BTLITEM_H__
#pragma once

#include "niji_conv_header/poke_lib/item/itemsym.h"


namespace item {


  /**
   * @brief ポケットの種類
   */
  enum BtlPocket {
    BTLPOCKET_ROTOPON, // ロトポン
    BTLPOCKET_HP,      // HP/PP 回復
    BTLPOCKET_STATUS,  // 状態異常 回復
    BTLPOCKET_BALL,    // ボール
    BTLPOCKET_BATTLE,  // 戦闘用
    BTLPOCKET_NUM,
  };



  /**
   * @brief アイテムのバトルポケットデータを読み込む
   * @param heap  バッファ確保に使用するヒープ
   *
   * @caution 
   * 読み込んだデータの使用が終了したらUnloadBtlPocketData()で破棄してください.
   */
  void LoadBtlPocketData( gfl2::heap::HeapBase* heap );

   /**
   * @brief アイテムのバトルポケットデータを読み込む
   * @param heap  バッファ確保に使用するヒープ
   *
   * @caution 
   * 読み込んだデータの使用が終了したらUnloadBtlPocketData()で破棄してください.
   */
  void AsyncLoadBtlPocketData( gfl2::heap::HeapBase* heap , gfl2::heap::HeapBase* workheap );
  bool IsFinishLoadBtlPocketData(void);
  void FinishLoadBtlPocketData(void); //Asyncの読み込み後に1回だけ呼んでください。

  /**
   * @brief 読み込んだアイテムのバトルポケットデータを破棄する
   */
  void UnloadBtlPocketData( void );

  /**
   * @brief 指定したバトルポケットに所属するかどうかを調べる
   * @param pocket  チェックするポケット
   * @param item    チェックする道具
   * @retval true   item は pocket に所属する
   * @retval false  item は pocket に所属しない
   *
   * @attention
   * 必ず, LoadBtlPocketData() でデータを読み込んでから使用してください.
   * 未ロードの場合は false を返します.
   */
  bool CheckBtlPocketInclude( BtlPocket pocket, u32 item );

  /**
   * @brief バトルポケットのIDを取得
   * @return バトルポケットのID
   */
  u8 GetBattlePocketID( BtlPocket pocket );




}  // namespace item


#endif  // __BTLITEM_H__
