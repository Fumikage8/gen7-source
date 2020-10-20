#if !defined( __APP_STATUS_WAZA_CURSOR_H_INCLUDED__ )
#define __APP_STATUS_WAZA_CURSOR_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusWazaCursor.h
 * @date    2015/12/09 13:01:48
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：技カーソル(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusWazaExchange.h"

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
class MenuCursor;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   技カーソル(下)
//------------------------------------------------------------------------------
class WazaCursor
  : public WazaExchangeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( WazaCursor );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  WazaCursor( DrawerBase* base, MenuCursor* cursor );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~WazaCursor( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Initialize( void );

  //------------------------------------------------------------------
  /**
   * @brief   所持技数の設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetWazaCount( const pml::pokepara::PokemonParam* pp );

public:
  //------------------------------------------------------------------
  /**
   * @brief   カーソルの位置の設定
   *
   * @param   pos   カーソル位置
   * @param   flg   表示切替を行うかどうか
   */
  //------------------------------------------------------------------
  void SetCursorPos( u8 pos, bool flg = true );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルの位置の取得
   *
   * @return  カーソル位置
   */
  //------------------------------------------------------------------
  u8 GetCursorPos( void ) const { return m_CursorPos; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   次のプレートへ
   *
   * @param   次のプレートインデックス
   */
  //------------------------------------------------------------------
  u8 NextWazaIndex( void );

  //------------------------------------------------------------------
  /**
   * @brief   前のプレートへ
   *
   * @param   前のプレートインデックス
   */
  //------------------------------------------------------------------
  u8 PrevWazaIndex( void );

public: //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   プレートデータの設定
   *
   * @param   plateDataTbl    プレートデータ配列
   */
  //------------------------------------------------------------------
  virtual void SetPlateData( PlateData* plateDataTbl );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルの表示切替
   *
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  virtual void SetVisibleCursor( bool visible );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルの表示位置の設定
   *
   * @param   pos   カーソル位置
   */
  //------------------------------------------------------------------
  virtual void SetPosCursor( u32 pos );

private:
  static const u8   WAZA_MAX = 4;

private:
  DrawerBase*       m_pBase;

  MenuCursor*       m_pCursor;

  u8                m_WazaCount;

  u8                m_CursorPos;

  u8                m_PlateIndex[WAZA_MAX];

private:
  gfl2::lyt::LytPane*     m_pNullCursor[WAZA_MAX];
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_WAZA_CURSOR_H_INCLUDED__
