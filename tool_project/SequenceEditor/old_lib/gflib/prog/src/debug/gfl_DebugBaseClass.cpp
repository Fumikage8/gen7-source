//=============================================================================
/**
 * @file	  gfl_DebugBaseClass.cpp
 * @brief	  �f�o�b�O�p���N���X
            �f�o�b�O�ׂ̈ɃN���X���𕶎���ŊǗ����܂�
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
 *	@brief    �N���X����ݒ肷��
 *  @param    name �N���X��
 */
//-----------------------------------------------------------------------------
void DebugBaseClass::AddClass(const char* name)
{
  DebugBaseClassManager::AddClass(name, this);
}



//----------------------------------------------------------------------------
/**
 *	@brief    �N���X���𓾂�
 *  @return   �N���X��
 */
//-----------------------------------------------------------------------------
const char* DebugBaseClass::GetClassName(void)
{
  return DebugBaseClassManager::GetClassName(this);
}


//----------------------------------------------------------------------------
/**
 *	@brief    �N���X��p������
 *  @param    name �N���X��
 */
//-----------------------------------------------------------------------------
void DebugBaseClass::DeleteClass(void)
{
  DebugBaseClassManager::RemoveClass(this);
}


} //debug
} //gfl

#endif //GFL_DEBUG
