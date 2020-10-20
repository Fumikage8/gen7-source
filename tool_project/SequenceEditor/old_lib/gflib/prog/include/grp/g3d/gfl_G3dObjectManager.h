#ifndef __GFL_G3DOBJECTMANAGER_H__
#define __GFL_G3DOBJECTMANAGER_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dObjectManager.h
 *	@brief  3Dオブジェクトマネージャー
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/ut.h>

// gflib
#include <gfl_debug.h>
#include <gfl_Base.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dObject.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					関数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *  @brief         オブジェクトにIDを設定する
 *                 class Objectのフレンド関数
 *
 *  @param[in]     object    オブジェクト 
 *  @param[in]     a_id      ID
 */
//-----------------------------------------------------------------------------
extern void Object_SetId(Object* object, s32 a_id);
//-----------------------------------------------------------------------------
/**
 *  @brief         オブジェクトに種類を設定する
 *                 class Objectのフレンド関数
 *
 *  @param[in]     object    オブジェクト 
 *  @param[in]     a_type    オブジェクトの種類
 */
//-----------------------------------------------------------------------------
extern void Object_SetType(Object* object, ObjectType a_type);




//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dオブジェクトマネージャークラス
//=====================================
class ObjectManager
{
  GFL_FORBID_COPY_AND_ASSIGN(ObjectManager);

private:
  typedef gfl::base::SimpleContainer<Object*> ObjectArray;

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   * 
   *  @param[in]     heap_memory    ヒープメモリ
   *  @param[in]     object_num     登録できるオブジェクト数
   *  @param[in]     object_type    オブジェクトの種類
   */
  //-----------------------------------------------------------------------------
  ObjectManager(gfl::heap::HeapBase* heap_memory, u32 object_num, ObjectType object_type)
    : m_object_num(object_num),
      m_object_type(object_type),
      m_object_array(NULL),
      m_object_id_bit(NULL)
  {
    m_object_id_bit = GFL_NEW(heap_memory) gfl::base::Bit(heap_memory, object_num);
    m_object_array = GFL_NEW(heap_memory) ObjectArray(heap_memory, object_num);
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ObjectManager()
  {
    if(m_object_array)
    {
#if GFL_DEBUG
      {
        // 残っているオブジェクトの情報を出力する
        if( !(m_object_array->IsEmpty()) )
        {
          GFL_PRINT( "オブジェクトが残っています。ObjectType=%d num=%d\n", m_object_type, m_object_array->GetDataNum() );
          s32 l_index = 0;
          for(s32 l_id=0; l_id<m_object_num; l_id++)
          {
            if( m_object_id_bit->GetBit(l_id) != 0 )
            {
              Object* l_object;
              b32 ret = m_object_array->GetDataByIndex(l_index, &l_object);
              if(ret)
              {
                GFL_PRINT("id=%d, index=%d\n", l_id, l_index);
                PrintObjectInformation(l_object);
              }
              else
              {
                GFL_ASSERT_MSG( 0, "オブジェクトが残っているのにその情報が取得できません。ObjectType=%d num=%d, id=%d, index=%d\n", m_object_type, m_object_array->GetDataNum(), l_id, l_index );
              }
              ++l_index;
            }
          }
        }
      }
#endif  // #if GFL_DEBUG
      GFL_ASSERT_MSG( m_object_array->IsEmpty(), "オブジェクトが残っています。 ObjectType=%d num=%d\n", m_object_type, m_object_array->GetDataNum() );  //@check メモリに残ったままだが続行はできる。
      GFL_DELETE m_object_array;
      m_object_array = NULL;
    }
    if(m_object_id_bit)
    {
      GFL_DELETE m_object_id_bit;
      m_object_id_bit = NULL;
    }
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         オブジェクトを登録する
   *
   *  @param[in]     object  登録するオブジェクト
   *
   *  @retval        bool  登録できたらtrue
   */
  //-----------------------------------------------------------------------------
  bool AddObject(Object* object)
  {
#if GFL_DEBUG
    if( m_object_array->GetDataNum() != m_object_id_bit->GetOnBitNum() )
    {
      GFL_PRINT("DataNum=%d, OnBitNum=%d\n", m_object_array->GetDataNum(), m_object_id_bit->GetOnBitNum());
      GFL_ASSERT_MSG( 0, "AddObjectで個数ずれ\n" );
    }
#endif  // #if GFL_DEBUG

    Object_SetType(object, m_object_type);

    u32 current_num = m_object_array->GetDataNum();
    GFL_ASSERT_MSG( current_num < m_object_num, "オブジェクトが既に%d個あるので登録できません。\n", current_num );  //@check
    if(current_num < m_object_num)
    {
      for(s32 l_id=0; l_id<m_object_num; l_id++)
      {
        if( m_object_id_bit->GetBit(l_id) == 0 )
        {
          m_object_array->AddDataToLast(object);
          m_object_id_bit->On(l_id);
          Object_SetId(object, l_id);

#if GFL_DEBUG
          if( m_object_array->GetDataNum() != m_object_id_bit->GetOnBitNum() )
          {
            GFL_PRINT("DataNum=%d, OnBitNum=%d, l_id=%d\n", m_object_array->GetDataNum(), m_object_id_bit->GetOnBitNum(), l_id);
            GFL_ASSERT_MSG( 0, "AddObjectで個数ずれ\n" );
          }
#endif  // #if GFL_DEBUG
          
          return true;
        }
      }
    }

#if GFL_DEBUG
    GFL_PRINT("MaxDataNum=%d, DataNum=%d\n", m_object_array->GetMaxDataNum(), m_object_array->GetDataNum());
    GFL_PRINT("BitNum=%d, OnBitNum=%d, OffBitNum=%d\n", m_object_id_bit->GetBitNum(), m_object_id_bit->GetOnBitNum(), m_object_id_bit->GetOffBitNum());
#endif  // #if GFL_DEBUG
    GFL_ASSERT_MSG( 0, "オブジェクトを登録できませんでした。\n" );
    return false;
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         オブジェクトの登録を解除する
   *
   *  @param[in]     object  登録を解除するオブジェクト
   * 
   *  @retval        bool  解除できたらtrue
   */
  //-----------------------------------------------------------------------------
  bool RemoveObject(Object* object)
  {
    ObjectType l_type = object->GetObjectType();
    GFL_ASSERT_MSG( l_type == m_object_type, "管理しているのとは異なる種類のオブジェクト%dなので登録を解除しません。\n", l_type );  //@check
    if( l_type != m_object_type ) return false;

    s32 l_id = object->GetId();
    GFL_ASSERT_MSG( l_id >= 0, "管理外のID%dのオブジェクトなので登録を解除しません。\n", l_id );
    if(l_id >= 0)
    {
      b32 ret = m_object_array->RemoveData(object);
      GFL_ASSERT_MSG( ret, "ID%dのオブジェクトを見付けられませんでした。\n", l_id );
      m_object_id_bit->Off(l_id);
      Object_SetId(object, OBJECT_ID_NOT_MANAGED);

#if GFL_DEBUG
      if( m_object_array->GetDataNum() != m_object_id_bit->GetOnBitNum() )
      {
        GFL_PRINT("DataNum=%d, OnBitNum=%d\n", m_object_array->GetDataNum(), m_object_id_bit->GetOnBitNum());
        GFL_ASSERT_MSG( 0, "RemoveObjectで個数ずれ\n" );
      }
#endif  // #if GFL_DEBUG

      return true;
    }
    GFL_ASSERT_MSG( 0, "オブジェクトの登録を解除できませんでした。\n" );
    return false;
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         オブジェクトを取得する
   *  
   *  @param[in]     index    何番目のオブジェクトか(0<=index && index<GetObjectNum)(オブジェクトのIDではなく配列のインデックス)
   *  @param[out]    object   取得したオブジェクト
   *  @retval        b32      取得できたらtrue, 取得できなかったらfalse
   */
  //-----------------------------------------------------------------------------
  b32 GetObjectByIndex(const u32 index, Object** object) const
  {
    return m_object_array->GetDataByIndex(index, object);
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         現在のオブジェクト数を取得する
   *  
   *  @retval        u32  現在のオブジェクト数
   */
  //-----------------------------------------------------------------------------
  u32 GetObjectNum(void) const
  {
    return m_object_array->GetDataNum();
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         オブジェクトの情報を出力する
   *  
   *  @param[in]  object  オブジェクト
   */
  //-----------------------------------------------------------------------------
  void PrintObjectInformation(Object* object) const;


private:
  u32                         m_object_num;
  ObjectType                  m_object_type;
  ObjectArray*                m_object_array;
  gfl::base::Bit*             m_object_id_bit;  // 使用中のID番目のビットはオンになっている
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DOBJECTMANAGER_H__
