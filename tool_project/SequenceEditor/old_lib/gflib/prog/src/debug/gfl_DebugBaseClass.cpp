//=============================================================================
/**
 * @file	  gfl_DebugBaseClass.cpp
 * @brief	  デバッグ用基底クラス
            デバッグの為にクラス名を文字列で管理します
 * @author  k.ohno
 * @date	  2011.3.6
 */
//=============================================================================


#if GFL_DEBUG

#include "gfl_Base.h"
#include "gfl_Heap.h"
#include "debug/gfl_DebugBaseClassManager.h"
#include "debug/gfl_DebugBaseClass.h"

namespace gfl{
namespace debug{



//----------------------------------------------------------------------------
/**
 *	@brief    クラス名を設定する
 *  @param    name クラス名
 */
//-----------------------------------------------------------------------------
void DebugBaseClass::AddClass(const char* name)
{
  DebugBaseClassManager::AddClass(name, this);
}



//----------------------------------------------------------------------------
/**
 *	@brief    クラス名を得る
 *  @return   クラス名
 */
//-----------------------------------------------------------------------------
const char* DebugBaseClass::GetClassName(void)
{
  return DebugBaseClassManager::GetClassName(this);
}


//----------------------------------------------------------------------------
/**
 *	@brief    クラスを廃棄する
 *  @param    name クラス名
 */
//-----------------------------------------------------------------------------
void DebugBaseClass::DeleteClass(void)
{
  DebugBaseClassManager::RemoveClass(this);
}


} //debug
} //gfl

#endif //GFL_DEBUG
