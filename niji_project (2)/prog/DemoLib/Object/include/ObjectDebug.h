//======================================================================
/**
 * @file	Object_debug.h
 * @brief	オブジェクトデバッグ用
 * @author	ariizumi
 * @data	15/03/16
 */
//======================================================================
#if !defined( __OBJECT_DEUBG_H__ )
#define __OBJECT_DEUBG_H__
#pragma once

#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <debug/include/gfl2_DebugWinSystem.h>

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

class Object;

gfl2::debug::DebugWinGroup* AddDebugWin_Object(gfl2::heap::HeapBase *heap ,gfl2::debug::DebugWinGroup* parent,Object *obj,wchar_t *name);

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)

#endif //PM_DEBUG
#endif //__OBJECT_DEUBG_H__