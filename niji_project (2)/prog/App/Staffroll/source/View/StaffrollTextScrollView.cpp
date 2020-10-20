// ============================================================================
/*
 * @file StaffrollTextScrollView.h
 * @brief スタッフロールアプリのテキストスクロール
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/source/View/StaffrollTextScrollView.h"
#include "App/Staffroll/source/StaffrollResourceID.h"
#include "App/Staffroll/source/System/StaffrollWork.h"
#include "App/Staffroll/source/StaffrollTextBox.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "GameSys/include/GameManager.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/app/staffroll/res2d/StaffrollLyt.h>
#include <niji_conv_header/app/staffroll/res2d/StaffrollLyt_anim_list.h>
#include <niji_conv_header/app/staffroll/res2d/StaffrollLyt_pane.h>

#include <niji_conv_header/sound/SoundMiddleID.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(View)

  StaffrollTextScrollView::StaffrollTextScrollView( 
  App::Staffroll::System::StaffrollWork* pWork, 
  const App::Staffroll::StaffrollResourceID resId,
  const app::util::G2DUtil::SETUP_DISPLAY_TYPE dispType,
  const f32 dispUpperLimit, 
  const f32 dispLowLimit
  )
    : NetAppLib::System::NetApplicationViewBase( pWork->GetAppHeap() )
  {
    Initialize2D( pWork, resId );

    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    m_dispLowLimit  = dispLowLimit;

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    //  テキストボックスリストを作成
    {
      static const gfl2::lyt::LytPaneIndex  aTextPaneIdx[] =
      {
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_00,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_01,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_02,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_03,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_04,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_05,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_06,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_07,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_08,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_09,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_10,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_11,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_12,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_13,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_14,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_15,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_16,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_17,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_18,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_19,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_20,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_21,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_22,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_23,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_24,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_25,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_26,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_27,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_28,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_29,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_30,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_31,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_32,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_33,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_34,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_35,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_36,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_37,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_38,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_39,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_40,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_41,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_42,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_43,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_44,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_45,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_46,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_47,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_48,
        PANENAME_STAFFROLL_TXT_LOW_000_PANE_TEXTBOX_49,
      };
      
      gfl2::lyt::LytWk * lytwk[LAYOUT_WORK_ID_MAX];
      for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
      {
        lytwk[i] = pG2DUtil->GetLayoutWork( i );
        GFL_ASSERT( lytwk[i] );

        m_pPaneRoot[i] = lytwk[i]->GetPane( PANENAME_STAFFROLL_TXT_LOW_000_PANE_ROOT );
        GFL_ASSERT( m_pPaneRoot[i] );
      }

      u32 textMax = GFL_NELEMS( aTextPaneIdx );
      m_ppTextBoxArray = GFL_NEW_ARRAY( m_pWork->GetAppHeap()->GetDeviceHeap() ) StaffrollTextBox*[ textMax ];
      for( u32 i=0; i<textMax; i++ )
      {
        gfl2::lyt::LytTextBox * pane[LAYOUT_WORK_ID_MAX];
        for( u32 j=0; j<LAYOUT_WORK_ID_MAX; j++ )
        {
          pane[j] = lytwk[j]->GetTextBoxPane( aTextPaneIdx[i] );
          GFL_ASSERT( pane[j] );
        }
        m_ppTextBoxArray[i] = GFL_NEW( m_pWork->GetAppHeap()->GetDeviceHeap() ) StaffrollTextBox( pane[0], pane[1], dispUpperLimit, dispLowLimit, m_pWork->GetEventTextParam() );
      }

      m_textBoxLength = textMax;
    }

    ResetPos();
  }

  StaffrollTextScrollView::~StaffrollTextScrollView()
  {
    for( u32 i = 0; i < m_textBoxLength; ++i )
    {
      GFL_SAFE_DELETE( m_ppTextBoxArray[ i ] );
    }
    GFL_SAFE_DELETE_ARRAY( m_ppTextBoxArray );

    Terminate2D();
  }

  //  移動
  void StaffrollTextScrollView::MoveYPos( const f32 speed )
  {
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      gfl2::math::VEC3 pos = m_pPaneRoot[i]->GetTranslate();
      pos.y += speed;

      m_pPaneRoot[i]->SetTranslate( pos );
    }
  }

  //  位置リセット
  void StaffrollTextScrollView::ResetPos()
  {
    {
      gfl2::math::VEC3 pos( 0, -DISPLAY_UPPER_HEIGHT, 0 );
      m_pPaneRoot[LAYOUT_WORK_ID_UPPER]->SetTranslate( pos );
    }
    {
      gfl2::math::VEC3 pos( 0, 0, 0 );
      m_pPaneRoot[LAYOUT_WORK_ID_LOWER]->SetTranslate( pos );
    }
  }

  //  書き込み位置リセット
  //  書き込み位置設定
  void StaffrollTextScrollView::SetWritePos( const f32 pos )
  {
    m_writePosY = pos;
  }

  void StaffrollTextScrollView::AddWritePos( const f32 num )
  {
    m_writePosY += num;
  }

  //  すべてのテキストが流れているか
  bool StaffrollTextScrollView::IsTextAllOut()
  {
    bool  bRet  = true;
    for( u32 i = 0; i < m_textBoxLength; ++i )
    {
      if( m_ppTextBoxArray[ i ]->IsUsed() == true )
      {
        bRet  = false;
      }
    }

    return  bRet;
  }

  //  アルファ設定
  void StaffrollTextScrollView::SetAlpha( const u8 alpha )
  {
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      m_pPaneRoot[i]->SetAlpha( alpha );
    }
  }

  //  テキスト設定
  bool StaffrollTextScrollView::SettingBoxText( 
  const u32 messageId, 
  const f32 xPos, 
  const f32 xOffsetPos, 
  const f32 yPos, 
  const u32 strPos, 
  const u32 font, 
  const u32 color )
  {
    App::Staffroll::StaffrollTextBox* pTextBox = GetEmptyTextBox();
    if( pTextBox == NULL )
    {
      return false;
    }

    m_writePosY += yPos;

    gfl2::str::MsgData* pMsgData  = m_pWork->GetMessageUtility()->GetData( GARC_message_staff_list_DAT );

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::str::StrBuf* strbuf = pG2DUtil->GetTempStrBuf(0);

    pMsgData->GetString( messageId, *strbuf );
    UCHIDA_PRINT( "文字描画 messageId=%d x=%lf xofs=%lf strpos=%d y=%lf\n", messageId, xPos, xOffsetPos, strPos, yPos );

    pTextBox->Start( strbuf, xPos, xOffsetPos, m_writePosY, strPos, font, color );

    return  true;
  }

  //  テキストボックスをすべてクリア
  void StaffrollTextScrollView::ClearAllTextBox()
  {
    //画面内のテキストをクリアー
    for( u32 i = 0; i < m_textBoxLength; ++i )
    {
      if( m_ppTextBoxArray[i]->IsUsed() == true )
      {
        m_ppTextBoxArray[i]->Clear();
      }
    }
  }

  //  未使用テキストボックスを取得
  App::Staffroll::StaffrollTextBox* StaffrollTextScrollView::GetEmptyTextBox()
  {
    for( u32 i = 0; i < m_textBoxLength; ++i )
    {
      if( m_ppTextBoxArray[i]->IsUsed() == false )
      {
        return m_ppTextBoxArray[i];
      }
    }

    return NULL;
  }

  //  使用してかつ画面一番下に表示しているテキストボックスを取得
  App::Staffroll::StaffrollTextBox* StaffrollTextScrollView::GetLastPosYDrawTextBox()
  {
    App::Staffroll::StaffrollTextBox* pTextBox  = NULL;
    for( u32 i = 0; i < m_textBoxLength; ++i )
    {
      if( m_ppTextBoxArray[ i ]->IsUsed() == true )
      {
        if( pTextBox == NULL )
        {
          pTextBox  = m_ppTextBoxArray[ i ];
        }
        else
        {
          //  現在参照しているより下位置にあるなら更新
          if( m_ppTextBoxArray[ i ]->GetPosY() <= pTextBox->GetPosY() )
          {
            pTextBox  = m_ppTextBoxArray[ i ];
          }
        }
      }
    }

    return  pTextBox;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void StaffrollTextScrollView::Update(void)
  {
    //画面上部に行ったら解放
    for( u32 i = 0; i < m_textBoxLength; ++i )
    {
      if( m_ppTextBoxArray[ i ]->IsUsed() && m_ppTextBoxArray[ i ]->IsScrollOver() )
      {
        // GFL_PRINT( "text scroll over clear %d\n", i );
        m_ppTextBoxArray[ i ]->Clear();
      }
    }

    NetAppLib::System::NetApplicationViewBase::Update();
  }

  void StaffrollTextScrollView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, i, gfl2::lyt::ORDER_NORMAL + 24 );
    }
  }




  void StaffrollTextScrollView::Initialize2D(
        App::Staffroll::System::StaffrollWork * work,
        const App::Staffroll::StaffrollResourceID res_id )
  {
    NetAppLib::System::ResourceManager * man = work->GetResourceManager();
    void * buf = man->GetResourceBuffer( res_id );

    // リソースデータ
    const app::util::G2DUtil::LytResData res_tbl[] =
    {
      { buf, 0, app::util::G2DUtil::ATTACH_ROOT },
    };
    // レイアウトデータ
	  const app::util::G2DUtil::LytwkData data_tbl[] =
    {
      {
        0,
        LYTRES_STAFFROLLLYT_STAFFROLL_TXT_LOW_000_BFLYT,
        LA_STAFFROLLLYT_STAFFROLL_TXT_LOW_000___NUM,
        buf,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
      {
        0,
        LYTRES_STAFFROLLLYT_STAFFROLL_TXT_LOW_000_BFLYT,
        LA_STAFFROLLLYT_STAFFROLL_TXT_LOW_000___NUM,
        buf,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      },
    };

//  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
//  CreateGraphicsSystem( pAppHeap, false );

    NetAppLib::Message::MessageUtility * msg = work->GetMessageUtility();

    Create2D(
      work->GetAppHeap(),
      NULL,
      LAYOUT_WORK_ID_MAX,
      res_tbl,
      GFL_NELEMS(res_tbl),
      data_tbl,
      GFL_NELEMS(data_tbl),
      msg->GetData(GARC_message_staff_list_DAT),
      msg->GetWordSet() );
  }

  void StaffrollTextScrollView::Terminate2D(void)
  {
    Delete2D();
//    DeleteGraphicsSystem();
  }


  GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)
