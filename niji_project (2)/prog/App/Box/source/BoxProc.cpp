//=============================================================================================
/**
 * @file   BoxProc.cpp
 * @brief  呼び出しのProcコミットテスト
 * @author k.ohno
 * @date   2015.7.11
 */
//=============================================================================================


#include "System/include/HeapDefine.h"
#include "System/include/nijiAllocator.h"
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h" 

#include "App/Box/include/BoxProc.h"
#include "App/Box/include/BoxAppSetup.h"
#include "App/BoxSearch/include/BoxSearchAppParam.h"

#include "BoxLowBgFrame.h"
#include "BoxUppChatDraw.h"
#include "BoxUppPokeStatusFrame.h"
#include "BoxSimpleAccesser.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>

#include "AppLib/include/Tool/PokeIcon.h"
#include "AppLib/include/Tool/ItemIcon.h"

//  テキストのインクルード
#include "niji_conv_header/message/msg_box.h"

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameData.h>

#include <Fade/include/gfl2_FadeManager.h>
#include <Print/include/SystemFont.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )



Proc::Proc(void)  //コンストラクタ
{
  mSeq = 0;
  mEndSeq = 0;
  mbTIRTerminate = false;
  mbOnTradeExitReceived=false;
}


Proc::~Proc(void)  //デストラクタ
{
}


//----------------------------------------------------------------------------
/**
 *	@brief  フィールド復帰　再稼働
 */
//-----------------------------------------------------------------------------
//void Proc::Resume( void ){};

//----------------------------------------------------------------------------
/**
 *	@brief  フィールド削除　一時停止
 */
//-----------------------------------------------------------------------------
//void Proc::Suspend( void ){};

enum{
  LOCAL_INIT,
  LOCAL_ICON,
  LOCAL_OPEN_START,
  LOCAL_READ_START,
  LOCAL_READ_WAIT1,
  LOCAL_READ_WAIT2,
  LOCAL_READ_WAIT3,
  LOCAL_READ_WAIT4,
  LOCAL_READ_WAIT5,
  LOCAL_READ_WAIT6,
  LOCAL_READ_WAIT7,
  LOCAL_READ_WAIT,
  LOCAL_CLOSE_WAIT,
  LOCAL_CHAT_WAIT,
  FRAME_INIT_INIT,
  FRAME_INIT_WAIT,

};

