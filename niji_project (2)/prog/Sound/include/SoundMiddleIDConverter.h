//=============================================================================
/**
 * @brief サウンド中間IDをNWのサウンドアイテムIDに変換する
 * @file SoundMiddleIDConverter.h
 * @author obata_toshihiro
 * @date 2012.01.23
 */
//=============================================================================
#ifndef __MIDDLEIDCONVERTER_H__
#define __MIDDLEIDCONVERTER_H__ 
#pragma once


#include "niji_conv_header/sound/SoundMiddleIDTable.h"

namespace Sound {


  class MiddleIDConverter
  {
    public:

      /**
       * @brief 中間IDからサウンドアイテムIDを求める
       * @param middleID サウンド中間ID
       * @return NWサウンドアイテムID
       */
      static u32 CalcSoundItemID( u32 middleID ); 
  };


}  // namespace Sound

#endif  // __MIDDLEIDCONVERTER_H__
