#ifndef __GFL_FIXEDLISTSENTINEL_H__
#define __GFL_FIXEDLISTSENTINEL_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FixedListSentinel.h
 *	@brief  固定サイズのリストのクラス(番兵sentinel使用版)
 *	@author	Koji Kawada
 *	@date		2012.02.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


//用語や解説はgfl_FixedList.hに書いてある。

// CTR_SDK

// NW4C

// gflib
#include <gfl_Macros.h>
#include <gfl_Debug.h>
#include <base/gfl_FixedList.h>


namespace gfl {
namespace base {


///////////////////////////////////
//
// 構造体宣言
//
///////////////////////////////////
// 固定サイズのリストの要素
// template <typename T> struct FixedListElement
// はgfl_FixedList.hで定義してある。


///////////////////////////////////
//
// クラス宣言
//
///////////////////////////////////
// 固定サイズのリストの配列
template <typename T, typename K> class FixedListArraySentinel
{
  GFL_FORBID_COPY_AND_ASSIGN(FixedListArraySentinel);

public:
  // 関数ポインタ定義
  // コンストラクタで最初に1回だけ呼ばれる生成関数
  typedef void (*CreateDataFunction)(T* a, void* work);
  // デストラクタで最後に1回だけ呼ばれる破棄関数
  typedef void (*DestroyDataFunction)(T* a, void* work);
  // データをデフォルト初期化する
  typedef void (*InitializeDefaultDataFunction)(T* a, void* work);         
  // データを比較する
  // aに含まれるKとbを比較する
  // 同じとき0を返す。aのほうが大きいとき正の値を返す。aのほうが小さいとき負の値を返す
  typedef s32  (*CompareDataFunction)(const T* a, const K b, void* work); 
  // データをコピーする
  // srcをdstにコピーする 
  typedef void (*CopyDataFunction)(T* dst, const T* src, void* work);      

public:
  // コンストラクタ
  FixedListArraySentinel(
      gfl::heap::HeapBase*  heap_memory,
      u32                   max_element_num,
      CreateDataFunction             create_data_function,
      DestroyDataFunction            destroy_data_function,
      InitializeDefaultDataFunction  initialzie_default_data_function,
      CompareDataFunction            compare_data_function,
      CopyDataFunction               copy_data_function,
      void*                          function_work  // このクラスで保持されるので、このクラスが生存中は呼び出し元で破棄しないように。
  )
    : m_max_element_num(max_element_num),
      m_element_num(0),
      m_element_array(NULL),
      m_element_exist(NULL),
      m_create_data_function(create_data_function),
      m_destroy_data_function(destroy_data_function),
      m_initialzie_default_data_function(initialzie_default_data_function),
      m_compare_data_function(compare_data_function),
      m_copy_data_function(copy_data_function),
      m_function_work(function_work)
  {
    GFL_ASSERT_STOP( m_max_element_num > 0 );

    GFL_ASSERT_STOP(m_create_data_function);
    GFL_ASSERT_STOP(m_destroy_data_function);
    GFL_ASSERT_STOP(m_initialzie_default_data_function);
    GFL_ASSERT_STOP(m_compare_data_function);
    GFL_ASSERT_STOP(m_copy_data_function);

    constructSentinel();

    m_element_array = GFL_NEW_ARRAY(heap_memory) FixedListElement<T>[m_max_element_num];
    for(u32 i=0; i<m_max_element_num; ++i)
    {
      m_create_data_function(&(m_element_array[i].data), m_function_work);
      // 使用前に必ず呼ばれるgetSpaceElement関数からm_initialzie_default_data_functionを呼び出しているので
      // ここではm_initialzie_default_data_functionを呼ぶ必要はない。
    }

    m_element_exist = GFL_NEW(heap_memory) gfl::base::Bit(heap_memory, m_max_element_num);
  }

  // デストラクタ
  virtual ~FixedListArraySentinel()
  {
    if( m_element_exist )
    {
      GFL_DELETE m_element_exist;
    }
    if( m_element_array )
    {
      for(u32 i=0; i<m_max_element_num; ++i)
      {
        m_destroy_data_function(&(m_element_array[i].data), m_function_work);
      }
      GFL_DELETE_ARRAY m_element_array;
    }
    destructSentinel();
  }

public:
  // 配列の確保してある要素数を取得する
  u32 GetMaxElementNum(void) const
  {
    return m_max_element_num;
  }

  // データが入っている個数を取得する
  u32 GetElementNum(void) const
  {
    return m_element_num;
  }

public:
  // 番兵要素を得る
  // 使っていいのは
  // ・この戻り値のポインタそのもの(ループの終了判定の比較式に使う)
  // ・next(先頭要素が得られる)
  // ・prev(末尾要素が得られる)
  // のどれか。
  const FixedListElement<T>* GetSentinelElement(void) const
  {
    // const FixedListElement<T>* GetSentinelElement(void) constはコンパイルできたが、
    // FixedListElement<T>* GetSentinelElement(void) constはコンパイルできなかった。
    return &m_sentinel;
  }
  
  // 先頭要素を取得する
  // 得られた要素がGetSentinelElementのときデータなし(NULLではないので注意)
  FixedListElement<T>* GetElementHead(void) const
  {
    return m_sentinel.next;
  }

  // 末尾要素を取得する
  // 得られた要素がGetSentinelElementのときデータなし(NULLではないので注意)
  FixedListElement<T>* GetElementTail(void) const
  {
    return m_sentinel.prev;
  }

  // データを探して、それが入っている要素を取得する
  // 同じkeyのデータが複数あるとき、前にあるものを取得する
  // NULLのときkeyのデータがない
  FixedListElement<T>* GetElement(const K key) const
  {
    FixedListElement<T>* target_element = NULL;
    FixedListElement<T>* curr_element = m_sentinel.next;
    while(curr_element != (&m_sentinel))
    {
      if( m_compare_data_function(&(curr_element->data), key, m_function_work) == 0 )
      {
        target_element = curr_element;
        break;
      }
      curr_element = curr_element->next;
    }
    return target_element;
  }

public:
  // 先頭に空データの要素を追加し、それを得る
  // データが入っている扱いになり、データが入っている個数も増える
  // NULLのとき追加できなかった
  FixedListElement<T>* AddGetSpaceElementHead(void)
  {
    u32 index = 0;
    FixedListElement<T>* space_element = getSpaceElement(&index);
    if(space_element)
    {
      space_element->prev = (&m_sentinel);
      space_element->next = m_sentinel.next;
      m_sentinel.next->prev = space_element;
      m_sentinel.next = space_element;
      m_element_exist->On(index);
      ++m_element_num;
    }
    return space_element;
  }

  // 末尾に空データの要素を追加し、それを得る
  // データが入っている扱いになり、データが入っている個数も増える
  // NULLのとき追加できなかった
  FixedListElement<T>* AddGetSpaceElementTail(void)
  {
    u32 index = 0;
    FixedListElement<T>* space_element = getSpaceElement(&index);
    if(space_element)
    {
      space_element->next = &(m_sentinel);
      space_element->prev = m_sentinel.prev;
      m_sentinel.prev->next = space_element;
      m_sentinel.prev = space_element;
      m_element_exist->On(index);
      ++m_element_num;
    }
    return space_element;
  }
  
  // 先頭にデータを追加する
  // aは与えられているコピー関数によってコピーされる
  // 追加できたときそのデータが追加された要素を返す
  // 追加できなかったときNULLを返す
  FixedListElement<T>* SetDataHead(const T* a)
  {
    FixedListElement<T>* space_element = AddGetSpaceElementHead();
    if( space_element )
    {
      m_copy_data_function(&(space_element->data), a, m_function_work);
    }
    return space_element;
  }
  
  // 末尾にデータを追加する
  // aは与えられているコピー関数によってコピーされる
  // 追加できたときそのデータが追加された要素を返す
  // 追加できなかったときNULLを返す
  FixedListElement<T>* SetDataTail(const T* a)
  {
    FixedListElement<T>* space_element = AddGetSpaceElementTail();
    if( space_element )
    {
      m_copy_data_function(&(space_element->data), a, m_function_work);
    }
    return space_element;
  }

public:
  // 要素を削除し、リストを繋ぎ直す
  // データが入っている個数も減り、データが入っていない扱いになる
  // 引数target_elementは与えられているデフォルト初期化関数で初期化される
  void RemoveElement(FixedListElement<T>* target_element)
  {
    target_element->prev->next = target_element->next;
    target_element->next->prev = target_element->prev;
    u32 index = getElementIndex(target_element);
    initializeDefaultElement(target_element);
    m_element_exist->Off(index);
    --m_element_num;
  }
  
  // データを探して削除する
  // 同じkeyのデータが複数あるとき、前にあるものを削除する
  void RemoveData(const K key)
  {
    FixedListElement<T>* target_element = GetElement(key);
    if( target_element )
    {
      RemoveElement(target_element);
    }
  }

  // データを探して、取得してから削除する
  // 同じkeyのデータが複数あるとき、前にあるものが選択される
  // aに取得したデータを入れて返す
  // 正常に取得できたときtrueを返す
  b32 GetRemoveData(T* a, const K key)
  {
    b32 ret = false;
    FixedListElement<T>* target_element = GetElement(key);
    if( target_element )
    {
      m_copy_data_function(a, &(target_element->data), m_function_work);
      RemoveElement(target_element);
      ret = true;
    }
    return ret;
  }

  // 先頭のデータを取得してから削除する
  // aに取得したデータを入れて返す
  // 正常に取得できたときtrueを返す
  b32 GetRemoveDataHead(T* a)
  {
    b32 ret = false;
    FixedListElement<T>* target_element = m_sentinel->next;
    if( target_element != (&m_sentinel) )
    {
      m_copy_data_function(a, &(target_element->data), m_function_work);
      RemoveElment(target_element); 
      ret = true;
    }
    return ret;
  }
  
  // 末尾のデータを取得してから削除する
  // aに取得したデータを入れて返す
  // 正常に取得できたときtrueを返す
  b32 GetRemoveDataTail(T* a)
  {
    b32 ret = false;
    FixedListElement<T>* target_element = m_sentinel->prev;
    if( target_element != (&m_sentinel) )
    {
      m_copy_data_function(a, &(target_element->data), m_function_work);
      RemoveElment(target_element); 
      ret = true;
    }
    return ret;
  }

private:
  // 空データの要素を得る
  // indexには空データの要素の配列インデックスを入れて返す
  // NULLのとき空データなしでindexは不定
  FixedListElement<T>* getSpaceElement(u32* index)
  {
    FixedListElement<T>* space_element = NULL;
    b32 ret = m_element_exist->GetOffBitMinIndex(index);
    if( ret )
    {
      space_element = &(m_element_array[*index]);
      initializeDefaultElement(space_element);
    }
    return space_element;
  }

  // 要素をデフォルト初期化する
  void initializeDefaultElement(FixedListElement<T>* target_element)
  {
    m_initialzie_default_data_function(&(target_element->data), m_function_work);
    target_element->prev = NULL;
    target_element->next = NULL;
  }

  // 要素の配列インデックスを得る
  u32 getElementIndex(FixedListElement<T>* target_element) const
  {
    u32 index = 0;

    // (target_element-m_element_array)で配列のインデックスの差が得られたが、どの環境でもそういうものなのか？
    // よく分からないので、uptrでアドレスを数字にして差を求めることにした。
    uptr target_address = reinterpret_cast<uptr>(target_element);
    uptr array_address  = reinterpret_cast<uptr>(m_element_array);

    GFL_ASSERT_MSG( ( target_element >= m_element_array && (target_address-array_address)%sizeof(FixedListElement<T>) == 0 ), /* //@check */ \
        "target_element=0x%p, m_element_array=0x%p, sa=%d, address_sa=%d, amari=%d, address_amari=%d, sizeof=%d\n", \
        target_element, m_element_array, (target_element-m_element_array), (target_address-array_address),
        (target_element-m_element_array)%sizeof(FixedListElement<T>), (target_address-array_address)%sizeof(FixedListElement<T>),
        sizeof(FixedListElement<T>) );
    
    index = (target_address-array_address)/sizeof(FixedListElement<T>);
    
    return index;
  }

  // コンストラクタから呼び出す番兵要素を生成する関数
  void constructSentinel()
  {
    m_create_data_function(&(m_sentinel.data), m_function_work);
    m_initialzie_default_data_function(&(m_sentinel.data), m_function_work);
    // m_sentinelに対してはgetSpaceElement関数が呼ばれないのでm_initialzie_default_data_functionをここで呼んでおく。
    m_sentinel.next = m_sentinel.prev = (&m_sentinel);
  }
  // デストラクタから呼び出す番兵要素を破棄する関数
  void destructSentinel(void)
  {
    m_destroy_data_function(&(m_sentinel.data), m_function_work);
  }

private:
  // メンバ変数
  u32                  m_max_element_num;  // m_element_array配列の確保してある要素数
  u32                  m_element_num;      // m_element_arrayの中でデータが入っている個数
  FixedListElement<T>  m_sentinel;         // 番兵要素。nextは先頭要素を指す。prevは末尾要素を指す。
                                           // データが1つも入っていないときはnext=prev=(&m_sentinel)となっている。
  FixedListElement<T>* m_element_array;    // データを入れる配列。配列の確保してある要素数はm_max_element_num
  gfl::base::Bit*      m_element_exist;    // m_element_array[i]にデータが入っているときはi番目のビットが1になる。
 
  CreateDataFunction             m_create_data_function;
  DestroyDataFunction            m_destroy_data_function;
  InitializeDefaultDataFunction  m_initialzie_default_data_function;
  CompareDataFunction            m_compare_data_function; 
  CopyDataFunction               m_copy_data_function;
  void*                          m_function_work;

};


}  // namespace base
}  // namespace gfl


#endif  // #ifndef __GFL_FIXEDLISTSENTINEL_H__

