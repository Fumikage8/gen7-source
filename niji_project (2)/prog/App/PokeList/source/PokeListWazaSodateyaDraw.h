#ifndef NIJI_PROJECT_APP_POKELIST_WAZASODATEYA_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_WAZASODATEYA_DRAW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListWazaSodateyaDraw.h
 * @author  fukushima_yuya
 * @date    2015.03.25
 * @brief   ポケモンリスト画面　技教え・育て屋
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListBaseDraw.h"
#include "App/PokeList/source/PokeListConst.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト描画クラス
//------------------------------------------------------------------------------
class PokeListWazaSodateyaDraw
  : public PokeListBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListWazaSodateyaDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListWazaSodateyaDraw( app::util::Heap * heap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListWazaSodateyaDraw( void );
  
public:
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

  //------------------------------------------------------------------
  /**
   * @brief   UIのセットアップ完了待ち
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  virtual bool IsSetupUIReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   * @param   pButton
   */
  //------------------------------------------------------------------
  void KeyActionNormal( gfl2::ui::Button* pButton );

public:   //!< リスナー処理
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
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_WAZASODATEYA_DRAW_H_INCLUDED
