#ifndef __GFL_DEBUGBASECLASSMANAGER_H__
#define __GFL_DEBUGBASECLASSMANAGER_H__
//=============================================================================
/**
 * @file	  gfl_DebugBaseClassManager.h
 * @brief	  �f�o�b�O�p���N���X
            �f�o�b�O�ׂ̈ɃN���X���𕶎���ŊǗ����܂�
            �f�o�b�O�p�̂��߈����₷�����ŗD�悷�邽�ߐÓI�ɂȂ��Ă��܂�
 * @author  k.ohno
 * @date	  2011.3.6
 */
//=============================================================================
#pragma once



#if GFL_DEBUG

namespace gfl{
namespace debug{

class DebugBaseClassManager{
  GFL_FORBID_COPY_AND_ASSIGN(DebugBaseClassManager); //�R�s�[�R���X�g���N�^�{����֎~


public:
  static const char CLASSNAME_SIZE = 24;  ///< ���O��ۑ�����o�b�t�@

private:

  
  struct ClassData{
    void* Next;       ///< ���̃|�C���^
    void* ClassPtr;   ///< �N���X�̃A�h���X
    unsigned long count;  ///< �m�ۏ���
    char Name[CLASSNAME_SIZE];  ///< �N���X��
  } ;

  static ClassData* mpClassDataStart; ///< �N���X�f�[�^�̐擪�̃A�h���X
  static ClassData* mpClassDataEnd;   ///< �N���X�f�[�^�̍Ō�̃A�h���X
  static unsigned long mCount;               ///< �J�E���^�[
  static gfl::heap::HeapBase *mpDebugHeap;   ///< �f�o�b�O�p�q�[�v�A�h���X

public:

  //----------------------------------------------------------------------------
  /**
   *	@brief    ����������
   *  @param    debugHeap �f�o�b�O�q�[�v�N���X�|�C���^
   */
  //-----------------------------------------------------------------------------
  static void Initialize( gfl::heap::HeapBase *debugHeap );
  //----------------------------------------------------------------------------
  /**
   *	@brief    �N���X���L�^����
   *  @param    name �N���X��
   */
  //-----------------------------------------------------------------------------
  static void AddClass(const char* name,void* classAddr);
  //----------------------------------------------------------------------------
  /**
   *	@brief    �N���X���𓾂�
   *  @return   �N���X��
   */
  //-----------------------------------------------------------------------------
  static const char* GetClassName(void* classAddr);
  //----------------------------------------------------------------------------
  /**
   *	@brief    �N���X������
   *  @return   �N���X�A�h���X
   */
  //-----------------------------------------------------------------------------
  static void RemoveClass(void* classAddr);

  //----------------------------------------------------------------------------
  /**
   *	@brief    �J������
   */
  //-----------------------------------------------------------------------------
  static void Finalize( void );

  
};  // class DebugBaseClassManager

}  //namespace debug
}  //namespace gfl

#endif //GFL_DEBUG

#endif //__GFL_DEBUGBASECLASSMANAGER_H__

