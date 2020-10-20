//==============================================================================
/**
 @file    ConfigUIViewPool.cpp
 @date    2015.12.15
 */
//==============================================================================
#include "App/Config/source/ConfigUIViewPool.h"

//  nijiのインクルード
#include "System/include/HeapDefine.h"
#include "System/include/ArcIdGetter.h"

#include "AppLib/include/Util/app_util_heap.h"
#include "Applib/include/Tool/app_tool_MenuWindow.h"
#include "Applib/include/Util/AppUtilFileControlState.h"

#include "Savedata/include/ConfigSave.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_option.h"

//  リソースのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/configMenu.gaix>
#include <arc_index/MenuWindow.gaix>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(config)

  ConfigUIViewPool::ConfigUIViewPool( const struct INIT_PARAM& rInitParam )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pUtilHeap );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  ConfigUIViewPool::~ConfigUIViewPool()
  {
    GFL_SAFE_DELETE( m_pLowMessageMenuView );
    GFL_SAFE_DELETE( m_pUppRootView );
    GFL_SAFE_DELETE( m_pLowRootView );

    m_pMenuCursor->DeleteResource();
    GFL_SAFE_DELETE( m_pMenuCursor );

    {
      u32 loopNum = GFL_NELEMS( m_pFileCtrlStateList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_pFileCtrlStateList[ i ] );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief  初期化
    * @return     初期化完了: true / 初期化中: false
    */
  //------------------------------------------------------------------
  bool ConfigUIViewPool::InitFunc()
  {
    app::util::Heap*  pUtilHeap = m_initParam.pUtilHeap;

    if( m_initSeq == 0 )
    {
      //  ファイル制御ステータス
      {
        //  レイアウトデータをファイル制御
        m_pFileCtrlStateList[ FILE_DATA_UI_LYT ]       = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( NIJI_ARCID_NUM( ARCID_CONFIG_MENU ), GARC_configMenu_lyt_applyt_COMP, pUtilHeap->GetDeviceHeap(), true, 128 );
        m_pFileCtrlStateList[ FILE_DATA_MENU_WINDOW ]  = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( NIJI_ARCID_NUM( ARCID_MENU_WINDOW ), GARC_MenuWindow_MenuWindow_applyt_COMP, pUtilHeap->GetDeviceHeap(), true, 128 );

        //  ファイルロード
        u32 loopNum = GFL_NELEMS( m_pFileCtrlStateList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_pFileCtrlStateList[ i ]->Start();
        }
      }

      //  メニューカーソル作成
      {
        m_pMenuCursor = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::tool::MenuCursor( pUtilHeap, &m_pMenuCursorLytDataBuffer );
      }

      ++m_initSeq;
    }
    
    if( m_initSeq == 1 )
    {
      bool  bEnd  = true;

      //  他のファイル処理
      {
        u32 loopNum = GFL_NELEMS( m_pFileCtrlStateList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_pFileCtrlStateList[ i ]->Update();

          if( m_pFileCtrlStateList[ i ]->IsEnd() == false )
          {
            bEnd  = false;
          }
        }
      }

      //  カーソルの読み込み
      if( m_pMenuCursor->LoadResource() == false )
      {
        bEnd  = false;
      }

      if( bEnd == true )
      {
        ++m_initSeq;
      }
    }
    
    if( m_initSeq == 2 )
    {
      app::util::AppUtilFileControlState* pFileCtrlStateLytData = m_pFileCtrlStateList[ FILE_DATA_UI_LYT ];
      GFL_ASSERT( pFileCtrlStateLytData );

      //  下画面のView作成
      {
        {
          ConfigLowerUIDraw::INIT_PARAM  initParam;
          {
            initParam.pLytBuff                  = pFileCtrlStateLytData->GetLoadDataBuffer();
            initParam.pRenderingManager         = m_initParam.pRenderManager;
            initParam.pMsgData                  = m_initParam.pAppMsgData;
            initParam.pWordSet                  = m_initParam.pWordSet;
            initParam.pMenuCursorLytDataBuffer  = m_pMenuCursorLytDataBuffer;

            //  項目追加
            {
              if( m_initParam.pConfigData->IsJapanese() )
              {
                const ConfigLowerUIDraw::ConfigType aSelectConfigType[] =
                {
                  ConfigLowerUIDraw::CONFIG_TYPE_TALK_SPEED,
                  ConfigLowerUIDraw::CONFIG_TYPE_TEXT_MODE,
                  ConfigLowerUIDraw::CONFIG_TYPE_BATTLE_ANIM_SHOW,
                  ConfigLowerUIDraw::CONFIG_TYPE_BATTLE_RULE,
                  ConfigLowerUIDraw::CONFIG_TYPE_BUTTOM_MODE,
                  ConfigLowerUIDraw::CONFIG_TYPE_BOX_STATUS
                };

                u32 loopNum = GFL_NELEMS( aSelectConfigType );
                for( u32 i = 0; i < loopNum; ++i )
                {
                  _OutputItemListData( &initParam.aListItemParam[ i ], aSelectConfigType[ i ] );
                }
                initParam.listMax = loopNum;
              }
              else
              {
                const ConfigLowerUIDraw::ConfigType aSelectConfigType[] =
                {
                  ConfigLowerUIDraw::CONFIG_TYPE_TALK_SPEED,
                  ConfigLowerUIDraw::CONFIG_TYPE_BATTLE_ANIM_SHOW,
                  ConfigLowerUIDraw::CONFIG_TYPE_BATTLE_RULE,
                  ConfigLowerUIDraw::CONFIG_TYPE_BUTTOM_MODE,
                  ConfigLowerUIDraw::CONFIG_TYPE_BOX_STATUS
                };

                u32 loopNum = GFL_NELEMS( aSelectConfigType );
                for( u32 i = 0; i < loopNum; ++i )
                {
                  _OutputItemListData( &initParam.aListItemParam[ i ], aSelectConfigType[ i ] );
                }
                initParam.listMax = loopNum;
              }

              GFL_ASSERT( initParam.listMax <= GFL_NELEMS( initParam.aListItemParam ) );
            }
          }

          m_pLowRootView          = GFL_NEW( pUtilHeap->GetDeviceHeap() ) ConfigLowerUIDraw( pUtilHeap );
          m_pLowRootView->Initialize( initParam );

          m_pLowRootView->SetInputEnabled( false );
        }


        //  MessageMenuView作成
        {
          App::Tool::MessageMenuView::PARAM param( pUtilHeap, m_initParam.pRenderManager );
          m_pLowMessageMenuView  = GFL_NEW( pUtilHeap->GetDeviceHeap() ) App::Tool::MessageMenuView( param );
          m_pLowMessageMenuView->Setup( m_pFileCtrlStateList[ FILE_DATA_MENU_WINDOW ]->GetLoadDataBuffer(), m_pMenuCursorLytDataBuffer );
        }
      }

      //  上画面のView作成
      {
        ConfigUpperUIDraw::INIT_PARAM  initParam;
        {
          initParam.pLytBuff          = pFileCtrlStateLytData->GetLoadDataBuffer();
          initParam.pRenderingManager = m_initParam.pRenderManager;
          initParam.pMsgData          = m_initParam.pAppMsgData;
          initParam.pWordSet          = m_initParam.pWordSet;
        }

        m_pUppRootView          = GFL_NEW( pUtilHeap->GetDeviceHeap() ) ConfigUpperUIDraw( pUtilHeap );
        m_pUppRootView->Initialize( initParam );
      }

      ++m_initSeq;
    }
    
    if( m_initSeq == 3 )
    {
      bool  bRet  = true;
      if( m_pLowMessageMenuView->IsSetup() == false )
      {
        bRet  = false;
      }

      if( bRet == true )
      {
        ++m_initSeq;
      }
    }

    if( 4 <= m_initSeq )
    {
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief  終了
    * @return 終了完了: true / 終了中: false
    */
  //------------------------------------------------------------------
  bool ConfigUIViewPool::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      m_pLowRootView->Terminate();
      m_pUppRootView->Terminate();

      ++m_endSeq;
    }
    else
    if( m_endSeq == 1 )
    {
      if( ( m_pLowRootView->IsDelete() == true )
      ||  ( m_pLowRootView->IsDrawing() == false )
      ||  ( m_pUppRootView->IsDrawing() == false ) )
      {
        ++m_endSeq;
      }
    }
    else
    {
      return  true;
    }

    return  false;
  }

  //  指定した設定タイプに沿って、リスト項目のデータ出力
  void ConfigUIViewPool::_OutputItemListData( ConfigLowerUIDraw::LIST_ITEM_PARAM* pOutputData, const ConfigLowerUIDraw::ConfigType type )
  {
    GFL_ASSERT( pOutputData );

    Savedata::ConfigSave* pConfigSaveData = m_initParam.pConfigData;

    //  話の早さ
    if( type == ConfigLowerUIDraw::CONFIG_TYPE_TALK_SPEED )
    {
      pOutputData->firstSelectIdx = pConfigSaveData->GetTalkingSpeed();
    }
    //  文字モード
    else
    if( type == ConfigLowerUIDraw::CONFIG_TYPE_TEXT_MODE )
    {
      if( pConfigSaveData->IsKanjiMode() == false )
      {
        pOutputData->firstSelectIdx = 0;
      }
      else
      {
        pOutputData->firstSelectIdx = 1;
      }
    }
    //  戦闘アニメ
    else
    if( type == ConfigLowerUIDraw::CONFIG_TYPE_BATTLE_ANIM_SHOW )
    {
      //  各項目の選択idx = コンフィグでセーブしている値になっている
      pOutputData->firstSelectIdx = static_cast<u32>( pConfigSaveData->GetBattleAnimation() );
    }
    //  試合のルール
    else
    if( type == ConfigLowerUIDraw::CONFIG_TYPE_BATTLE_RULE )
    {
      //  各項目の選択idx = コンフィグでセーブしている値になっている
      pOutputData->firstSelectIdx = static_cast<u32>( pConfigSaveData->GetBattleRule() );
    }
    //  ボタンモード
    else
    if( type == ConfigLowerUIDraw::CONFIG_TYPE_BUTTOM_MODE )
    {
      //  各項目の選択idx = コンフィグでセーブしている値になっている
      pOutputData->firstSelectIdx = static_cast<u32>( pConfigSaveData->GetButtonMode() );
    }
    //  Boxの送信
    else
    if( type == ConfigLowerUIDraw::CONFIG_TYPE_BOX_STATUS )
    {
      //  各項目の選択idx = コンフィグでセーブしている値になっている
      pOutputData->firstSelectIdx = static_cast<u32>( pConfigSaveData->GetBoxStatus() );
    }
    else
    {
      GFL_ASSERT( 0 );
      return;
    }

    //  データ設定
    {
      const ConfigLowerUIDraw::LIST_ITEM_CREATE_DATA&  rCreateData = ConfigLowerUIDraw::GetCreateListItemData( type );

      pOutputData->itemData.title_text_id = rCreateData.title_text_id;
      pOutputData->itemData.select_idx    = pOutputData->firstSelectIdx;
      pOutputData->type                   = type;
      pOutputData->helpMsgId              = rCreateData.help_msg_id;

      //  選択の内容設定
      {
        u32 loopMax = GFL_NELEMS( rCreateData.a_select_text_id );

        GFL_ASSERT( loopMax <= GFL_NELEMS( pOutputData->itemData.a_select_text_id ) );

        pOutputData->itemData.select_num  = 0;
        while( rCreateData.a_select_text_id[ pOutputData->itemData.select_num ] != ConfigLowerUIDraw::GetTextErrorId() )
        {
          if( loopMax <= pOutputData->itemData.select_num )
          {
            break;
          }

          pOutputData->itemData.a_select_text_id[ pOutputData->itemData.select_num ] = rCreateData.a_select_text_id[ pOutputData->itemData.select_num ];

          ++pOutputData->itemData.select_num;
        }
      }
    }
  }

GFL_NAMESPACE_END(config)
GFL_NAMESPACE_END(app)
