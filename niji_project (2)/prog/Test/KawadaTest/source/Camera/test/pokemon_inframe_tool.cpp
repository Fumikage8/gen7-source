
#if PM_DEBUG

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   pokemon_inframe_tool.cpp
 *  @brief  ポケモンインフレームカメラ　ツール
 *  @author
 *  @date   2012.11.12
 *  @author kawada_koji@gamefreak.co.jp
 *  @date   2015.11.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <Test/KawadaTest/include/Camera/test/pokemon_inframe_tool.h>

#include <GameSys/include/DllProc.h>
#include <Debug/DebugKeyboard/include/DebugKeyboard.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/pokemon_inframe.gaix>
#include <arc_index/message.gaix>

#include <Print/include/PrintSystem.h>

#include <niji_conv_header/app/pokemon_inframe/ui_camera.h>
#include <niji_conv_header/app/pokemon_inframe/ui_camera_anim_list.h>
#include <niji_conv_header/app/pokemon_inframe/ui_camera_pane.h>

#include <System/source/Camera/field/CameraDebugPrimitive.h>
#include <System/source/Camera/field/CameraUtility.h>

#include <System/include/Camera/ModelViewerInFrame.h>
#include <System/include/Camera/InFrameCamera.h>
#include <System/include/nijiAllocator.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <PokeTool/include/PokeModelSystem.h>
#include <PokeTool/include/PokeModel.h>

#include <Fade/include/gfl2_FadeManager.h>

#include <System/include/HeapDefine.h>
#include <System/include/GflUse.h>
#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>

#include <util/include/gfl2_UtilTextRenderingPipeLine.h>
#include <util/include/gfl2_DrawUtilText.h>


GFL_NAMESPACE_BEGIN(System)
GFL_NAMESPACE_BEGIN(Camera)
  GFL_NAMESPACE_BEGIN(test)

    class PokemonInframeToolView : public app::ui::UIView
    {
      GFL_FORBID_COPY_AND_ASSIGN(PokemonInframeToolView);

    public:
      PokemonInframeToolView(app::util::Heap* appHeap, app::util::AppRenderingManager* appRenderingManager)
        : app::ui::UIView(appHeap, appHeap->GetDeviceAllocator(), appHeap->GetDeviceAllocator()),
          m_appRenderingManager(appRenderingManager),
          m_buf(NULL)
      {
        ///////////////////////
        // 読み込み
        ///////////////////////
        {
          app::util::FileAccessor::FileOpenSync(ARCID_POKEMON_INFRAME, appHeap->GetDeviceHeap(), true);
          app::util::FileAccessor::FileLoadSync(
              ARCID_POKEMON_INFRAME,
              GARC_pokemon_inframe_ui_camera_applyt_COMP,
              &m_buf,
              appHeap->GetDeviceHeap(),
              true,
              128
          );
          app::util::FileAccessor::FileCloseSync(ARCID_POKEMON_INFRAME);
        }

        ///////////////////////
        // レイアウト生成
        ///////////////////////
        {
          const app::util::G2DUtil::LytResData resTbl[] =
          {
            // res_0
            {
              m_buf,
              0,
              app::util::G2DUtil::ATTACH_ROOT
            },
          };
       
          const app::util::G2DUtil::LytwkData wkTbl[] =
          {
            // wk_0
            {
              // 下画面
              0,  // res_0
              LYTRES_UI_CAMERA_UI_CAMERA_BFLYT,
              LA_UI_CAMERA_UI_CAMERA___NUM,
              m_buf,
              true,
              app::util::G2DUtil::SETUP_LOWER,
              true
            },
          };
       
          this->Create2D(  // 親であるapp::ui::UIViewのメンバ
              appHeap,
              NULL,
              1,  // wk_0の1個だけ 
              resTbl,
              GFL_NELEMS(resTbl), 
              wkTbl,
              GFL_NELEMS(wkTbl), 
              app::ui::UIView::UNUSE_MESSAGE_DATA
          );
        }
      }
      virtual ~PokemonInframeToolView()
      {
        this->Delete2D();  // 親であるapp::ui::UIViewのメンバ
        GflHeapSafeFreeMemory(m_buf);
      }
      virtual void Update(void)
      {
        app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
      }
      virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo)
      {
        app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
        pG2DUtil->AddDrawableLytWkForOneFrame( m_appRenderingManager, displayNo, 0, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_0
      }

    private:
      app::util::AppRenderingManager*  m_appRenderingManager;
      void*                            m_buf;

    };

  GFL_NAMESPACE_END(test)
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(System)




namespace System{
namespace Camera{
  namespace test{

