//==============================================================================
/**
 * @file   gfl2_DebugLogWindow.cpp
 * @date   12/12/7
 * @date   2016/01/26 Tue. 15:25:47
 * @author ariizumi
 * @author muto_yu
 * @brief  デバッグログウィンドウ
 * @note   旧作より移植
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <stdarg.h>
#include  <stdio.h>

#include  <system/include/Date/gfl2_Date.h>
#include  <fs/include/gfl2_FsDirectSdmcFile.h>
#include  <debug/include/gfl2_DebugWinSystem.h>
#include  <str/include/gfl2_Font.h>

#include  <debug/include/gfl2_DebugLogWindow.h>
//#include  "arc/debug_message.gaix"



//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if USE_DEBUG_LOG_WIN
SINGLETON_INSTANCE_DEFINITION(gfl2::debug::DebugLogWindow)
#endif

namespace gfl2  {
namespace debug {
// =============================================================================
/**/
//==============================================================================
void InitDebugLogWindow(gfl2::heap::HeapBase *heap, gfl2::util::DrawUtilText* pTextUtil , gfl2::util::UtilTextRenderingPipeLine* pPipeLine )
{
#if USE_DEBUG_LOG_WIN
  DebugLogWindow *debLogWin = GFL_NEW(heap) DebugLogWindow(heap, pTextUtil,pPipeLine);
#endif
}

void LogWindow_SetEnable(bool flg, const LogWinID logIdx )
{
#if USE_DEBUG_LOG_WIN
  if( GFL_SINGLETON_INSTANCE(DebugLogWindow) == NULL )
    return;

  GFL_SINGLETON_INSTANCE(DebugLogWindow)->SetIsEnable(flg);

  if( logIdx != LOGWIN_ID_INVALID )
  {
    GFL_SINGLETON_INSTANCE(DebugLogWindow)->SetIsDispFlg(logIdx,flg);
  }

#endif
}

void LogWindow_SetDispFlg(bool flg, const LogWinID logIdx)
{
#if USE_DEBUG_LOG_WIN
  if( GFL_SINGLETON_INSTANCE(DebugLogWindow) == NULL )
    return;

  GFL_SINGLETON_INSTANCE(DebugLogWindow)->SetIsDispFlg(logIdx,flg);
#endif
}

void LogWindow_AddMsg(const LogWinID logIdx,const wchar_t *msg , ... )
{
#if USE_DEBUG_LOG_WIN
  if( GFL_SINGLETON_INSTANCE(DebugLogWindow) == NULL )
    return;

  wchar_t *tempMsg = GFL_SINGLETON_INSTANCE(DebugLogWindow)->GetTempBuf();

  va_list VA;
  va_start( VA, msg );
  vswprintf( tempMsg, DebugLogWindow::MSG_BUF_LEN, msg, VA );
  va_end( VA );

  GFL_SINGLETON_INSTANCE(DebugLogWindow)->AddMsg(logIdx,tempMsg);
#endif
}



void LogWindow_SetFont(gfl2::str::Font* pFont)
{
#if USE_DEBUG_LOG_WIN
  if( GFL_SINGLETON_INSTANCE(DebugLogWindow) == NULL )
    return;

  GFL_SINGLETON_INSTANCE(DebugLogWindow)->SetFont(pFont);
#endif
}


void LogWindow_SetFontScale(const f32 scaleH, const f32 scaleV)
{
#if USE_DEBUG_LOG_WIN
  if( GFL_SINGLETON_INSTANCE(DebugLogWindow) == NULL )
    return;

  GFL_SINGLETON_INSTANCE(DebugLogWindow)->SetFontScale(scaleH, scaleV);
#endif
}


void LogWindow_SetFontScaleDefault(void)
{
#if USE_DEBUG_LOG_WIN
  if( GFL_SINGLETON_INSTANCE(DebugLogWindow) == NULL )
    return;

  GFL_SINGLETON_INSTANCE(DebugLogWindow)->SetFontScale();
#endif
}




