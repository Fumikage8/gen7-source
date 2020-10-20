//===========================================================================
/**
 * @file    RttiEditor.cpp
 * @brief   送られてきたRtti情報を解析しエディタを作成する
 * @date    2014.11.18
 * @author  yy-ikeuchi
 */
//===========================================================================

// プラットフォーム共通のインクルード
#include "stdarg.h"
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_Map.h>
#include <util/include/gfl2_DrawUtilText.h>
#include <FontRenderer/include/gfl2_FontRenderer.h>
#include "../../include/RttiDebug/RttiEditor.h"
#include "../../include/RttiDebug/RttiManager.h"

#if USE_RTTI_DEBUG

namespace gfl2{ namespace rttidebug{

// Editorの初期座標、Editorの行数
const float EDITOR_X = 20.0f;
const float EDITOR_Y = 20.0f;
const int   MAX_LINE = 8;

// RTTI_DEBUG機能で利用する矩形・フォント描画
#define RTTI_DEBUG_FONT_CLEAR                       m_pDrawUtilText->ClearDrawText();
#define RTTI_DEBUG_FONT_PRINT( x, y , color, text ) m_pDrawUtilText->SetTextColor( gfl2::math::Vector4(color._r,color._g,color._b, 1.0f ) ); \
                                                    m_pDrawUtilText->SetTextScale( 0.5f , 0.5f ); \
                                                    m_pDrawUtilText->DrawText    ( x, y, text );

//------------------------------------------------------------------
/**
* @brief   コンストラクタ(使用禁止)
*/
//------------------------------------------------------------------
RttiEditor::RttiEditor()
: m_scrollCnt          (0)
, m_editorPosX         (EDITOR_X)
, m_editorPosY         (EDITOR_Y)
, m_editorLine         (MAX_LINE)
, m_isDraw             (false)
, m_isChangeRequestList(false)
, m_tagControl         (TAG_NONE)
, m_paramControl       (PARAM_NONE)
, m_valueControl       (VALUE_NONE)
, m_valueCoefficient   (1.0f)
, m_pDrawUtilText      (NULL)
{}

//------------------------------------------------------------------
/**
* @brief   初期化
* @param   ヒープ
* @param   バッファサイズ
*/
//------------------------------------------------------------------
void RttiEditor::Initialize(gfl2::heap::HeapBase* pHeap, u32 bufferSize)
{
  m_pHeap          = pHeap;
  m_bufferSize     = bufferSize;
  m_editorRequestList.CreateBuffer( pHeap , bufferSize );
  m_dispMemberPropertyList.CreateBuffer( pHeap , bufferSize );
}

//------------------------------------------------------------------
/**
* @brief   破棄
*/
//------------------------------------------------------------------
void RttiEditor::Terminate(void)
{
  for( gfl2::util::List<RttiParamManager::MemberProperty*>::Iterator iteListParam = m_dispMemberPropertyList.Begin() ; iteListParam != m_dispMemberPropertyList.End() ; iteListParam++ )
  {
    GFL_SAFE_DELETE(*iteListParam);
  }
  m_dispMemberPropertyList.Clear();
  m_editorRequestList.Clear();
}

//------------------------------------------------------------------
/**
* @brief   描画
* @return  なし
*/
//------------------------------------------------------------------
void RttiEditor::Draw(void)
{
  RTTI_DEBUG_FONT_CLEAR;

  if( !IsEnableDraw() )
  {
    return;
  }

  // リクエストリストの先頭ノードを取得
  gfl2::util::List<EditorRequest>::Iterator iteEditorRequest = m_editorRequestList.Begin();
  if( iteEditorRequest == m_editorRequestList.End() )
  {
    return;
  }

  // 処理するリクエスト情報を取得
  const void*      pTargetClassAddress = iteEditorRequest->_pInstance;

  //--------------------------------------------------------------
  // レイアウトを決定
  //--------------------------------------------------------------
  RttiEditor::ColorRGB VIEW_COLOR          ( 1.0f, 1.0f, 1.0f );  // 白
  RttiEditor::ColorRGB EDIT_COLOR          ( 1.0f, 0.3f, 0.3f );  // 赤
  RttiEditor::ColorRGB DISABLE_VIEW_COLOR  ( 0.5f, 0.5f, 0.5f );  // 灰色
  RttiEditor::ColorRGB DISABLE_EDIT_COLOR  ( 0.5f, 0.2f, 0.2f );  // 暗い赤色

  const float FONT_SIZE_X         =  8;   // フォントの横幅
  const float FONT_SIZE_Y         = 16;   // フォントの縦幅

  // これより下はEditorの座標/サイズを変更すると自動的についてくるものです

  // TagNameの位置設定
  const float TAG_NAME_X          = m_editorPosX + (FONT_SIZE_X * 2);
  const float TAG_NAME_Y          = m_editorPosY;
  const float TAG_HEIGHT          = FONT_SIZE_Y;
  // TextNameの位置設定
  const float TEXT_NAME_WIDTH     = FONT_SIZE_X * (RttiParamManager::MemberProperty::DISP_NAME_SIZE + 1);
  const float TEXT_NAME_X         = TAG_NAME_X;
  const float TEXT_NAME_Y         = m_editorPosY + TAG_HEIGHT;
  // TextValueの位置設定
  const float TEXT_VALUE_X        = TEXT_NAME_X + TEXT_NAME_WIDTH;
  const float TEXT_VALUE_Y        = TEXT_NAME_Y;
  // ScrollBarの位置/サイズ設定
  const float SCROLL_BAR_X        = m_editorPosX;
  const float SCROLL_BAR_Y        = TEXT_NAME_Y;

  //--------------------------------------------------------------
  // タグを描画
  //--------------------------------------------------------------

  // クラスのハッシュコードから表示するタグの色を取得
  RttiEditor::ColorRGB tagColor( 1.0f, 1.0f ,1.0f );

  unsigned int textIndex = 0;

  // タグが複数ある場合
  if( m_editorRequestList.Size() > 1 )
  {
    m_textBuffer[textIndex++] = L'<';
    m_textBuffer[textIndex++] = L'<';
    m_textBuffer[textIndex++] = L' ';
    m_textBuffer[textIndex++] = L'[';
    m_textBuffer[textIndex++] = L' ';
  }
  // タグが一つしかない場合
  else
  {
    m_textBuffer[textIndex++] = L'[';
    m_textBuffer[textIndex++] = L' ';
  }

  // タグの名前を取得
  for( int i=0 ; i<EditorRequest::DISP_NAME_SIZE ; i++ )
  {
    m_textBuffer[textIndex++] = iteEditorRequest->_dispName[i];
    if( m_textBuffer[textIndex-1] == L'\0' )
    {
      m_textBuffer[textIndex-1] = L' ';
      break;
    }
  }

  // 別のタグがある場合
  if( m_editorRequestList.Size() > 1 )
  {
    m_textBuffer[textIndex++] = L' ';
    m_textBuffer[textIndex++] = L']';
    m_textBuffer[textIndex++] = L' ';
    m_textBuffer[textIndex++] = L'>';
    m_textBuffer[textIndex++] = L'>';
    m_textBuffer[textIndex++] = L'\0';
  }
  // タグが一つしかない場合
  else
  {
    m_textBuffer[textIndex++] = L' ';
    m_textBuffer[textIndex++] = L']';
    m_textBuffer[textIndex++] = L'\0';
  }
  
  RTTI_DEBUG_FONT_PRINT( TAG_NAME_X , TAG_NAME_Y , VIEW_COLOR , m_textBuffer );
  
  //--------------------------------------------------------------
  // メンバ変数情報を描画
  //--------------------------------------------------------------
  unsigned int nowIndex             = 0;
  unsigned int maxLine              = m_editorLine;
  unsigned int maxSize              = (unsigned int)m_dispMemberPropertyList.Size();
  unsigned int executeCount         = 0;
  int          textSize             = 0;
  bool         isEnableUpScroll     = false;
  bool         isEnableDownScoroll  = false;

  for( gfl2::util::List<RttiParamManager::MemberProperty*>::Iterator iteListParam = m_dispMemberPropertyList.Begin() ; iteListParam != m_dispMemberPropertyList.End() ; iteListParam++,nowIndex++ )
  {
    // 全項目が収まらなかった場合
    if( maxSize >= maxLine )
    {
      // 描画するメンバ変数情報のスタート位置を変更
      if( m_scrollCnt > nowIndex && m_scrollCnt-nowIndex >= maxLine )
      {
        isEnableUpScroll = true;
        continue;
      }
      // 行の限界まで書いたかどうか
      if( executeCount >= maxLine)
      {
        isEnableDownScoroll = true;
        break;
      }
    }

    // メンバ変数情報を取得
    RttiParamManager::MemberProperty* pParam = *iteListParam;
    const uint64_t      memberAddressOffset = pParam->_offsetAddress;
    const unsigned int  memberAttribute     = pParam->_attribute;
    const c16*          memberDisp          = pParam->_dispName;
    const type_info*    pMemberTypeInfo     = pParam->_pTypeInfo;

    // "インスタンスの先頭アドレス + メンバ変数のオフセットアドレス"で指定インスタンスのメンバへのアドレスを取得
    unsigned char* pAddress = (unsigned char*)pTargetClassAddress + memberAddressOffset;

    // 自身がポインタかどうか調査
    bool isPtr  = false;
    if( typeid(         bool*     ) == *pMemberTypeInfo
     || typeid(         float*    ) == *pMemberTypeInfo
     || typeid(         double*   ) == *pMemberTypeInfo
     || typeid(         char*     ) == *pMemberTypeInfo
     || typeid(         short*    ) == *pMemberTypeInfo
     || typeid(         int*      ) == *pMemberTypeInfo
     || typeid(         long*     ) == *pMemberTypeInfo
     || typeid(         long long*) == *pMemberTypeInfo
     || typeid(unsigned char*     ) == *pMemberTypeInfo
     || typeid(unsigned short*    ) == *pMemberTypeInfo
     || typeid(unsigned int*      ) == *pMemberTypeInfo
     || typeid(unsigned long*     ) == *pMemberTypeInfo
     || typeid(unsigned long long*) == *pMemberTypeInfo)
    {
      isPtr   = true;
    }

    // ポインタかどうか ＆ 属性に応じてフォントカラーを設定
    RttiEditor::ColorRGB textColor;
    if     (  isPtr && nowIndex == m_scrollCnt )                                                        { textColor = DISABLE_EDIT_COLOR; }
    else if(  isPtr && nowIndex != m_scrollCnt )                                                        { textColor = DISABLE_VIEW_COLOR; }
    else if( !isPtr && !(memberAttribute & RttiParamManager::ATTR_READONLY) && nowIndex == m_scrollCnt ){ textColor = EDIT_COLOR; }
    else if( !isPtr && !(memberAttribute & RttiParamManager::ATTR_READONLY) && nowIndex != m_scrollCnt ){ textColor = VIEW_COLOR; }
    else if( !isPtr &&  (memberAttribute & RttiParamManager::ATTR_READONLY) && nowIndex == m_scrollCnt ){ textColor = DISABLE_EDIT_COLOR; }
    else if( !isPtr &&  (memberAttribute & RttiParamManager::ATTR_READONLY) && nowIndex != m_scrollCnt ){ textColor = DISABLE_VIEW_COLOR; }

    // メンバ変数に関連付けらられた表示名を描画
    const float  offsetY = executeCount * FONT_SIZE_Y;
    RTTI_DEBUG_FONT_PRINT( TEXT_NAME_X , TEXT_NAME_Y + offsetY, textColor , memberDisp );

    // メンバ変数の値を描画    
    if     ( typeid(         bool       ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%d"    , *(         bool*      )pAddress); }
    else if( typeid(         float      ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%.3f"  , *(         float*     )pAddress); }
    else if( typeid(         double     ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%.3lf" , *(         double*    )pAddress); }
    else if( typeid(         char       ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%c"    , *(         char*      )pAddress); }
    else if( typeid(         short      ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%d"    , *(         short*     )pAddress); }
    else if( typeid(         int        ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%d"    , *(         int*       )pAddress); }
    else if( typeid(         long       ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%ld"   , *(         long*      )pAddress); }
    else if( typeid(         long long  ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%lld"  , *(         long long* )pAddress); }
    else if( typeid(unsigned char       ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%d"    , *(unsigned char*      )pAddress); }
    else if( typeid(unsigned short      ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%u"    , *(unsigned short*     )pAddress); }
    else if( typeid(unsigned int        ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%u"    , *(unsigned int*       )pAddress); }
    else if( typeid(unsigned long       ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%lu"   , *(unsigned long*      )pAddress); }
    else if( typeid(unsigned long long  ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"%llu"  , *(unsigned long long* )pAddress); }
    else if( typeid(         bool*      ) == *pMemberTypeInfo
          || typeid(         float*     ) == *pMemberTypeInfo
          || typeid(         double*    ) == *pMemberTypeInfo
          || typeid(         char*      ) == *pMemberTypeInfo
          || typeid(         short*     ) == *pMemberTypeInfo
          || typeid(         int*       ) == *pMemberTypeInfo
          || typeid(         long*      ) == *pMemberTypeInfo
          || typeid(         long long* ) == *pMemberTypeInfo
          || typeid(unsigned char*      ) == *pMemberTypeInfo
          || typeid(unsigned short*     ) == *pMemberTypeInfo
          || typeid(unsigned int*       ) == *pMemberTypeInfo
          || typeid(unsigned long*      ) == *pMemberTypeInfo
          || typeid(unsigned long long* ) == *pMemberTypeInfo){ CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L"0x%x" , *(size_t*)pAddress);}
    // 当てはまる型がない場合
    else                                                      { CopyWstr(m_textBuffer, &textSize, TEXT_MAX_SIZE , L" "); }

    RTTI_DEBUG_FONT_PRINT( TEXT_VALUE_X , TEXT_VALUE_Y + offsetY , textColor , m_textBuffer );

    executeCount++;
  }

  //--------------------------------------------------------------
  // スクロールバーを描画
  //--------------------------------------------------------------
  if( maxSize >= maxLine )
  {
    float barHeight = (f32)maxLine/(f32)maxSize * (FONT_SIZE_Y);
    RttiEditor::ColorRGB	barColor(1.0f,1.0f,1.0f);
    RTTI_DEBUG_FONT_PRINT( SCROLL_BAR_X , SCROLL_BAR_Y + barHeight * (f32)m_scrollCnt , barColor , L"■" );
  }
}

//------------------------------------------------------------------
/**
* @brief   更新
* @return  なし
*/
//------------------------------------------------------------------
void RttiEditor::Update(void)
{
  // 表示リクエストがない場合は終了
  if( m_editorRequestList.Size() < 1 )
  {
    ClearEditor();
    return;
  }

  //--------------------------------------------------------------
  // 編集するタグを変更
  //--------------------------------------------------------------
  if( m_tagControl != TAG_NONE && m_editorRequestList.Size() > 1 )
  {
    m_scrollCnt = 0;

    // EditorRequestのコピーが走ります
    if( m_tagControl == TAG_NEXT ){ m_editorRequestList.PushBack ( *(  m_editorRequestList.Begin()) ); m_editorRequestList.PopFront(); }
    if( m_tagControl == TAG_PREV ){ m_editorRequestList.PushFront( *(--m_editorRequestList.End())   ); m_editorRequestList.PopBack();  }
  }

  // "タグが切り替わった" or "リクエストに増減があった" 場合は表示物リストを更新
  if( m_tagControl != TAG_NONE || m_isChangeRequestList )
  {
    for( gfl2::util::List<RttiParamManager::MemberProperty*>::Iterator iteListParam = m_dispMemberPropertyList.Begin() ; iteListParam != m_dispMemberPropertyList.End() ; iteListParam++ )
    {
      GFL_SAFE_DELETE(*iteListParam);
    }
    m_dispMemberPropertyList.Clear();
    m_dispMemberPropertyList.CreateBuffer(m_pHeap,m_bufferSize);

    // 対象となるクラスを取得
    gfl2::util::List<EditorRequest>::Iterator iteEditorRequest = m_editorRequestList.Begin();
    const type_info*  pTargetClassInfo = iteEditorRequest->_pTypeInfo;

    // 表示物を取得
    c16 baseName[RttiParamManager::MemberProperty::DISP_NAME_SIZE];
    baseName[0] = L'\0';
    RttiParamManager::getInstance()->getParamList( pTargetClassInfo,&m_dispMemberPropertyList,baseName );
  }

  //--------------------------------------------------------------
  // 編集するパラメータを変更
  //--------------------------------------------------------------
  if( m_paramControl != PARAM_NONE )
  {
    if( m_paramControl== PARAM_NEXT ){ if( m_scrollCnt < m_dispMemberPropertyList.Size() - 1 ) { m_scrollCnt++; } }
    if( m_paramControl== PARAM_PREV ){ if( m_scrollCnt > 0 )                                   { m_scrollCnt--; } }
  }

  //--------------------------------------------------------------
  // パラメータの値を変更
  //--------------------------------------------------------------
  if( m_valueControl != VALUE_NONE )
  {
    gfl2::util::List<RttiParamManager::MemberProperty*>::Iterator iteListParam = m_dispMemberPropertyList.Begin();
    for( unsigned index=0; index<m_scrollCnt ; index++ )
    {
      iteListParam++;
    }

    if( iteListParam != m_dispMemberPropertyList.End() )
    {
      // リクエストから対象となるインスタンスを取得
      gfl2::util::List<EditorRequest>::Iterator iteEditorRequest = m_editorRequestList.Begin();
      const void* pTargetClassAddress  = iteEditorRequest->_pInstance;

      // 対象となるインスタンスのメンバ変数情報を取得
      RttiParamManager::MemberProperty* pParam = *iteListParam;
      const uint64_t      memberAddressOffset = pParam->_offsetAddress;
      const unsigned int  memberAttribute     = pParam->_attribute;
      const type_info*    pMemberTypeInfo     = pParam->_pTypeInfo;

      if( !(memberAttribute & RttiParamManager::ATTR_READONLY) )
      {
        // 属性からメンバ変数に増減させる値を決定
        float fAddValue  = 1.0f * m_valueCoefficient;
        int   iAddValue  = 1     * (int)m_valueCoefficient;
        if( memberAttribute & RttiParamManager::ATTR_ADD_SMALLEST ){ fAddValue = 0.01f  * m_valueCoefficient;  }
        if( memberAttribute & RttiParamManager::ATTR_ADD_SMALL    ){ fAddValue = 0.1f   * m_valueCoefficient;  }
        if( memberAttribute & RttiParamManager::ATTR_ADD_BIG      ){ fAddValue = 10.0f  * m_valueCoefficient; iAddValue = 10  * (int)m_valueCoefficient; }
        if( memberAttribute & RttiParamManager::ATTR_ADD_BIGGEST  ){ fAddValue = 100.0f * m_valueCoefficient; iAddValue = 100 * (int)m_valueCoefficient; }

        // "インスタンスの先頭アドレス + メンバ変数のオフセットアドレス"で指定インスタンスのメンバへのアドレスを取得
        unsigned char* pAddress = (unsigned char*)pTargetClassAddress + memberAddressOffset;

        // 型を判別してメンバ情報を変更(コードサイズの計算がしやすいようテンプレート化していません)
        if     ( typeid(     bool    ) == *pMemberTypeInfo)
        {
          *(bool*)pAddress = *(bool*)pAddress==true ? false:true;
        }
        else if( typeid(     float    ) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(float*)pAddress += fAddValue; }
          if( m_valueControl == VALUE_PULL){ *(float*)pAddress -= fAddValue; }
        }
        else if( typeid(     double    ) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(double*)pAddress += fAddValue; }
          if( m_valueControl == VALUE_PULL){ *(double*)pAddress -= fAddValue; }
        }
        else if( typeid(     char    ) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(char*)pAddress += iAddValue; }
          if( m_valueControl == VALUE_PULL){ *(char*)pAddress -= iAddValue; }
        }
        else if( typeid(     short    ) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(short*)pAddress += iAddValue; }
          if( m_valueControl == VALUE_PULL){ *(short*)pAddress -= iAddValue; }
        }
        else if( typeid(     int    ) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(int*)pAddress += iAddValue; }
          if( m_valueControl == VALUE_PULL){ *(int*)pAddress -= iAddValue; }
        }
        else if( typeid(     long    ) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(long*)pAddress += iAddValue; }
          if( m_valueControl == VALUE_PULL){ *(long*)pAddress -= iAddValue; }
        }
        else if( typeid(     long long) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(long long*)pAddress += iAddValue; }
          if( m_valueControl == VALUE_PULL){ *(long long*)pAddress -= iAddValue; }
        }
        else if( typeid(unsigned char    ) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(unsigned char*)pAddress += iAddValue; }
          if( m_valueControl == VALUE_PULL){ *(unsigned char*)pAddress -= iAddValue; }
        }
        else if( typeid(unsigned short    ) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(unsigned short*)pAddress += iAddValue; }
          if( m_valueControl == VALUE_PULL){ *(unsigned short*)pAddress -= iAddValue; }
        }
        else if( typeid(unsigned int    ) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(unsigned int*)pAddress += iAddValue; }
          if( m_valueControl == VALUE_PULL){ *(unsigned int*)pAddress -= iAddValue; }
        }
        else if( typeid(unsigned long    ) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(unsigned long*)pAddress += iAddValue; }
          if( m_valueControl == VALUE_PULL){ *(unsigned long*)pAddress -= iAddValue; }
        }
        else if( typeid(unsigned long long) == *pMemberTypeInfo)
        {
          if( m_valueControl == VALUE_ADD ){ *(unsigned long long*)pAddress += iAddValue; }
          if( m_valueControl == VALUE_PULL){ *(unsigned long long*)pAddress -= iAddValue; }
        }
        // 当てはまる型がない場合は処理をしない
        else
        {
        }
      }
    }
  }

  m_isChangeRequestList = false;
  m_tagControl       = TAG_NONE;
  m_valueControl     = VALUE_NONE;
  m_paramControl     = PARAM_NONE;
}

//------------------------------------------------------------------
/**
* @brief   エディタの作成リクエスト
* @return  なし
*/
//------------------------------------------------------------------
void RttiEditor::CreateEditorFromInstace(c16* windowName,const type_info* pTypeInfo,void* pObj)
{
  if( !pObj )
  {
    return;
  }

  for( gfl2::util::List<EditorRequest>::Iterator iteEditorRequest=m_editorRequestList.Begin() ; iteEditorRequest!=m_editorRequestList.End() ; iteEditorRequest++ )
  {
    if( iteEditorRequest->_pInstance == pObj )
    {
      return;
    }
  }
  m_editorRequestList.PushBack( EditorRequest( windowName , pTypeInfo , pObj ) );
  m_isChangeRequestList = true;
}

//------------------------------------------------------------------
/**
* @brief   エディタの削除リクエスト
* @return  なし
*/
//------------------------------------------------------------------
void RttiEditor::DeleteEditorFromInstace(void* pObj)
{
  for( gfl2::util::List<EditorRequest>::Iterator iteEditorRequest=m_editorRequestList.Begin() ; iteEditorRequest!=m_editorRequestList.End() ; iteEditorRequest++ )
  {
    if( iteEditorRequest->_pInstance == pObj )
    {
      m_editorRequestList.Erase( iteEditorRequest );
      m_isChangeRequestList = true;
      return;
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  エディタに何も表示しない状態にする
* @return  なし
*/
//------------------------------------------------------------------
void RttiEditor::ClearEditor(void)
{
  for( gfl2::util::List<RttiParamManager::MemberProperty*>::Iterator iteListParam = m_dispMemberPropertyList.Begin() ; iteListParam != m_dispMemberPropertyList.End() ; iteListParam++ )
  {
    GFL_SAFE_DELETE( *iteListParam);
  }
  m_dispMemberPropertyList.Clear();

  if( m_pHeap )
  {
    m_dispMemberPropertyList.CreateBuffer(m_pHeap,m_bufferSize);
  }

  m_isChangeRequestList = false;
  m_tagControl          = TAG_NONE;
  m_valueControl        = VALUE_NONE;
  m_paramControl        = PARAM_NONE;
  m_scrollCnt           = 0;
}

//------------------------------------------------------------------
/**
* @brief   書式付き出力をワイド文字配列にコピー
* @return  なし
*/
//------------------------------------------------------------------
void RttiEditor::CopyWstr( wchar_t *pOut,int* pOutSize,size_t size,const wchar_t* wstr, ... )
{
  va_list argp;
  va_start(argp, wstr);
  *pOutSize = vswprintf( pOut, size, wstr, argp );
  va_end(argp);
}

} }

#endif
