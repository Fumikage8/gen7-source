//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		GameClearSaveView.cpp
 *	@brief  ゲームクリアー時のセーブ画面描画クラス
 *	@author	Toru=Nagihashi
 *	@date		2013.01.29
 *	@author	koji_kawada@gamefreak.co.jp
 *	@date		2016.01.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "GameClearSaveView.h"
#include "GameClearSaveMessageWindowView.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <arc_def_index/arc_def.h>
#include <Sound/include/Sound.h>
#include <arc_index/message.gaix>
#include <arc_index/game_clear_save.gaix>
#include <niji_conv_header/app/game_clear_save/game_clear_save_res2d_pack.h>
#include <niji_conv_header/app/game_clear_save/game_clear_save_dummy.h>
#include <niji_conv_header/app/game_clear_save/game_clear_save_dummy_anim_list.h>
#include <niji_conv_header/app/game_clear_save/game_clear_save_dummy_pane.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(game_clear_save)

//---------------------------------------------------------------------
// クラス
//---------------------------------------------------------------------
class MessageWindowViewEventHandler : public App::Tool::CommonMessageWindowView::IEventHandler
{
public:
  virtual void CommonMessageWindowView_OnEvent(const App::Tool::CommonMessageWindowView::IEventHandler::EventCode event)
  {
    switch(event)
    {
    case App::Tool::CommonMessageWindowView::IEventHandler::EVENT_OnEOM:
      {
        if(m_messageType == View::MESSAGE_TYPE_TIMER_ICON)
        {
          m_view->RequestFinishMessage();
        }
        break;
      }
    case App::Tool::CommonMessageWindowView::IEventHandler::EVENT_OnFinish:
      {
        if(m_messageType != View::MESSAGE_TYPE_TIMER_ICON)
        {
          m_view->RequestFinishMessage();
        }
        break;
      }
    }
  }
public:
  MessageWindowViewEventHandler(View* view)
    : App::Tool::CommonMessageWindowView::IEventHandler(),
      m_view(view),
      m_messageType(View::MESSAGE_TYPE_WAIT)
  {}
  virtual ~MessageWindowViewEventHandler()
  {}
  void SetMessageType(View::MessageType messageType)
  {
    m_messageType = messageType;
  }
private:
  View*             m_view;
  View::MessageType m_messageType;
};


//---------------------------------------------------------------------
// 定数
//---------------------------------------------------------------------
enum
{
  CREATE_STATE_NONE                           = 0,
  CREATE_STATE_APP_RENDERING_MANAGER_CREATING    ,
  CREATE_STATE_DATA_LOADING                      ,
  CREATE_STATE_LAYOUT_CREATING                   ,
  CREATE_STATE_MESSAGE_WINDOW_CREATING           ,
  CREATE_STATE_CREATED                           ,
  CREATE_STATE_WATE_NOT_DRAW                     ,
  CREATE_STATE_DESTROYING                        ,
  CREATE_STATE_DESTROYED                         ,
};


//---------------------------------------------------------------------
// コンストラクタ・デストラクタ
//---------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param heap  ヒープ
 * @param file_read_manager    ファイル読み込みシステム
 */
View::View( app::util::Heap * heap, gfl2::fs::AsyncFileManager *file_manager ) :
  app::ui::UIView( heap ),
  m_message_type(MESSAGE_TYPE_WAIT),  // @fix cov_ctr[10792]: 未初期化を初期化
  m_is_message_end(true),
  m_appHeap(heap),
  m_fileManager(file_manager),
  m_createState(0),
  m_appRenderingManager(NULL),
  m_binLinker(),
  m_dataBuf(NULL),
  m_appLytBuf(NULL),
  m_menuWindowBuf(NULL),
  m_messageWindowView(NULL),
  m_messageWindowViewEventHandler(NULL),
  m_requestMessageEnd(false)
{}
/**
 * @brief デストラクタ
 */
View::~View()
{}
/**
 * @brief 初期化処理
 *
 * @return true 初期化処理完了 false  初期化処理中
 */