//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
#if USE_DEBUG_LOG_WIN
DebugLogWindow::DebugLogWindow(gfl2::heap::HeapBase* pHeap , gfl2::util::DrawUtilText* pTextUtil , gfl2::util::UtilTextRenderingPipeLine* pPipeLine )
  : m_FontInfo()
  , m_pTextUtil(NULL)
  , mFlgBit(pHeap,LOGWIN_ID_MAX)
  , mFlgSdBit(pHeap,LOGWIN_ID_MAX)
  , mStrBuf(gfl2::debug::DEBUGWIN_NAME_LEN,pHeap)
{
  GFL_ASSERT(pHeap);

  /*  render  */
  {
    //m_pHeapAllocator  = GFL_NEW(pHeap) gfl2::heap::NwAllocator( pHeap );
    m_pTextRenderPipe = pPipeLine;
    m_pTextUtil       = pTextUtil;

    //m_pTextUtil->Initialize( m_pHeapAllocator, m_pHeapAllocator, m_pTextRenderPipe->GetUtilTextRenderPath() );
  }


  mSdFile = NULL;
  mDispStart = 0;
  mDispEnd = 0;
  mFlgBit.OffAll();
  mFlgSdBit.OffAll();

  //デバッグウィンドウ用
  mPageIdx = 0;
//  mMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_d_logwin_label_DAT, pHeap , gfl2::str::MsgData::LOAD_FULL );
  mHeapMem = pHeap;

  for( int i=0;i<MSG_BUF_NUM;i++ )
  {
    mBuf[i].cnt = 0;
  }

  mDispPos = 0;
  mLifeTime = 500;

  //全部有効にする
  mIsEnable = true;
  SetIsDispFlg( LOGWIN_ID_POKE_MEMORY, true );
  SetIsDispFlg( LOGWIN_ID_POKE_FISHING, true );
  SetIsDispFlg( LOGWIN_ID_POKE_KINOMI, true );
  SetIsDispFlg( LOGWIN_ID_POKE_SAVE, true );
  SetIsDispFlg( LOGWIN_ID_WEATHER, true );
  SetIsDispFlg( LOGWIN_ID_SAKASA_SHINKA, true );
  SetIsDispFlg( LOGWIN_ID_BATTLE_VIEW, true );
  SetIsDispFlg( LOGWIN_ID_NEX, true );
  SetIsDispFlg( LOGWIN_ID_PGL, true );
  SetIsDispFlg( LOGWIN_ID_VALIDATION, true );
}


//------------------------------------------------------------------
/**
  *  @brief    dtor
  */
