//===========================================================================
/**
 * @file  gfl_HioSwitch.h
 * @brief GAMEFREAK Library HIO有効無効の切り替えフラグ
 * @author  GAMEFREAK inc.  taya
 * @date  2012.09.19
 *
 * @note  C / C++ 双方から参照できるように記述してください。
 */
//===========================================================================

#ifndef __GFL_HIO_SWITCH_H__
#define __GFL_HIO_SWITCH_H__
#pragma once

// HIO によるファイルアクセスが可能か？
#if GFL_DEBUG

  #ifdef NN_SWITCH_ENABLE_HOST_IO  // PC通信用
    #ifdef GFL_USE_HIO_SOUND_EDIT
      #define GFL_HIO_ENABLE 0  //SoundEdit機能を使う時は GFL_HIO_ENABLEはfalse
    #else
      #define GFL_HIO_ENABLE 0
    #endif
  #else
    #define GFL_HIO_ENABLE 0
  #endif

#else
  #define GFL_HIO_ENABLE 0

#endif




#endif

