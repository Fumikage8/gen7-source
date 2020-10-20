﻿#ifndef __GFL2_DEBUGTICK_H__
#define __GFL2_DEBUGTICK_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl_DebugTick.h
 *  @brief  チックダンプ表示システム
 *  @author tomoya takahashi
 *  @date   2010.11.10
 *  @note   gflib2への移殖、Win32対応 2015.02.18 saita_kazuki
 *
 *
 *  使用方法
 *  　１．バッファの確保
 *  　　オリジナルのネームスペース内に、グローバル変数を定義します。
 *      GFL_DEBUG_TICK_SETUP(VALNAME, 10 ); // このマクロ内で、グローバル変数が宣言されます。
 *
 *    ２．チックダンプ機能を有効にする。 
 *      // ダンプ機能のフラグは初期値falseです。　
 *      //チェックするときは、必ず機能をONにしてください。
 *    　GFL_DEBUG_TICK_SET_FLAG(VALNAME,true);
 *
 *    ３．計測する
 *    　{
 *        GFL_DEBUG_TICK_START(VALNAME, 0, "描画処理" );    // 計測開始
 *
 *        Draw( MAIN );
 *
 *        GFL_DEBUG_TICK_END(VALNAME,0);  // 計測終了
 *      }
 *
 *    ４．計測情報を表示
 *    　GFL_DEBUG_TICK_DUMP_ALL(VALNAME); //全情報の表示
 *
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef GF_PLATFORM_CTR
#include <nn.h>
#endif

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <types/include/gfl2_Typedef.h>

namespace gfl2
{
namespace debug
{

#if GFL_DEBUG
  
//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------
// ダンプ表示
#define GFL_DEBUG_TICK_DUMP_PRINT_FLG (gfl2::debug::PRINT_FLAG_LEVEL2 | gfl2::debug::PRINT_FLAG_CATEGORY_DEBUG)



//-----------------------------------------------------------------------------
/**
 *        マクロ定義
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *  @brief  必要なバッファの定義
 *
 *  @param  valName 変数名
 *  @param  num     バッファ要素最大数
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_SETUP(valName, num)   static gfl2::debug::DebugTickDump valName(num)

//----------------------------------------------------------------------------
/**
 *  @brief  ダンプ機能のON・OFF
 *
 *  @param  valName 変数名
 *  @param  flag    ダンプ機能フラグ　true:ON   false:OFF
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_SET_FLAG(valName, flag) valName.SetFlag( (flag) )

//----------------------------------------------------------------------------
/**
 *  @brief  ダンプ情報のリセット
 *
 *  @param  valName 変数名
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_RESET(valName)      valName.ResetTickDump()

//----------------------------------------------------------------------------
/**
 *  @brief  チック計測開始
 *
 *  @param  valName 変数名
 *  @param  index   格納先インデックス
 *  @param  name    情報名（ログ出力に使用）
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_START(valName, index, name) valName.StartTickDump( (index), (name) )

//----------------------------------------------------------------------------
/**
 *  @brief  チック計測終了
 *
 *  @param  valName 変数名
 *  @param  index   格納先インデックス
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_END(valName, index) valName.EndTickDump( (index) )

//----------------------------------------------------------------------------
/**
 *  @brief  ダンプ
 *
 *  @param  valName 変数名
 *  @param  index   格納先インデックス
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_DUMP_ALL(valName) valName.PrintAll()
#define GFL_DEBUG_TICK_DUMP(valName, index) valName.Print( (index) )

//----------------------------------------------------------------------------
/**
 *  @brief  μ秒を取得
 *
 *  @param  index インデックス
 *
 *  @retval μ秒 
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_US_GET(valName, index) valName.GetTickNowUs((index))

//----------------------------------------------------------------------------
/**
 *  @brief  μ秒(平均)を取得
 *
 *  @param  index インデックス
 *
 *  @retval μ秒 
 *
 *  @note   平均値取得カウントが0のときは0を返します
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_US_AVE_GET(valName, index) valName.GetTickAveUs((index))

//----------------------------------------------------------------------------
/**
 *  @brief  μ秒(最大)を取得
 *
 *  @param  index インデックス
 *
 *  @retval μ秒 
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_US_MAX_GET(valName, index) valName.GetTickMaxUs((index))


//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------
typedef struct
{
  const char * printname;
#if defined(GF_PLATFORM_CTR)
  nn::os::Tick start; 
  nn::os::Tick end;
  nn::os::Tick ave;
  nn::os::Tick max;
#elif defined(GF_PLATFORM_WIN32)
  u64 start;
  u64 end;
  u64 ave;
  u64 max;
#endif
  int          count;
} DebugTick;


//-----------------------------------------------------------------------------
/**
 *          関数定義
*/
//-----------------------------------------------------------------------------
extern void ResetTickBuffer( DebugTick* buff, int num );
extern void StartTickBuffer( DebugTick* buff, int index, const char* name );
extern void EndTickBuffer( DebugTick* buff, int index );
extern void DumpTickBuffer( const DebugTick* buff, int num );

//-----------------------------------------------------------------------------
/**
 *      クラス宣言
 *
 *      ＊製品版にデータを含めない為に、
 *      　このクラスを直接使用せず、下に定義されているマクロを使用してください。
 *      
 */
//-----------------------------------------------------------------------------
class DebugTickDump
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugTickDump);

public:
  enum{
    TICK_BUFFER_MAX = 30    // tblnumに指定できる最大数
  };

public:
  // コンストラクタ
  DebugTickDump( u32 tblnum );

