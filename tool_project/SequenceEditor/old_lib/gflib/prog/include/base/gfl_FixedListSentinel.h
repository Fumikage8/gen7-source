#ifndef __GFL_FIXEDLISTSENTINEL_H__
#define __GFL_FIXEDLISTSENTINEL_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FixedListSentinel.h
 *	@brief  �Œ�T�C�Y�̃��X�g�̃N���X(�ԕ�sentinel�g�p��)
 *	@author	Koji Kawada
 *	@date		2012.02.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


//�p�������gfl_FixedList.h�ɏ����Ă���B

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
// �\���̐錾
//
///////////////////////////////////
// �Œ�T�C�Y�̃��X�g�̗v�f
// template <typename T> struct FixedListElement
// ��gfl_FixedList.h�Œ�`���Ă���B


///////////////////////////////////
//
// �N���X�錾
//
///////////////////////////////////
// �Œ�T�C�Y�̃��X�g�̔z��
template <typename T, typename K> class FixedListArraySentinel
{
  GFL_FORBID_COPY_AND_ASSIGN(FixedListArraySentinel);

public:
  // �֐��|�C���^��`
  // �R���X�g���N�^�ōŏ���1�񂾂��Ă΂�鐶���֐�
  typedef void (*CreateDataFunction)(T* a, void* work);
  // �f�X�g���N�^�ōŌ��1�񂾂��Ă΂��j���֐�
  typedef void (*DestroyDataFunction)(T* a, void* work);
  // �f�[�^���f�t�H���g����������
  typedef void (*InitializeDefaultDataFunction)(T* a, void* work);         
  // �f�[�^���r����
  // a�Ɋ܂܂��K��b���r����
  // �����Ƃ�0��Ԃ��Ba�̂ق����傫���Ƃ����̒l��Ԃ��Ba�̂ق����������Ƃ����̒l��Ԃ�
  typedef s32  (*CompareDataFunction)(const T* a, const K b, void* work); 
  // �f�[�^���R�s�[����
  // src��dst�ɃR�s�[���� 
  typedef void (*CopyDataFunction)(T* dst, const T* src, void* work);      

public:
  // �R���X�g���N�^
  FixedListArraySentinel(
      gfl::heap::HeapBase*  heap_memory,
      u32                   max_element_num,
      CreateDataFunction             create_data_function,
      DestroyDataFunction            destroy_data_function,
      InitializeDefaultDataFunction  initialzie_default_data_function,
      CompareDataFunction            compare_data_function,
      CopyDataFunction               copy_data_function,
      void*                          function_work  // ���̃N���X�ŕێ������̂ŁA���̃N���X���������͌Ăяo�����Ŕj�����Ȃ��悤�ɁB
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
      // �g�p�O�ɕK���Ă΂��getSpaceElement�֐�����m_initialzie_default_data_function���Ăяo���Ă���̂�
      // �����ł�m_initialzie_default_data_function���ĂԕK�v�͂Ȃ��B
    }

    m_element_exist = GFL_NEW(heap_memory) gfl::base::Bit(heap_memory, m_max_element_num);
  }

  // �f�X�g���N�^
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
  // �z��̊m�ۂ��Ă���v�f�����擾����
  u32 GetMaxElementNum(void) const
  {
    return m_max_element_num;
  }

  // �f�[�^�������Ă�������擾����
  u32 GetElementNum(void) const
  {
    return m_element_num;
  }

