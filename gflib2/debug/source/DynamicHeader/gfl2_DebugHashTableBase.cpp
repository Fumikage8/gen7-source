//=============================================================================
/**
* @brief  ハッシュテーブル基底クラス実装
* @file   gfl2_DebugHashTableBase.cpp
* @author Kanamaru Masanori
* @date   2015.2.19
*/
//=============================================================================

#include <debug/include/DynamicHeader/internal/gfl2_DebugHashTableBase.h>

// 明示的特殊化のために仕方なく...
#include <debug/include/DynamicHeader/internal/gfl2_DebugAddOnlyHashTable.h>

// HashTableBaseの実装
namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {
      //--------------------------------------------------
      /**
      * @brief	コンストラクタ
      *
      * @param	capacity	最大格納要素数
      * @param	pHeap		利用するヒープへのポインタ
      */
      //-------------------------------------------------
      template<typename TKey, typename TValue>
      HashTableBase<TKey, TValue>::HashTableBase(IdxType capacity, gfl2::heap::HeapBase* pHeap)
        :capacity(capacity),
        iterator(this)
      {
        buckets = GFL_NEW(pHeap) IdxType[capacity];
        entrys = GFL_NEW(pHeap) Entry[capacity];

        for (int i = 0; i < capacity; ++i)
        {
          buckets[i] = TERMINATE_VALUE;
        }
      }

      //--------------------------------------------------
      /**
      * @brief	デストラクタ
      */
      //--------------------------------------------------
      template<typename TKey, typename TValue>
      HashTableBase<TKey, TValue>::~HashTableBase()
      {
        GFL_DELETE[] buckets;
        GFL_DELETE[] entrys;
      }

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
      template<typename TKey, typename TValue>
      bool HashTableBase<TKey, TValue>::Add(TKey key, TValue value)
      {
        IdxType hash = this->GetHash(key);

        // キーの重複チェック
        IdxType exist_idx = this->GetEntryIndex(key, hash);
        if (exist_idx != TERMINATE_VALUE)
        {
          PostAdd(exist_idx, false);
          return false;
        }

        IdxType new_idx = this->GetAvaiableIndex();

        this->SetKey(new_idx, key);
        this->entrys[new_idx].value = value;
        this->entrys[new_idx].nextIdx = this->buckets[hash];

        this->buckets[hash] = new_idx;

        PostAdd(new_idx, true);

        return true;
      }

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
      template<typename TKey, typename TValue>
      TValue HashTableBase<TKey, TValue>::GetValue(TKey key, bool* isSuccess) const
      {
        IdxType idx = this->GetEntryIndex(key, this->GetHash(key));

        bool exist = (idx != TERMINATE_VALUE);

        if (isSuccess != NULL)
        {
          *isSuccess = exist;
        }

        if (exist == false)
        {
          GFL_ASSERT_MSG(isSuccess != NULL, "Key NotFound [key = %s]\n", key);

          return NULL;
        }

        return this->entrys[idx].value;
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
      template<typename TKey, typename TValue>
      typename HashTableBase<TKey, TValue>::IdxType HashTableBase<TKey, TValue>::GetEntryIndex(TKey key, IdxType hash) const
      {
        IdxType idx = this->buckets[hash];

        while(idx != TERMINATE_VALUE)
        {
          if (KeyDependsMethods<TKey>::KeyEqual(this->GetKey(idx), key))
          {
            return idx;
          }

          Entry entry = this->entrys[idx];
          idx = entry.nextIdx;
        }

        return TERMINATE_VALUE;
      }

      //--------------------------------------------------
      /**
      * @brief	キーからハッシュ値(エントリ数依存)を取得する
      *
      * @param	key		キー
      * @return			ハッシュ値
      */
      //--------------------------------------------------
      template<typename TKey, typename TValue>
      typename HashTableBase<TKey, TValue>::IdxType HashTableBase<TKey, TValue>::GetHash(TKey key) const
      {
        return KeyDependsMethods<TKey>::GetRowHash(key) % this->capacity;
      }

      //---------------------------------------
      // Iteratorの実装
      //---------------------------------------
      // コンストラクタ
      template<typename TKey, typename TValue>
      HashTableBase<TKey, TValue>::Iterator::Iterator(HashTableBase* pSrc)
        :pSrcTable(pSrc), currentBucketIdx(0), currentEntryIdx(TERMINATE_VALUE)
      {}

      // イテレータを先頭に戻す
      template<typename TKey, typename TValue>
      void HashTableBase<TKey, TValue>::Iterator::Reset()
      {
        this->currentBucketIdx = 0;
        this->currentEntryIdx = TERMINATE_VALUE;
      }

      // 次の要素を取得する
      template<typename TKey, typename TValue>
      bool HashTableBase<TKey, TValue>::Iterator::Next(TKey* outKey, TValue* outValue)
      {
        while (this->currentEntryIdx == TERMINATE_VALUE)
        {
          if (this->currentBucketIdx >= this->pSrcTable->capacity)
          {
            return false;
          }

          this->currentEntryIdx = this->pSrcTable->buckets[this->currentBucketIdx];
          ++this->currentBucketIdx;
        }

        (*outKey)   = this->pSrcTable->GetKey(this->currentEntryIdx);
        (*outValue) = this->pSrcTable->entrys[this->currentEntryIdx].value;

        this->currentEntryIdx = this->pSrcTable->entrys[this->currentEntryIdx].nextIdx;

        return true;
      }

      //---------------------------------------
      // 特殊化
      //---------------------------------------

      template<typename TKey, typename TValue>
      template<typename TDumy>
      class HashTableBase<TKey, TValue>::KeyDependsMethods<const char*, TDumy>
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
        static u32 GetRowHash(const char* key)
        {
          return CalcHash(key);
        }



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
        static bool KeyEqual(const char* a, const char* b)
        {
          return strcmp(a, b) == 0;
        }
      private:
        //--------------------------------------------------
        /**
        * @brief	文字列からハッシュ値を計算
        *
        * @param	str 対象文字列
        *
        * @return	  ハッシュ値
        */
        //--------------------------------------------------
        static u32 CalcHash(const char* str)
        {
          u32 value = 0;
          while(*str) {
            value *= HASH_MAGIC_NUMBER;
            value ^= static_cast<u32> (*str++);
          }
          return value;
        }

        static const u32 HASH_MAGIC_NUMBER = 0x83;

      };

      template class HashTableBase<const char*, int>;
      template class HashTableBase<const char*, AddOnlyHashTable<const char*, int>* >;


    } // DynamicHeader
  } // debug
} // gfl
