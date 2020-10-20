//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   app_util_Common.cpp
 *  @brief  アプリ関連共通定義
 *  @author Toru=Nagihashi
 *  @date   2013.01.07
 *	
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "AppLib/include/Util/app_util_Common.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

const u32 DEFAULT_FADE_SYNC = 8;    //!< 共通フェード速度

const u32 DEFAULT_GET_COUNT_60F = 20;                         //!< 項目取得カウント(60frm用)
const u32 DEFAULT_GET_COUNT_30F = DEFAULT_GET_COUNT_60F / 2;  //!< 項目取得カウント(30frm用)

// hにstatic constを書くと他のDLLのdataに載ってしまうためこちらに定義する
const gfl2::lyt::ColorU8 SEIKAKU_POWER_UP( 255,215,215,255 );   //<性格による能力補正での、「こうげき」等見出しの色付け、上昇版
const gfl2::lyt::ColorU8 SEIKAKU_POWER_DOWN( 200,230,255,255 ); //< 性格による能力補正での、「こうげき」等見出しの色付け、下降版

//  レーダーチャートの基本値の色
const gfl2::math::Vector4 g_raderChartPokeParamBaseColor( 1.0f, 0.5f, 0.1f, 1.0f );

//  性格から特徴パラメータ定義
struct APP_UTIL_POKE_SEIKAKU_DATA
{
  pml::pokepara::Seikaku  seikaku;
  pml::pokepara::PowerID  goodPowerId;
  pml::pokepara::PowerID  weakPowerId;
};

//-----------------------------------------------------------------------------------------
/**
  * @brief  性格から得意・不得意のパラメータ項目を出力
  * @param  seiakku:        ポケモンの性格
  * @param  pOutGoodPoint:  得意なパラメータを出力
  * @param  pOutWeekPoint:  不得意なパラメータを出力
  */
//-----------------------------------------------------------------------------------------
void OutputSpecialPointPokemSeikaku( const pml::pokepara::Seikaku seikaku, pml::pokepara::PowerID* pOutGoodPoint, pml::pokepara::PowerID* pOutWeakPoint )
{
  static const APP_UTIL_POKE_SEIKAKU_DATA aTable[ pml::pokepara::SEIKAKU_NUM ]  =
  {
    { pml::pokepara::SEIKAKU_GANBARIYA,   pml::pokepara::POWER_NUM,   pml::pokepara::POWER_NUM    },  // がんばりや
    { pml::pokepara::SEIKAKU_SAMISHIGARI, pml::pokepara::POWER_ATK,   pml::pokepara::POWER_DEF    },  // さみしがり
    { pml::pokepara::SEIKAKU_YUUKAN,      pml::pokepara::POWER_ATK,   pml::pokepara::POWER_AGI    },  // ゆうかん
    { pml::pokepara::SEIKAKU_IJIPPARI,    pml::pokepara::POWER_ATK,   pml::pokepara::POWER_SPATK  },  // いじっぱり
    { pml::pokepara::SEIKAKU_YANTYA,      pml::pokepara::POWER_ATK,   pml::pokepara::POWER_SPDEF  },  // やんちゃ
    { pml::pokepara::SEIKAKU_ZUBUTOI,     pml::pokepara::POWER_DEF,   pml::pokepara::POWER_ATK    },  // ずぶとい
    { pml::pokepara::SEIKAKU_SUNAO,       pml::pokepara::POWER_NUM,   pml::pokepara::POWER_NUM    },  // すなお
    { pml::pokepara::SEIKAKU_NONKI,       pml::pokepara::POWER_DEF,   pml::pokepara::POWER_AGI    },  // のんき
    { pml::pokepara::SEIKAKU_WANPAKU,     pml::pokepara::POWER_DEF,   pml::pokepara::POWER_SPATK  },  // わんぱく
    { pml::pokepara::SEIKAKU_NOUTENKI,    pml::pokepara::POWER_DEF,   pml::pokepara::POWER_SPDEF  },  // のうてんき
    { pml::pokepara::SEIKAKU_OKUBYOU,     pml::pokepara::POWER_AGI,   pml::pokepara::POWER_ATK    },  // おくびょう
    { pml::pokepara::SEIKAKU_SEKKATI,     pml::pokepara::POWER_AGI,   pml::pokepara::POWER_DEF    },  // せっかち
    { pml::pokepara::SEIKAKU_MAJIME,      pml::pokepara::POWER_NUM,   pml::pokepara::POWER_NUM    },  // まじめ
    { pml::pokepara::SEIKAKU_YOUKI,       pml::pokepara::POWER_AGI,   pml::pokepara::POWER_SPATK  },  // ようき
    { pml::pokepara::SEIKAKU_MUJYAKI,     pml::pokepara::POWER_AGI,   pml::pokepara::POWER_SPDEF  },  // むじゃき
    { pml::pokepara::SEIKAKU_HIKAEME,     pml::pokepara::POWER_SPATK, pml::pokepara::POWER_ATK    },  // ひかえめ
    { pml::pokepara::SEIKAKU_OTTORI,      pml::pokepara::POWER_SPATK, pml::pokepara::POWER_DEF    },  // おっとり
    { pml::pokepara::SEIKAKU_REISEI,      pml::pokepara::POWER_SPATK, pml::pokepara::POWER_AGI    },  // れいせい
    { pml::pokepara::SEIKAKU_TEREYA,      pml::pokepara::POWER_NUM,   pml::pokepara::POWER_NUM    },  // てれや
    { pml::pokepara::SEIKAKU_UKKARIYA,    pml::pokepara::POWER_SPATK, pml::pokepara::POWER_SPDEF  },  // うっかりや
    { pml::pokepara::SEIKAKU_ODAYAKA,     pml::pokepara::POWER_SPDEF, pml::pokepara::POWER_ATK    },  // おだやか
    { pml::pokepara::SEIKAKU_OTONASHII,   pml::pokepara::POWER_SPDEF, pml::pokepara::POWER_DEF    },  // おとなしい
    { pml::pokepara::SEIKAKU_NAMAIKI,     pml::pokepara::POWER_SPDEF, pml::pokepara::POWER_AGI    },  // なまいき
    { pml::pokepara::SEIKAKU_SINTYOU,     pml::pokepara::POWER_SPDEF, pml::pokepara::POWER_SPATK  },  // しんちょう
    { pml::pokepara::SEIKAKU_KIMAGURE,    pml::pokepara::POWER_NUM,   pml::pokepara::POWER_NUM    }  // きまぐれ
  };

  if( pOutGoodPoint != NULL )
  {
    *pOutGoodPoint  = pml::pokepara::POWER_NUM;
  }

  if( pOutWeakPoint != NULL )
  {
    *pOutWeakPoint  = pml::pokepara::POWER_NUM;
  }

  u32 loopNum = GFL_NELEMS( aTable );
  for( u32 i = 0; i < loopNum; ++i )
  {
    if( aTable[ i ].seikaku == seikaku )
    {
      if( pOutGoodPoint != NULL )
      {
        *pOutGoodPoint  = aTable[ i ].goodPowerId;
      }

      if( pOutWeakPoint != NULL )
      {
        *pOutWeakPoint  = aTable[ i ].weakPowerId;
      }
    }
  }
}

