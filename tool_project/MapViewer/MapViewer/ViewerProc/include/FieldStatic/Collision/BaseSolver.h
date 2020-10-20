#pragma once
//=================================================================================================
/**
 *  @file   BaseSolver.h
 *  @brief  �\���o�[�x�[�X
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <types/include/gfl2_Typedef.h>
class BaseActor;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {
  class InstanceNode;
} } } }

//----------------------------------------------------------------------------------------
//! @brief �\���o�[�x�[�X
//----------------------------------------------------------------------------------------
class BaseSolver
{
public:

  static const s32 MARJIN = 0.5f;     //< �挒���̂��߂ɗ]���ɉ����Ԃ���

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �R���X�g���N�^
  */
  //-----------------------------------------------------------------------------
  BaseSolver();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �f�X�g���N�^
  */
  //-----------------------------------------------------------------------------
  virtual ~BaseSolver();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �c���[�Ԃ̏Փ˂�����
  *
  *  @param  pRootStaticActorRoot  �Փ˂����c���[
  *  @param  pRootStaticActorRoot  �Փ˂���c���[
  */
  //-----------------------------------------------------------------------------
  virtual void Solved(gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootStaticActorRoot,
                      gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootDynamicActorRoot);

private:

};