//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::InitFunc(gfl2::proc::Manager* pManager)
{
  GameSys::GameManager*  pGameManager  = GameProc::GetGameManager();

  switch(mSeq){
  case LOCAL_INIT:
    {
#if defined(GF_PLATFORM_CTR)
      OHNO_PRINT("Proc::InitFunc %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif
      //----------------------------------------------------------------------------allocater
      //HEAPID_APP  本体に関するものはAPPめもりーから確保します
      gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      gfl2::heap::HeapBase *appHeap = devHeap;

      // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
      mpLowManagerHeap = GFL_CREATE_LOCAL_HEAP( devHeap, SIZE_PROC_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false );
      mpUpManagerHeap = GFL_CREATE_LOCAL_HEAP( devHeap, SIZE_PROC_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false );

      // このアプリ全体に必要なメモリを作成
      mpAppHeap = GFL_NEW(devHeap) app::util::Heap();
      mpAppHeapUpper = GFL_NEW(devHeap) app::util::Heap();
      mpAppParam->mpAppHeap = mpAppHeap;
      mpAppParam->mpAppHeapUpper = mpAppHeapUpper;
      // ローカルヒープを作る
      mpAppHeap->LocalHeapCreate(devHeap, devHeap,  0x230000,  0x880000);
      // sys 0.4Mあき
      //  下画面デバイスのこりAllocatableSize:0x0030bcc8  通信時AllocatableSize:0x002b3e00

      
      mpAppHeapUpper->LocalHeapCreate(appHeap, devHeap,  0x100000,  0x1700000);
      ///   dev 8Mあき sys 0.5Mあき
      // ファイル読み込み用はローカルではなくAPP_DEVICEの後方からとる   2015.05.20修正  0x25a0000
      
      mpAppHeap->SetFileReadHeap( devHeap );
      mpAppHeapUpper->SetFileReadHeap( devHeap );
      //描画に必要なメモリー
      m_pLocalDeviceHeap = GFL_CREATE_LOCAL_HEAP(devHeap, 0x190000, gfl2::heap::HEAP_TYPE_EXP, false );

      m_pLocalDeviceHeapAllocator = GFL_NEW( m_pLocalDeviceHeap ) System::nijiAllocator( m_pLocalDeviceHeap, System::nijiAllocator::APPLICATION );

      m_pNijiAllocator = GFL_NEW( mpAppHeap->GetDeviceHeap() ) System::nijiAllocator( mpAppHeap->GetDeviceHeap() );


   //   devHeap->Dump();
      //----------------------------------------------------------------------------allocater
      
      mpAppParam->pZukanData = pGameManager->GetGameData()->GetZukanData();
      mpAsyncFileManager = mpAppParam->mpAsyncFileManager;
      mpUpFrameManager = GFL_NEW(mpUpManagerHeap) applib::frame::Manager(mpUpManagerHeap);
      mpLowFrameManager = GFL_NEW(mpLowManagerHeap) applib::frame::Manager(mpLowManagerHeap);

      
      CreateMainPipeline();///blockがあつまっている
      {

        if(mpAppParam->pP2PTradeRequestClient){//通信の場合
          mpNetworkResponceListener = GFL_NEW(devHeap) App::Box::Proc::NetworkResponce();
          mpNetworkResponceListener->SetThis(this);
          mpAppParam->pP2PTradeRequestClient->SetP2PTradeResponseListener(mpNetworkResponceListener);

        }
        mpBoxAppSetup->CreateMenuWindowResource(mpAsyncFileManager, mpAppHeap);
      }
     
#if PM_DEBUG
      // デバッグメニュー作成
      DebugMenu_Init( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ) );
#endif
      
    }
    mSeq++;
    break;
  case LOCAL_ICON: ///読み込み開始をここに記述
    if(!mpBoxAppSetup->IsMenuWindowResourceLoaded(mpAsyncFileManager, mpAppHeap)){
      break;
    }
    if(!CreatePokeIcon()){
      break;
    }
    mSeq++;
//    break;  //through

  case LOCAL_OPEN_START: ///読み込み開始をここに記述
    {
      mpBoxAppSetup->BinLinkLoad(mpAppHeap);

      mpBoxSimpleAccesser = GFL_NEW(mpAppHeap->GetDeviceHeap()) BoxSimpleAccesser();
      mpBoxSimpleAccesser->Init(mpAppParam,mpBoxAppSetup,mpBoxSearch);
      mpBoxAppSetup->SetBoxSimpleAccesser(mpBoxSimpleAccesser);

      mSeq = LOCAL_READ_START;
    }
    break;
  case LOCAL_READ_START:
    mpBoxSimpleAccesser->SetupBox();
    mpBoxSimpleAccesser->SetupTemochi();
    mpBoxSimpleAccesser->SetupBattleTeam();
    mSeq = LOCAL_READ_WAIT1;
    break;
  case LOCAL_READ_WAIT1:

    if(!mpBoxAppSetup->IsBinLinkLoad()){
      break;
    }
    mSeq = LOCAL_READ_WAIT2;

  case LOCAL_READ_WAIT2:
    if(mpAppParam->pP2PTradeRequestClient){//通信の場合
      mpBoxAppSetup->ProcResourceRead( GARC_boxLyt_chtlow_applyt_COMP,  mpAppHeap, mpAsyncFileManager );
    }
    mSeq = LOCAL_READ_WAIT3;

    break;
  case LOCAL_READ_WAIT3:
    if(mpAppParam->pP2PTradeRequestClient){//通信の場合
      if( !mpBoxAppSetup->IsResourceLoaded( GARC_boxLyt_chtlow_applyt_COMP)){
        break;
      }
    }
    mSeq = LOCAL_READ_WAIT4;

  case LOCAL_READ_WAIT4:
    mSeq = LOCAL_READ_WAIT5;

  case LOCAL_READ_WAIT5:
    if(mpAppParam->pP2PTradeRequestClient){ //通信の場合

      mpBoxAppSetup->ProcResourceRead( GARC_boxLyt_trdupp_applyt_COMP,  mpAppHeap, mpAsyncFileManager );
    }
    mSeq = LOCAL_READ_WAIT6;
    
  case LOCAL_READ_WAIT6:
    if(mpAppParam->pP2PTradeRequestClient){ //通信の場合
      if( !mpBoxAppSetup->IsResourceLoaded( GARC_boxLyt_trdupp_applyt_COMP)){
        break;
      }
    }
    if(mpAppParam->pP2PTradeRequestClient){ //通信の場合
      //チャットウインド作成
      mpAppParam->pBoxUppChatDraw = GFL_NEW(mpAppHeapUpper->GetDeviceHeap()) BoxUppChatDraw(  mpBoxAppSetup->GetArcReadBuff(GARC_boxLyt_trdupp_applyt_COMP), mpAppRenderingManager,
                                                                                              mpAppHeapUpper, mpAppParam->pMsgData );
    }
    mSeq = LOCAL_READ_WAIT7;
    break;
  case LOCAL_READ_WAIT7:
    mSeq = LOCAL_READ_WAIT;
  case LOCAL_READ_WAIT:

    mSeq=LOCAL_CLOSE_WAIT;
  case LOCAL_CLOSE_WAIT:

    mSeq=LOCAL_CHAT_WAIT;

  case LOCAL_CHAT_WAIT:
    if(mpAppParam->pP2PTradeRequestClient){ //通信の場合
      if( !mpAppParam->pBoxUppChatDraw->InitFunc()){
        return gfl2::proc::RES_CONTINUE;
      }
      CreateTrainerPicture( 0 );   //たいして時間はかかっていない
      CreateTrainerPicture( 1 );
    }
    mSeq = FRAME_INIT_INIT;
    break;

  case FRAME_INIT_INIT:
    mpBoxLowBgFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxLowBgFrame >( mpLowFrameManager );
    mpBoxLowBgFrame->Setup(mpAppParam, mpBoxAppSetup);

    //ステータスを読み込む
    mpBoxUppPokeStatusFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, BoxUppPokeStatusFrame >( mpUpFrameManager );
    mpBoxUppPokeStatusFrame->Setup(mpAppParam, mpBoxAppSetup);

    mpBoxLowBgFrame->SetBoxUppStatusListener(mpBoxUppPokeStatusFrame->GetBoxUppStatusListener());

    mSeq = FRAME_INIT_WAIT;

    break;

  case FRAME_INIT_WAIT:
    int upst = mpUpFrameManager->Main();
    int lowst = mpLowFrameManager->Main();
    
    if(mpLowFrameManager->GetBaseProcessInitialized()){
			if(mpUpFrameManager->GetBaseProcessInitialized()){
        mSeq=0;
        return gfl2::proc::RES_FINISH;
			}
		}
    mpAppRenderingManager->Update();
    break;
  }

  if( mpBoxAppSetup ){
    mpBoxAppSetup->Update(mpAsyncFileManager);
  }
  
  return gfl2::proc::RES_CONTINUE;
}




