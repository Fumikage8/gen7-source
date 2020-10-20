//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file    ActionCmdTsuriTest.cpp
 *  @brief  �t�B�[���h�A�N�V�����R�}���h�ނ�e�X�g
 *  @author  yy-ikeuchi
 *  @date    2015.02.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#ifndef __FIELDACTIONCMDTSURITEST_H__
#define __FIELDACTIONCMDTSURITEST_H__

#include "FieldActionCmdBase.h"
#include <math/include/gfl2_Quaternion.h>
#include <math/include/gfl2_Vector.h>

namespace field
{
namespace mmodel
{

class ActionCmdTsuriTest : public ActionCmdBase
{
public:

  struct Work
  {
    s16 frame_max;
    s16 frame;
  };

public:

  // �R���X�g���N�^
  ActionCmdTsuriTest();

  //----------------------------------------------------------------------------
	/**
	*	@brief  �Ή�����A�N�V�����R�}���h��Ԃ�
	*/
	//-----------------------------------------------------------------------------
  ///virtual Acmd GetAcmdID( void ) const{ return AC_TSURI_TEST; }@saito del

  //-------------------------------------------------------------------------
  /**
  *  @brief  �ނ�(�������[�V�����e�X�g)�R�}���h
  */
  //-------------------------------------------------------------------------
  static ActionCmdBase::AcmdRet AcTsuriStart( ActionCmdBase::ActionCmdWork* p_work );
  static ActionCmdBase::AcmdRet AcTsuriWait ( ActionCmdBase::ActionCmdWork* p_work );
};

} // mmodel
} // field

#endif  // __FIELDACTIONCMDANALOGMOVE_H__

