#if PM_DEBUG
//=============================================================================
/**
 * @file    d_muto.cpp
 * @brief   武藤用ランチャーリスト処理
 * @author  Yu Muto
 * @date    2015.05.08
 */
//=============================================================================

#include <System/include/HeapDefine.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Sound/include/Sound.h>
#include "../include/d_muto.h"

#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_mutou.h>

#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/DllProc.h>

#include <App/AppEvent/include/PokeList/PokeListEvent.h>
#include <App/FInderStudioStatic/include/FinderStudioDefine.h>
#include <Test/LayoutArcTest/include/LayoutArcTest.h>
#include <Test/DrawUtilTextTest/include/DrawUtilTextProc.h>

#include  "App/AppDemo/include/AppDemoProc.h"

#include  "Test/MutoTest/include/MutoTest_TestProc.h"
#include  "Test/MutoTest/include/MutoTest_TestProc_GiftDemo.h"
#include  "Test/MutoTest/include/MutoTest_TestProc_SurfUI.h"
#include  "Test/MutoTest/include/MutoTest_TestProc_LunaSolDiveUI.h"
#include  "Test/MutoTest/include/CTROnly/TestProc_Delivery.h"
#include  "Test/MutoTest/include/CTROnly/TestProc_DeviceCamera.h"
#include  "Test/MutoTest/include/CTROnly/TestProc_SendTool_LiveRegulation.h"
#include  "Test/MutoTest/include/CTROnly/TestProc_HIOView.h"

#include  "NetStatic/NetEvent/include/MysteryGiftEvent.h"
#include  "NetStatic/NetEvent/include/LiveEvent.h"
#include  "NetStatic/NetEvent/include/EvolutionEvent.h"
#include  "NetStatic/NetEvent/include/BattleSpotEvent.h"
#include  "NetStatic/NetEvent/include/EggHatchingEvent.h"
#include  "NetStatic/NetEvent/include/QRReaderEvent.h"
#include  "NetStatic/NetEvent/include/JoinFestaPokeStatusEvent.h"
#include  "NetStatic/NetEvent/include/PhotoViewEvent.h"



GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)


/**
 *
 */
const u32 LauncherMuto::ItemList[] =
{
  Muto_DebugMenuItem_MysteryGift,
  Muto_DebugMenuItem_MutoTest,
  Muto_DebugMenuItem_Live,
  Muto_DebugMenuItem_DeviceCamera,
  Muto_DebugMenuItem_AppDemo,
  Muto_DebugMenuItem_Evolution,
  Muto_DebugMenuItem_Evolution_Alola,
  Muto_DebugMenuItem_Evolution_NormalToAlola,
  Muto_DebugMenuItem_BattleSpot,
  Muto_DebugMenuItem_EggHatching,
  Muto_DebugMenuItem_EggHatching_Alola,
  Muto_DebugMenuItem_SendTool_LiveRegulation,
  Muto_DebugMenuItem_BGMPop,
  Muto_DebugMenuItem_QRRead_Full,
  Muto_DebugMenuItem_QRRead_Simple,
  Muto_DebugMenuItem_QRMake,
  Muto_DebugMenuItem_GiftDemo,
  Muto_DebugMenuItem_SurfUI,
  Muto_DebugMenuItem_BFCStatus,
  Muto_DebugMenuItem_BFCTrainerList,
  Muto_DebugMenuItem_DiveUI,
  Muto_DebugMenuItem_PhotoView,
};




//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/09/04 Fri. 19:25:58
  */
//==============================================================================
class LauncherMuto::Inner
{
  GFL_FORBID_COPY_AND_ASSIGN(Inner);
public:
  Inner(gfl2::heap::HeapBase* pHeapBase)
    : m_pHeapBase(pHeapBase)
    , m_bBGMPushed(false)
  {
    evolutionInfo.m_pTargetPokepara = NULL;

    eggHatchingInfo.m_pParentName         = GFL_NEW(m_pHeapBase) gfl2::str::StrBuf(128, m_pHeapBase);
    eggHatchingInfo.m_pParentPokepara[0]  = NULL;
    eggHatchingInfo.m_pParentPokepara[1]  = NULL;
    eggHatchingInfo.m_pTargetEgg          = NULL;
  }


  virtual ~Inner()
  {
    ReleasePokepara();
    ReleaseEgg();
    ReleasePhoto();
    GFL_SAFE_DELETE(eggHatchingInfo.m_pParentName);
  }


