//======================================================================
/**
 * @file    FinderStudioCaptureViewBase.cpp
 * @date    2017/03/03 17:29:41
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影専用Viewベース
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>


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
CaptureViewBase::CaptureViewBase( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf /* = NULL */ )
  : Static::ViewBase( heap, renderMan, cursorBuf )
  , m_pFinderStudioSave( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFinderStudioSave() )
  , m_pListener( NULL )
  , m_pWork( NULL )
  , m_pInfoStrArray( NULL )
  , m_pInfoTexArray( NULL )
  , m_InfoParamCount( 0 )
  , m_bCreateInfoParam( false )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・終了
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   Viewの初期化
 */
void CaptureViewBase::_InitView( void )
{
  this->SetVisible( false );
  this->SetInputEnabled( false );

  // Viewユーティリティの生成
  this->CreateViewUtil();
  this->SetUIInputExclusiveKeyByButtonAnimation();
}

/**
 * @brief   Viewの終了
 */
void CaptureViewBase::_TermView( void )
{
  _DeleteInfoParam();

  this->ResetUIInputExclusiveKeyByButtonAnimation();
  this->DeleteViewUtil();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    インフォ用パラメータ：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成：インフォ用文字列バッファ
 *
 * @param   pageNum   ページ数
 * @param   charNum   文字列バッファ数：デフォルト1024
 */
void CaptureViewBase::_CreateInfoParam( const u32 pageNum, const u32 charNum /* = 1024 */ )
{
  GFL_ASSERT( pageNum > 0 );
  GFL_ASSERT( charNum > 0 );
  GFL_ASSERT( !m_bCreateInfoParam );

  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();

  m_pInfoStrArray = GFL_NEW_LOW_ARRAY( heap ) gfl2::str::StrBuf*[pageNum];
  m_pInfoTexArray = GFL_NEW_LOW_ARRAY( heap ) const gfl2::lyt::LytResTextureInfo*[pageNum];

  for( u32 i = 0; i < pageNum; ++i )
  {
    m_pInfoStrArray[i] = GFL_NEW_LOW( heap ) gfl2::str::StrBuf( charNum, heap );
  }

  m_InfoParamCount = pageNum;
  m_bCreateInfoParam = true;
}

/**
 * @brief   破棄：インフォ用文字列バッファ
 */
void CaptureViewBase::_DeleteInfoParam( void )
{
  if( m_bCreateInfoParam )
  {
    for( u32 i = 0; i < m_InfoParamCount; ++i )
    {
      GFL_SAFE_DELETE( m_pInfoStrArray[i] );
    }

    GFL_SAFE_DELETE_ARRAY( m_pInfoTexArray );
    GFL_SAFE_DELETE_ARRAY( m_pInfoStrArray );

    m_bCreateInfoParam = false;
  }
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
