//=============================================================================
/**
* @brief  要素の削除が可能なハッシュテーブル実装
* @file   gfl2_DebugAddOnlyHashTable.h
* @author Kanamaru Masanori
* @date   2015.2.20
*/
//=============================================================================

#ifndef __GFL_DEBUG_REMOVABLEHASHTABLE_H__
#define __GFL_DEBUG_REMOVABLEHASHTABLE_H__
#pragma once

#include <debug/include/DynamicHeader/internal/gfl2_DebugHashTableBase.h>

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {
      //--------------------------------------------------
      /**
      * @class	RemovableHashTable
      * @brief	削除も可能なハッシュテーブル
      *			各エントリーに参照カウンタを用意し、
      *			同一キーでAddする毎に増加する
      *			Remove時はこの参照カウンタを減産し、0になった時点で初めて削除する
      *
      * @tparam TKey		キーの型
      * @tparam TValue	値の型
      */
      //--------------------------------------------------
      template<typename TKey, typename TValue>
      class RemovableHashTable : public HashTableBase<TKey, TValue>
      {
        bool* usedInfo;
        char* referenceCount;

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

        RemovableHashTable(typename HashTableBase<TKey, TValue>::IdxType capacity, gfl2::heap::HeapBase* pHeap);
        virtual ~RemovableHashTable();

        //--------------------------------------------------
        /**
        * @brief	指定したキーのレコードの削除を試みる
        *			参照レコードを減算し、0になった時点で削除する
        *
        * @param	key		対象のキー
        * @return			テーブルから削除した値
        */
        //--------------------------------------------------
        TValue Remove(TKey key);

      protected:
        KeyContainer<TKey>* keys;

        virtual typename HashTableBase<TKey, TValue>::IdxType GetAvaiableIndex();
        virtual void PostAdd(typename HashTableBase<TKey,TValue>::IdxType entryIndex, bool isSuccess);

        virtual TKey GetKey(typename HashTableBase<TKey,TValue>::IdxType index) const {return this->keys->GetKey(index);}
        virtual void SetKey(typename HashTableBase<TKey,TValue>::IdxType index, TKey key) {this->keys->SetKey(index, key);}
      };


    } // DynamicHeader
  } // debug
} // gfl

#endif