void Proc::CreateMainPipeline(void)
{
      //自分のパイプラインを作成
  mpAppRenderingManager  = GFL_NEW(mpAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();
  mpAppParam->mpAppRenderingManager = mpAppRenderingManager;

  m_pNijiAllocatorForEdgeMapTexture = GFL_NEW( mpAppHeap->GetDeviceHeap() ) System::nijiAllocator( mpAppHeap->GetDeviceHeap() );

  app::util::AppRenderingManager::Description desc;
      {
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1_POKEMODEL2;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
	      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].pokeModelNumMax = 1;
				desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].charaModelNumMax=0;
				desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].dressUpModelNumMax=2;
				desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].modelNumMax=0;

			}
  
 // mpAppRenderingManager->StartAsyncCreate( desc, mpAppHeap, m_pNijiAllocatorForEdgeMapTexture, mpAsyncFileManager );
  mpAppRenderingManager->SyncCreate( desc, mpAppHeap, m_pNijiAllocatorForEdgeMapTexture, mpAsyncFileManager );
  
  mpAppParam->pMsgData = GFL_NEW(mpAppHeap->GetDeviceHeap())
    gfl2::str::MsgData( print::GetMessageArcId(),
                        GARC_message_box_DAT, mpAppHeap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );

  //  mpAppParam->outJumpMode = BOXJUMP_CANCEL;
  
  print::SystemFont_CreateNumFont( mpAppHeap->GetDeviceHeap(), mpAppHeap->GetDeviceHeap() );
  print::SystemFont_SetLytSystemNumFont();
}



