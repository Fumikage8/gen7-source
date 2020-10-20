/**
* @brief  テスト用プロック
* @author NP Yam
* @data   2015.04.16
*/
#if PM_DEBUG

#include <System/include/HeapDefine.h>
#include "../include/PeteTestProc.h"
#include <GameSys/include/GameProcManager.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>

#include <system/include/Timer/gfl2_PerformanceCounter.h>

// CharaModel:
#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

// SceneGraph
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

// garc FileLoading
#include <arc_index/chara_model_field.gaix>
#include <arc_def_index/arc_def.h>

#include <arc_index/demo_seq_script.gaix>
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_demoviewer.h>

// Fade
#include <Fade/include/gfl2_FadeManager.h>

// Sound:
#include "Sound/include/Sound.h"
#include <niji_conv_header/sound/SoundMiddleID.h>

// Pokemon:
#include <niji_conv_header/poke_lib/monsno_def.h>

#include <GameSys\include\GameManager.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

GFL_NAMESPACE_BEGIN(Test)
	GFL_NAMESPACE_BEGIN(Pete)

enum
{
	PT_STATE_INIT_HEAP= 0,
	PT_STATE_INIT_FONT,
	PT_STATE_INIT_SEQ,
	PT_STATE_RUN_SELECTSEQ,
  PT_STATE_RUN_SETTINGSMENU,
  PT_STATE_RUN_LOADSEQ,
  PT_STATE_RUN_LOADSEQHIO,
	PT_STATE_RUN_PLAYSEQ,
	PT_STATE_RUN_MAIN,
	PT_STATE_RUN_WAIT,
  PT_STATE_RUN_WAITUNLOAD,
	PT_STATE_RUN_FINISH,
};

PeteTestProc::PeteTestProc( void ) : 
  mStateReg(PT_STATE_INIT_HEAP),
  mStateRegSub(0),
  mSequenceID( 1 ),
  mHeap(NULL),
	mRenderingPipeline( NULL ),
	mRenderingPipelineText( NULL ),
	mFontResource( NULL )	,
  mPlayerGender(0),
  mCurrentPokemon(0),
  mTimezone(0)
{

  mDrawEnvNode = NULL;
  
	for(int n=0;n<MAX_DEBUG_POKEMON_SETTINGS;n++)
  {
    mPokemonSettings[n].monsNo = 25;
    mPokemonSettings[n].formNo = 0;
    mPokemonSettings[n].sex = 0;
    mPokemonSettings[n].enable = true;
  }

  mDemoType = -1;
  mVSDemoType = 0;

  mGLAllocator = NULL;

  mUseHIO = false;
  mDebugInfo = false;
  mFrameLogPos = 0;
  mSkipFrames = 0;
  mLastFrame = 0;

  // 連続再生用変数初期化
  mContinuePlaySeq = 0;
  mIsContinuePlay = false;
  mFirstContinuePlaySeqID = mSequenceID;
  mEndContinuePlaySeqID = mSequenceID;
}
	
PeteTestProc::~PeteTestProc( void )
{
}

/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
*/
gfl2::proc::Result PeteTestProc::InitFunc( gfl2::proc::Manager* pManager )
{
	gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

	GFL_PRINT("PeteTestProc->Init(%i)\n",mStateReg);

	switch(mStateReg)
	{
		case PT_STATE_INIT_HEAP:			      
			StateInit_Heap();						
			SetState(PT_STATE_INIT_FONT);     
			break;
		
		case PT_STATE_INIT_FONT:      
			// font
			{
				gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
				gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
		
				mMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
				mDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
        mGLAllocator = GFL_NEW(pMemHeap) System::nijiAllocator(pDevHeap);

        mFrameLogF = GFL_NEW( pMemHeap ) unsigned short[SEQ_MAX_FRAME];
        mFrameLogC = GFL_NEW( pMemHeap ) unsigned short[SEQ_MAX_FRAME];
        mFrameLogG = GFL_NEW( pMemHeap ) unsigned short[SEQ_MAX_FRAME];

        StateInit_RenderPipeline();		

				mRenderingPipelineText = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine(mGLAllocator, 512, 256 );
				mFontResource = gfl2::str::CreateResFont( "data/app/test/fukushima_test/layout_sample_data/fonts/cbf_std_kujira.bffnt", pMemHeap, pDevHeap, pMemHeap );
				mDrawUtilText = GFL_NEW( pMemHeap ) gfl2::util::DrawUtilText(mMemAllocator, mDevAllocator);
				mDrawUtilText->Initialize( mMemAllocator, mDevAllocator, mRenderingPipelineText->GetUtilTextRenderPath() );
				mDrawUtilText->SetFont( mFontResource );
			}
			StateInit_SeqPlayer();
      CreateDebugMenu();
			SetState(PT_STATE_INIT_SEQ);
			break;

    case PT_STATE_INIT_SEQ:
      result = gfl2::proc::RES_FINISH;
      break;

	
		default:
			result = gfl2::proc::RES_CONTINUE;
			break;
	}
		
	

	return result;
}

