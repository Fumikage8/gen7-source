//=============================================================================
/**
* @brief  削除も可能なハッシュテーブル
* @file   gfl2_DebugRemovableHashTable.cpp
* @author Kanamaru Masanori
* @date   2015.2.20
*/
//=============================================================================

#include <debug/include/DynamicHeader/internal/gfl2_DebugDynamicHeaderDefine.h>

#include <debug/include/DynamicHeader/internal/gfl2_DebugRemovableHashTable.h>

// 明示的特殊化のために仕方なく...
#include <debug/include/DynamicHeader/internal/gfl2_DebugAddOnlyHashTable.h>

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
      RemovableHashTable<TKey, TValue>::RemovableHashTable(typename HashTableBase<TKey, TValue>::IdxType capacity, gfl2::heap::HeapBase* pHeap)
        :HashTableBase<TKey, TValue>(capacity, pHeap)
      {
        usedInfo = GFL_NEW_ARRAY(pHeap) bool[capacity];
        referenceCount = GFL_NEW_ARRAY(pHeap) char[capacity];
        for (int i = 0; i < capacity; ++i)
        {
          usedInfo[i] = false;
          referenceCount[i] = 0;
        }
        keys = GFL_NEW(pHeap) KeyContainer<TKey>(capacity, pHeap);
      }

      //--------------------------------------------------
      /**
      * @brief	デストラクタ
      */
      //--------------------------------------------------
      template<typename TKey, typename TValue>
      RemovableHashTable<TKey, TValue>::~RemovableHashTable()
      {
        GFL_DELETE_ARRAY this->usedInfo;
        GFL_DELETE_ARRAY this->referenceCount;
        GFL_DELETE keys;
      }

      //--------------------------------------------------
      /**
      * @brief	使用されていないインデックスを取得する
      *
      * @return	使用可能なentrysのインデックス
      */
      //--------------------------------------------------
      template<typename TKey, typename TValue>
      typename HashTableBase<TKey, TValue>::IdxType RemovableHashTable<TKey, TValue>::GetAvaiableIndex()
      {
        for(int i = 0; i < this->capacity; ++i)
        {
          if (this->usedInfo[i] == false)
          {
            this->usedInfo[i] = true;
            return i;
          }
        }

        GFL_ASSERT_MSG(false, "HeaderHashManager Assert : Over Capacity.\n");

        return 0;
      }


      //--------------------------------------------------
      /**
      * @brief	追加処理後に呼び出される
      *          (追加成功時のみでなく、キー重複時にも呼ばれる)
      *
      * @param	entryIndex	追加された(重複していた)エントリーのindex
      * @param	isSuccess	追加処理が成功したかどうか
      */
      //--------------------------------------------------
      template<typename TKey, typename TValue>
      void RemovableHashTable<TKey, TValue>::PostAdd(typename HashTableBase<TKey,TValue>::IdxType entryIndex, bool isSuccess)
      {
        ++this->referenceCount[entryIndex];
      }

      //--------------------------------------------------
      /**
      * @brief	指定したキーのレコードの削除を試みる
      *			参照レコードを減算し、0になった時点で削除する
      *
      * @param	key		対象のキー
      * @return			テーブルから削除した値
      */
      //--------------------------------------------------
      template<typename TKey, typename TValue>
      TValue RemovableHashTable<TKey, TValue>::Remove(TKey key)
      {
        typename HashTableBase<TKey,TValue>::IdxType hash = this->GetHash(key);

        typename HashTableBase<TKey,TValue>::IdxType targetIdx = this->GetEntryIndex(key, hash);

        //参照カウンタのデクリメント
        --this->referenceCount[targetIdx];
        if (this->referenceCount[targetIdx] > 0)
        {
          return NULL;
        }

        // エントリーの削除処理
        typename HashTableBase<TKey,TValue>::Entry* pTarget =&(this->entrys[targetIdx]);

        typename HashTableBase<TKey,TValue>::IdxType* pIdx = &(this->buckets[hash]);
        while ((*pIdx) != targetIdx)
        {
          pIdx = &(this->entrys[(*pIdx)].nextIdx);
        }

        (*pIdx) = pTarget->nextIdx;

        this->usedInfo[targetIdx] = false;

        this->SetKey(targetIdx, NULL);

        return pTarget->value;
      }

      //---------------------------------------
      // 特殊化
      //---------------------------------------

      // キーの実体を確保しない実装
      template<typename TKey, typename TValue>
      template<typename TDumy>
      class RemovableHashTable<TKey, TValue>::KeyContainer<const char*, TDumy>
      {
        typename HashTableBase<TKey, TValue>::IdxType capacity;
        const char **entrys;

      public:
        KeyContainer(typename HashTableBase<TKey, TValue>::IdxType capacity, gfl2::heap::HeapBase* pHeap)
          :capacity(capacity)
        {
          entrys = GFL_NEW(pHeap) const char*[capacity];
        }

        ~KeyContainer()
        {
          GFL_DELETE entrys;
        }

        const char* GetKey(typename HashTableBase<TKey, TValue>::IdxType index) const
        {
          return this->entrys[index];
        }


        void SetKey(typename HashTableBase<TKey, TValue>::IdxType index, const char* key)
        {
          this->entrys[index] = key;
        }

      };

      /*
      // キーの実体を確保する実装
      template<typename TKey, typename TValue>
      template<typename TDumy>
      class RemovableHashTable<TKey, TValue>::KeyContainer<const char*, TDumy>
      {
      typename HashTableBase<TKey, TValue>::IdxType capacity;
      char (*entrys)[KeyMaxLength+1];

      public:
      KeyContainer(typename HashTableBase<TKey, TValue>::IdxType capacity, gfl2::heap::HeapBase* pHeap)
      :capacity(capacity)
      {
      entrys = GFL_NEW(pHeap) char[capacity][KeyMaxLength+1];
      }

      ~KeyContainer()
      {
      GFL_DELETE this->entrys;
      }

      const char* GetKey(typename HashTableBase<TKey, TValue>::IdxType index) const
      {
      return this->entrys[index];
      }


      void SetKey(typename HashTableBase<TKey, TValue>::IdxType index, const char* key)
      {
      if (key == NULL)
      {
      return;
      }

      memcpy(this->entrys[index], key, strlen(key) + 1);
      }
      };
      */

      template class RemovableHashTable<const char*, AddOnlyHashTable<const char*, int>* >;

    } // DynamicHeader
  } //debug
} // gfl

