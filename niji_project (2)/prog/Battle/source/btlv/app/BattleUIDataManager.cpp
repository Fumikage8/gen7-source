//=============================================================================
/**
 * @file  BattleUIDataManager.h
 * @brief バトル中に使用するパラメータのアクセス用クラス
 *
 * @author uchida_y
 * @date  2015.04.10
 * @note  BattleViewSystemを使用する
 */
//=============================================================================
#include "BattleUIDataManager.h"

//  nijiのインクルード
#include <GameSys/include/GameData.h>

//  view
#include "BattleUIActSelectLowerDraw.h"
#include "BattleUISkillSelectLowerDraw.h"
#include "BattleUITargetSelectLowerDraw.h"

//  battleのインクルード
#include "Battle/source/btl_SideEffectStatus.h"
#include "Battle/source/btl_PosEffectStatus.h"
#include "Battle/source/btl_ZenryokuWazaStatus.h"
#include "Battle/source/btl_ZenryokuWaza.h"
#include "Battle/source/btl_mainmodule.h"

#include "niji_conv_header/message/msg_btl_app.h"
#include "niji_conv_header/message/msg_wazainfo.h"

#include "pml/include/item/btl_item.h"
#include "pml/include/item/item.h"

#include "niji_conv_header/message/msg_btl_state.h"

#include "Savedata/include/MyItemSaveDef.h"

// 動的ヘッダシステムを利用するときは以下のヘッダをインクルードする
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

//  バトルUI用のarcヘッダーファイル
#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"
#include "arc_index/message.gaix"

#define NIJI_DEBUG_BTL_LOWER_HEADER_FILEPATH "niji_conv_header/app/battle/BtlLowerDynamic.h"

// 動的ヘッダシステムで利用するヘッダは以下のマクロを通してincludeしてください
#include GFL_DYNAMIC_HEADER(NIJI_DEBUG_BTL_LOWER_HEADER_FILEPATH)

