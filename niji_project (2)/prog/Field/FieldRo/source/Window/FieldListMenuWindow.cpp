//==============================================================================
/**
 * @file        FieldListMenuWindow.cpp
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/05/08(金) 21:24:41
 */
//==============================================================================

// niji
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "Field/FieldRo/include/Window/FieldWindowManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Window/FieldListMenuWindow.h"


/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldWindow )


//------------------------------------------------------------------------------
/**
 * @func        FieldListMenuWindow
 * @brief       コンストラクタ.
 * @author      N.Takeda
 * @date        2015/04/17(金) 12:38:36
 *
 * @param[in]     const gfl2::fs::ArcFile::ARCID    _uArcFileId       メッセージファイルID.
 * @param[in]     const gfl2::fs::ArcFile::ARCDATID _uArcDatId        メッセージデータID.
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
FieldListMenuWindow::FieldListMenuWindow( Field::FieldWindow::FieldWindowManager *_pFieldWindowManager,
  s32 _iX,
  s32 _iY,
  gfl2::fs::ArcFile::ARCID _uArcId,
  gfl2::fs::ArcFile::ARCDATID _uDatId,
  s32 _iStartIdx,
  u32 _uCancelType,
  bool _bIsRight,
  gfl2::str::MsgData *_pMshData
 )
 : m_pListMenu( NULL )
 , m_pFieldWindowManager( _pFieldWindowManager )
 , m_iIteMax( 0 )
 , m_pWordSet( NULL )
 , m_bDecide( false )
 , m_iX( _iX )
 , m_iY( _iY )
 , m_uArcId( _uArcId )
 , m_uDatId( _uDatId )
 , m_iStartIdx( _iStartIdx )
 , m_uCancelType( _uCancelType )
 , m_bIsRight( _bIsRight )
 , m_pMsgData( NULL )
 , m_bExtMsgData( false )
{
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  
  for( s32 ii=0 ; ii<App::Tool::ListMenu::ITEMLIST_MAX ; ii++ ) ///< Coverity.11694
  {
    m_stItem[ ii ].m_pTempStrBuf = NULL;
    m_stItem[ ii ].m_iIndex = 0;        ///< Coverity.11694
  }
  if( _pMshData )
  {
    m_pMsgData = GFL_NEW( m_pHeap )gfl2::str::MsgData( _pMshData, m_pHeap );
    m_bExtMsgData = true;
  }
  if( !m_bExtMsgData ){
    m_pMsgData = GFL_NEW(m_pHeap->GetLowerHandle()) gfl2::str::MsgData( m_uArcId, m_uDatId, m_pHeap->GetLowerHandle(), gfl2::str::MsgData::LOAD_PART );
  }
}


//------------------------------------------------------------------------------
/**
 * @func        ~FieldListMenuWindow
 * @brief       デストラクタ.
 * @author      N.Takeda
 * @date        2015/04/17(金) 12:38:31
 */
//------------------------------------------------------------------------------
FieldListMenuWindow::~FieldListMenuWindow()
{
  for( s32 ii=0 ; ii<m_iIteMax ; ii++ )
  {
    GFL_DELETE m_stItem[ ii ].m_pTempStrBuf;
  }
  GFL_DELETE m_pMsgData;
  
  m_pListMenu->RemoveFromSuperView();
  
  m_pListMenu->DeleteMenu();
  GFL_DELETE( m_pListMenu );
  
  m_appUtilHeap.Delete();
}