/**
* @brief	  StateInit_Heap, initiliaze & setup Heap
* @return	Result - None
*/
void PeteTestProc::StateInit_Heap(void)
{	
	gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);

  devHeap->Dump();

	mHeap = GFL_CREATE_LOCAL_HEAP_NAME(devHeap,0x200000,gfl2::heap::HEAP_TYPE_EXP,false,"PeteTestProc");
	GFL_ASSERT( mHeap != NULL );

}



void PeteTestProc :: StateInit_SeqPlayer(void)
{	
	gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
	gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
	
	mSequenceViewSys = GFL_NEW(heap)DemoLib::Sequence::SequenceViewSystem(true);
#if PM_DEBUG
  if(mUseHIO)
  {
    mSequenceViewSys->SetUseHIO(true);
  }    
#endif
	mSequenceViewSys->Initialize( heap,devHeap,0x300000,0x1D00000 );

}

/**
* @brief	  StateInit_RenderPipeline, initialize & setup Render Pipeline
* @return	Result - None
*/
void PeteTestProc::StateInit_RenderPipeline(void)
{		
	mRenderingPipeline = GFL_NEW(mHeap)gfl2::renderingengine::renderer::DefaultRenderingPipeLine(mGLAllocator);	
	mDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(mGLAllocator);
	
	gfl2::math::AABB envSpace(gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f));
	mDrawEnvNode->SetAABB(envSpace);

	gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild(mDrawEnvNode);
	mRenderingPipeline->AddDrawEnv(mDrawEnvNode);

}

void PeteTestProc::PrintFrameLog(void)
{	
  if(mFrameLogPos<=0) return;

  GFL_RELEASE_PRINT("\n\n****DEMO PERFORMANCE LOG****\n\n");

  for(int z=0;z<mFrameLogPos;z++)
  {
    GFL_RELEASE_PRINT("FRAME: %i\tGPU: %i (%i%%)\tCPU: %i (%i%%)\n",mFrameLogF[z],mFrameLogG[z],mFrameLogG[z]/320,mFrameLogC[z],mFrameLogC[z]/320);
  }
  
  GFL_RELEASE_PRINT("\nTotal Skipped Frames: %i / %i (%i%%)",mSkipFrames,mTotalFrames,  (int)(mSkipFrames*100.0f/mTotalFrames)  );

  GFL_RELEASE_PRINT("\n\n****END PERFORMANCE LOG****\n\n");

  mFrameLogPos = 0;
}

