#ifndef __GFL_SIMPLECONTAINER_H__
#define __GFL_SIMPLECONTAINER_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_SimpleContainer.h
 *	@brief  コンテナテンプレート
 *	@author	Koji Kawada
 *	@date		2011.04.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>


namespace gfl {
namespace base {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	コンテナクラス
//  コンストラクタやデストラクタが呼ばれるようなものをTにしないで下さい。
//=====================================
template <typename T> class SimpleContainer
{
  GFL_FORBID_COPY_AND_ASSIGN(SimpleContainer);


public:

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // コンストラクタ / デストラクタ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /*
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  SimpleContainer(void)
    : m_data_array(NULL),
      m_max_data_num(0),
      m_data_num(0)
  {
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         コンストラクタ
   *                 Createを呼びます。
   *  
   *  @param[in]     heap_meomry   ヒープメモリ
   *  @param[in]     max_data_num  データ数
   */
  //-----------------------------------------------------------------------------
  SimpleContainer(gfl::heap::HeapBase* heap_memory, const u32 max_data_num)
    : m_data_array(NULL),
      m_max_data_num(max_data_num),
      m_data_num(0)
  {
    Create(heap_memory, max_data_num);
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  ~SimpleContainer()
  {
    Destroy();
  }


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 生成 / 破棄
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /*
   *  @brief         生成
   *  
   *  @param[in]     heap_meomry   ヒープメモリ
   *  @param[in]     max_data_num  データ数
   */
  //-----------------------------------------------------------------------------
  void Create(gfl::heap::HeapBase* heap_memory, const u32 max_data_num)
  {
    GFL_ASSERT_MSG( ( !m_data_array ), "既に生成されています。\n" );  //@check
    
    if( !m_data_array )
    {
      m_max_data_num = max_data_num;

      if( m_max_data_num > 0 )
      {
        m_data_array = GFL_NEW_ARRAY(heap_memory) T[m_max_data_num];
      }

      m_data_num = 0;
    }
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  void Destroy(void)
  {
    if( m_data_array )
    {
      GFL_DELETE_ARRAY m_data_array;
      m_data_array = NULL;
    }
    m_max_data_num = 0;
    m_data_num = 0;
  }


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 追加 / 削除
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /*
   *  @brief         最後尾にデータを追加
   *  
   *  @param[in]     data  追加するデータ
   *  @retval        b32   追加できたらtrue, 追加できなかったらfalse
   */
  //-----------------------------------------------------------------------------
  b32 AddDataToLast(T data)
  {
    if( m_data_array && m_data_num < m_max_data_num )
    {
      m_data_array[m_data_num] = data;
      ++m_data_num;
      return true;
    }
    return false;
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         削除
   *  
   *  @param[in]     data  削除するデータ
   *  @retval        b32   削除できたらtrue, 削除できなかったらfalse
   */
  //-----------------------------------------------------------------------------
  b32 RemoveData(T data)
  {
    if( m_data_array )
    {
      for( u32 i=0; i<m_data_num; ++i )
      {
        if( m_data_array[i] == data )
        {
          for( u32 j=i; j<m_data_num -1; ++j )
          {
            m_data_array[j] = m_data_array[j+1];
          }
          --m_data_num;
          return true;
        }
      }
    }
    return false;
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         最後尾のデータを削除
   */
  //-----------------------------------------------------------------------------
  void RemoveLastData(void)
  {
    if( m_data_num > 0 )
    {
      --m_data_num;
    }
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         全削除
   */
  //-----------------------------------------------------------------------------
  void RemoveAllData(void)
  {
    m_data_num = 0;
  }


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 取得＆削除
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /*
   *  @brief         最後尾のデータを取得した後、削除する
   *  
   *  @param[out]    data   取得したデータ
   *  @retval        b32    取得できたらtrue, 取得できなかったらfalse
   */
  //-----------------------------------------------------------------------------
  b32 GetAndRemoveLastData(T* data)
  {
    if( data && m_data_array && m_data_num > 0 )
    {
      *data = m_data_array[m_data_num -1];
      --m_data_num;
      return true;
    }
    return false;
  }

  
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 取得
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  
  //-----------------------------------------------------------------------------
  /*
   *  @brief         データを取得する
   *  
   *  @param[in]     index  何番目のデータか(0<=index && index<m_data_num)
   *  @param[out]    data   取得したデータ
   *  @retval        b32    取得できたらtrue, 取得できなかったらfalse
   */
  //-----------------------------------------------------------------------------
  b32 GetDataByIndex(const u32 index, T* data) const
  {
    if( data && m_data_array && /*0<=index &&*/ index<m_data_num )
    {
      *data = m_data_array[index];
      return true;
    }
    return false;
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         最後尾のデータを取得する
   *  
   *  @param[out]    data   取得したデータ
   *  @retval        b32    取得できたらtrue, 取得できなかったらfalse
   */
  //-----------------------------------------------------------------------------
  b32 GetLastData(T* data) const
  {
    if( data && m_data_array && m_data_num > 0 )
    {
      *data = m_data_array[m_data_num -1];
      return true;
    }
    return false;
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         追加できる最大データ数
   *  
   *  @retval        u32  追加できる最大データ数
   */
  //-----------------------------------------------------------------------------
  u32 GetMaxDataNum(void) const
  {
    return m_max_data_num;
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         現在のデータ数
   *  
   *  @retval        u32  現在のデータ数
   */
  //-----------------------------------------------------------------------------
  u32 GetDataNum(void) const
  {
    return m_data_num;
  }

  
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 判定
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
 
  //-----------------------------------------------------------------------------
  /*
   *  @brief         空判定
   *  
   *  @retval        b32  データが全くないときtrue, データが1つ以上あるときfalse
   */
  //-----------------------------------------------------------------------------
  b32 IsEmpty(void) const
  {
    return (m_data_num == 0);
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         一杯判定
   *  
   *  @retval        b32  データが一杯のときtrue, データに余裕があるときfalse
   */
  //-----------------------------------------------------------------------------
  b32 IsFull(void) const
  {
    return (m_data_num == m_max_data_num);
  }


private:
  T*  m_data_array;
  u32 m_max_data_num;
  u32 m_data_num;
};


}  // namespace base
}  // namespace gfl

#endif // __GFL_SIMPLECONTAINER_H__
