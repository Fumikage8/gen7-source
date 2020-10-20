//=============================================================================
/**
 * @file   btl_ReinforceSystem.h
 * @date   2015/12/07
 * @author obata_toshihiro
 * @brief  援軍
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_REINFORCESYSTEM_H_INCLUDED
#define BTL_REINFORCESYSTEM_H_INCLUDED
#pragma once

#include <pml/include/pml_PokePara.h>
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_SetupParam.h> // for BTL_CLIENT_ID
#include "./btl_common.h"


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )
class PokeSet;
GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )


GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class POKECON;
  class BTL_POKEPARAM;


  /**
   * @class ReinforceSystem
   * @brief 援軍に関する処理を定義するクラス
   *
   * @note 「援軍」とは…
   *        エンカウントしたポケモンを助けるために、ポケモンが乱入してくる仕様のこと。
   *        乱入後は 1vs2 のバトルになります。
   *        乱入ポケモンごとに設定された出現条件を満たすことで、助けを呼び、必ず助けが来ます。
   *        エンカウントしたポケモンが場にいない場合、新しいポケモンが乱入してくることはありません。
   */
  class ReinforceSystem
  {
    GFL_FORBID_COPY_AND_ASSIGN( ReinforceSystem );

  public:

    /**
     * @brief コンストラクタ
     * @param mainModule  関連づけるMainModule
     * @param pokecon     関連付けるPOKECON
     */
    ReinforceSystem( const MainModule* mainModule, const POKECON* pokecon );

    /**
     * @brief デストラクタ
     */
    virtual ~ReinforceSystem();


    //-------------------------------------------------------------------------
    /**
     * @brief 助けを呼ぶ
     *
     * @param      callClientId         助けを呼ぶクライアントのID
     * @param[out] out_isCalled         助けを呼んだか？
     * @param[out] out_isAppeared       助けが来たか？
     * @param[out] out_callPokeId       助けを呼ぶポケモンのID
     * @param[out] out_callPokePos      助けを呼ぶポケモンの位置
     * @param[out] out_appearPokePos    乱入ポケモンの立ち位置
     * @param[out] out_appearPokeParam  乱入ポケモンのパラメータ
     */
    //-------------------------------------------------------------------------
    void CallHelp(
      BTL_CLIENT_ID                callClientId,
      bool*                        out_isCalled, 
      bool*                        out_isAppeared,
      u8*                          out_callPokeId, 
      BtlPokePos*                  out_callPokePos,
      BtlPokePos*                  out_appearPokePos,
      pml::pokepara::PokemonParam* out_appearPokeParam );




  private:

    BtlPokePos DecideCallPokePos( BTL_CLIENT_ID callClientId ) const;
    const BTL_POKEPARAM* GetCallPokeParam( BtlPokePos callPokePos ) const;
    BtlPokePos DecideAppearPokePos( BTL_CLIENT_ID callClientId ) const;
    bool CheckCall( const BTL_POKEPARAM& callPoke ) const;

    bool SetupAppearPokeParam( pml::pokepara::PokemonParam* pokeParam, BtlPokePos appearPos, const BTL_POKEPARAM& callPoke );
    s32 DecideReinforcePokeIndex( const BTL_POKEPARAM& callPoke, const ReinforceBattleParam& reinforceParam ) const;
    bool IsReinforceConditionSatisfied( const BTL_POKEPARAM& callPoke, const ReinforcePokeParam& reinforcePokeParam ) const;


  private:

    const MainModule* m_mainModule;
    const POKECON*    m_pokecon;

    u32 m_intrudeCount[ MAX_REINFORCE_POKE_NUM ];

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_REINFORCESYSTEM_H_INCLUDED