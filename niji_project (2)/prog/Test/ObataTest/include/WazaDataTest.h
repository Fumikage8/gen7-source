//=============================================================================
/**
 * @file    WazaDataTest.h
 * @brief   技データ関連のテスト
 * @author  obata_toshihiro
 * @date    2015.05.15
 */
//=============================================================================
#if PM_DEBUG
#ifndef WAZADATATEST_H_INCLUDED
#define WAZADATATEST_H_INCLUDED
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

#include <niji_conv_header/poke_lib/wazano_def.h>
#include "ObataTestBase.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )


//=============================================================================
/**
 * @class  WazaDataTestProc
 * @brief  技データ関連のテストプロセス
 */
//=============================================================================
class WazaDataTestProc : 
  public GameSys::GameProc, 
  public ObataTestBase
{
  GFL_FORBID_COPY_AND_ASSIGN( WazaDataTestProc );

public:
  WazaDataTestProc( void );
  virtual ~WazaDataTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* procManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* procManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* procManager );
  virtual void DrawFunc( gfl2::proc::Manager* procManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* procManager );


private:
  void UpdateWazaNo( void );
  void DumpWazaData( WazaNo wazano ) const;

private:
  s32 m_wazano;
};


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )


#endif // WAZADATATEST_H_INCLUDED
#endif // PM_DEBUG

