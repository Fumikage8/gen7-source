#if PM_DEBUG

//======================================================================
/**
 * @file    FinderStudioCaptureDebugPhotoCompareFrame.cpp
 * @date    2017/04/13 11:34:41
 * @author  fukushima_yuya
 * @brief   デバッグ機能フレーム：写真差分表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureDebugPhotoCompareFrame.h"

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioCapture/source/Debug/FinderStudioCaptureDebug.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewDefine.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureUpperView.h>
#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioViewer/include/FinderStudioViewer.h>

#include <System/include/gflUse.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
DebugPhotoCompareFrame::DebugPhotoCompareFrame( void )
  : m_pUpperView( NULL )
  , m_bVisibleImage( true )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
applib::frame::Result DebugPhotoCompareFrame::InitFunc( void )
{
  if( m_MainSeq == 0 )
  {
    /// 上画面Viewの取得
    m_pUpperView = reinterpret_cast<UpperView*>( m_pMainWork->viewList->GetView( VIEW_ID_UPPER ) );

    const Viewer::PhotoParam* photoParam = &m_pMainWork->work->viewer.photo_param[0];
    {
      /// 写真パラメータのセット
      DebugClass::SetPhotoCompare_PhotoParam( photoParam );

      /// Viewerを写真モードに変更
      Viewer::FinderStudioViewer* viewer = m_pMainWork->viewer->GetFinderStudioViewer();
      {
        viewer->DebugChangeViewerMode( Viewer::VIEWER_MODE_PHOTO );
        viewer->StartPhotoReproduct( *photoParam );
      }
    }

    /// 写真を表示
    m_pUpperView->VisiblePhotoImage();

    m_MainSeq++;
  }
  if( m_MainSeq == 1 )
  {
    Viewer::FinderStudioViewer* viewer = m_pMainWork->viewer->GetFinderStudioViewer();

    if( viewer->IsFinishPhotoReproduct() )
    {
      m_MainSeq = 0;
      return applib::frame::RES_FINISH;
    }
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   終了
 */
applib::frame::Result DebugPhotoCompareFrame::EndFunc( void )
{
  Viewer::FinderStudioViewer* viewer = m_pMainWork->viewer->GetFinderStudioViewer();
  {
    /// Viewerを元に戻す
    viewer->DebugChangeViewerMode( Viewer::VIEWER_MODE_NORMAL );
  }

  /// 写真パラメータのセット
  DebugClass::SetPhotoCompare_PhotoParam( NULL );

  /// 写真を非表示
  m_pUpperView->InvisiblePhotoImage();

  this->GetFrameManager()->SetResult( Frame::RES_NEXT );

  return applib::frame::RES_FINISH;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
applib::frame::Result DebugPhotoCompareFrame::UpdateFunc( void )
{
  /// キー入力の取得
  gfl2::ui::Button* button = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  {
    if( button->IsTrigger( gfl2::ui::BUTTON_L ) || button->IsTrigger( gfl2::ui::BUTTON_R ) )
    {
      if( m_bVisibleImage )
      {
        /// 写真を非表示
        m_pUpperView->InvisiblePhotoImage();
      }
      else {
        /// 写真を表示
        m_pUpperView->VisiblePhotoImage();
      }

      m_bVisibleImage = !m_bVisibleImage;
    }
    else if( button->IsTrigger( gfl2::ui::BUTTON_X ) || button->IsTrigger( gfl2::ui::BUTTON_Y ) )
    {
      return applib::frame::RES_FINISH;
    }
  }

  return applib::frame::RES_CONTINUE;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif  // PM_DEBUG