    //----------------------------------------------------------------------------
    /**
     *  @brief  コンストラクタ
     */
    //-----------------------------------------------------------------------------
    PokemonInframeTool::PokemonInframeTool(void) :
      GameSys::GameProc(),
      m_Seq(0),
      m_SubSeq(0),
      m_pRootDeviceHeap(NULL),
      m_pAppHeap(NULL),
      m_pEdgeMapTextureAllocator(NULL),
      m_pAppRenderingManager(NULL),
      m_pView(NULL),
      m_pPokeModelSys(NULL),
      m_PokeModelSysSeq(0),
      m_pPokeModel(NULL),
      m_PokeModelIsLoaded(false),
      m_PokeModelIsCreated(false),
      m_PokeModelIsAdded(false),
      m_pPokeParam(NULL),
      m_PokeDataSeq(0),
      m_bIsHIO(false) ,
      m_eSelectedPokemonMonsNo(MONSNO_HUSIGIDANE),
      m_nSelectedPokemonFormNo(0),
      m_eSelectedPokemonSex(pml::SEX_MALE),
      m_keyboard_buf(NULL),
      m_keyboard_choice(-1),
      m_layout_string(NULL),
      m_UiPokeParamChange(false),
      m_bIsChangeModel(false) ,
      m_pInModelInFrame(NULL),
      m_pInFrameCamera(NULL),
      m_fFrameScale(1.0f) ,
      m_fFramePosiOfstRatioY(0.0f) ,
      m_fModelYaw(0.0f) ,
      m_fModelYawOfstR(0.0f) ,
      m_fFrameLength(100.000001f) ,
      m_bIsEnableDrawDebug(false) ,
      m_eDebugObjectLevel(DEBUG_OBJECT_LEVEL_3) ,
      m_bIsDispHelp(false) ,
      m_bIsTurnRightMode(false),
      m_fAnimeFrame( 0.0f),
      m_isContestMode( false),
      m_drawUtilTextRenderingPipeLine(NULL),
      m_drawUtilText(NULL)
    {
      return ;
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  初期化
     */
    //-----------------------------------------------------------------------------
    gfl2::proc::Result PokemonInframeTool::InitFunc(gfl2::proc::Manager* pProcManager)
    {
      const gfl2::ui::DeviceManager* pUIDeviceManager = GetGameManager()->GetUiDeviceManager();
      const gfl2::ui::Button*        pButton = pUIDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
      if( pButton->IsHold( gfl2::ui::BUTTON_L))
      {
        m_isContestMode = true;
      }

      return gfl2::proc::RES_FINISH;
    }

    //-----------------------------------------------------------------------------
    // 枠の位置を取得する
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::GetFramePosition(int* pnLeft, int* pnTop, int* pnRight, int* pnBottom)
    {
      if( !m_isContestMode)
      {
        const gfl2::math::VEC2 inCenter(200.0f, 120.0f);
        const float fLengthHalf = m_fFrameLength*0.5f;
        (*pnLeft)   = inCenter.x - fLengthHalf;
        (*pnTop)    = inCenter.y - fLengthHalf;
        (*pnRight)  = inCenter.x + fLengthHalf;
        (*pnBottom) = inCenter.y + fLengthHalf;
      }
      else
      {
        m_fFrameLength = 180.0f;
        f32 sizeX = 170.0f / 2.0f;
        f32 sizeY = 124.0f / 2.0f;
        const gfl2::math::VEC2 inCenter( 200.0f, 102.0f);
        //const float fLengthHalf = m_fFrameLength*0.5f;
        (*pnLeft)   = inCenter.x - sizeX;
        (*pnTop)    = inCenter.y - sizeY;
        (*pnRight)  = inCenter.x + sizeX;
        (*pnBottom) = inCenter.y + sizeY;
      }
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  更新
     */
    //-----------------------------------------------------------------------------
    gfl2::proc::Result PokemonInframeTool::UpdateFunc(gfl2::proc::Manager* pProcManager)
    {
      switch( m_Seq ){
      case SEQ_INITIALIZE:
        if( initialize() ){
          ++m_Seq;
          m_SubSeq = 0;
        }
        break;
        
      case SEQ_UPDATE:
        switch( update() ){
        case RET_NONE:
          break;
        case RET_POKEINPUT:
          m_Seq = SEQ_POKEINPUT;
          m_SubSeq = 0;
          break;
        case RET_END:
          ++m_Seq;
          m_SubSeq = 0;
          break;
        }
        break;
        
      case SEQ_FINALIZE:
        if( finalize() ){
          return gfl2::proc::RES_FINISH;
        }
        break;
        
      case SEQ_POKEINPUT:
        {
          m_keyboard_buf->Clear();
          debug::KeyboardProc* proc = GameSys::CallDebugKeyboardProc();
          proc->SetupParam( m_pAppHeap->GetDeviceHeap(), m_pAppHeap->GetDeviceHeap(), m_keyboard_buf, 256 );
          //proc->SetLayoutSystem( m_pView->GetLayoutSystem() );
          proc->SetSearchWords( GARC_message_monsname_DAT, &m_keyboard_choice );
          m_keyboard_choice = -1;
        }
        ++m_Seq;
        break;
      case SEQ_POKEINPUT_RECOVER:
        {
          if( m_keyboard_choice > 0 ){
            // モンスターナンバー保存
            changeMonsNo( static_cast<MonsNo>(m_keyboard_choice) );
          }
        }
        m_Seq = SEQ_UPDATE;
        break;
      }

      return gfl2::proc::RES_CONTINUE;
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  描画
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::PreDrawFunc(gfl2::proc::Manager* pProcManager)
    {
      if( m_Seq == SEQ_UPDATE )
      {
        m_pAppRenderingManager->PreDraw();
      }
    }

    void PokemonInframeTool::DrawFunc(gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo)
    {
      if( m_Seq == SEQ_UPDATE )
      {
        draw3D(displayNo);
        m_pView->DrawTree(displayNo);
        m_pAppRenderingManager->Draw(displayNo);
      }
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  初期化関数
     */
    //-----------------------------------------------------------------------------
    bool PokemonInframeTool::initialize( void )
    {
      enum{
        SEQ_INITIALIZE,
        SEQ_FADEIN,
        SEQ_FADEIN_WAIT,
      };

      switch( m_SubSeq ){
      case SEQ_INITIALIZE:
        {
          initializeHeap();
          
          initializeDrawUtilText();

          initializeInFrame();
          initializeAppRenderingManager();
          m_pAppRenderingManager->SetInFrameCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pInFrameCamera);

          m_bIsEnableDrawDebug = false;
          
          initialize3D();
          initialize2D();

          if( initializePokeModelSys() == false ) return false; 

          initializePokeData();
          initializeUi();
          m_SubSeq = SEQ_FADEIN_WAIT; // フェード処理は必要ない
          return true;
          // ++m_SubSeq;
          // break through
        }
      case SEQ_FADEIN:
        {
          GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
          ++m_SubSeq;
          break;
        }
      case SEQ_FADEIN_WAIT:
        {
          if( GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_DOUBLE) ){
            return true;
          }
          break;
        }
      }
      return false;
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  更新処理
     */
    //-----------------------------------------------------------------------------
    u32 PokemonInframeTool::update( void )
    {
      const float fPreFrameScale          = m_fFrameScale;
      const float fPreFramePosiOfstRatioY = m_fFramePosiOfstRatioY;
      const float fPreModelYaw            = m_fModelYaw;
      u32 ret = RET_NONE;

      if( !isPokeDataLoading() )
      {
        ret = updateUi();
        UpdateByInput();
      }

      if( ret == RET_NONE )
      {
        updatePokeData();
        update3D();
        m_pView->UpdateTree();
        m_pAppRenderingManager->Update();
      }

      if( m_fFrameScale          != fPreFrameScale          ) setTextboxPaneString(PANENAME_UI_CAMERA_PANE_SCALE_NAME,     m_fFrameScale);
      if( m_fFramePosiOfstRatioY != fPreFramePosiOfstRatioY ) setTextboxPaneString(PANENAME_UI_CAMERA_PANE_Y_OFFSET_NAME2, m_fFramePosiOfstRatioY);
      if( m_fModelYaw            != fPreModelYaw            ) setTextboxPaneString(PANENAME_UI_CAMERA_PANE_ROTATE_NAME,    m_fModelYaw, true);

      return ret;
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  破棄処理
     */
    //-----------------------------------------------------------------------------
    bool PokemonInframeTool::finalize( void )
    {
      enum
      {
        // SEQ_FADEOUT,
        // SEQ_FADEOUT_WAIT,
        SEQ_ULCD_OFF_REQUEST,
        SEQ_ULCD_OFF_WAIT,
        SEQ_FINALIZE,
      };

      switch( m_SubSeq )
      {
      // フェード処理は必要ない
      /*
      case SEQ_FADEOUT:
        gfl::grp::Fade_RequestOut( gfl::grp::Fade::DISP_DOUBLE, gfl::grp::Fade::OUTTYPE_BLACK, gfl::grp::FADE_DEFAULT_SYNC, true );
        ++m_SubSeq;
        // 描画OFF
        break;
      case SEQ_FADEOUT_WAIT:
        if( !gfl::grp::Fade_IsEnd() ){
          break;
        }
        ++m_SubSeq;
        // break through
      */

      case SEQ_ULCD_OFF_REQUEST:
        m_SubSeq = SEQ_ULCD_OFF_WAIT;
        break;

      case SEQ_ULCD_OFF_WAIT:
        m_SubSeq++;
        break;

      case SEQ_FINALIZE:
        if(finalizePokeData())
        {
          // UI
          if( m_pView )
          {
            GFL_SAFE_DELETE(m_layout_string);
          
            GFL_SAFE_DELETE(m_pView);
          }
          
          if( finalizePokeModelSys() == false ) return false; 

          if( m_pAppRenderingManager )
          {
            if(m_pAppRenderingManager->CanDestroy())
            {
              GFL_SAFE_DELETE(m_pAppRenderingManager);
            }
            else
            {
              GFL_ASSERT(0);
            }
          }

          if( m_pInModelInFrame )
          {
            GFL_SAFE_DELETE(m_pInFrameCamera);
            GFL_SAFE_DELETE(m_pInModelInFrame);
          }

          finalizeDrawUtilText();
          
          if( m_pRootDeviceHeap )
          {
            GFL_SAFE_DELETE(m_pEdgeMapTextureAllocator);
            GFL_SAFE_DELETE(m_pAppHeap);
           
            GFL_DELETE_HEAP(m_pRootDeviceHeap);
            m_pRootDeviceHeap = NULL;
          }

          return true;
        }
      }

      return false;
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  ヒープ初期化
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::initializeHeap( void )
    {
      // すでに保持されている場合、何もしない。
      if( m_pRootDeviceHeap == NULL ){
        gfl2::heap::HeapBase* deviceHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);

        m_pRootDeviceHeap = GFL_CREATE_LOCAL_HEAP(deviceHeap, ROOT_DEVICE_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);

        m_pAppHeap = GFL_NEW(m_pRootDeviceHeap) app::util::Heap();
        m_pAppHeap->LocalHeapCreate(m_pRootDeviceHeap, m_pRootDeviceHeap, HEAP_SIZE, DEVICE_SIZE);

        m_pEdgeMapTextureAllocator = GFL_NEW(m_pRootDeviceHeap) System::nijiAllocator(m_pRootDeviceHeap);
      }
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  DrawUtilText生成
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::initializeDrawUtilText( void )
    {
      // すでに保持されている場合、何もしない。
      if( m_drawUtilText == NULL )
      {
        m_drawUtilTextRenderingPipeLine = GFL_NEW(m_pAppHeap->GetDeviceHeap()) gfl2::util::UtilTextRenderingPipeLine( m_pAppHeap->GetDevGLHeapAllocator(), 512, 256 );
        m_drawUtilText = GFL_NEW(m_pAppHeap->GetDeviceHeap()) gfl2::util::DrawUtilText( m_pAppHeap->GetDeviceAllocator(), m_pAppHeap->GetDeviceAllocator() ); 
        m_drawUtilText->Initialize( m_pAppHeap->GetDeviceAllocator(), m_pAppHeap->GetDeviceAllocator(), m_drawUtilTextRenderingPipeLine->GetUtilTextRenderPath() );
      } 
    }
    //----------------------------------------------------------------------------
    /**
     *  @brief  DrawUtilText破棄
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::finalizeDrawUtilText(void)
    {
      if( m_drawUtilText )
      {
        m_drawUtilText->Terminate();
        GFL_SAFE_DELETE( m_drawUtilText );
        GFL_SAFE_DELETE( m_drawUtilTextRenderingPipeLine );
      }
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  インフレーム初期化
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::initializeInFrame( void )
    {
      // すでに保持されている場合、何もしない。
      if( m_pInModelInFrame == NULL )
      {
        m_pInModelInFrame = GFL_NEW(m_pAppHeap->GetDeviceHeap()) System::Camera::CModelViewerInFrame();
        int nLeft, nTop, nRight, nBottom; GetFramePosition(&nLeft, &nTop, &nRight, &nBottom);
        m_pInModelInFrame->Init(System::Camera::CModelViewerInFrame::SCREEN_UPPER, nLeft, nTop, nRight, nBottom, System::Camera::CModelViewerInFrame::TURN_DIRECTION_FREE);
       
        m_pInFrameCamera = GFL_NEW(m_pAppHeap->GetDeviceHeap()) System::Camera::InFrameCamera();
      }
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  AppRenderingManager初期化
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::initializeAppRenderingManager( void )
    {
      // すでに保持されている場合、何もしない。
      if( m_pAppRenderingManager == NULL )
      {
        app::util::AppRenderingManager::Description desc;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        m_pAppRenderingManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();
        m_pAppRenderingManager->SyncCreate(
          desc, 
          m_pAppHeap,
          m_pEdgeMapTextureAllocator,
          GetGameManager()->GetAsyncFileManager()
        );
        if(!m_pAppRenderingManager->IsCreated())
        {
          GFL_ASSERT(0);
        }
      }
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  ３Dシステム初期化
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::initialize3D( void )
    {
      // 何もしない。initializeInFrame、initializeAppRenderingManagerで十分。
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  ３D更新
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::update3D( void )
    {
      // 何もしない。AppRenderingManagerのUpdateで十分。
    }
    
    //----------------------------------------------------------------------------
    /**
     *  @brief  ３D描画
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::draw3D( gfl2::gfx::CtrDisplayNo displayNo )
    {
      if( m_bIsEnableDrawDebug == true )
      {
#ifdef GF_PLATFORM_WIN32
        if( m_eDebugObjectLevel >= DEBUG_OBJECT_LEVEL_1 ) m_pInModelInFrame->DrawFrame(displayNo, (m_eDebugObjectLevel >= DEBUG_OBJECT_LEVEL_2));
        if( m_eDebugObjectLevel >= DEBUG_OBJECT_LEVEL_3 ) m_pInModelInFrame->DrawAABB(displayNo, *m_pInFrameCamera);
#endif  // GF_PLATFORM_WIN32
        DispHelp(displayNo);
      }
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  2Dシステムの初期化
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::initialize2D( void )
    {
      // 何もしない。initializeUiで十分。
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  ポケモンモデルシステム生成
     */
    //-----------------------------------------------------------------------------
    bool PokemonInframeTool::initializePokeModelSys( void )
    {
      enum
      {
        SEQ_NONE     = 0,
        SEQ_CREATING = 1,
        SEQ_WORKING  = 2,
      };
      bool ret = false;  // 生成できたらtrue
      switch( m_PokeModelSysSeq )
      {
      case SEQ_NONE    :
        {
          m_pPokeModelSys = GFL_NEW(m_pAppHeap->GetDeviceHeap()) PokeTool::PokeModelSystem();
          //システムの非同期初期化開始
          m_pPokeModelSys->InitSystemAsync(m_pAppHeap->GetDeviceHeap(), m_pAppHeap->GetDeviceHeap(), GetGameManager()->GetAsyncFileManager(), 1);
          ++m_PokeModelSysSeq;
          break;
        }
      case SEQ_CREATING:
        {
          if( m_pPokeModelSys->IsFinishInitSystemAsync() )
          {
            //システム初期化
            m_pPokeModelSys->CreateSystem(false, m_pAppHeap->GetDevGLHeapAllocator());
            //ポケモン用ヒープ作成
            PokeTool::PokeModelSystem::HeapOption opt;
            opt.animeType      = PokeTool::MODEL_ANIMETYPE_BATTLE;
            opt.useIdModel     = false;
            opt.useShadow      = true; 
            opt.useColorShader = false;
            m_pPokeModelSys->CreatePokeModelHeap(m_pAppHeap->GetDeviceHeap(), &opt);
            ++m_PokeModelSysSeq;
          }
          break;
        }
      case SEQ_WORKING :
        {
          ret = true;
          break;
        }
      }
      return ret;
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  ポケモンモデルシステム破棄
     */
    //-----------------------------------------------------------------------------
    bool PokemonInframeTool::finalizePokeModelSys( void )
    {
      enum
      {
        SEQ_WORKING    = 2,
        SEQ_DESTROYING = 3,
        SEQ_NONE       = 0,
      };
      bool ret = false;  // 破棄できたらtrue
      switch( m_PokeModelSysSeq )
      {
      case SEQ_WORKING:
        {
          //ポケモン用ヒープ削除
          m_pPokeModelSys->DeletePokeModelHeap();
          //システム開放開始
          m_pPokeModelSys->TermSystemAsync();
          ++m_PokeModelSysSeq;
          break;
        }
      case SEQ_DESTROYING:
        {
          if( m_pPokeModelSys->IsFinishTermSystemAsync() )
          {
            GFL_SAFE_DELETE(m_pPokeModelSys);
            m_PokeModelSysSeq = SEQ_NONE;
          }
          break;
        }
      case SEQ_NONE:
        {
          ret = true;
          break;
        }
      }
      return ret;
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  ポケデータ生成
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::initializePokeData( void )
    {
      if( m_pPokeModel == NULL ){

        MonsNo      eMonsNo;
        pml::FormNo nFormNo;
        pml::Sex    eSex;
        GetSelectedPokemonInViewer(&eMonsNo, &nFormNo, &eSex);

        m_pPokeParam = GFL_NEW(m_pAppHeap->GetDeviceHeap()) pml::pokepara::PokemonParam( m_pAppHeap->GetDeviceHeap(), eMonsNo, 8, 0 );
        m_pPokeParam->ChangeFormNo(nFormNo);
        m_pPokeParam->ChangeSex(eSex);
        PokeTool::GetSimpleParam( &m_PokeParamSimple, m_pPokeParam );

        m_keyboard_buf = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 256, m_pAppHeap->GetDeviceHeap() );

        m_PokeDataSeq = SEQ_POKEDATA_LOAD_LOAD;
        m_bIsHIO = false;
      }
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  破棄処理
     */
    //-----------------------------------------------------------------------------
    bool PokemonInframeTool::finalizePokeData( void )
    {
      if( m_PokeDataSeq < SEQ_POKEDATA_DELETE_START ){ m_PokeDataSeq = SEQ_POKEDATA_DELETE_START; }
      
      switch( m_PokeDataSeq ){
      case SEQ_POKEDATA_DELETE_START:
        if( destroyPokeModel() )
        {
          m_PokeDataSeq++;
        }
        break;
        
      case SEQ_POKEDATA_DELETE:
        m_PokeDataSeq++;
        break;
        
      case SEQ_POKEDATA_DELETE_END:
        GFL_SAFE_DELETE(m_pPokeParam);
        GFL_SAFE_DELETE(m_keyboard_buf);
        return true;
      }
      return false;
    }
    
    //----------------------------------------------------------------------------
    /**
     *  @brief  ポケモンデータ更新
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::updatePokeData( void )
    {
      switch(m_PokeDataSeq){
      case SEQ_POKEDATA_UPDATE:
        if( isUiPokeParamChange() ){
          ++m_PokeDataSeq;
          uiPokeParamChangeFlagReset();
          m_bIsChangeModel = false;  // 破棄するのでフラグをfalseにしておく
          m_pInModelInFrame->ResetModel();
          if( isPokeModelEnable() ) m_pPokeModel->SetVisible(false);
        }
        break;
      case SEQ_POKEDATA_LOAD_DELETE:
        if( destroyPokeModel() )
        {
          ++m_PokeDataSeq;
          // break through
        }
        else
        {
          break;
        }
        
      case SEQ_POKEDATA_LOAD_LOAD:
        {
          startCreatePokeModel();
          m_bIsChangeModel = true;
          m_PokeDataSeq = SEQ_POKEDATA_UPDATE;
        }
        break;
      }

      if( m_bIsChangeModel == true )
      {
        if( m_pPokeModel != NULL )
        {
          if( !m_PokeModelIsLoaded )
          {
            if( m_pPokeModel->IsFinishLoad() != false )
            {
              m_PokeModelIsLoaded = true;
            }
          }
         
          if( m_PokeModelIsLoaded )
          {
            if( !m_PokeModelIsCreated )
            {
              if( m_pPokeModel->IsEnable() == false )
              {
                m_pPokeModel->Create();
                m_PokeModelIsCreated = true;
                GFL_ASSERT( m_pPokeModel->IsEnable() );
              }
            }
          }
         
          if( isPokeModelEnable() )
          {
            m_pPokeModel->SetVisible(true);
           
            m_pPokeModel->ChangeAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A, true);
            m_pPokeModel->SetAnimationIsLoop(true);
           
            m_pInModelInFrame->SetAndInitModel(m_pPokeModel);
            m_fFrameScale          = m_pInModelInFrame->GetFrameScale();
            m_fFramePosiOfstRatioY = m_pInModelInFrame->GetFramePositionOffsetRatioY();
            m_fModelYaw            = m_pInModelInFrame->GetModelYaw();
            m_fModelYawOfstR       = m_pInModelInFrame->GetModelYawOffsetR();
           
            m_bIsChangeModel = false;
            m_bIsEnableDrawDebug = true; // 最初のモデル描画を待たないと、なぜかデバッグフォントがうまく表示されない  // [niji注] このコメントはkujira時代のコメントそのままです。nijiでどうなのかは試していません。
            SetSelectedPokemonInViewer(m_pPokeParam->GetMonsNo(), m_pPokeParam->GetFormNo(), m_pPokeParam->GetSex());
           
            m_pAppRenderingManager->AddPokeModel(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pPokeModel);
            m_PokeModelIsAdded = true;
          }
          //else
          //  m_pPokeModel->SetVisible(false);  // [niji注] この記述要らないと思うのでnijiでコメントアウトしました。
        }
      }

      field::CUtil::MaxMinClip(&m_fFrameScale, 3.0f, 0.1f);
      field::CUtil::MaxMinClip(&m_fFramePosiOfstRatioY, 1.0f, -1.0f);
      m_fModelYaw      = field::CUtil::ConvertFromMinusPiToPlusPi(m_fModelYaw);
      m_fModelYawOfstR = field::CUtil::ConvertFromMinusPiToPlusPi(m_fModelYawOfstR);

      m_pInModelInFrame->SetFrameScale(m_fFrameScale);
      m_pInModelInFrame->SetFramePositionOffsetRatioY(m_fFramePosiOfstRatioY);
      m_pInModelInFrame->SetModelYaw((m_bIsTurnRightMode == true) ? gfl2::math::FAbs(m_fModelYaw) + m_fModelYawOfstR : m_fModelYaw);

      int nLeft, nTop, nRight, nBottom; GetFramePosition(&nLeft, &nTop, &nRight, &nBottom);
      m_pInModelInFrame->SetFrame(nLeft, nTop, nRight, nBottom);
      m_pInModelInFrame->Update(m_pInFrameCamera); // ぽけもんいんふれーむ処理

      m_pPokeModelSys->Update();
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  ポケモンモデル生成開始
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::startCreatePokeModel( void )
    {
      if( m_pPokeModel == NULL )
      {
        PokeTool::GetSimpleParam( &m_PokeParamSimple, m_pPokeParam );
        PokeTool::PokeModel::SetupOption opt;
        opt.dataHeap = m_pAppHeap->GetDeviceHeap();
        opt.workHeap = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
        opt.animeType = PokeTool::MODEL_ANIMETYPE_BATTLE;
        opt.useShadow = false;//true;  // @todo 影がちゃんと出るようになったらtrueに変えよう
        opt.useIdModel = false;
        m_pPokeModel = m_pPokeModelSys->CreateModelSimple( &m_PokeParamSimple, opt );
      }
    }
    //----------------------------------------------------------------------------
    /**
     *  @brief  ポケモンモデル破棄(破棄できたらtrueを返す)
     */
    //-----------------------------------------------------------------------------
    bool PokemonInframeTool::destroyPokeModel( void )
    {
      if( m_pPokeModel )
      {
        if( !m_PokeModelIsLoaded )
        {
          if( m_pPokeModel->IsFinishLoad() != false )
          {
            m_PokeModelIsLoaded = true;
          }
        }

        if( m_PokeModelIsLoaded )
        {
          bool pokeModelIsAdded = m_PokeModelIsAdded;
          if( m_PokeModelIsCreated )
          {
            if( m_PokeModelIsAdded )
            {
              m_pAppRenderingManager->RemovePokeModel(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pPokeModel);
              m_PokeModelIsAdded = false;
            }
          }

          if( pokeModelIsAdded == false )
          {
            GFL_ASSERT( m_pPokeModel->CanDestroy() );
            m_pPokeModelSys->DeleteModel(m_pPokeModel);
            m_pPokeModel = NULL;
            m_PokeModelIsLoaded  = false;
            m_PokeModelIsCreated = false;
          }
        }
      }

      return ( m_pPokeModel == NULL );
    }
    //----------------------------------------------------------------------------
    /**
     *  @brief  ポケモンモデルが有効(あらゆる操作ができる)か(有効ならtrueを返す)
     */
    //-----------------------------------------------------------------------------
    bool PokemonInframeTool::isPokeModelEnable( void ) const
    {
      bool ret = false;
      if( m_pPokeModel )
      {
        if( m_PokeModelIsLoaded && m_PokeModelIsCreated )
        {
          ret = true;
        }
      }
      return ret;
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  UI初期化
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::initializeUi( void )
    {
      if( m_pView == NULL ){
        m_pView = GFL_NEW(m_pAppHeap->GetDeviceHeap()) PokemonInframeToolView(m_pAppHeap, m_pAppRenderingManager);
        
        m_layout_string = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 32, m_pAppHeap->GetDeviceHeap() );

        setupUiDisp();
        setTextboxPaneString(PANENAME_UI_CAMERA_PANE_SCALE_NAME,     m_fFrameScale);
        setTextboxPaneString(PANENAME_UI_CAMERA_PANE_Y_OFFSET_NAME2, m_fFramePosiOfstRatioY);
        setTextboxPaneString(PANENAME_UI_CAMERA_PANE_ROTATE_NAME,    m_fModelYaw, true);
      }
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  UIタッチ判定
     */
    //-----------------------------------------------------------------------------
    u32 PokemonInframeTool::updateUi(void)
    {
      gfl2::lyt::LytWk* pLytWk = m_pView->GetLayoutWork(0);  // 親であるapp::ui::UIViewのメンバ  // wk_0
      if( !pLytWk->IsHitCheckOK() ){
        return RET_NONE;
      }

      GameSys::GameManager* p_gman = this->GetGameManager();
      gfl2::ui::TouchPanel*     p_touchPanel;
      gfl2::ui::Button*        p_button;
      p_touchPanel = p_gman->GetUiDeviceManager()->GetTouchPanel(0);
      p_button = p_gman->GetUiDeviceManager()->GetButton(0);

      // StartButtonで終了
      if( p_button->IsTrigger( gfl2::ui::BUTTON_START ) ){
        return RET_END;
      }

      static const UI_Reaction sc_Reaction[] = {
        // ポケモン種類
        {
          PANENAME_UI_CAMERA_PANE_MONS_L,
          &PokemonInframeTool::updateUi_MonsNo,
          -1,
        },
        {
          PANENAME_UI_CAMERA_PANE_MONS_R,
          &PokemonInframeTool::updateUi_MonsNo,
          1,
        },
        {
          PANENAME_UI_CAMERA_PANE_MONS_NAME,
          &PokemonInframeTool::updateUi_MonsNo,
          0,
        },

        // フォルム
        {
          PANENAME_UI_CAMERA_PANE_FORM_L,
          &PokemonInframeTool::updateUi_FormNo,
          -1,
        },
        {
          PANENAME_UI_CAMERA_PANE_FORM_R,
          &PokemonInframeTool::updateUi_FormNo,
          1,
        },

        // 性別
        {
          PANENAME_UI_CAMERA_PANE_SEX_BACK,
          &PokemonInframeTool::updateUi_Sex,
          1,
        },

        // 卵
        {
          PANENAME_UI_CAMERA_PANE_EGG_BACK,
          &PokemonInframeTool::updateUi_Egg,
          1,
        },

        // Scale
        {
          PANENAME_UI_CAMERA_PANE_SCALE_L,
          &PokemonInframeTool::updateUi_Scale,
          -1,
        },
        {
          PANENAME_UI_CAMERA_PANE_SCALE_R,
          &PokemonInframeTool::updateUi_Scale,
          1,
        },

        // Yオフセット
        {
          PANENAME_UI_CAMERA_PANE_Y_OFFSET_L,
          &PokemonInframeTool::updateUi_OffsetY,
          -1,
        },
        {
          PANENAME_UI_CAMERA_PANE_Y_OFFSET_R,
          &PokemonInframeTool::updateUi_OffsetY,
          1,
        },

        // Y回転
        {
          PANENAME_UI_CAMERA_PANE_ROTATE_L,
          &PokemonInframeTool::updateUi_RotateY,
          -1,
        },
        {
          PANENAME_UI_CAMERA_PANE_ROTATE_R,
          &PokemonInframeTool::updateUi_RotateY,
          1,
        },

      };
      
      if( p_touchPanel->IsTouchRepeat() ){
        {
          for( u32 i=0; i<GFL_NELEMS(sc_Reaction); ++i ){
            if( pLytWk->GetHitPane( p_touchPanel->GetX(), p_touchPanel->GetY(), sc_Reaction[i].index ) ){
              return (this->*sc_Reaction[i].func)( sc_Reaction[i].plus_minus );
            }
          }
        }
      }

      return RET_NONE;
    }

    u32 PokemonInframeTool::updateUi_MonsNo( s32 plus_minus )
    {
      if( gfl2::math::Abs(plus_minus) > 0 ){
        MonsNo old_monsno = m_pPokeParam->GetMonsNo();
        MonsNo new_monsno;
        if(plus_minus < 0){
          new_monsno = static_cast<MonsNo>( ( old_monsno + MONSNO_END ) % ( MONSNO_END + 1 ) );
        }else{
          new_monsno = static_cast<MonsNo>( ( old_monsno + 1 ) % ( MONSNO_END + 1 ) );
        }

        changeMonsNo( new_monsno );
        return RET_NONE;
      }
      else{

        // 入力へ
        return RET_POKEINPUT;
      }
    }

    void PokemonInframeTool::changeMonsNo( MonsNo new_monsno )
    {
      MonsNo old_monsno = m_pPokeParam->GetMonsNo();
      m_pPokeParam->ChangeMonsNo( new_monsno, 0 );  // フォルムは0にする
      if( old_monsno == MONSNO_NULL ) {  // MONSNO_NULLでなくなった際にレベル1にする
        m_pPokeParam->SetExp(0);
        m_pPokeParam->RecoverHpFull();
        m_pPokeParam->SetDefaultWaza();
        m_pPokeParam->SetNotRare();
        m_pPokeParam->Birth();  // [niji注] nijiでいれた対策。MONSNO_NULLでなくたった際にタマゴフラグをfalseにしておく。
        pml::personal::LoadGrowTable( m_pPokeParam->GetMonsNo(), m_pPokeParam->GetFormNo() );
      }
      if( new_monsno == MONSNO_NULL )
      {
        m_pPokeParam->SetEggFlag();  // [niji注] nijiでいれた対策。MONSNO_NULLのときはタマゴフラグを立てておく。
      }

      setupUiDisp();
      m_UiPokeParamChange = true;
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  フォルム更新
     */
    //-----------------------------------------------------------------------------
    u32 PokemonInframeTool::updateUi_FormNo( s32 plus_minus )
    {
      MonsNo monsno = m_pPokeParam->GetMonsNo();
      pml::FormNo old_formno = m_pPokeParam->GetFormNo();
      pml::personal::LoadPersonalData( monsno, old_formno );
      u32 form_num = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_form_max );
      if( 1 < form_num ) {
        pml::FormNo new_formno = ( old_formno + form_num + plus_minus ) % form_num;
        m_pPokeParam->ChangeFormNo( new_formno );
      }

      setupUiDisp();
      m_UiPokeParamChange = true;
      return RET_NONE;
    }
    
    u32 PokemonInframeTool::updateUi_Sex( s32 plus_minus )
    {
      pml::personal::SexType sex_type = m_pPokeParam->GetSexType();
      if( sex_type == pml::personal::SEX_TYPE_RANDOM ) {
        pml::Sex old_sex = m_pPokeParam->GetSex();
        pml::Sex new_sex = ( old_sex == pml::SEX_MALE ) ? pml::SEX_FEMALE : pml::SEX_MALE;
        m_pPokeParam->ChangeSex( new_sex );
      }

      setupUiDisp();
      m_UiPokeParamChange = true;
      return RET_NONE;
    }
    
    u32 PokemonInframeTool::updateUi_Egg( s32 plus_minus )
    {
      if( m_pPokeParam->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG ) ) {
        if( m_pPokeParam->GetMonsNo() != MONSNO_NULL )  // [niji注] nijiでいれた対策。MONSNO_NULLのときはタマゴフラグを立てたままにしておく。
        {
          m_pPokeParam->Birth();
        }
      }
      else {
        m_pPokeParam->SetEggFlag();
      }

      setupUiDisp();
      m_UiPokeParamChange = true;
      return RET_NONE;
    }

    u32 PokemonInframeTool::updateUi_Scale( s32 plus_minus )
    {
      m_fFrameScale += static_cast<f32>(plus_minus)*0.01f;
      return RET_NONE;
    }
    
    u32 PokemonInframeTool::updateUi_OffsetY( s32 plus_minus )
    {
      m_fFramePosiOfstRatioY += static_cast<f32>(plus_minus)*0.01f;
      return RET_NONE;
    }

    u32 PokemonInframeTool::updateUi_RotateY( s32 plus_minus )
    {
      m_fModelYaw += static_cast<f32>(plus_minus)*GFL_MATH_DEG_TO_RAD(2.0f);
      return RET_NONE;
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  ディスプレイの文字列更新
     */
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::setupUiDisp()
    {
      {
        MonsNo monsno = m_pPokeParam->GetMonsNo();
        pml::FormNo formno = m_pPokeParam->GetFormNo();
        pml::personal::LoadPersonalData( monsno, formno );

        // ポケモン名
        // ナンバー
        {
          pml::personal::GetMonsName( m_layout_string, monsno );
          setTextboxPaneString( PANENAME_UI_CAMERA_PANE_MONS_NAME, m_layout_string );

          print::StrNumber( m_layout_string, monsno, 3, print::NUM_DISP_ZERO, print::NUM_CODE_HANKAKU );
          setTextboxPaneString( PANENAME_UI_CAMERA_PANE_NO_VAL, m_layout_string );
        }

        // フォルム名
        {
          pml::FormNo max_formno = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_form_max );
          print::StrNumber( m_layout_string, formno, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
          setTextboxPaneString( PANENAME_UI_CAMERA_PANE_FORM_VAL, m_layout_string );
          print::StrNumber( m_layout_string, max_formno, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
          setTextboxPaneString( PANENAME_UI_CAMERA_PANE_FORM_MAX_VAL, m_layout_string );
        }

        // 性別
        {
          static const gfl2::str::STRCODE* SEX_NAME[ pml::SEX_NUM ] = 
          {
            L"♂",
            L"♀",
            L"？",
          };
          pml::Sex sex = m_pPokeParam->GetSex();
          setTextboxPaneString( PANENAME_UI_CAMERA_PANE_SEX_VAL, SEX_NAME[sex] );
        }

        // たまご
        {
          if( m_pPokeParam->IsEgg( pml::pokepara::CHECK_ONLY_ILLEGAL_EGG ) ) {
            setTextboxPaneString( PANENAME_UI_CAMERA_PANE_EGG_VAL, L"ダメ" );
          }
          else if( m_pPokeParam->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG ) ) {
            setTextboxPaneString( PANENAME_UI_CAMERA_PANE_EGG_VAL, L"○" );
          }
          else {
            setTextboxPaneString( PANENAME_UI_CAMERA_PANE_EGG_VAL, L"－" );
          }
        }
      }
    }

    void PokemonInframeTool::setTextboxPaneString( u32 pane_index, gfl2::str::StrBuf* p_buf )
    {
      gfl2::lyt::LytWk* pLytWk = m_pView->GetLayoutWork(0);  // 親であるapp::ui::UIViewのメンバ  // wk_0
      gfl2::lyt::LytTextBox* text_box = pLytWk->GetTextBoxPane( pane_index );
      GFL_ASSERT( text_box );

      if( text_box ) {
        print::SetStringForLytTextBox( text_box, p_buf );
      }
    }

    void PokemonInframeTool::setTextboxPaneString( u32 pane_index, const gfl2::str::STRCODE* p_buf )
    {
      gfl2::lyt::LytWk* pLytWk = m_pView->GetLayoutWork(0);  // 親であるapp::ui::UIViewのメンバ  // wk_0
      gfl2::lyt::LytTextBox* text_box = pLytWk->GetTextBoxPane( pane_index );
      GFL_ASSERT( text_box );

      if( text_box ) {
        text_box->SetString( p_buf );
      }
    }

    void PokemonInframeTool::setTextboxPaneString(u32 pane_index, f32 number, const bool bIsRadian)
    {
      static wchar_t StrWork[ 64 ];
      
      gfl2::lyt::LytWk* pLytWk = m_pView->GetLayoutWork(0);  // 親であるapp::ui::UIViewのメンバ  // wk_0
      gfl2::lyt::LytTextBox* text_box = pLytWk->GetTextBoxPane( pane_index );
      GFL_ASSERT( text_box );

      if( text_box )
      {
        if( bIsRadian ) swprintf(StrWork, 64, L"%05.1f", GFL_MATH_RAD_TO_DEG(number));
        else            swprintf(StrWork, 64, L"%05.3f", number);
        text_box->SetString(StrWork);
      }
    }

    //-----------------------------------------------------------------------------
    // 入力による更新を行う
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::UpdateByInput()
    {
      const gfl2::ui::DeviceManager* pUIDeviceManager = GetGameManager()->GetUiDeviceManager();
      const gfl2::ui::Button*       pButton = pUIDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
      const gfl2::ui::VectorDevice* pStick  = pUIDeviceManager->GetStick (gfl2::ui::DeviceManager::STICK_STANDARD);

      m_bIsTurnRightMode = pButton->IsHold(gfl2::ui::BUTTON_B);

      if( pButton->IsTrigger(gfl2::ui::BUTTON_R) )
      {
        // [niji注] ポケモンモデルに対するHIO操作がnijiにはないのでコメントアウトした。
        /*
        m_bIsHIO = !m_bIsHIO;
        m_pPokeModel->GetPokeModelSystem()->ChangeArcMode(m_pHeap, m_bIsHIO);
        m_UiPokeParamChange = true;
        */
      }

      if( pButton->IsTrigger(gfl2::ui::BUTTON_L) ) m_bIsDispHelp = !m_bIsDispHelp;

      if( pButton->IsTrigger(gfl2::ui::BUTTON_UP) == true )
      {
        int nDebugObjectLevel = m_eDebugObjectLevel;
        nDebugObjectLevel--;
        if( nDebugObjectLevel < DEBUG_OBJECT_LEVEL_0 ) nDebugObjectLevel = DEBUG_OBJECT_LEVEL_MAX;

        m_eDebugObjectLevel = static_cast<EDebugObjectLevel>(nDebugObjectLevel);
        // GFL_PRINT("Debug Object Level:%d \n", m_eDebugObjectLevel);
      }

      if( pButton->IsTrigger(gfl2::ui::BUTTON_DOWN) == true )
      {
        GFL_PRINT("ぽけもんいんふれーむ情報出力(おおきさ, Ｙオフセット, 回転, 右向きオフセット, Ｘ軸回転)\n");
        GFL_PRINT("%f\n%f\n%f\n%f\n%f\n", m_fFrameScale, m_fFramePosiOfstRatioY, GFL_MATH_RAD_TO_DEG(m_fModelYaw), GFL_MATH_RAD_TO_DEG(m_fModelYawOfstR), GFL_MATH_RAD_TO_DEG(m_pInModelInFrame->GetCameraPitch()));
      }

      if     ( pButton->IsHold(gfl2::ui::BUTTON_A) ) m_fFrameScale          += pStick->GetY()*0.002f;
      else if( pButton->IsHold(gfl2::ui::BUTTON_Y) ) m_fFramePosiOfstRatioY += pStick->GetY()*0.0014f;
      else if( pButton->IsHold(gfl2::ui::BUTTON_X) )
      {
#if 1
        m_pInModelInFrame->AddCameraPitch(-pStick->GetY()*0.003f);
#else
        const float MARGIN = 0.2f;
        m_inModelInFrame.AddCameraYaw  (field::CUtil::GetAddMarginInputValue(pStick->GetX(), MARGIN)*0.02f);
        m_inModelInFrame.AddCameraPitch(field::CUtil::GetAddMarginInputValue(pStick->GetY(), MARGIN)*0.02f);
#endif
      }
      else if( m_bIsTurnRightMode ) m_fModelYawOfstR += pStick->GetX()*0.003f;
      else
        m_fModelYaw += pStick->GetX()*0.01f;


      if( isPokeModelEnable() )
      {
        if( pButton->IsHold(gfl2::ui::BUTTON_Y) == true )
        {
          if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT) == true)
          {
            m_pPokeModel->SetAnimationStepFrame(0.0f);
          }
          else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT) == true)
          {
            m_pPokeModel->SetAnimationStepFrame(1.0f);
          }
        }
        else if( pButton->IsRepeat(gfl2::ui::BUTTON_LEFT) == true && m_pPokeModel->GetAnimationStepFrame()==0.0f)
        {
          m_fAnimeFrame += 1.0f;
          if( m_fAnimeFrame > m_pPokeModel->GetAnimationEndFrame())
          {
            m_fAnimeFrame = 0.0f;
          }
          m_pPokeModel->SetAnimationFrame( m_fAnimeFrame);
        }
        else if( pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT) == true && m_pPokeModel->GetAnimationStepFrame()==0.0f)
        {
          m_fAnimeFrame -= 1.0f;
          if( m_fAnimeFrame < 0.0f)
          {
            m_fAnimeFrame = m_pPokeModel->GetAnimationEndFrame();
          }
          m_pPokeModel->SetAnimationFrame( m_fAnimeFrame);
        }
      }
    }

    //-----------------------------------------------------------------------------
    // ヘルプを表示する
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::DispHelp(gfl2::gfx::CtrDisplayNo displayNo)
    {
      if( displayNo == gfl2::gfx::CtrDisplayNo::LEFT || displayNo == gfl2::gfx::CtrDisplayNo::RIGHT )
      {
        m_drawUtilText->ClearDrawText();
        m_drawUtilText->SetFont( print::SystemFont_GetFont() );
        m_drawUtilText->SetTextScale( 0.75f, 0.75f );

        const int x = 8;
        const int offset_y = 14;
        int y = 4;

        if( m_bIsHIO == true )
        {
          m_drawUtilText->SetTextColor( gfl2::math::ConvertNwVec4ToGfVec4(System::Camera::field::VEC4_COLOR_WHITE) );
          m_drawUtilText->DrawText(x, y, L"Rボタン：HIO [ON]"); y += offset_y;
        }
        else
        {
          m_drawUtilText->SetTextColor( gfl2::math::ConvertNwVec4ToGfVec4(System::Camera::field::VEC4_COLOR_THIN_WHITE) );
          m_drawUtilText->DrawText(x, y, L"Rボタン：HIO [OFF]"); y += offset_y;
          m_drawUtilText->SetTextColor( gfl2::math::ConvertNwVec4ToGfVec4(System::Camera::field::VEC4_COLOR_WHITE) );
        }

        m_drawUtilText->DrawText(x, y, L"右向きオフセット：%.1f", GFL_MATH_RAD_TO_DEG(m_fModelYawOfstR)); y += offset_y;
        m_drawUtilText->DrawText(x, y, L"Ｘ軸回転：%.1f", GFL_MATH_RAD_TO_DEG(m_pInModelInFrame->GetCameraPitch())); y += offset_y;
        m_drawUtilText->DrawText(x, y, L"モーションフレーム：%.1f", m_fAnimeFrame); y += offset_y;

        m_drawUtilText->DrawText(x, y, L"Lボタン：ヘルプ表示"); y += offset_y;
        if( m_bIsDispHelp == true )
        {
          m_drawUtilText->DrawText(x, y, L"十字キー上：デバッグオブジェクト表示"); y += offset_y;
          m_drawUtilText->DrawText(x, y, L"十字キー下：コンソール出力"); y += offset_y;
          m_drawUtilText->DrawText(x, y, L"アナログ左右：Ｙ軸回転");     y += offset_y;
          m_drawUtilText->DrawText(x, y, L"Ａ＋アナログ上下：おおきさ"); y += offset_y;
          m_drawUtilText->DrawText(x, y, L"Ｙ＋アナログ上下：Ｙオフセ"); y += offset_y;
          m_drawUtilText->DrawText(x, y, L"Ｂ＋アナログ左右：右オフセ"); y += offset_y;
          m_drawUtilText->DrawText(x, y, L"Ｘ＋アナログ上下：Ｘ軸回転"); y += offset_y;
          m_drawUtilText->DrawText(x, y, L"STARTボタン：終了");          y += offset_y;
          
          m_drawUtilText->DrawText(x, y, L"Ｙ＋十字キー左：モーション停止"); y += offset_y;
          m_drawUtilText->DrawText(x, y, L"Ｙ＋十字キー右：モーション再生"); y += offset_y;
          m_drawUtilText->DrawText(x, y, L"十字キー左右：モーションフレーム変更"); y += offset_y;
        }

        {
          gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
          gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
          u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
          u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );
         
          m_drawUtilTextRenderingPipeLine->SetRenderTarget( pBackBuffer );
          m_drawUtilTextRenderingPipeLine->SetDepthStencil( pDepthBuffer );
          m_drawUtilTextRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );
         
          m_drawUtilTextRenderingPipeLine->Execute();
        }
      }
    }


    //-----------------------------------------------------------------------------
    // びゅーわーで選択されているポケモンを取得・設定する
    //-----------------------------------------------------------------------------
    void PokemonInframeTool::GetSelectedPokemonInViewer(MonsNo* peMonsNo, pml::FormNo* pnFormNo, pml::Sex* peSex)
    {
      (*peMonsNo) = m_eSelectedPokemonMonsNo;
      (*pnFormNo) = m_nSelectedPokemonFormNo;
      (*peSex)    = m_eSelectedPokemonSex;
    }
    void PokemonInframeTool::SetSelectedPokemonInViewer(const MonsNo eMonsNo, const pml::FormNo nFormNo, const pml::Sex eSex)
    {
      m_eSelectedPokemonMonsNo = eMonsNo;
      m_nSelectedPokemonFormNo = nFormNo;
      m_eSelectedPokemonSex    = eSex;
    }


  } // namespace test
} // namespace Camera
} // namespace System

#endif // PM_DEBUG

/*  EOF  */





