//==============================================================================
/**
 * @file	  ResortChangeIslandListener.h
 * @brief	  島の切り替え
 * @author	k.ohno
 * @date	  2015.09.21
 */
// =============================================================================

#if !defined( __RESORT_CHANGE_ISLAND_LISTENER_H__ )
#define	__RESORT_CHANGE_ISLAND_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

class ResortChangeIslandListener {

public:

  
public:
  //--------------------------------------------------------------------------
  /**
   * @brief   切り替え開始
   * @param   dir   移動方向(0: 左、1: 右)
   * @param   dir   移動方向(0: 左、1: 右)
   *  @return  切りかえれる場合true
   */
  //--------------------------------------------------------------------------
  virtual bool ChangeStart( int id,bool bfade ) = 0;

  enum ModelAnim_e{
    ISLAND_MODELANIM_BEANS,
    ISLAND_MODELANIM_TREE,
    ISLAND_MODELANIM_BEANSBOX,
    ISLAND_MODELANIM_ADVENTURE_IN,
    ISLAND_MODELANIM_ADVENTURE_OUT,
  };
  //--------------------------------------------------------------------------
  /**
   * @brief   モデルアニメ開始
   * @param   type  ModelAnim_e
   */
  //--------------------------------------------------------------------------
  virtual void StartModelAnim( int type ) = 0;



};

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif	/* __RESORT_CHANGE_ISLAND_LISTENER_H__ */
