//==============================================================================
/**
 * @file	  BoxDrawPartsListener.h
 * @brief	  部品の取得をしたい時のlistener
 * @author	k.ohno
 * @date	  Jan/16/2015
 */
// =============================================================================

#if !defined( __BOX_DRAW_PARTS_LISTENER_H__ )
#define	__BOX_DRAW_PARTS_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxDrawPartsListener {


public:
  virtual gfl2::lyt::LytParts* GetIconParts(int pos)=0;
  virtual gfl2::lyt::LytParts* GetCarryIconParts(int pos)=0;
  virtual gfl2::lyt::LytParts* GetTemotiIconParts(int pos)=0;
  virtual void TemotiIconPartsReady(int page,int pos) = 0;

  
};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif	/* __BOX_DRAW_PARTS_LISTENER_H__ */
