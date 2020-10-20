#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEW_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEW_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanView.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanView
 @brief     図鑑のビュー
 */
//==============================================================================
class ZukanView : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanView);

public:
  ZukanView(app::util::Heap* appHeap);
  
  virtual ~ZukanView();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEW_H_INCLUDED