/**
* @brief	  PROCのメイン更新を行う関数
* @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result PeteTestProc::UpdateFunc( gfl2::proc::Manager* pManager )
{	
	

	gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
	gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

	switch(mStateReg)
	{
		case PT_STATE_INIT_SEQ:
			Sound::ChangeAppBGMReqForce(STRM_BGM_T01);
			SetState(PT_STATE_RUN_SELECTSEQ);
		break;

    case PT_STATE_RUN_WAITUNLOAD:
      {
        if(mSequenceViewSys->Finalize())
        {
          GFL_SAFE_DELETE(mSequenceViewSys);	
          if(mUseHIO) mUseHIO = false;
          else mUseHIO = true;

          StateInit_SeqPlayer();
          SetState(PT_STATE_RUN_SELECTSEQ);
          break;
        }               
      }
      break;

		case PT_STATE_RUN_SELECTSEQ:

      // Set params:
      {
        // Apply Settings:
        if(mPlayerGender==0) mSequenceViewSys->SetPlayerSexMale();
        else mSequenceViewSys->SetPlayerSexFemale();

        if(mTimezone==0) mSequenceViewSys->SetTimezone_Realtime();
        else if(mTimezone==1) mSequenceViewSys->SetTimezone_Morning();
        else if(mTimezone==2) mSequenceViewSys->SetTimezone_Noon();
        else if(mTimezone==3) mSequenceViewSys->SetTimezone_Evening();
        else if(mTimezone==4) mSequenceViewSys->SetTimezone_Night();

        // Pokemon Settings:
        for(int n=0;n<MAX_DEBUG_POKEMON_SETTINGS;n++)
        {
          mSequenceViewSys->SetDebugPokemonSetting(n,mPokemonSettings[n].monsNo,mPokemonSettings[n].formNo,mPokemonSettings[n].sex,true);
        }

        // Demo Type:
        if(mDemoType==-1) mSequenceViewSys->ResetAllDemoType();
        else mSequenceViewSys->SetDemoType(mDemoType);

        // VS Battle Type:
        mSequenceViewSys->SetPlaybackParameter(DemoLib::Sequence::SEQPARAMTYPE_VSBATTLE,mVSDemoType);
      }
            

			mDrawUtilText->ClearDrawText();
			mDrawUtilText->SetTextScale( 1.5f, 1.5f );
			mDrawUtilText->DrawText( 0,   0, L"DemoPlayer");			
			mDrawUtilText->SetTextScale( 1.0f, 1.0f );

      // Write SEQ Name?
      {
        wchar_t *seqName = mSequenceViewSys->SeqComFunc_GetSeqName(mSequenceID);
        if(seqName!=NULL) mDrawUtilText->DrawText( 0,   100, seqName);
        
     
        mDrawUtilText->DrawText( 0,   50, L"SequenceID: %i",mSequenceID);

        if(mUseHIO) mDrawUtilText->DrawText( 0,   130, L"*HIO ENABLE*");


        mDrawUtilText->SetTextScale( 0.6f, 0.6f );
#if defined(GF_PLATFORM_WIN32)
        mDrawUtilText->DrawText( 0,   220, L"Y:MENU   X:HIO   A:PLAY   START:HIOSEQ");
#else
        mDrawUtilText->DrawText( 0,   220, L"X:MENU   Y:HIO   A:PLAY   START:HIOSEQ");
#endif
        mDrawUtilText->SetTextScale( 1.0f, 1.0f );

			  if(mSequenceID >= GARC_demo_seq_script_DATA_NUM && seqName==NULL)
			  {
				  gfl2::math::Vector4 col;
				  col.Set(1.0f,0.0f,0.0f,1.0f);
				  mDrawUtilText->SetTextColor(col);
				  mDrawUtilText->DrawText( 0,   100, L"*WARNING*");
				  col.Set(1.0f,1.0f,1.0f,1.0f);
				  mDrawUtilText->SetTextColor(col);
			  }

        

			}

      if( gfl2::debug::DebugWin_IsOpen() )
      {
        break;
      }

      // 連続再生機能追加
      if( mIsContinuePlay )
      {
        enum
        {
          CP_PLAY_BEFORE = 0,
          CP_PLAY,
          CP_END,
        };

        switch ( mContinuePlaySeq )
        {
        case CP_PLAY_BEFORE:
          mSequenceID = mFirstContinuePlaySeqID;
          mContinuePlaySeq = CP_PLAY;
          break;

        case CP_PLAY:
          mSequenceID++;
          // 終了ＩＤを超えていない又はSequenceIDが存在する場合、再生を続ける
          if( mSequenceID <= mEndContinuePlaySeqID && mSequenceViewSys->SeqComFunc_GetSeqName( mSequenceID ) != NULL )
          {
            break;
          }
          // breakしない

        case CP_END:
          mSequenceID = mEndContinuePlaySeqID;
          mIsContinuePlay = false;
          mContinuePlaySeq = CP_PLAY_BEFORE;
          break;

        default:
          break;
        }
      }

			if( ( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) || pButton->IsTrigger( gfl2::ui::BUTTON_UP ) ) && ( !mIsContinuePlay ) )
			{				
        if( pButton->IsHold( gfl2::ui::BUTTON_R )) mSequenceID+=10;
				else mSequenceID++;
			}
			else if( ( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) || pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ) ) && (!mIsContinuePlay) )
			{
        if( pButton->IsHold( gfl2::ui::BUTTON_R )) 
        {
            if(mSequenceID>10) mSequenceID-=10;
            else mSequenceID = 1;
        }
				else if(mSequenceID>1) mSequenceID--;
			}

			else if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) || mIsContinuePlay )
			{
        mSequenceViewSys->ShowDebugInfo(mDebugInfo);
        mSequenceViewSys->SetSkipMilliSec(mSkipMilliSec);
				SetState(PT_STATE_RUN_LOADSEQ);
        mFrameLogPos = 0;
        mSkipFrames = 0;
        mLastFrame = 0;
        mTotalFrames = 0;
				// Fade to black:
				gfl2::math::Vector4 blackCol(0.0f,0.0f,0.0f,0.0f);					
				gfl2::math::Vector4 blackCol2(0.0f,0.0f,0.0f,255.0f);					
				gfl2::math::Vector4 *col = &blackCol;
				gfl2::math::Vector4 *col2 = &blackCol2;				
				GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,1);				
			}			

      else if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
      {
        mSequenceViewSys->ShowDebugInfo(mDebugInfo);
        mSequenceViewSys->SetSkipMilliSec(mSkipMilliSec);
        SetState(PT_STATE_RUN_LOADSEQHIO);
        mFrameLogPos = 0;
        mSkipFrames = 0;
        mLastFrame = 0;
        mTotalFrames = 0;
       
        // Fade to black:
        gfl2::math::Vector4 blackCol(0.0f,0.0f,0.0f,0.0f);					
        gfl2::math::Vector4 blackCol2(0.0f,0.0f,0.0f,255.0f);					
        gfl2::math::Vector4 *col = &blackCol;
        gfl2::math::Vector4 *col2 = &blackCol2;				
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,1);
        
      }			

			else if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
			{				
        mSequenceViewSys->ProcessToFinalize();
				SetState(PT_STATE_RUN_FINISH);
			}
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
      {		
        SetState(PT_STATE_RUN_SETTINGSMENU);
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
      {		
        mSequenceViewSys->ProcessToFinalize();
        SetState(PT_STATE_RUN_WAITUNLOAD);
        break;        
      }
			break;

		case PT_STATE_RUN_LOADSEQ:
			GFL_PRINT("Loading Seqence File\n");
      mSequenceViewSys->SeqComFunc_PlaySequenceFile(mSequenceID); 
			SetState(PT_STATE_RUN_PLAYSEQ);
			
			break;

    case PT_STATE_RUN_LOADSEQHIO:
      GFL_PRINT("Loading Seqence File\n");
      mSequenceViewSys->SeqComFunc_PlaySequenceFileDebug();
      SetState(PT_STATE_RUN_PLAYSEQ);

      break;

    case PT_STATE_RUN_SETTINGSMENU:
      mDrawUtilText->ClearDrawText();
      mDrawUtilText->SetTextScale( 1.5f, 1.5f );
      mDrawUtilText->DrawText( 0,   0, L"DemoSettings");			
      mDrawUtilText->SetTextScale( 0.5f, 0.5f );
      
      {

        

        int dy = 20;
        int dx = 30;
        int xPos = dx;
        int yPos = 40+20;
        u32 n = 0;
        
        // 0 SEX:MALE or FEMALE
        if(mStateRegSub==n) mDrawUtilText->DrawText( 0, yPos-1, L"->");
        mDrawUtilText->DrawText( xPos, yPos, L"PlayerSex: %ls", (mPlayerGender==0) ? L"MALE" : L"FEMALE");	yPos+=dy; n++;

        // 1 TIMEZONE: MORNING, NOON, EVENING, NIGHT
        {
          wchar_t *timeZoneStr[] = { L"REALTIME",L"MORNING", L"NOON",L"EVENING",L"NIGHT" };
          if(mStateRegSub==n) mDrawUtilText->DrawText( 0, yPos-1, L"->");          
          mDrawUtilText->DrawText( xPos, yPos, L"Timezone: %ls",timeZoneStr[mTimezone]);	yPos+=dy; n++;
        }

        // 2
        if(mStateRegSub==n) mDrawUtilText->DrawText( 0, yPos-1, L"->");
        mDrawUtilText->DrawText( xPos, yPos, L"PokemonIndex: %i",mCurrentPokemon);	yPos+=dy; n++;

        // 3 Pokemon Name:
        gfl2::str::StrBuf nickname( pml::MONS_NAME_LENGTH + pml::EOM_LENGTH, mHeap->GetLowerHandle() );
        pml::personal::GetMonsName( &nickname, (MonsNo)mPokemonSettings[mCurrentPokemon].monsNo );

        if(mStateRegSub==n) mDrawUtilText->DrawText( 0, yPos-1, L"->");
        mDrawUtilText->DrawText( xPos, yPos, L"   MonsNo: %i %ls",mPokemonSettings[mCurrentPokemon].monsNo,nickname.GetPtr());	yPos+=dy; n++;

        // 4 0-27
        if(mStateRegSub==n) mDrawUtilText->DrawText( 0, yPos-1, L"->");
        mDrawUtilText->DrawText( xPos, yPos, L"   FormNo: %i",mPokemonSettings[mCurrentPokemon].formNo);	yPos+=dy; n++;

        // 5 MALE/FEMALE
        if(mStateRegSub==n) mDrawUtilText->DrawText( 0, yPos-1, L"->");
        mDrawUtilText->DrawText( xPos, yPos, L"   Sex: %ls",( mPokemonSettings[mCurrentPokemon].sex==0) ? L"MALE" : L"FEMALE");	yPos+=dy; n++;

        // 6 DemoType
        if(mStateRegSub==n) mDrawUtilText->DrawText( 0, yPos-1, L"->");
        if(mDemoType==-1)
        {
          mDrawUtilText->DrawText( xPos, yPos, L"DemoType: NONE"); yPos+=dy; n++;
        }
        else
        {
          wchar_t demoTypeStr[] = { L"A" };
          demoTypeStr[0] += mDemoType;
          mDrawUtilText->DrawText( xPos, yPos, L"DemoType: %ls",demoTypeStr );	yPos+=dy; n++;
        }

        // 7 VS DemoType:
        if(mStateRegSub==n) mDrawUtilText->DrawText( 0, yPos-1, L"->");
        {
          wchar_t *vsTypeStr[] = { L"PRE_BATTLE", L"WIN",L"LOSE",L"DRAW" };
          mDrawUtilText->DrawText( xPos, yPos, L"VS Type: %ls",vsTypeStr[mVSDemoType] );	yPos+=dy; n++;
        }

        // 7 VS DemoType:
        if(mStateRegSub==n) mDrawUtilText->DrawText( 0, yPos-1, L"->");
        {
          wchar_t *debugInfoStr[] = { L"OFF", L"ON" };
          mDrawUtilText->DrawText( xPos, yPos, L"Show Debug Info: %ls",debugInfoStr[mDebugInfo] );	yPos+=dy; n++;
        }

        // Go up and down the list:
        if(pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ) && mStateRegSub<n-1)
        {
          mStateRegSub++;
        }
        else if(pButton->IsTrigger( gfl2::ui::BUTTON_DOWN )) // Wrap around
        {
          mStateRegSub=0;
        }
        else if(pButton->IsTrigger( gfl2::ui::BUTTON_UP ) && mStateRegSub>0)
        {
          mStateRegSub--;
        }
        else if(pButton->IsTrigger( gfl2::ui::BUTTON_UP )) // Wrap around
        {
          mStateRegSub = n-1;
        }

        switch(mStateRegSub)
        {
          case 0: // SEX:MALE,FEMALE
            {
              if(pButton->IsTrigger( gfl2::ui::BUTTON_LEFT )  || pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
              {
                mPlayerGender = 1 - mPlayerGender;
              }
            }
            break;

          case 1: // TIMEZONE
            {
              const int MAX_TIMEZONE = 5;
              if(pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
              {
                mTimezone++;
                if(mTimezone>=MAX_TIMEZONE) mTimezone = 0;
              }

              if(pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
              {
                if(mTimezone==0) mTimezone = MAX_TIMEZONE;
                mTimezone--;                
              }
            }
            break;

          case 2: // Current Pokemon
            {
              if(pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
              {
                mCurrentPokemon++;
                if(mCurrentPokemon>=MAX_DEBUG_POKEMON_SETTINGS) mCurrentPokemon = 0;
              }

              if(pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
              {
                if(mCurrentPokemon==0) mCurrentPokemon = MAX_DEBUG_POKEMON_SETTINGS;
                mCurrentPokemon--;                
              }
            }
            break;

          case 3: // Pokemon MonsNo:
            {
              const int MAX_MONSNO = MONSNO_MAX+1;
              if(pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
              {
                u32 off = 1;
                if(pButton->IsHold( gfl2::ui::BUTTON_R )) off = 10;
                else if(pButton->IsHold( gfl2::ui::BUTTON_L )) off = 100;
                mPokemonSettings[mCurrentPokemon].monsNo+=off;
                if(mPokemonSettings[mCurrentPokemon].monsNo>=MAX_MONSNO) mPokemonSettings[mCurrentPokemon].monsNo = 0;
              }

              if(pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
              {
                u32 off = 1;
                if(pButton->IsHold( gfl2::ui::BUTTON_R )) off = 10;
                else if(pButton->IsHold( gfl2::ui::BUTTON_L )) off = 100;

                if(mPokemonSettings[mCurrentPokemon].monsNo<off) mPokemonSettings[mCurrentPokemon].monsNo = MAX_MONSNO-1;
                else mPokemonSettings[mCurrentPokemon].monsNo-=off;                   
                            
              }
            }
            break;

          case 4: // PokemonFormNo:
            {
              const int MAX_FORMNO = 28;
              if(pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
              {
                mPokemonSettings[mCurrentPokemon].formNo++;
                if(mPokemonSettings[mCurrentPokemon].formNo>=MAX_FORMNO) mPokemonSettings[mCurrentPokemon].formNo = 0;
              }

              if(pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
              {
                if(mPokemonSettings[mCurrentPokemon].formNo==0) mPokemonSettings[mCurrentPokemon].formNo = MAX_FORMNO;
                mPokemonSettings[mCurrentPokemon].formNo--;                
              }
            }
            break;

          case 5: // POKEMON SEX:MALE,FEMALE
            {
              if(pButton->IsTrigger( gfl2::ui::BUTTON_LEFT )  || pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
              {
                mPokemonSettings[mCurrentPokemon].sex = 1 - mPokemonSettings[mCurrentPokemon].sex;
              }
            }
            break;

          case 6: // DEMO TYPE NONE,A-G
            {
              if(pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
              {
                mDemoType--; if(mDemoType==-2) mDemoType=11;
              }
              else if(pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
              {
                mDemoType++; if(mDemoType>=12) mDemoType=-1;
              }
            }
            break;

          case 7: // VS Battle Type: 0~3
            {
              if(pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
              {
                mVSDemoType--; if(mVSDemoType==-1) mVSDemoType=3;
              }
              else if(pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
              {
                mVSDemoType++; if(mVSDemoType>=4) mVSDemoType=0;
              }
            }

          case 8: // DebugInfo
            {
              if(pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) ||  pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
              {
                if(mDebugInfo) mDebugInfo = false;
                else mDebugInfo = true;
              }              
            }
        }
      }

      

      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) || pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
      {
        // Apply Settings:
        if(mPlayerGender==0) mSequenceViewSys->SetPlayerSexMale();
        else mSequenceViewSys->SetPlayerSexFemale();

        if(mTimezone==0) mSequenceViewSys->SetTimezone_Realtime();
        else if(mTimezone==1) mSequenceViewSys->SetTimezone_Morning();
        else if(mTimezone==2) mSequenceViewSys->SetTimezone_Noon();
        else if(mTimezone==3) mSequenceViewSys->SetTimezone_Evening();
        else if(mTimezone==4) mSequenceViewSys->SetTimezone_Night();

        // Pokemon Settings:
        for(int n=0;n<MAX_DEBUG_POKEMON_SETTINGS;n++)
        {
          mSequenceViewSys->SetDebugPokemonSetting(n,mPokemonSettings[n].monsNo,mPokemonSettings[n].formNo,mPokemonSettings[n].sex,true);
        }

        // Demo Type:
        if(mDemoType==-1) mSequenceViewSys->ResetAllDemoType();
        else mSequenceViewSys->SetDemoType(mDemoType);

        // VS Battle Type:
        mSequenceViewSys->SetPlaybackParameter(DemoLib::Sequence::SEQPARAMTYPE_VSBATTLE,mVSDemoType);

        SetState(PT_STATE_RUN_SELECTSEQ);
      }
      break;

		case PT_STATE_RUN_PLAYSEQ:

       if( pButton->IsTrigger( gfl2::ui::BUTTON_X )  ) mSequenceViewSys->TogglePausePlayback();
       if( pButton->IsTrigger( gfl2::ui::BUTTON_R )  ) mSequenceViewSys->PausePlaybackAdvanceFrame();
       if( pButton->IsTrigger( gfl2::ui::BUTTON_START )  ) mSequenceViewSys->StopPlayback();
       if( pButton->IsTrigger( gfl2::ui::BUTTON_B )  ) mSequenceViewSys->SetSystemCancel(true);
       if( pButton->IsTrigger( gfl2::ui::BUTTON_Y )  ) mSequenceViewSys->SystemSync();
       
       if( pButton->IsHold(gfl2::ui::BUTTON_R) ) mSequenceViewSys->SetFastForwardPlayback(true);
       else mSequenceViewSys->SetFastForwardPlayback(false);

			mDrawUtilText->ClearDrawText();

      if(!mDebugInfo) // Don't draw when measuring performance:
      {
			  mDrawUtilText->SetTextScale( 1.5f, 1.5f );
			  mDrawUtilText->DrawText( 0,   0, L"DemoPlayer");			
			  mDrawUtilText->SetTextScale( 1.0f, 1.0f );
			  mDrawUtilText->DrawText( 0,   50, L"SequenceID: %i",mSequenceID);
      }

			// DrawTimer:
			{			
        
        gfl2::math::Vector4 col;
        col.Set(1.0f,1.0f,1.0f,1.0f);

        gfl2::math::Vector4 colRed;
        colRed.Set(1.0f,0.0f,0.0f,1.0f);

        if(mDebugInfo)
        {
          const int update = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
          const int upnext = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);
          const int draw   = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
          const int gpu    = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);

          mDrawUtilText->SetTextScale( 0.6f, 0.6f );
          bool isOver = false;

          if(gpu>=32000) 
          {
            mDrawUtilText->SetTextColor(colRed);
            isOver = true;
          }
          mDrawUtilText->DrawText( 0,   100, L"G %i",gpu);

          if(update + upnext + draw >=32000)
          {
            mDrawUtilText->SetTextColor(colRed);
            isOver = true;
          }
          else mDrawUtilText->SetTextColor(col);
          
          mDrawUtilText->DrawText( 0,   120, L"C %i",update+upnext+draw);

          int frame = mSequenceViewSys->GetCurrentFrame();   
          mDrawUtilText->SetTextColor(col);
          mDrawUtilText->DrawText( 0,   140, L"F %i",(int)frame);
                   
          if(isOver && mFrameLogPos<SEQ_MAX_FRAME)
          {
            mFrameLogF[mFrameLogPos] = frame;
            mFrameLogG[mFrameLogPos] = gpu&0xFFFF;
            mFrameLogC[mFrameLogPos] = (update + upnext + draw)&0xFFFF;
            mFrameLogPos++;            
          }

          if(frame>=0)
          {
            if(frame - mLastFrame > 1) mSkipFrames+= (frame-mLastFrame)-1;
            mLastFrame = frame;
            mTotalFrames = frame;
          }
          
        }
        
        if(!mDebugInfo) // Don't draw when measuring performance:
        {

          mDrawUtilText->SetTextColor(col);
          mDrawUtilText->SetTextScale( 1.0f, 1.0f );

				  int seqFrame = mSequenceViewSys->GetCurrentFrame();
				  mDrawUtilText->DrawText( 0,   180, L"SeqFrame: %i",(int)seqFrame);

				  int bgmPos = Sound::GetStreamSoundPlaySamplePosition();
				  f32 rate = (Sound::GetStreamSoundSampleRate()/30.0);
				  f32 pos = (f32)bgmPos / rate;
				        
          col.Set(1.0f,1.0f,1.0f,1.0f);  
          mDrawUtilText->SetTextColor(col);
				  mDrawUtilText->DrawText( 0,   200, L"BGMFrame: %i",(int)pos);
        }
								
			}

			if(mSequenceViewSys->SeqComFunc_IsFinishSequenceFile() && mSequenceViewSys->Finalize())
			{	
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,1); 
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,1);
        GFL_PRINT("Sequence Has Completed!\n");		

        GFL_SAFE_DELETE(mSequenceViewSys);	

        //BGMのPushPop状態も含みで元に戻せる関数として使っている by iwasawa
        Sound::EndBattleBGMReq(STRM_BGM_T01);

        StateInit_SeqPlayer();
        SetState(PT_STATE_RUN_SELECTSEQ);

        // Dump Frame Log:
        if(mDebugInfo)
        {
          PrintFrameLog();
        }

			}
			break;

		case PT_STATE_RUN_FINISH:
      {               
      }
      return gfl2::proc::RES_FINISH;

		default:
			break;

}

	mSequenceViewSys->Update();

	// ワールド行列更新
	if( mStateReg!=PT_STATE_RUN_PLAYSEQ ) 
	{
		gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
	}



	return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void PeteTestProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
	if( mStateReg==PT_STATE_RUN_PLAYSEQ ) 
	{
		mSequenceViewSys->PreDraw();
		return;
	}
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void PeteTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{	
///	GFL_PRINT("PeteTestProc->Draw()\n"); 

	if( mStateReg==PT_STATE_RUN_PLAYSEQ ) 
	{
		mSequenceViewSys->Draw(displayNo);
	///	return;
	}
//	else 
	if( mStateReg==PT_STATE_RUN_SELECTSEQ  || mStateReg==PT_STATE_RUN_PLAYSEQ || mStateReg == PT_STATE_RUN_SETTINGSMENU)
	{
		
		switch( displayNo )
		{
			case gfl2::gfx::CtrDisplayNo::DOWN:			
				{
					gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
					gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
					u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
					u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

					mRenderingPipelineText->SetRenderTarget( pBackBuffer );
					mRenderingPipelineText->SetDepthStencil( pDepthBuffer );
					mRenderingPipelineText->SetDisplaySize( displayWidth, displayHeight );

					mRenderingPipelineText->Execute();
				}
				break;
		}	
		return;
	}
/*
	// Only Draw Left Display
	if(displayNo != gfl2::gfx::CtrDisplayNo::LEFT) return;

	gfl2::gfx::DisplayDeviceHandle displayHandle = System::GflUse::m_HandleList[displayNo];
	u32 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth(displayHandle);
	u32 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight(displayHandle);

	// 射影変換行列(パースペクティブ(透視法)射影)
	gfl2::math::Matrix44 projMatrix = gfl2::math::Matrix44::GetPerspective(
		gfl2::math::ConvDegToRad(37.0f),		// 視野角30°
		(float)displayWidth / (float)displayHeight,	// アスペクト比
		1.1f,							// 前方投影面までの距離
		1000.0f,							// 後方投影面までの距離
		true);                // 画面の向きに合わせる

	mRenderingPipeline->SetProjectionMatrix(projMatrix);

	// Setup View Matrix:
	gfl2::math::Vector3 eyePosition(80.0f, 100.0f, 250.0f);  // 視点(カメラの位置)
	gfl2::math::Vector3 focusPosition(0.0f, 80.0f,  0.0f);  // 注視点
	gfl2::math::Vector3 upDirection(0.0f, 1.0f,  0.0f);     // カメラの上方向

	gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);
	mRenderingPipeline->SetViewMatrix(viewMatrix);

	// Draw:
	mRenderingPipeline->SetDisplaySize(displayWidth, displayHeight);
	mRenderingPipeline->Execute();
	*/
}

