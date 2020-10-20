
#ifndef __XY_EXTSAVESYSTEMLISTENER_H__
#define __XY_EXTSAVESYSTEMLISTENER_H__
//==============================================================================
/**
 * @file    NijiExtSaveSystemListener.h
 */
//==============================================================================
#pragma once
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(ExtSavedata)


//--------------------------------------------------------------
/**
 * @class NijiExtSaveSystem
 */
//--------------------------------------------------------------
class NijiExtSaveSystemListener{
  GFL_FORBID_COPY_AND_ASSIGN(NijiExtSaveSystemListener); //コピーコンストラクタ＋代入禁止

public:
  NijiExtSaveSystemListener(){};
  virtual ~NijiExtSaveSystemListener(){};

  //拡張セーブのフェイタルエラー
  virtual void OnExtSaveFatalError( void ) = 0;
  
}; //class NijiExtSaveSystem

GFL_NAMESPACE_END(ExtSavedata)

#endif //__XY_EXTSAVESYSTEMLISTENER_H__
