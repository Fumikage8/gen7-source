
#if defined(GF_PLATFORM_CTR)


//==============================================================================
/**
 * @file   QRReaderFrame_QRSimpleRead.cpp
 * @date   2016/01/28 Thu. 16:41:17
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <pml/include/pokepara/pml_PokemonCoreParam.h>
#include  <DeviceCamera/include/gfl2_DeviceCamera.h>

#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "NetStatic/NetAppLib/include/QR/QRUtility.h"
#include  "NetStatic/NetAppLib/include/QR/QRRecordAndDiary.h"

#include  "../System/QRReaderWork.h"
#include  "../QRReaderResource.h"

#include  "./QRReaderFrame_QRSimpleRead.h"
// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_WaitForReadyToCamera,

    STATE_QRDecodeStart,
    STATE_QRDecode,

    STATE_QRDecodeRetryStart,
    STATE_QRDecodeRetryPending,

    STATE_ShowResult,
    STATE_ShowInstruction,

    STATE_CameraEndRequest,
    STATE_WaitForCameraEnd,
    STATE_Exit,

    STATE_     /*  terminator  */
  };

  static const u32  DeviceCameraImageW            = 512;
  static const u32  DeviceCameraImageH            = 384;
  static const u32  DeviceCameraImageBPP          = 2;
  static const u32  DeviceCameraImageBytes        = DeviceCameraImageW * DeviceCameraImageH * DeviceCameraImageBPP;
  static const u32  DeviceCameraImageDirtyFrame   = 30;


  /** QRDecoderに入力する画像のPixelサイズ  */
  static const u16  QRSourceImageSize[2]  =
                    {
                      DeviceCameraImageW, DeviceCameraImageH
                    };

  /** デコード対象とする矩形のサイズ  */
  static const u32  QRTargetImageSize[2]  =
                    {
                      DeviceCameraImageW, DeviceCameraImageH
                    };

  /** デコード対象とする矩形  */
  static const s32  QRTargetImageRect[4]  = {
                      (QRSourceImageSize[1] - QRTargetImageSize[1]) / 2,                          /*  top     */
                      (QRSourceImageSize[0] - QRTargetImageSize[0]) / 2,                          /*  left    */
                      (QRSourceImageSize[0] - QRTargetImageSize[0]) / 2 + QRTargetImageSize[0],   /*  right   */
                      (QRSourceImageSize[1] - QRTargetImageSize[1]) / 2 + QRTargetImageSize[1],   /*  bottom  */
                    };

  /** カメラを終了させるフレーム数（終了メッセージの視認性を確保するため）    */
  static const u32  CameraEndFrames           = 15;

} /*  namespace  */



