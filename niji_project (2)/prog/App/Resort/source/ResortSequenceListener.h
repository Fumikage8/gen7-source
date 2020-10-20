//==============================================================================
/**
 * @file	  ResortSequenceListener.h
 * @brief	  シーケンスの切り替えを相互伝達する
 * @author	k.ohno
 * @date	  2015.09.30
 */
// =============================================================================

#if !defined( __RESORT_SEQUENCE_LISTENER_H__ )
#define	__RESORT_SEQUENCE_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

class ResortSequenceListener {


public:

  //--------------------------------------------------------------------------
  /**
   * @brief   ホームメニューオープン
   */
  //--------------------------------------------------------------------------
  virtual void OpenHomeMneu( bool flg ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンをタッチした後、メニューフローが終わったときに呼ばれる
   */
  //--------------------------------------------------------------------------
  virtual void ClosePokemonTouchMenu( void ) = 0;



  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンをタッチ出来る状態になっていればtrue 忙しいとfalse
   */
  //--------------------------------------------------------------------------
  virtual bool IsPokemonTouchEnable( void ) = 0;

  
  
};

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif	/* __RESORT_SEQUENCE_LISTENER_H__ */
