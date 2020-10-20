//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   RandomGroup.cpp
 *  @brief  ランダムグループ
 *  @author tomoya takahashi
 *  @date   2011.12.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Savedata/include/RandomGroup.h"
#include "debug/include/gfl2_Assert.h"
#include <debug/include/gfl2_DebugPrint.h>


namespace Savedata{

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //--------------------------------------------------------------------------------------------
  RandomGroup::RandomGroup(void)
  {
    this->Initialize();
  }

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  RandomGroup::~RandomGroup(void)
  {
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  ランダムグループ初期化
   */
  //-----------------------------------------------------------------------------
  void RandomGroup::Initialize( void )
  {
    Data* p_data = &m_Data;
    gfl2::math::Random rand;

    for( u32 i=0; i<RANDOM_MAX; ++i ){
      gfl2::std::MemClear( &p_data->random_state[i], sizeof(gfl2::math::Random::State) );
      gfl2::std::MemClear( &p_data->random_seed[i], sizeof(u32) );
    }

    // 最初の乱数の種計算
    rand.Initialize();
    p_data->random_state[RANDOM_TODAY] = rand.SaveState();

  }
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  ゲーム内で使用する。今日の乱数の種を取得
   */
  //-----------------------------------------------------------------------------
  u32 RandomGroup::GetTodayRandSeed( void ) const
  {
    gfl2::math::Random rand;
    rand.Initialize( m_Data.random_state[ RANDOM_TODAY ] );
    return rand.Next();
  }


  //----------------------------------------------------------------------------
  /**
   *  @brief  ランダムの種を経過日数分まわす。
   *
   *  @param  p_data      データ
   *  @param  diff_day    経過日数 
   */
  //-----------------------------------------------------------------------------
  void RandomGroup::UpdateRandSeed( u32 diff_day )
  {
    gfl2::math::Random rand;

    rand.Initialize( m_Data.random_state[RANDOM_TODAY] );
#pragma push
#pragma diag_suppress 550
    // 乱数の種計算
    for( u32 i=0; i<diff_day; ++i ){
      u32 num = rand.Next();
      (void)num;
      TOMOYA_PRINT( "Todys Random Seed %d\n", num );
    }
#pragma pop
    m_Data.random_state[RANDOM_TODAY] = rand.SaveState();
  }


  //----------------------------------------------------------------------------
  /**
   *  @brief  ゲーム内で使用する。今日の乱数の種を取得
   */
  //-----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /**
   *  @brief  乱数の種取得
   */
  //-----------------------------------------------------------------------------
  u32 RandomGroup::GetRandSeed( RandomTypes type ) const
  {
    gfl2::math::Random rand;

    if( type < RANDOM_MAX ){
      // 乱数の種設定
      rand.Initialize(m_Data.random_state[type]);
      return rand.Next();
    }else{
      GFL_ASSERT_STOP( type < RANDOM_MAX ); //@check
    }

    // ここには、絶対にこない。
    rand.Initialize();
    return rand.Next();
  }


} // Savedata
