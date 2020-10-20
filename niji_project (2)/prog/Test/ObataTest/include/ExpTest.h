#if PM_DEBUG
#if !defined( EXPTEST_H_INCLUDED )
#define EXPTEST_H_INCLUDED
#pragma once
//=============================================================================
/**
 * @file    ExpTest.h
 * @brief   バトルシステムテスト( 経験値計算 )
 * @author  obata_toshihiro
 * @date    2015.03.02
 */
//=============================================================================
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )


//-----------------------------------------------------------------------------
/**
 * @class   ExpTest
 * @brief   バトルシステムテスト( 経験値計算 )
 */
//-----------------------------------------------------------------------------
class ExpTest
{
public:
  //---------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //---------------------------------------------------------------------------
  ExpTest( void );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~ExpTest();

  //---------------------------------------------------------------------------
  /**
   * @brief テストを実行する
   */
  //---------------------------------------------------------------------------
  void Exec();


private:
  void Exec_LevelRatioTest( void );
  void Exec_Bonus( bool isMatch );
  void Exec_Illegal( void );
};


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )

#endif // EXPTEST_H_INCLUDED
#endif // PM_DEBUG
