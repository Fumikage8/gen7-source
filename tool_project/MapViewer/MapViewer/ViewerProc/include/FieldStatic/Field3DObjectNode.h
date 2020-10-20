//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		Field3DObjectNode.h
 *	@brief  �t�B�[���h3D�I�u�W�F�N�g���m�[�h
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_3DOBJECT_NODE_H__ )
#define __FIELD_3DOBJECT_NODE_H__
#pragma once

#include <math/include/gfl2_math.h>
#include <renderingengine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

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
 *  @class �t�B�[���h���m�[�h
 */
class IField3DObjectNode : public gfl2::renderingengine::scenegraph::NodeTrait<IField3DObjectNode,FieldNode>
{
public:

  /**
   * @class ����̑���𒊏ۉ�
   *
   * IField3DObjectNode���p��������̏����ŁA
   * �O�����J���鏈���Ɋւ��ẮA
   * IHandle���p�������N���X�ō쐬����B
   *
   * �E�A�j���[�V��������
   * �E����̏��̎擾
   *
   * �T�|�[�g�O�̌^�������ɂ킽���Ă��Ă��Ȃ����̃`�F�b�N�͕K���s�Ȃ��B
   */
  class IHandle
  {
  public:
    IHandle(){}
    virtual ~IHandle(){}

    /**
     *  @brief ����\�ȃI�u�W�F�N�g���`�F�b�N
     */
    virtual bool IsSupport( const IField3DObjectNode & node ) const = 0;

    /**
     * @brief Handle������SafeCast���ăA�j���[�V�����̏I������҂B
     */
    virtual bool IsEnd( const IField3DObjectNode * node ) const = 0;
  };

  class ISetHandle : public IHandle
  {
  public:
    ISetHandle(){}
    virtual ~ISetHandle(){}
    /**
     * @brief Handle������SafeCast���ăA�j���[�V�����̐��䓙���s�Ȃ��B
     */
    virtual void Handle( IField3DObjectNode * node ) = 0;
  };
  class IGetHandle : public IHandle
  {
  public:
    IGetHandle(){}
    virtual ~IGetHandle(){}

    /**
     * @brief Handle������SafeCast���ăA�j���[�V�����̐��䓙���s�Ȃ��B
     */
    virtual void Handle( const IField3DObjectNode * node ) = 0;
  };


public:
  virtual ~IField3DObjectNode()
  {
  }

	static gfl2::util::FnvHash GetTypeHash()
	{
		return gfl2::util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::FieldNode::Field3DObjectNode") ) );
	}

  /**
   * @brief �\��ON�EOFF
   */
  virtual void SetVisible( bool isVisible ) = 0;
  virtual bool GetVisible( void ) const = 0;

  /**
   * @brief SRT�R���g���[��
   */
  virtual void SetLocalSRT( const gfl2::math::SRT & srt ) = 0;
  virtual const gfl2::math::SRT & GetLocalSRT() const = 0;

  /**
   * @brief ���f���C���X�^���X���擾
   */
  virtual u32 GetModelInstanceNum() const = 0;
  virtual const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * GetModelInstance( int index = 0 ) const = 0;

  //! ����̑�����s�Ȃ��B
  void Set( ISetHandle * handle )
  {
    if( handle->IsSupport( *this ) )
    {
      handle->Handle( this );
    }
    else
    {
      GFL_ASSERT( 0 );
    }

  }
  //! ����̒l�̎擾���s�Ȃ��B
  void Get( IGetHandle * handle ) const
  {
    if( handle->IsSupport( *this ) )
    {
      handle->Handle( this );
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

  //! ����̊�����҂�
  bool IsEnd( const IHandle & handle ) const
  {
    if( handle.IsSupport( *this ) )
    {
      return handle.IsEnd( this );
    }
    else
    {
      GFL_ASSERT( 0 );
    }
    return true;
  }


  //! ���f���C���X�^���X�̒x������͎��R�Ɏ���
  /**! �폜���N�G�X�g���󂯂����f���C���X�^���X�����
   * -���s�^�C�~���O�@�t�B�[���h�v���Z�X�̐擪�Ŏ��s���܂��B
   * -�����@�@�@�@�@�@PipeLine�̕`���Ƀ��f���C���X�^���X�̒x��������N�G�X�g���o���Ȃ����ƁB
   */
  virtual void ExecuteTerminateRequest(void) = 0;
  //! ���S�ɔj���ł����Ԃ��`�F�b�N����B
  virtual bool IsCanTerminate(void) = 0;
  /*�@��j
   void TerminateRequest()
   {
      m_TerminateModelInstance = true;
      m_pModelInstance->SetVisible(false); // �\��OFF
   }

   virtual void ExecuteTerminateRequest()
   {
     if( m_TerminateModelInstance )
     {
       GFL_DELETE m_pModelInstance; // �C���X�^���X��j��
       m_TerminateModelInstance = false;
     }
   }
   */


protected:
  IField3DObjectNode()
  {
  }
};

GFL_NAMESPACE_END(Field);


#endif	// __FIELD_3DOBJECT_NODE_H__

