//==============================================================================
/**
 * @file	  ResortTouchToFlowListener.h
 * @brief	  タッチから流れに遷移するもの
 * @author	k.ohno
 * @date	  2015.09.30
 */
// =============================================================================

#if !defined( __RESORT_TOUCH_TO_FLOW_LISTENER_H__ )
#define	__RESORT_TOUCH_TO_FLOW_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

class ResortTouchToFlowListener {


public:

  enum TouchKind{
    TOUCH_BEANS_BOX,
    TOUCH_BOARD,
    TOUCH_KINOMI_FARM,  //木の実の位置
    TOUCH_ADVENTURE,
    TOUCH_FARM_IMPUT_LIST,
    TOUCH_GIM,
    TOUCH_POKEMON_SELECT_GIM,
    TOUCH_POKEMON_SELECT_HOTSPA,
    TOUCH_POKEMON,   //ポケモンをタッチ
    TOUCH_BEANS,     //落ちてるまめをタッチ
    TOUCH_BEANS_BOTTLE,  //まめびんをたっち
		TOUCH_FARM_PLANT_PLACE, //木の実の植えてる場所

    TOUCH_TUTORIAL_STATE_TOUCH_BEANS, //ポケマメタッチ時チュートリアル
    TOUCH_TUTORIAL_STATE_TOUCH_TREE,  //ポケマメの木をタッチ時チュートリアル
    TOUCH_TUTORIAL_STATE_TOUCH_BOX,   //ポケマメいれをタッチ時チュートリアル

    ADVENTURE_INANIM_END, //探検の行く時のアニメ終了
    
  };


  
  //--------------------------------------------------------------------------
  /**
   * @brief   タッチを通達
   */
  //--------------------------------------------------------------------------
  virtual void OnTouchToFlow( TouchKind kind,int param ) = 0;


  //--------------------------------------------------------------------------
  /**
   * @brief   タッチを通達
   */
  //--------------------------------------------------------------------------
  virtual void OnTouchToFlowXY( TouchKind kind,int index, int x,int y ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   抑制だけ行う
   */
  //--------------------------------------------------------------------------
  virtual void ResortInputEnabled( bool bflg )=0;

  //--------------------------------------------------------------------------
  /**
   * @brief   ボタンのONOFF
   */
  //--------------------------------------------------------------------------
  virtual void ButtonDisplayTypeEnabled( bool bflg )=0;
  

  //木の実畑の収穫数アニメ終了を得る
  virtual bool IsAnimEndGetEffect(void)=0;


  //ボタンアニメ中かを得る
  virtual bool IsButtonIdleState(void)=0;

  
  
  //チュートリアル用

  enum{
    TUTORIAL_BEANS_POS_X = 320/2+30, //チュートリアルでまめが落ちている場所 X座標
    TUTORIAL_BEANS_POS_Y = 128, //チュートリアルでまめが落ちている場所 Y座標

    TUTORIAL_TREE_POS_X = 320/2, //チュートリアルで木の場所 X座標
    TUTORIAL_TREE_POS_Y = 80, //チュートリアルで木の場所Y座標

    TUTORIAL_BEANSBOX_POS_X = 320/2-35, //チュートリアルではこの場所 X座標
    TUTORIAL_BEANSBOX_POS_Y = 110, //チュートリアルではこの場所 Y座標

  };


  
};

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif	/* __RESORT_TOUCH_TO_FLOW_LISTENER_H__ */
