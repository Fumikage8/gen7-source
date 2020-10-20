//=============================================================================
/**
 * @file   btl_Capture.cpp
 * @date   2016/02/02 17:03:14
 * @author obata_toshihiro
 * @brief  捕獲仕様
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <pml/include/pml_Personal.h>
#include "./btl_Capture.h"

GFL_NAMESPACE_BEGIN( btl )


//-----------------------------------------------------------------------------
/**
 * @brief ヘビーボールによる捕獲値の補正値を取得する
 * @param weight  捕獲対象ポケモンの体重[100g単位]
 * @return 捕獲値の補正値
 */
//-----------------------------------------------------------------------------
s16 Capture::GetHeavyBallCaptureCorrectValue( u16 weight )
{
  if( weight < 1000 )
  {
    return -20;
  }
  else if( weight < 2000 )
  {
    return 0;
  }
  else if( weight < 3000 )
  {
    return 20;
  }
  else
  {
    return 30;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief ムーンボールが有効かどうかをチェックする
 * @param monsno  捕獲対象ポケモンのモンスター番号
 * @param formno  捕獲対象ポケモンのフォルム番号
 * @retval true   ムーンボールで捕まえやすい
 * @retval false  ムーンボールで捕まえやすくない
 */
//-----------------------------------------------------------------------------
bool Capture::CheckMoonBallEffective( MonsNo monsno, FormNo formno )
{
  pml::personal::LoadEvolutionTable( monsno, formno );
  const u8 routeNum = pml::personal::GetEvolutionRouteNum();

  for( u8 i=0; i<routeNum; ++i )
  {
    // 進化条件が「つきのいし使用」なら、ムーンボールが有効
    pml::personal::EvolveCond evolveCond = pml::personal::GetEvolutionCondition( i );
    if( evolveCond == pml::personal::EVOLVE_COND_ITEM )
    {
      u16 itemno = pml::personal::GetEvolutionParam( i );
      if( itemno == ITEM_TUKINOISI )
      {
        return true;
      }
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief レベルボールによる捕獲倍率を取得する
 * @param myLevel      自分のポケモンのレベル
 * @param targetLevel  捕獲対象ポケモンのレベル
 * @return 捕獲倍率
 */
//-----------------------------------------------------------------------------
fx32 Capture::GetLevelBallCaptureRatio( u8 myLevel, u8 targetLevel )
{
  if( targetLevel <= ( myLevel / 4 ) )
  {
    return FX32_CONST(8);
  }

  if( targetLevel <= ( myLevel / 2 ) )
  {
    return FX32_CONST(4);
  }

  if( targetLevel < myLevel )
  {
    return FX32_CONST(2);
  }

  return FX32_CONST(1);
}


GFL_NAMESPACE_END( btl )