//------------------------------------------------------------------------------
/**
 * @func        AddItem
 * @brief       Itemの追加.
 * @author      N.Takeda
 * @date        2015/05/08(金) 21:34:43
 *
 * @param[in] s32 _iMsg1  メッセージ.
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  FieldListMenuWindow::AddItem( s32 _iMsg1, s32 _iIndex )
{
  GFL_ASSERT( m_iIteMax < App::Tool::ListMenu::ITEMLIST_MAX );
  
  m_stItem[ m_iIteMax ].m_iIndex = _iIndex;
  
  m_stItem[ m_iIteMax ].m_pTempStrBuf = GFL_NEW( m_pHeap->GetLowerHandle() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, m_pHeap->GetLowerHandle() );
  if( m_pWordSet )
  {
    gfl2::str::StrBuf   *pTempStrBuf2;
    pTempStrBuf2 = GFL_NEW( m_pHeap->GetLowerHandle() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, m_pHeap->GetLowerHandle() );
    m_pMsgData->GetString( _iMsg1, *pTempStrBuf2 );
    m_pWordSet->Expand( m_stItem[ m_iIteMax ].m_pTempStrBuf, pTempStrBuf2 );
    GFL_DELETE pTempStrBuf2;
  }else{
    m_pMsgData->GetString( _iMsg1, *m_stItem[ m_iIteMax ].m_pTempStrBuf );
  }
  
  m_iIteMax++;
}

#if PM_DEBUG
//------------------------------------------------------------------------------
/**
 * @func        AddItem
 * @brief       Itemの追加.
 *
 * @param[in] s32 _iMsg1  メッセージ.
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  FieldListMenuWindow::DebugAddItem( const char* pText, s32 _iIndex )
{
  GFL_ASSERT( m_iIteMax < App::Tool::ListMenu::ITEMLIST_MAX );
  
  m_stItem[ m_iIteMax ].m_iIndex = _iIndex;
  
  wchar_t workStr[512];
  // @note　半角英数字のみだからこの変換方法でなりたつ
#if defined(GF_PLATFORM_WIN32)
  swprintf(workStr, 512, L"%S", pText);
#elif defined(GF_PLATFORM_CTR)
  swprintf(workStr, 512, L"%s", pText); //printfの%S書式がARMには無い
#endif


  m_stItem[ m_iIteMax ].m_pTempStrBuf = GFL_NEW( m_pHeap->GetLowerHandle() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, m_pHeap->GetLowerHandle() );
  if( m_pWordSet )
  {
    gfl2::str::StrBuf   *pTempStrBuf2;
    pTempStrBuf2 = GFL_NEW( m_pHeap->GetLowerHandle() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, m_pHeap->GetLowerHandle() );
    pTempStrBuf2->SetStr( workStr );
    m_pWordSet->Expand( m_stItem[ m_iIteMax ].m_pTempStrBuf, pTempStrBuf2 );
    GFL_DELETE pTempStrBuf2;
  }else{
    m_stItem[m_iIteMax].m_pTempStrBuf->SetStr( workStr );
  }
  
  m_iIteMax++;
}
#endif

//------------------------------------------------------------------------------
/**
 * @func        Start
 * @brief       開始.
 * @author      N.Takeda
 * @date        2015/05/08(金) 21:34:42
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  FieldListMenuWindow::Start()
{
  static const u32 LOCAL_HEAP_SIZE        = 0x20000;
  static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x20000;

  GFL_ASSERT( m_pFieldWindowManager );  ///< デバッグモードから呼び出しているなど(Test/TakedaTest/source/TestBagFrame.cpp(79): デバッグランチャーでのテスト起動ではFieldMapは作られないので、以下のメソッドを gamemanager をに追加する.
  if( !m_pFieldWindowManager )  return;

  // ローカルヒープ生成
  m_appUtilHeap.LocalHeapCreate( m_pHeap, m_pHeap, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
  
  App::Tool::ListMenu::PARAM param;
  param.pHeap = &m_appUtilHeap;
  param.pAppRenderingManager = m_pFieldWindowManager->GetRenderingManager();
  param.pFileManager = m_pFieldWindowManager->GetFileManager();
  param.lytRes = NULL;
  param.initPos = m_iStartIdx;
  param.listMax = m_iIteMax;
  param.posTypeX = App::Tool::ListMenu::LIST_POS_RIGHT;
  param.posTypeY = App::Tool::ListMenu::LIST_POS_TOP;
  param.x = m_iX;
  param.y = m_iY;
  param.strPosX = (m_bIsRight) ? App::Tool::ListMenu::HORIZONTALPOS_RIGHT :App::Tool::ListMenu::HORIZONTALPOS_LEFT;
  param.startWait = 1;
  if( m_uCancelType ) param.bMode = App::Tool::ListMenu::B_BUTTON_MODE_CANCEL;
  else                param.bMode = App::Tool::ListMenu::B_BUTTON_MODE_NONE;
  param.visible = true;
  param.autoDraw = false;
  param.winType = App::Tool::ListMenu::WIN_TYPE_WHITE;

  m_pListMenu = GFL_NEW(m_pHeap) App::Tool::ListMenu(&param);
  
  for( s32 ii=0 ; ii<m_iIteMax ; ii++ )
  {
    m_pListMenu->AddItem( m_stItem[ ii ].m_iIndex, m_stItem[ ii ].m_pTempStrBuf );
  }
  
  m_pListMenu->Start();
  m_pFieldWindowManager->GetUIView()->AddSubView( m_pListMenu );
  
}


//------------------------------------------------------------------------------
/**
 * @func        GetDecide
 * @brief       Decideの取得.
 * @author      N.Takeda
 * @date        2015/04/28(火) 16:53:27
 *
 * @return      bool  TRUE:成 / FALSE:否
 */
//------------------------------------------------------------------------------
bool  FieldListMenuWindow::GetDecide() const
{
  return m_pListMenu->IsInstanceDelete();
}

//------------------------------------------------------------------------------
/**
 * @func        GetSelectId
 * @brief       選択Idの取得.
 * @author      N.Takeda
 * @date        2015/05/11(月) 17:18:24
 *
 * @return      s32 
 */
//------------------------------------------------------------------------------
s32 FieldListMenuWindow::GetSelectId() const
{
  return m_pListMenu->GetSelectId();
}


GFL_NAMESPACE_END( FieldWindow )
GFL_NAMESPACE_END( Field )