namespace NetApp   {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_QRSimpleRead::Frame_QRSimpleRead(QRReaderWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_IsCameraError(false)
  , m_IsCameraErrorInfoEnd(false)
  /*  QR  */
  , m_QRDecoder()
  , m_pQRDecodeSource(NULL)
  , m_QRDecodeDirtyFrame(DeviceCameraImageDirtyFrame)
  , m_CameraEndFrameCount(CameraEndFrames)
  , m_IsDeviceCameraUpdate(true)
  , m_IsQRDecoderUpdate(true)
  , m_IsQRDecodeRequestEnable(false)
  , m_IsOK(false)
  , m_IsDataTypeMismatch(false)
  /*  view  */
  , m_pUpperView(NULL)
  , m_pUpperView_Pokemon(NULL)
  , m_pLowerView(NULL)
  , m_pMessageMenu(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_QRSimpleRead
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_QRSimpleRead::~Frame_QRSimpleRead()
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
bool  Frame_QRSimpleRead::startup()
{
  /*  view  */
  m_pUpperView          = m_pViewContainer->m_pUpperView_Main;
  m_pUpperView_Pokemon  = m_pViewContainer->m_pUpperView_Pokemon;
  m_pLowerView          = m_pViewContainer->m_pLowerView_Main;
  m_pMessageMenu        = m_pLowerView->GetMessageMenu();

  m_pViewContainer->SetScene(QRReaderViewContainer::SCENE_QRRead);
  m_pViewContainer->SetAllInputEnable(true);


  m_pUpperView->SetBatteryGaugeInvilibeImmediate();
  m_pUpperView->SetGaugeEnable(false);

  m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_Camera);
  m_pUpperView_Pokemon->SetViewMode(UpperView_Pokemon::VIEWMODE_None);
  m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_None);
  m_pMessageMenu->Hide();

  /*  DeviceCamera  */
  {
    using namespace gfl2::devicecamera;

    DeviceCamera::Config  config;


//    config.pAppHeap           = m_Heap.GetDeviceHeap();
    config.pAppHeap           = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
    config.pDevHeap           = m_pWork->GetDeviceHeap();

    config.camType            = CAM_OUT_R;

    config.resolution.Set(DeviceCameraImageW, DeviceCameraImageH);
    config.resolution.SetCropCenter(640, 480);

    config.photoMode.type     = MODE_LETTER;

    config.trimming.isEnable  = false;


    config.exposure.isEnable    = true;
    config.exposure.value       = 2;
    config.exposure.rectUnit[0] = 0;
    config.exposure.rectUnit[1] = 0;
    config.exposure.rectUnit[2] = 16;
    config.exposure.rectUnit[3] = 16;


    config.whiteBalance.type        = WB_AUTO;
    config.whiteBalance.rectUnit[0] = 0;
    config.whiteBalance.rectUnit[1] = 0;
    config.whiteBalance.rectUnit[2] = 16;
    config.whiteBalance.rectUnit[3] = 16;

    config.sharpness.value      = 5;
    config.flip.type            = FLIP_NONE;
    config.frameRate.type       = FPS_30;
    config.effect.type          = EFFECT_NONE;
    config.contrast.type        = CONTRAST_HIGH;
    config.lensCorrection.type  = LC_NORMAL;
    config.noiseFilter.isEnable = false ;
    config.outImageSize[0]      = 512;
    config.outImageSize[1]      = 512;
    config.outputFormat         = Y2RThread::OUTPUT_RGB_24;
    config.alpha                = 0xff;


    GFL_NEW(m_pWork->GetDeviceHeap()) DeviceCamera();

    DeviceCamera* pDeviceCamera = GFL_SINGLETON_INSTANCE(DeviceCamera);

    pDeviceCamera->Setup(config);
    pDeviceCamera->Start();
  }

  /*  QR  */
  {
    QRUtil::QRDecoder::Config config;
    gfl2::heap::HeapBase*     pHeapBase = m_pWork->GetDeviceHeap();
    
    config.pHeapBase              = pHeapBase;
    config.decodeImageSizeMax[0]  = DeviceCameraImageW;    /*  入力画像サイズ：カメラの出力画像のうち、有効部分  */
    config.decodeImageSizeMax[1]  = DeviceCameraImageH;
    
    m_QRDecoder.Setup(config);
    m_pQRDecodeSource = GflHeapAllocMemoryAlign(pHeapBase, DeviceCameraImageBytes, 32);
  }

  m_pMessageMenu->Show(MessageMenu::MODE_ActivateCameraInformation);
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_QRSimpleRead::setListener()
{
//  m_pUpperView->SetEventHandler(this);
  m_pLowerView->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_QRSimpleRead::removeListener()
{
//  m_pUpperView->SetEventHandler(NULL);
  m_pLowerView->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_QRSimpleRead::updateSequence()
{
  using namespace gfl2::devicecamera;
  DeviceCamera*   pDeviceCamera       = GFL_SINGLETON_INSTANCE(DeviceCamera);
  bool            isDeviceCameraReady = false;

  /*  devicecamera  */
  if(m_IsDeviceCameraUpdate)
  {
    pDeviceCamera->Update();
    isDeviceCameraReady = pDeviceCamera->SetPicturePaneTexture(
                            m_pUpperView->GetCameraImagePane(),
                            Y2RThread::CH_1
                          );
  }

  /*  QR  */
  if(m_IsQRDecoderUpdate)
  {
    m_QRDecoder.Update();
  }

  switch(m_State)
  {
    case  STATE_Entry:
      QRDecodeUpdateEnable(true, true);
      m_State = STATE_WaitForReadyToCamera;
      /*  fallthrough  */



    case  STATE_WaitForReadyToCamera:
    {
      if(isDeviceCameraReady || m_IsCameraErrorInfoEnd)
      {
        m_pViewContainer->SetAllInputEnable(true);
        m_pLowerView->RequestInputClean();    /*  ボタン類のリリースを要求  */

        m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_QRRead);
        m_State = STATE_QRDecodeStart;
      }
      else if(!m_IsCameraError && pDeviceCamera->IsError())
      {
        m_IsCameraError = true;
        m_pViewContainer->SetAllInputEnable(true);
        m_pLowerView->RequestInputClean();    /*  ボタン類のリリースを要求  */
        m_pMessageMenu->Show(MessageMenu::MODE_CameraBroken);
      }
    }
      break;


    /*
      decode
    */
    case  STATE_QRDecodeStart:
    {
      const u8* pYUVImage  = pDeviceCamera->GetYUVImage(gfl2::devicecamera::CAM_OUT_R);

      if(m_IsQRDecodeRequestEnable)
      {
        m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_OnScan);
        m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_QRRead, LowerView_Main::VIEWMODE_SUB_QRSimpleReadOnScan);

        if(pYUVImage)
        {
          /*  cameraのReadBufferのSwapはUpdateからなのでcopyは排他不要  */
          memcpy(m_pQRDecodeSource, pYUVImage, DeviceCameraImageBytes);
          m_QRDecoder.StartDecode(
            m_pQRDecodeSource,
            QRSourceImageSize,
            QRTargetImageRect,
            true
          );
          m_State = STATE_QRDecode;
        }
      }
      else
      {
        m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_Camera);
        m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_QRRead, LowerView_Main::VIEWMODE_SUB_QRSimpleRead);
      }




    }
      break;

