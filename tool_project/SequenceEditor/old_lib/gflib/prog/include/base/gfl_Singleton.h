#ifndef __GFL_SINGLETON_H__
#define __GFL_SINGLETON_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Singleton.h
 *	@brief  �V���O���g���N���X
 *	@author	Koji Kawada
 *	@date		2010.10.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C

// gflib
#include <debug/gfl_Assert.h>
#include <gfl_Macros.h>


namespace gfl {
namespace base {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
// �V���O���g���N���X
template <typename T> class Singleton
{
  GFL_FORBID_COPY_AND_ASSIGN(Singleton);

public:

  ///  Type of the singleton object
  typedef T ObjectType;

  //-----------------------------------------------------------------------------
  /*
   *  @brief         �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  inline Singleton(void)
  {
    GFL_ASSERT_STOP(!m_instance);
    m_instance = static_cast<T*>(this);
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  inline ~Singleton()  // �p������
  {
    GFL_ASSERT_STOP(m_instance);
    m_instance = 0;
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         �C���X�^���X���擾
   *
   *  @retval        T*    �V���O���g���N���X�̃C���X�^���X
   */
  //-----------------------------------------------------------------------------
  static inline T* GetInstance(void)
  {
    return m_instance;
  }

private:
  static T* m_instance;  // �C���X�^���X
};

//-----------------------------------------------------------------------------
/**
 *					�N���X��static�����o�ϐ�
*/
//-----------------------------------------------------------------------------
// �V���O���g���N���X��static�����o�ϐ�
template <typename T> T* Singleton<T>::m_instance = 0;

}  // namespace base
}  // namespace gfl




//-----------------------------------------------------------------------------
/**
 *					Singleton�N���X�A�N�Z�X��p�N���X�iDLL�o�R�ł̃A�N�Z�X�̂��߁j
*/
//-----------------------------------------------------------------------------
namespace gfl {
namespace base {

#ifdef GFL_USE_RO
  //---------------------------------------------------------------------------
  // DLL�L����
  //---------------------------------------------------------------------------
  #ifndef SINGLETON_EXPORT
  #define SINGLETON_EXPORT
  #endif

  template <typename T> class SINGLETON_EXPORT SingletonAccessor
  {
    public:
      static T* GetInstance();
  };

  /**
  * @brief SINGLETON�p�v���g�^�C�v�錾
  * ���L�}�N�����Asingleton�p���N���X�̃w�b�_�t�@�C�����ŃR�[�����邱��
  */
  #define SINGLETON_INSTANCE_DECLARATION(classname)                         \
      template<> classname::ObjectType*                                     \
        gfl::base::SingletonAccessor<classname::ObjectType>::GetInstance();

  /**
  * @brief SINGLETON�p���̉��R�[�h
  * ���L�}�N�����Asingleton�p���N���X�̃\�[�X�R�[�h���ŃR�[�����邱��
  */
  #define SINGLETON_INSTANCE_DEFINITION(classname)                         \
      template<> classname::ObjectType*                                    \
        gfl::base::SingletonAccessor<classname::ObjectType>::GetInstance() \
      {                                                                    \
          return classname::GetInstance();                                 \
      }
#else

  //---------------------------------------------------------------------------
  // DLL������
  //---------------------------------------------------------------------------
  #define SINGLETON_INSTANCE_DECLARATION(classname)
  //
  #define SINGLETON_INSTANCE_DEFINITION(classname)

#endif

}  // namespace base
}  // namespace gfl

//-----------------------------------------------------------------------------
/**
 *					�}�N����`
*/
//-----------------------------------------------------------------------------
// �V���O���g���N���X�̃C���X�^���X���擾
#ifdef GFL_USE_RO
  #define GFL_SINGLETON_INSTANCE(classname) (gfl::base::SingletonAccessor<classname>::GetInstance())
#else
  #define GFL_SINGLETON_INSTANCE(classname) (classname::GetInstance())
#endif



#endif // __GFL_SINGLETON_H__