/**
* @brief   PROCのメインを行う関数
*  1/30フレームの②フレーム目の処理です。
*  1/60フレーム動作時には、使われません。
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result PeteTestProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
	return gfl2::proc::RES_CONTINUE;	
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result PeteTestProc::EndFunc(gfl2::proc::Manager* pManager)
{	
	if(!mSequenceViewSys->Finalize())
  {
    return gfl2::proc::RES_CONTINUE;
  }
  DeleteDebugMenu();
	GFL_SAFE_DELETE(mSequenceViewSys);	


	// font
	{
		mDrawUtilText->Terminate();

		GFL_SAFE_DELETE( mDrawUtilText );
		void* pFontBuffer = mFontResource->RemoveResource();
		GflHeapFreeMemory( pFontBuffer );
		GFL_SAFE_DELETE( mFontResource );

		GFL_SAFE_DELETE( mRenderingPipelineText );
      
	}

  GFL_SAFE_DELETE( mDrawEnvNode );

  GFL_SAFE_DELETE( mFrameLogF );
  GFL_SAFE_DELETE( mFrameLogC );
  GFL_SAFE_DELETE( mFrameLogG );

  GFL_SAFE_DELETE( mRenderingPipeline );

  GFL_SAFE_DELETE( mGLAllocator );
  GFL_SAFE_DELETE( mDevAllocator );
  GFL_SAFE_DELETE( mMemAllocator );
    	
	GFL_DELETE_HEAP(mHeap);
	
	return gfl2::proc::RES_FINISH;
}

//デバッグメニューの作成
void PeteTestProc::CreateDebugMenu(void)
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  mSkipMilliSec = 0;

  gfl2::str::MsgData* msgData;
  msgData = GFL_NEW(heap)gfl2::str::MsgData(print::GetMessageArcId_Debug(),GARC_debug_message_d_demoviewer_DAT,heap->GetLowerHandle());

  //開放のためにトップグループは取っておく
  mDebugGroup = gfl2::debug::DebugWin_AddGroup(heap, msgData, d_demoviewer_00,NULL);

  gfl2::debug::DebugWinItem* item;
  item = gfl2::debug::DebugWin_AddItemValueInt( mDebugGroup ,heap, msgData,d_demoviewer_01,&mSkipMilliSec,0,200*1000,100);

  // 連続再生用デバック項目
  gfl2::debug::DebugWinItem* continuePlay;
  continuePlay = gfl2::debug::DebugWin_AddItemValueBool( mDebugGroup ,heap, msgData, d_demoviewer_02, &mIsContinuePlay);

  gfl2::debug::DebugWinItem* fisrtContinuePlay;
  fisrtContinuePlay = gfl2::debug::DebugWin_AddItemValueInt( mDebugGroup ,heap, msgData, d_demoviewer_03, reinterpret_cast<int*>(&mFirstContinuePlaySeqID), 1, GARC_demo_seq_script_DATA_NUM - 1 );

  gfl2::debug::DebugWinItem* endContinuePlay;
  endContinuePlay = gfl2::debug::DebugWin_AddItemValueInt( mDebugGroup ,heap, msgData, d_demoviewer_04, reinterpret_cast<int*>(&mEndContinuePlaySeqID), 1, GARC_demo_seq_script_DATA_NUM - 1 );

  GFL_DELETE(msgData);
}

//デバッグメニューの破棄
void PeteTestProc::DeleteDebugMenu(void)
{
  gfl2::debug::DebugWin_RemoveGroup(mDebugGroup);
}


	GFL_NAMESPACE_END(Pete)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