    case  STATE_QRDecode:
    {
      if(m_QRDecoder.IsEnd())
      {
        /*  処理が完了  */
        if(m_QRDecoder.IsSuccess())
        {
          /*  QRが認識され、データを取得できた  */
//          GFL_PRINT("----decode success----\n");

          /*  結果を分析  */
          AnalyzeDecodedBinary();
          if(m_IsOK)
          {
            Sound::PlaySE(SEQ_SE_SYS_NJ_025);
          }
        }
        else
        {
          /*  QR画像が認識できなければ即時再開  */
          m_State = STATE_QRDecodeStart;
        }
      }
    }
      break;

    /*
      retry
    */
    case  STATE_QRDecodeRetryStart:
      QRDecodeUpdateEnable(true, true);
      m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_QRRead, LowerView_Main::VIEWMODE_SUB_QRSimpleRead);
      m_pLowerView->RequestInputClean();    /*  ボタン類のリリースを要求  */
      m_pMessageMenu->Show(MessageMenu::MODE_QRReadGuide);
      m_QRDecodeDirtyFrame  = DeviceCameraImageDirtyFrame;
      m_State               = STATE_QRDecodeRetryPending;
      /*  through  */

    case  STATE_QRDecodeRetryPending:
      if(m_QRDecodeDirtyFrame)
      {
        --m_QRDecodeDirtyFrame;
      }
      if(!m_QRDecodeDirtyFrame)
      {
        m_State = STATE_QRDecodeStart;
      }
      break;


    /*
      result
    */
    case  STATE_ShowResult:
      break;


    /*
      devicecamera shutdown
    */
    case STATE_CameraEndRequest:
      QRDecodeUpdateEnable(false, false);
      m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_None);

      /*  NMcat[2435]   QRコード読み込みアプリから戻る際に不要なボタンが一瞬見える  */
      /*    animで見えてしまうので現状維持で終了することとする                      */
