#ifndef __GFL_G3DOBJECTMANAGER_H__
#define __GFL_G3DOBJECTMANAGER_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dObjectManager.h
 *	@brief  3D�I�u�W�F�N�g�}�l�[�W���[
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
 *					�֐��錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *  @brief         �I�u�W�F�N�g��ID��ݒ肷��
 *                 class Object�̃t�����h�֐�
 *
 *  @param[in]     object    �I�u�W�F�N�g 
 *  @param[in]     a_id      ID
 */
//-----------------------------------------------------------------------------
extern void Object_SetId(Object* object, s32 a_id);
//-----------------------------------------------------------------------------
/**
 *  @brief         �I�u�W�F�N�g�Ɏ�ނ�ݒ肷��
 *                 class Object�̃t�����h�֐�
 *
 *  @param[in]     object    �I�u�W�F�N�g 
 *  @param[in]     a_type    �I�u�W�F�N�g�̎��
 */
//-----------------------------------------------------------------------------
extern void Object_SetType(Object* object, ObjectType a_type);




//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�I�u�W�F�N�g�}�l�[�W���[�N���X
//=====================================
class ObjectManager
{
  GFL_FORBID_COPY_AND_ASSIGN(ObjectManager);

private:
  typedef gfl::base::SimpleContainer<Object*> ObjectArray;

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   * 
   *  @param[in]     heap_memory    �q�[�v������
   *  @param[in]     object_num     �o�^�ł���I�u�W�F�N�g��
   *  @param[in]     object_type    �I�u�W�F�N�g�̎��
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
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~ObjectManager()
  {
    if(m_object_array)
    {
#if GFL_DEBUG
      {
        // �c���Ă���I�u�W�F�N�g�̏����o�͂���
        if( !(m_object_array->IsEmpty()) )
        {
          GFL_PRINT( "�I�u�W�F�N�g���c���Ă��܂��BObjectType=%d num=%d\n", m_object_type, m_object_array->GetDataNum() );
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
                GFL_ASSERT_MSG( 0, "�I�u�W�F�N�g���c���Ă���̂ɂ��̏�񂪎擾�ł��܂���BObjectType=%d num=%d, id=%d, index=%d\n", m_object_type, m_object_array->GetDataNum(), l_id, l_index );
              }
              ++l_index;
            }
          }
        }
      }
#endif  // #if GFL_DEBUG
      GFL_ASSERT_MSG( m_object_array->IsEmpty(), "�I�u�W�F�N�g���c���Ă��܂��B ObjectType=%d num=%d\n", m_object_type, m_object_array->GetDataNum() );  //@check �������Ɏc�����܂܂������s�͂ł���B
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
   *  @brief         �I�u�W�F�N�g��o�^����
   *
   *  @param[in]     object  �o�^����I�u�W�F�N�g
   *
   *  @retval        bool  �o�^�ł�����true
   */
  //-----------------------------------------------------------------------------
  bool AddObject(Object* object)
  {
#if GFL_DEBUG
    if( m_object_array->GetDataNum() != m_object_id_bit->GetOnBitNum() )
    {
      GFL_PRINT("DataNum=%d, OnBitNum=%d\n", m_object_array->GetDataNum(), m_object_id_bit->GetOnBitNum());
      GFL_ASSERT_MSG( 0, "AddObject�Ō�����\n" );
    }
#endif  // #if GFL_DEBUG

    Object_SetType(object, m_object_type);

    u32 current_num = m_object_array->GetDataNum();
    GFL_ASSERT_MSG( current_num < m_object_num, "�I�u�W�F�N�g������%d����̂œo�^�ł��܂���B\n", current_num );  //@check
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
            GFL_ASSERT_MSG( 0, "AddObject�Ō�����\n" );
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
    GFL_ASSERT_MSG( 0, "�I�u�W�F�N�g��o�^�ł��܂���ł����B\n" );
    return false;
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �I�u�W�F�N�g�̓o�^����������
   *
   *  @param[in]     object  �o�^����������I�u�W�F�N�g
   * 
   *  @retval        bool  �����ł�����true
   */
  //-----------------------------------------------------------------------------
  bool RemoveObject(Object* object)
  {
    ObjectType l_type = object->GetObjectType();
    GFL_ASSERT_MSG( l_type == m_object_type, "�Ǘ����Ă���̂Ƃ͈قȂ��ނ̃I�u�W�F�N�g%d�Ȃ̂œo�^���������܂���B\n", l_type );  //@check
    if( l_type != m_object_type ) return false;

    s32 l_id = object->GetId();
    GFL_ASSERT_MSG( l_id >= 0, "�Ǘ��O��ID%d�̃I�u�W�F�N�g�Ȃ̂œo�^���������܂���B\n", l_id );
    if(l_id >= 0)
    {
      b32 ret = m_object_array->RemoveData(object);
      GFL_ASSERT_MSG( ret, "ID%d�̃I�u�W�F�N�g�����t�����܂���ł����B\n", l_id );
      m_object_id_bit->Off(l_id);
      Object_SetId(object, OBJECT_ID_NOT_MANAGED);

#if GFL_DEBUG
      if( m_object_array->GetDataNum() != m_object_id_bit->GetOnBitNum() )
      {
        GFL_PRINT("DataNum=%d, OnBitNum=%d\n", m_object_array->GetDataNum(), m_object_id_bit->GetOnBitNum());
        GFL_ASSERT_MSG( 0, "RemoveObject�Ō�����\n" );
      }
#endif  // #if GFL_DEBUG

      return true;
    }
    GFL_ASSERT_MSG( 0, "�I�u�W�F�N�g�̓o�^�������ł��܂���ł����B\n" );
    return false;
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �I�u�W�F�N�g���擾����
   *  
   *  @param[in]     index    ���Ԗڂ̃I�u�W�F�N�g��(0<=index && index<GetObjectNum)(�I�u�W�F�N�g��ID�ł͂Ȃ��z��̃C���f�b�N�X)
   *  @param[out]    object   �擾�����I�u�W�F�N�g
   *  @retval        b32      �擾�ł�����true, �擾�ł��Ȃ�������false
   */
  //-----------------------------------------------------------------------------
  b32 GetObjectByIndex(const u32 index, Object** object) const
  {
    return m_object_array->GetDataByIndex(index, object);
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���݂̃I�u�W�F�N�g�����擾����
   *  
   *  @retval        u32  ���݂̃I�u�W�F�N�g��
   */
  //-----------------------------------------------------------------------------
  u32 GetObjectNum(void) const
  {
    return m_object_array->GetDataNum();
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �I�u�W�F�N�g�̏����o�͂���
   *  
   *  @param[in]  object  �I�u�W�F�N�g
   */
  //-----------------------------------------------------------------------------
  void PrintObjectInformation(Object* object) const;


private:
  u32                         m_object_num;
  ObjectType                  m_object_type;
  ObjectArray*                m_object_array;
  gfl::base::Bit*             m_object_id_bit;  // �g�p����ID�Ԗڂ̃r�b�g�̓I���ɂȂ��Ă���
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DOBJECTMANAGER_H__
