#if !defined( __FINDER_STUDIO_CAPTURE_ZMOTION_DATA_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_ZMOTION_DATA_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureZMotionData.h
 * @date    2016/12/01 13:21:27
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：Zモーションデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

class PlayerMotionData
{
public:
  enum {
    PLAYER_MOTION_NUM = 20,
  };
  enum MotionType
  {
    MOTION_TYPE_ZMOTION,
    MOTION_TYPE_ALORA,
    MOTION_TYPE_PIKA_Z,
    MOTION_TYPE_NUM,
    MOTION_TYPE_NULL = MOTION_TYPE_NUM,
  };

  class MotionButton
  {
  public:
    MotionType          motion_type;
    PokeType_tag        poke_type;
    Viewer::ZMotionType z_motion;
    u16                 itemNo;
  };

public:
  static MotionButton GetMotionButton( u32 index );
  static MotionButton GetMotionButton( Viewer::ZMotionType type );
  static MotionButton GetMotionButton( PokeType_tag type );
  static MotionButton GetDefaultMotionButton( void ) { return m_DefaultButtonList; }

private:
  static MotionButton   m_DefaultButtonList;
  static MotionButton   m_StaticButtonList[PLAYER_MOTION_NUM];
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_ZMOTION_DATA_H_INCLUDED__
