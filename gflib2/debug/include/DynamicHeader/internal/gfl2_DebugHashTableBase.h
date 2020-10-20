//=============================================================================
/**
* @brief  ハッシュテーブル基底クラス
* @file   gfl2_DebugHashTableBase.h
* @author Kanamaru Masanori
* @date   2015.2.20
*/
//=============================================================================

#ifndef __GFL_DEBUG_HASHTABLEBASE_H__
#define __GFL_DEBUG_HASHTABLEBASE_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {
      //--------------------------------------------------
      /**
      * @class	HashTableBase
      * @brief	ハッシュテーブルの基底クラス
      *			格納可能な要素数はコンストラクタで決定し、自動拡張はしない
      *			検索、ハッシュ化、追加を実装
      *			追加時の「追加位置の取得」は派生クラスで実装する
      *
      * @tparam TKey		キーの型
      * @tparam TValue	値の型
      */
      //--------------------------------------------------
      template<typename TKey, typename TValue>
      class HashTableBase
      {
      public:
        typedef u16  IdxType;

      protected:
        static const IdxType TERMINATE_VALUE = USHRT_MAX;

      public:
        //--------------------------------------------------
        /**
        * @class	Iterator
        * @brief	テーブルのイテレータクラス
        */
        //--------------------------------------------------
        class Iterator
        {
          HashTableBase* pSrcTable;

          IdxType currentBucketIdx;
          IdxType currentEntryIdx;

        public:
          // コンストラクタ
          Iterator(HashTableBase* pSrc);
          // イテレータを先頭に戻す
          void Reset();
          // 次の要素を取得する
          bool Next(TKey* outKey, TValue* outValue);
        };

        //--------------------------------------------------
        /**
        * @class	KeyDependsMethods
        * @brief	キーの型に依存したメソッドを部分特殊化するためのクラス
        *
        * @tparam TDKey	キーの型
        * @tparam TDumy	ダミー(常にvoid)
        */
        //--------------------------------------------------
        template<typename TDKey, typename TDumy = void>
        class KeyDependsMethods
        {
        public:
          //--------------------------------------------------
          /**
          * @brief	キーからハッシュ値(エントリ数非依存)を取得する
          *
          * @param	key	キー
          *
          * @return	ハッシュ値
          */
          //--------------------------------------------------
          static u32 GetRowHash(TDKey key);

          //--------------------------------------------------
          /**
          * @brief	キーの比較
          *
          * @param	a	比較する値
          * @param	b	比較する値
          *
          * @return		aとbが同値であるかどうか
          */
          //--------------------------------------------------
          static bool KeyEqual(TDKey a, TDKey b);
        };

        //--------------------------------------------------
        /**
        * @brief	コンストラクタ
        *
        * @param	capacity	最大格納要素数
        * @param	pHeap		利用するヒープへのポインタ
        */
        //--------------------------------------------------
        HashTableBase(IdxType capacity, gfl2::heap::HeapBase* pHeap);

        //--------------------------------------------------
        /**
        * @brief	デストラクタ
        */
        //--------------------------------------------------
        virtual ~HashTableBase();

        //--------------------------------------------------
        /**
        * @brief	要素の追加
        *
        * @param	key		キー
        * @param	value	値
        *
        * @return			追加に成功したかどうか
        */
        //--------------------------------------------------
        bool Add(TKey key, TValue value);

        //--------------------------------------------------
        /**
        * @brief	キーによる値の検索
        *
        * @param	key			検索するキー
        * @param	isSuccess	値が見つかったかどうか
        *
        * @return				(isSuccess==trueのときのみ)検索結果の値
        */
        //--------------------------------------------------
        TValue GetValue(TKey key, bool* isSuccess = NULL) const;

        //--------------------------------------------------
        /**
        * @brief	イテレータ取得
        *			イテレータ初期化コストを削減するために実体はテーブルが抱えている
        *			スレッドセーフではないので注意
        *
        * @return				イテレータへのポインタ
        */
        //--------------------------------------------------
        Iterator* GetIterator()
        {
          this->iterator.Reset();
          return &(this->iterator);
        }

      protected:
        //--------------------------------------------------
        /**
        * @brief	要素の「値」に対応するノード
        */
        //--------------------------------------------------
        struct Entry
        {
          TValue value;		/**< 要素の値 */
          IdxType nextIdx;	/**< 次のエントリのインデックス */
        };

        /**
        * @brief	要素の許容量
        */
        IdxType capacity;

        /**
        * @brief	ハッシュ値毎の先頭エントリインデックス配列
        */
        IdxType* buckets;

        /**
        * @brief	要素格納配列
        */
        Entry* entrys;


        //--------------------------------------------------
        /**
        * @brief	使用されていないインデックスを取得する
        *
        * @return	使用可能なentrysのインデックス
        */
        //--------------------------------------------------
        virtual IdxType GetAvaiableIndex() = 0;

        //--------------------------------------------------
        /**
        * @brief	指定したインデックスのエントリーに対応するキーを取得
        *
        * @param	index	エントリーのインデックス
        *
        * @return	キー
        */
        //--------------------------------------------------

        virtual TKey GetKey(typename HashTableBase<TKey,TValue>::IdxType index) const = 0;

        //--------------------------------------------------
        /**
        * @brief	指定したインデックスのエントリーのキーをセット
        *
        * @param	index	エントリーのインデックス
        * @param	key		設定するキー
        */
        //--------------------------------------------------

        virtual void SetKey(typename HashTableBase<TKey,TValue>::IdxType index, TKey key) = 0;


        //--------------------------------------------------
        /**
        * @brief	追加処理後に呼び出される
        *          (追加成功時のみでなく、キー重複時にも呼ばれる)
        *
        * @param	entryIndex	追加された(重複していた)エントリーのindex
        * @param	isSuccess	追加処理が成功したかどうか
        */
        //--------------------------------------------------
        virtual void PostAdd(IdxType entryIndex, bool isSuccess)
        {
          GFL_ASSERT(isSuccess);
        }

        //--------------------------------------------------
        /**
        * @brief	キーに対応するエントリーのインデックスを取得する
        *
        * @param	key		対象のキー
        * @param	hash	キーから計算したハッシュ値
        *
        * @return			エントリーのインデックス
        */
        //--------------------------------------------------
        IdxType GetEntryIndex(TKey key, IdxType hash) const;

        //--------------------------------------------------
        /**
        * @brief	キーからハッシュ値(エントリ数依存)を取得する
        *
        * @param	key		キー
        * @return			ハッシュ値
        */
        //--------------------------------------------------
        IdxType GetHash(TKey key) const;

      private:
        Iterator iterator;
      };

    } // DynamicHeader
  } // debug
} // gfl

#endif