void Proc::CreateTrainerPicture(int index)
{
  app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData;
  GameSys::GameManager*  pGameManager  = GameProc::GetGameManager();
  Savedata::MyStatus* pStatus=NULL;

#if defined(GF_PLATFORM_CTR)
  if(index==1){
    if(mpAppParam->pP2PTradeRequestClient){ //通信の場合
      NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager); 
      pStatus = pP2pConnectionManager->GetMyStatus( 1 - pP2pConnectionManager->GetNetID() );
    }
  }
#endif
  if(pStatus==NULL && index==1){
    return;
  }
  if(pStatus==NULL){
    pStatus = pGameManager->GetGameData()->GetPlayerStatus();
  }
  iconData.ConvFromMyStatus( *pStatus );

  mpAppParam->pBoxUppChatDraw->SetTrainerParam( (App::Box::BoxUppChatDraw::PlayerType)index, iconData);

}



enum {
  STATUS_INIT,
  STATUS_FADE,
  STATUS_FADE_END,
	STATUS_LISTENER,
  STATUS_LOOP,
  CLOSE_FUNCTION,
  CLOSE_END,
};

//=============================================================================
//メモリ取得
//=============================================================================
bool Proc::CreatePokeIcon(void)
{
	
  switch( m_CurSeq )
  {
  case 0:
    {
      GFL_ASSERT(mpAppParam->pPokeIcon==NULL);
      {
        int num = ( Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS ) * 3;
        mpAppParam->pPokeIcon = GFL_NEW(mpAppHeap->GetDeviceHeap()) app::tool::PokeIcon( mpAppHeap->GetDeviceHeap(), mpAppHeap->GetDeviceHeap(), num );
        for( int i=0; i<num; i++ )
        {
          mpAppParam->pPokeIcon->AllocTextureBuffer( i );
        }
      }
      GFL_ASSERT(mpAppParam->pItemIcon==NULL);
      {
        int num = Savedata::BoxPokemon::TRAY_POKE_MAX+ pml::PokeParty::MAX_MEMBERS;
        mpAppParam->pItemIcon = GFL_NEW( mpAppHeap->GetDeviceHeap() ) app::tool::ItemIcon( mpAppHeap->GetDeviceHeap(), mpAppHeap->GetDeviceHeap(), num );
        for( int i=0; i<num; i++ )
        {
          mpAppParam->pItemIcon->AllocTextureBuffer( i );
        }
      }
      mpAppParam->pPokeIcon->FileOpen( mpAppHeap->GetDeviceHeap() );
      mpAppParam->pItemIcon->FileOpen( mpAppHeap->GetDeviceHeap() );
      m_CurSeq++;
    }
    /* FALL THROUGH */
  case 1:
    if( mpAppParam->pPokeIcon->IsFileOpen() == false )
    {
      break;
    }
    if( mpAppParam->pItemIcon->IsFileOpen() == false )
    {
      break;
    }
    m_CurSeq=0;
    return true;
  default:
    return true;
  }
  return false;
}


