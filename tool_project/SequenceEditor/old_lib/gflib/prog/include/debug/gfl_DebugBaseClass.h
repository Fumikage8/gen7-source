#ifndef __GFL_DEBUGBASECLASS_H__
#define __GFL_DEBUGBASECLASS_H__
//=============================================================================
/**
 * @file	  gfl_DebugBaseClass.h
 * @brief	  �f�o�b�O�p���N���X
            �f�o�b�O�ׂ̈ɃN���X���𕶎���ŊǗ����܂�
 * @author  k.ohno
 * @date	  2011.3.6
 */
//=============================================================================
#pragma once



#if GFL_DEBUG

namespace gfl{
namespace debug{

class DebugBaseClass{
  GFL_FORBID_COPY_AND_ASSIGN(DebugBaseClass); //�R�s�[�R���X�g���N�^�{����֎~

public:
  DebugBaseClass(void){};
  virtual ~DebugBaseClass(){};

  //----------------------------------------------------------------------------
  /**
   *	@brief    �N���X��o�^����
   *  @param    name �N���X��
   */
  //-----------------------------------------------------------------------------
  void AddClass(const char* name);
  //----------------------------------------------------------------------------
  /**
   *	@brief    �N���X���𓾂�
   *  @return   �N���X��
   */
  //-----------------------------------------------------------------------------
  const char* GetClassName(void);
  //----------------------------------------------------------------------------
  /**
   *	@brief    �N���X��p������
   *  @param    name �N���X��
   */
  //-----------------------------------------------------------------------------
  void DeleteClass(const char* name);



  

};


}  //debug
}  //gfl

#endif //GFL_DEBUG


//�N���X�錾�̃}�N��

//#if GFL_DEBUG
//#define CLASS_DECLARATION( className ) class className : public gfl::debug::DebugBaseClass
//#else //GFL_DEBUG
//#define CLASS_DECLARATION( className ) class className
//#endif //GFL_DEBUG




#endif //__GFL_DEBUGBASECLASS_H__

