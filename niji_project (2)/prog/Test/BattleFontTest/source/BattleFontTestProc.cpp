#if PM_DEBUG

//======================================================================
/**
 * @file    BattleFontTestProc.cpp
 * @date    2016/10/03 12:00:30
 * @author  fukushima_yuya
 * @brief   バトルフォントテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <Test/BattleFontTest/include/BattleFontTestProc.h>
#include <System/include/nijiAllocator.h>
#include <System/include/HeapDefine.h>
#include <System/include/GflUse.h>
#include <Print/include/PrintSystem.h>

// exec
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(BattleFont)

BattleFontTestProc::BattleFontTestProc( void )
  : m_pMemAllocator(NULL)
  , m_pDevAllocator(NULL)
  , m_pNijiAllocator(NULL)
  , m_pDrawUtilText(NULL)
  , m_pRenderingPipeLine(NULL)
  , m_pDeviceManager(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager())
  , m_pTempStrBuf(NULL)
  , m_ProcResult(gfl2::proc::RES_CONTINUE)
  , m_MainSeq(0)
  , m_SubSeq(0)
  , m_StartX(0)
  , m_PageNum(0)
{
  for( u32 i=0; i<LANGUAGE_MAX; ++i )
  {
    m_pMsgData[i] = NULL;
  }
  for( u32 i=0; i<LINE_MAX; ++i )
  {
    m_pStrBuf[i] = NULL;
  }
}


gfl2::proc::Result BattleFontTestProc::InitFunc( gfl2::proc::Manager* manager )
{
  bool (BattleFontTestProc::*CreateFuncTable[])( void ) = {
    &BattleFontTestProc::CreateAllocator,
    &BattleFontTestProc::CreateRenderingPipeLine,
    &BattleFontTestProc::CreateBattleFont,
    &BattleFontTestProc::CreateDrawUtilText,
    &BattleFontTestProc::CreateMsgData,
    &BattleFontTestProc::CreateStrBuf,
  };

  if( (this->*CreateFuncTable[m_MainSeq])() )
  {
    m_MainSeq++;

    if( m_MainSeq == GFL_NELEMS( CreateFuncTable ) )
    {
      m_MainSeq = 0;
      return gfl2::proc::RES_FINISH;
    }
  }

  return gfl2::proc::RES_CONTINUE;
}

gfl2::proc::Result BattleFontTestProc::EndFunc( gfl2::proc::Manager* manager )
{
  bool (BattleFontTestProc::*DeleteFuncTable[])() = {
    &BattleFontTestProc::DeleteStrBuf,
    &BattleFontTestProc::DeleteMsgData,
    &BattleFontTestProc::DeleteDrawUtilText,
    &BattleFontTestProc::DeleteBattleFont,
    &BattleFontTestProc::DeleteRenderingPipeLine,
    &BattleFontTestProc::DeleteAllocator,
  };

  if( (this->*DeleteFuncTable[m_MainSeq])() )
  {
    m_MainSeq++;

    if( m_MainSeq == GFL_NELEMS( DeleteFuncTable ) )
    {
      m_MainSeq = 0;
      return gfl2::proc::RES_FINISH;
    }
  }

  return gfl2::proc::RES_CONTINUE;
}

gfl2::proc::Result BattleFontTestProc::UpdateFunc( gfl2::proc::Manager* manager )
{
  DrawText();

  InputButton();

  return m_ProcResult;
}

void BattleFontTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo dispNo )
{
  switch( dispNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:
  case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      gfl2::gfx::Surface* backBuffer  = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ dispNo ] );
      gfl2::gfx::Surface* depthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ dispNo ] );
      u32                 dispWidth   = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ dispNo ] );
      u32                 dispHeight  = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ dispNo ] );

      m_pRenderingPipeLine->SetRenderTarget( backBuffer );
      m_pRenderingPipeLine->SetDepthStencil( depthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( dispWidth, dispHeight );

      m_pRenderingPipeLine->Execute();
    } break;
  }
}



bool BattleFontTestProc::CreateAllocator( void )
{
  gfl2::heap::HeapBase* sysHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  m_pMemAllocator  = GFL_NEW(sysHeap) gfl2::heap::NwAllocator( sysHeap );
  GFL_ASSERT( m_pMemAllocator );

  m_pDevAllocator  = GFL_NEW(devHeap) gfl2::heap::NwAllocator( devHeap );
  GFL_ASSERT( m_pDevAllocator );

  m_pNijiAllocator = GFL_NEW(devHeap) System::nijiAllocator( devHeap );
  GFL_ASSERT( m_pNijiAllocator );

  return true;
}

bool BattleFontTestProc::DeleteAllocator( void )
{
  GFL_SAFE_DELETE( m_pNijiAllocator );
  GFL_SAFE_DELETE( m_pDevAllocator );
  GFL_SAFE_DELETE( m_pMemAllocator );

  return true;
}


bool BattleFontTestProc::CreateRenderingPipeLine( void )
{
  gfl2::heap::HeapBase* devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  m_pRenderingPipeLine  = GFL_NEW(devHeap) gfl2::util::UtilTextRenderingPipeLine( m_pNijiAllocator, 512, 256 );
  GFL_ASSERT( m_pRenderingPipeLine );

  return true;
}

bool BattleFontTestProc::DeleteRenderingPipeLine( void )
{
  if( m_pRenderingPipeLine )
  {
    m_pRenderingPipeLine->GetUtilTextRenderPath()->ClearDrawableText();

    GFL_DELETE( m_pRenderingPipeLine );
  }

  return true;
}


bool BattleFontTestProc::CreateBattleFont( void )
{
  gfl2::heap::HeapBase* sysHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // バトルフォントの生成と追加
  print::SystemFont_CreateBattleFont( sysHeap, devHeap );
  print::SystemFont_SetLytSystemBattleFont();

  return true;
}

bool BattleFontTestProc::DeleteBattleFont( void )
{
  // バトルフォントの解放
  print::SystemFont_ResetDefaultFontInfo();

  return true;
}


bool BattleFontTestProc::CreateDrawUtilText( void )
{
  gfl2::heap::HeapBase* devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  m_pDrawUtilText = GFL_NEW(devHeap) gfl2::util::DrawUtilText( m_pMemAllocator, m_pDevAllocator, 2, LINE_MAX * 128, LINE_MAX, 128 );
  GFL_ASSERT( m_pDrawUtilText );

  m_pDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
  m_pDrawUtilText->SetFont( print::SystemFont_GetBattleFont() );

  return true;
}

bool BattleFontTestProc::DeleteDrawUtilText( void )
{
  if( m_pDrawUtilText )
  {
    m_pDrawUtilText->Terminate();
    GFL_DELETE( m_pDrawUtilText );
  }

  return true;
}


bool BattleFontTestProc::CreateMsgData( void )
{
  const System::MSGLANGID c_LangIDList[] = {
    System::MSGLANGID_KANA,
    System::MSGLANGID_ENGLISH,
    System::MSGLANGID_FRENCH,
    System::MSGLANGID_ITALIAN,
    System::MSGLANGID_GERMAN,
    System::MSGLANGID_SPANISH,
    System::MSGLANGID_KOREAN,
    System::MSGLANGID_CHINA,
    System::MSGLANGID_TAIWAN,
  };

  gfl2::heap::HeapBase* devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  for( u32 i=0; i<LANGUAGE_MAX; ++i )
  {
    m_pMsgData[i] = GFL_NEW(devHeap) gfl2::str::MsgData(
                                          print::GetMessageArcId(c_LangIDList[i]),
                                          GARC_message_monsname_DAT, devHeap, gfl2::str::MsgData::LOAD_FULL );
    GFL_ASSERT( m_pMsgData[i] );
  }

  return true;
}

bool BattleFontTestProc::DeleteMsgData( void )
{
  for( u32 i=0; i<LANGUAGE_MAX; ++i )
  {
    GFL_SAFE_DELETE( m_pMsgData[i] );
  }

  return true;
}


bool BattleFontTestProc::CreateStrBuf( void )
{
  gfl2::heap::HeapBase* devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  for( u32 i=0; i<LINE_MAX; ++i )
  {
    m_pStrBuf[i] = GFL_NEW(devHeap) gfl2::str::StrBuf( 256, devHeap );
  }

  m_pTempStrBuf = GFL_NEW(devHeap) gfl2::str::StrBuf( System::STRLEN_POKEMON_NAME + 1, devHeap );

  return true;
}

bool BattleFontTestProc::DeleteStrBuf( void )
{
  GFL_SAFE_DELETE( m_pTempStrBuf );

  for( u32 i=0; i<LINE_MAX; ++i )
  {
    GFL_SAFE_DELETE( m_pStrBuf[i] );
  }

  return true;
}



void BattleFontTestProc::InputButton( void )
{
  gfl2::ui::Button* button = m_pDeviceManager->GetButton(0);

  if( button->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_ProcResult = gfl2::proc::RES_FINISH;
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_StartX >= 0 )
    {
      m_StartX = 0;
    }
    else {
      m_StartX += 50;
    }
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_StartX <= -300 )
    {
      m_StartX = -300;
    }
    else {
      m_StartX -= 50;
    }
  }
  else if( button->IsRepeat( gfl2::ui::BUTTON_L ) )
  {
    if( m_PageNum > 0 )
    {
      m_PageNum--;
    }

    GFL_PRINT("PageNum=[%d]\n", m_PageNum);
  }
  else if( button->IsRepeat( gfl2::ui::BUTTON_R ) )
  {
    if( m_PageNum < (MONSNO_MAX / LINE_MAX) )
    {
      m_PageNum++;
    }

    GFL_PRINT("PageNum=[%d]\n", m_PageNum);
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    m_StartX = 0;
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    m_PageNum = (MONSNO_MAX / LINE_MAX);
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    m_PageNum = 0;
  }
}


void BattleFontTestProc::DrawText( void )
{
  m_pDrawUtilText->ClearDrawText();

  const gfl2::str::STRCODE* delimiter = L" ";

  for( u32 i=0; i<LINE_MAX; ++i )
  {
    m_pStrBuf[i]->Clear();

    u32 monsID = i + m_PageNum * LINE_MAX;

    if( monsID > MONSNO_END )
    {
      break;
    }

    // 各言語の名前を取得
    for( u32 j=0; j<LANGUAGE_MAX; ++j )
    {
      // ' '区切り文字を追加
      m_pStrBuf[i]->AddCode( *delimiter );
      // 名前を取得
      m_pMsgData[j]->GetString( monsID, *m_pTempStrBuf );
      // 連結
      m_pStrBuf[i]->AddStr( *m_pTempStrBuf );
    }

    m_pDrawUtilText->DrawText( m_StartX, i * 14, m_pStrBuf[i]->GetPtr() );
  }
}

GFL_NAMESPACE_END(BattleFont)
GFL_NAMESPACE_END(Test)

#endif  // PM_DEBUG
