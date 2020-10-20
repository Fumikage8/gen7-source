//=============================================================================
/**
 * @file   btl_IntrudeSystem.h
 * @date   2015/10/09 12:18:43
 * @author obata_toshihiro
 * @brief  乱入
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_INTRUDESYSTEM_H_INCLUDED
#define BTL_INTRUDESYSTEM_H_INCLUDED
#pragma once

#include <pml/include/pml_PokePara.h>
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Field/FieldRo/include/Encount/FieldEncountData.h>
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
   * @class IntrudeSystem
   * @brief 乱入に関する処理を定義するクラス
   */
  class IntrudeSystem
  {
    GFL_FORBID_COPY_AND_ASSIGN( IntrudeSystem );

  public:

    /**
     * @brief コンストラクタ
     * @param mainModule                関連づけるMainModule
     * @param pokecon                   関連付けるPOKECON
     * @param appearPokeParamGenerator  乱入ポケモンのパラメータ抽選を行うオブジェクト
     */
    IntrudeSystem( const MainModule* mainModule, const POKECON* pokecon, Field::Encount::PokeSet* appearPokeParamGenerator );

    /**
     * @brief デストラクタ
     */
    virtual ~IntrudeSystem();


    //-------------------------------------------------------------------------
    /**
     * @brief 道具「びびりだま」を使用済みか？
     * @retval true   使用済み
     * @retval false  未使用
     */
    //-------------------------------------------------------------------------
    bool IsBibiridamaUsed( void ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 道具「びびりだま」を使用したことを記録する
     */
    //-------------------------------------------------------------------------
    void SetBibiridamaUsed( void );

    //-------------------------------------------------------------------------
    /**
     * @brief 乱入回数をインクリメントする
     */
    //-------------------------------------------------------------------------
    void IncIntruceCount( void );

    //-------------------------------------------------------------------------
    /**
     * @brief 前ターンにおける、助けを呼んだ結果をセットする
     *
     * @param callPokeId  前ターンにおいて、助けを呼んだポケモンのID( 助けを呼んでいないなら BTL_POKEID_NULL )
     * @param isAppeared  前ターンにおいて、助けが来たか？
     */
    //-------------------------------------------------------------------------
    void SetPrevCallResult( u8 callPokeId, bool isAppeared );

    //-------------------------------------------------------------------------
    /**
     * @brief 効果抜群のダメージを受けたことを記録する
     *
     * @param pokeId  ダメージを受けたポケモンのID
     */
    //-------------------------------------------------------------------------
    void RecordBatugunDamage( u8 pokeId );

    //-------------------------------------------------------------------------
    /**
     * @brief 効果抜群のダメージ履歴をクリアする
     */
    //-------------------------------------------------------------------------
    void ClearBatugunDamageRecord( void );

    //-------------------------------------------------------------------------
    /**
     * @brief 助けを呼ぶ
     *
     * @param      weather              天候
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
      BtlWeather                   weather,
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
    bool CheckCall( const BTL_POKEPARAM& callPoke );
    u32 CalcCallPercentage( const BTL_POKEPARAM& callPoke, bool isBibiridamaUsed ) const;
    fx32 CalcCallPercentageRate( const BTL_POKEPARAM& callPoke, bool isBibiridamaUsed ) const;
    bool CheckAppear( const BTL_POKEPARAM& callPoke, BtlPokePos callPokePos );
    BtlPokePos DecideAppearPokePos( BTL_CLIENT_ID callClientId ) const;
    u32 CalcAppearPercentage( const BTL_POKEPARAM& callPoke, BtlPokePos callPokePos, bool isBatugunDamage, u8 prevCallPokeId, bool prevNotAppeared ) const;
    fx32 CalcAppearPercentageRate( const BTL_POKEPARAM& callPoke, BtlPokePos callPokePos, bool isBatugunDamage, u8 prevCallPokeId, bool prevNotAppeared ) const;
    bool IsPressureEnemyExist( BtlPokePos callPokePos ) const;
    bool IsPressurePoke( const BTL_POKEPARAM& poke ) const;
    bool SetupAppearPokeParam( pml::pokepara::PokemonParam* pokeParam, BtlPokePos appearPos, BtlWeather weather );
    bool SetupAppearPokeParam_ByEncountData( pml::pokepara::PokemonParam* pokeParam, BtlPokePos appearPos, BtlWeather weather );
    Field::Encount::IntruderType GetIntruderType( BtlWeather weather ) const;
    u8 GetRareDrawCount( u32 intrudeCount ) const;
    void ApplyIntrudeCountBonus_TalentPower( pml::pokepara::PokemonParam* pokeParam );
    void ApplyIntrudeCountBonus_Tokusei( pml::pokepara::PokemonParam* pokeParam );
    u32 GetBonusVNum( u32 intrudeCount ) const;
    u32 Get3rdTokuseiPercentage( u32 intrudeCount ) const;


  private:

    const MainModule*        m_mainModule;
    const POKECON*           m_pokecon;
    Field::Encount::PokeSet* m_appearPokeParamGenerator;

    RandSys m_randSys;
    bool    m_isBibiridamaUsed;                    // 道具「びびりだま」を使用したか？
    u8      m_intrudeCount;                        // 乱入回数
    u8      m_prevCallPokeId;                      // 前ターンにおいて、助けを呼んだポケモンのID( 助けを呼んでいないなら BTL_POKEID_NULL )
    bool    m_prevAppeared;                        // 前ターンにおいて、助けが来たか？
    bool    m_isBatugunDamaged[ BTL_POKEID_MAX ];  // 各ポケモンがについて、効果抜群のダメージを受けたか？

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_INTRUDESYSTEM_H_INCLUDED