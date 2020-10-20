//======================================================================
/**
 * @file btl_PokeIDRec.h
 * @date 2016/06/17 13:01:49
 * @author taya
 * @brief 汎用ポケモンID履歴管理クラス
 * @detail
 *  登録されたポケモンIDをクライアントごとに管理して、登録の有無や直近に登録されたIDの問い合わせに対応する。
 *  汎用と言いつつnijiの乱入ポケモン管理のために使いやすい構造になっているが汎用に使えるといいなという願いが込められている。
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __BTL_POKEID_H_INCLUDED__
#define __BTL_POKEID_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"
#include "./btl_const.h"
#include "./btl_pokeID.h"

GFL_NAMESPACE_BEGIN(btl)

  /**
   * @class PokeIDRec
   * @brief 汎用ポケモンID履歴管理クラス
   */
  class PokeIDRec
  {
      GFL_FORBID_COPY_AND_ASSIGN(PokeIDRec);

  public:
    // ctor / dtor
    PokeIDRec();
    ~PokeIDRec();

    /**
     * @brief 記録領域の初期化
     */
    void Clear( void );

    /**
     * @brief ID１件登録
     * @param[in]  pokeID   登録するポケモンID
     */
    void Register( u8 pokeID );

    /**
     * @brief 指定IDが登録されているか確認する
     * @param[in]  pokeID  確認したいポケモンID
     * @return  登録されていたら true
     */
    bool IsRegistered( u8 pokeID ) const;

    /**
     * @brief 指定IDが、直近 n 件以内に登録されているか確認する（※ n 件 = クライアントごとにカウント）
     * @param[in]  pokeID      確認したいポケモンID
     * @param[in]  checkCount  さかのぼって調べる件数（n）
     * @return  登録されていたら true
     */
    bool IsRegisteredRecent( u8 pokeID, u8 checkCount ) const;


  private:
    enum {
      CAPACITY_PER_CLIENT = BTL_PARTY_MEMBER_MAX,  // １クライアントあたりの格納可能件数
    };
    //! 記録領域
    u8  m_registeredID[ BTL_CLIENT_NUM ][ CAPACITY_PER_CLIENT ];


    void removeID( u8 clientID, u8 pokeID );
    void addID( u8 clientID, u8 pokeID );
    GFL_WARNING_PUSH  //この文で#pragma設定を退避
    GFL_WARNING_CTR_IGNORE(427)  //#427-Dを抑止
    int PokeIDRec::findIndex( u8 clientID, u8 pokeID ) const;
    GFL_WARNING_POP  //この文で#pragma設定を回復 

  };

GFL_NAMESPACE_END(btl)

#endif // __BTL_POKEID_H_INCLUDED__