//------------------------------------------------------------------
DebugLogWindow::~DebugLogWindow()
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
void DebugLogWindow::Update(void)
{
  //SDカードを開く処理
  if( mSdFile == NULL && mIsEnableOutptSd )
  {
    char                fileName[256];
    const u32           fileNameMaxLen  = GFL_NELEMS(fileName)-1;
    gfl2::system::Date  currentDate;
    
    gfl2::system::Date::GetNow(&currentDate);

#if defined(GF_PLATFORM_WIN32)
    _snprintf_s(
#else
    snprintf(
#endif
      fileName,
#if defined(GF_PLATFORM_WIN32)
      sizeof(fileName),
#endif
      fileNameMaxLen,
      "LogWin_%02d%02d_%02d%02d%02d.log",
      currentDate.GetMonth(),
      currentDate.GetDay(),
      currentDate.GetHour(),
      currentDate.GetMinute(),
      currentDate.GetSecond()
    );

    mSdFile = gfl2::fs::System::CreateDirectSdmcFile(mHeapMem, fileName, gfl2::fs::Define::OpenMode::WRITE);
    if(mSdFile)
    {
      u16 bom = 0xFEFF;
      mSdFile->Write( &bom, sizeof(bom));
    }
    else
    {
      mIsEnableOutptSd = false;
    }
  }
  else
  if( mSdFile && mIsEnableOutptSd == false )
  {
    gfl2::fs::System::DestroyDirectSdmcFile(mSdFile);
    mSdFile = NULL;
  }

  if( gfl2::debug::DebugWin_IsOpen() )
  {
    return;
  }

  /*
    寿命計算
  */

#if 0
  int pos = mDispStart;

  while( pos != mDispEnd )
  {
    if( mBuf[pos].cnt != -1 )
    {
      mBuf[pos].cnt--;
      if( mBuf[pos].cnt == 0 )
      {
        mDispStart++;
      }
    }
    pos++;

    if( pos >= MSG_BUF_NUM )
    {
      pos -= MSG_BUF_NUM;
    }

  }

  if( mDispStart >= MSG_BUF_NUM )
  {
    mDispStart -= MSG_BUF_NUM;
  }
#else
  {
    const u32 indexStart  = mDispStart;


    for(u32 count = 0; count < MSG_BUF_NUM; ++count)
    {
      const u32 index = (indexStart + count) % MSG_BUF_NUM;

      if(index == mDispEnd) break;

      if(mBuf[index].cnt)
      {
        --mBuf[index].cnt;
      }

      if(!mBuf[index].cnt)
      {
        ++mDispStart;
        mDispStart  %= MSG_BUF_NUM;
      }
    }
  }
#endif
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
#if 0
void DebugLogWindow::Draw(gfl2::grp::GraphicsSystem *grpSys )
{
  if( mIsEnable == false )
  {
    return;
  }

  gfl2::grp::DisplayType dispType;
  if( mDispPos == 0 )
  {
    dispType = gfl2::grp::DISPLAY_UPPER;
  }
  else
  {
    dispType = gfl2::grp::DISPLAY_LOWER;
  }

  gfl2::grp::RenderTarget* renderTarget = grpSys->GetCurrentFrameBuffer(dispType);
  if( grpSys->BindCurrentCommandList( renderTarget ) )
  {
    //gfl2::grp::util::DrawUtil::DrawText( 0, 0, L"[%ls]", dispStr );
    //gfl2::grp::util::DrawUtil::DrawText( 0, 0, L"テスト" );
    gfl2::grp::util::DrawUtil::BeginRender( renderTarget );
    gfl2::grp::util::DrawUtil::MaterialInfo info;
    gfl2::grp::util::DrawUtil::SetMaterial( info );

    int dispXBase = 512;
    if( dispType == gfl2::grp::DISPLAY_LOWER )
    {
      dispXBase = 320;
    }
    static const f32 DISP_HEIGHT = 14.0f;
    static const gfl2::math::VEC4 SHADOW_COL(0,0,0,1);
    static const gfl2::math::VEC4 MESSAGE_COL(1,1,1,1);
    int dispPos = mDispStart;
    int dispY = 0;

    gfl2::grp::util::DrawUtil::SetTextScale(m_FontInfo.scaleH, m_FontInfo.scaleV);

    while( dispPos != mDispEnd )
    {
      if( GetIsDispFlg(mBuf[dispPos].logIdx) )
      {

        gfl2::grp::util::DrawUtil::SetTextColor( SHADOW_COL );
        gfl2::grp::util::DrawUtil::DrawText( dispXBase-mBuf[dispPos].strLen+1, dispY+1, mBuf[dispPos].str );
        gfl2::grp::util::DrawUtil::DrawText( dispXBase-mBuf[dispPos].strLen-1, dispY-1, mBuf[dispPos].str );

        gfl2::grp::util::DrawUtil::SetTextColor( MESSAGE_COL );
        gfl2::grp::util::DrawUtil::DrawText( dispXBase-mBuf[dispPos].strLen  , dispY  , mBuf[dispPos].str );
        dispY += DISP_HEIGHT;
      }
      dispPos++;
      if( dispPos >= MSG_BUF_NUM )
      {
        dispPos = 0;
      }
    }

    gfl2::grp::util::DrawUtil::EndRender();
  }
}
#endif


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DebugLogWindow::Draw( gfl2::gfx::CtrDisplayNo dispNo, gfl2::gfx::DisplayDeviceHandle handle )
{
  if( mIsEnable == false )
  {
    return;
  }

  int dispXBase = 400;

  /*  filtering */
  {
    bool  isAbort = false;

    switch(dispNo)
    {
      case  gfl2::gfx::CtrDisplayNo::LEFT:    /*  fallthrough */
      case  gfl2::gfx::CtrDisplayNo::RIGHT:
        dispXBase = 400;
        isAbort = !(mDispPos == 0);
        break;

      case  gfl2::gfx::CtrDisplayNo::DOWN:
        dispXBase = 320;
        isAbort = !(mDispPos != 0);
        break;
    }

    isAbort |=  (!m_FontInfo.pFont);

    if(isAbort) return;
  }

  m_pTextUtil->SetRenderPath( m_pTextRenderPipe->GetUtilTextRenderPath() );

  /*  draw  */
  {
    static const gfl2::math::Vector4 SHADOW_COL(0,0,0,1);
    static const gfl2::math::Vector4 MESSAGE_COL(1,1,1,1);

    int dispPos = mDispStart;
    int dispY = 0;

    /*  setup  */
    m_pTextUtil->ClearDrawText();
    m_pTextUtil->SetFont(m_FontInfo.pFont);
    m_pTextUtil->SetTextScale(m_FontInfo.scaleH, m_FontInfo.scaleV);

    while( dispPos != mDispEnd )
    {
      if( GetIsDispFlg(mBuf[dispPos].logIdx) )
      {
        /*  shadow  */
        m_pTextUtil->SetTextColor(SHADOW_COL);
        m_pTextUtil->DrawText( dispXBase-mBuf[dispPos].strLen+1, dispY+1, mBuf[dispPos].str );
        m_pTextUtil->DrawText( dispXBase-mBuf[dispPos].strLen-1, dispY-1, mBuf[dispPos].str );

        /*  text  */
        m_pTextUtil->SetTextColor(MESSAGE_COL);
        m_pTextUtil->DrawText( dispXBase-mBuf[dispPos].strLen  , dispY  , mBuf[dispPos].str );
        dispY += mBuf[dispPos].strV;
      }
      ++dispPos;
      dispPos %= MSG_BUF_NUM;
    }
  }


  /*  run pipeline  */
  {
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
    const u32           displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
    const u32           displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );

    m_pTextRenderPipe->SetRenderTarget( pBackBuffer );
    m_pTextRenderPipe->SetDepthStencil( pDepthBuffer );
    m_pTextRenderPipe->SetDisplaySize( displayWidth, displayHeight );

    m_pTextRenderPipe->Execute();
  }

}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void DebugLogWindow::ClearLog(void)
{
  mDispStart = mDispEnd;
}


void DebugLogWindow::AddMsg( const LogWinID logIdx, const wchar_t *msg , ...)
{
  if( mIsEnable == false )
  {
    return;
  }

  //一応クリア
  gfl2::std::MemClear( mBuf[mDispEnd].str , sizeof(mBuf[mDispEnd].str) );

  va_list VA;
  va_start( VA, msg );
  vswprintf( mBuf[mDispEnd].str, MSG_BUF_CHR_MAX, msg, VA );
  va_end( VA );

  //mBuf[mDispEnd].str[MSG_BUF_LEN-1] = 0;
  mBuf[mDispEnd].cnt = mLifeTime;
  mBuf[mDispEnd].logIdx = logIdx;

  //文字列長さの計算
  /**
    @note
    Yu Muto
    nw::font::TextWriterBase::CalcStringWidth / CalcFormatStringWidth などを使用すれば、文字列の描画サイズを計算できる
    また、AddMsgやFontScale変更時などdirtyなときに再計算するのが良い
  */
  {
    gfl2::str::Font*  font = m_FontInfo.pFont;

    if(font)
    {
      mBuf[mDispEnd].strLen = 10;
//      mBuf[mDispEnd].strV   = static_cast<f32>(font->GetHeight()) * m_FontInfo.scaleV;
      mBuf[mDispEnd].strV   = static_cast<f32>(font->GetLineFeed()) * m_FontInfo.scaleV;

      for( int i=0;i<MSG_BUF_LEN;i++ )
      {
        if( mBuf[mDispEnd].str[i] != L'\0' )
        {
#if defined(GF_PLATFORM_WIN32)
  #ifdef GetCharWidth
    #undef  GetCharWidth    /**<  @note Muto: wingdi.hで#defineされているため、無効化  */
  #endif // GetCharWidth
#endif
          mBuf[mDispEnd].strLen +=  font->GetCharWidth( mBuf[mDispEnd].str[i] ) * m_FontInfo.scaleH;
        }
        else
        {
          break;
        }
      }
    }
  }

  if( GetIsOutputSdFlg(logIdx) )
  {
    OutputSdLog( mBuf[mDispEnd].str );
  }

#if 0
  mDispEnd++;
  if( mDispEnd >= MSG_BUF_NUM )
  {
    mDispEnd = 0;
  }

  if( mDispEnd == mDispStart )
  {
    mDispStart++;
    if( mDispStart >= MSG_BUF_NUM )
    {
      mDispStart = 0;
    }
  }
#else
  ++mDispEnd;
  mDispEnd  %= MSG_BUF_NUM;

  /*  書き込み位置が先頭に追いついた場合  */
  /**
    @note
    Muto
    この実装では、有効行がMSG_BUF_LEN行であった場合最後の行が(((mDispStart + MSG_BUF_LEN) % MSG_BUF_LEN) == mDispEnd)となるため、
    (MSG_BUF_LEN-1)行しか描画できない
  */
  if(mDispEnd == mDispStart)
  {
    ++mDispStart;
    mDispStart %= MSG_BUF_NUM;
  }
#endif
}

bool DebugLogWindow::GetIsDispFlg( const LogWinID idx )
{
  return mFlgBit.GetBit(idx);
}

void DebugLogWindow::SetIsDispFlg( const LogWinID idx , const bool flg )
{
  if( flg )
  {
    mFlgBit.On(idx);
  }
  else
  {
    mFlgBit.Off(idx);
  }
}


bool DebugLogWindow::GetIsOutputSdFlg( const LogWinID idx )
{
  return mFlgSdBit.GetBit(idx);
}

void DebugLogWindow::SetIsOutputSdFlg( const LogWinID idx , const bool flg )
{
  if( flg )
  {
    mFlgSdBit.On(idx);
  }
  else
  {
    mFlgSdBit.Off(idx);
  }
}

void DebugLogWindow::OutputSdLog( const wchar_t *msg , ... )
{
  if( mSdFile )
  {
    /**   */
    wchar_t           tmp[1024];
    static const u32  maxStrLen = GFL_NELEMS(tmp)-1;

    va_list VA;
    va_start( VA, msg);
    vswprintf( tmp, maxStrLen, msg, VA );
    va_end( VA );

    if( mOutputSdTime )
    {
      wchar_t             date[64];
      const u32           dateMaxLen  = GFL_NELEMS(date)-1;
      gfl2::system::Date  currentDate;
      

      gfl2::system::Date::GetNow(&currentDate);

      swprintf(
        date,
        dateMaxLen,
        L"%02d:%02d:%02d ",
        currentDate.GetHour(),
        currentDate.GetMinute(),
        currentDate.GetSecond()
      );
      mSdFile->Write( date, ::wcslen(date) * sizeof(wchar_t) );
    }

    mSdFile->Write( tmp, ::wcslen(tmp) * sizeof(wchar_t) );

    wchar_t cr = '\n';
    mSdFile->Write( &cr, sizeof(cr));
  }
}

#if 0
const wchar_t* DebugLogWindow::GetDebugWinMsgData(int idx)
{
  mMsgData->GetString( idx , mStrBuf );
  return mStrBuf.GetPtr();
}
#endif

#endif //USE_DEBUG_LOG_WIN

} /*  namespace debug */
} /*  namespace gfl2  */

