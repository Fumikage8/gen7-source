//==============================================================================
/**
 @file    ZukanFrameMap.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の分布画面のフレーム
 */
//==============================================================================


// nijiのインクルード
#include <App/TownMapParts/include/TownMapPartsDraw.h>

// 図鑑のインクルード
#include "ZukanViewMap.h"
#include "ZukanFrameMap.h"

#include "ZukanDebugWin.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)

class ZukanTownMapListener : public App::TownMap::TownMapPartsDraw::SubScreenMapListener
{
public:
  virtual void OnAction( ActionID id )
  {
    switch(id)
    {
    case ACTION_ID_RETURN:
      {
        GFL_PRINT("SubScreenMapListener RETURN\n");
        if(m_view) m_view->SetInputEnabledAllView(false);
        if(m_zukanViewMap)
        {
          m_zukanViewMap->SetInputEnabledAllView(false);
          m_zukanViewMap->ReqEnd();
        }
        break;
      }
    case ACTION_ID_SELECT:
      {
        GFL_PRINT("SubScreenMapListener SELECT\n");
        break;
      }
    case ACTION_ID_ZOOM:
      {
        // @fix NMCat[2109] 島を切り替えた直後、エンカウントマークが一瞬異なる位置に表示される  →TownMapPartsDrawがアニメ中はマークを表示しないようにする。
        // アクション開始時(ACTION_ID_ZOOM, ACTION_ID_LAND_CHANGE)に、次のアイコンの設定をしたデータをSetExIconDataに渡す。TownMapPartsDrawは今の表示を消し、次の設定を保持してくれる。
        // アクション終了時(ACTION_ID_ZOOM_END, ACTION_ID_LAND_CHANGE_END)は何もしなくてよい。TownMapPartsDrawが保持していた設定で表示してくれる(「ResetIcon,ResetIconPosition」相当の内容)。

        GFL_PRINT("SubScreenMapListener ZOOM\n");

        bool nextIsZoom = false;
        if(m_view)  // @fix cov_ctr[10184]: NULLチェック追加
        {
          nextIsZoom = !(m_view->IsZoom());  // ACTION_ID_ZOOM_ENDしたときの値は、今取得できる値の反対の値。
        }
        if(m_zukanViewMap)
        {
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
          s32 mapIconCheckMode = 0;
#if PM_DEBUG
          mapIconCheckMode = ZukanDebugWin::GetMapIconCheckMode();
#endif
          m_zukanViewMap->SetTownMapPartsDrawIconData(nextIsZoom, mapIconCheckMode);
#else
          m_zukanViewMap->SetTownMapPartsDrawIconData(nextIsZoom);
#endif
          m_zukanViewMap->SetIslandButtonVisible(false);  // ボタンの表示をONにするのはACTION_ID_ZOOM_ENDで行う
          m_zukanViewMap->SetInputEnabledAllView(false);
        }
        break;
      }
    case ACTION_ID_ZOOM_END:
      {
        GFL_PRINT("SubScreenMapListener ZOOM_END\n");
        
        bool isZoom = false;
        if(m_view)  // @fix cov_ctr[10184]: NULLチェック追加
        {
          isZoom = m_view->IsZoom();
        } 
        if(m_zukanViewMap)
        {
          m_zukanViewMap->SetIslandButtonVisible(!isZoom);  // 縮小マップなら、ボタンの表示をONにする
          m_zukanViewMap->SetInputEnabledAllView(true);
        }
        break;
      }
    case ACTION_ID_LAND_CHANGE:
      {
        // m_zukanViewMap->SetTownMapPartsDrawIconDataは、島変更命令ChangeLandNoを出したときにm_zukanViewMap内から呼ばれる。

        GFL_PRINT("SubScreenMapListener LAND_CHANGE\n");
        if(m_zukanViewMap) m_zukanViewMap->SetInputEnabledAllView(false);
        break;
      }
    case ACTION_ID_LAND_CHANGE_END:
      {
        GFL_PRINT("SubScreenMapListener LAND_CHANGE_END\n");
        if(m_zukanViewMap) m_zukanViewMap->SetInputEnabledAllView(true);
        break;
      }
    case ACTION_ID_SET_ICONDATA:
      {
        GFL_PRINT("SubScreenMapListener SET_ICONDATA\n");
        //if(m_zukanViewMap) m_zukanViewMap->SetTownMapPartsDrawIconData(m_view->IsZoom());  // ここは元からコメントアウトされていた
        break;
      }
    }
  }

/*
　●島切り替え

　　ChangeLandNo()で切り替え


□注意点
　●リスナー
　　動作ID"ACTION_ID_ZOOM"が渡された場合、他Viewの入力を停止、
　　動作ID"ACTION_ID_ZOOM_END"が渡された場合、他Viewの入力を再開してください。
*/

public:
  ZukanTownMapListener(void)
    : App::TownMap::TownMapPartsDraw::SubScreenMapListener(),
      m_view(NULL),
      m_zukanFrameMapCell(NULL),
      m_zukanViewMap(NULL)
  {}
  virtual ~ZukanTownMapListener()
  {}

