#ifndef __GFL_FIXEDLIST_H__
#define __GFL_FIXEDLIST_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FixedList.h
 *	@brief  �Œ�T�C�Y�̃��X�g�̃N���X
 *	@author	Koji Kawada
 *	@date		2012.02.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


/*

�p��
Element, element :  FixedListElement�������Ƃ��B�v�f�B���X�g�̗v�f�B
Data, data       :  FixedListElement�̃����o�ł���data�������Ƃ��B�f�[�^�B

���
���x�d���Ȃ̂ŁuFixedListArray��FixedListArraySentinel�̋��ʍ��𔲂��o�������N���X������A
���̊��N���X���p������virtual�֐����I�[�o�[���C�h����v�Ƃ������Ƃ͂��Ă��܂���B

C++�֐��Ăяo���̑��x
�ďo�� �b 
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
// �\���̐錾
//
///////////////////////////////////
// �Œ�T�C�Y�̃��X�g�̗v�f
template <typename T> struct FixedListElement
{
  T                    data;  // �f�[�^
  FixedListElement<T>* prev;  // �O�̗v�f�ւ̃|�C���^�B�擪�v�f�̂Ƃ���NULL�B�ԕ��ł̍ۂ́A�擪�v�f�̂Ƃ��͔ԕ����w���B
  FixedListElement<T>* next;  // ���̗v�f�ւ̃|�C���^�B�����v�f�̂Ƃ���NULL�B�ԕ��ł̍ۂ́A�擪�v�f�̂Ƃ��͔ԕ����w���B
};


///////////////////////////////////
//
// �N���X�錾
//
///////////////////////////////////
// �Œ�T�C�Y�̃��X�g�̔z��
template <typename T, typename K> class FixedListArray
{
  GFL_FORBID_COPY_AND_ASSIGN(FixedListArray);

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
  FixedListArray(
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
      // �g�p�O�ɕK���Ă΂��getSpaceElement�֐�����m_initialzie_default_data_function���Ăяo���Ă���̂�
      // �����ł�m_initialzie_default_data_function���ĂԕK�v�͂Ȃ��B
    }

    m_element_exist = GFL_NEW(heap_memory) gfl::base::Bit(heap_memory, m_max_element_num);
  }

  // �f�X�g���N�^
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
  // �擪�v�f���擾����
  FixedListElement<T>* GetElementHead(void) const
  {
    return m_head;
  }

  // �����v�f���擾����
  FixedListElement<T>* GetElementTail(void) const
  {
    return m_tail;
  }

  // �f�[�^��T���āA���ꂪ�����Ă���v�f���擾����
  // ����key�̃f�[�^����������Ƃ��A�O�ɂ�����̂��擾����
  // NULL�̂Ƃ�key�̃f�[�^���Ȃ�
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
  // �擪�ɋ�f�[�^�̗v�f��ǉ����A����𓾂�
  // �f�[�^�������Ă��鈵���ɂȂ�A�f�[�^�������Ă������������
  // NULL�̂Ƃ��ǉ��ł��Ȃ�����
  FixedListElement<T>* AddGetSpaceElementHead(void)
  {
    u32 index = 0;
    FixedListElement<T>* space_element = getSpaceElement(&index);
    if(space_element)
    {
      if( m_head )
      {
        // m_head������̂ŁA�f�[�^��1�͂���
        space_element->next = m_head;
        m_head->prev = space_element;
      }
      else
      {
        // �f�[�^��1���Ȃ��Ƃ�
        m_tail = space_element;
      }
      m_head = space_element;
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
      if( m_head )
      {
        // m_head������̂ŁA�f�[�^��1�͂���
        space_element->prev = m_tail;
        m_tail->next = space_element;
      }
      else
      {
        // �f�[�^��1���Ȃ��Ƃ�
        m_head = space_element;
      }
      m_tail = space_element;
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
    if( m_head == target_element )
    {
      m_head = target_element->next;
    }
    else  // if( target_element->prev ) �ɓ���
    {
      target_element->prev->next = target_element->next;
    }

    if( m_tail == target_element )
    {
      m_tail = target_element->prev;
    }
    else  // if( target_element->next ) �ɓ���
    {
      target_element->next->prev = target_element->prev;
    }

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
    if( m_head )
    {
      m_copy_data_function(a, &(m_head->data), m_function_work);
      RemoveElment(m_head); 
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
    if( m_tail )
    {
      m_copy_data_function(a, &(m_head->data), m_function_work);
      RemoveElement(m_tail); 
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

private:
  // �����o�ϐ�
  u32                  m_max_element_num;  // m_element_array�z��̊m�ۂ��Ă���v�f��
  u32                  m_element_num;      // m_element_array�̒��Ńf�[�^�������Ă����
  FixedListElement<T>* m_head;             // �擪�v�f�ւ̃|�C���^�B�f�[�^��1�������Ă��Ȃ��Ƃ���NULL�B
  FixedListElement<T>* m_tail;             // �����v�f�ւ̃|�C���^�B�f�[�^��1�������Ă��Ȃ��Ƃ���NULL�B
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


#endif  // #ifndef __GFL_FIXEDLIST_H__




#if 0

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//���������g�p�၄������

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
    //s32* ppp;  // ����ł̓_��
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
  //typedef FixedListArray<TestData9Byte, s32*> ListArrayTestData;  // ����ł̓_��
  typedef FixedListArray<TestData9Byte, s32ptr> ListArrayTestData;
  static void FixedListInitializeDefaultDataFunction(TestData9Byte* a)
  {
    GFL_PRINT("init\n");
    a->aaa = 101;
  }
  //static s32  FixedListCompareDataFunction(const TestData9Byte* a, const s32* b)  // ����ł̓_��
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
��������template��typename�Ƀ|�C���^��n��������������

  typedef s32* s32ptr;
  struct TestData9Byte : public TestData4Byte
  {
    //s32* ppp;  // ����ł̓_��
    s32ptr ppp;
    s8   padding;
    TestData9Byte(void)
      : TestData4Byte(),
        ppp(NULL)
    {}
  };

  typedef FixedListElement<TestData9Byte> ListElementTestData;
  //typedef FixedListArray<TestData9Byte, s32*> ListArrayTestData;  // ����ł̓_��
  typedef FixedListArray<TestData9Byte, s32ptr> ListArrayTestData;
  static void FixedListInitializeDefaultDataFunction(TestData9Byte* a)
  {
    GFL_PRINT("init\n");
    a->aaa = 101;
  }
  //static s32  FixedListCompareDataFunction(const TestData9Byte* a, const s32* b)  // ����ł̓_��
  static s32  FixedListCompareDataFunction(const TestData9Byte* a, const s32ptr b)
  {
    GFL_PRINT("comp\n");
    if( a->ppp == 0 )     return 0;
    else if( a->ppp > 0 ) return 1;
    else                  return -1;
  }

http://www.ced.is.utsunomiya-u.ac.jp/lecture/2010/prog/p2/kadai2/typedef.php
typedef�̗��p
typedef�̗��p�@�@�Q�@�|�|�C���^�̍쐬��e�Ղɂ���


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
���������z��̃T�C�Y�̃�����������

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
    //s32* ppp;  // ����ł̓_��
    s32ptr ppp;
    s8   padding;
    TestData9Byte(void)
      : TestData4Byte(),
        ppp(NULL)
    {}
  };

    GFL_PRINT("T size=%d\n", sizeof(FixedListElement<T>));
    GFL_PRINT("0 size=%d\n", sizeof(m_element_array[0]));

�T�C�Y�͂ǂ����20�o�C�g�������B
�܂�Asizeof(FixedListElement<T>)�ŁA������ƃA���C�����g���ꂽ�z���1�v�f�̃T�C�Y������B

http://www.g-ishihara.com/c_st_01.htm
�\���̂̃T�C�Y�ƃA���C�����g
sizeof���Z�q�ŕԂ��Ă���T�C�Y�́A�A���C�����g�����f����Ă���̂ŁA��ɐ������ƌ�����ł��傤�B


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
���������֐��|�C���^�̏�����������������

[�p�^�[�����̂P]
typedef��template�͂ł��Ȃ��̂ŁA���̏������̓G���[�B

///////////////////////////////////
//
// �^�錾
//
///////////////////////////////////
// �f�[�^���f�t�H���g����������
typedef template <typename T> void (*FixedListInitializeDefaultDataFunction)(T* a);

// �f�[�^���r����
// a�Ɋ܂܂��K��b���r����
// �����Ƃ�0��Ԃ��Ba�̂ق����傫���Ƃ����̒l��Ԃ��Ba�̂ق����������Ƃ����̒l��Ԃ�
typedef template <typename T, typename K> s32 (*FixedListCompareDataFunction)(const T* a, const K* b);

// �f�[�^���R�s�[����
// src��dst�ɃR�s�[����
typedef template <typename T> void (*FixedListCopyDataFunction)(T* dst, const T* src);


///////////////////////////////////
//
// �N���X�錾
//
///////////////////////////////////
// �Œ�T�C�Y�̃��X�g�̔z��
template <typename T, typename K> class FixedListArray
{
  GFL_FORBID_COPY_AND_ASSIGN(FixedListArray);

public:
  // [�p�^�[�����̂S]
  // �֐��|�C���^��`
  typedef void (*InitializeDefaultDataFunction)(T* a);         
  typedef s32  (*CompareDataFunction)(const T* a, const K* b); 
  typedef void (*CopyDataFunction)(T* dst, const T* src);      

private:
  // �����o�ϐ�
  u32               m_max_data_num;  // m_data_array�̔z��̃T�C�Y
  u32               m_data_num;      // m_data_array�̒��Ńf�[�^�������Ă����
  FixedListData<T>* m_head;          // �擪�f�[�^�ւ̃|�C���^�B�f�[�^��1�������Ă��Ȃ��Ƃ���NULL�B
  FixedListData<T>* m_tail;          // �����f�[�^�ւ̃|�C���^�B�f�[�^��1�������Ă��Ȃ��Ƃ���NULL�B
  FixedListData<T>* m_data_array;    // �f�[�^������z��B�z��̃T�C�Y��m_max_data_num
  gfl::base::Bit*   m_data_exist;    // m_data_array[i]�Ƀf�[�^�������Ă���Ƃ���i�Ԗڂ̃r�b�g��1�ɂȂ�B
#if 0
//  [�p�^�[�����̂Q]
//  ���̏������̓G���[
  void (*FixedListInitializeDefaultDataFunction)(T* a)         m_initialzie_default_data_function;
  s32  (*FixedListCompareDataFunction)(const T* a, const K* b) m_compare_data_function;
  void (*FixedListCopyDataFunction)(T* dst, const T* src)      m_copy_data_function;
#elif 0
  // [�p�^�[�����̂R]
  // ���̏�������OK
  void (*m_initialzie_default_data_function)(T* a);
  s32  (*m_compare_data_function)(const T* a, const K* b);
  void (*m_copy_data_function)(T* dst, const T* src);
#elif 1
  // [�p�^�[�����̂S]
  // ���̏�������OK
  InitializeDefaultDataFunction  m_initialzie_default_data_function; 
  CompareDataFunction            m_compare_data_function; 
  CopyDataFunction               m_copy_data_function;      
#endif

public:
  // �R���X�g���N�^
  FixedListArray(
      gfl::heap::HeapBase*  heap_memory,
      u32                   max_data_num,
#if 0
//      [�p�^�[�����̂Q]
//      ���̏������̓G���[
      void (*FixedListInitializeDefaultDataFunction)(T* a)         initialzie_default_data_function,
      s32  (*FixedListCompareDataFunction)(const T* a, const K* b) compare_data_function,
      void (*FixedListCopyDataFunction)(T* dst, const T* src)      copy_data_function
#elif 0
      // [�p�^�[�����̂R]
      // ���̏�������OK
      void (*initialzie_default_data_function)(T* a),
      s32  (*compare_data_function)(const T* a, const K* b),
      void (*copy_data_function)(T* dst, const T* src)
#elif 1
      // [�p�^�[�����̂S]
      // ���̏�������OK
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

