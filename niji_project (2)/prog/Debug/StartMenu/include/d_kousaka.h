#if PM_DEBUG
//=============================================================================
/**
 * @file    d_kousaka.h
 * @brief   幸坂用ランチャーリスト処理
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================
#if !defined( D_KOUSAKA_H_INCLUDED )
#define D_KOUSAKA_H_INCLUDED
#pragma once

#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>


#include <gflnet2/include/base/gflnet2_InitParam.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include <gflnet2/include/p2p/gflnet2_NetPlayer.h>
#include <gflnet2/include/beacon/gflnet2_BeaconManager.h>
#include <gflnet2/include/p2p/gflnet2_P2pManager.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <util/include/gfl2_std_string.h>

#include "System/include/HeapDefine.h"

#include  <NetStatic/NetLib/include/Delivery/DeliveryRecvListener.h>
#include "ExtSavedata/include/BattleRecorderSaveData.h"
#include  <System/include/ThreadPriority.h>
#include "Battle/include/battle_SetupParam.h"


#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_kousaka.h>
#include  "GameSys/include/GameEventManager.h"
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"
#include <Savedata/include/BoxPokemonSave.h>
#include <Savedata/include/ConfigSave.h>
#include  "GameSys/include/DllProc.h"
#include <GameSys/include/GameProcManager.h>
#include <thread/include/gfl2_ThreadStatic.h>
#include <base/include/gfl2_Singleton.h>
#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/wifi/gflnet2_WifiSystem.h>
#include <heap/include/gfl2_Heap.h>
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include <NetStatic/NetEvent/include/QuickMatchEvent.h>

// ui
#include <ui/include/gfl2_UI.h>
#include <system/include/HeapDefine.h>
#include "System/include/HeapSize.h"

#include "Test/KousakaTest/LocalQuickMatchTest.h"
#include "Test/KousakaTest/NijiSaveLoadTest.h"

#include  <NetStatic/NetLib/include/Delivery/DeliveryManager.h>
#include <NetStatic/NetLib/include/P2P/P2pConnectionManager.h>
#include <NetStatic/NetLib/include/NetDefine.h>
#include  "Battle/include/battle_proc.h"
#ifdef  GF_PLATFORM_CTR
#include <imgdb/ImageIO.h>
#include <zorro.h>
#endif
#include <heap/include/gfl2_NewAndDeleteOperators.h> 

#include <pml/include/personal/pml_PersonalSystem.h> 

// VSデモイベント
#include "NetStatic/NetEvent/include/VsDemoEvent.h"

#include "Sound/include/Sound.h"


// draw
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>
#include <System/include/nijiAllocator.h>

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <debug/include/gfl2_DebugWinSystem.h>
// niji
#include "Debug/DebugWin/include/DebugWinSystem.h"

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

class LiveRecDrawProc : public GameSys::GameProc
{
  LiveRecDrawProc(){}
  ~LiveRecDrawProc(){}
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager)
  {
    //HEAPID_APP  本体に関するものはAPPメモリから確保します
    gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

    // font
    {
      m_pMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
      m_pDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
      m_pNijiAllocator = GFL_NEW(pDevHeap) System::nijiAllocator( pDevHeap );
      m_pRenderingPipeLine = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine( m_pNijiAllocator , 512, 256 );
      m_pDrawUtilText = GFL_NEW(pManager->GetHeap()) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator); 
      m_pDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
    }

    m_mode = 0;
    m_count = 0;
    m_isEnd = false;

    return gfl2::proc::RES_FINISH; 
  };
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager)
  {
    // 文字描画
    this->DrawText();

    if( m_isEnd )
    {
      return gfl2::proc::RES_FINISH; 
    }
    else
    {
      return gfl2::proc::RES_CONTINUE; 
    }
  };
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
  {
    switch( displayNo )
    {
    case gfl2::gfx::CtrDisplayNo::LEFT:
    case gfl2::gfx::CtrDisplayNo::RIGHT:
      {
        gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
        gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
        u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
        u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

        m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
        m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
        m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

        m_pRenderingPipeLine->Execute();
      }
      break;
    }
  }
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager)
  { 
    m_pDrawUtilText->Terminate();
    GFL_SAFE_DELETE( m_pDrawUtilText );
    GFL_SAFE_DELETE( m_pRenderingPipeLine );
    GFL_SAFE_DELETE( m_pNijiAllocator );
    GFL_SAFE_DELETE( m_pDevAllocator );
    GFL_SAFE_DELETE( m_pMemAllocator );
    return gfl2::proc::RES_FINISH; 
  };
  void DrawText( void )
  {
    gfl2::math::Vector4 color( 1.0f, 1.0f, 1.0f, 1.0f );
    m_pDrawUtilText->ClearDrawText();
    m_pDrawUtilText->SetTextScale( 1.0f, 1.0f );


    switch( m_mode )
    {
    case 0:
      {
        u32 count = m_count % 120;
        if( count < 30 )
        {
          m_pDrawUtilText->DrawText( 0, 100, L"Waiting for receive", color, print::SystemFont_GetFont() );
        }
        else if( count < 60 )
        {
          m_pDrawUtilText->DrawText( 0, 100, L"Waiting for receive.", color, print::SystemFont_GetFont() );
        }
        else if( count < 90 )
        {
          m_pDrawUtilText->DrawText( 0, 100, L"Waiting for receive..", color, print::SystemFont_GetFont() );
        }
        else
        {
          m_pDrawUtilText->DrawText( 0, 100, L"Waiting for receive...", color, print::SystemFont_GetFont() );
        }

        m_count++;
      }
      break;
    case 1:
      color.Set( 1.0f , 0.0f , 0.0f , 1.0f );
      m_pDrawUtilText->DrawText( 0, 100, L"Receive has been completed!", color, print::SystemFont_GetFont() );
      break;
    };

  }

  public:
  void SetMode( u32 mode ){ m_mode = mode; }
  void End( void ){ m_isEnd = true; }

  private:
  gfl2::heap::NwAllocator*		m_pMemAllocator;
  gfl2::heap::NwAllocator*		m_pDevAllocator;
  System::nijiAllocator*      m_pNijiAllocator;
  gfl2::util::UtilTextRenderingPipeLine*		m_pRenderingPipeLine;
  gfl2::util::DrawUtilText* m_pDrawUtilText;
  u32 m_mode;
  u32 m_count;
  bool m_isEnd;
};

class LiveRecEvent : public GameSys::GameEvent
#ifdef  GF_PLATFORM_CTR
  , public NetLib::Delivery::DeliveryRecvListener
#endif
{
  GFL_FORBID_COPY_AND_ASSIGN( LiveRecEvent );
private:


public:

  LiveRecEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent( pHeap )
  {
    m_pRecorderSaveData = NULL;
    m_pBattleSetupParam = NULL;
    m_isFirstRecvComplete = false;
    m_eventSeq = 0;

    m_nickNameEnable = 0;

    Sound::StartBGMReq(STRM_BGM_RANDOM_MATCH);

    gfl2::debug::DebugWinGroup* pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, L"LiveRecPlayer" );  
    gfl2::debug::DebugWin_AddItemValueU8(pGroup, pHeap, L"NickNameEnable", &m_nickNameEnable, 0, 1 , 1 );
  };
  virtual~LiveRecEvent()
  {
#if defined(GF_PLATFORM_CTR)
    NetLib::Delivery::DeliveryManager* pDeliv = GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager);
    GFL_SAFE_DELETE( pDeliv );
#endif
    GFL_SAFE_DELETE( m_pRecorderSaveData );
    if( m_pBattleSetupParam )
    {
      m_pRecorderSaveData->ClearBattleSetupParam( m_pBattleSetupParam );
      GFL_SAFE_DELETE( m_pBattleSetupParam );
    }

    NetLib::NijiNetworkSystem::DeleteLiveRecSender();

    gfl2::debug::DebugWin_RemoveGroupByName( L"LiveRecPlayer" );
  };

  virtual bool BootChk( GameSys::GameManager* pGameManager ){ return true; };
  virtual void InitFunc( GameSys::GameManager* pGameManager ){};
  virtual void EndFunc( GameSys::GameManager* pGameManager ){};
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager )
  {
#ifdef  GF_PLATFORM_CTR
    if( GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager) )
    {
      GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->Update();
    }
#endif

    switch( m_eventSeq )
    {
    case 0:
      {
#ifdef  GF_PLATFORM_CTR

        m_pProc = NIJI_PROC_CALL< GameSys::GameProcManager, LiveRecDrawProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );

        NetLib::NijiNetworkSystem::CreateLiveRecSender();

        NetLib::Delivery::DeliveryManager* pDeliv = GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager);
        GFL_SAFE_DELETE( pDeliv );
        GFL_SAFE_DELETE( m_pRecorderSaveData );
        if( m_pBattleSetupParam )
        {
          m_pRecorderSaveData->ClearBattleSetupParam( m_pBattleSetupParam );
          GFL_SAFE_DELETE( m_pBattleSetupParam );
        }

        gfl2::heap::HeapBase * pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
        pDeliv  = GFL_NEW( pDebugHeap )  NetLib::Delivery::DeliveryManager();

        /*  観戦は署名なし  */
        pDeliv->SetUDSSignatureMode(false, false);

        const gflnet2::InitParameter
          initParameter = {
            nn::cfg::GetTransferableId(GameSys::GameManager::APLICATION_UNIQUE_ID),   //  自分のTransferdID 
            gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT ),
            gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ),
            (gflnet2::EN_NET_TYPE)NIJI_BEACON_TYPE_ID_DELIVERY,    //ビーコンIDむりやり設定
            NetLib::P2P::NIJI_P2P_GIFT,   /*  ENUM_NIJI_P2P_ID,    // 通信別のID  */
            2,                          /*  最大接続人数                */
            false                       /*  //マイグレーション有効無効  */
        };

        const gflnet2::InitParameterFixed
          initParameterFixed  = {
            NIJI_PRODUCT_CODE,      ///< 任天堂からソフトごとに割り当ててくれるID 
            System::GetLang(),      ///< 言語番号 
            System::GetVersion(),   ///< ＲＯＭバージョン番号 
            NIJI_NET_VERSION        ///< 通信バージョン              
        };

        m_pRecorderSaveData = GFL_NEW( pDebugHeap )  ExtSavedata::BattleRecorderSaveData( pDebugHeap, GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
        m_pBattleSetupParam = GFL_NEW( pDebugHeap ) BATTLE_SETUP_PARAM();

        GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->Initialize(&initParameter, &initParameterFixed, NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_wireless);

        GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->SetRecvListener( this );

        GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->StartRecv( System::ThreadPriority::DELIVERY_THREAD_PRIORITY );

        m_isFirstRecvComplete = false;




        m_eventSeq++;
#endif
        
      }
    break;
    case 1:
      {
        if( m_isFirstRecvComplete == false )
        {
          break;
        }


        

        // 戦闘前デモアプリ呼び出しパラメータの初期化

        App::AppDemo::AppDemoProcParam_VS procParam( (Regulation::BATTLE_RULE)m_pRecorderSaveData->GetBattleRecData()->upload.header.rule );
        {
          procParam.demoType = 0; // VSBATTLE_PRESTART

          for( int btlClientId=0; btlClientId<2; btlClientId++ )
          {
            // 立ち位置とデモのインデックス違う・・・
            const int demoIndex = btlClientId;

            // MyStatus
            procParam.pMyStatus[ demoIndex ] = m_pBattleSetupParam->playerStatus[demoIndex];

            // ポケモンデータ
            // ポケモン状態(0:無効値 1:通常 2:状態異常 3:瀕死）
            {
              pml::PokeParty* pParty = m_pBattleSetupParam->party[btlClientId];
              for( u32 pokeIndex=0; pokeIndex<pParty->GetMemberCount(); pokeIndex++ )
              {
                procParam.pokemonStatus[demoIndex][pokeIndex] = 1;
              }
            }
          }

          // 背景タイプ (0:通常背景 1:WCS予選 2:WCS決勝）
          procParam.bgType = 0;
          if( m_pRecorderSaveData->GetBattleRecData()->upload.header.background == Regulation::BACKGROUND_WCS1 )
          {
            procParam.bgType = 1;
          }
          else if( m_pRecorderSaveData->GetBattleRecData()->upload.header.background == Regulation::BACKGROUND_WCS2 )
          {
            procParam.bgType = 2;
          }

          if( m_pRecorderSaveData->GetBattleRecData()->upload.header.background == Regulation::BACKGROUND_WCS2 )
          {
            Sound::StartBattleBGMReq( STRM_BGM_VS_SECRET_01 );
          }
          else
          {
            Sound::StartBattleBGMReq( STRM_BGM_VS_TRAINER_NJ );
          }

          GFL_PRINT( "bgType[%d]\n" , procParam.bgType );
        }
        NetEvent::VsDemoEvent::StartEvent( pGameManager, procParam );

        m_eventSeq++;
      }
    break;
    case 2:
      {
        btl::BATTLE_PROC_PARAM  param;

        param.pAppDeviceMemory  = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
        param.pAppMemory        = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
        param.pGameManager      = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
        param.pSetupParam       = m_pBattleSetupParam;

        param.pSetupParam->LimitTimeGame = 0;
        
        //[fix]NMCat[4204]:[#4204 【観戦機能】対戦側の設定が反映されバトルビデオが再生されている]
        //アニメーションあり、メッセージ早いで固定にする
        param.pSetupParam->configData->SetTalkingSpeed( Savedata::ConfigSave::MSG_FAST );
        param.pSetupParam->configData->SetBattleAnimation( Savedata::ConfigSave::BATTLE_ANIMATION_VISIBLE );

        //@fix [#4206 【観戦機能】観戦ROMのポケモン名表示が、観戦ROMの言語に変換されない]
        //ニックネーム不許可だったら全てデフォルトにする
        if( m_nickNameEnable == 0 )
        {
          u32 PLAYRE_NUM = 2;
          for( u32 index = 0 ; index < PLAYRE_NUM ; ++index )
          {
            u32 memberNum = param.pSetupParam->party[index]->GetMemberCount();
            for( u32 memberIndex = 0 ; memberIndex < memberNum ; ++memberIndex )
            {
              param.pSetupParam->party[index]->GetMemberPointer( memberIndex )->SetDefaultNickName();
            }
          }
        }

        GameSys::CallBattleProc( &param );
        m_eventSeq++;
      }
    break;
    case 3:
      if( pGameManager->IsProcessExists() == false )
      {
        Sound::EndBattleBGMReq();
        return GameSys::GMEVENT_RES_FINISH;
      }
    break;
    }

    return GameSys::GMEVENT_RES_CONTINUE;
  };



  #ifdef  GF_PLATFORM_CTR

  //------------------------------------------------------------------
  /**
  * @brief 受信完了イベント
  *
  * @param pData 受信データ
  * @return 受信を続けるかどうか( true : 続ける , false : 続けない)
  */
  //------------------------------------------------------------------
  virtual bool OnRecvComplete( const void* pData , size_t dataSize );

  //------------------------------------------------------------------
  /**
  * @brief 受信経過更新イベント
  *
  * @param progress 受信経過（一番進んでいるものを%で表したもの）
  */
  //------------------------------------------------------------------
  virtual void OnUpdateProgress( const u32 progress ){};

  //------------------------------------------------------------------
  /**
  * @brief ニンテンドーゾーン接続通知
  */
  //------------------------------------------------------------------
  virtual void OnNintendoZoneConnected(){};

  //------------------------------------------------------------------
  /**
  * @brief ネットワークエラーイベント
  *
  * @param result 結果
  */
  //------------------------------------------------------------------
  virtual void OnNetworkError( const nn::Result &result , u32 errorCode ){};

  //------------------------------------------------------------------
  /**
  * @brief ネットワークエラーコード
  *
  * @param result 結果
  */
  //------------------------------------------------------------------
  virtual void OnWifiError( const u32 errorCode ){};