  // デストラクタ
  ~DebugTickDump();

  //----------------------------------------------------------------------------
  /**
   *  @brief  チックダンプ機能のON・OFF
   *
   *  @param  flag フラグ  true:ON   false:OFF
   */
  //-----------------------------------------------------------------------------
  void SetFlag( bool flag );

  //----------------------------------------------------------------------------
  /**
   *  @brief  チックダンプ機能の状態を取得
   *
   *  @retval true  ON
   *  @retval false OFF
   */
  //-----------------------------------------------------------------------------
  bool IsFlagOn( void ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  チックバッファの情報クリア
   */
  //-----------------------------------------------------------------------------
  void ResetTickDump( void );

  // チック計測開始
  void StartTickDump( u32 index, const char* cp_name );

  // チック計測終了
  void EndTickDump( u32 index );


  // チック情報の表示
  void PrintAll( void ) const;
  void Print( u32 index ) const;

  // チック平均値取得
  s32 GetTickAveUs( u32 index );

  // チック直前値を取得
  s32 GetTickNowUs( u32 index );

   // チック最大値を取得
  s32 GetTickMaxUs( u32 index );


private:
  
  bool        m_bFlag;

  DebugTick   m_TickTbl[TICK_BUFFER_MAX];
  u32         m_TickTblNum;
};


#else     // GFL_DEBUG

#define GFL_DEBUG_TICK_SETUP(valName, num)  /**/ 
#define GFL_DEBUG_TICK_SET_FLAG(valName, flag) /**/ 
#define GFL_DEBUG_TICK_RESET(valName) /**/ 
#define GFL_DEBUG_TICK_START(valName, index, name) /**/
#define GFL_DEBUG_TICK_END(valName, index) /**/
#define GFL_DEBUG_TICK_DUMP_ALL(valName) /**/
#define GFL_DEBUG_TICK_DUMP(valName, index) /**/
#define GFL_DEBUG_TICK_US_GET(valName, index) /**/
#define GFL_DEBUG_TICK_US_AVE_GET(valName, index) /**/
#define GFL_DEBUG_TICK_US_MAX_GET(valName, index) /**/

#endif    // GFL_DEBUG  


}//namespace debug
}//namespace gfl2
#endif // __GFL2_DEBUGTICK_H__
