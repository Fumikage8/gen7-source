
#if PM_DEBUG

//==============================================================================
/**
 * @file   MutoTest_TestProc_GiftDemo.cpp
 * @date   2016/03/14 Mon. 12:22:05
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// ============================================================================
/*	includes	*/
// ============================================================================
#include  <heap/include/gfl2_Heap.h>
#include  <System/include/HeapDefine.h>
#include  <GameSys/include/DllProc.h>

#include  <pml/include/pokepara/pml_PokemonParam.h>
#include  <app/AppDemo/include/AppDemoProc.h>
#include  "../include/Mutotest_TestProc_GiftDemo.h"



// ============================================================================
/*  defs  */
// ============================================================================


// ============================================================================
/*  local prms  */
// ============================================================================
namespace	{
  static const int    frameMax  = 30;


  enum  {
    SUBSEQ_Initial,

    SUBSEQ_Select,
    SUBSEQ_Setup,
    SUBSEQ_Demo,

    SUBSEQ_Cleanup,
    SUBSEQ_Exit,
    
    SUBSEQ_
  };



}


/*
  Test::MutoTest
*/
namespace Test      {
namespace MutoTest  {
// ============================================================================
/*  funcs */
// ============================================================================

//-----------------------------------------------------------------------------
/**
 *  @func   TestProc_GiftDemo
 *  @brief  ctor
 */
//-----------------------------------------------------------------------------
TestProc_GiftDemo::TestProc_GiftDemo(void)
  : GameSys::GameProc()
  , m_Heap()
  , m_TextDrawer()
  /*    */
  , m_pDemoProc(NULL)
  , m_MonsNo(MONSNO_NULL)
  , m_IsEgg(false)
  , m_DemoType(0)
  , m_pPokepara(NULL)
  , m_pStrBuf(NULL)
{
  m_pParentPokepara[0]  = NULL;
  m_pParentPokepara[1]  = NULL;
}


//-----------------------------------------------------------------------------
/**
 *  @func   ~TestProc_GiftDemo
 *  @brief  dtor
 */
//-----------------------------------------------------------------------------
TestProc_GiftDemo::~TestProc_GiftDemo()
{
}


//-----------------------------------------------------------------------------
/**
 *  @func     InitFunc
 *  @brief    PROCの初期化を行う関数
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result	TestProc_GiftDemo::InitFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;


  CreateHeap();

  m_pStrBuf = GFL_NEW(m_Heap.GetDeviceHeap()) gfl2::str::StrBuf(16, m_Heap.GetDeviceHeap());
  m_TextDrawer.Initialize(&m_Heap);
  m_TextDrawer.Scale(0.8f);



  SetSubSeq(SUBSEQ_Initial);

  result	= gfl2::proc::RES_FINISH;
  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
gfl2::proc::Result TestProc_GiftDemo::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;


  GFL_SAFE_DELETE(m_pStrBuf);
  m_TextDrawer.Finalize();
  DeleteHeap();

  result	= gfl2::proc::RES_FINISH;
  return result;
}


//-----------------------------------------------------------------------------
/**
  * @func UpdateFunc
  * @brief	PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//-----------------------------------------------------------------------------
gfl2::proc::Result  TestProc_GiftDemo::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result        result          = gfl2::proc::RES_CONTINUE;
  gfl2::ui::DeviceManager*  pDeviceManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button*         pButton         = pDeviceManager->GetButton(0);
  gfl2::heap::HeapBase*     pHeapBase       = m_Heap.GetDeviceHeap();
  


  m_TextDrawer.CLS();
  m_TextDrawer.PrintLn(L"GiftDemoTest");


  switch(GetSubSeq())
  {
    case  SUBSEQ_Initial:
      m_TextDrawer.PrintLn(L"initial");
      GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA );
      SetSubSeq(SUBSEQ_Select);
      break;

    case  SUBSEQ_Select:
    {
      m_TextDrawer.PrintLn(L"select");

      const u32 monsNoOffset  = pButton->IsHold(gfl2::ui::BUTTON_L) ? 10 : 1;

      if(     pButton->IsRepeat(gfl2::ui::BUTTON_UP))     m_MonsNo    = (m_MonsNo + MONSNO_END-monsNoOffset) % MONSNO_END;
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_DOWN))   m_MonsNo    = (m_MonsNo + monsNoOffset)            % MONSNO_END;
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))   m_DemoType  = (m_DemoType + App::AppDemo::AppDemoProcParam_Gift::TYPE_MAX-1) % App::AppDemo::AppDemoProcParam_Gift::TYPE_MAX;
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))  m_DemoType  = (m_DemoType + 1)  % App::AppDemo::AppDemoProcParam_Gift::TYPE_MAX;
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_R))     m_IsEgg     = !m_IsEgg;
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_A))     SetSubSeq(SUBSEQ_Setup);
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_B))     SetSubSeq(SUBSEQ_Cleanup);


      if(m_MonsNo == MONSNO_NULL) ++m_MonsNo;


      m_TextDrawer.CRLF();
      m_TextDrawer.PrintLn(L"  MonsNo [U / D] : %d", m_MonsNo);
      m_TextDrawer.PrintLn(L"  isEgg  [RB]    : %s", m_IsEgg ? L"Y" : L"N");
      m_TextDrawer.PrintLn(L"  type   [L / R] : %d", m_DemoType);
      m_TextDrawer.PrintLn(L"  start  [A]");
      m_TextDrawer.PrintLn(L"  exit   [B]");
      m_TextDrawer.CRLF();
      m_TextDrawer.PrintLn(L"  type:");
      m_TextDrawer.PrintLn(L"    0: poke, sp,    gold");
      m_TextDrawer.PrintLn(L"    1: poke, norm,  silv");
      m_TextDrawer.PrintLn(L"    2: poke, norm,  brnz");
      m_TextDrawer.PrintLn(L"    3: item, sp,    gold");
      m_TextDrawer.PrintLn(L"    4: item, norm,  silv");
      m_TextDrawer.PrintLn(L"    5: item, norm,  brnz");
    }
      break;

    case  SUBSEQ_Setup:
    {
      m_TextDrawer.PrintLn(L"setup");

      GFL_SAFE_DELETE(m_pParentPokepara[0]);
      GFL_SAFE_DELETE(m_pParentPokepara[1]);
      GFL_SAFE_DELETE(m_pPokepara);
      if(m_IsEgg)
      {
        pml::pokepara::EggGenerator::Setting  setting;
        gfl2::math::Random                    random;

        m_pParentPokepara[0]  = GFL_NEW(pHeapBase) pml::pokepara::PokemonParam( pHeapBase, static_cast<MonsNo>(m_MonsNo), 1, 0ULL);
        m_pParentPokepara[1]  = GFL_NEW(pHeapBase) pml::pokepara::PokemonParam( pHeapBase, static_cast<MonsNo>(m_MonsNo), 1, 0ULL);

        m_pParentPokepara[0]->ChangeSex(pml::SEX_MALE);
        m_pParentPokepara[1]->ChangeSex(pml::SEX_FEMALE);

        random.Initialize();

        setting.IDNo                    = 0;
        setting.parentName              = m_pStrBuf;
        setting.haveItem_HIKARUOMAMORI  = false;
        setting.pRandom                 = &random;
        setting.realRegionFormNo        = 0;

        m_pPokepara           = pml::pokepara::EggGenerator::CreateEgg(
                                pHeapBase,
                                m_pParentPokepara[0],
                                m_pParentPokepara[1],
                                setting
                              );
      }
      else
      {
        m_pPokepara = GFL_NEW(pHeapBase) pml::pokepara::PokemonParam( pHeapBase, static_cast<MonsNo>(m_MonsNo), 1, 0);
//        m_pPokepara->ChangeFormNo(formNo);
      }


      {
        App::AppDemo::AppDemoProcParam_Gift  param;

        param.pPokemonParams[0]   = m_pPokepara;
        param.type                = static_cast<App::AppDemo::AppDemoProcParam_Gift::Type>(m_DemoType);

        m_pDemoProc = GameSys::CallAppDemoProc_Gift( &param );
      }

      SetSubSeq(SUBSEQ_Demo);
    }
      break;

    case  SUBSEQ_Demo:
      m_TextDrawer.PrintLn(L"demo");

      if( !pManager->IsRegisterProcess(m_pDemoProc) )
      {
        GFL_SAFE_DELETE(m_pParentPokepara[0]);
        GFL_SAFE_DELETE(m_pParentPokepara[1]);
        GFL_SAFE_DELETE(m_pPokepara);

        GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA );
        SetSubSeq(SUBSEQ_Select);
      }
      break;

    case  SUBSEQ_Cleanup:
    {
      m_TextDrawer.PrintLn(L"cleanup");

      if(true)
      {
        SetSubSeq(SUBSEQ_Exit);
      }
    }
      break;

    case  SUBSEQ_Exit:
      m_TextDrawer.PrintLn(L"exit");
      result  = gfl2::proc::RES_FINISH;
      break;
  }


  return result;
}


//-----------------------------------------------------------------------------
/**
  * @func   PreDrawFunc
  * @brief  PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
//-----------------------------------------------------------------------------
void  TestProc_GiftDemo::PreDrawFunc( gfl2::proc::Manager* pManager )
{
}


//------------------------------------------------------------------
/**
  * @func   DrawFunc
  * @brief  PROCの描画処理
  * @param  pManager   プロセス管理マネージャー
  * @param  displayNo  描画対象画面
  */
//------------------------------------------------------------------
void  TestProc_GiftDemo::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
#if 0
  ../../../../../gflib2/gfx/include/gfl2_enums.h

  //! @brief CTRのディスプレイ番号
  DECLARE_ENUM_BEGIN( CtrDisplayNo )

    LEFT,           //!< 3DSの左画面 400x240
    RIGHT,          //!< 3DSの右画面 400x240
    DOWN,           //!< 3DSの下画面 320x240
    
  	NumberOf,

  DECLARE_ENUM_END( CtrDisplayNo )
#endif
  m_TextDrawer.Draw(displayNo);
}





/*
*/
void TestProc_GiftDemo::CreateHeap( void )
{
  gfl2::heap::HeapBase*   pAppHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase*   pDevHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  const size_t            appHeapSize   = 0x00080000;
  const size_t            devHeapSize   = 0x00180000;

  m_Heap.LocalHeapCreate( pAppHeapBase, pDevHeapBase, appHeapSize, devHeapSize);
}


/*
*/
void TestProc_GiftDemo::DeleteHeap( void )
{
  m_Heap.Delete();
}




} /*  namespace MutoTest  */
} /*  namespace Test      */

#endif // PM_DEBUG