#endif

private:

  u32 m_eventSeq;


  ExtSavedata::BattleRecorderSaveData* m_pRecorderSaveData;
  BATTLE_SETUP_PARAM* m_pBattleSetupParam;
  bool m_isFirstRecvComplete;

  LiveRecDrawProc* m_pProc;

  u8 m_nickNameEnable;
};


//-----------------------------------------------------------------------------
/**
 * @class   LauncherKousaka
 * @brief   幸坂用ランチャークラス
 * @date    
 */
//-----------------------------------------------------------------------------
class LauncherKousaka
  : public launcher::ListData 
{
private:
  static const u32 LIST_MAX = 5;        //!< 項目数
  static const u32 ItemList[LIST_MAX];  //!< 項目文字列データテーブル

public:

  enum
  {
    SAVE_TYPE_JP = 0 , 
    SAVE_TYPE_EN,
  };

  struct CONVERT_POKE
  {
    u8 monsNo;
    u8 nowHp[2];
    u8 level;
    u8 condition;
    u8 type1;
    u8 type2;
    u8 itemNo_captureValue;
    u8 waza1;
    u8 waza2;
    u8 waza3;
    u8 waza4;
    u8 idNumber[2];
    u8 exp[3];
    u8 hpEffort[2];
    u8 atkEffort[2];
    u8 defEffort[2];
    u8 agiEffort[2];
    u8 spEffort[2];
    u8 powerRand1 : 4;
    u8 powerRand2 : 4;
    u8 powerRand3 : 4;
    u8 powerRand4 : 4;
    u8 wazaPP1 : 6;
    u8 wazaPPUP1 : 2;
    u8 wazaPP2 : 6;
    u8 wazaPPUP2 : 2;
    u8 wazaPP3 : 6;
    u8 wazaPPUP3 : 2;
    u8 wazaPP4 : 6;
    u8 wazaPPUP4 : 2;
  };

  struct CONVERT_NAME_JP
  {
    u8 name[6];
  };


  struct CONVERT_NAME_EN
  {
    u8 name[11];
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  LauncherKousaka(void);
  virtual ~LauncherKousaka();

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectListener
   * @brief   決定時のリスナー
   * @date    2015.02.18
   *
   * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
   * @param   launcher  呼び出し元のランチャークラス
   *
   * @return  ランチャーへのリクエストID
   */
  //-----------------------------------------------------------------------------
  virtual launcher::ListData::RequestID SelectListener( u32 pos, launcher::LauncherProc * launcher );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetArcDataID
   * @brief   アークデータID取得
   * @date    2015.02.18
   *
   * @return  アークデータID
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブル取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブル
   */
  //-----------------------------------------------------------------------------
  virtual const u32 * GetListTable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブルの要素数取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブルの要素数
   */
  //-----------------------------------------------------------------------------
  virtual u32 GetListTableSize(void);

public:
#ifdef  GF_PLATFORM_CTR



  bool _ConvertVCSavedata( u8* pData , u32 type );

  void _ConvertString( gfl2::str::StrBuf* pOutput , void* pAddr , u32 type );

  u16 SwapU16(u16 val);
  u32 SwapU32(u32 val);

#endif
private:


};










GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)


#endif  // D_KOUSAKA_H_INCLUDED


#endif  // PM_DEBUG
