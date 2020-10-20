//=============================================================================
/**
* @brief  外部テキストファイルを読み込むクラス(デバッグ専用)
* @file   gfl2_DebugTextFileReadStream.h
* @author Kanamaru Masanori
* @date   2015.2.18
*/
//=============================================================================

#ifndef __GFL_DEBUG_TEXTFILEREADSTREAM_H__
#define __GFL_DEBUG_TEXTFILEREADSTREAM_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>

#include <fs/include/gfl2_Fs.h>

#include <debug/include/DynamicHeader/internal/gfl2_DebugDynamicHeaderDefine.h>

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {

      //--------------------------------------------------
      /**
      * @class TextFileReadStream
      * @brief デバッグ専用テキストファイルリーダ(簡易)
      */
      //--------------------------------------------------
      class TextFileReadStream
      {
      private:
        size_t fileSize;
        char*  contentBuffer;
        size_t currentOffset;

      public:
        //--------------------------------------------------
        /**
        * @brief	初期化処理
        *
        * @param	path	    読み込むファイルパス
        * @param	pRootPath pathのルートとなるパスの設定
        * @param	pHeap	    使用するヒープへのポインタ
        */
        //--------------------------------------------------
        void Initialize(const char* path, const RootPathConfig* pRootPath, gfl2::heap::HeapBase* pHeap);

        //--------------------------------------------------
        /**
        * @brief	終了処理(ファイルのクローズ)
        */
        //--------------------------------------------------
        void Finalize();

        //--------------------------------------------------
        /**
        * @brief	1行取得する
        *			1行がmaxLengthを超えていた場合はアサートを発生させる
        *
        * @param	outBuffer	出力先
        * @param	maxLength	読み出す最大の長さ
        *
        * @return	実際に読み出した文字列の長さ
        */
        //--------------------------------------------------
        size_t ReadLine(char* outBuffer, size_t maxLength);
      };
    }
  }
}

#endif // __GFL_DEBUG_TEXTFILEREADSTREAM_H__