bool Proc::DeletePokeIcon(void)
{
  if( mpAppParam->pPokeIcon == NULL )
  {
    return true;
  }
  if( mpAppParam->pPokeIcon->IsModuleFree() == false )
  {
    return false;
  }

  switch( m_CurSeq )
  {
  case 0:
  case 1:
  case 2:
    mpAppParam->pPokeIcon->FileClose(mpAppHeap->GetDeviceHeap());
    m_CurSeq =3;
    /* FALL THROUGH */
  case 3:
    if( mpAppParam->pPokeIcon->IsFileClose() == false )
    {
      break;
    }
    GFL_SAFE_DELETE( mpAppParam->pPokeIcon );
    m_CurSeq = 0;
    return true;
    
  default:
    break;
  }

  return false;
}



bool Proc::DeleteItemIcon(void)
{
  if( mpAppParam->pItemIcon == NULL )
  {
    return true;
  }
  if( mpAppParam->pItemIcon->IsModuleFree() == false )
  {
    return false;
  }

  switch( m_CurSeq )
  {
  case 0:
  case 1:
  case 2:
    mpAppParam->pItemIcon->FileClose(mpAppHeap->GetDeviceHeap());
    m_CurSeq =3;
    /* FALL THROUGH */
  case 3:
    if( mpAppParam->pItemIcon->IsFileClose() == false )
    {
      break;
    }
    GFL_SAFE_DELETE( mpAppParam->pItemIcon );
    m_CurSeq = 0;
    return true;
    
  default:
    break;
  }

  return false;
}






//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::UpdateFunc(gfl2::proc::Manager* pManager)
{
#if PM_DEBUG
  // デバッグメニュー更新(box_debug.cppに定義）
  DebugMenu_Update();
#endif

  if( mpBoxAppSetup ){
    mpBoxAppSetup->Update(mpAsyncFileManager);
  }
  
  {
    int upst = mpUpFrameManager->Main();
    int lowst = mpLowFrameManager->Main();
    
    if((upst==applib::frame::MAINSTATUS_NULL)&& (lowst==applib::frame::MAINSTATUS_NULL)){
      return gfl2::proc::RES_FINISH;
    }
  }
  mpAppRenderingManager->Update();
  if(!mpAppRenderingManager->IsCreated()){
    return gfl2::proc::RES_CONTINUE;
  }

  switch(mSeq){
  case STATUS_INIT:
    mpBoxLowBgFrame->drawPokeIconAll();
    mSeq++;
  case STATUS_FADE:
    if(mpBoxLowBgFrame->IsPokeIconInit() || mpAppParam->bAllBoxMode){
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
      mSeq++;
    }
    break;
  case STATUS_FADE_END:
    if(GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_DOUBLE)){
      mpBoxLowBgFrame->SetFadeEnd();
      mSeq++;
    }
    break;
  case STATUS_LISTENER:
    mpBoxLowBgFrame->SetListener();
#if defined(GF_PLATFORM_CTR)
    OHNO_PRINT("Proc::UpdateFunc END %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif
    mSeq++;
    
//    break;
  case STATUS_LOOP:

   /* if(mpBoxLowBgFrame->IsDispChat()){  //チャット送信
      mpBoxUppPokeStatusFrame->DispSelfMessage();
      mpBoxLowBgFrame->ResetDispChat();
    }*/
    if(mpBoxLowBgFrame->IsEndReq()){     //終了
      mpBoxLowBgFrame->SetBoxUppStatusListener(NULL);
      mpBoxUppPokeStatusFrame->CloseWindow();  //上画面に終了を伝える
      mpBoxLowBgFrame->CloseWindow();  //下も消す
      //消す
      if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEndStatus(gfl2::Fade::DISP_DOUBLE)  ){
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );
      }
      mpBoxUppPokeStatusFrameFinish=mpBoxUppPokeStatusFrame;
      mpBoxUppPokeStatusFrame=NULL;
      mSeq++;
    }