// 引数指定したポケモンパラメータから現在のEXP値を取得
u32 GetPokeNowLvExpNum( const pml::pokepara::PokemonParam* pPokeParam )
{
  GFL_ASSERT( pPokeParam );

  u32 currentExp = pPokeParam->GetExp();
  // 現在のレベルになるのに必要な経験値
  return  ( currentExp - pPokeParam->GetExpForCurrentLevel() );
}

// 引数指定したポケモンパラメータからレベルアップに必要なExp値を取得
u32 GetPokeNextLvExpNum( const pml::pokepara::PokemonParam* pPokeParam )
{
  GFL_ASSERT( pPokeParam );
  return ( pPokeParam->GetExpForNextLevel() - pPokeParam->GetExpForCurrentLevel() );
}

//  わざのPP値と最大PP値からのわざ名のテキスト色
const gfl2::lyt::ColorU8 GetWazaTextColor( const u32 nowPPNum, const u32 maxPPNum, const gfl2::lyt::ColorU8 defaultColor )
{
  //  割り算使用の為、小数レベルまで見れるように固定小数化
  u32 nowValue  = nowPPNum << 8;
  u32 maxValue  = maxPPNum << 8;

  if( nowValue > ( maxValue >> 1 ) )
  {
    //  デフォルト
    //  100 - 51
    return defaultColor;
  }
  else if( nowValue >= ( maxValue / 4 ) )
  {
    //  黄
    //  50 - 25
    return gfl2::lyt::ColorU8( 255, 255, 0, 255 );
  }
  else if( nowValue > 0 )
  {
    //  橙
    //  24 - 0
    return gfl2::lyt::ColorU8( 255, 183, 76, 255 );
  }

  //  赤
  return gfl2::lyt::ColorU8( 255, 0, 0, 255 );
}

//-----------------------------------------------------------------------------------------
/**
  * @brief  ポケモンパラメータからポケモンのそれぞれのタイプを出力
  * @param[out]           : １つ目のタイプの出力
  * @param[out]           : ２つ目のタイプの出力
  * @param[in]            : ポケモンパラメータ
  */
//-----------------------------------------------------------------------------------------
void OutputPokemonType( pml::PokeType* pOutputType1, pml::PokeType* pOutputType2, const pml::pokepara::PokemonParam* pPokeParam )
{
  if( pPokeParam == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( pOutputType1 == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( pOutputType2 == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  //  タイプ１
  pml::PokeType type1 = pPokeParam->GetType1();
  {
    *pOutputType1 = type1;
  }

  //  タイプ２
  {
    pml::PokeType type2 = pPokeParam->GetType2();
    //  タイプ１で得たのと同じのであればタイプ２はないとしている(sango参照)
    if( ( type2 == POKETYPE_NULL ) || ( type1 == type2 ) )
    {
      *pOutputType2 = POKETYPE_NULL;
    }
    else
    {
      *pOutputType2 = type2;
    }
  }
}

//-----------------------------------------------------------------------------------------
/**
  * @brief  各タイプ設定からそれぞれのタイプを出力
  * @param[out]           : １つ目のタイプの出力
  * @param[out]           : ２つ目のタイプの出力
  * @param[in]            : １つ目のタイプの設定
  * @param[in]            : ２つ目のタイプの設定
  */
//-----------------------------------------------------------------------------------------
extern void OutputPokemonTypeManual( pml::PokeType* pOutputType1, pml::PokeType* pOutputType2, const pml::PokeType& rInType1, const pml::PokeType& rInType2 )
{
  //  OutputPokemonType関数のほぼ同じ処理だが他の処理に影響を与えないためにコピペにしました。
  if( pOutputType1 == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( pOutputType2 == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  //  タイプ１
  pml::PokeType type1 = rInType1;
  {
    *pOutputType1 = type1;
  }

  //  タイプ２
  {
    pml::PokeType type2 = rInType2;
    //  タイプ１で得たのと同じのであればタイプ２はないとしている(sango参照)
    if( ( type2 == POKETYPE_NULL ) || ( type1 == type2 ) )
    {
      *pOutputType2 = POKETYPE_NULL;
    }
    else
    {
      *pOutputType2 = type2;
    }
  }

}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
