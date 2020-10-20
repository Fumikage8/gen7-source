//=============================================================================
/**
* @brief  追加のみ可能なハッシュテーブル実装
* @file   gfl2_DebugAddOnlyHashTable.h
* @author Kanamaru Masanori
* @date   2015.2.20
*/
//=============================================================================

#ifndef __GFL_DEBUG_ADDONLYHASHTABLE_H__
#define __GFL_DEBUG_ADDONLYHASHTABLE_H__
#pragma once

#include <debug/include/DynamicHeader/internal/gfl2_DebugHashTableBase.h>

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {
      //--------------------------------------------------
      /**
      * @class	AddOnlyHashTable
      * @brief	追加のみ可能なハッシュテーブル
      *
      * @tparam TKey		キーの型
      * @tparam TValue	値の型
      */
      //--------------------------------------------------
      template<typename TKey, typename TValue>
      class AddOnlyHashTable : public HashTableBase<TKey, TValue>
      {
        typename HashTableBase<TKey, TValue>::IdxType nextEntryIdx;
      public:
        //--------------------------------------------------
        /**
        * @class KeyContainer
        *
        *			キーの型に依存した部分特殊化のためにクラス化
        */
        //--------------------------------------------------
        template<typename TDKey, typename TDumy = void>
        class KeyContainer				
        {

        public:
          KeyContainer(typename HashTableBase<TKey, TValue>::IdxType capacity, gfl2::heap::HeapBase* pheap);

          ~KeyContainer();

          TDKey GetKey(typename HashTableBase<TKey, TValue>::IdxType index) const;

          void SetKey(typename HashTableBase<TKey, TValue>::IdxType index, TDKey key);
        };

        AddOnlyHashTable(typename HashTableBase<TKey, TValue>::IdxType capacity, gfl2::heap::HeapBase* pHeap);
        virtual ~AddOnlyHashTable();

      protected:
        // @memo 値型だとテンプレート特殊化位置の問題でARMCCを通らないのでポインタで実装
        KeyContainer<TKey>* keys;

        virtual typename HashTableBase<TKey, TValue>::IdxType GetAvaiableIndex();

        virtual TKey GetKey(typename HashTableBase<TKey,TValue>::IdxType index) const {return this->keys->GetKey(index);}
        virtual void SetKey(typename HashTableBase<TKey,TValue>::IdxType index, TKey key) {this->keys->SetKey(index, key);}

      };

    } // DynamicHeader
  } // debug
} // gfl

#endif