  void  CreatePokepara(const MonsNo monsNo, const u16 lv = 1, const u64 id = 0UL, const pml::FormNo formNo = 0)
  {
    ReleasePokepara();
    evolutionInfo.m_pTargetPokepara = GFL_NEW(m_pHeapBase) pml::pokepara::PokemonParam( m_pHeapBase, monsNo, lv, id);
    evolutionInfo.m_pTargetPokepara->ChangeFormNo(formNo);
  }

  void  ReleasePokepara(void)
  {
    GFL_SAFE_DELETE(evolutionInfo.m_pTargetPokepara);
  }


  void  CreateEgg(const MonsNo monsNo1, const MonsNo monsNo2, const pml::FormNo formNo = 0)
  {
    const u32 lv  = 1;
    const u64 id  = 0ULL;
    
    pml::pokepara::EggGenerator::Setting  setting;
    
    
    ReleaseEgg();
    eggHatchingInfo.m_pParentPokepara[0]  = GFL_NEW(m_pHeapBase) pml::pokepara::PokemonParam( m_pHeapBase, monsNo1, lv, id);
    eggHatchingInfo.m_pParentPokepara[1]  = GFL_NEW(m_pHeapBase) pml::pokepara::PokemonParam( m_pHeapBase, monsNo2, lv, id);

    eggHatchingInfo.m_pParentPokepara[0]->ChangeFormNo(formNo);
    eggHatchingInfo.m_pParentPokepara[1]->ChangeFormNo(formNo);
    eggHatchingInfo.m_pParentPokepara[0]->ChangeSex(pml::SEX_MALE);
    eggHatchingInfo.m_pParentPokepara[1]->ChangeSex(pml::SEX_FEMALE);
    eggHatchingInfo.m_pParentPokepara[0]->GetNickName(eggHatchingInfo.m_pParentName);
    gfl2::math::Random random;
    random.Initialize();

    setting.IDNo                    = 0;
    setting.parentName              = eggHatchingInfo.m_pParentName;
    setting.haveItem_HIKARUOMAMORI  = false;
    setting.pRandom                 = &random;
    setting.realRegionFormNo        = 0;
    
    eggHatchingInfo.m_pTargetEgg          = pml::pokepara::EggGenerator::CreateEgg(
                                                                  m_pHeapBase,
                                                                  eggHatchingInfo.m_pParentPokepara[0],
                                                                  eggHatchingInfo.m_pParentPokepara[1],
                                                                  setting
                                                                 );
    eggHatchingInfo.m_pTargetEgg->ChangeFormNo(formNo);
  }

  void  ReleaseEgg(void)
  {
    GFL_SAFE_DELETE(eggHatchingInfo.m_pParentPokepara[0]);
    GFL_SAFE_DELETE(eggHatchingInfo.m_pParentPokepara[1]);
    GFL_SAFE_DELETE(eggHatchingInfo.m_pTargetEgg);
  }

  /*
    photo
  */
  void  CreatePhoto(void)
  {
    ReleasePhoto();

    NetApp::PhotoView::APP_PARAM::PhotoInfo photoInfo;

    photoInfo.photoData.photo_param  = App::FinderStudio::Static::c_DefPhotoParam;
    photoInfo.name[0]     = L'て';
    photoInfo.name[1]     = L'す';
    photoInfo.name[2]     = L'と';
    photoInfo.name[3]     = L'\0';

    photoViewInfo.m_pPhotoInfoList  = NetApp::PhotoView::APP_PARAM::CreatePhotoInfoList(m_pHeapBase, 1);
    photoViewInfo.m_pPhotoInfoList->PushBack(photoInfo);
  }

  void  ReleasePhoto(void)
  {
    GFL_SAFE_DELETE(photoViewInfo.m_pPhotoInfoList);
  }


  /*
    members
  */
  gfl2::heap::HeapBase*           m_pHeapBase;
  /*    */
  struct  {
    pml::pokepara::PokemonParam*  m_pTargetPokepara;
  }evolutionInfo;

  struct  {
    gfl2::str::StrBuf*            m_pParentName;
    pml::pokepara::PokemonParam*  m_pParentPokepara[2];
    pml::pokepara::PokemonParam*  m_pTargetEgg;
  }eggHatchingInfo;

  struct  {
    gfl2::util::List<NetApp::PhotoView::APP_PARAM::PhotoInfo>*  m_pPhotoInfoList;
  }photoViewInfo;


  bool                            m_bBGMPushed;
};





