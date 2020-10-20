//=============================================================================
/**
 * @brief サウンド中間IDをNWのサウンドアイテムIDに変換する
 * @file SoundMiddleIDConverter.cpp
 * @author obata_toshihiro
 * @date 2012.01.23
 */
//=============================================================================

#if defined(GF_PLATFORM_WIN32)
#include <windows.h>
#endif

#include <debug/include/gfl2_Assert.h>
#include <types/include/gfl2_Typedef.h>

#include "Sound/include/SoundMiddleIDConverter.h"
#include "niji_conv_header/sound/SoundMiddleIDTable.h"
#include "niji_conv_header/sound/niji_sound.nwid"
#include "niji_conv_header/sound/SoundMiddleIDTable.hpp"

namespace Sound {

  /**
   * @brief 中間IDからサウンドアイテムIDを求める
   * @param middleID サウンド中間ID
   * @return NWサウンドアイテムID
   */
  u32 MiddleIDConverter::CalcSoundItemID( u32 middleID )
  {
    MiddleIDGroup group = static_cast<MiddleIDGroup>( middleID >> 16 );
    u32 index = middleID & 0xffff;
    
    if( ( group < MID_GROUP_NUM ) &&
        ( MID_TABLE[ group ].IDArray ) &&
        ( index < MID_TABLE[ group ].IDCount ) )
    {
      return MID_TABLE[ group ].IDArray[ index ];
    }

    GFL_ASSERT_MSG(0,"mid=%d\n",middleID);
    return 0;
  }


}  // namespace Sound

