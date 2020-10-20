#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_POKE_MODEL_TEST_MODEL_H_INCLUDED__
#define __TEST_FUKUSHIMA_POKE_MODEL_TEST_MODEL_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file PokeModelTestModel.h
 * @date 2015/09/07 11:22:49
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Tool/AppToolPokeSimpleModel.h>
#include <PokeTool/include/PokeModelSystem.h>

// gflib2
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

typedef app::tool::PokeSimpleModel        PokeModelTool;
typedef app::util::Heap                   AppHeap;
typedef app::util::AppRenderingManager    AppRenderMan;

//------------------------------------------------------------------------------
// @brief   ポケモンモデルクラス
//------------------------------------------------------------------------------
class PokeModel
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeModel );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   * @param   heap          app::util::Heapクラス
   * @param   renderMan     app::util::AppRenderingManagerクラス
   */
  //------------------------------------------------------------------
  PokeModel( AppHeap* heap, AppRenderMan* renderMan );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~PokeModel( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  bool Initialize( void );

  //------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //------------------------------------------------------------------
  bool Terminate( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   モデルのセット
   * @param   monsNo
   */
  //------------------------------------------------------------------
  void SetPokeModel( MonsNo monsNo );

  //------------------------------------------------------------------
  /**
   * @brief   モデルの準備チェック
   */
  //------------------------------------------------------------------
  bool IsModelReady( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   モデルの表示
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisibleModel( bool visible );

private:
  AppHeap*            m_pHeap;

  AppRenderMan*       m_pRenderMan;

  PokeTool::PokeModelSystem*      m_pPokeModelSystem;

  PokeModelTool*      m_pSimpleModel;

  u8    m_Seq;
};

GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // __TEST_FUKUSHIMA_POKE_MODEL_TEST_MODEL_H_INCLUDED__

#endif // PM_DEBUG