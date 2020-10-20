// ============================================================================
/*
 * @file SaveTestProc.h
 * @brief GameDataのセーブテストです。
 * @date 2015.10.02
 * @author Akira endo
 */
// ============================================================================
#if PM_DEBUG

#if !defined( SAVETESTPROC_H_INCLUDED )
#define SAVETESTPROC_H_INCLUDED
#pragma once


#include <macro/include/gfl2_macros.h>
#include <GameSys/include/GameProc.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(SaveTest)


class SaveTestProc : public GameSys::GameProc
{
	GFL_FORBID_COPY_AND_ASSIGN( SaveTestProc );

public:
  SaveTestProc();
  virtual~SaveTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

private:

  bool  m_bLoadError;



};


GFL_NAMESPACE_END(SaveTest)
GFL_NAMESPACE_END(Test)

#endif // SAVETESTPROC_H_INCLUDED

#endif // PM_DEBUG
