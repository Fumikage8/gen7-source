#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMESCENETEMPLATE_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMESCENETEMPLATE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameSceneTemplate.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑のシーンのフレームのテンプレート
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDataManager.h"
#include "ZukanFrameSceneBase.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameSceneTemplateCell
 @brief   図鑑のシーンのフレームのセルのテンプレート
 */
//==============================================================================
template<typename T_VIEW_SCENE>
class ZukanFrameSceneTemplateCell : public ZukanFrameSceneBaseCell
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanFrameSceneTemplateCell);

public:
  ZukanFrameSceneTemplateCell(void)
    : ZukanFrameSceneBaseCell(),
      m_view(NULL),
      m_endReq(false)
  {}

  virtual ~ZukanFrameSceneTemplateCell()
  {}

  virtual applib::frame::Result InitFunc(void)
  {
    m_param.frameCell = this;
    m_view = GFL_NEW(m_param.appHeap->GetDeviceHeap()) T_VIEW_SCENE(m_param);
    m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
    return applib::frame::RES_FINISH;
  }


  virtual applib::frame::Result UpdateFunc(void)
  {
    enum
    {
      SEQ_WAIT_START         = 0,
      SEQ_WAIT_END_REQ          ,
      SEQ_WAIT_END              ,
      SEQ_END                   ,
    };
 
    applib::frame::Result res = applib::frame::RES_CONTINUE;
 
    switch( this->GetSubSeq() )  // 親であるapplib::frame::CellSuperのメンバ
    {
    case SEQ_WAIT_START:
      {
        ++m_seqCount;
        if(m_seqCount >= 2)  // T_VIEW_SCENEのアニメの初期値が反映されるまで待つ
        {
          m_param.rootView->SetInputEnabledAllView(true);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
          this->changeSeq(SEQ_WAIT_END_REQ);
        }
        break;
      }
    case SEQ_WAIT_END_REQ:
      {
        if(m_endReq)
        {
          this->changeSeq(SEQ_WAIT_END);
        }
        break;
      }
    case SEQ_WAIT_END:
      {
        if(m_seqCount == 0)
        {
          m_view->SetVisible(false);
          ++m_seqCount;
          break;
        }
        else
        {
          this->changeSeq(SEQ_END);
          //breakしない
        }
      }
    case SEQ_END:
    default:
      {
        res = applib::frame::RES_FINISH;
        break;
      }
    }
 
    return res;
  }

  virtual void                  DrawFunc(gfl2::gfx::CtrDisplayNo no)
  {}

  virtual applib::frame::Result EndFunc(void)
  {
    GFL_SAFE_DELETE(m_view);
    return applib::frame::RES_FINISH;
  }

public:
  virtual void Req(s32 reqId)
  {
    switch(reqId)
    {
    case REQ_ID_END:
      {
        m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
        m_endReq = true;
        break;
      }
    }
  }

public:
  // Req関数に渡すリクエストID
  enum
  {
    REQ_ID_END    = 0,
    REQ_ID_MAX          // 派生クラスのリクエストIDはREQ_ID_MAX以上の値にして下さい。
  };

protected:
  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // ビュー
  T_VIEW_SCENE*     m_view;
  bool              m_endReq;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMESCENETEMPLATE_H_INCLUDED

