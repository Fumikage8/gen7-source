//==============================================================================
/**
 * @file PaintAction.h
 * @brief
 * @author kimura_shin
 * @data 2012/03/30, 15:33
 */
// =============================================================================

#if !defined( __PAINTACTION_H__ )
#define __PAINTACTION_H__

#include <gflib.h>

class PaintAction;

/**
 * ID pixel
 */

/**
 * pixel info
 */
struct IdPixel {
  IdPixel(){}
  IdPixel(s32 x, s32 y, u32 col) : m_X(x),m_Y(y),m_Color(col) {}
  s16 m_X;
  s16 m_Y;
  u32 m_Color;
};

/**
 * PaintCommandBase
 */

class PaintCommandBase : public QUndoCommand
{
public:
  PaintCommandBase( PaintAction* pa, const QVector<IdPixel>& pix );
  virtual ~PaintCommandBase( ){}

  virtual void undo();
  virtual void redo();

private:
  void ChangeColor( PaintAction* pa, const QVector<IdPixel>& pix, QVector<IdPixel>* old_pix=0 );

  PaintAction *m_pPaintAction;
  QVector<IdPixel> m_PixelList;
  QVector<IdPixel> m_OldPixelList;
};

/**
 * Paint action
 */
class PaintAction : public QWidget {

public:

  explicit PaintAction( const s32 id, QUndoStack* us );

  virtual ~PaintAction( void ) {
  }

  gfl::cmdl::Image* GetImage( ) const {
    return m_pImage;
  }

  void ChangeColor( const QVector<IdPixel>& pix ){
    m_pUndoStack->push( DbgNew PaintCommandBase(this,pix) );
  }

protected:
  void paintEvent(QPaintEvent *event);

private:
  gfl::cmdl::Image* m_pImage;

  QUndoStack *m_pUndoStack;
};

#endif /* __PAINTACTION_H__ */
