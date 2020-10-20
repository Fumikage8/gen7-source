#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANPROC_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANPROC_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanProc.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑のProc
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanImpl;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanProc
 @brief     図鑑のProc
 */
//==============================================================================
class ZukanProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanProc);

public:
  //! @brief  コンストラクタ
  //! @param[in,out]  param  引数。入力、出力のやりとりをする。
  //!                        本クラス内部ではparamのポインタを保持するだけなので、呼び出し元はparamを破棄しないようにして下さい。
  ZukanProc(ZukanProcParam* param);

  virtual ~ZukanProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

private:
  ZukanProcParam*       m_param;
  gfl2::heap::HeapBase* m_heapMem;
  gfl2::heap::HeapBase* m_devMem;
  app::util::Heap*      m_appHeap;
  ZukanImpl*            m_impl;  // create判定にも使う
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPROC_H_INCLUDED

