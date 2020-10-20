#//=============================================================================
/**
* @brief  動的ヘッダシステムの設定に関するファイル
* @file   gfl2_DebugDynamicHeaderDefine.h
* @author Kanamaru Masanori
* @date   2015.2.20
*/
//=============================================================================

#ifndef __GFL_DEBUG_DYNAMICHEADERDDEFINE__
#define __GFL_DEBUG_DYNAMICHEADERDDEFINE__
#pragma once

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader{
      
      //Libコンパイル時に決定している必要があるパラメータ
      enum
      {
        KeyMaxLength = 70, /**< 定数とファイルパス文字列の最大長 */
        RootPathArrayMax = 4, //++< ルート候補として設定できるパスの数の最大値 */
      };

      //--------------------------------------------------
      /**
      * @brief  ルートパス候補の集合
      */
      //--------------------------------------------------
      struct RootPathConfig
      {
        const char* rootPathArray[RootPathArrayMax];
      };


    } // DynamicHeader
  } // debug
} //gfl

#endif