/* 
 * File:   MainWindow.h
 * Author: s-kimura
 *
 * Created on 2010/06/09, 14:31
 */

#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <vector>
#include "GLWidget.h"
#include "GLWindow.h"
#include "ToolWidget.h"
#include "IdWidget.h"
#include "TextureWidget.h"
#include "DisplayWidget.h"
#include "MotionWidget.h"
#include "UvPaintWindow.h"

#define ID_FILE_EXT "Id.tga"

class GLAllocator : public gfl2::gfx::IGLAllocator
{
public:
  GLAllocator(gfl2::heap::HeapBase *devHeap):IGLAllocator(), mHeap(devHeap){}
  virtual ~GLAllocator(){}

#if GFL_DEBUG
void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
{
  return mHeap->AllocMemoryAlign(size,alignment,filename,linenum);
}
#else
void* SystemMemoryMallocAlign(u32 size, u32 alignment)
{
  return GflHeapAllocMemoryAlign( mHeap , size , alignment );
}
#endif

  void SystemMemoryFree(void* const address)
  {
    GflHeapFreeMemory( address );
  }

  void* LocalMemoryMalloc(u32 size, u32 alignment = 4U){
    return NULL;
  }

  void LocalMemoryFree(void* const address)
  {
  }

private:
  gfl2::heap::HeapBase* mHeap;
};

// ===========================================================================
// Main Window
// ===========================================================================

class MainWindow : public gfl::core::SingletonInherit<MainWindow, gfl::qt::MainWindow> {
  Q_OBJECT

public:
  MainWindow( gfl::heap::HeapBase* heap = 0 );
  virtual ~MainWindow( void );

  GLWindow* GetGLWindow( void ) {
    return m_pGLWindow;
  }
  virtual bool LoadFile( const QString& fname );

  DisplayWidget* GetDisplayWidget( ) const {
    return m_pDisplayWidget;
  }

  QSettings* GetSettings( ) const {
    return m_pSettings;
  }

  /**
   * IDテクスチャファイル名の取得
   * @param ctexfname　元テクスチャファイル名
   * @return IDテクスチャファイル名
   */
  static QString GetIdTexFileName( const QString& ctexfname ) {
    QString fname( ctexfname );
    fname.replace( ".tga", ID_FILE_EXT );
    return fname;
  }

  /**
   * テクスチャグループを取得
   * @return テクスチャウィジェット
   */
  TextureWidget* GetTextureWidget( ) const {
    return m_pTextureWidget;
  }

  /**
   * ID　ウィジェットを取得
   * @return ID　ウィジェット
   */
  IdWidget* GetIdWidget( ) const {
    return m_pIdWidget;
  }

  /**
   * ツールウィジェットを取得
   * @return ツールウィジェット
   */
  ToolWidget* GetToolWidget( ) const {
    return m_pToolWidget;
  }

  MotionWidget* GetMotionWidget( ) const {
    return m_pMotionWidget;
  }

  QUndoStack* GetUndoStack( ) const {
    return m_pUndoStack;
  }

  HGLRC GetHGLRC()
  {
    return m_hRC;
  }

  //UVペイントビュー取得
  std::vector<Gui::UvPaintWindow*> GetUvPaintWindowList() const
  {
    return m_pUvPaintWindowList;
  }

private:
  void CreateAction( );
  void CreateMenu( );
  void CheckId();
  void SaveIdExec(bool mes_on=true);

  QList<gfl::cmdl::Image*> m_ReloadImage;

  enum {
    SCREEN_UP_LEFT,
    SCREEN_UP_RIGHT,
    SCREEN_DOWN_LEFT,
    SCREEN_DOWN_RIGHT,
    SCREEN_MAX,
  };
  GLWindow* m_pGLWindow;
  QMenu *m_pFileMenu;
  QMenu *m_pActionMenu;
  QMenu *m_pHelpMenu;
  QAction* m_pLoadGfmdlFile;
  QAction* m_pSaveIdFile;

  QAction* m_pUndo;
  QAction* m_pRedo;
  QUndoStack* m_pUndoStack;

  QAction* m_pHelpXml;

  ToolWidget* m_pToolWidget;
  IdWidget* m_pIdWidget;
  DisplayWidget* m_pDisplayWidget;
  MotionWidget* m_pMotionWidget;
  TextureWidget* m_pTextureWidget;
  QMainWindow* m_pDockWindow;
  QDialog* m_pHelpDialog;

  std::vector<Gui::UvPaintWindow*> m_pUvPaintWindowList;
  HGLRC m_hRC;
  bool m_IdLoaded;
  QString m_LastLoadFileName;

  GLAllocator* allocator;

protected:
  void timerEvent(QTimerEvent *event);

signals:
  void ChangeDock(bool);

  public slots:

    void LoadGfmdl( );
    void SaveId( );
    void ShowHelpXml();
};

#endif	/* MAINWINDOW_H */
