//======================================================================
/**
 * @file    FinderStudioCaptureZMotionData.cpp
 * @date    2016/12/01 13:28:11
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：Zモーションデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureZMotionData.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

PlayerMotionData::MotionButton PlayerMotionData::m_DefaultButtonList = {
  MOTION_TYPE_NUM, POKETYPE_NULL, Viewer::Z_MOTION_TYPE_MAX, ITEM_DUMMY_DATA
};

PlayerMotionData::MotionButton PlayerMotionData::m_StaticButtonList[PlayerMotionData::PLAYER_MOTION_NUM] = {
  { PlayerMotionData::MOTION_TYPE_ALORA,   POKETYPE_NULL,    Viewer::Z_MOTION_TYPE_GREET,   ITEM_DUMMY_DATA },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_NORMAL,  Viewer::Z_MOTION_TYPE_NORMAL,  ITEM_DUMMY_DATA },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_KAKUTOU, Viewer::Z_MOTION_TYPE_KAKUTOU, ITEM_KAKUTOUBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_MIZU,    Viewer::Z_MOTION_TYPE_MIZU,    ITEM_MIZUBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_HONOO,   Viewer::Z_MOTION_TYPE_HONOO,   ITEM_HONOOBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_KUSA,    Viewer::Z_MOTION_TYPE_KUSA,    ITEM_KUSABIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_IWA,     Viewer::Z_MOTION_TYPE_IWA,     ITEM_IWABIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_DENKI,   Viewer::Z_MOTION_TYPE_DENKI,   ITEM_DENKIBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_GHOST,   Viewer::Z_MOTION_TYPE_GOOSUTO, ITEM_GOOSUTOBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_MUSHI,   Viewer::Z_MOTION_TYPE_MUSI,    ITEM_MUSIBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_AKU,     Viewer::Z_MOTION_TYPE_AKU,     ITEM_AKUBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_DOKU,    Viewer::Z_MOTION_TYPE_DOKU,    ITEM_DOKUBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_HAGANE,  Viewer::Z_MOTION_TYPE_HAGANE,  ITEM_HAGANEBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_FAIRY,   Viewer::Z_MOTION_TYPE_FEARII,  ITEM_FEARIIBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_JIMEN,   Viewer::Z_MOTION_TYPE_ZIMEN,   ITEM_ZIMENBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_KOORI,   Viewer::Z_MOTION_TYPE_KOORI,   ITEM_KOORIBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_HIKOU,   Viewer::Z_MOTION_TYPE_HIKOU,   ITEM_HIKOUBIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_ESPER,   Viewer::Z_MOTION_TYPE_ESUPAA,  ITEM_ESUPAABIIZU },
  { PlayerMotionData::MOTION_TYPE_ZMOTION, POKETYPE_DRAGON,  Viewer::Z_MOTION_TYPE_DORAGON, ITEM_DORAGONBIIZU },
  { PlayerMotionData::MOTION_TYPE_PIKA_Z,  POKETYPE_NULL,    Viewer::Z_MOTION_TYPE_PIKA,    ITEM_PIKATYUUBIIZU },
};


PlayerMotionData::MotionButton PlayerMotionData::GetMotionButton( u32 index )
{
  if( index >= PLAYER_MOTION_NUM )
  {
    return GetDefaultMotionButton();
  }

  return m_StaticButtonList[index];
}


PlayerMotionData::MotionButton PlayerMotionData::GetMotionButton( Viewer::ZMotionType type )
{
  for( u32 i = 0; i < PLAYER_MOTION_NUM; ++i )
  {
    if( m_StaticButtonList[i].z_motion == type )
    {
      return m_StaticButtonList[i];
    }
  }

  return GetDefaultMotionButton();
}

PlayerMotionData::MotionButton PlayerMotionData::GetMotionButton( PokeType_tag type )
{
  for( u32 i = 0; i < PLAYER_MOTION_NUM; ++i )
  {
    if( m_StaticButtonList[i].poke_type == type )
    {
      return m_StaticButtonList[i];
    }
  }

  return GetDefaultMotionButton();
}


GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
