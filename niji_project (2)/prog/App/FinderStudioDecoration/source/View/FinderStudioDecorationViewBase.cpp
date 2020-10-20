//======================================================================
/**
 * @file    FinderStudioDecorationViewBase.cpp
 * @date    2017/01/31 17:28:42
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：デコ専用Viewベース
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationViewBase.h"

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
DecoViewBase::DecoViewBase( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : Static::ViewBase( heap, renderMan )
  , m_pListener( NULL )
  , m_pFinderStudioSave( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFinderStudioSave() )
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
void DecoViewBase::_InitView( void )
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
void DecoViewBase::_TermView( void )
{
  this->ResetUIInputExclusiveKeyByButtonAnimation();
  this->DeleteViewUtil();
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
