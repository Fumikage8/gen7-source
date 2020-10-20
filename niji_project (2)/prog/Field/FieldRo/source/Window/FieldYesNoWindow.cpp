//==============================================================================
/**
 * @file        FieldYesNoWindow.cpp
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/04/16(木) 16:12:37
 */
//==============================================================================

// niji
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "Field/FieldRo/include/Window/FieldWindowManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Window/FieldYesNoWindow.h"


/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldWindow )


//------------------------------------------------------------------------------
/**
 * @func        FieldYesNoWindow
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
FieldYesNoWindow::FieldYesNoWindow( Field::FieldWindow::FieldWindowManager *_pFieldWindowManager,
  s32 _iX,
  s32 _iY,
  gfl2::fs::ArcFile::ARCID _uArcId,
  gfl2::fs::ArcFile::ARCDATID _uDatId,
  s32 _iMsg1,
  s32 _iMsg2,
  s32 _iStartIdx,
  u32 _uCancelType,
  void *_pMshData
 )
 : m_pListMenu( NULL )
 , m_pFieldWindowManager( _pFieldWindowManager )
 , m_bDecide( false )
 , m_pHeap( NULL )
{
  static const u32 TEMP_STRBUF_LENGTH = (18*3+1);    //!< 汎用文字列バッファの文字数
  static const u32 LOCAL_HEAP_SIZE        = 0x20000;
  static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x20000;

  GFL_ASSERT( m_pFieldWindowManager );  ///< デバッグモードから呼び出しているなど(Test/TakedaTest/source/TestBagFrame.cpp(79): デバッグランチャーでのテスト起動ではFieldMapは作られないので、以下のメソッドを gamemanager をに追加する.
  if( !m_pFieldWindowManager )  return;

  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
   // ローカルヒープ生成
  m_appUtilHeap.LocalHeapCreate( m_pHeap, m_pHeap, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
  
  gfl2::str::MsgData  *pMsgData;       ///< メッセージデータ
  gfl2::str::StrBuf   *pTempStrBuf1;
  gfl2::str::StrBuf   *pTempStrBuf2;

  App::Tool::ListMenu::PARAM param;
  param.pHeap = &m_appUtilHeap;
  param.pAppRenderingManager = m_pFieldWindowManager->GetRenderingManager();
  param.pFileManager = m_pFieldWindowManager->GetFileManager();
  param.lytRes = NULL;
  param.initPos = _iStartIdx;
  param.listMax = 2;
  param.posTypeX = App::Tool::ListMenu::LIST_POS_RIGHT;
  param.posTypeY = App::Tool::ListMenu::LIST_POS_TOP;
  param.x = static_cast<f32>(_iX);
  param.y = static_cast<f32>(_iY);                    ///< +で上、-で下
  param.strPosX = App::Tool::ListMenu::HORIZONTALPOS_LEFT;
  param.startWait = 1;
  param.bMode = _uCancelType;
  param.visible = true;
  param.autoDraw = false;
  param.winType = App::Tool::ListMenu::WIN_TYPE_WHITE;

  m_pListMenu = GFL_NEW(m_pHeap) App::Tool::ListMenu(&param);
  
  if( _pMshData ) pMsgData = GFL_NEW( m_pHeap )gfl2::str::MsgData( _pMshData, m_pHeap );
  else            pMsgData = GFL_NEW(m_pHeap->GetLowerHandle()) gfl2::str::MsgData( _uArcId, _uDatId, m_pHeap->GetLowerHandle(), gfl2::str::MsgData::LOAD_PART );

  pTempStrBuf1 = GFL_NEW( m_pHeap->GetLowerHandle() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, m_pHeap->GetLowerHandle() );
  pTempStrBuf2 = GFL_NEW( m_pHeap->GetLowerHandle() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, m_pHeap->GetLowerHandle() );

  pMsgData->GetString( _iMsg1, *pTempStrBuf1 );
  m_pListMenu->AddItem( 0, pTempStrBuf1 );
  pMsgData->GetString( _iMsg2, *pTempStrBuf2 );
  m_pListMenu->AddItem( 1, pTempStrBuf2 );
  
  m_pListMenu->Start();
  m_pFieldWindowManager->GetUIView()->AddSubView( m_pListMenu );
  
  GFL_DELETE pTempStrBuf1;
  GFL_DELETE pTempStrBuf2;
  GFL_DELETE pMsgData;
}


//------------------------------------------------------------------------------
/**
 * @func        ~FieldYesNoWindow
 * @brief       デストラクタ.
 * @author      N.Takeda
 * @date        2015/04/17(金) 12:38:31
 */
//------------------------------------------------------------------------------
FieldYesNoWindow::~FieldYesNoWindow()
{
  m_pListMenu->RemoveFromSuperView();
  
  m_pListMenu->DeleteMenu();
  GFL_DELETE( m_pListMenu );
  
  m_appUtilHeap.Delete();
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
bool  FieldYesNoWindow::GetDecide() const
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
s32 FieldYesNoWindow::GetSelectId() const
{
  return m_pListMenu->GetSelectId();
}


GFL_NAMESPACE_END( FieldWindow )
GFL_NAMESPACE_END( Field )
