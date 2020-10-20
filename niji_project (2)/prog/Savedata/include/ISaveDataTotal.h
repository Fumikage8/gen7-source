#ifndef __ISAVEDATATOTAL_H__
#define __ISAVEDATATOTAL_H__
//===================================================================
/**
 * @file    ISaveDataTotal.h
 * @brief   セーブデータ全体の仮想クラス
 * @author  tamada
 * @date    2012.09.22
 * @note    セーブデータ全体のインターフェイス定義を定義する
 */
//===================================================================
#pragma once

//#include <gflib.h>

GFL_NAMESPACE_BEGIN(Savedata)

  class SaveDataInterface;

//-----------------------------------------------------------------------------
/**
 * @brief セーブデータ全体のインターフェイスクラス
 * class ISaveDataTotal
 */
//-----------------------------------------------------------------------------
class ISaveDataTotal
{
  GFL_FORBID_COPY_AND_ASSIGN(ISaveDataTotal); //コピーコンストラクタ＋代入禁止

public:
  enum State {
    STATE_NULL, ///<データが存在しない（初期状態）
    STATE_OK,   ///<正常
    STATE_NG,   ///<異常、使用できない
  };

  ISaveDataTotal(void){};
  virtual ~ISaveDataTotal(){};

  virtual int GetSaveObjectNum( void ) const = 0;
  virtual SaveDataInterface * GetSaveObject( u32 class_id ) = 0;
  virtual size_t GetAlignmentSize( void ) const = 0;

  virtual void Clear( gfl2::heap::HeapBase * heap ) = 0;
  virtual State GetStatus( s32 * error_info = NULL ) const = 0;
  virtual void SetupCheckData( void ) = 0;
  virtual void SetupNullData( void ) = 0;
  virtual void LockData( void ) = 0;
  virtual void UnlockData( void ) = 0;

};

GFL_NAMESPACE_END(Savedata)

#endif  // __ISAVEDATATOTAL_H__

