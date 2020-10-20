#ifndef NIJI_PROJECT_APP_POKELIST_WAZASODATEYA_FRAME_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_WAZASODATEYA_FRAME_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListWazaSodateyaFrame.h
 * @author  fukushima_yuya
 * @date    2015.04.08
 * @brief   ポケモンリスト画面　技教え・育て屋
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListBaseFrame.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class PokeListWazaSodateyaDraw;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト画面　技教え・育て屋フレーム
//------------------------------------------------------------------------------
class PokeListWazaSodateyaFrame
  : public PokeListBaseFrame
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListWazaSodateyaFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListWazaSodateyaFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListWazaSodateyaFrame( void );

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

public: // PokeListDrawListener
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
  virtual void SetupOutData( EndMode endMode );

private:
  PokeListWazaSodateyaDraw*   m_pPokeListDraw;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_WAZASODATEYA_FRAME_H_INCLUDED
