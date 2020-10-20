#ifndef __TURTLESALMON_SAVE_H__
#define __TURTLESALMON_SAVE_H__
//=============================================================================================
/**
 * @file  TurtleSalmonSave.h
 * @brief TurtleSalmonのセーブインターフェイス用クラス
 */
//=============================================================================================
#pragma once
//#include <gflib.h>
#include "Savedata/include/SaveDataInterface.h"
#include "debug/include/gfl2_Assert.h"


GFL_NAMESPACE_BEGIN(Savedata)

class TurtleSalmonSave : public SaveDataInterface
{
private:
  struct CORE_DATA
  {
    u8 padding[256];
    u8 padding2[256];//niji署名領域
  };

  public:
    TurtleSalmonSave();
    ~TurtleSalmonSave();

  public:
    virtual void * GetData( void ) { return &mData; };
    virtual size_t GetDataSize( void ) { return sizeof(mData); };
    virtual void Clear( gfl2::heap::HeapBase * heap )
    {
      gfl2::std::MemClear( &mData, sizeof(mData) );
    };



  private:
    CORE_DATA mData;
};  // TurtleSalmonSave

GFL_NAMESPACE_END(Savedata)

#endif //__TURTLESALMON_SAVE_H__