//      m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_None);

      m_pMessageMenu->Show(MessageMenu::MODE_InactivateCameraInformation);    /*  カメラを終了しています  */
      m_CameraEndFrameCount = CameraEndFrames;
      m_State               = STATE_WaitForCameraEnd;
      break;

    case STATE_WaitForCameraEnd:
      if(m_CameraEndFrameCount) --m_CameraEndFrameCount;
      
      if(!m_CameraEndFrameCount)
      {
        pDeviceCamera->Cleanup();
        m_State = STATE_Exit;
      }
      break;


    case  STATE_Exit:
      m_pWork->SetFrameResult( m_Result );
      break;
  }
  
    m_IsQRDecodeRequestEnable = false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_QRSimpleRead::cleanup()
{
  bool  bAllDone = false;

  /*  QR  */
  {
    m_QRDecoder.Cleanup();
    GflHeapFreeMemory(m_pQRDecodeSource);
    m_pQRDecodeSource = NULL;
  }

  /*  DeviceCamera  */
  {
    using namespace gfl2::devicecamera;

    DeviceCamera*   pDeviceCamera  = GFL_SINGLETON_INSTANCE(DeviceCamera);

    pDeviceCamera->Cleanup();
    GFL_SAFE_DELETE(pDeviceCamera);
  }

  bAllDone  = true;


  return  bAllDone;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_QRSimpleRead::ShowInstruction(void)
{
  switch(m_pWork->m_pAppParam->m_BootMode)
  {
    case  APP_PARAM::BOOTMODE_BattleTeam:   m_pMessageMenu->Show(MessageMenu::MODE_QRSearch_TeamData);          break;
    case  APP_PARAM::BOOTMODE_FriendMatch:  m_pMessageMenu->Show(MessageMenu::MODE_QRSearch_FriendMatchData);   break;
    case  APP_PARAM::BOOTMODE_LiveMatch:    m_pMessageMenu->Show(MessageMenu::MODE_QRSearch_LiveMatchData);     break;


    case  APP_PARAM::BOOTMODE_QRScan:  /*  fallthrough  */
    default:
      GFL_ASSERT(false);
      break;
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
void  Frame_QRSimpleRead::LowerView_Main_OnEvent(const LowerView_Main::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Main::IEventHandler::EVENT_EOM:
      if(m_State == STATE_ShowResult)
      {
        if(m_IsOK)
        {
          ExitFrame(RESULT_QRSimpleRead_OK);
        }
        else if(m_IsDataTypeMismatch)
        {
          /*  data種別が違う  */
          /*  ～を読み込んで  */
          ShowInstruction();
          m_State = STATE_ShowInstruction;
        }
        else
        {
          /*  data種別は正しい    */
          /*  messageなしでretry  */
          m_State = STATE_QRDecodeRetryStart;
        }
      }
      else if(m_State == STATE_ShowInstruction)
      {
         m_State = STATE_QRDecodeRetryStart;
      }
      else if(m_State == STATE_WaitForReadyToCamera)
      {
        /*  故障メッセージ終了  */
        m_pViewContainer->SetAllInputEnable(false);
        m_IsCameraErrorInfoEnd  = true;
      }
      break;

    case  LowerView_Main::IEventHandler::EVENT_MessageFinish:
      break;



    case  LowerView_Main::IEventHandler::EVENT_Back:
      ExitFrame();
      break;

    case  LowerView_Main::IEventHandler::EVENT_QRRead:
      break;

    case  LowerView_Main::IEventHandler::EVENT_FieldScan:
      break;

    case  LowerView_Main::IEventHandler::EVENT_ModeChange:
      break;

    case  LowerView_Main::IEventHandler::EVENT_Information:
      break;

    case LowerView_Main::IEventHandler::EVENT_QRScanButtonHolding:
      m_IsQRDecodeRequestEnable = true;
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_QRSimpleRead::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
//  m_State   = STATE_Exit;
  m_State   = STATE_CameraEndRequest;
  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_QRSimpleRead::AnalyzeDecodedBinary(void)
{
  switch(m_pWork->m_pAppParam->m_BootMode)
  {
    case  APP_PARAM::BOOTMODE_BattleTeam:   AnalyzeDecodedBinary_BattleTeam();    break;
    case  APP_PARAM::BOOTMODE_FriendMatch:  AnalyzeDecodedBinary_FriendMatch();   break;
    case  APP_PARAM::BOOTMODE_LiveMatch:    AnalyzeDecodedBinary_LiveMatch();     break;


    case  APP_PARAM::BOOTMODE_QRScan:  /*  fallthrough  */
    default:
      GFL_ASSERT(false);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    チーム選択用解析
  */
//------------------------------------------------------------------
void  Frame_QRSimpleRead::AnalyzeDecodedBinary_BattleTeam(void)
{
  const void* pData     = NULL;
  u32         dataBytes = 0;


  pData = m_QRDecoder.GetDecodeData(&dataBytes);

  pml::PokeParty*   pDest = m_pWork->m_pAppParam->pDst_PokeParty;

  const NetApp::QR::QR_TYPE_BATTLE_TEAM result  = NetApp::QR::QRUtility::AnalyzeQRBinaryForBattleTeam( m_pWork->GetDeviceHeap(), pData, dataBytes, pDest);

  m_IsOK                = false;
  m_IsDataTypeMismatch  = false;
  switch(result)
  {
    case  NetApp::QR::QR_TYPE_BATTLE_TEAM_OK:
      m_pMessageMenu->Show(MessageMenu::MODE_QROK);
      m_pWork->m_pAppParam->m_QRSimpleReadDecoded = true;
      m_IsOK  = true;
      break;

    case  NetApp::QR::QR_TYPE_BATTLE_TEAM_FRIEND_NG:        //仲間大会用なのでNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_FriendMatchData);
      m_IsDataTypeMismatch  = true;
      break;

    case  NetApp::QR::QR_TYPE_BATTLE_TEAM_LIVE_NG:          //ライブ大会用なのでNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_LiveMatchData);
      m_IsDataTypeMismatch  = true;
      break;

    case  NetApp::QR::QR_TYPE_BATTLE_TEAM_ZUKAN_NG:         //図鑑系なのでNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_ZukanData);
      m_IsDataTypeMismatch  = true;
      break;

    /*
      Data種別は正しいが、内容に問題がある
    */
    case  NetApp::QR::QR_TYPE_BATTLE_TEAM_ROM_NG:           //ROMコードがNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_ROM);
      break;

    case  NetApp::QR::QR_TYPE_BATTLE_TEAM_REGION_NG:        //リージョンがNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_Region);
      break;

    case  NetApp::QR::QR_TYPE_BATTLE_TEAM_GAMESYNC_ID_NG:   //ゲームシンクID一致せず
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_GameSyncID);
      break;
  }


  /*  diary  */
  {
    NetApp::QR::QRDiaryFunc(result);
  }

  QRDecodeUpdateEnable(false, false);
  m_State = STATE_ShowResult;
}


//------------------------------------------------------------------
/**
  *  @brief    なかま大会用解析
  */
//------------------------------------------------------------------
void  Frame_QRSimpleRead::AnalyzeDecodedBinary_FriendMatch(void)
{
  const void* pData     = NULL;
  u32         dataBytes = 0;


  pData = m_QRDecoder.GetDecodeData(&dataBytes);

  Regulation*   pDest = m_pWork->m_pAppParam->pDst_Regulation;

  const NetApp::QR::QR_TYPE_FRIEND_CUP result  = NetApp::QR::QRUtility::AnalyzeQRBinaryForFriendCup( m_pWork->GetDeviceHeap(), pData, dataBytes, pDest);

  m_IsOK                = false;
  m_IsDataTypeMismatch  = false;
  switch(result)
  {
    case  NetApp::QR::QR_TYPE_FRIEND_CUP_OK:
      m_pMessageMenu->Show(MessageMenu::MODE_QROK);
      m_pWork->m_pAppParam->m_QRSimpleReadDecoded = true;
      m_IsOK  = true;
      break;

    case  NetApp::QR::QR_TYPE_FRIEND_CUP_LIVE_NG:           //ライブ大会用なのでNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_LiveMatchData);
      m_IsDataTypeMismatch  = true;
      break;

    case  NetApp::QR::QR_TYPE_FRIEND_CUP_BATTLE_TEAM_NG:    //バトルチーム用なのでNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_TeamData);
      m_IsDataTypeMismatch  = true;
      break;

    case  NetApp::QR::QR_TYPE_FRIEND_CUP_ZUKAN_NG:          //図鑑系なのでNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_ZukanData);
      m_IsDataTypeMismatch  = true;
      break;

    /*
      Data種別は正しいが、内容に問題がある
    */
    case  NetApp::QR::QR_TYPE_FRIEND_CUP_ROM_NG:            //ROMコードがNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_ROM);
      break;

    case  NetApp::QR::QR_TYPE_FRIEND_CUP_REGION_NG:         //リージョンがNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_Region);
      break;

    case  NetApp::QR::QR_TYPE_FRIEND_CUP_GAMESYNC_ID_NG:    //ゲームシンクID一致せず
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_GameSyncID);
      break;
  }

  /*  diary  */
  {
    NetApp::QR::QRDiaryFunc(result);
  }
  QRDecodeUpdateEnable(false, false);
  m_State = STATE_ShowResult;

}


//------------------------------------------------------------------
/**
  *  @brief    ライブ大会用解析
  */
//------------------------------------------------------------------
void  Frame_QRSimpleRead::AnalyzeDecodedBinary_LiveMatch(void)
{
  const void* pData     = NULL;
  u32         dataBytes = 0;


  pData = m_QRDecoder.GetDecodeData(&dataBytes);

  Regulation*   pDest = m_pWork->m_pAppParam->pDst_Regulation;

  const NetApp::QR::QR_TYPE_LIVE_CUP result  = NetApp::QR::QRUtility::AnalyzeQRBinaryForLiveCup( m_pWork->GetDeviceHeap(), pData, dataBytes, pDest);

  m_IsOK                = false;
  m_IsDataTypeMismatch  = false;
  switch(result)
  {
    case  NetApp::QR::QR_TYPE_LIVE_CUP_OK:
      m_pMessageMenu->Show(MessageMenu::MODE_QROK);
      m_pWork->m_pAppParam->m_QRSimpleReadDecoded = true;
      m_IsOK  = true;
      break;


    case  NetApp::QR::QR_TYPE_LIVE_CUP_FRIEND_CUP_NG:   //仲間大会用なのでNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_FriendMatchData);
      m_IsDataTypeMismatch  = true;
      break;

    case  NetApp::QR::QR_TYPE_LIVE_CUP_BATTLE_TEAM_NG:  //バトルチーム用なのでNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_TeamData);
      m_IsDataTypeMismatch  = true;
      break;

    case  NetApp::QR::QR_TYPE_LIVE_CUP_ZUKAN_NG:        //図鑑系なのでNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_ZukanData);
      m_IsDataTypeMismatch  = true;
      break;

    /*
      Data種別は正しいが、内容に問題がある
    */
    case  NetApp::QR::QR_TYPE_LIVE_CUP_ROM_NG:          //ROMコードがNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_ROM);
      break;

    case  NetApp::QR::QR_TYPE_LIVE_CUP_REGION_NG:       //リージョンがNG
      m_pMessageMenu->Show(MessageMenu::MODE_QRNG_Region);
      break;
  }

  /*  diary  */
  {
    NetApp::QR::QRDiaryFunc(result);
  }
  QRDecodeUpdateEnable(false, false);
  m_State = STATE_ShowResult;
}


} /*  namespace QRReader */
} /*  namespace NetApp   */


#endif  /* #if defined(GF_PLATFORM_CTR)  */