  void SetFrameCellView(App::TownMap::TownMapPartsDraw* view, ZukanFrameMapCell* zukanFrameMapCell, ZukanViewMap* zukanViewMap)
  {
    m_view              = view;
    m_zukanFrameMapCell = zukanFrameMapCell;
    m_zukanViewMap      = zukanViewMap;
  }

private:
  App::TownMap::TownMapPartsDraw* m_view;
  ZukanFrameMapCell*              m_zukanFrameMapCell;
  ZukanViewMap*                   m_zukanViewMap;
};

GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)

ZukanFrameMapCell::ZukanFrameMapCell(void)
  : ZukanFrameSceneBaseCell(),
    m_townMapPartsDrawAppHeap(NULL),
    m_view(NULL),
    m_endReq(false),
    m_townMapListener(NULL),
    m_townMapPartsDraw(NULL)
{}

ZukanFrameMapCell::~ZukanFrameMapCell()
{}

applib::frame::Result ZukanFrameMapCell::InitFunc(void)
{
  enum
  {
    SEQ_CREATE_HEAP            = 0,
    SEQ_SETUP_VIEW                ,
    SEQ_START_TOWNMAP_CREATE      ,
    SEQ_WAIT_TOWNMAP_CREATE_1     ,
    SEQ_WAIT_TOWNMAP_CREATE_2     ,
    SEQ_WAIT_TOWNMAP_CREATE_3     ,
    SEQ_WAIT_TOWNMAP_CREATE_4     ,
    SEQ_CREATE_VIEW               ,
    SEQ_END                       ,
  };

  applib::frame::Result res = applib::frame::RES_CONTINUE;

  switch( this->GetSubSeq() )  // 親であるapplib::frame::CellSuperのメンバ
  {
  case SEQ_CREATE_HEAP:
    {
      // メモリ
      m_townMapPartsDrawAppHeap = GFL_NEW(m_param.appHeap->GetDeviceHeap()) app::util::Heap();
      m_townMapPartsDrawAppHeap->LocalHeapCreate(m_param.appHeap->GetSystemHeap(), m_param.appHeap->GetDeviceHeap(), TOWN_MAP_PARTS_DRAW_HEAP_MEM_SIZE, TOWN_MAP_PARTS_DRAW_DEV_MEM_SIZE);
      this->changeSeq(SEQ_SETUP_VIEW);
      //breakしない
    }
  case SEQ_SETUP_VIEW:
    {
      m_param.frameCell = this;
      m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
      this->changeSeq(SEQ_START_TOWNMAP_CREATE);
      //breakしない
    }
  case SEQ_START_TOWNMAP_CREATE:
    {
      m_townMapListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanTownMapListener;

      m_townMapPartsDraw = GFL_NEW(m_param.appHeap->GetDeviceHeap()) App::TownMap::TownMapPartsDraw(
        App::TownMap::TownMapPartsDraw::CALL_ZUKAN,
        m_townMapPartsDrawAppHeap,
        m_param.appRenderingManager,
        ZukanViewMap::GetFirstIslandNo(m_param.contents)  // 最初の島番号
      );
      m_townMapPartsDraw->SetMapListener(m_townMapListener); 
      this->changeSeq(SEQ_WAIT_TOWNMAP_CREATE_1);
      //breakしない
    }
  case SEQ_WAIT_TOWNMAP_CREATE_1:
    {
      if( m_townMapPartsDraw->LoadResource() )
      {
        this->changeSeq(SEQ_WAIT_TOWNMAP_CREATE_2);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_TOWNMAP_CREATE_2:
    {
      if( m_townMapPartsDraw->Init( m_townMapPartsDraw->GetLayoutResource() ) )
      {
        m_param.parentView->AddSubView(m_townMapPartsDraw);  // app::ui::UIViewのメンバ
        m_townMapPartsDraw->SetModal(true);  // app::ui::UIViewのメンバ
        this->changeSeq(SEQ_WAIT_TOWNMAP_CREATE_3);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_TOWNMAP_CREATE_3:
    {
      if( m_townMapPartsDraw->Start() )
      {
        this->changeSeq(SEQ_WAIT_TOWNMAP_CREATE_4);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_TOWNMAP_CREATE_4:
    {
      if( m_townMapPartsDraw->IsStart() )
      {
        this->changeSeq(SEQ_CREATE_VIEW);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_CREATE_VIEW:
    {
      m_view = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewMap(m_param);
      GFL_PRINT("call ZukanViewMap::SetTownMapPartsDraw\n");
      m_view->SetTownMapPartsDraw(m_townMapPartsDraw);
      
      m_townMapListener->SetFrameCellView(m_townMapPartsDraw, this, m_view);
      this->changeSeq(SEQ_END);
      //breakしない
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

applib::frame::Result ZukanFrameMapCell::UpdateFunc(void)
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
        //m_param.rootView->SetInputEnabledAllView(true);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
        // ZukanViewMapで入力可能にする
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

void                  ZukanFrameMapCell::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{}

applib::frame::Result ZukanFrameMapCell::EndFunc(void)
{
  enum
  {
    SEQ_START_TOWNMAP_DESTROY   = 0,
    SEQ_WAIT_TOWNMAP_DESTROY_1     ,
    SEQ_WAIT_TOWNMAP_DESTROY_2     ,
    SEQ_DESTROY_VIEW               ,
    SEQ_DESTROY_HEAP               ,
    SEQ_END                        ,
  };

  applib::frame::Result res = applib::frame::RES_CONTINUE;

  switch( this->GetSubSeq() )  // 親であるapplib::frame::CellSuperのメンバ
  {
  case SEQ_START_TOWNMAP_DESTROY:
    {
      m_townMapPartsDraw->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ
      this->changeSeq(SEQ_WAIT_TOWNMAP_DESTROY_1);
      //breakしない
    }
  case SEQ_WAIT_TOWNMAP_DESTROY_1:
    {
      if( m_townMapPartsDraw->End() )
      {
        this->changeSeq(SEQ_WAIT_TOWNMAP_DESTROY_2);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_TOWNMAP_DESTROY_2:
    {
      if( m_townMapPartsDraw->UnloadResource() )
      {
        GFL_SAFE_DELETE(m_townMapPartsDraw);
        GFL_SAFE_DELETE(m_townMapListener);
        this->changeSeq(SEQ_DESTROY_VIEW);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_DESTROY_VIEW:
    {
      GFL_SAFE_DELETE(m_view);
      this->changeSeq(SEQ_DESTROY_HEAP);
      //breakしない
    }
  case SEQ_DESTROY_HEAP:
    {
      GFL_SAFE_DELETE(m_townMapPartsDrawAppHeap);
      this->changeSeq(SEQ_END);
      //breakしない
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

void ZukanFrameMapCell::Req(s32 reqId)
{
  switch(reqId)
  {
  case REQ_ID_END:
    {
      // ZukanViewMapでも入力不可能にしている
      m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
      m_endReq = true;
      break;
    }
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