public:
  // �ԕ��v�f�𓾂�
  // �g���Ă����̂�
  // �E���̖߂�l�̃|�C���^���̂���(���[�v�̏I������̔�r���Ɏg��)
  // �Enext(�擪�v�f��������)
  // �Eprev(�����v�f��������)
  // �̂ǂꂩ�B
  const FixedListElement<T>* GetSentinelElement(void) const
  {
    // const FixedListElement<T>* GetSentinelElement(void) const�̓R���p�C���ł������A
    // FixedListElement<T>* GetSentinelElement(void) const�̓R���p�C���ł��Ȃ������B
    return &m_sentinel;
  }
  
  // �擪�v�f���擾����
  // ����ꂽ�v�f��GetSentinelElement�̂Ƃ��f�[�^�Ȃ�(NULL�ł͂Ȃ��̂Œ���)
  FixedListElement<T>* GetElementHead(void) const
  {
    return m_sentinel.next;
  }

  // �����v�f���擾����
  // ����ꂽ�v�f��GetSentinelElement�̂Ƃ��f�[�^�Ȃ�(NULL�ł͂Ȃ��̂Œ���)
  FixedListElement<T>* GetElementTail(void) const
  {
    return m_sentinel.prev;
  }

  // �f�[�^��T���āA���ꂪ�����Ă���v�f���擾����
  // ����key�̃f�[�^����������Ƃ��A�O�ɂ�����̂��擾����
  // NULL�̂Ƃ�key�̃f�[�^���Ȃ�
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
  // �擪�ɋ�f�[�^�̗v�f��ǉ����A����𓾂�
  // �f�[�^�������Ă��鈵���ɂȂ�A�f�[�^�������Ă������������
  // NULL�̂Ƃ��ǉ��ł��Ȃ�����
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

  // �����ɋ�f�[�^�̗v�f��ǉ����A����𓾂�
  // �f�[�^�������Ă��鈵���ɂȂ�A�f�[�^�������Ă������������
  // NULL�̂Ƃ��ǉ��ł��Ȃ�����
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
  
  // �擪�Ƀf�[�^��ǉ�����
  // a�͗^�����Ă���R�s�[�֐��ɂ���ăR�s�[�����
  // �ǉ��ł����Ƃ����̃f�[�^���ǉ����ꂽ�v�f��Ԃ�
  // �ǉ��ł��Ȃ������Ƃ�NULL��Ԃ�
  FixedListElement<T>* SetDataHead(const T* a)
  {
    FixedListElement<T>* space_element = AddGetSpaceElementHead();
    if( space_element )
    {
      m_copy_data_function(&(space_element->data), a, m_function_work);
    }
    return space_element;
  }
  
  // �����Ƀf�[�^��ǉ�����
  // a�͗^�����Ă���R�s�[�֐��ɂ���ăR�s�[�����
  // �ǉ��ł����Ƃ����̃f�[�^���ǉ����ꂽ�v�f��Ԃ�
  // �ǉ��ł��Ȃ������Ƃ�NULL��Ԃ�
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
  // �v�f���폜���A���X�g���q������
  // �f�[�^�������Ă����������A�f�[�^�������Ă��Ȃ������ɂȂ�
  // ����target_element�͗^�����Ă���f�t�H���g�������֐��ŏ����������
  void RemoveElement(FixedListElement<T>* target_element)
  {
    target_element->prev->next = target_element->next;
    target_element->next->prev = target_element->prev;
    u32 index = getElementIndex(target_element);
    initializeDefaultElement(target_element);
    m_element_exist->Off(index);
    --m_element_num;
  }
  
  // �f�[�^��T���č폜����
  // ����key�̃f�[�^����������Ƃ��A�O�ɂ�����̂��폜����
  void RemoveData(const K key)
  {
    FixedListElement<T>* target_element = GetElement(key);
    if( target_element )
    {
      RemoveElement(target_element);
    }
  }

  // �f�[�^��T���āA�擾���Ă���폜����
  // ����key�̃f�[�^����������Ƃ��A�O�ɂ�����̂��I�������
  // a�Ɏ擾�����f�[�^�����ĕԂ�
  // ����Ɏ擾�ł����Ƃ�true��Ԃ�
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

  // �擪�̃f�[�^���擾���Ă���폜����
  // a�Ɏ擾�����f�[�^�����ĕԂ�
  // ����Ɏ擾�ł����Ƃ�true��Ԃ�
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
  
  // �����̃f�[�^���擾���Ă���폜����
  // a�Ɏ擾�����f�[�^�����ĕԂ�
  // ����Ɏ擾�ł����Ƃ�true��Ԃ�
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
  // ��f�[�^�̗v�f�𓾂�
  // index�ɂ͋�f�[�^�̗v�f�̔z��C���f�b�N�X�����ĕԂ�
  // NULL�̂Ƃ���f�[�^�Ȃ���index�͕s��
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

  // �v�f���f�t�H���g����������
  void initializeDefaultElement(FixedListElement<T>* target_element)
  {
    m_initialzie_default_data_function(&(target_element->data), m_function_work);
    target_element->prev = NULL;
    target_element->next = NULL;
  }

  // �v�f�̔z��C���f�b�N�X�𓾂�
  u32 getElementIndex(FixedListElement<T>* target_element) const
  {
    u32 index = 0;

    // (target_element-m_element_array)�Ŕz��̃C���f�b�N�X�̍�������ꂽ���A�ǂ̊��ł������������̂Ȃ̂��H
    // �悭������Ȃ��̂ŁAuptr�ŃA�h���X�𐔎��ɂ��č������߂邱�Ƃɂ����B
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

  // �R���X�g���N�^����Ăяo���ԕ��v�f�𐶐�����֐�
  void constructSentinel()
  {
    m_create_data_function(&(m_sentinel.data), m_function_work);
    m_initialzie_default_data_function(&(m_sentinel.data), m_function_work);
    // m_sentinel�ɑ΂��Ă�getSpaceElement�֐����Ă΂�Ȃ��̂�m_initialzie_default_data_function�������ŌĂ�ł����B
    m_sentinel.next = m_sentinel.prev = (&m_sentinel);
  }
  // �f�X�g���N�^����Ăяo���ԕ��v�f��j������֐�
  void destructSentinel(void)
  {
    m_destroy_data_function(&(m_sentinel.data), m_function_work);
  }

private:
  // �����o�ϐ�
  u32                  m_max_element_num;  // m_element_array�z��̊m�ۂ��Ă���v�f��
  u32                  m_element_num;      // m_element_array�̒��Ńf�[�^�������Ă����
  FixedListElement<T>  m_sentinel;         // �ԕ��v�f�Bnext�͐擪�v�f���w���Bprev�͖����v�f���w���B
                                           // �f�[�^��1�������Ă��Ȃ��Ƃ���next=prev=(&m_sentinel)�ƂȂ��Ă���B
  FixedListElement<T>* m_element_array;    // �f�[�^������z��B�z��̊m�ۂ��Ă���v�f����m_max_element_num
  gfl::base::Bit*      m_element_exist;    // m_element_array[i]�Ƀf�[�^�������Ă���Ƃ���i�Ԗڂ̃r�b�g��1�ɂȂ�B
 
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

