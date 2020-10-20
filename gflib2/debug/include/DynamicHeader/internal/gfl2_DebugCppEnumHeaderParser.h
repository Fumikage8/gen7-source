//=============================================================================
/**
* @brief  ヘッダファイルを解析しEnumの定義を取得するクラス
* @file   gfl2_CppEnumHeaderParser.h
* @author Kanamaru Masanori
* @date   2015.2.18
*/
//=============================================================================

#ifndef __GFL_DEBUG_CPPENUMHEADERPARSER_H__
#define __GFL_DEBUG_CPPENUMHEADERPARSER_H__
#pragma once

#include <debug/include/DynamicHeader/internal/gfl2_DebugFixedStack.h>
#include <debug/include/DynamicHeader/internal/gfl2_DebugTextFileReadStream.h>

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {
      //--------------------------------------------------
      /**
      * @class CppEnumHeaderParser
      * @brief ヘッダファイルを解析しEnumの定義を取得するクラス
      */
      //--------------------------------------------------
      class CppEnumHeaderParser
      {
        /**
        * @enum	State
        * @brief	解析中のスコープ
        */
        enum State
        {
          Global,			/**<  */
          BlockComment,	/**< ブロックコメント内 */
          Enum,			/**< Enum定義内 */
          EnumReady		/**< Enum定義直前 */
        };

        static const int MaxScopeDepth = 8;   /**< 解析時のスコープの深さ */
        static const int LineMaxLength = 256; /**< 解析対象のファイルの一行あたりの最大文字数 */

        FixedStack<State, MaxScopeDepth> stateStack;

        TextFileReadStream trs;

        int currentValue;

      public:
        //----------------------------------------
        /**
        * @brief	初期化処理
        *
        * @param	path      処理するファイルのパス
        * @param	pRootPath pathのルート設定
        * @param	pHeap	    使用するヒープへのポインタ
        */
        //----------------------------------------
        void Initialize(const char* path, const RootPathConfig* pRootPath, gfl2::heap::HeapBase* pHeap);

        //----------------------------------------
        /**
        * @brief	enumで定義された定数値のキーと値の組を1組取得する
        *
        * @param	outKey		キー文字列
        * @param	outValue	キーに対応する値
        *
        * @retval	true	定義を取得した
        * @retval	false	定義を取得できず、ファイルを末端まで読み終わった
        */
        //----------------------------------------
        bool ReadDefinition(char* outKey, int* outValue);

        //----------------------------------------
        /**
        * @brief	終了処理
        */
        //----------------------------------------
        void Finalize();
      };

    } // DynamicHeader
  } // debug
} // gfl

#endif
