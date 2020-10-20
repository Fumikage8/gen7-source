//=============================================================================
/**
 * @file    PersonalDataTest.h
 * @brief   パーソナルデータ関連のテスト
 * @author  obata_toshihiro
 * @date    2015.05.11
 */
//=============================================================================
#if PM_DEBUG
#ifndef PERSONALDATATEST_H_INCLUDED
#define PERSONALDATATEST_H_INCLUDED
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
 * @class  PersonalDataTestProc
 * @brief  パーソナルデータ関連のテストプロセス
 */
//=============================================================================
class PersonalDataTestProc : 
  public GameSys::GameProc,
  public ObataTestBase
{
  GFL_FORBID_COPY_AND_ASSIGN( PersonalDataTestProc );

public:
  PersonalDataTestProc( void );
  virtual ~PersonalDataTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* procManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* procManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* procManager );
  virtual void DrawFunc( gfl2::proc::Manager* procManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* procManager );


private:
  void UpdateMonsNo( void );
  void DumpPersonalData( s32 monsno, s32 formno ) const;
  void DumpWazaOboeData( s32 monsno, s32 formno ) const;
  void DumpEvolveData( s32 monsno, s32 formno ) const;
  void DumpMegaEvolveData( s32 monsno ) const;

private:
  s32 m_monsno;
  s32 m_formno;
};


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )


#endif // PERSONALDATATEST_H_INCLUDED
#endif // PM_DEBUG

