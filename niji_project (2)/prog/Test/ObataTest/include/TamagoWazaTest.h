//=============================================================================
/**
 * @file    TamagoWazaTest.h
 * @brief   タマゴ技関連のテスト
 * @author  obata_toshihiro
 * @date    2015.05.11
 */
//=============================================================================
#if PM_DEBUG
#ifndef TAMAGOWAZATEST_H_INCLUDED
#define TAMAGOWAZATEST_H_INCLUDED
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

#include "ObataTestBase.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )


//=============================================================================
/**
 * @class  TamagoWazaTestProc
 * @brief  パーソナルデータ関連のテストプロセス
 */
//=============================================================================
class TamagoWazaTestProc : 
  public GameSys::GameProc,
  public ObataTestBase
{
  GFL_FORBID_COPY_AND_ASSIGN( TamagoWazaTestProc );

public:
  TamagoWazaTestProc( void );
  virtual ~TamagoWazaTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* procManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* procManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* procManager );
  virtual void DrawFunc( gfl2::proc::Manager* procManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* procManager );
};


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )


#endif // TAMAGOWAZATEST_H_INCLUDED
#endif // PM_DEBUG

