
/**
 * @file FieldDebugFlagWorkSearchProc.h
 * @date 2015/06/11
 * @author ikuta_junya 
 * @brief FieldDebugFlagWorkSearchProcヘッダーファイル
 */

#if (PM_DEBUG)

#ifndef	__FIELDDEBUGFLAGWORKSEARCH_PROC_H__
#define	__FIELDDEBUGFLAGWORKSEARCH_PROC_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>


#include <GameSys/include/GameProc.h>
#include <System/include/GflUse.h>
#include <system/include/nijiAllocator.h>

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

#include <model/include/gfl2_DressUpModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include <str/include/gfl2_Str.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)


/**
 * キャラクターを順次着せ替えてスクリーンショットを撮影する
 */
class FieldDebugFlagWorkSearchProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldDebugFlagWorkSearchProc); // forbid copy and assign

public:
  
  struct Param
  {
    void *userWork;
    gfl2::debug::DebugWinItem *item;
  };

  /**
   * @brief constructor
   * 
   */
  FieldDebugFlagWorkSearchProc();

  /**
   * @brief destructor
   *
   */
  virtual ~FieldDebugFlagWorkSearchProc();

  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  virtual void PreDrawFunc(gfl2::proc::Manager* pManager);
  virtual void DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo);
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
  virtual GameSys::FrameMode GetFrameMode(void) const {return GameSys::FRAMEMODE_30;}
  void SetParam( const Param& rParam ){ m_Param = rParam; }

private:
  gfl2::heap::HeapBase* m_pHeap;
  System::nijiAllocator* m_pAllocator;

  u32 m_InitSeq;
  u32 m_UpdateSeq;
  u32 m_EndSeq;

  Param              m_Param;
  gfl2::str::StrBuf* m_pSearchWord;

private:
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif	// __FIELDDEBUGFLAGWORKSEARCH_PROC_H__

#endif // PM_DEBUG
