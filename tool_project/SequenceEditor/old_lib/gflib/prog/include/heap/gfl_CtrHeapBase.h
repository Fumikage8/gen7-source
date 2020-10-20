#ifndef __GFL_CTRHEAPBASE_H__
#define __GFL_CTRHEAPBASE_H__
//======================================================================
/**
 * @file	gfl_CtrHeapBase.h
 * @brief	3DS�q�[�v���N���X
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#pragma once

#include <nn.h>

#include <gfl_Macros.h>
#include <heap/gfl_HeapBase.h>

namespace gfl
{
namespace heap
{

//#pragma mark [> CtrHeapBase
class CtrHeapBase:public GflHeapBase
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(CtrHeapBase);
  
public:
  CtrHeapBase(void){};
  virtual ~CtrHeapBase(){};

  //�K�v�ł���Ύ��� �K�v�ȕ��͗L��܂ŁB
  virtual void SetGroupId( const bit8 id ){GFL_UNUSED(id);};
  virtual bit8 GetGroupId(){return 0;};

  virtual int GetDefaultAlign(){return nn::fnd::HeapBase::DEFAULT_ALIGNMENT;};

};


} //namespace heap
} //namespace gfl
#endif // __GFL_CTRHEAPBASE_H__
