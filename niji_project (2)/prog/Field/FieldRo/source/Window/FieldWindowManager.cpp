//==============================================================================
/**
 * @file        FieldWindowManager.cpp
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/04/21(火) 13:12:04
 */
//==============================================================================

#include <GameSys/include/GameManager.h>
#include "Applib/include/Util/AppRenderingManager.h"

#include "System/include/HeapDefine.h"

#include "Field/FieldRo/include/Window/FieldWindowManager.h"

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldWindow )


//------------------------------------------------------------------------------
/**
 * @func        FieldWindowManager
 * @brief       コンストラクタ.
 * @author      N.Takeda
 * @date        2015/04/21(火) 15:42:46
 */
//------------------------------------------------------------------------------
FieldWindowManager::FieldWindowManager( gfl2::heap::HeapBase* pHeap, app::util::Heap *_AppHeap, app::util::AppRenderingManager *_AppRenderManager, gfl2::fs::AsyncFileManager *pFileManager )
 : m_pUIView( NULL )
 , m_pAppHeap( _AppHeap )
 , m_pRenderingManager( _AppRenderManager )
 , m_pManagerHeap( NULL )
 , m_pFileManager( pFileManager )
 , m_iLastResultYesNo( LASTRESULT_NULL )
 , m_iLastResultListMenu( LASTRESULT_NULL )
{
  m_pUIView = GFL_NEW( pHeap ) FieldWindowView( m_pAppHeap, m_pAppHeap->GetSystemAllocator(), m_pAppHeap->GetDeviceAllocator() );
}


//------------------------------------------------------------------------------
/**
 * @func        ~FieldWindowManager
 * @brief       デストラクタ.
 * @author      N.Takeda
 * @date        2015/04/21(火) 15:42:44
 */
//------------------------------------------------------------------------------
FieldWindowManager::~FieldWindowManager()
{
  GFL_DELETE m_pUIView;
}



void  FieldWindowManager::Update()
{
  if( m_pUIView ) m_pUIView->UpdateTree();
  
}


void  FieldWindowManager::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_pUIView ) m_pUIView->DrawTree( displayNo );
}



GFL_NAMESPACE_END( FieldWindow )
GFL_NAMESPACE_END( Field )

