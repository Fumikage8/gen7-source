//==============================================================================
/**
  *  @file   NetAppDebugTextDrawer.cpp
  *  @brief  
  *  @date   2015.06.16 Tue.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <system/include/GflUse.h>
#include  <print/include/SystemFont.h>
#include  <AppLib/include/Util/app_util_heap.h>
#include  "NetStatic/NetAppLib/include/Debug/NetAppDebugTextDrawer.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  static const f32  TextRenderPipeScreenW = 512.0f;
  static const f32  TextRenderPipeScreenH = 256.0f;
  static const u32  TempStrBufSize        = 128;
  static const u32  TempCharMax           = TempStrBufSize-1;
  static const f32  DefaultScale          = 0.5f;
}




namespace NetAppLib {
namespace Debug {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015.06.18 Thu.
  */
//==============================================================================
class NetAppDebugTextDrawerImplBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppDebugTextDrawerImplBase );
public:
  NetAppDebugTextDrawerImplBase(void){}
  virtual ~NetAppDebugTextDrawerImplBase(){}

  virtual void  Initialize(app::util::Heap* pHeap) {}
  virtual void  Finalize(void) {}
  virtual void  CLS(void) {}
  virtual void  LOC(const s32 x, const s32 y) {}
  virtual void  CRLF(void) {}
  virtual void  COLOR(const u8 r, const u8 g, const u8 b){}
  virtual void  Scale(const f32 scaleH, const f32 scaleV){}
  virtual void  Print(const wchar_t* pFormat, va_list valist) {}
  virtual void  PrintLn(const wchar_t* pFormat, va_list valist) {}
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo) {} 
};



//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015.06.16 Tue.
  */
//==============================================================================
#if PM_DEBUG
class NetAppDebugTextDrawerImpl
  : public NetAppDebugTextDrawerImplBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppDebugTextDrawerImpl );
public:
  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  NetAppDebugTextDrawerImpl(void)
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
    */
  //------------------------------------------------------------------
  virtual ~NetAppDebugTextDrawerImpl()
  {
    Finalize();
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  Initialize(app::util::Heap* pHeap)
  {
    gfl2::heap::HeapBase*     pHeapBase   = pHeap->GetDeviceHeap();
    gfl2::heap::NwAllocator*  pAllocator  = pHeap->GetDeviceAllocator();

    Finalize();
    m_pTextRenderPipe = GFL_NEW(pHeapBase) gfl2::util::UtilTextRenderingPipeLine( pHeap->GetDevGLHeapAllocator(), 512, 256 );
    m_pTextUtil       = GFL_NEW(pHeapBase) gfl2::util::DrawUtilText(pAllocator, pAllocator); 
    m_pTextUtil->Initialize( pAllocator, pAllocator, m_pTextRenderPipe->GetUtilTextRenderPath() );
    m_pTextUtil->SetFont(print::SystemFont_GetFont());

    m_pTextUtil->SetTextScale( DefaultScale, DefaultScale );
    m_FontH = print::SystemFont_GetFont()->GetHeight() * DefaultScale;
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  Finalize(void)
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
  virtual void  CLS(void)
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
  virtual void  LOC(const s32 x, const s32 y)
  {
    m_Pos.x = x * m_FontH;
    m_Pos.y = y * m_FontH;
  }

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  CRLF(void)
  {
    LOC(0, (m_Pos.y / m_FontH)+1);
  }

  //------------------------------------------------------------------
  /**
  *  @func
  *  @brief
  */
  //------------------------------------------------------------------
  virtual void  COLOR(const u8 r, const u8 g, const u8 b)
  {
    m_Color.x = (f32)r / 255.0f;
    m_Color.y = (f32)g / 255.0f;
    m_Color.z = (f32)b / 255.0f;
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  Scale(const f32 scaleH, const f32 scaleV)
  {
    m_FontH = print::SystemFont_GetFont()->GetHeight() * scaleH;
    m_pTextUtil->SetTextScale(scaleH, scaleV);
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  Print(const wchar_t* pFormat, va_list valist)
  {
    wchar_t   strBuf[TempStrBufSize];
    vswprintf( strBuf, TempCharMax, pFormat, valist );
    m_pTextUtil->DrawText(m_Pos.x, m_Pos.y, strBuf, m_Color, m_Color);
  }



  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  PrintLn(const wchar_t* pFormat, va_list valist)
  {
    Print(pFormat, valist);
    CRLF();
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo)
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

private:
  gfl2::util::UtilTextRenderingPipeLine*  m_pTextRenderPipe;
  gfl2::util::DrawUtilText*               m_pTextUtil;
  struct{s32 x, y;}                       m_Pos;
  gfl2::math::Vector4                     m_Color;
  int                                     m_FontH;
};


#endif  /*  #if PM_DEBUG  */


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppDebugTextDrawer::NetAppDebugTextDrawer(void)
  : m_pImpl(NULL)
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
NetAppDebugTextDrawer::~NetAppDebugTextDrawer()
{
  Finalize();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::Initialize(app::util::Heap* pHeap)
{
#if PM_DEBUG
  if(!m_pImpl)
  {
    m_pImpl = GFL_NEW(pHeap->GetDeviceHeap()) NetAppDebugTextDrawerImpl();
    m_pImpl->Initialize(pHeap);
  }
#endif
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::Finalize(void)
{
  if(!m_pImpl)  return;

  m_pImpl->Finalize();
  GFL_SAFE_DELETE(m_pImpl);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::CLS(void)
{
  if(!m_pImpl)  return;

  m_pImpl->CLS();
  m_pImpl->LOC(0,0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::LOC(const s32 x, const s32 y)
{
  if(!m_pImpl)  return;

  m_pImpl->LOC(x, y);
}


//------------------------------------------------------------------
/**
*  @func
*  @brief
*/
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::CRLF(void)
{
  if (!m_pImpl)  return;

  m_pImpl->CRLF();
}


//------------------------------------------------------------------
/**
*  @func
*  @brief
*/
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::COLOR(const u8 r, const u8 g, const u8 b)
{
  if (!m_pImpl)  return;

  m_pImpl->COLOR(r, g, b);
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::Scale(const f32 scaleH, const f32 scaleV)
{
  if(!m_pImpl)  return;

  m_pImpl->Scale(scaleH, scaleV);
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::Print(const wchar_t* pFormat, ...)
{
  if(!m_pImpl)  return;

  va_list valist;
  va_start( valist, pFormat );

  Print(pFormat, valist);

  va_end( valist );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::Print(const wchar_t* pFormat, va_list valist)
{
  if(!m_pImpl)  return;
  
  m_pImpl->Print(pFormat, valist);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::PrintLn(const wchar_t* pFormat, ...)
{
  if(!m_pImpl)  return;
  
  va_list valist;
  va_start( valist, pFormat );

  m_pImpl->PrintLn(pFormat, valist);

  va_end( valist );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppDebugTextDrawer::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(!m_pImpl)  return;

  m_pImpl->Draw(displayNo);
}










} /*  namespace Debug     */
} /*  namespace NetAppLib */
