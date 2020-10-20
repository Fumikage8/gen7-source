//======================================================================
/**
 * @file	SequenceLocalDef.h
 * @brief	SequencePlayer描画ローカル定義
 * @author	Pete
 * @data	15/05/12
 */
//======================================================================

#if !defined __SEQUENCE_LOCAL_DEF_H__
#define __SEQUENCE_LOCAL_DEF_H__
#pragma once

#include <math/include/gfl2_math.h>
#include <heap/include/gfl2_Heap.h>

///#include "../btl_common.h"
#include "Demolib/Object/include/ObjectManager.h"

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

//オブジェクト管理
template<typename T> class SequenceObjectManager
{
public:
  SequenceObjectManager(gfl2::heap::HeapBase *heap,int objectNum)
  {
    mObjBufNum = objectNum;
    mObjArr = GFL_NEW_ARRAY(heap)T**[mObjBufNum];
    mObjNum = GFL_NEW_ARRAY(heap)int[mObjBufNum];
    mObjId = GFL_NEW_ARRAY(heap)int[mObjBufNum];
  }
  virtual ~SequenceObjectManager()
  {
    GFL_SAFE_DELETE_ARRAY(mObjId);
    GFL_SAFE_DELETE_ARRAY(mObjNum);
    GFL_SAFE_DELETE_ARRAY(mObjArr);
  }

  void SetObjectArr( T **objArr , int objNum , int objId )
  {
    for( int i=0;i<mObjBufNum;i++ )
    {
      if( mObjArr[i] == NULL )
      {
        mObjArr[i] = objArr;
        mObjNum[i] = objNum;
        mObjId[i] = objId;
        return;
      }
    }
  }

  T** GetObjectArr( int objId , int *num )const
  {
    const int idx = GetObjectIndex(objId);
    if( idx != -1 )
    {
      if( num != NULL )
      {
        *num = mObjNum[idx];
      }
      return mObjArr[idx];
    }

    if( num != NULL )
    {
      *num = 0;
    }
    return NULL;
  }

  void ClearBuffer(DemoLib::Obj::ObjectManager *objMng)
  {
    for( int i=0;i<mObjBufNum;i++ )
    {
      if( mObjArr[i] != NULL )
      {
        bool isEnable = false;
        for( int ii=0;ii<mObjNum[i];ii++ )
        {
          if( mObjArr[i][ii]->IsEnable() == false )
          {
            objMng->RemoveObject(mObjArr[i][ii]);
            GFL_SAFE_DELETE(mObjArr[i][ii]);
          }
          else
          {
            isEnable = true;
          }
        }
        if( isEnable == false )
        {
          GFL_SAFE_DELETE_ARRAY( mObjArr[i] );
        }
      }
    }
  }

  //エフェクトのIDに使う
  int GetObjectIndex( int objId )const
  {
    for( int i=0;i<mObjBufNum;i++ )
    {
      if( mObjArr[i] != NULL &&
        mObjId[i] == objId )
      {
        return i;
      }
    }
    return -1;
  }

private:
  //ポインタの配列を格納する配列・・・
  T*** mObjArr;
  int* mObjNum;
  int* mObjId;

  int mObjBufNum;
};

	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)


#endif //__SEQUENCE_LOCAL_DEF_H__

