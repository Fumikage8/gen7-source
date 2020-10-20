#if PM_DEBUG
//=============================================================================
/**
 * @file    ExpTest.cpp
 * @brief   バトルシステムテスト( 経験値計算 )
 * @author  obata_toshihiro
 * @date    2015.03.02
 */
//=============================================================================
#include <Test/ObataTest/include/ExpTest.h>
#include <Battle/source/btl_Exp.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )


//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
ExpTest::ExpTest( void )
{
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
ExpTest::~ExpTest()
{
}

//---------------------------------------------------------------------------
/**
  * @brief テストを実行する
  */
//---------------------------------------------------------------------------
void ExpTest::Exec()
{
  GFL_PRINT("\n");
  this->Exec_LevelRatioTest();

  GFL_PRINT("\n");
  this->Exec_Bonus( true );

  GFL_PRINT("\n");
  this->Exec_Bonus( false );

  GFL_PRINT("\n");
  this->Exec_Illegal();
}

/**
 * @brief レベル差による基礎経験値の倍率計算テスト
 */
void ExpTest::Exec_LevelRatioTest( void )
{
  GFL_PRINT( "レベル差による基礎経験値の倍率計算テスト\n" );

  static const u32 getPokeLevels[] = { 1, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
  static const u32 deadPokeLevels[] = { 1, 2, 3, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };

  for( u32 i=0; i<GFL_NELEMS( getPokeLevels ); ++i ) 
  {
    for( u32 j=0; j<GFL_NELEMS( deadPokeLevels ); ++j ) 
    {
      u32 exp = btl::Exp::getexp_calc_adjust_level( 100, getPokeLevels[i], deadPokeLevels[j] );
      GFL_PRINT( "[自分のレベル]%3d, [相手のレベル]%3d, [経験値]%3d\n", getPokeLevels[i], deadPokeLevels[j], exp ); 
    }
  }
}

/**
 * @brief 各種ボーナスの反映テスト
 */
void ExpTest::Exec_Bonus( bool isMatch )
{
  btl::Exp::CalcResult result;

  btl::Exp::CalcParam calcParam;
  calcParam.competitor = BTL_COMPETITOR_WILD;
  calcParam.playerLanguageId = 0;
  calcParam.deadPokeExp = 100;
  calcParam.deadPokeLevel = 50;
  calcParam.getPokeLevel = 50;
  calcParam.getPokeFriendship = 0;
  calcParam.getPokeLanguageId = 0;
  calcParam.isMatch = isMatch;
  calcParam.isGakusyusoutiOn = true;
  calcParam.haveSiawasetamago = false;
  calcParam.isEvoCanceledPoke = false;
  calcParam.isMyPoke = true;

  GFL_PRINT( "各種ボーナスによる増加テスト" );
  GFL_PRINT( ( isMatch ) ? ( "( 対面した )\n" ) : ( "( 対面していない )\n" ) );
  GFL_PRINT( "敵ポケモンの経験値:%d\n", calcParam.deadPokeExp );
  GFL_PRINT( "敵ポケモンのレベル:%d\n", calcParam.deadPokeLevel );
  GFL_PRINT( "敵ポケモンと対面したか？:%d\n", calcParam.isMatch );

  btl::Exp::CalcExp( calcParam, &result );
  GFL_PRINT( "[ボーナスなしの場合]exp=%d, isBonus=%d, byGakusyusouti=%d\n", result.exp, result.isContainBonus, result.byGakusyusouti );

  {
    btl::Exp::CalcParam temp = calcParam;
    temp.competitor = BTL_COMPETITOR_TRAINER;
    btl::Exp::CalcExp( temp, &result );
    GFL_PRINT( "[トレーナー戦の場合]exp=%d, isBonus=%d, byGakusyusouti=%d\n", result.exp, result.isContainBonus, result.byGakusyusouti );
  }

  {
    btl::Exp::CalcParam temp = calcParam;
    temp.isMyPoke = false;
    temp.getPokeLanguageId = 0;
    temp.playerLanguageId = 0;
    btl::Exp::CalcExp( temp, &result );
    GFL_PRINT( "[国内から貰ったポケモンの場合]exp=%d, isBonus=%d, byGakusyusouti=%d\n", result.exp, result.isContainBonus, result.byGakusyusouti );
  }

  {
    btl::Exp::CalcParam temp = calcParam;
    temp.isMyPoke = false;
    temp.getPokeLanguageId = 0;
    temp.playerLanguageId = 1;
    btl::Exp::CalcExp( temp, &result );
    GFL_PRINT( "[海外から貰ったポケモンの場合]exp=%d, isBonus=%d, byGakusyusouti=%d\n", result.exp, result.isContainBonus, result.byGakusyusouti );
  }

  {
    btl::Exp::CalcParam temp = calcParam;
    temp.haveSiawasetamago = true;
    btl::Exp::CalcExp( temp, &result );
    GFL_PRINT( "[しあわせたまごを持ったポケモンの場合]exp=%d, isBonus=%d, byGakusyusouti=%d\n", result.exp, result.isContainBonus, result.byGakusyusouti );
  }

  {
    btl::Exp::CalcParam temp = calcParam;
    temp.isEvoCanceledPoke = true;
    btl::Exp::CalcExp( temp, &result );
    GFL_PRINT( "[進化キャンセルされたポケモンの場合]exp=%d, isBonus=%d, byGakusyusouti=%d\n", result.exp, result.isContainBonus, result.byGakusyusouti );
  }

  {
    btl::Exp::CalcParam temp = calcParam;
    temp.getPokeFriendship = 50;
    btl::Exp::CalcExp( temp, &result );
    GFL_PRINT( "[なかよし度が50以上のポケモンの場合]exp=%d, isBonus=%d, byGakusyusouti=%d\n", result.exp, result.isContainBonus, result.byGakusyusouti );
  }

  {
    btl::Exp::CalcParam temp = calcParam;
    temp.isMyPoke = false;
    temp.getPokeLanguageId = 0;
    temp.playerLanguageId = 1;
    temp.haveSiawasetamago = true;
    temp.isEvoCanceledPoke = true;
    temp.getPokeFriendship = 50;
    temp.getPokeLevel = 1;
    temp.deadPokeLevel = 100;
    temp.deadPokeExp = 390; // ハピナスを想定
    btl::Exp::CalcExp( temp, &result );
    GFL_PRINT( "[経験値が最大になるケース]exp=%d, isBonus=%d, byGakusyusouti=%d\n", result.exp, result.isContainBonus, result.byGakusyusouti );
  }
  
}


/**
 * @brief 不正値テスト
 */
void ExpTest::Exec_Illegal( void )
{
  btl::Exp::CalcResult result;

  btl::Exp::CalcParam calcParam;
  calcParam.competitor = BTL_COMPETITOR_WILD;
  calcParam.playerLanguageId = 0;
  calcParam.deadPokeExp = 100;
  calcParam.deadPokeLevel = 50;
  calcParam.getPokeLevel = 50;
  calcParam.getPokeFriendship = 0;
  calcParam.getPokeLanguageId = 0;
  calcParam.isMatch = true;
  calcParam.isGakusyusoutiOn = true;
  calcParam.haveSiawasetamago = false;
  calcParam.isEvoCanceledPoke = false;
  calcParam.isMyPoke = true;

  {
    btl::Exp::CalcParam temp = calcParam;
    temp.deadPokeExp = 0;
    btl::Exp::CalcExp( temp, &result );
    GFL_PRINT( "[倒されたポケモンの経験値が0の場合]exp=%d, isBonus=%d, byGakusyusouti=%d\n", result.exp, result.isContainBonus, result.byGakusyusouti );
  }

  {
    btl::Exp::CalcParam temp = calcParam;
    temp.deadPokeLevel = 0;
    btl::Exp::CalcExp( temp, &result );
    GFL_PRINT( "[倒されたポケモンのレベルが0の場合]exp=%d, isBonus=%d, byGakusyusouti=%d\n", result.exp, result.isContainBonus, result.byGakusyusouti );
  }
}


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )
#endif //PM_DEBUG
