//======================================================================
/**
 * @file CharaViewerProc.h
 * @date 2015/07/21 20:56:40
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined CHARA_VIEWER_PROC_H_INCLUDED
#define CHARA_VIEWER_PROC_H_INCLUDED
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

// 図鑑のインクルード
#include <Test/CharaViewer/include/CharaViewerProcParam.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)
  class CharaViewerImpl;
GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)




GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)


//==============================================================================
/**
 @class     CharaViewerProc
 @brief     CharaViewerのProc
 */
//==============================================================================
class CharaViewerProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(CharaViewerProc);

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  paramのコピーを行うか、paramのポインタを保持するだけにするか、は迷っているところ。@todo
  //!                    呼び出し元がparamを破棄していいかどうかは、上記次第。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  CharaViewerProc(void);
  //CharaViewerProc(CharaViewerProcParam* param);

  virtual ~CharaViewerProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

  void SetFieldDebugModeEnabled(b32 enabled)
  {
    m_param.fieldDebugModeEnabled = enabled;
  }

private:
  CharaViewerProcParam        m_param;
  gfl2::heap::HeapBase* m_heapMem;
  gfl2::heap::HeapBase* m_devMem;
  app::util::Heap*      m_appHeap;
  CharaViewerImpl*            m_impl;  // create判定にも使う
};


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // CHARA_VIEWER_PROC_H_INCLUDED

#endif // PM_DEBUG
