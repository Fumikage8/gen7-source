#ifndef __GFL_FIXEDLIST_H__
#define __GFL_FIXEDLIST_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FixedList.h
 *	@brief  固定サイズのリストのクラス
 *	@author	Koji Kawada
 *	@date		2012.02.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


/*

用語
Element, element :  FixedListElementを扱うとき。要素。リストの要素。
Data, data       :  FixedListElementのメンバであるdataを扱うとき。データ。

解説
速度重視なので「FixedListArrayとFixedListArraySentinelの共通項を抜き出した基底クラスをつくり、
その基底クラスを継承してvirtual関数をオーバーライドする」ということはしていません。

C++関数呼び出しの速度
呼出し 秒 
Static Call 2.757345 
Member Call 2.759990 
Virtual Call 9.552945 
Pointer Member Call 2.719109 

*/


// CTR_SDK

// NW4C

// gflib
#include <gfl_Macros.h>
#include <gfl_Debug.h>


namespace gfl {
namespace base {


///////////////////////////////////
//
// 構造体宣言
//
///////////////////////////////////
// 固定サイズのリストの要素
template <typename T> struct FixedListElement
{
  T                    data;  // データ
  FixedListElement<T>* prev;  // 前の要素へのポインタ。先頭要素のときはNULL。番兵版の際は、先頭要素のときは番兵を指す。
  FixedListElement<T>* next;  // 次の要素へのポインタ。末尾要素のときはNULL。番兵版の際は、先頭要素のときは番兵を指す。
};


///////////////////////////////////
//
// クラス宣言
//
///////////////////////////////////
// 固定サイズのリストの配列
template <typename T, typename K> class FixedListArray
{
  GFL_FORBID_COPY_AND_ASSIGN(FixedListArray);

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
  FixedListArray(
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
      m_head(NULL),
      m_tail(NULL),
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
  virtual ~FixedListArray()
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
  // 先頭要素を取得する
  FixedListElement<T>* GetElementHead(void) const
  {
    return m_head;
  }

  // 末尾要素を取得する
  FixedListElement<T>* GetElementTail(void) const
  {
    return m_tail;
  }

  // データを探して、それが入っている要素を取得する
  // 同じkeyのデータが複数あるとき、前にあるものを取得する
  // NULLのときkeyのデータがない
  FixedListElement<T>* GetElement(const K key) const
  {
    FixedListElement<T>* curr_element = m_head;
    while(curr_element)
    {
      if( m_compare_data_function(&(curr_element->data), key, m_function_work) == 0 )
      {
        break;
      }
      curr_element = curr_element->next;
    }
    return curr_element;
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
      if( m_head )
      {
        // m_headがあるので、データが1つはある
        space_element->next = m_head;
        m_head->prev = space_element;
      }
      else
      {
        // データが1つもないとき
        m_tail = space_element;
      }
      m_head = space_element;
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
      if( m_head )
      {
        // m_headがあるので、データが1つはある
        space_element->prev = m_tail;
        m_tail->next = space_element;
      }
      else
      {
        // データが1つもないとき
        m_head = space_element;
      }
      m_tail = space_element;
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
    if( m_head == target_element )
    {
      m_head = target_element->next;
    }
    else  // if( target_element->prev ) に同じ
    {
      target_element->prev->next = target_element->next;
    }

    if( m_tail == target_element )
    {
      m_tail = target_element->prev;
    }
    else  // if( target_element->next ) に同じ
    {
      target_element->next->prev = target_element->prev;
    }

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
    if( m_head )
    {
      m_copy_data_function(a, &(m_head->data), m_function_work);
      RemoveElment(m_head); 
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
    if( m_tail )
    {
      m_copy_data_function(a, &(m_head->data), m_function_work);
      RemoveElement(m_tail); 
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

private:
  // メンバ変数
  u32                  m_max_element_num;  // m_element_array配列の確保してある要素数
  u32                  m_element_num;      // m_element_arrayの中でデータが入っている個数
  FixedListElement<T>* m_head;             // 先頭要素へのポインタ。データが1つも入っていないときはNULL。
  FixedListElement<T>* m_tail;             // 末尾要素へのポインタ。データが1つも入っていないときはNULL。
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


#endif  // #ifndef __GFL_FIXEDLIST_H__




#if 0

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//＜＜＜＜使用例＞＞＞＞

  ////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////
  //.h

private:
  struct TestData3Byte
  {
    s8  bbb;
    s8  ccc[2];
    TestData3Byte(void)
      : bbb(2)
    {
      ccc[0] = 3;
      ccc[1] = 4;
    }
  };
  struct TestData4Byte
  {
    s16 aaa;
    s8  padding[2];
    TestData4Byte(void)
      : aaa(1)
    {}
  };
  typedef s32* s32ptr;
  struct TestData9Byte : public TestData4Byte
  {
    //s32* ppp;  // これではダメ
    s32ptr ppp;
    s8   padding;
    TestData9Byte(void)
      : TestData4Byte(),
        ppp(NULL)
    {}
  };

  static const u32 LIST_ARRAY_NUM_MAX = 30;

#if 0
  typedef FixedListElement<TestData4Byte> ListElementTestData;
  typedef FixedListArray<TestData4Byte, s16> ListArrayTestData;
  static void FixedListInitializeDefaultDataFunction(TestData4Byte* a)
  {
    GFL_PRINT("init\n");
    a->aaa = 101;
  }
  static s32  FixedListCompareDataFunction(const TestData4Byte* a, const s16 b)
  {
    GFL_PRINT("comp\n");
    if( a->aaa == b )     return 0;
    else if( a->aaa > b ) return 1;
    else                  return -1;
  }
  static void FixedListCopyDataFunction(TestData4Byte* dst, const TestData4Byte* src)
  {
    GFL_PRINT("copy\n");
    dst->aaa = src->aaa;
  }
#elif 0
  typedef FixedListElement<TestData3Byte> ListElementTestData;
  typedef FixedListArray<TestData3Byte, s8> ListArrayTestData;
  static void FixedListInitializeDefaultDataFunction(TestData3Byte* a)
  {
    GFL_PRINT("init\n");
    a->bbb = 102;
  }
  static s32  FixedListCompareDataFunction(const TestData3Byte* a, const s8 b)
  {
    GFL_PRINT("comp\n");
    if( a->bbb == b )     return 0;
    else if( a->bbb > b ) return 1;
    else                  return -1;
  }
  static void FixedListCopyDataFunction(TestData3Byte* dst, const TestData3Byte* src)
  {
    GFL_PRINT("copy\n");
    dst->bbb = src->bbb;
  }
#elif 1
  typedef FixedListElement<TestData9Byte> ListElementTestData;
  //typedef FixedListArray<TestData9Byte, s32*> ListArrayTestData;  // これではダメ
  typedef FixedListArray<TestData9Byte, s32ptr> ListArrayTestData;
  static void FixedListInitializeDefaultDataFunction(TestData9Byte* a)
  {
    GFL_PRINT("init\n");
    a->aaa = 101;
  }
  //static s32  FixedListCompareDataFunction(const TestData9Byte* a, const s32* b)  // これではダメ
  static s32  FixedListCompareDataFunction(const TestData9Byte* a, const s32ptr b)
  {
    GFL_PRINT("comp\n");
    if( a->ppp == 0 )     return 0;
    else if( a->ppp > 0 ) return 1;
    else                  return -1;
  }
  static void FixedListCopyDataFunction(TestData9Byte* dst, const TestData9Byte* src)
  {
    GFL_PRINT("copy\n");
    dst->aaa = src->aaa;
  }
#endif


private:
  ListArrayTestData* m_list_array_test;

  ////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////
 
  ////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////
  //.cpp

  {
    m_list_array_test = GFL_NEW(l_heap_memory) ListArrayTestData(
        l_heap_memory,
        LIST_ARRAY_NUM_MAX,
        FixedListInitializeDefaultDataFunction,
        FixedListCompareDataFunction,
        FixedListCopyDataFunction
    );

    u32 count = 0;
    ListElementTestData* curr = NULL;

#if 0
    curr = m_list_array_test->AddGetSpaceElementHead();
    curr->data.aaa = 200;
    curr = m_list_array_test->AddGetSpaceElementHead();
    curr->data.aaa = 300;
    curr = m_list_array_test->AddGetSpaceElementHead();
    curr->data.aaa = 400;
    curr = m_list_array_test->AddGetSpaceElementTail();
    curr->data.aaa = 555;
    {
      TestData4Byte b;
      b.aaa = 654;
      m_list_array_test->SetDataHead(&b);
      b.aaa = 765;
      m_list_array_test->SetDataTail(&b);
    }
    {
      s16 c;
      c = 400;
      m_list_array_test->RemoveData(c);
    }
    
    curr = m_list_array_test->GetElementHead();
    while(curr != NULL)
    {
      GFL_PRINT("[%d] aaa=%d\n", count, curr->data.aaa);
      count++;
      curr = curr->next;
    }
#elif 0
    curr = m_list_array_test->AddGetSpaceElementHead();
    curr->data.bbb = 20;
    curr = m_list_array_test->AddGetSpaceElementHead();
    curr->data.bbb = 30;
    curr = m_list_array_test->AddGetSpaceElementHead();
    curr->data.bbb = 40;
    curr = m_list_array_test->AddGetSpaceElementTail();
    curr->data.bbb = 55;
    {
      TestData3Byte b;
      b.bbb = 65;
      m_list_array_test->SetDataHead(&b);
      b.bbb = 76;
      m_list_array_test->SetDataTail(&b);
    }
    {
      s8 c;
      c = 40;
      m_list_array_test->RemoveData(c);
    }
    
    curr = m_list_array_test->GetElementHead();
    while(curr != NULL)
    {
      GFL_PRINT("[%d] bbb=%d\n", count, curr->data.bbb);
      count++;
      curr = curr->next;
    }

#elif 1
    curr = m_list_array_test->AddGetSpaceElementHead();
    curr->data.aaa = 200;
    curr = m_list_array_test->AddGetSpaceElementHead();
    curr->data.aaa = 300;
    curr = m_list_array_test->AddGetSpaceElementHead();
    curr->data.aaa = 400;
    curr = m_list_array_test->AddGetSpaceElementTail();
    curr->data.aaa = 555;
    {
      TestData9Byte b;
      b.aaa = 654;
      m_list_array_test->SetDataHead(&b);
      b.aaa = 765;
      m_list_array_test->SetDataTail(&b);
    }
    {
      s32* c;
      c = 0;
      m_list_array_test->RemoveData(c);
    }
    
    curr = m_list_array_test->GetElementHead();
    while(curr != NULL)
    {
      GFL_PRINT("[%d] aaa=%d\n", count, curr->data.aaa);
      count++;
      curr = curr->next;
    }
#endif

    GFL_PRINT("num=%d, %d\n", count, m_list_array_test->GetElementNum());

    GFL_DELETE m_list_array_test;
  }
  ////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
＜＜＜＜templateのtypenameにポインタを渡すメモ＞＞＞＞

  typedef s32* s32ptr;
  struct TestData9Byte : public TestData4Byte
  {
    //s32* ppp;  // これではダメ
    s32ptr ppp;
    s8   padding;
    TestData9Byte(void)
      : TestData4Byte(),
        ppp(NULL)
    {}
  };

  typedef FixedListElement<TestData9Byte> ListElementTestData;
  //typedef FixedListArray<TestData9Byte, s32*> ListArrayTestData;  // これではダメ
  typedef FixedListArray<TestData9Byte, s32ptr> ListArrayTestData;
  static void FixedListInitializeDefaultDataFunction(TestData9Byte* a)
  {
    GFL_PRINT("init\n");
    a->aaa = 101;
  }
  //static s32  FixedListCompareDataFunction(const TestData9Byte* a, const s32* b)  // これではダメ
  static s32  FixedListCompareDataFunction(const TestData9Byte* a, const s32ptr b)
  {
    GFL_PRINT("comp\n");
    if( a->ppp == 0 )     return 0;
    else if( a->ppp > 0 ) return 1;
    else                  return -1;
  }

http://www.ced.is.utsunomiya-u.ac.jp/lecture/2010/prog/p2/kadai2/typedef.php
typedefの利用
typedefの利用法　２　－ポインタの作成を容易にする


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
＜＜＜＜配列のサイズのメモ＞＞＞＞

  struct TestData4Byte
  {
    s16 aaa;
    s8  padding[2];
    TestData4Byte(void)
      : aaa(1)
    {}
  };
  typedef s32* s32ptr;
  struct TestData9Byte : public TestData4Byte
  {
    //s32* ppp;  // これではダメ
    s32ptr ppp;
    s8   padding;
    TestData9Byte(void)
      : TestData4Byte(),
        ppp(NULL)
    {}
  };

    GFL_PRINT("T size=%d\n", sizeof(FixedListElement<T>));
    GFL_PRINT("0 size=%d\n", sizeof(m_element_array[0]));

サイズはどちらも20バイトだった。
つまり、sizeof(FixedListElement<T>)で、きちんとアライメントされた配列の1要素のサイズが取れる。

http://www.g-ishihara.com/c_st_01.htm
構造体のサイズとアライメント
sizeof演算子で返ってくるサイズは、アライメントが反映されているので、常に正しいと言えるでしょう。


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
＜＜＜＜関数ポインタの書き方メモ＞＞＞＞

[パターンその１]
typedefのtemplateはできないので、この書き方はエラー。

///////////////////////////////////
//
// 型宣言
//
///////////////////////////////////
// データをデフォルト初期化する
typedef template <typename T> void (*FixedListInitializeDefaultDataFunction)(T* a);

// データを比較する
// aに含まれるKとbを比較する
// 同じとき0を返す。aのほうが大きいとき正の値を返す。aのほうが小さいとき負の値を返す
typedef template <typename T, typename K> s32 (*FixedListCompareDataFunction)(const T* a, const K* b);

// データをコピーする
// srcをdstにコピーする
typedef template <typename T> void (*FixedListCopyDataFunction)(T* dst, const T* src);


///////////////////////////////////
//
// クラス宣言
//
///////////////////////////////////
// 固定サイズのリストの配列
template <typename T, typename K> class FixedListArray
{
  GFL_FORBID_COPY_AND_ASSIGN(FixedListArray);

public:
  // [パターンその４]
  // 関数ポインタ定義
  typedef void (*InitializeDefaultDataFunction)(T* a);         
  typedef s32  (*CompareDataFunction)(const T* a, const K* b); 
  typedef void (*CopyDataFunction)(T* dst, const T* src);      

private:
  // メンバ変数
  u32               m_max_data_num;  // m_data_arrayの配列のサイズ
  u32               m_data_num;      // m_data_arrayの中でデータが入っている個数
  FixedListData<T>* m_head;          // 先頭データへのポインタ。データが1つも入っていないときはNULL。
  FixedListData<T>* m_tail;          // 末尾データへのポインタ。データが1つも入っていないときはNULL。
  FixedListData<T>* m_data_array;    // データを入れる配列。配列のサイズはm_max_data_num
  gfl::base::Bit*   m_data_exist;    // m_data_array[i]にデータが入っているときはi番目のビットが1になる。
#if 0
//  [パターンその２]
//  この書き方はエラー
  void (*FixedListInitializeDefaultDataFunction)(T* a)         m_initialzie_default_data_function;
  s32  (*FixedListCompareDataFunction)(const T* a, const K* b) m_compare_data_function;
  void (*FixedListCopyDataFunction)(T* dst, const T* src)      m_copy_data_function;
#elif 0
  // [パターンその３]
  // この書き方はOK
  void (*m_initialzie_default_data_function)(T* a);
  s32  (*m_compare_data_function)(const T* a, const K* b);
  void (*m_copy_data_function)(T* dst, const T* src);
#elif 1
  // [パターンその４]
  // この書き方はOK
  InitializeDefaultDataFunction  m_initialzie_default_data_function; 
  CompareDataFunction            m_compare_data_function; 
  CopyDataFunction               m_copy_data_function;      
#endif

public:
  // コンストラクタ
  FixedListArray(
      gfl::heap::HeapBase*  heap_memory,
      u32                   max_data_num,
#if 0
//      [パターンその２]
//      この書き方はエラー
      void (*FixedListInitializeDefaultDataFunction)(T* a)         initialzie_default_data_function,
      s32  (*FixedListCompareDataFunction)(const T* a, const K* b) compare_data_function,
      void (*FixedListCopyDataFunction)(T* dst, const T* src)      copy_data_function
#elif 0
      // [パターンその３]
      // この書き方はOK
      void (*initialzie_default_data_function)(T* a),
      s32  (*compare_data_function)(const T* a, const K* b),
      void (*copy_data_function)(T* dst, const T* src)
#elif 1
      // [パターンその４]
      // この書き方はOK
      InitializeDefaultDataFunction  initialzie_default_data_function,
      CompareDataFunction            compare_data_function,
      CopyDataFunction               copy_data_function
#endif
  )
    : m_max_data_num(max_data_num),
      m_data_num(0),
      m_head(NULL),
      m_tail(NULL),
      m_data_array(NULL),
      m_data_exist(NULL),
      m_initialzie_default_data_function(initialzie_default_data_function),
      m_compare_data_function(compare_data_function),
      m_copy_data_function(copy_data_function)
  {
    GFL_ASSERT_STOP( m_max_data_num > 0 );

    GFL_ASSERT_STOP(m_initialzie_default_data_function);
    GFL_ASSERT_STOP(m_compare_data_function);
    GFL_ASSERT_STOP(m_copy_data_function);

    m_data_array = GFL_NEW_ARRAY(heap_memory) FixedListData<T>[m_max_data_num];
    m_data_exist = GFL_NEW(heap_memory) gfl::base::Bit(heap_memory, m_max_data_num);
  }


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#endif

