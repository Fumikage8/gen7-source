//==============================================================================

#include "../include/PaintAction.h"
#include "../include/MainWindow.h"

/**
* @file	PaintAction.cpp
* @brief	
* @author	kimura_shin
* @data	2012/03/30, 15:35
*/

//==============================================================================
// PaintCommand
//==============================================================================
//------------------------------------------------------------------------------
// construct command
//------------------------------------------------------------------------------
PaintCommandBase::PaintCommandBase( PaintAction* pa, const QVector<IdPixel>& pix ) : m_pPaintAction(pa), m_PixelList(pix)
{
  ChangeColor(pa,pix,&m_OldPixelList);
}

//------------------------------------------------------------------------------
// undo
//------------------------------------------------------------------------------
void PaintCommandBase::undo()
{
  ChangeColor( m_pPaintAction, m_OldPixelList );
}

//------------------------------------------------------------------------------
// redo
//------------------------------------------------------------------------------
void PaintCommandBase::redo()
{
  ChangeColor( m_pPaintAction, m_PixelList );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void PaintCommandBase::ChangeColor(PaintAction* pa, const QVector<IdPixel>& pix, QVector<IdPixel>* old_pix)
{
  u32* img = (u32*)pa->GetImage()->GetImage();
  const s32 w = pa->GetImage()->GetWidth();

  for( s32 i = 0; i < pix.size(); ++i )
  {
    IdPixel p = pix.at(i);

    u32* img_p = &img[ p.m_X + p.m_Y*w ];
    if(old_pix){
      old_pix->append(IdPixel( p.m_X,p.m_Y, *img_p ));
    }
    *img_p = p.m_Color;
  }
  pa->repaint();
}

//==============================================================================
// PaintAction
//==============================================================================

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PaintAction::PaintAction(const s32 id, QUndoStack* us) : m_pUndoStack(us)
{
  m_pImage = MainWindow::GetInstance( )->GetGLWindow( )->GetGLWidget( )->GetModel( GLWidget::MODEL_ID )->GetImageData( )->at( id );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void PaintAction::paintEvent( QPaintEvent* )
{
  QPainter painter( this );
  painter.drawImage( QPoint( 0, 0 ), QImage((u8*)m_pImage->GetImage(),m_pImage->GetWidth(),m_pImage->GetHeight(),QImage::Format_ARGB32) );
}
