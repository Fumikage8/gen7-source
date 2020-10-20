#if !defined( __RESORT_UPPER_HOME_MENU_DRAW_H__ )
#define __RESORT_UPPER_HOME_MENU_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    ResortUpperHomeMenuDraw.h
 * @date    2015/09/24 16:23:56
 * @author  fukushima_yuya
 * @brief   ポケリゾート：管理人メニュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortUpperBaseDraw.h"

#include <AppLib/include/Tool/TalkWindow.h>
#include <AppLib/include/Util/AppRenderingManager.h>

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)
class ResortCharaModel;
GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   ポケリゾート：管理人メニュー
//------------------------------------------------------------------------------
class UpperHomeMenuDraw
  : public UpperBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN(UpperHomeMenuDraw);

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  UpperHomeMenuDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~UpperHomeMenuDraw( void ) { ; }


private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void Initialize2D( InitParam& param );
  
  //------------------------------------------------------------------
  /**
   * @brief   クラスの初期化
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void InitializeClass( InitParam& param );
  
  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化
   */
  //------------------------------------------------------------------
  virtual void InitializePane( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  virtual bool Terminate( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );


public:
  enum DispType {
    DISP_TYPE_TALK,
    DISP_TYPE_HOME_MENU,
    DISP_TYPE_NUM,
  };
public:
  //------------------------------------------------------------------
  /**
   * @brief   開く
   */
  //------------------------------------------------------------------
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   開くタイプの設定
   *
   * @param   type    画面タイプ
   */
  //------------------------------------------------------------------
  void SetDispType( DispType type );

private:
  //------------------------------------------------------------------
  /**
   * @brief   画面モードの設定
   */
  //------------------------------------------------------------------
  void SetDispMode( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   数値の更新
   */
  //------------------------------------------------------------------
  void UpdateCount( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   キャラモデルのセット
   *
   * @param   charaModel    キャラモデルクラス
   */
  //------------------------------------------------------------------
  void SetCharaModel( ResortCharaModel* charaModel ) { m_pCharaModel = charaModel; }


private:
  static const u32    UPDATE_FRAME = 30;

  //! レイアウトID
  enum LytID
  {
    LYTID_FRAME = 0,  //!< フレーム
    LYTID_BG,         //!< 背景
    LYTID_MAX,        //!< 最大値
  };

private:
  DrawerBase*         m_pDrawerBaseBG;  //!< 背景レイアウト用

  ResortCharaModel*   m_pCharaModel;

private:
  DispType    m_DispType;

  u32         m_UpdateFrame;

  u8          m_MainSeq;

private:
  gfl2::lyt::LytPane*     m_pDispVisiblePane;

  enum {
    NUM_TEXT_MAME_NORMAL,
    NUM_TEXT_MAME_RARE,
    NUM_TEXT_MAME_S_RARE,
    NUM_TEXT_POKE_COUNT,
    NUM_TEXT_NUM,
  };
  gfl2::lyt::LytTextBox*  m_pNumText[NUM_TEXT_NUM];
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_UPPER_HOME_MENU_DRAW_H__