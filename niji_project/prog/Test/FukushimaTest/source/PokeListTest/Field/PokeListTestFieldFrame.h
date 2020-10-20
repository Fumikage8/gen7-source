#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_TEST_POKELIST_FIELD_FRAME_H__
#define __TEST_FUKUSHIMA_TEST_POKELIST_FIELD_FRAME_H__
#pragma once
//==============================================================================
/**
 * @file    PokeListTestFieldFrame.h
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   フィールド用ポケモンリストテストフレーム
 */
//==============================================================================

//niji
#include "Test/FukushimaTest/source/PokeListTest/PokeListTestBaseFrame.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )
class TestPokeListFieldDraw;
GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   フィールド用ポケモンリストテストフレーム
//------------------------------------------------------------------------------
class TestPokeListFieldFrame
  : public TestPokeListBaseFrame
{
  GFL_FORBID_COPY_AND_ASSIGN( TestPokeListFieldFrame );
  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  TestPokeListFieldFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TestPokeListFieldFrame( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   PokeListのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupPokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの生成
   */
  //------------------------------------------------------------------
  virtual void CreatePokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの破棄
   */
  //------------------------------------------------------------------
  virtual bool EndPokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの更新
   */
  //------------------------------------------------------------------
  virtual void UpdatePokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの描画
   */
  //------------------------------------------------------------------
  virtual void DrawPokeList( gfl2::gfx::CtrDisplayNo no );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの終了
   */
  //------------------------------------------------------------------
  virtual void PokeListEnd( void );

  //------------------------------------------------------------------
  /**
   * @brief   外部設定パラメータの設定
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void SetupOutData( App::PokeList::EndMode endMode );

private:
  TestPokeListFieldDraw*      m_pPokeListDraw;
};


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif  // __TEST_FUKUSHIMA_TEST_POKELIST_FIELD_FRAME_H__

#endif // PM_DEBUG