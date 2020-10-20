

#if PM_DEBUG

//==============================================================================
/**
  *  @file   TextDrawer.cpp
  *  @brief  CTR専用デバッグ用テキスト描画
  *  @date   2015.06.11 Thu.
  *  @author Yu Muto
  */
//==============================================================================




// =============================================================================
/**/
//==============================================================================
#include  <system/include/GflUse.h>
#include  <print/include/SystemFont.h>
#include  <AppLib/include/Util/app_util_heap.h>
#include  "./TextDrawer.h"


// =============================================================================
/**/
//==============================================================================
namespace {
  static const f32  TextRenderPipeScreenW = 512.0f;
  static const f32  TextRenderPipeScreenH = 256.0f;
  static const u32  TempStrBufSize        = 256;
  static const u32  TempCharMax           = TempStrBufSize-1;
  static const f32  DefaultScale          = 1.0f;
}










namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
TextDrawer::TextDrawer(void)
  : m_pTextRenderPipe(NULL)
  , m_pTextUtil(NULL)
  , m_Color(1.0f, 1.0f, 1.0f, 1.0f)
  , m_FontH(0)
{
  m_Pos.x = m_Pos.y = 0;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
TextDrawer::~TextDrawer()
{
  Finalize();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TextDrawer::Initialize(app::util::Heap* pHeap)
{
  gfl2::heap::HeapBase*     pHeapBase   = pHeap->GetDeviceHeap();
  gfl2::heap::NwAllocator*  pAllocator  = pHeap->GetDeviceAllocator();

  Finalize();
  m_pTextRenderPipe = GFL_NEW(pHeapBase) gfl2::util::UtilTextRenderingPipeLine( pHeap->GetDevGLHeapAllocator(), 512, 256 );
  m_pTextUtil       = GFL_NEW(pHeapBase) gfl2::util::DrawUtilText(pAllocator, pAllocator); 
  m_pTextUtil->Initialize( pAllocator, pAllocator, m_pTextRenderPipe->GetUtilTextRenderPath() );
  m_pTextUtil->SetFont(print::SystemFont_GetFont());

  Scale(DefaultScale);

}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TextDrawer::Finalize(void)
{
  if(m_pTextUtil) {
    m_pTextUtil->Terminate();
  }
  GFL_SAFE_DELETE(m_pTextUtil);
  GFL_SAFE_DELETE(m_pTextRenderPipe);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TextDrawer::CLS(void)
{
  m_pTextUtil->ClearDrawText();
  LOC(0,0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
int  TextDrawer::Print(const wchar_t* pFormat, ...)
{
  int outChars  = 0;

  va_list valist;
  va_start( valist, pFormat );

  outChars  = Print(pFormat, valist);

  va_end( valist );
  
  return outChars;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
int  TextDrawer::Print(const wchar_t* pFormat, va_list valist)
{
  int outChars  = 0;

  wchar_t   strBuf[TempStrBufSize] = {L'0'};
  vswprintf( strBuf, TempCharMax, pFormat, valist );
  outChars  = wcslen(strBuf);
  if(outChars)
  {
    m_pTextUtil->DrawText(m_Pos.x, m_Pos.y, strBuf, m_Color, m_Color);
    LOC(m_Pos.x / m_FontH + outChars, m_Pos.y);
  }

  return outChars;
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
int  TextDrawer::PrintLn(const wchar_t* pFormat, ...)
{
  int outChars  = 0;

  va_list valist;
  va_start( valist, pFormat );

  outChars  = Print(pFormat, valist);
  CRLF();

  va_end( valist );

  return outChars;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TextDrawer::Scale(const f32 scale)
{
  m_pTextUtil->SetTextScale( scale, scale );
  m_FontH = print::SystemFont_GetFont()->GetHeight() * scale;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  TextDrawer::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
    case gfl2::gfx::CtrDisplayNo::LEFT:
    case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      const u32           displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      const u32           displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      m_pTextRenderPipe->SetRenderTarget( pBackBuffer );
      m_pTextRenderPipe->SetDepthStencil( pDepthBuffer );
      m_pTextRenderPipe->SetDisplaySize( displayWidth, displayHeight );

      m_pTextRenderPipe->Execute();
    }
    break;
  }
}






} /*  namespace MutoTest */
} /*  namespace Test      */


#endif  /*  #if PM_DEBUG  */