/*    if(mpBoxUppPokeStatusFrame){
      mpBoxUppPokeStatusFrame->SetPokemonFolderDisp(mpBoxLowBgFrame->GetChangeFolderPoke());
    }*/
    break;
  case CLOSE_FUNCTION:
    if(mpBoxUppPokeStatusFrameFinish != mpUpFrameManager->GetBaseProcess() ){
      mpBoxUppPokeStatusFrameFinish=NULL;
//      mpBoxLowBgFrame->CloseWindow();
      mSeq=CLOSE_END;
    }
    break;
  }

  if(mSeq>=STATUS_FADE && mSeq<=STATUS_LOOP){
    if(mpBoxUppPokeStatusFrame){
      mpBoxUppPokeStatusFrame->SetPokemonFolderDisp(mpBoxLowBgFrame->GetChangeFolderPoke());
    }
  }


  
  return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    PreDrawFunc
 * @brief   Proc描画下準備
 * @date    2015.07.09
 *
 * @param   pManager    プロセス管理マネージャー
 */
//-----------------------------------------------------------------------------
void Proc::PreDrawFunc( gfl2::proc::Manager * pManager )
{
  mpAppRenderingManager->PreDraw();
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。 順番を変更しました
 */
//------------------------------------------------------------------
void Proc::DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo no)
{
  mpUpFrameManager->Draw(no);
  mpLowFrameManager->Draw(no);
  mpAppRenderingManager->Draw(no);
}


//------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 *  1/30フレームの②フレーム目の処理です。
 *  1/60フレーム動作時には、使われません。
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  return gfl2::proc::RES_CONTINUE;
}


enum{
  _END_LOCAL_CLOSE,
  _END_LOCAL_CLOSE_WAIT,
  _END_LOCAL_MEMORY_DEL,
};

//------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::EndFunc(gfl2::proc::Manager* pManager)
{

  switch(mEndSeq){
  case _END_LOCAL_CLOSE:
    if(mpAppParam->pP2PTradeRequestClient)
    {
      mpAppParam->pP2PTradeRequestClient->SetP2PTradeResponseListener(NULL);//GFNMCat5697
    }


    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE );  //GFNMCat[4997]

    
/*    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
      creq.arcId       = ARCID_BOX;
      creq.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。
      mpAsyncFileManager->AddArcFileCloseReq(creq);
    }*/
    
    mEndSeq = _END_LOCAL_CLOSE_WAIT;
    break;
  case _END_LOCAL_CLOSE_WAIT:
  ////  if(!mpAsyncFileManager->IsArcFileCloseFinished(ARCID_BOX) ){
   //   break;
//    }
    mEndSeq = _END_LOCAL_MEMORY_DEL;

  case _END_LOCAL_MEMORY_DEL:

    if(mpAppParam->pBoxUppChatDraw){
      if( !mpAppParam->pBoxUppChatDraw->EndFunc() )
      {
        return gfl2::proc::RES_CONTINUE;
      }
    }
    GFL_SAFE_DELETE(mpAppParam->pBoxUppChatDraw);
    if(false == DeletePokeIcon()){
      return gfl2::proc::RES_CONTINUE;
    }
    if(false == DeleteItemIcon()){
      return gfl2::proc::RES_CONTINUE;
    }
    // 終了処理が完了するまで待つ
    if( mpUpFrameManager->End() == true )
    {
      return gfl2::proc::RES_CONTINUE;
    }
    if( mpLowFrameManager->End() == true )
    {
      return gfl2::proc::RES_CONTINUE;
    }
    if( !mpAppRenderingManager->CanDestroy() )
    {
      return gfl2::proc::RES_CONTINUE;
    }

    
    if(mpAppParam->pP2PTradeRequestClient)
    {
//      mpAppParam->pP2PTradeRequestClient->SetP2PTradeResponseListener(NULL);
      GFL_SAFE_DELETE( mpNetworkResponceListener );
    }
    mpBoxAppSetup->DeleteMenuWindowResource();
    mpBoxAppSetup->ProcResourceFree();

    mpBoxAppSetup->BinLinkClose(mpAppHeap);

    print::SystemFont_ResetDefaultFontInfo();
    print::SystemFont_DeleteNumFont();
    
    GFL_SAFE_DELETE( mpAppParam->pMsgData );
    GFL_SAFE_DELETE( mpUpFrameManager );
    GFL_SAFE_DELETE( mpLowFrameManager );
    GFL_SAFE_DELETE( mpAppRenderingManager );
    
    mpBoxAppSetup->SetBoxSimpleAccesser(NULL);
    mpBoxSimpleAccesser->End();
    GFL_SAFE_DELETE( mpBoxSimpleAccesser );
    
    GFL_SAFE_DELETE(m_pNijiAllocator);
    //  gfl2::gfx::GLMemory::SetAllocator(mpOldAllocator);  //ふっきさせる
    GFL_SAFE_DELETE(m_pLocalDeviceHeapAllocator);
    gfl2::heap::Manager::DeleteHeap(m_pLocalDeviceHeap);
    
    GFL_SAFE_DELETE( m_pNijiAllocatorForEdgeMapTexture );
    GFL_DELETE mpAppHeapUpper;
    GFL_DELETE mpAppHeap;
    
    gfl2::heap::Manager::DeleteHeap( mpUpManagerHeap );
    gfl2::heap::Manager::DeleteHeap( mpLowManagerHeap );
    
#if PM_DEBUG
  // デバッグメニュー破棄(box_debug.cppに定義）
  DebugMenu_Exit();
#endif

    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
} 

