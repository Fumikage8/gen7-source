//==============================================================================
/**
 * @file  PawnObject.h
 * @brief 
 * @author  Miyuki Iwasawa
 * @data  2015/02/26
 */
// =============================================================================
#ifndef __PAWNOBJECT_H__
#define __PAWNOBJECT_H__
#pragma once

#include "pawn/include/gfl2_Pawn.h"

GFL_NAMESPACE_BEGIN( PawnScript )

//------------------------------------------------------------------------------
/**
 * @class       PawnObject
 * @brief       
 */
//------------------------------------------------------------------------------
class PawnObject : public gfl2::pawn::Pawn
{
public:
  static cell Random( AMX *, const cell * );
  
  PawnObject(gfl2::heap::HeapBase* heap=0, gfl2::heap::HeapBase* heap_debug=0) : gfl2::pawn::Pawn(heap, heap_debug) {}
  virtual ~PawnObject(){}

  virtual void Load( const void* buffer, const size_t size, const c8* const amxname="" );
};

GFL_NAMESPACE_END( PawnScript )

#endif  /* __PAWNOBJECT_H__ */

