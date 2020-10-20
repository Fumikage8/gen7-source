//=============================================================================
/**
 * @file   btl_RecordSave.h
 * @date   2016/02/15 18:22:51
 * @author obata_toshihiro
 * @brief  レコードデータ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined __HEADER_H_INCLUDED__
#define __HEADER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class BTL_POKEPARAM;


  /**
   * @class RecordSave
   * @brief レコードデータ
   */
  class RecordSave
  {
    GFL_FORBID_COPY_AND_ASSIGN( RecordSave );

  public:

    /**
     * @brief 技を使用した回数をインクリメントする
     * @param attacker   技を使用したポケモン
     * @param zWazaKind  使用した技の全力技分類
     */
    static void IncWazaUseCount( const MainModule& mainModule, const BTL_POKEPARAM& attacker, ZenryokuWazaKind zWazaKind );
    
    /**
     * @brief ボールを投げた回数をインクリメントする
     */
    static void IncBallThrowCount( const MainModule& mainModule );

    /**
     * @brief 回復アイテムを使用した回数をインクリメントする
     * @param itemID  使用した道具のID
     */
     static void IncRecoverItemUseCount( const MainModule& mainModule, u16 itemID );

     /**
      * @brief 野生のポケモンから道具を盗んだ回数をインクリメントする
      */
     static void IncStealItemFromWildPoke( const MainModule& mainModule );

     /**
      * @brief 技「ねこにこばん」で取得した金額を加算する
      * @param result      対戦結果
      * @param money       加算する金額
      */
     static void AddNekonikobanTotal( const MainModule& mainModule, BtlResult result, u32 money );

  };



GFL_NAMESPACE_END( btl )

#endif // __HEADER_H_INCLUDED__