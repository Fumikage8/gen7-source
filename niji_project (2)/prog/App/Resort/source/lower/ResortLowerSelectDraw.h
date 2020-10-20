#if !defined( __RESORT_LOWER_SELECT_DRAW_H__ )
#define __RESORT_LOWER_SELECT_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    ResortLowerSelectDraw.h
 * @date    2015/09/09 18:23:06
 * @author  fukushima_yuya
 * @brief   ポケリゾート：選択画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerBaseDraw.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   選択画面(下)の描画クラス
//------------------------------------------------------------------------------
class LowerSelectDraw
  : public LowerBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerSelectDraw);

public:
  //! 選択画面のタイプ
  enum SelectType
  {
    SEL_TYPE_YESNO,
    SEL_TYPE_HOUSE,
    SEL_TYPE_STATUS,
    SEL_TYPE_NUM,
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  LowerSelectDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LowerSelectDraw( void ) { ; }


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


public:
  //------------------------------------------------------------------
  /**
   * @brief   選択タイプの設定
   *
   * @param   type    選択タイプ
   */
  //------------------------------------------------------------------
  void SetSelectType( SelectType type ) { m_SelectType = type; }

  //------------------------------------------------------------------
  /**
   * @brief   開く
   */
  //------------------------------------------------------------------
  virtual void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  virtual void Close( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   ボタンのテキスト設定
   *
   * @param   type    選択画面のタイプ
   */
  //------------------------------------------------------------------
  void SetBtnText( SelectType type );


private:   //!< app::ui::UIInputListener
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );


private:   //!< app::tool::ButtonManager
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの個別セットアップ
   *
   * @param   btnMan    ボタンマネージャ
   */
  //------------------------------------------------------------------
  virtual void SetupButton( app::tool::ButtonManager* btnMan );


private:
  SelectType    m_SelectType;
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_LOWER_SELECT_DRAW_H__