//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file    ActionCmdTsuriTest.cpp
 *  @brief  フィールドアクションコマンド釣りテスト
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

  // コンストラクタ
  ActionCmdTsuriTest();

  //----------------------------------------------------------------------------
	/**
	*	@brief  対応するアクションコマンドを返す
	*/
	//-----------------------------------------------------------------------------
  ///virtual Acmd GetAcmdID( void ) const{ return AC_TSURI_TEST; }@saito del

  //-------------------------------------------------------------------------
  /**
  *  @brief  釣り(同期モーションテスト)コマンド
  */
  //-------------------------------------------------------------------------
  static ActionCmdBase::AcmdRet AcTsuriStart( ActionCmdBase::ActionCmdWork* p_work );
  static ActionCmdBase::AcmdRet AcTsuriWait ( ActionCmdBase::ActionCmdWork* p_work );
};

} // mmodel
} // field

#endif  // __FIELDACTIONCMDANALOGMOVE_H__

