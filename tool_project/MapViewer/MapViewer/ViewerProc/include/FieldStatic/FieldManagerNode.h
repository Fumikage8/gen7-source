//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldManagerNode.h
 *	@brief  �t�B�[���h�}�l�[�W���[���m�[�h 
 *	@author	takahashi tomoya
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_MANAGER_NODE_H__ )
#define __FIELD_MANAGER_NODE_H__
#pragma once

#include "ViewerProc/include/FieldStatic/FieldNode.h"
GFL_NAMESPACE_BEGIN(Field);


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
/**
 *  @class �}�l�[�W���[�m�[�h
 */
class IFieldManagerNode : public FieldNode
{
public:
  /**
   * @class �f�[�^�A�N�Z�T�[
   * ��X��Viewer�쐬��f�o�b�O�@�\�����̂��߂ɁA��{�I�ȃ}�l�[�W���[����у}�l�[�W���[����������N���X�͈ˑ��x��Ⴍ�ۂB
   *
   * ���̃��W���[���Ɉˑ��������ȏꏊ�́A�f�[�^�A�N�Z�T�[�ɂ܂Ƃ߂�B
   */
  class IAccesser
  {
    IAccesser(){}
    virtual ~IAccesser(){}

    /*
      virtual gfl2::heap::HeapBase * GetUseHeapBase() const = 0;
      virtual const XXData & GetXXData() const = 0;
     */
    
  };

public:
  virtual ~ IFieldManagerNode()
  {
  }

  /**
   * @brief �t�@�C���ǂݍ��݊Ǘ�
   */
  virtual void StartFileRead( void ) = 0;
  virtual bool WaitFileRead( void ) = 0;

  /**
   * @brief �g�p�\��ԂɃZ�b�g�A�b�v
   */
  virtual bool Setup( void ) = 0;


  /**
   * @brief �j��
   */
  virtual bool Finalize( void ) = 0;

  /**
   * @brief �j���̕K�v�����邩�B
   */
  virtual bool IsExists( void ) const = 0;

protected:
  IFieldManagerNode()
  {
  }
};

GFL_NAMESPACE_END(Field);



#endif	// __FIELD_MANAGER_NODE_H__





