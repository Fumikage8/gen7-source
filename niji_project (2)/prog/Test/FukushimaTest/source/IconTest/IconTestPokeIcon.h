#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_ICON_TEST_POKE_ICON_H_INCLUDED__
#define __TEST_FUKUSHIMA_ICON_TEST_POKE_ICON_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file IconTestPokeIcon.h
 * @date 2015/07/17 18:07:31
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>

// pml
#include <pml/include/pmlib.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class PokeIcon;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   ポケモンアイコン
//------------------------------------------------------------------------------
class PokeIconTool
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeIconTool );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeIconTool( app::util::Heap* pHeap, u32 max );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~PokeIconTool( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  bool DeleteFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンの読み込み
   */
  //------------------------------------------------------------------
  void LoadPokeIcon( u32 id, const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   テクスチャの置き換え
   * @return  "true" = 成功
   * @return  "false" = 失敗
   */
  //------------------------------------------------------------------
  bool ReplaceTexture( u32 id, gfl2::lyt::LytPicture* pane );

private:
  app::util::Heap*          m_pHeap;

  app::tool::PokeIcon*      m_pPokeIcon;

  u32   m_UpdateSeq;

  u32   m_DeleteSeq;

  u32   m_IconMax;

  bool* m_RequestTable;
};

GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // __TEST_FUKUSHIMA_ICON_TEST_POKE_ICON_H_INCLUDED__

#endif // PM_DEBUG