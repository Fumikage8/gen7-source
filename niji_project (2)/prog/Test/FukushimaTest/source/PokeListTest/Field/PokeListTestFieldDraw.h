#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_TEST_POKELIST_FIELD_DRAW_H__
#define __TEST_FUKUSHIMA_TEST_POKELIST_FIELD_DRAW_H__
#pragma once
//==============================================================================
/**
 * @file    PokeListTestFieldDraw.h
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   フィールド用ポケモンリスト
 */
//==============================================================================

// niji
#include "Test/FukushimaTest/source/PokeListTest/PokeListTestBaseDraw.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )
class TestPokePlate;
GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   フィールド用ポケモンリストクラス
//------------------------------------------------------------------------------
class TestPokeListFieldDraw
  : public TestPokeListBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN( TestPokeListFieldDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  TestPokeListFieldDraw( app::util::Heap * heap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TestPokeListFieldDraw( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  virtual bool EndFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //------------------------------------------------------------------
  virtual void UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画処理
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   UIのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupUI( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupButton( app::tool::ButtonManager* pBtnMan );


private:  //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
   */
  //------------------------------------------------------------------
  virtual void OnPreLoadResource( void );

  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

  //------------------------------------------------------------------
  /**
   * @brief   キー入力通知用リスナ
   * @param   pKey
   * @param   pStick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );


private:
  u32     m_animIndex;
  u32     m_prevAnimIndex;

  TestPokePlate*    m_pPokePlate[App::PokeList::PLATE_NUM];
};


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif  // __TEST_FUKUSHIMA_TEST_POKELIST_FIELD_DRAW_H__

#endif // PM_DEBUG