//------------------------------------------------------------------
/**
 *  @brief    メッセージIDを受信した時に呼ばれるハンドラです
 *  @param    eMessageID 受信したメッセージID 
 *  @param    monsno 欲しいポケモンの種族番号
 *  @note     関連関数 AddSendMessageIDRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnMessageIDReceived( NetAppLib::P2PTrade::MessageID eMessageID, MonsNo monsno, void* pPokemonParamBuffer )
{
	OHNO_PRINT("Message受信 %d %d\n",eMessageID ,monsno );
  mpThis->mpAppParam->chatMsgOther = static_cast<u8>(eMessageID);
  mpThis->mpAppParam->chatPokemonOther = monsno;
  mpThis->mpAppParam->pWantPokemonParam->Deserialize_Full( pPokemonParamBuffer );

  if(mpThis->mpBoxUppPokeStatusFrame){
    mpThis->mpBoxUppPokeStatusFrame->DispOtherMessage( );
  }
}



//------------------------------------------------------------------
/**
 *  @brief    ポケモンのパラメータを受信した時に呼ばれるハンドラです
 *  @param    pokemonParam 受信したポケモンのパラメータ
 *  @note     関連関数 AddSendPokemonParamRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnPokemonParamReceived( const pml::pokepara::PokemonParam& pokemonParam )
{
	OHNO_PRINT("ポケモン受信 %d\n", pokemonParam.GetMonsNo() );

  if(mpThis->mpBoxUppPokeStatusFrame){
    mpThis->mpBoxUppPokeStatusFrame->SetOtherPokemon( pokemonParam );
  }
}
//------------------------------------------------------------------
/**
 *  @brief    最初の交換が成立した時に呼ばれるハンドラです
 *  @param    pokemonParam 交換相手のポケモンのパラメータ
 *  @note     関連関数 AddFirstTradeRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnFirstTradeSuccess( const pml::pokepara::PokemonParam& pokemonParam )
{
  OHNO_PRINT("OnFirstTradeSuccess つぎにすすむ \n" );
  if(mpThis->mpBoxLowBgFrame){
    mpThis->mpBoxLowBgFrame->OnFirstTradeSuccess(pokemonParam);   //A5
  }
}

//------------------------------------------------------------------
/**
 *  @brief    交換相手のポケモンが不正だった時に呼ばれるハンドラです  表示はしないでもどるだけ
 *  @param    bMyPokemonInvalid [ true : 自分のポケモンが不正 / false : 相手のポケモンが不正 ]
 *  @note     関連関数 AddFirstTradeRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnFirstTradePokemonInvalid(bool bMyPokemonInvalid)  //GFNMCat4378
{
  if(mpThis->mpBoxLowBgFrame){
    mpThis->mpBoxLowBgFrame->OnFirstTradePokemonInvalid(bMyPokemonInvalid);
  }
}

//------------------------------------------------------------------
/**
 *  @brief    不正チェックサーバーへ接続出来なかった時などに呼ばれるハンドラです
 *  @param    bMyValidationError [ true : 自分の通信エラー / false : 相手の通信エラー ]
 *  @note     関連関数 AddFirstTradeRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnFirstTradeValidationError(bool bMyValidationError)  //GFNMCat4378
{
  //あいてが交換に出せないポケモンを選びました
  if(mpThis->mpBoxLowBgFrame){
    mpThis->mpBoxLowBgFrame->OnFirstTradeValidationError(bMyValidationError);
  }
}

//------------------------------------------------------------------
/**
 *  @brief    最初の交換リクエストがキャンセルされた時に呼ばれるハンドラです
 *  @note     関連関数 CancelFirstTradeRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnFirstTradeCancel(void)
{
  //こうかんをちゅうししました。
  if(mpThis->mpBoxLowBgFrame){
    mpThis->mpBoxLowBgFrame->OnFinalTradeCancel();
  }
}

//------------------------------------------------------------------
/**
 *  @brief    最後の交換が成立した時に呼ばれるハンドラです
 *  @note     関連関数 AddFinalTradeRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnFinalTradeSuccess(void)
{

  OHNO_PRINT("成立しました\n");

  if(mpThis->mpBoxLowBgFrame){
    mpThis->mpBoxLowBgFrame->OnTradeSuccess();
  }
  
}
//------------------------------------------------------------------
/**
 *  @brief    最後の交換が完了した時に呼ばれるハンドラです
 *  @note     関連関数 AddFinalTradeRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnFinalTradeComplete(void)
{

  OHNO_PRINT("セーブも完了\n");
  if(mpThis->mpBoxLowBgFrame){
    mpThis->mpBoxLowBgFrame->OnTradeComplete();
  }
}


//------------------------------------------------------------------
/**
 *  @brief    最後の交換リクエストがキャンセルされた時に呼ばれるハンドラです
 *  @note     関連関数 CancelFinalTradeRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnFinalTradeCancel(void)
{
  //こうかんをちゅうししました。
  if(mpThis->mpBoxLowBgFrame){
    mpThis->mpBoxLowBgFrame->OnFinalTradeCancel();
  }
}
//------------------------------------------------------------------
/**
 *  @brief    交換キャンセルを受信した時に呼ばれるハンドラです
 *  @note     関連関数 AddSendTradeCancelRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnTradeCancelReceived(void)
{
  //あいてがこうかんをちゅうししました。
  if(mpThis->mpBoxLowBgFrame){
    mpThis->mpBoxLowBgFrame->OnTradeCancelReceived();
  }
}


//------------------------------------------------------------------
/**
 *  @brief    交換終了を受信した時に呼ばれるハンドラです
 *  @note     関連関数 AddTradeExitRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnTradeExitReceived(void)
{
  GFL_ASSERT(mpThis->mpAppParam);
  mpThis->mpAppParam->mbNetEndRequest=true;
  mpThis->mpAppParam->pP2PTradeRequestClient->SetP2PTradeResponseListener(NULL);//GFNMCat5760
}


//------------------------------------------------------------------
/**
 *  @brief    交換終了が成立した時に呼ばれるハンドラです
 *  @note     関連関数 AddTradeExitRequest
 */
//------------------------------------------------------------------
void Proc::NetworkResponce::OnTradeExitSuccess(void)
{
  if(mpThis->mpBoxLowBgFrame){
    mpThis->mpBoxLowBgFrame->OnTradeExitSuccess();
  }
  mpThis->mpAppParam->pP2PTradeRequestClient->SetP2PTradeResponseListener(NULL);//GFNMCat5760
}





GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