GFL_NAMESPACE_BEGIN(btl)
GFL_NAMESPACE_BEGIN(app)

  //  このクラスが使用するヒープサイズ
  const u32 BattleUIDataManager::ms_useSystemHeapSize = 1024 * 200;
  const u32 BattleUIDataManager::ms_useDeviceHeapSize = 1024 * 600;

  BattleUIDataManager::BattleUIDataManager(BattleViewSystem* btlv_core) :
  m_btlv(btlv_core)
  {
    _Clear();
  }

  BattleUIDataManager::~BattleUIDataManager()
  {
    m_appCommonGrpIconData.Terminate();

    //  メモリ関連の破棄はここで
    {
      u32 loopNum = GFL_NELEMS( m_pCacthFileCtrlStateList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_pCacthFileCtrlStateList[ i ] );
      }
    }

    //  事前開閉ファイルキャッシュリスト作成
    {
      u32 loopNum = GFL_NELEMS( m_pCatchFileOnepCloseList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_pCatchFileOnepCloseList[ i ] );
      }
    }

    GFL_SAFE_DELETE( m_pRecPlayLowerView );
    GFL_SAFE_DELETE( m_pInfoWindowView );
    GFL_SAFE_DELETE( m_pItemMessage );
    GFL_SAFE_DELETE( m_pWordSetLoader );
    GFL_SAFE_DELETE( m_pSelfPosPlaceList );
    GFL_SAFE_DELETE( m_pEnemyPosPlaceList );
    GFL_SAFE_DELETE( m_pItemManager );
    GFL_SAFE_DELETE( m_pMenuCursor );
    GFL_SAFE_DELETE( m_pItemInfoMsgData );
    GFL_SAFE_DELETE( m_pTokuseInfoMsgData );
    GFL_SAFE_DELETE( m_pWzaaInfoMsgData );
    GFL_SAFE_DELETE( m_pStatusMsgData );
    GFL_SAFE_DELETE( m_pAppMsgData );
    GFL_SAFE_DELETE( m_pWordSet );
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    * @param   pResidentHeap : NEWが一度しか行われず、バトル終了まで開放しない時に使うヒープ
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::Initialize( gfl2::heap::HeapBase* pResidentHeap , void *btlAppMsgBuf)
  {
    GFL_ASSERT( pResidentHeap );

    // ヘッダファイルのロード
    // ファイルパスと同時にテーブルのキャパシティ(最大定義数)も指定する必要がある
    GFL_LOAD_HEADER_HASH(NIJI_DEBUG_BTL_LOWER_HEADER_FILEPATH, 32);

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      pResidentHeap->PrintHeapInfo( "BattleUIDataManager(pResidentHeap) Initialize\n" );
#endif

    //  クラス内部で使用するヒープ
    m_appUtilHeap.LocalHeapCreate( pResidentHeap, pResidentHeap, ms_useSystemHeapSize, ms_useDeviceHeapSize );

    //  UIで使うメッセージとワードデータを作成する
    {
      //  UI画面で使う共通メッセージ(あらかじめメモリに全展開)
      //m_pAppMsgData = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_MAX ), GARC_message_btl_app_DAT, m_appUtilHeap.GetDeviceHeap(), gfl2::str::MsgData::LOAD_PART );
      //カクツキ防止のため非同期読みして渡す
      m_pAppMsgData = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) gfl2::str::MsgData( btlAppMsgBuf , m_appUtilHeap.GetDeviceHeap() );
      GFL_ASSERT( m_pAppMsgData );

      //  バトルUIで使用するwordset
      m_pWordSet = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) print::WordSet( print::WordSet::DEFAULT_WORD_MAX, print::WordSet::DEFAULT_BUF_LEN , m_appUtilHeap.GetDeviceHeap() );
      GFL_ASSERT( m_pWordSet );

      //メッセージ系はここだとカクツクので、後で読み込む。
      m_loadMsgCnt = 0;
      }

    //  UIで共通のメニューカーソルリソース作成(カーソル描画自体は必要ない、ほしいのはリソースバッファ)
    {
      m_pMenuCursor = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) ::app::tool::MenuCursor( m_btlv->GetAppHeap(), &m_pMenuCursorLytBuffer );
      GFL_ASSERT( m_pMenuCursor );
    }

    //  名前取得時のデータローダーを作成(常駐させる,あくまでファイルを開くだけで、テキストデータの展開は使用するたびに行われる、メモリ省エネ！)
    {
      u32 readBit = PRELOAD_ITEMNAME | PRELOAD_WAZANAME | PRELOAD_TOKUSEI;
      m_pWordSetLoader  = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) WordSetLoader( m_appUtilHeap.GetDeviceHeap(), readBit );

      m_pWordSetLoader->StartLoad( m_appUtilHeap.GetDeviceHeap()->GetLowerHandle() );

      m_pWordSet->SetWordSetLoader( m_pWordSetLoader );
    }

    //  バトルの味方・敵の配置リストバッファ作成
    {
      m_pSelfPosPlaceList  = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) gfl2::util::List<BtlvPos>( m_appUtilHeap.GetDeviceHeap(), 4 );
      m_pEnemyPosPlaceList = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) gfl2::util::List<BtlvPos>( m_appUtilHeap.GetDeviceHeap(), 4 );
    }

    //  ファイルキャッシュリスト作成
    {
      //  ファイルキャッシュリスト設定データ構造体( ローカル限定 )
      typedef struct
      {
        gfl2::fs::ArcFile::ARCID    arcId;
        gfl2::fs::ArcFile::ARCDATID dataId;
        bool bFileComp;

      } CATCH_FILE_SETUP_LOCAL_DATA;

      const CATCH_FILE_SETUP_LOCAL_DATA aSetupData[]  =
      {
        {
          ARCID_BATTLE_LOWER_MENU,
          GARC_battleLowwerMenu_skllow_APPLYT,
          false
        }
      };

      //  ファイルロード時のヒープは取得開放を繰り返す。
      //  ファイルロードで使うヒープは一時用に
      u32 loopNum = GFL_NELEMS( m_pCacthFileCtrlStateList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pCacthFileCtrlStateList[ i ]  = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) ::app::util::AppUtilFileControlState( aSetupData[ i ].arcId, aSetupData[ i ].dataId, m_appUtilHeap.GetDeviceHeap()->GetLowerHandle(), aSetupData[ i ].bFileComp, 128 );
      }
    }

    //  事前開閉ファイルキャッシュリスト作成
    {
      const u32 aArcID[ FILE_CATCH_OPEN_MAX ]  =
      {
        ARCID_BATTLE_LOWER_MENU,
        ARCID_BATTLE_LOWER_POKE_LIST_MENU,
      };

      u32 loopNum = GFL_NELEMS( m_pCatchFileOnepCloseList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pCatchFileOnepCloseList[ i ]  = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) ::app::util::AppUtilFileControlState( aArcID[ i ], m_appUtilHeap.GetDeviceHeap(), true, 128 );
        m_pCatchFileOnepCloseList[ i ]->Start();
      }
    }

    //  共通アイコングラフィックデータの初期化
    {
      m_appCommonGrpIconData.Initialize( &m_appUtilHeap );
    }

    //  ビデオ再生用のView生成
    //  ビデオ再生モード時のみ生成
    if( m_btlv->IsRecordPlayMode() == true )
    {
      BattleUIRecPlayLowerView::INIT_PARAM initParam;
      initParam.pMsgData          = m_pAppMsgData;
      initParam.pWordSet          = m_pWordSet;
      initParam.pUtilHeap         = &m_appUtilHeap;
      initParam.pRenderingManager = m_btlv->GetRenderSys()->GetAppRenderingManager();

      m_pRecPlayLowerView = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) BattleUIRecPlayLowerView( initParam );
    }
    else
    {
      //  バトルで使用するアイテムポケットの振り分けデータを展開(常駐させる)
      {
        // バトルポケットのデータを読み込む
        item::AsyncLoadBtlPocketData( m_appUtilHeap.GetDeviceHeap() , m_appUtilHeap.GetDeviceHeap()->GetLowerHandle() );
      }

      //  インフォウィンドウView生成
      {
        m_pInfoWindowView = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) ::app::tool::InfoWindowSheetType( &m_appUtilHeap );
        m_pInfoWindowView->StartAsyncCreate( &m_appUtilHeap, m_btlv->GetRenderSys()->GetAppRenderingManager(), NULL );
      }
    }
  }

  //非同期処理の待ち部分
  bool BattleUIDataManager::InitializeWait( gfl2::heap::HeapBase* pResidentHeap )
  {
    if( item::IsFinishLoadBtlPocketData() == true )
    {
      if( m_btlv->IsRecordPlayMode() == true )
      {
#ifdef DEBUG_ONLY_FOR_uchida_yuto
        // ヒープ残量チェック
        pResidentHeap->PrintHeapInfo( "BattleUIDataManager(pResidentHeap) Create Data\n" );
#endif
        return true;
      }
      else
      {
        m_pInfoWindowView->Update();  //生成のため
        if( m_pInfoWindowView->IsCreated() == true )
        {
          item::FinishLoadBtlPocketData();
          // @fix GFNMCat[1363] 通信対戦の時はインフォボタンを押した時の開閉SEは使わない
          m_pInfoWindowView->SetSEOn( IsPlayInputSE() );
#ifdef DEBUG_ONLY_FOR_uchida_yuto
          // ヒープ残量チェック
          pResidentHeap->PrintHeapInfo( "BattleUIDataManager(pResidentHeap) Create Data\n" );
#endif
          return true;
        }
      }
    }
    return false;
  }

  //メッセージの分割読み
  // @return true 終了
  bool BattleUIDataManager::LoadMessage(gfl2::heap::HeapBase* pResidentHeap)
  {
    switch( m_loadMsgCnt )
    {
    case 0:
      //  わざの説明メッセージ(メッセージ量が多いので、データは使うたびにロードする、ファイルの開閉はあらかじめ行う)
      m_pWzaaInfoMsgData  = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_MAX ), GARC_message_wazainfo_DAT, m_appUtilHeap.GetDeviceHeap(), gfl2::str::MsgData::LOAD_PART );
      GFL_ASSERT( m_pWzaaInfoMsgData );
      m_loadMsgCnt++;
      break;
    case 1:
      //  特性の説明メッセージ(メッセージデータがでかいのでファイルの開閉のみする)
      m_pTokuseInfoMsgData  = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_MAX ), GARC_message_tokuseiinfo_DAT, m_appUtilHeap.GetDeviceHeap(), gfl2::str::MsgData::LOAD_PART );
      m_loadMsgCnt++;
      break;
    case 2:
      //  アイテムの説明メッセージ(メッセージデータがでかいのでファイルの開閉のみする)
      m_pItemInfoMsgData  = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_MAX ), GARC_message_iteminfo_DAT, m_appUtilHeap.GetDeviceHeap(), gfl2::str::MsgData::LOAD_PART );
      m_loadMsgCnt++;
      break;
    case 3:
      //  アイテムメッセージを作成（ファイルの開きはあらかじめしておく, テキストデータはメモリの問題で展開はしない ）
      m_pItemMessage  = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) print::ItemMessage( m_appUtilHeap.GetDeviceHeap(), gfl2::str::MsgData::LOAD_PART );
      m_loadMsgCnt++;
      break;
    case 4:
      //  UI画面で使うステータスメッセージ(データ量が多いので、ファイルの開閉のみ)
      m_pStatusMsgData  = GFL_NEW( m_appUtilHeap.GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_MAX ), GARC_message_btl_state_DAT, m_appUtilHeap.GetDeviceHeap(), gfl2::str::MsgData::LOAD_PART );
      GFL_ASSERT( m_pStatusMsgData );
      m_loadMsgCnt++;
      break;
    case 5:
      return true;
    }
    return false;
  }

  //  パラメータアクセサが使えるかどうか
  bool BattleUIDataManager::IsReady()
  {
    if( m_btlv == NULL )
    {
      //  BattleViewSystemのインスタンスを持っているのが前提なのでもっていない場合がありえない
      GFL_ASSERT( 0 );
      return  false;
    }

    if( m_readyStep == 0 )
    {
      bool  bNextStep = true;

      //  ワードローダのロード終了チャック
      if( m_pWordSetLoader->WaitLoad() == false )
      {
        bNextStep  = false;
      }

      //  メニュー用のカーソル非同期読み
      if( m_pMenuCursor->LoadResource() == false )
      {
        bNextStep  = false;
      }

      //  共通アイコンの非同期読み待ち
      if( m_appCommonGrpIconData.IsReady() == false )
      {
        bNextStep  = false;
      }

      //  開ファイルキャッシュが終わっているか
      {
        u32 loopNum = GFL_NELEMS( m_pCatchFileOnepCloseList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          //  ファイルを開いている
          m_pCatchFileOnepCloseList[ i ]->Update();
          if( m_pCatchFileOnepCloseList[ i ]->IsManualIdleState() == false )
          {
            bNextStep  = false;
          }
        }
      }

      if( bNextStep == true )
      {
        ++m_readyStep;
      }
    }

    if( m_readyStep == 1 )
    {
      bool  bNextStep = true;

      if( m_pRecPlayLowerView != NULL )
      {
        BattleUIRecPlayLowerView::SETUP_PARAM   setupParam;
        setupParam.pMenuCursorLytBuffer = m_pMenuCursorLytBuffer;

        if( m_pRecPlayLowerView->InitFunc( setupParam ) == false )
        {
          bNextStep = false;
        }
      }

      if( bNextStep == true )
      {
        ++m_readyStep;
      }
    }

    if( m_readyStep == 2 )
    {
      return  true;
    }

    return  false;
  }

  //  インスタンスを破棄可能か
  bool BattleUIDataManager::IsDelete()
  {
    bool  bEnd           = true;

    if( m_pRecPlayLowerView != NULL )
    {
      if( m_pRecPlayLowerView->EndFunc() == false )
      {
        bEnd  = false;
      }
    }

    if( m_pInfoWindowView != NULL )
    {
      if( m_pInfoWindowView->EndFunc() == false )
      {
        bEnd  = false;
      }
    }

    if( IsFreeAllFileLoad() == false )
    {
      bEnd  = false;
    }

    //  開ファイルキャッシュを閉じているかチェック
    {
      u32 loopNum = GFL_NELEMS( m_pCatchFileOnepCloseList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pCatchFileOnepCloseList[ i ]->Update();
          if( m_pCatchFileOnepCloseList[ i ]->IsEnd() == false )
          {
            bEnd  = false;
          }
      }
    }

    if( bEnd == true )
    {
      m_pWordSetLoader->Release();

      //  共通カーソルリソースを削除
      if( m_pMenuCursorLytBuffer != NULL )
      {
        m_pMenuCursor->DeleteResource();
        m_pMenuCursorLytBuffer  = NULL;
      }
    }

    return  bEnd;
  }

  void BattleUIDataManager::Terminate()
  {
    //  ヘッダーファイルのアンロード
    GFL_UNLOAD_HEADER_HASH(NIJI_DEBUG_BTL_LOWER_HEADER_FILEPATH);

    //  破棄してもすぐに破棄完了しない非同期系はここに

    //  先読みファイルが残っている可能性があるので削除
    FreeAllFileLoad();

    if( m_btlv->IsRecordPlayMode() == false )
    {
      //  バトルポケットのデータを開放
      item::UnloadBtlPocketData();
    }

    //  開ファイルキャッシュを閉じる
    {
      u32 loopNum = GFL_NELEMS( m_pCatchFileOnepCloseList );
      for( u32 i = 0; i < loopNum; ++i )
      {
         m_pCatchFileOnepCloseList[ i ]->End();
      }
    }
  }

  //  更新
  void BattleUIDataManager::Update()
  {
    u32 loopNum = GFL_NELEMS( m_pCacthFileCtrlStateList );
    for( u32 i = 0; i < loopNum; ++i )
    {
      m_pCacthFileCtrlStateList[ i ]->Update();
    }

    if( m_pRecPlayLowerView != NULL )
    {
      m_pRecPlayLowerView->UpdateTree();
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ショートカットボールアイテムが使えるか
    */
  //------------------------------------------------------------------
  bool BattleUIDataManager::IsShortCutUseItemBall()
  {
    // ぬし相手には投げられない
    if( m_btlv->GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) == true )
    {
      return  false;
    }
    // 試練中は投げられない
    else
    if( m_btlv->GetSetupStatusFlag( BTL_STATUS_FLAG_SIREN_BATTLE ) == true )
    {
      return  false;
    }
    // 捕獲不可戦闘では投げられない
    else
    if( m_btlv->GetSetupStatusFlag( BTL_STATUS_FLAG_NO_CAPTURE ) == true )
    {
      return  false;
    }

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ファイルロードデータすべて開放しているか
    */
  //------------------------------------------------------------------
  bool BattleUIDataManager::IsFreeAllFileLoad()
  {
    u32 loopNum = GFL_NELEMS( m_pCacthFileCtrlStateList );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pCacthFileCtrlStateList[ i ]->IsEnd() == false )
      {
        return  false;
      }
    }

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ファイルロードデータを一括開放
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::FreeAllFileLoad()
  {
    u32 loopNum = GFL_NELEMS( m_pCacthFileCtrlStateList );
    for( u32 i = 0; i < loopNum; ++i )
    {
      EndPrefetchBtlFileLoad( (FileCatchType)i );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ファイルロード開始
    * @note    レイアウトバイナリの取得を先読みするために作成
    */
  //------------------------------------------------------------------
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  void BattleUIDataManager::__StartPrefetchBtlFileLoad( const FileCatchType type, const char* pCallerFileName, const u32 callerLineNo )
  {
    GFL_PRINT( "\nStartPrefetchBtlFileLoad( %d ): %s(%d)\n", type, pCallerFileName, callerLineNo );
#else
  void BattleUIDataManager::StartPrefetchBtlFileLoad( const FileCatchType type )
  {
#endif

    GFL_ASSERT( type < FILE_CATCH_TYPE_MAX );
    m_pCacthFileCtrlStateList[ type ]->Start();
  }

  //------------------------------------------------------------------
  /**
    * @brief   ファイルロード終了
    */
  //------------------------------------------------------------------
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  void BattleUIDataManager::__EndPrefetchBtlFileLoad( const FileCatchType type, const char* pCallerFileName, const u32 callerLineNo )
  {
    GFL_PRINT( "\nEndPrefetchBtlFileLoad( %d ): %s(%d)\n", type, pCallerFileName, callerLineNo );
#else
  void BattleUIDataManager::EndPrefetchBtlFileLoad( const FileCatchType type )
  {
#endif
    GFL_ASSERT( type < FILE_CATCH_TYPE_MAX );

    m_pCacthFileCtrlStateList[ type ]->End();
  }

  //------------------------------------------------------------------
  /**
    * @brief   ファイルロードしているか
    */
  //------------------------------------------------------------------
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  bool BattleUIDataManager::__IsPrefetchBtlFileLoad( const FileCatchType type, const char* pCallerFileName, const u32 callerLineNo )
  {
    GFL_PRINT( "\nIsPrefetchBtlFileLoad( %d ): %s(%d)\n", type, pCallerFileName, callerLineNo );
#else
  bool BattleUIDataManager::IsPrefetchBtlFileLoad( const FileCatchType type )
  {
#endif
    GFL_ASSERT( type < FILE_CATCH_TYPE_MAX );

    return m_pCacthFileCtrlStateList[ type ]->IsStart();
  }

  //------------------------------------------------------------------
  /**
    * @brief   ファイルロードのデータ取得
    */
  //------------------------------------------------------------------
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  void* BattleUIDataManager::__GetPrefetchBtlFileLoadData( const FileCatchType type, const char* pCallerFileName, const u32 callerLineNo )
  {
    GFL_PRINT( "\nGetPrefetchBtlFileLoadData( %d ): %s(%d)\n", type, pCallerFileName, callerLineNo );
#else
  void* BattleUIDataManager::GetPrefetchBtlFileLoadData( const FileCatchType type )
  {
#endif
    GFL_ASSERT( type < FILE_CATCH_TYPE_MAX );

    if( m_pCacthFileCtrlStateList[ type ]->IsEnd() )
    {
      return  m_pCacthFileCtrlStateList[ type ]->GetLoadDataBuffer();
    }

    return  NULL;
  }

  const BTL_PARTY* BattleUIDataManager::GetBattleParty_Self( void ) const
  {
    return m_btlv->GetBattleParty_Self();
  }

  u8 BattleUIDataManager::GetFriendCleintID( void ) const
  {
    if( m_btlv )
    {
      return m_btlv->GetFriendCleintID();
    }

    return 0;
  }

  u8 BattleUIDataManager::GetClientID( void ) const
  {
    if( m_btlv )
    {
      return m_btlv->GetClientID();
    }

    return 0;
  }

  bool BattleUIDataManager::IsClientNPC( u8 clientID ) const
  {
    if( m_btlv )
    {
      return m_btlv->IsClientNpc( clientID );
    }

    return false;
  }

  //  にげることができるか
  bool BattleUIDataManager::IsEsc()
  {
    if( GetBattleCompetitor() != BTL_COMPETITOR_WILD )
    {
      //  野生以外では逃げることはできない（sangoのロジックを使用）
      return  false;
    }
    else
    if( m_btlv->GetSetupStatusFlag( BTL_STATUS_FLAG_SIREN_BATTLE ) == true )
    {
      //  試練中は逃げることはできない
      return  false;
    }
    // @fix GFMMCat[209] VSヒカリドラゴン戦で、にげることができる
    // @fix GFMMCat[547] 合体ネクロズマ戦にて逃げられる
    else
    if (m_btlv->GetSetupStatusFlag(BTL_STATUS_FLAG_NO_CAPTURE) == true)
    {
      //  ヒカリドラゴン戦は逃げることはできない
      return  false;
    }

    BtlEscapeMode escMode = m_btlv->GetEscapeMode();
    if( escMode == BTL_ESCAPE_MODE_NG )
    {
      //  バトル側で逃げるが禁止している場合
      return  false;
    }

    return  true;
  }

  BtlCompetitor BattleUIDataManager::GetBattleCompetitor( void ) const
  {
    return  m_btlv->GetBattleCompetitor();

    //  test.
    //return  BTL_COMPETITOR_COMM;
  }

  POKECON* BattleUIDataManager::GetBattleContainer( void ) const
  {
    return m_btlv->GetBattleContainer();
  }

  const BTL_PARTY* BattleUIDataManager::GetBattleParty_Enemy( int idx ) const
  {
    return m_btlv->GetBattleParty_Enemy(idx);
  }

  const BTL_PARTY* BattleUIDataManager::GetBattleParty_Friend() const
  {
    return m_btlv->GetBattleParty_Friend();
  }

  BTL_POKEPARAM* BattleUIDataManager::GetBattlePokeParam( BtlvPos pos ) const
  {
    return m_btlv->GetBattlePokeParam(pos);
  }

  /** バトルUI画面の入力SEが再生許可 */
  bool BattleUIDataManager::IsPlayInputSE() const
  {
    //  通信対戦の場合は再生しない
    if( GetBattleCompetitor() == BTL_COMPETITOR_COMM )
    {
      return  false;
    }

    return  true;
  }

  bool BattleUIDataManager::IsPlayerInLeftSide( void )
  {
    if( m_btlv )
    {
      return m_btlv->IsPlayerInLeftSide();
    }

    return true;
  }

  bool BattleUIDataManager::IsMultiMode( void )
  {
    if( m_btlv )
    {
      return m_btlv->IsMultiMode();
    }

    return false;
  }

  BtlRule BattleUIDataManager::GetBattleRule( void ) const
  {
    if( m_btlv )
    {
      return m_btlv->GetBattleRule();
    }

    return  BTL_RULE_SINGLE;
  }

  BtlWeather BattleUIDataManager::GetWeather() const
  {
    if( m_btlv )
    {
      POKECON*      pPokeContainer  = GetBattleContainer();
      if( pPokeContainer )
      {
        FieldStatus*  pFieldStatus    = pPokeContainer->GetFieldStatus();
        if( pFieldStatus )
        {
          return pFieldStatus->GetWeather();
        }
      }
    }

    return BTL_WEATHER_NONE;
  }

  Savedata::MyItem* BattleUIDataManager::GetMyItem()
  {
    return m_btlv->GetMyItem();
  }

  bool BattleUIDataManager::IsCommMode(void) const
  {
    if( m_btlv )
    {
      return m_btlv->IsCommMode();
    }

    return  false;
  }

  bool BattleUIDataManager::GetEnableVoiceChat(void) const
  {
    if( m_btlv )
    {
      return m_btlv->GetEnableVoiceChat();
    }

    return false;
  }

  bool BattleUIDataManager::IsDemoCapture(void) const
  {
    if( m_btlv != NULL )
    {
      return  ( m_btlv->GetBtlvMode() == BTLV_MODE_CAPTURE );
    }

    return  false;
  }

  // @fix MCT要望 デバッグバトルのAIコントロールしているポケモンかどうかの判定に必要
  //  AIコントロールターンか
  bool BattleUIDataManager::IsAICtrlTurn()
  {
    //  デバッグ機能なので製品版には載せない
#if PM_DEBUG
    return  ( GetMyStatus() == NULL );
#else
    return  false;
#endif
  }

  //SBTS[1234] タグ対応
  //"パパの　ことば……[Name:TrainerName ]！　こういうものには▽つかいどきが　あるんだ！"対応
  const Savedata::MyStatus* BattleUIDataManager::GetMyStatus(void) const
  {
    return m_btlv->GetClientPlayerData(GetClientID());
  }

  //------------------------------------------------------------------
  /**
    * @brief   バトルのルールなどによるポケモンリストタイプ取得
    */
  //------------------------------------------------------------------
  BattleUIDataManager::PokeListType  BattleUIDataManager::GetPokeListType()
  {
    BtlRule rule  = GetBattleRule();
    switch( rule )
    {
    default:
    case BTL_RULE_SINGLE:   ///< シングル
    case BTL_RULE_INTRUDE:  ///< 乱入
      break;
    case BTL_RULE_DOUBLE:    ///< ダブル
      if( m_btlv->IsFriendExist() )
      {
        //  マルチバトル
        if( IsMultiMode() == true )
        {
          // @fix NMCat[1195] 通信対戦のみしかパートナー有の判定をしていました。
          //                  バトルツリーのマルチバトルについては考えていませんでした。
          //                  ↓の通信対戦かの判定は不要でコメントします。

          // @fix NMCat[2066] 上の対応では不十分だった、シナリオ上でのマルチバトルではパートナーのポケモンは見せないことを把握していなかった。
          //                  「パートナーのポケモンを見せる　＝　バッグが使えない状態」は条件が一致している（そもそもパートナーのポケモンにはバッグの道具を使えないから）
          //                  なのでバッグが使えないときしかパートナーのポケモンを見せないように変更
          if( IsUseBag() == false )
          {
            if( m_btlv->IsPlayerInLeftSide() )
            {
              return  MEMBER_MULTI_PARTY_L;
            }
            else
            {
              return  MEMBER_MULTI_PARTY_R;
            }
          }
        }
      }
    }

    return  MEMBER_SINGLE_PARTY;
  }

  //------------------------------------------------------------------
  /**
    * @brief   バトルの味方・敵の配置リスト一覧を作成
    * @note
    *  行動選択開始前に必ず呼び出す
    *  作成したリストは下記のケースで使用される
    *    ・行動選択画面のポケモンアイコン配置
    *    ・わざ選択の敵効果タイプ取得
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::CreatePokeList( const BtlRule rule )
  {
    m_pSelfPosPlaceList->Erase();
    m_pEnemyPosPlaceList->Erase();

    //  味方の配置
    {
      //  シングル
      if( rule == BTL_RULE_SINGLE )
      {
        m_pSelfPosPlaceList->PushBack( BTL_VPOS_NEAR_1 );
      }
      else
      //  ダブル
      if( rule == BTL_RULE_DOUBLE )
      {
        m_pSelfPosPlaceList->PushBack( BTL_VPOS_NEAR_1 );
        m_pSelfPosPlaceList->PushBack( BTL_VPOS_NEAR_2 );
      }
      else
      //  ロイヤル
      if( rule == BTL_RULE_ROYAL )
      {
        m_pSelfPosPlaceList->PushBack( BTL_VPOS_ROYAL_NEAR );
      }
      else
      //  乱入
      if( rule == BTL_RULE_INTRUDE )
      {
        m_pSelfPosPlaceList->PushBack( BTL_VPOS_NEAR_1 );
      }
    }

    //  敵の配置
    {
      //  シングル
      if( rule == BTL_RULE_SINGLE )
      {
        m_pEnemyPosPlaceList->PushBack( BTL_VPOS_FAR_1 );
      }
      else
      //  ダブル
      if( rule == BTL_RULE_DOUBLE )
      {
        m_pEnemyPosPlaceList->PushBack( BTL_VPOS_FAR_1 );
        m_pEnemyPosPlaceList->PushBack( BTL_VPOS_FAR_2 );
      }
      else
      //  ロイヤル
      if( rule == BTL_RULE_ROYAL )
      {
        m_pEnemyPosPlaceList->PushBack( BTL_VPOS_ROYAL_RIGHT );
        m_pEnemyPosPlaceList->PushBack( BTL_VPOS_ROYAL_LEFT );
        m_pEnemyPosPlaceList->PushBack( BTL_VPOS_ROYAL_FAR );
      }
      else
      //  乱入
      if( rule == BTL_RULE_INTRUDE )
      {
        m_pEnemyPosPlaceList->PushBack( BTL_VPOS_FAR_1 );
        m_pEnemyPosPlaceList->PushBack( BTL_VPOS_FAR_2 );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   バトルで使用するポケットデータ作成
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::OutputPocketData( ::app::bag::POCKET_DATA aOutputPocket[ BATTLE_BAG_POCKET_MAX ] )
  {
    Savedata::MyItem* pMyItem = GetMyItem();

    //  個数確保
    {
      for( s32 btlPocket_i = 0; btlPocket_i < BATTLE_BAG_POCKET_MAX; ++btlPocket_i )
      {
        //  メモリ確保(個数は最大値で固定)
        {
          aOutputPocket[ btlPocket_i ].table  = GFL_NEW_ARRAY( m_appUtilHeap.GetDeviceHeap()->GetLowerHandle() ) Savedata::MyItem::ITEM_ST[ POCKET_DATA_MAX ];
          aOutputPocket[ btlPocket_i ].max    = 0;
        }
      }
    }

    //  バトルポケットのデータ設定
    {
      //  全所持アイテムを検索する
      for( s32 pocket_i = 0; pocket_i < FIELD_BAG_POCKET_MAX; ++pocket_i )
      {
        //  バトルポケットごとにチェック
        for( s32 btlPocket_i = 0; btlPocket_i < BATTLE_BAG_POCKET_MAX; ++btlPocket_i )
        {
          _UpdateBtlPocketItemData( &aOutputPocket[ btlPocket_i ], (item::BtlPocket)btlPocket_i, pocket_i, pMyItem );
        }
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   バトルで使用するポケットデータ作成
    * @note
    *   出力するバトルポケットを指定
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::OutputPocketData( ::app::bag::POCKET_DATA* pOutputPocketList, const item::BtlPocket btlPocketType )
  {
    GFL_ASSERT( pOutputPocketList );
    GFL_ASSERT( pOutputPocketList->table == NULL );

    pOutputPocketList->table  = GFL_NEW_ARRAY( m_appUtilHeap.GetDeviceHeap()->GetLowerHandle() ) Savedata::MyItem::ITEM_ST[ POCKET_DATA_MAX ];
    pOutputPocketList->max    = 0;

    Savedata::MyItem* pMyItem = GetMyItem();

    //  バトルポケットのデータ設定
    {
      //  全所持アイテムを検索する
      for( u32 pocket_i = 0; pocket_i < FIELD_BAG_POCKET_MAX; ++pocket_i )
      {
        _UpdateBtlPocketItemData( pOutputPocketList, btlPocketType, pocket_i, pMyItem );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   バトルで使用するポケットデータ作成して出力したのを後処理する
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::DeletePocketData( ::app::bag::POCKET_DATA aOutputPocket[ BATTLE_BAG_POCKET_MAX ] )
  {
    for( s32 btlPocket_i = 0; btlPocket_i < BATTLE_BAG_POCKET_MAX; ++btlPocket_i )
    {
      GFL_SAFE_DELETE_ARRAY( aOutputPocket[ btlPocket_i ].table );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   わざからポケモン配置対象に対する効果タイプを取得
     * @param[out]  pPutFlg           : 表示可能
     * @param[in]   wazaNo            : 攻撃側が使用する技NO
     * @param[in]   attackerPokeId    : 攻撃側のポケモンid
     * @param[in]   rDefenderPokeList : 防御側のポケモンたちの位置
     * @return      相性値
    */
  //------------------------------------------------------------------
  BtlTypeAffAbout BattleUIDataManager::GetPokePosListKoukaType( bool* pPutFlg, const WazaNo wazaNo, const u32 attackerPokeId, gfl2::util::List<BtlvPos>& rDefenderPokeList )
  {
    if( pPutFlg == NULL )
    {
      GFL_ASSERT( 0 );
      return  pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;
    }

    *pPutFlg  = false;

    //  モジュールがない場合はノーマルを返す
    if( m_btlv->GetMainModule() == NULL )
    {
      return  pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;
    }

    //  各効果の優先順位値(高い値ほど優先して設定する)
    static const u32 s_koukaRankMapTableMax = 4;
    const s32 aKoukaRankMapTable[ s_koukaRankMapTableMax ][ 2 ]  =
    {
      { pml::battle::TypeAffinity::TYPEAFF_ABOUT_NONE,         0 },
      // @fix GFNMCat[2388] 「効果あり」については表示優先を一番下にしていた。（表示しない対象と思っていたから除外しても良いと考えていた）
      { pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL,       2 },
      { pml::battle::TypeAffinity::TYPEAFF_ABOUT_ADVANTAGE,    3 },
      { pml::battle::TypeAffinity::TYPEAFF_ABOUT_DISADVANTAGE, 1 },
    };

    BtlTypeAffAbout nowTypeAffAbout = pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;

    BTL_POKEPARAM*  pParam  = NULL;
    gfl2::util::ListIterator<BtlvPos> ite = rDefenderPokeList.Begin();
    u32 count = 0;
    while( ite != rDefenderPokeList.End() )
    {
      pParam  = GetBattlePokeParam( *ite );
      if( pParam == NULL )
      {
      }
      else
      if( pParam->IsDead() == true )
      {
        //  死亡している相手は効果取得の非対象
      }
      else
      {
        bool  bKoukaPutFlg  = false;
        BtlTypeAffAbout newTypeAffAbout = GetPokePosListKoukaType( &bKoukaPutFlg, wazaNo, attackerPokeId, pParam->GetID() );
        if( bKoukaPutFlg == true )
        {
          //  一度でも効果表示可能であればフラグを設定
          *pPutFlg  = true;

          if( nowTypeAffAbout != newTypeAffAbout )
          {
            //  初回は優先度判定はいらない
            if( count == 0 )
            {
              nowTypeAffAbout = newTypeAffAbout;
            }
            else
            {
              s32 oldRankNum  = -1;
              s32 newRankNum  = -1;

              //  現在設定している効果と新しく設定した効果の優先度をチェックして、
              //  新しく設定した効果の優先どが高い場合は新しい効果に上書き
              for( u32 i = 0; i < s_koukaRankMapTableMax; ++i )
              {
                if( aKoukaRankMapTable[ i ][ 0 ] == nowTypeAffAbout )
                {
                  oldRankNum  = aKoukaRankMapTable[ i ][ 1 ];
                }

                if( aKoukaRankMapTable[ i ][ 0 ] == newTypeAffAbout )
                {
                  newRankNum  = aKoukaRankMapTable[ i ][ 1 ];
                }
              }

              //  新しい効果のラインが高い場合は上書き
              if( oldRankNum < newRankNum )
              {
                nowTypeAffAbout = newTypeAffAbout;
              }
            }
          }

          ++count;
        }
      }

      ++ite;
    }

    return  nowTypeAffAbout;
  }

  //------------------------------------------------------------------
  /**
    * @brief   わざからポケモンタイプに対する効果タイプを取得(自分自身のを取得)
     * @param[out]  pPutFlg           : 表示可能
     * @param[in]   wazaNo            : 攻撃側が使用する技NO
     * @param[in]   attackerPokeId    : 攻撃側のポケモンid
     * @@aram[in]   defPokeId         : 防御側のポケモンid
     * @return    相性値
    */
  //------------------------------------------------------------------
  BtlTypeAffAbout BattleUIDataManager::GetPokePosListKoukaType( bool* pPutFlg, const WazaNo wazaNo, const u32 attackerPokeId, const u32 defPokeId )
  {
    if( pPutFlg == NULL )
    {
      GFL_ASSERT( 0 );
      return  pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;
    }

    return  m_btlv->CalcWazaAffinityAbout( attackerPokeId, defPokeId, wazaNo, pPutFlg );
  }

  //------------------------------------------------------------------
  /**
    * @brief   アイテム管理インスタンス作成
    */
  //------------------------------------------------------------------
  itemman::ITEM_MANAGER* BattleUIDataManager::CreateInstanceItemManager()
  {
    if( m_pItemManager != NULL )
    {
      return  m_pItemManager;
    }

    m_pItemManager  = GFL_NEW_LOW( m_appUtilHeap.GetDeviceHeap() ) itemman::ITEM_MANAGER( m_appUtilHeap.GetDeviceHeap(), 1 );

    return  m_pItemManager;
  }

  //------------------------------------------------------------------
  /**
    * @brief   アイテム管理インスタンス破棄
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::DeleteInstanceItemManager()
  {
    GFL_SAFE_DELETE( m_pItemManager );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ターン内でのユニークボタン入力フラグをクリア
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::ClearUniquPushBtnFlg( const s32 pokePartyListIdx )
  {
    m_aUniquPushBtnList[ pokePartyListIdx ].bMega     = 0;
    m_aUniquPushBtnList[ pokePartyListIdx ].bZenryoku = 0;
    m_aUniquPushBtnList[ pokePartyListIdx ].bUltraBurst = 0;
  }

  void BattleUIDataManager::ClearAllUniquPushBtnFlg()
  {
    gfl2::std::MemClear( m_aUniquPushBtnList, sizeof( m_aUniquPushBtnList ) );      
  }

  //  指定したpoke_idxがメガ進化できるかどうか
  bool BattleUIDataManager::IsMegaPoke( const s32 self_party_poke_idx )
  {
    u8  selfClientID  = m_btlv->GetClientID();

    // @fix MCT要望 自分操作ポケモンの場合はStatusが入っている(AIコントロールターン時はデバッグ用なので重複チェックはしない)
    if( IsAICtrlTurn() == false )
    {
      // @fix NMCat[1734] すでにメガ進化しているかどうかの判定はバトルロジック側のシステムで判定するように変更
      if( m_btlv->IsMegaEvolved( selfClientID ) == true )
      {
        return  false;
      }

      //  現在のターンですでにメガ進化を選択しているか
      {
        u32 loopNum = GFL_NELEMS( m_aUniquPushBtnList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( m_aUniquPushBtnList[ i ].bMega == 1 )
          {
            return  false;
          }
        }
      }
    }

    //  自身のパーティデータ
    const BTL_PARTY* pSelfParty = GetBattleParty_Self();
    GFL_ASSERT( pSelfParty );

    //  バトルのポケモンデータ（バトル中のデータ反映）
    const BTL_POKEPARAM*  pBtlParam = pSelfParty->GetMemberDataConst( self_party_poke_idx );
    GFL_ASSERT( pBtlParam );

    //  データ変更前のオリジナルデータ
    const pml::pokepara::PokemonParam*  pSrcPokeParam = pBtlParam->GetSrcData();
    GFL_ASSERT( pSrcPokeParam );

    //  メガ進化のボタンを出すか
    bool  bMegaBtn = false;
    {
      bMegaBtn = ( 0 < pSrcPokeParam->GetMegaFormCount() );
    }

    //  メガ進化できるかどうか
    if( bMegaBtn )
    { 
      if( pSrcPokeParam->CanMegaEvolve() )
      {
        //  プレイヤーのデータ取得
        const Savedata::MyStatus* pMyStatus;
        //momiji バトルフェス対応
        if (m_btlv->IsBattleFesMode())
        {
          //PokeIDがバトルフェスの何人目のトレーナーかと一致している
          pMyStatus = m_btlv->GetClientPlayerData(selfClientID, pBtlParam->GetID());
        }
        else
        {
          pMyStatus = m_btlv->GetClientPlayerData(selfClientID);
        }

        if( pMyStatus != NULL )
        {
          if( pMyStatus->HasMegaRing() )
          {
            return  true;
          }
        }
        else
        {
          // @fix MCT要望 AIコントローラーの時はここに
          return  true;
        }
      }
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   メガ進化ボタンを押しているかどうかの設定
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::PushMegaBtn( const s32 pokePartyListIdx )
  {
    m_aUniquPushBtnList[ pokePartyListIdx ].bMega  = 1;
  }

  //  指定したpoke_idxがウルトラバーストできるかどうか
  bool BattleUIDataManager::IsUltraBurstPoke(const s32 self_party_poke_idx)
  {
    u8  selfClientID = m_btlv->GetClientID();

    // @fix MCT要望 自分操作ポケモンの場合はStatusが入っている(AIコントロールターン時はデバッグ用なので重複チェックはしない)
    if (IsAICtrlTurn() == false)
    {
      // @fix NMCat[1734] すでにウルトラバーストしているかどうかの判定はバトルロジック側のシステムで判定するように変更
      if (m_btlv->IsUltraBursted(selfClientID) == true)
      {
        return  false;
      }

      //  現在のターンですでにウルトラバーストを選択しているか
      {
        u32 loopNum = GFL_NELEMS(m_aUniquPushBtnList);
        for (u32 i = 0; i < loopNum; ++i)
        {
          if (m_aUniquPushBtnList[i].bUltraBurst == 1)
          {
            return  false;
          }
        }
      }
    }

    //  自身のパーティデータ
    const BTL_PARTY* pSelfParty = GetBattleParty_Self();
    GFL_ASSERT(pSelfParty);

    //  バトルのポケモンデータ（バトル中のデータ反映）
    const BTL_POKEPARAM*  pBtlParam = pSelfParty->GetMemberDataConst(self_party_poke_idx);
    GFL_ASSERT(pBtlParam);

    //  データ変更前のオリジナルデータ
    const pml::pokepara::PokemonParam*  pSrcPokeParam = pBtlParam->GetSrcData();
    GFL_ASSERT(pSrcPokeParam);

    if(pSrcPokeParam->GetMonsNo() == MONSNO_PURIZUMU &&
      (pSrcPokeParam->GetFormNo() == FORMNO_PURIZUMU_FORM_S || pSrcPokeParam->GetFormNo() == FORMNO_PURIZUMU_FORM_L))
    {
      if (pBtlParam->GetItem() == ITEM_NEKUROZUMAz_1)
      {
        return true;
      }
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
  * @brief   ウルトラバーストボタンを押しているかどうかの設定
  */
  //------------------------------------------------------------------
  void BattleUIDataManager::PushUltraBurstBtn(const s32 pokePartyListIdx)
  {
    m_aUniquPushBtnList[pokePartyListIdx].bUltraBurst = 1;
  }

  //  全力技ができるか
  bool BattleUIDataManager::IsZenryoku( const s32 pokePartyListIdx, const BTL_POKEPARAM* pBtlParam, const btl::ZenryokuWazaStatus*  pAuraWazaStatus )
  {
    if( pBtlParam == NULL )
    {
      GFL_ASSERT( pBtlParam );
      return  false;
    }

    // @fix MCT要望 自分操作ポケモンの場合はStatusが入っている(AIコントロールターン時はデバッグ用なので重複チェックはしない)
    if( IsAICtrlTurn() == false )
    //  現在ターンですでに全力技を選択しているか
    {
      u32 loopNum = GFL_NELEMS( m_aUniquPushBtnList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( m_aUniquPushBtnList[ i ].bZenryoku == 1 )
        {
          return  false;
        }
      }
    }

    if( pAuraWazaStatus != NULL )
    {
      //  オーラを使っていない場合はオーラが使えるかチェック
      //  仕様では一度使うと現在の戦闘では二度と使えない
      if( pAuraWazaStatus->IsZenryokuWazaUsed() == false )
      {
        // @fix NMCat[891]  技が全力技を使えるどうかの判定も加えていたが、これを削除。
        //                  なぜなら技が全力技にならなくてもＺボタンを押せるケースもあるので

        // @fix GFNMCat[2647] ポケモンがＺクリスタル所持の判定が抜けていた
        return  btl::ZenryokuWaza::HaveZenryokuWazaItem( *pBtlParam );
      }
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   全力技ボタンを押しているかどうかの設定
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::PushZenryokuBtn( const s32 pokePartyListIdx )
  {
    m_aUniquPushBtnList[ pokePartyListIdx ].bZenryoku = 1;
  }

  /*
    @brief 技選択リストでわるあがきが、あるか
    @note
      わるあがきを発動させるかどうか
  */
  bool BattleUIDataManager::GetWazaSelectIdxByWaruagaki( const s32 self_party_poke_idx )
  {
    //  自身のパーティデータ
    const BTL_PARTY* pSelfParty = GetBattleParty_Self();
    GFL_ASSERT( pSelfParty );

    //  バトルのポケモンデータ（バトル中のデータ反映）
    const BTL_POKEPARAM*  pBtlParam = pSelfParty->GetMemberDataConst( self_party_poke_idx );
    GFL_ASSERT( pBtlParam );

    //  わざ選択にわるあがきがあるか
    WazaID  wazaId    = WAZANO_NULL;
    u8      ppNum     = 0;
    for( s32 i = 0; i < pBtlParam->WAZA_GetCount(); ++i )
    {
      wazaId  = pBtlParam->WAZA_GetID( i );
      ppNum   = pBtlParam->WAZA_GetPP( i );

      if( ( wazaId != WAZANO_NULL ) && ( 0 < ppNum ) )
      {
        if( m_btlv->IsUnselectableWaza( pBtlParam, wazaId ) == false )
        {
          return  false;
        }
      }
    }

    //  すべてのPPが０ならわるあがき発動
    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   バトルUI画面で使用する特性のタイトルとインフォを文字列出力
    * @param[out]  pOutputTitleStrBuf  特性名のテキスト出力ポインタ
    * @param[out]  pOutputInfoStrBuf   特性の詳細テキスト出力ポインタ
    * @param[in]   tokuseiId           テキスト出力対象の特性id
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::OutputTokuseiStrBuf( gfl2::str::StrBuf* pOutputTitleStrBuf, gfl2::str::StrBuf* pOutputInfoStrBuf, const u32 tokuseiId )
  {
    GFL_ASSERT( tokuseiId < TOKUSEI_NUM );
    GFL_ASSERT( TOKUSEI_NULL < tokuseiId );

    if( pOutputTitleStrBuf != NULL )
    {
      gfl2::str::StrBuf*  pTmpStrBuf    = GetTempStrBuf( 1 );
      GFL_ASSERT( pTmpStrBuf != pOutputTitleStrBuf );

      m_pWordSet->RegisterTokuseiName( 0, tokuseiId );
      m_pAppMsgData->GetString( STR_TOKUSEI_NAME, *pTmpStrBuf );
      m_pWordSet->Expand( pOutputTitleStrBuf, pTmpStrBuf );
    }

    if( pOutputInfoStrBuf != NULL )
    {
      m_pTokuseInfoMsgData->GetString( tokuseiId, *pOutputInfoStrBuf );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   バトルUI画面で使用するアイテムのタイトルとインフォを文字列出力
    * @param[out]  pOutputTitleStrBuf  アイテム名のテキスト出力ポインタ
    * @param[out]  pOutputInfoStrBuf   アイテムの詳細テキスト出力ポインタ
    * @param[in]   itemId              テキスト出力対象のアイテムid
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::OutputItemStrBuf( gfl2::str::StrBuf* pOutputTitleStrBuf, gfl2::str::StrBuf* pOutputInfoStrBuf, const u32 itemId )
  {
    if( pOutputTitleStrBuf != NULL )
    {
      gfl2::str::StrBuf*  pTmpStrBuf    = GetTempStrBuf( 1 );
      GFL_ASSERT( pOutputTitleStrBuf != pTmpStrBuf );

      m_pWordSet->RegisterItemName( 0, itemId, 1 );
      m_pAppMsgData->GetString( STR_ITEM_NAME, *pTmpStrBuf );
      m_pWordSet->Expand( pOutputTitleStrBuf, pTmpStrBuf );
    }

    if( pOutputInfoStrBuf != NULL )
    {
      m_pItemInfoMsgData->GetString( itemId, *pOutputInfoStrBuf );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   バトルUI画面で使用する状態のタイトルとインフォを文字列出力
    * @param[out]  pOutputTitleStrBuf  状態名のテキスト出力ポインタ
    * @param[out]  pOutputInfoStrBuf   状態の詳細テキスト出力ポインタ
    * @param[in]   titleMsgId          状態のタイトルテキストid
    * @param[in]   infoMsgId           状態のインフォテキストid
    * @param[in]   pPokeParam          現在対象のポケモンパラメータ
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::OutputStatusStrBuf( gfl2::str::StrBuf* pOutputTitleStrBuf, gfl2::str::StrBuf* pOutputInfoStrBuf, const u32 titleMsgId, const u32 infoMsgId, const BTL_POKEPARAM* pPokeParam )
  {
    if( pOutputTitleStrBuf != NULL )
    {
      gfl2::str::StrBuf*  pTmpStrBuf    = GetTempStrBuf( 1 );
      GFL_ASSERT( pTmpStrBuf != pOutputTitleStrBuf );

      m_pWordSet->RegisterWord( 0, m_pStatusMsgData, titleMsgId );
      m_pAppMsgData->GetString( STR_BTL_STATE_NAME, *pTmpStrBuf );
      m_pWordSet->Expand( pOutputTitleStrBuf, pTmpStrBuf );
    }

    if( pOutputInfoStrBuf != NULL )
    {
      //  詳細テキストで置換が必要なのがある。
      /*
      //  ほろびの歌は使わない
      if( infoMsgId == BTR_STATE_47_02 )
      {
        //  残りターン数に置換

        //  ほろびの歌
        gfl2::str::StrBuf*  pTmpStrBuf    = GetTempStrBuf( 1 );
        GFL_ASSERT( pTmpStrBuf != pOutputInfoStrBuf );

        m_pWordSet->RegisterNumber( 0, pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_HOROBINOUTA ), 1, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
        m_pStatusMsgData->GetString( infoMsgId, *pTmpStrBuf );
        m_pWordSet->Expand( pOutputInfoStrBuf, pTmpStrBuf );
      }
      else
      */
      
      //  テキスト内容は異なるが、置換の書式は同じなので問題ない
      if( ( infoMsgId == BTR_STATE_24_02 )
      ||  ( infoMsgId == BTR_STATE_16_02 ) )
      {
        btl::WazaSick wazaSick  = pml::wazadata::WAZASICK_ENCORE;
        // @fix NMCat[1297] 技説明テキストidを使って技idを取得する
        if( infoMsgId == BTR_STATE_24_02 )
        {
          wazaSick  = pml::wazadata::WAZASICK_KANASIBARI;
        }
        else
        if( infoMsgId == BTR_STATE_16_02 )
        {
          wazaSick  = pml::wazadata::WAZASICK_ENCORE;
        }
        
        // @fix 森本さん要望　かなしばりとアンコール状態では対象技を表記するようにする
        const BTL_SICKCONT& sickcont = pPokeParam->GetSickCont( wazaSick );
        u16 wazano = SICKCONT_GetParam( sickcont );

        gfl2::str::StrBuf*  pTmpStrBuf    = GetTempStrBuf( 1 );
        GFL_ASSERT( pTmpStrBuf != pOutputInfoStrBuf );

        m_pWordSet->RegisterWazaName( 0, wazano );
        m_pStatusMsgData->GetString( infoMsgId, *pTmpStrBuf );
        m_pWordSet->Expand( pOutputInfoStrBuf, pTmpStrBuf );
      }
      else
      {
        m_pStatusMsgData->GetString( infoMsgId, *pOutputInfoStrBuf );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief      通信対戦トレーナー数を取得
    * @return     登場しているトレーナ数を取得
    */
  //------------------------------------------------------------------
  u32 BattleUIDataManager::GetEnemyTrainerNum()
  {
    return  m_btlv->GetEnemyClientNum( GetClientID() );
  }

  //------------------------------------------------------------------
  /**
    * @brief      通信対戦の情報を取得
    * @param[out] pOutputParam: パラメータ出力用のアドレス
    * @param[in]  idx: 対戦情報idx
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::OutputEnemyTrainerParam( ENEMY_TRAINER_PARAM* pOutputParam, const u32 idx )
  {
    if( pOutputParam == NULL )
    {
      return;
    }

    
    ENEMY_TRAINER_PARAM retParam;
    u8  clientId  = m_btlv->GetEnemyClientID( idx );

    pOutputParam->pName        = m_btlv->GetClientTrainerName( &retParam.grammer, clientId );
    pOutputParam->ratingNum    = m_btlv->GetClientRating( clientId );
    pOutputParam->pInfoParam   = m_btlv->GetClientPublicInformation( clientId );
    pOutputParam->bRating      = m_btlv->IsClientRatingDisplay();
  }

  //------------------------------------------------------------------
  /**
    * @brief  所持しているモンスターボールリストの中の先頭のidを取得
    * @return 所持しているモンスターボールリストの先頭のアイテムid / 所持していない場合は Savedata::MyItem::ITEM_NONE を返す
    */
  //------------------------------------------------------------------
  u32 BattleUIDataManager::GetHaveFirstListMonsterBallId()
  {
    Savedata::MyItem* pMyItem = GetMyItem();

    const Savedata::MyItem::ITEM_ST*  pItemData = NULL;
    u32 itemCountNum  = 0;
    u32 firstBallItemId = Savedata::MyItem::ITEM_NONE;

    u32 lastUseBallItemId = GetLastShortcutUseBallItemId();
    for( u32 pocket_i = 0; pocket_i < FIELD_BAG_POCKET_MAX; ++pocket_i )
    {
      itemCountNum  = pMyItem->GetItemCount( pocket_i );

      //  バッグのポケットをひとつずつチェックして、バトルポケットに該当するのがあれば設定
      //  線形探索しないとリスト先頭のidが取れない
      for( u32 item_i = 0; item_i < itemCountNum; ++item_i )
      {
        pItemData = pMyItem->GetItem( pocket_i, item_i );
        if( item::CheckBtlPocketInclude( item::BTLPOCKET_BALL, pItemData->id ) )
        {
          //  見つけた

          //  最初に見つけたのを保存
          if( firstBallItemId == Savedata::MyItem::ITEM_NONE )
          {
            firstBallItemId = pItemData->id;
          }

          //  最後につかったボールアイテムidがない場合はすぐに返す
          if( lastUseBallItemId == GameSys::GameData::LAST_SHORT_CUT_USE_BALL_ITEM_ID_NONE )
          {
            return  firstBallItemId;
          }
          //  最後に使ったボールアイテムidがある場合はそれを返す
          else
          if( lastUseBallItemId == pItemData->id )
          {
            return  pItemData->id;
          }
        }
      }
    }

    //  ここにきたらボールアイテムがないか、最後に使ったボールアイテムがないのかのどちらかになる
    return  firstBallItemId;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ポケモンのステータスリスト更新
    * @note    種類が５０以上あり、それらのデータを線形探索でデータチェックしている上にバッファで保存しているかどうかを見ないといけないのでループ回数が多くなる
    *          行動選択開始時の一度のみしか呼ばないようにするべき
    */
  //------------------------------------------------------------------
  const BattleUIDataManager::POKE_STATUS_DATA* BattleUIDataManager::UpdatePokeStatusList( const BTL_POKEPARAM* pPokeParam, const BtlvPos btlvPos )
  {
    GFL_ASSERT( pPokeParam );

    const BattleViewSystem* pViewSys = GetCore();

    POKE_STATUS_DATA* pPokeStatusData = NULL;
    {
      //  パラメータの固有idをリストデータに割り振る
      s32 uniqId  = (s32)pPokeParam->GetID();
      u32 loopNum = GFL_NELEMS( m_aStatusDataBuffer );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( m_aStatusDataBuffer[ i ].pokeDataId == -1 )
        {
          //  割り振っていないのがあったので割り振る
          pPokeStatusData = &m_aStatusDataBuffer[ i ];
          pPokeStatusData->pokeDataId = uniqId;
          break;
        }
        else
        if( m_aStatusDataBuffer[ i ].pokeDataId == uniqId )
        {
          //  すでに割り振っている
          pPokeStatusData = &m_aStatusDataBuffer[ i ];
          break;
        }
      }
    }

    if( pPokeStatusData == NULL )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    const FieldStatus* pFieldStatus = pViewSys->GetFieldStatus();
    if( pFieldStatus == NULL )
    {
      return  pPokeStatusData;
    }


    //  ポケモン状態
    {
      struct _tag_poke_status_data_table
      {
        bool        bHit;
        StatusType  type;
        u32         title_msg_id;
        u32         info_msg_id;

        u32         num, turnMax;
      };

      BtlGround   ground  = pFieldStatus->GetGround();
      BtlWeather  weather = pFieldStatus->GetWeather();

      BtlPokePos  pokePos = pViewSys->ViewPosToBtlPos( btlvPos );

      //  天候残りターン数/天候最大ターン数
      u32         weatherTurnNum    = 0;
      u32         weatherTuneMaxNum = 0;
      pViewSys->GetUiDisplay_Turn_Weather( &weatherTurnNum, &weatherTuneMaxNum );

      //  地形残りターン数/地形最大ターン数
      u32         groundTurnNum     = 0;
      u32         groundTurnMaxNum  = 0;
      pViewSys->GetUiDisplay_Turn_Ground( &groundTurnNum, & groundTurnMaxNum );

      //  滅びの歌の最大ターン数
      u8          horobinoutaTurnMax  = 0;
      {
        const BTL_SICKCONT& sickcont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_HOROBINOUTA );
        horobinoutaTurnMax  = SICCONT_GetTurnMax( sickcont );
      }

      //  サイド取得
      BtlSide side  = m_btlv->ViewPosToBtlSide( btlvPos );

      //  アンコール情報取得
      const BTL_SICKCONT& sickEncoreCont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_ENCORE );
      u8 sickEncoreTurnMaxNum = SICCONT_GetTurnMax( sickEncoreCont );
      u8 sickEncoreTurnNowNum = sickEncoreTurnMaxNum - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_ENCORE );

      //  回復封じ情報取得
      const BTL_SICKCONT& sickKaihukuHuujiCont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_KAIHUKUHUUJI );
      u8 sickKaihukuHuujiTurnMaxNum = SICCONT_GetTurnMax( sickKaihukuHuujiCont );
      u8 sickKaihukuHuujiTurnNowNum = sickKaihukuHuujiTurnMaxNum - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_KAIHUKUHUUJI );

      //  かなしばり情報取得
      const BTL_SICKCONT& sickKanashibariCont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_KANASIBARI );
      u8 sickKanashibariTurnMaxNum = SICCONT_GetTurnMax( sickKanashibariCont );
      u8 sickKanashibariTurnNowNum = sickKanashibariTurnMaxNum - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_KANASIBARI );

      //  さしおさえ情報取得
      const BTL_SICKCONT& sickSasiosaeCont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_SASIOSAE );
      u8 sickSasiosaeTurnMaxNum = SICCONT_GetTurnMax( sickSasiosaeCont );
      u8 sickSasiosaeTurnNowNum = sickSasiosaeTurnMaxNum - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_SASIOSAE );

      //  ちょうはつ情報取得
      const BTL_SICKCONT& sickTyouhatsuCont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_TYOUHATSU );
      u8 sickTyouhatsuTurnMaxNum = SICCONT_GetTurnMax( sickTyouhatsuCont );
      u8 sickTyouhatsuTurnNowNum = sickTyouhatsuTurnMaxNum - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_TYOUHATSU );

      //  テレキネシス情報取得
      const BTL_SICKCONT& sickTelekinesisCont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_TELEKINESIS );
      u8 sickTelekinesisTurnMaxNum = SICCONT_GetTurnMax( sickTelekinesisCont );
      u8 sickTelekinesisTurnNowNum = sickTelekinesisTurnMaxNum - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_TELEKINESIS );

      //  でんじふゆう情報取得
      const BTL_SICKCONT& sickFlyingCont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_FLYING );
      u8 sickFlyingTurnMaxNum = SICCONT_GetTurnMax( sickFlyingCont );
      u8 sickFlyingTurnNowNum = sickFlyingTurnMaxNum - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_FLYING );

      //  さわぐ情報取得
      const BTL_SICKCONT& sickSawaguCont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_SAWAGU );
      u8 sickSawaguTurnMaxNum = SICCONT_GetTurnMax( sickSawaguCont );
      u8 sickSawaguTurnNowNum = sickSawaguTurnMaxNum - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_SAWAGU );

      /*  // @fix NMCat[1337] 仕様変更によりもうどくはターン数表示ではなくなった
      //  もうどく情報取得
      const BTL_SICKCONT& sickDokudokuCont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_DOKU );
      u8 sickDokudokuTurnMaxNum = SICCONT_GetTurnMax( sickDokudokuCont );
      u8 sickDokudokuTurnNowNum = sickDokudokuTurnMaxNum - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_DOKU );
      */

      //  じごくづきの情報取得
      const BTL_SICKCONT& sickZigokudukiCont = pPokeParam->GetSickCont( pml::wazadata::WAZASICK_ZIGOKUDUKI );
      u8 sickZigokudukiTurnMaxNum = SICCONT_GetTurnMax( sickZigokudukiCont );
      u8 sickZigokudukiTurnNowNum = sickZigokudukiTurnMaxNum - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_ZIGOKUDUKI );

      //  おいかぜサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectOikazeStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_OIKAZE );
      GFL_ASSERT( pSideEffectOikazeStatus );

      u32 effectOikazeTurnMaxNum  = pSideEffectOikazeStatus->GetMaxTurnCount();
      u32 effectOikazaTurnNowNum  = effectOikazeTurnMaxNum - pSideEffectOikazeStatus->GetCurrentTurnCount();

      //  おまじないサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectOmajinaiStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_OMAJINAI );
      GFL_ASSERT( pSideEffectOmajinaiStatus );

      // @fix NMCat[1356] おまじないにターン数を表示するように仕様変更
      u32 effectOmajinaiTurnMaxNum  = pSideEffectOmajinaiStatus->GetMaxTurnCount();
      u32 effectOmajinaiTurnNowNum  = effectOmajinaiTurnMaxNum - pSideEffectOmajinaiStatus->GetCurrentTurnCount();

      //  しろいきりサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectShiroikiriStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_SIROIKIRI );
      GFL_ASSERT( pSideEffectShiroikiriStatus );

      u32 effectShiroikiriTurnMaxNum  = pSideEffectShiroikiriStatus->GetMaxTurnCount();
      u32 effectShiroikiriTurnNowNum  = effectShiroikiriTurnMaxNum - pSideEffectShiroikiriStatus->GetCurrentTurnCount();

      //  しんぴのまもりサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectSinpinomamoriStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_SINPINOMAMORI );
      GFL_ASSERT( pSideEffectSinpinomamoriStatus );

      u32 effectSinpinomamoriTurnMaxNum  = pSideEffectSinpinomamoriStatus->GetMaxTurnCount();
      u32 effectSinpinomamoriTurnNowNum  = effectSinpinomamoriTurnMaxNum - pSideEffectSinpinomamoriStatus->GetCurrentTurnCount();

      //  ステルロックサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectStealthrRookStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_STEALTHROCK );
      GFL_ASSERT( pSideEffectStealthrRookStatus );

      //  どくびしサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectDokubisiStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_DOKUBISI );
      GFL_ASSERT( pSideEffectDokubisiStatus );

      //  ねばねばネットサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectNebanebaNetStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_NEBANEBANET );
      GFL_ASSERT( pSideEffectNebanebaNetStatus );

      //  ひかりのかべサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectHIkarinokabeStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_HIKARINOKABE );
      GFL_ASSERT( pSideEffectHIkarinokabeStatus );

      u32 effectHIkarinokabeTurnMaxNum  = 0;
      u32 effectHIkarinokabeTurnNowNum  = 0;
      pViewSys->GetUiDisplay_Turn_Hikarinokabe( &effectHIkarinokabeTurnNowNum, &effectHIkarinokabeTurnMaxNum, side );

      //  リフレクターサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectRefrectorStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_REFRECTOR );
      GFL_ASSERT( pSideEffectRefrectorStatus );

      u32 effectRefrectorTurnMaxNum  = 0;
      u32 effectRefrectorTurnNowNum  = 0;
      pViewSys->GetUiDisplay_Turn_Reflector( &effectRefrectorTurnNowNum, &effectRefrectorTurnMaxNum, side );

      //  まきびしサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectMakibisiStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_MAKIBISI );
      GFL_ASSERT( pSideEffectMakibisiStatus );

      //  にじサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectRaibowStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_RAINBOW );
      GFL_ASSERT( pSideEffectRaibowStatus );

      u32 effectRaibowTurnMaxNum  = pSideEffectRaibowStatus->GetMaxTurnCount();
      u32 effectRaibowTurnNowNum  = effectRaibowTurnMaxNum - pSideEffectRaibowStatus->GetCurrentTurnCount();

      //  しつげんサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectMoorStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_MOOR );
      GFL_ASSERT( pSideEffectMoorStatus );

      u32 effectMoorTurnMaxNum  = pSideEffectMoorStatus->GetMaxTurnCount();
      u32 effectMoorTurnNowNum  = effectMoorTurnMaxNum - pSideEffectMoorStatus->GetCurrentTurnCount();

      //  オーロラベールのサイドエフェクト取得
      const btl::SideEffectStatus* pSideEffectAuraVeilStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_AURORAVEIL );
      GFL_ASSERT( pSideEffectMoorStatus );

      u32 effectAuraVeilTurnMaxNum  = 0;
      u32 effectAuraVeilTurnNowNum  = 0;
      pViewSys->GetUiDisplay_Turn_AuroraVeil( &effectAuraVeilTurnNowNum, &effectAuraVeilTurnMaxNum, side );

      //  ひのうみサイドエフェクト状態取得
      const btl::SideEffectStatus* pSideEffectBurningStatus = pViewSys->GetSideEffectStatus( side, BTL_SIDEEFF_BURNING );
      GFL_ASSERT( pSideEffectBurningStatus );

      u32 effectBurningTurnMaxNum  = pSideEffectBurningStatus->GetMaxTurnCount();
      u32 effectBurningTurnNowNum  = effectBurningTurnMaxNum - pSideEffectBurningStatus->GetCurrentTurnCount();


      //  ねがいごと位置エフェクト状態取得
      const btl::PosEffectStatus* pPosEffectNegaigotoStatus = pViewSys->GetPosEffectStatus( pokePos, BTL_POSEFF_NEGAIGOTO );
      GFL_ASSERT( pPosEffectNegaigotoStatus );

      //  みらいよち/はめつのねがい位置エフェクト状態取得
      u32 delayAttackNowTurnNum = 0;
      u32 delayAttackMaxTurnNum = 0;
      const btl::PosEffectStatus* pPosEffectDelayAttackStatus = pViewSys->GetPosEffectStatus( pokePos, BTL_POSEFF_DELAY_ATTACK );
      GFL_ASSERT( pPosEffectDelayAttackStatus );
      {
        const PosEffect::EffectParam& rPosEffectDelayAttackParam  = pPosEffectDelayAttackStatus->GetEffectParam();

        //  [fix]NMCat[1367]:状態開始ターン時にはすでに-1になっているので発動ターンでは0になる。これを避けるために+1することで回避
        delayAttackNowTurnNum = ( rPosEffectDelayAttackParam.DelayAttack.execTurnMax - rPosEffectDelayAttackParam.DelayAttack.execTurnCount ) + 1;
        delayAttackMaxTurnNum = rPosEffectDelayAttackParam.DelayAttack.execTurnMax;
      }

      //  タイプ拡張しているか
      BTL_POKEPARAM::ExTypeCause  exTypeCause = BTL_POKEPARAM::EXTYPE_CAUSE_NONE;
      if( pPokeParam->HaveExType() == true )
      {
        // @fix NMCat[4067] 死亡時はタイプ拡張取得は無視
        if( pPokeParam->IsDead() == false )
        {
          //  タイプ拡張しているのはどれか
          exTypeCause = pPokeParam->GetExTypeCause();
        }
      }

      const _tag_poke_status_data_table aStatusDataTable[]  =
      {
        //  天候状態

        //  にほんばれ
        { ( weather == BTL_WEATHER_SHINE ),                                       STATUS_TYPE_TURN, BTR_STATE_01_01, BTR_STATE_01_02, weatherTurnNum, weatherTuneMaxNum },

        //  あまごい
        { ( weather == BTL_WEATHER_RAIN ),                                        STATUS_TYPE_TURN, BTR_STATE_02_01, BTR_STATE_02_02, weatherTurnNum, weatherTuneMaxNum },

        //  すなあらし
        { ( weather == BTL_WEATHER_SAND ),                                        STATUS_TYPE_TURN, BTR_STATE_03_01, BTR_STATE_03_02, weatherTurnNum, weatherTuneMaxNum },

        //  あられ
        { ( weather == BTL_WEATHER_SNOW ),                                        STATUS_TYPE_TURN, BTR_STATE_04_01, BTR_STATE_04_02, weatherTurnNum, weatherTuneMaxNum },

        //  フィールド状態

        //  エレキフィールド
        { ( ground == BTL_GROUND_ELEKI ),                                         STATUS_TYPE_TURN, BTR_STATE_05_01, BTR_STATE_05_02, groundTurnNum, groundTurnMaxNum },

        //  グラスフィールド
        { ( ground == BTL_GROUND_GRASS ),                                         STATUS_TYPE_TURN, BTR_STATE_06_01, BTR_STATE_06_02, groundTurnNum, groundTurnMaxNum },

        //  ミストフィールド
        { ( ground == BTL_GROUND_MIST ),                                          STATUS_TYPE_TURN, BTR_STATE_07_01, BTR_STATE_07_02, groundTurnNum, groundTurnMaxNum },

        //  ルーム

        //  トリックルーム
        { pFieldStatus->CheckEffect( btl::FieldStatus::EFF_TRICKROOM ),           STATUS_TYPE_TURN, BTR_STATE_08_01, BTR_STATE_08_02, pFieldStatus->CheckRemainingTurn( btl::FieldStatus::EFF_TRICKROOM ), pFieldStatus->GetWholeTurn( btl::FieldStatus::EFF_TRICKROOM ) },

        //  マジックルーム
        { pFieldStatus->CheckEffect( btl::FieldStatus::EFF_MAGICROOM ),           STATUS_TYPE_TURN, BTR_STATE_09_01, BTR_STATE_09_02, pFieldStatus->CheckRemainingTurn( btl::FieldStatus::EFF_MAGICROOM ), pFieldStatus->GetWholeTurn( btl::FieldStatus::EFF_MAGICROOM ) },

        //  ワンダールーム
        // @fix NMCat[1322] 説明テキストidがまちがっていた
        { pFieldStatus->CheckEffect( btl::FieldStatus::EFF_WONDERROOM ),          STATUS_TYPE_TURN, BTR_STATE_10_01, BTR_STATE_10_02, pFieldStatus->CheckRemainingTurn( btl::FieldStatus::EFF_WONDERROOM ), pFieldStatus->GetWholeTurn( btl::FieldStatus::EFF_WONDERROOM ) },

        //  ポケモン状態

        //  きあいだめ
        { pPokeParam->CONTFLAG_Get( btl::BTL_POKEPARAM::CONTFLG_KIAIDAME ),       STATUS_TYPE_NONE, BTR_STATE_11_01, BTR_STATE_11_02, 0, 0 },

        //  こんらん
        // @fix NMCat[1328] 仕様変更でターン数表記がなくなったのでターン数非表示に変更
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_KONRAN ),                STATUS_TYPE_NONE, BTR_STATE_12_01, BTR_STATE_12_02, 0, 0 },

        //  メロメロ
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_MEROMERO ),              STATUS_TYPE_NONE, BTR_STATE_13_01, BTR_STATE_13_02, 0, 0 },

        //  あくむ
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_AKUMU ),                 STATUS_TYPE_NONE, BTR_STATE_14_01, BTR_STATE_14_02, 0, 0 },

        //  あくび
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_AKUBI ),                 STATUS_TYPE_NONE, BTR_STATE_15_01, BTR_STATE_15_02, 0, 0 },

        //  アンコール
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_ENCORE ),                STATUS_TYPE_TURN, BTR_STATE_16_01, BTR_STATE_16_02, sickEncoreTurnNowNum, sickEncoreTurnMaxNum },

        //  いえき
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_IEKI ),                  STATUS_TYPE_NONE, BTR_STATE_17_01, BTR_STATE_17_02, 0, 0 },

        //  いちゃもん
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_ICHAMON ),               STATUS_TYPE_NONE, BTR_STATE_18_01, BTR_STATE_18_02, 0, 0 },

        //  おいかぜ
        { pSideEffectOikazeStatus->IsEffective(),                                 STATUS_TYPE_TURN, BTR_STATE_19_01, BTR_STATE_19_02, effectOikazaTurnNowNum, effectOikazeTurnMaxNum },

        //  おまじない
        // @fix NMCat[1356] おまじないにターン数を表示するように仕様変更
        { pSideEffectOmajinaiStatus->IsEffective(),                               STATUS_TYPE_TURN, BTR_STATE_20_01, BTR_STATE_20_02, effectOmajinaiTurnNowNum, effectOmajinaiTurnMaxNum },

        //  おんねん
        { pPokeParam->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_ONNEN ),               STATUS_TYPE_NONE, BTR_STATE_21_01, BTR_STATE_21_02, 0, 0 },

        //  回復封じ
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_KAIHUKUHUUJI ),          STATUS_TYPE_TURN, BTR_STATE_22_01, BTR_STATE_22_02, sickKaihukuHuujiTurnNowNum, sickKaihukuHuujiTurnMaxNum },

        //  かぎわける・みやぶる・ミラクルアイ
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_MIYABURU ),              STATUS_TYPE_NONE, BTR_STATE_23_01, BTR_STATE_23_02, 0, 0 },

        //  かなしばり
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_KANASIBARI ),            STATUS_TYPE_TURN, BTR_STATE_24_01, BTR_STATE_24_02, sickKanashibariTurnNowNum, sickKanashibariTurnMaxNum },

        //  クモのす/くろいまなざし
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_TOOSENBOU ),             STATUS_TYPE_NONE, BTR_STATE_25_01, BTR_STATE_25_02, 0, 0 },

        //  こころのめ
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_MUSTHIT_TARGET ),        STATUS_TYPE_NONE, BTR_STATE_26_01, BTR_STATE_26_02, 0, 0 },

        //  さしおさえ
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_SASIOSAE ),              STATUS_TYPE_TURN, BTR_STATE_27_01, BTR_STATE_27_02, sickSasiosaeTurnNowNum, sickSasiosaeTurnMaxNum },

        //  じゅうでん
        { pPokeParam->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_JUDEN ),               STATUS_TYPE_NONE, BTR_STATE_28_01, BTR_STATE_28_02, 0, 0 },

        //  じゅうりょく
        { pFieldStatus->CheckEffect( btl::FieldStatus::EFF_JURYOKU ),             STATUS_TYPE_TURN, BTR_STATE_29_01, BTR_STATE_29_02, pFieldStatus->CheckRemainingTurn( btl::FieldStatus::EFF_JURYOKU ), pFieldStatus->GetWholeTurn( btl::FieldStatus::EFF_JURYOKU ) },
        
        //  しろいきり
        { pSideEffectShiroikiriStatus->IsEffective(),                             STATUS_TYPE_TURN, BTR_STATE_30_01, BTR_STATE_30_02, effectShiroikiriTurnNowNum, effectShiroikiriTurnMaxNum },

        //  しんぴのまもり
        { pSideEffectSinpinomamoriStatus->IsEffective(),                          STATUS_TYPE_TURN, BTR_STATE_31_01, BTR_STATE_31_02, effectSinpinomamoriTurnNowNum, effectSinpinomamoriTurnMaxNum },

        //  ステルスロック
        { pSideEffectStealthrRookStatus->IsEffective(),                           STATUS_TYPE_NONE, BTR_STATE_32_01, BTR_STATE_32_02, 0, 0 },

        //  たくわえる
        { ( 0 < pPokeParam->COUNTER_Get( btl::BTL_POKEPARAM::COUNTER_TAKUWAERU ) ),       STATUS_TYPE_COUNT_SIGN, BTR_STATE_33_01, BTR_STATE_33_02, pPokeParam->COUNTER_Get( btl::BTL_POKEPARAM::COUNTER_TAKUWAERU ), 0 },

        //  ちょうはつ
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_TYOUHATSU ),             STATUS_TYPE_TURN, BTR_STATE_34_01, BTR_STATE_34_02, sickTyouhatsuTurnNowNum, sickTyouhatsuTurnMaxNum },

        //  テレキネシス
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_TELEKINESIS ),           STATUS_TYPE_TURN, BTR_STATE_35_01, BTR_STATE_35_02, sickTelekinesisTurnNowNum, sickTelekinesisTurnMaxNum },

        //  でんじふゆう
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_FLYING ),                STATUS_TYPE_TURN, BTR_STATE_36_01, BTR_STATE_36_02, sickFlyingTurnNowNum, sickFlyingTurnMaxNum },

        //  どくびし
        { pSideEffectDokubisiStatus->IsEffective(),                               STATUS_TYPE_COUNT_SIGN, BTR_STATE_37_01, BTR_STATE_37_02, pSideEffectDokubisiStatus->GetAddCount(), 0 },

        //  どろあそび
        { pFieldStatus->CheckEffect( btl::FieldStatus::EFF_DOROASOBI ),           STATUS_TYPE_TURN, BTR_STATE_38_01, BTR_STATE_38_02, pFieldStatus->CheckRemainingTurn( btl::FieldStatus::EFF_DOROASOBI ), pFieldStatus->GetWholeTurn( btl::FieldStatus::EFF_DOROASOBI ) },

        //  ねがいごと
        { pPosEffectNegaigotoStatus->IsEffective(),                               STATUS_TYPE_NONE, BTR_STATE_39_01, BTR_STATE_39_02, 0, 0 },

        //  ねばねばネット
        { pSideEffectNebanebaNetStatus->IsEffective(),                            STATUS_TYPE_NONE, BTR_STATE_40_01, BTR_STATE_40_02, 0, 0 },

        //  ねをはる
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_NEWOHARU ),              STATUS_TYPE_NONE, BTR_STATE_41_01, BTR_STATE_41_02, 0, 0 },

        //  のろい
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_NOROI ),                 STATUS_TYPE_NONE, BTR_STATE_42_01, BTR_STATE_42_02, 0, 0 },

        //  ハロウィン
        { exTypeCause == BTL_POKEPARAM::EXTYPE_CAUSE_HALLOWEEN,                   STATUS_TYPE_NONE, BTR_STATE_43_01, BTR_STATE_43_02, 0, 0 },

        //  ひかりのかべ
        { pSideEffectHIkarinokabeStatus->IsEffective(),                           STATUS_TYPE_TURN, BTR_STATE_44_01, BTR_STATE_44_02, effectHIkarinokabeTurnNowNum, effectHIkarinokabeTurnMaxNum },

        //  リフレクター
        { pSideEffectRefrectorStatus->IsEffective(),                              STATUS_TYPE_TURN, BTR_STATE_45_01, BTR_STATE_45_02, effectRefrectorTurnNowNum, effectRefrectorTurnMaxNum },

        //  ふういん
        { pFieldStatus->CheckEffect( btl::FieldStatus::EFF_FUIN ),                STATUS_TYPE_NONE, BTR_STATE_46_01, BTR_STATE_46_02, 0, 0 },

        //  ほろびのうた
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_HOROBINOUTA ),           STATUS_TYPE_COUNT, BTR_STATE_47_01, BTR_STATE_47_02, horobinoutaTurnMax - pPokeParam->GetSickTurnCount( pml::wazadata::WAZASICK_HOROBINOUTA ), 0 },

        //  まきびし
        { pSideEffectMakibisiStatus->IsEffective(),                               STATUS_TYPE_COUNT_SIGN, BTR_STATE_48_01, BTR_STATE_48_02, pSideEffectMakibisiStatus->GetAddCount(), 0 },

        //  みずあそび
        { pFieldStatus->CheckEffect( btl::FieldStatus::EFF_MIZUASOBI ),           STATUS_TYPE_TURN, BTR_STATE_49_01, BTR_STATE_49_02, pFieldStatus->CheckRemainingTurn( btl::FieldStatus::EFF_MIZUASOBI ), pFieldStatus->GetWholeTurn( btl::FieldStatus::EFF_MIZUASOBI ) },

        //  みちづれ
        { pPokeParam->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_MITIDURE ),            STATUS_TYPE_NONE, BTR_STATE_50_01, BTR_STATE_50_02, 0, 0 },

        //  もりののろい
        { exTypeCause == BTL_POKEPARAM::EXTYPE_CAUSE_MORINONOROI,                 STATUS_TYPE_NONE, BTR_STATE_51_01, BTR_STATE_51_02, 0, 0 },

        //  やどりぎのタネ
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_YADORIGI ),              STATUS_TYPE_NONE, BTR_STATE_52_01, BTR_STATE_52_02, 0, 0 },

        /*  // @fix NMCat[1339] 「こころのめ」と判定方法がかぶっていたので被っている「ロックオン」を削りました。
        //  ロックオン
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_MUSTHIT_TARGET ),        STATUS_TYPE_NONE, BTR_STATE_53_01, BTR_STATE_53_02, 0, 0 },
        */

        //  しめつける /まきつく /ほのおのうず / からではさむ / うずしお / マグマストーム / まとわりくつく
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_BIND ),                  STATUS_TYPE_NONE, BTR_STATE_54_01, BTR_STATE_54_02, 0, 0 },

        //  がまん
        { pPokeParam->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_GAMAN ),               STATUS_TYPE_NONE, BTR_STATE_55_01, BTR_STATE_55_02, 0, 0 },

        //  げきりん/あばれる/はなびらのまい
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_ABARERU ),               STATUS_TYPE_NONE, BTR_STATE_57_01, BTR_STATE_57_02, 0, 0 },

        // @fix NMCat[1337] 仕様変更によりもうどくはターン数表示ではなくなった
        //  どくどく
        { pPokeParam->CheckMoudoku(),                                             STATUS_TYPE_NONE, BTR_STATE_59_01, BTR_STATE_59_02, 0, 0 },

        //  みらいよち / はめつのねがい
        { pPosEffectDelayAttackStatus->IsEffective(),                             STATUS_TYPE_TURN, BTR_STATE_60_01, BTR_STATE_60_02, delayAttackNowTurnNum, delayAttackMaxTurnNum },

        //  さわぐ
        // @fix NMCat[986] かいふくふうじの判定をしていたので、さわぐの判定に修正
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_SAWAGU ),               STATUS_TYPE_TURN, BTR_STATE_61_01, BTR_STATE_61_02, sickSawaguTurnNowNum, sickSawaguTurnMaxNum },

        //  アクアリング
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_AQUARING ),              STATUS_TYPE_NONE, BTR_STATE_62_01, BTR_STATE_62_02, 0, 0 },

        //  ボディパージ
        { pPokeParam->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_BODYPURGE ),           STATUS_TYPE_NONE, BTR_STATE_63_01, BTR_STATE_63_02, 0, 0 },

        //  うちおとす
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_FLYING_CANCEL ),         STATUS_TYPE_NONE, BTR_STATE_64_01, BTR_STATE_64_02, 0, 0 },

        //  フェアリーロック
        { pFieldStatus->CheckEffect( btl::FieldStatus::EFF_FAIRY_LOCK ),          STATUS_TYPE_NONE, BTR_STATE_65_01, BTR_STATE_65_02, 0, 0 },

        //  虹
        { pSideEffectRaibowStatus->IsEffective(),                                 STATUS_TYPE_TURN, BTR_STATE_66_01, BTR_STATE_66_02, effectRaibowTurnNowNum, effectRaibowTurnMaxNum },

        //  湿原
        { pSideEffectMoorStatus->IsEffective(),                                   STATUS_TYPE_TURN, BTR_STATE_67_01, BTR_STATE_67_02, effectMoorTurnNowNum, effectMoorTurnMaxNum },

        //  火の海
        { pSideEffectBurningStatus->IsEffective(),                                STATUS_TYPE_TURN, BTR_STATE_68_01, BTR_STATE_68_02, effectBurningTurnNowNum, effectBurningTurnMaxNum },

        //  おおひでり
        { ( weather == BTL_WEATHER_DAY ),                                         STATUS_TYPE_TURN, BTR_STATE_69_01, BTR_STATE_69_02, weatherTurnNum, weatherTuneMaxNum },

        //  おおあめ
        { ( weather == BTL_WEATHER_STORM ),                                       STATUS_TYPE_TURN, BTR_STATE_70_01, BTR_STATE_70_02, weatherTurnNum, weatherTuneMaxNum },

        //  らんきりゅう
        { ( weather == BTL_WEATHER_TURBULENCE ),                                  STATUS_TYPE_TURN, BTR_STATE_71_01, BTR_STATE_71_02, weatherTurnNum, weatherTuneMaxNum },

        //  サイコフィールド
        { ( ground == BTL_GROUND_PHYCHO ),                                        STATUS_TYPE_TURN, BTR_STATE_72_01, BTR_STATE_72_02, groundTurnNum, groundTurnMaxNum },

        //  じこくつき
        // @fix NMCat[1266] ターン数表示に変更
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_ZIGOKUDUKI  ),           STATUS_TYPE_TURN, BTR_STATE_73_01, BTR_STATE_73_02, sickZigokudukiTurnNowNum, sickZigokudukiTurnMaxNum },

        //  オーロラベール
        { pSideEffectAuraVeilStatus->IsEffective(),                               STATUS_TYPE_TURN, BTR_STATE_74_01, BTR_STATE_74_02, effectAuraVeilTurnNowNum, effectAuraVeilTurnMaxNum },

        //  とぎすます
        // @fix NMCat[2913] 仕様追加対応
        { pPokeParam->CheckSick( pml::wazadata::WAZASICK_TOGISUMASU  ),           STATUS_TYPE_NONE, BTR_STATE_75_01, BTR_STATE_75_02, 0, 0 },
      };

      //  リストに残っていてもう効果がないのを消す
      {
        u32 i = 0;
        while( i < pPokeStatusData->statusNum )
        {
          if( aStatusDataTable[ pPokeStatusData->aStatus[ i ].id ].bHit == false )
          {
            //  状態になっていないでかつ持っている場合は削除
            if( 1 < pPokeStatusData->statusNum )
            {
              s32 moveDataNum = (s32)pPokeStatusData->statusNum -  ( i + 1 );

              //  配列の末尾以外であれば、memcpyを使って配列データ全体をずらして対処
              if( 0 < moveDataNum )
              {
                gfl2::std::MemCopy( &pPokeStatusData->aStatus[ i + 1 ], &pPokeStatusData->aStatus[ i ], sizeof( pPokeStatusData->aStatus[ 0 ] ) * moveDataNum );
              }
              else
              {
                //  配列末尾のデータ削除の場合は、リスト最後のデータを参照しないように
                //  リスト数を-1しているのでなにもしなくでも平気
              }

              --pPokeStatusData->statusNum;
            }
            else
            {
              pPokeStatusData->statusNum  = 0;
            }          
          }
          else
          {
            //  まだ効果が残っているので次をチェック
            ++i;
          }
        }
      }

      STATUS_DATA*      pStatusData     = NULL;

      s32 haveStatusListIdx = -1;
      const struct _tag_poke_status_data_table* pData = NULL;
      s32 loopNum = GFL_NELEMS( aStatusDataTable );
      for( s32 i = 0; i < loopNum; ++i )
      {
        pData = &aStatusDataTable[ i ];
        if( pData->bHit )
        {
          haveStatusListIdx = -1;

          //  すでに状態になっている場合は所持リストのidxを取得
          for( u32 j = 0; j < pPokeStatusData->statusNum; ++j )
          {
            if( pPokeStatusData->aStatus[ j ].id == i )
            {
              haveStatusListIdx = j;
              break;
            }
          }


          if( haveStatusListIdx < 0 )
          {
            //  持ってないので追加
            pStatusData = &pPokeStatusData->aStatus[ pPokeStatusData->statusNum ];
            ++pPokeStatusData->statusNum;

            pStatusData->id              = (s32)i;
            pStatusData->titleNameId     = pData->title_msg_id;
            pStatusData->infoNameId      = pData->info_msg_id;
            pStatusData->type            = pData->type;
          }
          else
          {
            //  もっている場合
            pStatusData = &pPokeStatusData->aStatus[ haveStatusListIdx ];
          }

          if( ( pStatusData->type == STATUS_TYPE_COUNT ) || ( pStatusData->type == STATUS_TYPE_COUNT_SIGN ) )
          {
            pStatusData->data.count.num   = pData->num;
          }
          else if( pStatusData->type == STATUS_TYPE_TURN )
          {
            pStatusData->data.turn.now   = pData->num;
            pStatusData->data.turn.max   = pData->turnMax;
          }
        }
      }
    }

    return  pPokeStatusData;
  }

  //------------------------------------------------------------------
  /**
    * @brief  ポケモンのカーソル記憶取得
    */
  //------------------------------------------------------------------
  PokeCursorMemory& BattleUIDataManager::GetPokeCursorMemory( const u32 pokeIdx )
  {
    u32 arrayNum  = GFL_NELEMS( m_aPokeCursorMemory );
    if( pokeIdx < arrayNum )
    {
      return  m_aPokeCursorMemory[ pokeIdx ];
    }

    GFL_ASSERT( 0 );
    return  m_aPokeCursorMemory[ 0 ];
  }

  //------------------------------------------------------------------
  /**
    * @brief  ポケモンのカーソルリセット
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::ResetPokeCursorMemory( const u32 pokeIdx )
  {
    PokeCursorMemory& rPokeCursorMemory = GetPokeCursorMemory( pokeIdx );
    rPokeCursorMemory.Reset();

    //  トップ画面のカーソルも一緒に
    m_topActCursorType  = BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_FIGHT;
  }

  //------------------------------------------------------------------
  /**
    * @brief  バッグを使用できるか
    */
  //------------------------------------------------------------------
  bool BattleUIDataManager::IsUseBag()
  {
    // @fix GFNMCat[1370]バトルロイヤルでどうぐが使える。
    BtlRule rule = GetBattleRule();
    if( rule == BTL_RULE_ROYAL )
    {
      //  バトルロイヤルでは使えない
      return false;
    }

    BtlCompetitor competitor  = GetBattleCompetitor();
    if( competitor == BTL_COMPETITOR_COMM )
    {
      //  通信対戦では使えない
      return  false;
    }
    else
    if( competitor == BTL_COMPETITOR_INST )
    {
      //  バトル施設では使えない
      return  false;
    }

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief  ショートカットボタンで使用したボールアイテムidを設定
    */
  //------------------------------------------------------------------
  void BattleUIDataManager::SetLastShortcutUseBallItemId( const u16 itemId )
  {
    btl::MainModule*  pBtlMainModule  = m_btlv->GetMainModule();
    if( pBtlMainModule == NULL )
    {
      return;
    }

    GameSys::GameData*  pGameData = pBtlMainModule->GetGameData();
    if( pGameData == NULL )
    {
      return;
    }

    // @fix ディレクターさん要望　バトルのショートカットボタンで最後に使ったボールアイテムidをアプリ終了まで保持しているデータを使う
    //                            再度バトルを入った時に前のバトルで最後に使ったのを参照するため
    pGameData->SetBattleDataShortcutUseBallItemId( itemId );
  }

  //------------------------------------------------------------------
  /**
    * @brief  ショートカットボタンで使用したボールアイテムidを取得
    */
  //------------------------------------------------------------------
  u16 BattleUIDataManager::GetLastShortcutUseBallItemId() const
  {
    btl::MainModule*  pBtlMainModule  = m_btlv->GetMainModule();
    if( pBtlMainModule == NULL )
    {
      return  GameSys::GameData::LAST_SHORT_CUT_USE_BALL_ITEM_ID_NONE;
    }

    GameSys::GameData*  pGameData = pBtlMainModule->GetGameData();
    if( pGameData == NULL )
    {
      return  GameSys::GameData::LAST_SHORT_CUT_USE_BALL_ITEM_ID_NONE;
    }

    // @fix ディレクターさん要望　バトルのショートカットボタンで最後に使ったボールアイテムidをアプリ終了まで保持しているデータを使う
    //                            再度バトルを入った時に前のバトルで最後に使ったのを参照するため
    return  pGameData->GetBattleDataShortcutUseBallItemId();
  }

  //  アイテムデータからバトルポケットアイテムデータを更新
  void BattleUIDataManager::_UpdateBtlPocketItemData( ::app::bag::POCKET_DATA* pOutputPocketDataList, const item::BtlPocket btlPocketType, const u32 bagPocketType, Savedata::MyItem* pChkItemData )
  {
    GFL_ASSERT( pOutputPocketDataList );
    GFL_ASSERT( pChkItemData );
    GFL_ASSERT( bagPocketType < FIELD_BAG_POCKET_MAX );

    u32 itemCountNum  = pChkItemData->GetItemCount( bagPocketType );

    const Savedata::MyItem::ITEM_ST*  pItemData = NULL;

//  検索範囲を半分に割って、検索しているが、この方法だとリストのid値がばらばらになって昇順にならない
//  昇順にするならこの後に昇順するようにソートする必要があるが、それでは結局処理負荷が増えるので線形処理と処理負荷が変わらないかも知れない
#if 0 
    //  左右探索する(アイテムの個数が多くなるとループ回数が多くなるので、左右探索をしてループ回数を少なくする)
    s32 searchCenterCountNum  = itemCountNum >> 1;
    s32 searchLeftChkIdx      = searchCenterCountNum - 1;
    s32 searchRightChkIdx     = searchCenterCountNum;

    //  バッグのポケットをひとつずつチェックして、バトルポケットに該当するのがあれば設定
    while( ( 0 <= searchLeftChkIdx ) || ( searchRightChkIdx < itemCountNum ) )
    {
      //  左探索
      if( 0 <= searchLeftChkIdx )
      {
        pItemData = pChkItemData->GetItem( bagPocketType, searchLeftChkIdx );
        if( item::CheckBtlPocketInclude( btlPocketType, pItemData->id ) )
        {
          //  見つけたので終了
          pOutputPocketDataList->table[ pOutputPocketDataList->max ]  = *pItemData;
          ++pOutputPocketDataList->max;
        }

        --searchLeftChkIdx;
      }

      //  右探索
      if( searchRightChkIdx < itemCountNum )
      {
        pItemData = pChkItemData->GetItem( bagPocketType, searchRightChkIdx );
        if( item::CheckBtlPocketInclude( btlPocketType, pItemData->id ) )
        {
          //  見つけたので終了
          pOutputPocketDataList->table[ pOutputPocketDataList->max ]  = *pItemData;
          ++pOutputPocketDataList->max;
        }

        ++searchRightChkIdx;
      }
    }
#endif
    //  線形によるリスト生成
    for( u32 i = 0; i < itemCountNum; ++i )
    {
      pItemData = pChkItemData->GetItem( bagPocketType, i );
      if( item::CheckBtlPocketInclude( btlPocketType, pItemData->id ) )
      {
        if( pOutputPocketDataList->max < POCKET_DATA_MAX )
        {
          //  見つけたので終了
          pOutputPocketDataList->table[ pOutputPocketDataList->max ]  = *pItemData;
          ++pOutputPocketDataList->max;
        }
        else
        {
          GFL_ASSERT( 0 );
          return;
        }
      }
    }
  }

  void BattleUIDataManager::_Clear()
  {
    m_pAppMsgData           = NULL;
    m_pWordSet              = NULL;
    m_pWzaaInfoMsgData      = NULL;
    m_pMenuCursor           = NULL;
    m_pMenuCursorLytBuffer  = NULL;
    m_pItemManager          = NULL;
    m_pSelfPosPlaceList     = NULL;
    m_pEnemyPosPlaceList    = NULL;
    m_pItemMessage          = NULL;
    m_pStatusMsgData        = NULL;
    m_pInfoWindowView       = NULL;
    m_pTokuseInfoMsgData    = NULL;
    m_pItemInfoMsgData      = NULL;
    m_pRecPlayLowerView     = NULL;
    m_pWordSetLoader        = NULL;

    m_readyStep = 0;
    m_loadMsgCnt  = 0;
    m_topActCursorType  = BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_FIGHT;

    gfl2::std::MemClear( m_pCacthFileCtrlStateList, sizeof( m_pCacthFileCtrlStateList ) );
    gfl2::std::MemClear( m_pCatchFileOnepCloseList, sizeof( m_pCatchFileOnepCloseList ) );
    
    ClearAllUniquPushBtnFlg();
  }

  void PokeCursorMemory::_Clear()
  {
    m_wazaSelectCursorPos   = app::BattleUISkillSelectLowerDraw::CURSOR_POS_TYPE_SKILL_00;

    for(u32 index = 0; index < GFL_NELEMS(m_TargetSelectCursorPosInfo); ++index)
    {
      m_TargetSelectCursorPosInfo[index].Clear();
    }
    m_TargetSelectCursorPosInfoCount  = 0;
  }

  /**
    @fix  MMcat[240]   立ち位置Cのポケモンが「味方場」「味方全ポケ」の効果範囲の技を使用した後の、カーソル位置がおかしい
          立ち位置Cのポケモンで「味方場」の技（リフレクターなど）によりカーソル位置を記憶（立ち位置A）した後、「通常ポケ」が対象の技（でんじはなど）を選択すると、
          立ち位置Aが選択可能であった場合にはそれが採用されてしまう（でんじはを味方Aに撃とうとする）
          目的の違う別の技の情報を使用して初期カーソル位置を決定していることに問題があるため、カーソル位置の記憶に技Noを紐付けて管理するようにした
    @fix  MMcat[240]
          更にカーソル位置記憶数を8にし、技ごとにカーソル位置を記憶できるようにした
  */
  s32 PokeCursorMemory::GetTargetCursorPos(const WazaNo wazaNo)  const
  {
    s32 infoIndex = FindTargetCursorPos(wazaNo);
    
    if(0 <= infoIndex)
    {
      return m_TargetSelectCursorPosInfo[infoIndex].cursorPos;
    }
    
    return app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE;
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  void PokeCursorMemory::SetTargetCursorPos( const s32 cursorPos, const WazaNo wazaNo )
  {
    s32 infoIndex = FindTargetCursorPos(wazaNo);

    if(0 <= infoIndex)
    {
      /*  overwrite  */
      m_TargetSelectCursorPosInfo[infoIndex].SetCursorPos(cursorPos, wazaNo);
    }
    else
    {
      /*  add  */
      m_TargetSelectCursorPosInfo[m_TargetSelectCursorPosInfoCount].SetCursorPos(cursorPos, wazaNo);

      ++m_TargetSelectCursorPosInfoCount;
      m_TargetSelectCursorPosInfoCount  %= TargetSelectCursorPosInfoNum;
    }
  }

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  PokeCursorMemory::TargetSelectCursorPosInfo::TargetSelectCursorPosInfo(void)
    : cursorPos(app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE)
    , wazaNo(WAZANO_NULL)
  {
  }


  //------------------------------------------------------------------
  /**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
  //------------------------------------------------------------------
  s32   PokeCursorMemory::FindTargetCursorPos(const WazaNo wazaNo) const
  {
    for(u32 index = 0; index < GFL_NELEMS(m_TargetSelectCursorPosInfo); ++index)
    {
      if(m_TargetSelectCursorPosInfo[index].wazaNo == wazaNo)
      {
        return index;
      }
    }
    
    return -1;  /*  not found  */
  }



GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(btl)
