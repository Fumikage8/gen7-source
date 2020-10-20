//=============================================================================
/**
 * @file   btl_Kawaigari.h
 * @date   2015/08/25 10:35:01
 * @author obata_toshihiro
 * @brief  可愛がり戦闘効果
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_KAWAIGARI_H_INCLUDED
#define BTL_KAWAIGARI_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"

GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class BTL_POKEPARAM;


  /**
   * @class Kawaigari
   * @brief 可愛がり戦闘効果
   */
  class Kawaigari
  {
    GFL_FORBID_COPY_AND_ASSIGN( Kawaigari );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief 戦闘開始後、最初の「○○は　どうする？」メッセージの代わりに、
     *        かわいがり特殊待機メッセージを出力するか判定
     *
     * @param mainModule  参照するメインモジュール
     * @param poke        表示対象のポケモン
     * @param rand        使用する乱数
     *
     * @return 特殊メッセージのメッセージID
     * @retval -1  特殊メッセージを表示しない場合
     */
    //-------------------------------------------------------------------------
    static int Check1stReadyMessage( const MainModule* mainModule, const BTL_POKEPARAM* poke, RandSys* rand );

    //-------------------------------------------------------------------------
    /**
     * @brief 「○○は　どうする？」メッセージの代わりに、かわいがり特殊待機メッセージを出力するか判定
     *
     * @param mainModule  参照するメインモジュール
     * @param poke        表示対象のポケモン
     * @param rand        使用する乱数
     *
     * @return 特殊メッセージのメッセージID
     * @retval -1  特殊メッセージを表示しない場合
     */
    //-------------------------------------------------------------------------
    static int CheckCommonReadyMessage( const MainModule* mainModule, const BTL_POKEPARAM* poke, RandSys* rand );



  private:

    // 可愛がり待機メッセージの最大候補数
    static const u32 MAX_KAWAIGARI_READY_MESSAGE_NUM = 5;

    static bool CheckSpecial1stReadyMessage( const MainModule* mainModule );
    static void GetReadyMessage_Friendship150( u32* destMessageNum, u16* destMessageArray, u32 destMessageArraySize, const BTL_POKEPARAM* poke );
    static void GetReadyMessage_Friendship100( u32* destMessageNum, u16* destMessageArray, u32 destMessageArraySize, const BTL_POKEPARAM* poke );
    static void GetReadyMessage_Friendship50( u32* destMessageNum, u16* destMessageArray, u32 destMessageArraySize, const BTL_POKEPARAM* poke );
    static void GetReadyMessage_Common( u32* destMessageNum, u16* destMessageArray, u32 destMessageArraySize, const BTL_POKEPARAM* poke, const u16* defaultMessageArray, u32 defaultMessageNum );

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_KAWAIGARI_H_INCLUDED