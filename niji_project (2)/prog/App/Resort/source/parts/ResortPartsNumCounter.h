#if !defined( __RESORT_PARTS_NUM_COUNTER_H__ )
#define __RESORT_PARTS_NUM_COUNTER_H__
#pragma once
//======================================================================
/**
 * @file    ResortPartsNumCounter.h
 * @date    2015/10/02 15:06:21
 * @author  fukushima_yuya
 * @brief   数字カウンター
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )
class DrawerBase;
GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

class ResortCounter
{
  GFL_FORBID_COPY_AND_ASSIGN( ResortCounter );

public:
  enum Operation {
    OPE_COUNT_UP_10,
    OPE_COUNT_DW_10,
    OPE_COUNT_UP_1,
    OPE_COUNT_DW_1,
    OPE_NUM,
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  ResortCounter( DrawerBase* base, u32 counterPartsIndex, u8 max = 99 );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~ResortCounter( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   カウンターの操作
   * @param   operation   カウンターの操作
   */
  //------------------------------------------------------------------
  void CounterOperation( Operation operation );

private:
  //------------------------------------------------------------------
  /**
   * @brief   カウンターの更新
   */
  //------------------------------------------------------------------
  void UpdateCounter( void );

  //------------------------------------------------------------------
  /**
   * @brief   数値の設定
   */
  //------------------------------------------------------------------
  void SetTotalNumber( u8 num );

public:
  //------------------------------------------------------------------
  /**
   * @brief   数値の取得
   */
  //------------------------------------------------------------------
  u8 GetTotalNumber( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   数値のリセット
   */
  //------------------------------------------------------------------
  void ResetTotalNumber( void )
  {
    SetTotalNumber( 0 );
    UpdateCounter();
  }

private:
  static const u32 NUM_KETA_MAX = 2;

private:
  DrawerBase*   m_pBase;

  u8    m_Max;

  u8    m_Number[NUM_KETA_MAX];

  gfl2::lyt::LytTextBox*    m_NumText[NUM_KETA_MAX];
};

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif // __RESORT_PARTS_NUM_COUNTER_H__