//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LauncherMuto::LauncherMuto(void)
  : m_pInner(NULL)
  , m_pLocalHeapBase(NULL)
{
  /*  heap  */
  {
    const u32 localHeapSize = 0x00080000;

    m_pLocalHeapBase  = GFL_CREATE_LOCAL_HEAP(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ), 0x00080000, gfl2::heap::HEAP_TYPE_EXP, false );
  }

  /*  inner  */
  {
    m_pInner  = GFL_NEW(m_pLocalHeapBase) Inner(m_pLocalHeapBase);
  }

  Sound::StartBGMReq(STRM_BGM_VS_TRAINER_NJ);
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherMuto::~LauncherMuto()
{
  Sound::EndBGMReq();

  /*  inner  */
  {
    GFL_SAFE_DELETE(m_pInner);
  }


  /*  heap  */
  {
    gfl2::heap::Manager::DeleteHeap( m_pLocalHeapBase );
    m_pLocalHeapBase  = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetArcDataID
 * @brief   アークデータID取得
 * @date    2015.02.18
 *
 * @return  アークデータID
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID LauncherMuto::GetArcDataID(void)
{
  return GARC_debug_message_d_mutou_DAT;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブル取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブル
 */
//-----------------------------------------------------------------------------
const u32*	LauncherMuto::GetListTable(void)
{
  return ItemList;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブルの要素数取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブルの要素数
 */
//-----------------------------------------------------------------------------
u32 LauncherMuto::GetListTableSize(void)
{
  return GFL_NELEMS( ItemList );
}

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
launcher::ListData::RequestID	LauncherMuto::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
	/*
		//! リスナーからランチャーへのリクエストID
		enum RequestID
		{
			REQID_NONE = 0,   //!< リクエストなし
			REQID_CALL,       //!< 他Proc呼び出し
			REQID_RESET,      //!< リストを再設定
			REQID_FINISH      //!< 終了
		};
	*/


  {
    if(!m_pInner->m_bBGMPushed)
    {
      Sound::PushBGMReq(Sound::BGM_FADE_VERY_FAST);
      m_pInner->m_bBGMPushed  = true;
    }
  }

  // キャンセル
  if(false 
  	|| (pos == launcher::LauncherProc::CURSOR_POS_CANCEL)
  	|| (GFL_NELEMS(ItemList) <= pos)
  )
  {
    launcher->CreateListData<StartLauncher>();
    return launcher::ListData::REQID_RESET;
  }

  GFL_PRINT("SelectListener [%d]", pos);


  switch( ItemList[pos] )
  {
    /** おくりもの配信テスト    */
    case Muto_DebugMenuItem_MutoTest:	{
      using namespace		Test::MutoTest;

//      NIJI_RO_PROC_CALL<GameSys::GameProcManager, TestProc>
      NIJI_RO_PROC_CALL<GameSys::GameProcManager, TestProc_Delivery>
      (
        pGameManager->GetProcManager(),
        pGameManager->GetAsyncFileManager(),
        "MutoTest.cro"
      );
      return launcher::ListData::REQID_CALL;
    }

    /** おくりもの  */
    case Muto_DebugMenuItem_MysteryGift:
      NetEvent::MysteryGift::MysteryGiftEvent_ForDebugMenu::StartEvent(pGameManager);
      return launcher::ListData::REQID_FINISH;

    /** ライブ大会  */
    case Muto_DebugMenuItem_Live:
      NetEvent::Live::LiveEvent_ForDebugMenu::StartEvent(pGameManager);
      return launcher::ListData::REQID_FINISH;

    /** デバイスカメラ  */
    case  Muto_DebugMenuItem_DeviceCamera:
    {
      using namespace		Test::MutoTest;

      NIJI_RO_PROC_CALL<GameSys::GameProcManager, TestProc_DeviceCamera>
      (
        pGameManager->GetProcManager(),
        pGameManager->GetAsyncFileManager(),
        "MutoTest.cro"
      );
      return launcher::ListData::REQID_CALL;
    }

    /** デモ  */
    case  Muto_DebugMenuItem_AppDemo:
    {
      App::AppDemo::AppDemoProcParam_Gift  param;
      GameSys::CallAppDemoProc_Gift( &param );
      return launcher::ListData::REQID_CALL;
    }


    /** 進化  */
    case  Muto_DebugMenuItem_Evolution:
    case  Muto_DebugMenuItem_Evolution_Alola:
    {
//      static u16     defaultMonsNo[2]  = {MONSNO_TUTININ, MONSNO_TEKKANIN};
//      static u16     defaultMonsNo[2]  = {MONSNO_OOKAMI1, MONSNO_OOKAMI2};
      static u16     defaultMonsNo[2]  = {MONSNO_SUNABA2, MONSNO_SUNABA2};    /*  最大サイズ  */
      const bool        isAlolaSpecies  = (ItemList[pos] == Muto_DebugMenuItem_Evolution_Alola);
      const MonsNo      monsBefore      = isAlolaSpecies ? MONSNO_KORATTA       : static_cast<MonsNo>(defaultMonsNo[0]);
      const MonsNo      monsAfter       = isAlolaSpecies ? MONSNO_RATTA         : static_cast<MonsNo>(defaultMonsNo[1]);
      const pml::FormNo formNo          = isAlolaSpecies ? FORMNO_KORATTA_ALOLA : 0;

      m_pInner->CreatePokepara(monsBefore, 1, 0, formNo);


      {
        NetEvent::Evolution::EvolutionEventParam  param;

        GameSys::GameData*      pGameData   = pGameManager->GetGameData();


        param.m_pTargetPokemon  = m_pInner->evolutionInfo.m_pTargetPokepara;
        param.m_MonsNoEvolve    = monsAfter;
        param.m_EvolveRootNum   = 0;

        param.m_Reason          = NetApp::Evolution::REASON_LevelUp;

        /*  所持品関連  */
        param.m_pPokeParty      = pGameData->GetPlayerParty();
        param.m_pMyItem         = pGameData->GetMyItem();


        NetEvent::Evolution::EvolutionEvent::StartEvent(pGameManager, param);
      }
      return launcher::ListData::REQID_CALL;
    }

    /** 進化（ピカチュウ）  */
    case  Muto_DebugMenuItem_Evolution_NormalToAlola:
    {
      const MonsNo      monsBefore      = MONSNO_PIKATYUU;
      const MonsNo      monsAfter       = MONSNO_RAITYUU;
      const pml::FormNo formNo          = FORMNO_PIKATYUU_NORMAL;

/*
  #define		FORMNO_PIKATYUU_NORMAL				( 0 )
  #define		FORMNO_PIKATYUU_STYLE				( 1 )
  #define		FORMNO_PIKATYUU_BEAUTIFUL				( 2 )
  #define		FORMNO_PIKATYUU_CUTE				( 3 )
  #define		FORMNO_PIKATYUU_CLEVER				( 4 )
  #define		FORMNO_PIKATYUU_STRONG				( 5 )
  #define		FORMNO_PIKATYUU_CONTEST				( 6 )
  #define		FORMNO_PIKATYUU_MAX				( 7 )
*/

      m_pInner->CreatePokepara(monsBefore, 1, 0, formNo);


      {
        NetEvent::Evolution::EvolutionEventParam  param;

        GameSys::GameData*      pGameData   = pGameManager->GetGameData();


        param.m_pTargetPokemon  = m_pInner->evolutionInfo.m_pTargetPokepara;
        param.m_MonsNoEvolve    = monsAfter;
        param.m_EvolveRootNum   = 0;

        param.m_Reason          = NetApp::Evolution::REASON_LevelUp;

        /*  所持品関連  */
        param.m_pPokeParty      = pGameData->GetPlayerParty();
        param.m_pMyItem         = pGameData->GetMyItem();


        NetEvent::Evolution::EvolutionEvent::StartEvent(pGameManager, param);
      }
    }
      return launcher::ListData::REQID_CALL;

    /** バトルスポット  */
    case  Muto_DebugMenuItem_BattleSpot:
    {
      {
        NetEvent::BattleSpot::BattleSpotEventParam  param;

        NetEvent::BattleSpot::BattleSpotEvent::StartEvent(pGameManager, param);
      }
      return launcher::ListData::REQID_CALL;
    }

    /** 孵化  */
    case  Muto_DebugMenuItem_EggHatching:
    case  Muto_DebugMenuItem_EggHatching_Alola:
    {
      {
        const bool        isAlolaSpecies  = (ItemList[pos] == Muto_DebugMenuItem_EggHatching_Alola);
        const MonsNo      monsParent[2]   = {
                            isAlolaSpecies ? MONSNO_KORATTA : MONSNO_AZUMAOU,
                            isAlolaSpecies ? MONSNO_KORATTA : MONSNO_AZUMAOU
                          };
        const pml::FormNo formNo          = isAlolaSpecies ? FORMNO_KORATTA_ALOLA : 0;
        NetEvent::EggHatching::EggHatchingEventParam  param;

        m_pInner->CreateEgg(monsParent[0], monsParent[1], formNo);

        param.m_pTargetEgg  = m_pInner->eggHatchingInfo.m_pTargetEgg;

        NetEvent::EggHatching::EggHatchingEvent::StartEvent(pGameManager, param);
      }
      return launcher::ListData::REQID_CALL;
    }

    /** 送信ツール    */
    case  Muto_DebugMenuItem_SendTool_LiveRegulation:
    {
      using namespace Test::MutoTest;

      NIJI_RO_PROC_CALL<GameSys::GameProcManager, TestProc_SendTool_LiveRegulation>
      (
        pGameManager->GetProcManager(),
        pGameManager->GetAsyncFileManager(),
        "MutoTest.cro"
      );
      return launcher::ListData::REQID_CALL;
    }
    
    /*  BGM復帰  */
    case  Muto_DebugMenuItem_BGMPop:
    {
      if(m_pInner->m_bBGMPushed)
      {
        Sound::PopBGMReq(Sound::BGM_FADE_VERY_FAST);
        m_pInner->m_bBGMPushed  = false;
      }
    }
      break;

    case  Muto_DebugMenuItem_QRRead_Full:
    {
      {
        NetEvent::QRReader::QRReaderEvent::StartEventFromField(pGameManager);
      }
    }
      return launcher::ListData::REQID_CALL;
    

    case  Muto_DebugMenuItem_QRRead_Simple:
    {
      {
        static NetEvent::QRReader::QRReaderEventParam  param;

        param.mode  = NetEvent::QRReader::QRReaderEventParam::MODE_Live;
        NetEvent::QRReader::QRReaderEvent::StartEvent(pGameManager, param);
      }
    }

    case  Muto_DebugMenuItem_QRMake:
    {
      using namespace Test::MutoTest;

      NIJI_RO_PROC_CALL<GameSys::GameProcManager, TestProc_HIOView>
      (
        pGameManager->GetProcManager(),
        pGameManager->GetAsyncFileManager(),
        "MutoTest.cro"
      );
      return launcher::ListData::REQID_CALL;
    }
    
    default:
      break;

    /*
      おくりものデモ
    */
    case  Muto_DebugMenuItem_GiftDemo:
    {
      using namespace Test::MutoTest;

      NIJI_RO_PROC_CALL<GameSys::GameProcManager, TestProc_GiftDemo>
      (
        pGameManager->GetProcManager(),
        pGameManager->GetAsyncFileManager(),
        "MutoTest.cro"
      );
      return launcher::ListData::REQID_CALL;
    }
    /*
      surfUI
    */
    case Muto_DebugMenuItem_SurfUI:
    {
      using namespace Test::MutoTest;

      NIJI_RO_PROC_CALL<GameSys::GameProcManager, TestProc_SurfUI>
        (
        pGameManager->GetProcManager(),
        pGameManager->GetAsyncFileManager(),
        "MutoTest.cro"
        );
      return launcher::ListData::REQID_CALL;
    }

    /*
      BattleFesCircle: status
    */
    case  Muto_DebugMenuItem_BFCStatus:
    {
      {
        static NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::Param  param;

//        m_pInner->CreatePokepara(MONSNO_PIKATYUU);
        m_pInner->CreatePokepara(MONSNO_HUSIGIDANE);

        param.partnerInfos[0].pTarget  = m_pInner->evolutionInfo.m_pTargetPokepara;
        NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::StartEvent(pGameManager, &param);
      }
    }
      return launcher::ListData::REQID_CALL;

    /*
      BattleFesCircle: trainerList
    */
    case  Muto_DebugMenuItem_BFCTrainerList:
    {
/*
      {
        static NetEvent::BattleFesCircleTrainerSelect::BattleFesCircleTrainerSelectEvent::Param  param;

        NetEvent::BattleFesCircleTrainerSelect::BattleFesCircleTrainerSelectEvent::StartEvent(pGameManager, param);
      }

      return launcher::ListData::REQID_CALL;
*/
      break;
    }
    
    /*
      DiveUI
    */
    case  Muto_DebugMenuItem_DiveUI:
    {
      using namespace Test::MutoTest;

      NIJI_RO_PROC_CALL<GameSys::GameProcManager, TestProc_LunaSolDiveUI>
        (
        pGameManager->GetProcManager(),
        pGameManager->GetAsyncFileManager(),
        "MutoTest.cro"
        );
      return launcher::ListData::REQID_CALL;
    }
    
    /*
      Photoview
    */
    case  Muto_DebugMenuItem_PhotoView:
    {
      NetEvent::PhotoView::PhotoViewEvent::Param  param;
      
      m_pInner->CreatePhoto();

      param.appParam.m_pPlayList = m_pInner->photoViewInfo.m_pPhotoInfoList;
      NetEvent::PhotoView::PhotoViewEvent::StartEvent(pGameManager, param);

      return launcher::ListData::REQID_CALL;
    }

  }

  return launcher::ListData::REQID_NONE;
}





GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif //PM_DEBUG
