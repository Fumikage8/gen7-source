//=============================================================================
/**
 * @file    TrainerMessageTest.h
 * @brief   トレーナーデータ関連のテスト
 * @author  obata_toshihiro
 * @date    2015.05.20
 */
//=============================================================================
#if PM_DEBUG
#ifndef TRAINERMESSAGETEST_H_INCLUDED
#define TRAINERMESSAGETEST_H_INCLUDED
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

#include "ObataTestBase.h"

#include <niji_conv_header/trainer/trid_def.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )


//=============================================================================
/**
 * @class  TrainerMessageTestProc
 * @brief  トレーナーデータ関連のテストプロセス
 */
//=============================================================================
class TrainerMessageTestProc : 
  public GameSys::GameProc, 
  public ObataTestBase
{
  GFL_FORBID_COPY_AND_ASSIGN( TrainerMessageTestProc );

public:
  TrainerMessageTestProc( void );
  virtual ~TrainerMessageTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* procManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* procManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* procManager );
  virtual void DrawFunc( gfl2::proc::Manager* procManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* procManager );


private:
  void UpdateTrainerId( void );
  void DumpTrainerMessage( trainer::TrainerID trainerId ) const;

private:
  s32 m_trainerId;
};


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )


#endif // TRAINERMESSAGETEST_H_INCLUDED
#endif // PM_DEBUG

