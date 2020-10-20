//=============================================================================
/**
* @brief  動的ヘッダシステム
* @file   gfl2_DebugHeaderHashManager.h
* @author Kanamaru Masanori
* @date   2015.2.20
*/
//=============================================================================

#ifndef __GFL_DEBUG_HEADERHASHMANAGER_H__
#define __GFL_DEBUG_HEADERHASHMANAGER_H__
#pragma once

#include <debug/include/DynamicHeader/internal/gfl2_DebugAddOnlyHashTable.h>
#include <debug/include/DynamicHeader/internal/gfl2_DebugRemovableHashTable.h>
#include <debug/include/DynamicHeader/internal/gfl2_DebugDynamicHeaderDefine.h>

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {

      //--------------------------------------------------
      /**
      * @class HeaderHashManager
      * @brief 動的ヘッダシステムの実体(Staticクラス)
      */
      //--------------------------------------------------
      class HeaderHashManager
      {
      public:
        // 1ヘッダファイルに対応するテーブル
        typedef AddOnlyHashTable<const char*, int> LabelHash;
        // ヘッダファイルテーブルのハッシュテーブル
        typedef RemovableHashTable<const char*, LabelHash*> MasterHash;

        //--------------------------------------------------
        /**
        * @brief	初期化
        *
        * @param  pHeap               利用するヒープのポインタ
        * @param  maxLabelHashAmount  同時ロード可能なファイルの上限	
        * @param  heapAlertLimit      ヒープ使用量の警告を出す閾値
        * @param  pRootPath            ロードされるヘッダパスのルート設定
        */
       //--------------------------------------------------
        static void Initialize(gfl2::heap::HeapBase* pHeap, const MasterHash::IdxType maxLabelHashAmount, const size_t heapAlertLimit, const RootPathConfig* pRootPath);

        //--------------------------------------------------
        /**
        * @brief	終了処理
        */
        //--------------------------------------------------
        static void Finalize();

        //--------------------------------------------------
        /**
        * @brief	ヘッダファイルを読み込み、ハッシュテーブルとしてロードする
        *			同時ロード上限を超えてロードしようとするとアサートで止まる
        *			読み込み済みのファイルを指定した場合は参照カウントのみ進めます
        *
        * @param	filePath	読み込むファイルパス
        * @param	capacity	ファイルから読み出す定義数 (実際に定義されていた数がこの値を超えるとアサートを出します)
        */
        //--------------------------------------------------
        static void LoadHeaderHash(const char* filePath, const LabelHash::IdxType capacity);

        //--------------------------------------------------
        /**
        * @brief	ハッシュテーブルのアンロード
        *			参照カウントが残っている場合はアンロードしません
        *
        * @param	filePath	対象ファイルパス(const char*)
        */
        //--------------------------------------------------
        static void UnLoadHeaderHash(const char* filePath);

        //--------------------------------------------------
        /**
        * @brief	値取得(filePathを指定すると高速に取得できます)
        *
        * @param	keyword		    定数名
        * @param defaultValue  無効時に返す値
        * @param	filePath	    対象ヘッダファイルのパス
        *
        * @return	keywordに対応した値
        */
        //--------------------------------------------------
        static s32 GetValue(const char* keyword, const s32 defaultValue, const char* filePath = NULL);

        //--------------------------------------------------
        /**
        * @brief	現在ロードしているヘッダの内容を出力
        */
        //--------------------------------------------------
        static void Dump();

        //--------------------------------------------------
        /**
        * @brief システムの有効/無効を変更する
        *
        * @param 変更する値
        */
        //--------------------------------------------------
        static void SetIsEnable(bool value);

        //--------------------------------------------------
        /**
        * @brief システムの有効/無効を取得する
        *
        * @return システムが有効か
        */
        //--------------------------------------------------
        static bool GetIsEnable();

      private:

        /**
        * @brief システムが有効か(実行時にユーザが操作可能な値)
        */
        static bool isEnable;

        /**
        * @brief ファイルパス -> ハッシュテーブルのテーブル
        */
        static MasterHash* hashRoot;

        /**
        * @brief 利用するヒープへのポインタ
        */
        static gfl2::heap::HeapBase* pHeap;

        /**
        * @brief ヒープ使用量警告の閾値
        */
        static size_t heapAlertLimit;

        /**
        * @brief ロードするファイルのルートパス
        */
        static const RootPathConfig* pRootPath;
      };

    } // DynamicHeader
  } // debug
} // gfl

#endif
