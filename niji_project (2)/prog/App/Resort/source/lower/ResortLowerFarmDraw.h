#if !defined( __RESORT_LOWER_FARM_DRAW_H__ )
#define __RESORT_LOWER_FARM_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    ResortLowerFarmDraw.h
 * @date    2015/10/01 20:16:51
 * @author  fukushima_yuya
 * @brief   ポケリゾート：はたけのようす画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerBaseDraw.h"
#include <App/Resort/source/ResortConst.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   ポケリゾート：はたけのようす画面(下)
//------------------------------------------------------------------------------
class LowerFarmDraw
  : public LowerBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerFarmDraw);

public:
  // はたけの状態
  enum FarmState
  {
    FARM_STATE_PLANTED,     //!< 植えた
    FARM_STATE_SPROUT,      //!< 芽
    FARM_STATE_SAPLING,     //!< 若木
    FARM_STATE_TREE,        //!< 木
    FARM_STATE_HARVEST,     //!< 収穫
    FARM_STATE_NUM,
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  LowerFarmDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LowerFarmDraw( void ) { ; }


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


public:
  //------------------------------------------------------------------
  /**
   * @brief  更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   きのみのインデックスの設定(0～17)
   *
   * @param   kinomiIdx   きのみ
   */
  //------------------------------------------------------------------
  void SetKinomiIndex( u8 kinomiIdx ) { m_KinomiIndex = kinomiIdx; }
  
  //------------------------------------------------------------------
  /**
   * @brief   開く
   */
  //------------------------------------------------------------------
  virtual void Open( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   時間表示の更新
   */
  //------------------------------------------------------------------
  void UpdateTime( void );
  
private:
  //------------------------------------------------------------------
  /**
   * @brief   畑の状態を取得
   *
   * @param   time    きのみの生育状態
   *
   * @return  畑の状態
   */
  //------------------------------------------------------------------
  FarmState GetFarmState( TimeParam time );

  //------------------------------------------------------------------
  /**
   * @brief   畑の状態をセットアップ
   */
  //------------------------------------------------------------------
  void SetupFarmState( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   はたけの状態のセットアップ：状態メッセージ
   */
  //------------------------------------------------------------------
  void SetupFarmState_Msg( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   畑インデックスの取得
   *
   * @return  畑インデックス
   */
  //------------------------------------------------------------------
  u8 GetFieldIndex( void ) const;

private:   //!< app::ui::UIInputListener
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   *
   * @param   paneID
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneID );


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
  u8    m_KinomiIndex;

private:
  gfl2::lyt::LytTextBox*    m_pItemName;

  gfl2::lyt::LytTextBox*    m_pTimeMsg;

  gfl2::lyt::LytTextBox*    m_pStateMsg;
  
  gfl2::lyt::LytPane*       m_pIconVisPane;
  
  gfl2::lyt::LytPicture*    m_pKinomiPic;
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_LOWER_FARM_DRAW_H__