//=============================================================================
/**
 * @file   btl_Capture.h
 * @date   2016/02/02 17:03:14
 * @author obata_toshihiro
 * @brief  捕獲仕様
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_CAPTURE_H_INCLUDED
#define BTL_CAPTURE_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_fx32.h"

GFL_NAMESPACE_BEGIN( btl )

  
  /**
   * @class Capture
   * @brief 捕獲仕様を定義するクラス
   */
  class Capture
  {
    GFL_FORBID_COPY_AND_ASSIGN( Capture );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief フレンドボールで捕獲した場合の「なつき度」
     */
    //-------------------------------------------------------------------------
    static const u32 FRIENDBALL_FAMILIARITY = 200;

    //-------------------------------------------------------------------------
    /**
     * @brief ヘビーボールによる捕獲値の補正値を取得する
     * @param weight  捕獲対象ポケモンの体重[100g単位]
     * @return 捕獲値の補正値
     */
    //-------------------------------------------------------------------------
    static s16 GetHeavyBallCaptureCorrectValue( u16 weight );

    //-------------------------------------------------------------------------
    /**
     * @brief ムーンボールが有効かどうかをチェックする
     * @param monsno  捕獲対象ポケモンのモンスター番号
     * @param formno  捕獲対象ポケモンのフォルム番号
     * @retval true   ムーンボールで捕まえやすい
     * @retval false  ムーンボールで捕まえやすくない
     */
    //-------------------------------------------------------------------------
    static bool CheckMoonBallEffective( MonsNo monsno, FormNo formno );

    //-------------------------------------------------------------------------
    /**
     * @brief レベルボールによる捕獲倍率を取得する
     * @param myLevel      自分のポケモンのレベル
     * @param targetLevel  捕獲対象ポケモンのレベル
     * @return 捕獲倍率
     */
    //-------------------------------------------------------------------------
    static fx32 GetLevelBallCaptureRatio( u8 myLevel, u8 targetLevel );
  };


GFL_NAMESPACE_END( btl )

#endif // BTL_CAPTURE_H_INCLUDED