bool View::Initialize( void )
{
  bool ret = false;

  switch(m_createState)
  {
  case CREATE_STATE_NONE:
    {
      // AppRenderingManager生成開始
      app::util::AppRenderingManager::Description desc;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      m_appRenderingManager = GFL_NEW(m_appHeap->GetDeviceHeap()) app::util::AppRenderingManager();
      m_appRenderingManager->StartAsyncCreate(
          desc,
          m_appHeap,
          NULL,
          m_fileManager
      );
      m_createState = CREATE_STATE_APP_RENDERING_MANAGER_CREATING;
      //breakしない
    }
  case CREATE_STATE_APP_RENDERING_MANAGER_CREATING:
    {
      if(m_appRenderingManager->IsCreated())
      {
        // Layout生成開始
        app::util::FileAccessor::FileOpen( ARCID_GAME_CLEAR_SAVE, m_appHeap->GetDeviceHeap(), true );
        app::util::FileAccessor::FileLoad( ARCID_GAME_CLEAR_SAVE, GARC_game_clear_save_game_clear_save_res2d_pack_pack_COMP, &m_dataBuf, m_appHeap->GetDeviceHeap(), true, 128 );
        app::util::FileAccessor::FileClose( ARCID_GAME_CLEAR_SAVE, m_appHeap->GetDeviceHeap() );
        m_createState = CREATE_STATE_DATA_LOADING;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case CREATE_STATE_DATA_LOADING:
    {
      if(
             app::util::FileAccessor::IsFileOpen( ARCID_GAME_CLEAR_SAVE )
          && app::util::FileAccessor::IsFileLoad( &m_dataBuf )
          && app::util::FileAccessor::IsFileClose( ARCID_GAME_CLEAR_SAVE )
      )
      {
        m_binLinker.Initialize(m_dataBuf);
        m_appLytBuf     = m_binLinker.GetData(BL_IDX_GAME_CLEAR_SAVE_RES2D_PACK_GAME_CLEAR_SAVE_DUMMY_APPLYT);
        m_menuWindowBuf = m_binLinker.GetData(BL_IDX_GAME_CLEAR_SAVE_RES2D_PACK_GAME_CLEAR_SAVE_MENU_WINDOW_APPLYT);
        m_createState = CREATE_STATE_LAYOUT_CREATING;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case CREATE_STATE_LAYOUT_CREATING:
    {
      this->CreateLayout(m_appHeap);
      m_createState = CREATE_STATE_MESSAGE_WINDOW_CREATING;
      //breakしない
    }
  case CREATE_STATE_MESSAGE_WINDOW_CREATING:
    {
      m_messageWindowViewEventHandler = GFL_NEW(m_appHeap->GetDeviceHeap()) MessageWindowViewEventHandler(this);
      
      App::Tool::CommonMessageWindowView::PARAM param( m_appHeap, m_appRenderingManager );
      m_messageWindowView = GFL_NEW(m_appHeap->GetDeviceHeap()) MessageWindowView(param, m_messageWindowViewEventHandler);
      UIView::AddSubView(m_messageWindowView);
      m_messageWindowView->Setup(m_menuWindowBuf);

      m_createState = CREATE_STATE_CREATED;
      //breakしない
    }
  case CREATE_STATE_CREATED:
  default:
    {
      ret = true;
      break;
    }
  };

  if(m_appRenderingManager)
  {
    m_appRenderingManager->Update();
  }

  return ret;
}
/**
 * @brief 解放処理
 *
 * @return true 解放処理完了 false  開放処理中
 */
bool View::Finalize( void )
{
  bool ret = false;

  switch(m_createState)
  {
  case CREATE_STATE_CREATED      :
    {
      m_createState = CREATE_STATE_WATE_NOT_DRAW;
      break;
    }
  case CREATE_STATE_WATE_NOT_DRAW:
    {
      m_createState = CREATE_STATE_DESTROYING;
      break;
    }
  case CREATE_STATE_DESTROYING   :
    {
      m_messageWindowView->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ
      GFL_SAFE_DELETE(m_messageWindowView);
      GFL_SAFE_DELETE(m_messageWindowViewEventHandler);
      
      this->DeleteLayout();

      m_menuWindowBuf = NULL;
      m_appLytBuf     = NULL; 
      GflHeapSafeFreeMemory(m_dataBuf);

      if(m_appRenderingManager->CanDestroy())
      {
        GFL_SAFE_DELETE(m_appRenderingManager); 
        m_createState = CREATE_STATE_DESTROYED;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case CREATE_STATE_DESTROYED    :
  default:
    {
      ret = true;
      break;
    }
  }

  if(m_appRenderingManager)
  {
    m_appRenderingManager->Update();
  }

  return ret;
}
//---------------------------------------------------------------------
// システム
//---------------------------------------------------------------------
/**
 * @brief 更新
 */
void View::Update( void )
{
  app::ui::UIView::Update();
  this->UpdateMessage();


  if(m_appRenderingManager)
  {
    m_appRenderingManager->Update();
  }
}
/**
 * @brief 描画
 */
void View::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if(m_createState == CREATE_STATE_CREATED)
  {
    if(m_appRenderingManager)
    {
      app::util::G2DUtil* l_g2dUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
      l_g2dUtil->AddDrawableLytWkForOneFrame( m_appRenderingManager, displayNo, 0 );  // wk_0
    }
  }
}
//---------------------------------------------------------------------
// メッセージ
//---------------------------------------------------------------------
/**
 * @brief 上画面ウィンドウへメッセージ表示
 *
 * @param message_id  メッセージ
 * @param message_type  メッセージタイプ
 */
void View::StartMessage( u32 message_id, MessageType message_type )
{
  app::util::G2DUtil* l_g2dUtil = this->GetG2DUtil();  // app::ui::UIViewのメンバ

  m_message_type  = message_type;
  m_messageWindowViewEventHandler->SetMessageType(message_type);
  
  //文字列をセット
  gfl2::str::StrBuf * buf = l_g2dUtil->GetTempStrBuf( 1 );
  l_g2dUtil->GetStringExpand( buf, message_id );

  switch( message_type )
  {
  case MESSAGE_TYPE_WAIT:
    m_messageWindowView->SetFinishMode_Timer(60);
    m_messageWindowView->SetVisible( true );
    m_messageWindowView->ShowMessage( buf );
    break;
  case MESSAGE_TYPE_USER_INPUT:
    m_messageWindowView->SetFinishMode_UserInput();
    m_messageWindowView->SetVisible( true );
    m_messageWindowView->ShowMessage( buf );
    break;
  case MESSAGE_TYPE_TIMER_ICON:
    m_messageWindowView->SetFinishMode_None();
    m_messageWindowView->SetVisible( true );
    m_messageWindowView->ShowMessage( buf, false, true );
    break;
  }
  
  m_is_message_end  = false;
}
/**
 * @brief メッセージ送り検知
 *
 * @return trueメッセージを送って良い falseよくない
 */
bool View::IsFinishMessage( void ) const
{
  return m_is_message_end;
}

/**
 * @brief メッセージ終了をリクエストする
 * @note friendからアクセスする
 */
void View::RequestFinishMessage( void )
{
  m_requestMessageEnd = true;
}


//---------------------------------------------------------------------
// PRIVATE  生成
//---------------------------------------------------------------------
/**
 * @brief レイアウト作成
 *
 * @param heap  ヒープ
 * @param layout_system レイアウトシステム
 */
void View::CreateLayout( app::util::Heap * heap )
{
  if(m_appLytBuf)
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ

    const app::util::G2DUtil::LytResData resTbl[] =
    {
      // res_0
      {
        m_appLytBuf,
        0,
        app::util::G2DUtil::ATTACH_ROOT
      },
    };
    
    const app::util::G2DUtil::LytwkData wkTbl[] =
    {
      // wk_0
      {
        // 上画面
        0,  // res_0
        LYTRES_GAME_CLEAR_SAVE_DUMMY_GAME_CLEAR_SAVE_DUMMY_BFLYT,
        LA_GAME_CLEAR_SAVE_DUMMY_GAME_CLEAR_SAVE_DUMMY___NUM,
        m_appLytBuf,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };

    this->Create2D(  // 親であるapp::ui::UIViewのメンバ
        m_appHeap,
        NULL,
        1,
        resTbl,
        GFL_NELEMS(resTbl), 
        wkTbl,
        GFL_NELEMS(wkTbl), 
        GARC_message_report_DAT
    );
  }
}

/**
 * @brief レイアウト破棄
 */
void View::DeleteLayout( void )
{
  if(m_appLytBuf)
  {
    this->Delete2D();  // 親であるapp::ui::UIViewのメンバ
  }
}

/**
 * @brief メッセージ待ち
 *
 */
void View::UpdateMessage( void )
{
  if( m_is_message_end == false )
  {
    if( m_requestMessageEnd )
    {
      m_requestMessageEnd = false;
      
      switch( m_message_type )
      {
      case MESSAGE_TYPE_WAIT:
        {
          m_is_message_end  = true;
        }
        break;
      case MESSAGE_TYPE_USER_INPUT:
        {
          // xy_snd::PlaySE( SEQ_SE_MESSAGE );  //増田さん要望SEなし
          m_is_message_end  = true;
        }
        break;
      case MESSAGE_TYPE_TIMER_ICON:
        {
          m_is_message_end  = true;
        }
        break;
      }
    }
  }
}

GFL_NAMESPACE_END(game_clear_save)
GFL_NAMESPACE_END(app)

