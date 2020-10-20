//=============================================================================
/**
* @brief  追加のみ可能なハッシュテーブル
* @file   gfl2_DebugAddOnlyHashTable.cpp
* @author Kanamaru Masanori
* @date   2015.2.20
*/
//=============================================================================

#include <debug/include/DynamicHeader/internal/gfl2_DebugDynamicHeaderDefine.h>

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
      AddOnlyHashTable<TKey, TValue>::AddOnlyHashTable(typename HashTableBase<TKey, TValue>::IdxType capacity, gfl2::heap::HeapBase* pHeap)
        :HashTableBase<TKey, TValue>(capacity, pHeap),
        nextEntryIdx(0)
      {
        keys = GFL_NEW(pHeap) KeyContainer<TKey>(capacity, pHeap);
      }

      //--------------------------------------------------
      /**
      * @brief	デストラクタ
      */
      //--------------------------------------------------
      template<typename TKey, typename TValue>
      AddOnlyHashTable<TKey, TValue>::~AddOnlyHashTable()
      {
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
      typename HashTableBase<TKey, TValue>::IdxType AddOnlyHashTable<TKey, TValue>::GetAvaiableIndex()
      {
        GFL_ASSERT_MSG(this->nextEntryIdx < this->capacity, "HeaderHashManager Assert : Over Capacity.\n");

        typename HashTableBase<TKey, TValue>::IdxType res = this->nextEntryIdx;

        ++this->nextEntryIdx;

        return res;
      }

      //---------------------------------------
      // 特殊化
      //---------------------------------------

      // 最大長 * キャパシティ分のメモリを確保する実装
      template<typename TKey, typename TValue>
      template<typename TDumy>
      class AddOnlyHashTable<TKey, TValue>::KeyContainer<const char*, TDumy>
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
          GFL_DELETE[] this->entrys;
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


      template class AddOnlyHashTable<const char*, int>;

    } // DynamicHeader
  } // debug
} // gfl
