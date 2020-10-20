//=============================================================================
/**
* @brief  汎用スタック
* @file   gfl2_DebugFixedStack.h
* @author Kanamaru Masanori
* @date   2015.2.18
*/
//=============================================================================

#ifndef __GFL_DEBUG_FIXEDSTACK_H__
#define __GFL_DEBUG_FIXEDSTACK_H__
#pragma once

#include <types/include/gfl2_Typedef.h>

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {

      //--------------------------------------------------
      /**
      * @class FixedStack
      * @brief 固定長スタック
      *
      * @tparam T			要素の型
      * @tparam TCapacity	格納可能な要素の数
      */
      //--------------------------------------------------
      template<typename T, size_t TCapacity>
      class FixedStack
      {
        T buffer[TCapacity];
        s32 current;
      public:

        //----------------------------------------
        /**
        * @brief	コンストラクタ
        */
        //----------------------------------------			
        FixedStack()
          :buffer()
        {
          current = -1;
        }

        //----------------------------------------
        /**
        * @brief	スタックが満杯かチェックする
        */
        //----------------------------------------			
        bool IsFull() const
        {
          return this->current >= static_cast<s32>(TCapacity) - 1;
        }

        //----------------------------------------
        /**
        * @brief	スタックが空かチェックする
        */
        //----------------------------------------
        bool IsEmpty() const
        {
          return this->current < 0;
        }

        //----------------------------------------
        /**
        * @brief	要素の追加
        *
        * @param	item		追加する要素
        *
        * @retval	true		要素の追加に成功
        * @retval	false		要素の追加に失敗(キャパシティオーバ)
        */
        //----------------------------------------			
        bool Push(T item)
        {
          if (this->IsFull())
          {
            return false;
          }

          ++this->current;
          this->buffer[this->current] = item;

          return true;
        }

        //----------------------------------------
        /**
        * @brief	要素の取得
        *
        * @param	out		取得した値を格納するポインタ
        *
        * @retval	true	取得成功
        * @retval	false	取得失敗(要素を1つも格納していない)
        */
        //----------------------------------------			
        bool Peek(T* out) const
        {
          if (this->IsEmpty())
          {
            return false;
          }

          (*out) = this->buffer[this->current];

          return true;
        }


        //----------------------------------------
        /**
        * @brief	要素の取得と削除
        *
        * @param	out		取得した値を格納するポインタ
        *
        * @retval	true	取得成功
        * @retval	false	取得失敗(要素を1つも格納していない)
        */
        //----------------------------------------			
        bool Pop(T* out)
        {
          bool res = Peek(out);

          if (res)
          {
            --this->current;
          }

          return true;
        }

        //----------------------------------------
        /**
        * @brief	スタックを空にする
        */
        //----------------------------------------			
        void Clear()
        {
          this->current = -1;
        }



      };
    }
  } // base
} // gfl

#endif
