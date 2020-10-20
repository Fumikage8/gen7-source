//=============================================================================
/**
 * @file    PrintSystem.cpp
 * @brief   文字列描画システム
 * @author  taya
 * @author  Hiroyuki Nakamura
 * @date    2011.03.08
 *
 * @li sangoのprintsys.cppを移植
 */
//=============================================================================
#include <str/include/gfl2_StrSys.h>
#include <str/include/gfl2_StrBuf.h>
#include <debug/include/gfl2_Assert.h>

#include <Print/include/PrintSystem.h>
#include <Print/include/MessageWindow.h>

#include "System/include/ArcIdGetter.h"
#include <arc_def_index/arc_def.h>



GFL_NAMESPACE_BEGIN( print )

/**
 *  文字コードシンボル（UTF-16）
 */
enum StrCodeSymbol {
  n0_ = 0xff10,
  n1_,
  n2_,
  n3_,
  n4_,
  n5_,
  n6_,
  n7_,
  n8_,
  n9_,
  spc_  = 0x3000,
  bou_  = 0x30fc,
  hate_ = 0xff1f,
  period_ = 0xff0e,

  h_n0_ = 0x0030,
  h_n1_,
  h_n2_,
  h_n3_,
  h_n4_,
  h_n5_,
  h_n6_,
  h_n7_,
  h_n8_,
  h_n9_,
  h_spc_  = 0x0020,
  h_bou_  = 0x002d,
  h_hate_ = 0x003f,
  h_period_ = 0x002e,

  a_ = 0xFF21,
  b_,
  c_,
  d_,
  e_,
  f_,

  h_a_ = 0x0041,
  h_b_,
  h_c_,
  h_d_,
  h_e_,
  h_f_,
};
/**
 *  ケタ数ごとの最低値
 */
static const u32 gNumLower[] =
{
  1,
  10,
  100,
  1000,
  10000,
  100000,
  1000000,
  10000000,
  100000000,
  1000000000,
};


//EFIGS時に横方向につぶす倍率
//デフォルトが0.64倍。最低値は0.5倍。
//nijiでは必要ない
const f32 PRINT_EFIGS_SCALE = 1.0f; //0.78125f; // 0.5/0.64      ///@fix



void strnum_core( gfl2::str::StrBuf* dst, s32 number, bool bMinus, u32 keta, NumberDispType dispType, NumberCodeType codeType );

//----------------------------------------------------------------------------------
/**
 * タグ開始コードを指している文字列から、そのタグのグループを取得
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指している必要がある）
 *
 * @retval  u8    タググループ
 */
//----------------------------------------------------------------------------------
u8 getTagGroup( const gfl2::str::STRCODE* sp )
{
  GFL_ASSERT_STOP( *sp == gfl2::str::TAG_START_CODE ); // @check

  if( *sp == gfl2::str::TAG_START_CODE )
  {
    u16 code = gfl2::str::GetTagCode( sp );
    return (((code)>>8)&0xff);
  }

  return TAGGROUP_NULL;
}


bool IsNameAreaTagGroup( const gfl2::str::STRCODE* sp )
{
  u8 type = getTagGroup( sp );
  if(type == TAGGROUP_WORD)
  {
    u8 tagIdx = GetTagIndex( sp );
    if( tagIdx == print::TAGIDX_WORD_TRAINER_NAME_FIELD)
    {
      return true;
    }
  }
  return false;
}

bool IsWordSetTagGroup( const gfl2::str::STRCODE* sp )
{
  u8 type = getTagGroup( sp );
  return ( (type == TAGGROUP_WORD) || (type==TAGGROUP_NUMBER) );
}

bool IsWordSetNumberTagGroup( const gfl2::str::STRCODE* sp )
{
  u8 type = getTagGroup( sp );
  return (type==TAGGROUP_NUMBER);
}

bool IsKoreanParticleTag( const gfl2::str::STRCODE* sp )
{
  u8 type = getTagGroup( sp );
  u8 index = GetTagIndex( sp );
  return ( (type == TAGGROUP_KOREAN) && (index == TAGIDX_KOREAN_PARTICLE) );
}

bool IsGrammarForceTag( const gfl2::str::STRCODE* sp )
{
  u8 group = getTagGroup( sp );
  return ( group == TAGGROUP_GRAMMAR_FORCE );
}

bool IsStringSelectTag( const gfl2::str::STRCODE* sp )
{
  u8 group = getTagGroup( sp );
  return ( group == TAGGROUP_STRING_SELECT );
}

bool IsWordFormTag( const gfl2::str::STRCODE* sp )
{
  u8 group = getTagGroup( sp );
  return ( (TAGGROUP_WORDFORM_START <= group) && (group < TAGGROUP_WORDFORM_MAX) );
}

u8 GetWordFormTagWordIndex( const gfl2::str::STRCODE* sp )
{
  GFL_ASSERT_STOP(IsWordFormTag(sp));
  u8 index = GetTagIndex( sp );
  return index;
}


//----------------------------------------------------------------------------------
/**
 * タグコードからタググループを取得
 *
 * @param   tagCode   タグコード
 *
 * @retval  TagGroup    タググループ
 */
//----------------------------------------------------------------------------------
TagGroup TagCodeToGroup( const gfl2::str::STRCODE tagCode )
{
  return static_cast<TagGroup>(((tagCode)>>8)&0xff);
}
//----------------------------------------------------------------------------------
/**
 * タグコードからタグIndexを取得
 *
 * @param   tagCode   タグコード
 *
 * @retval  u8        タグIndex（グループ内の番号）
 */
//----------------------------------------------------------------------------------
u8 TagCodeToIndex( const gfl2::str::STRCODE tagCode )
{
  return (tagCode&0xff);
}

//----------------------------------------------------------------------------------
/**
 * タグ開始コードを指している文字列から、そのタグのグループを取得
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指している必要がある）
 *
 * @retval  u8    タググループ
 */
//----------------------------------------------------------------------------------
TagGroup GetTagGroup( const gfl2::str::STRCODE* sp )
{
  u16 tagCode = gfl2::str::GetTagCode( sp );
  return TagCodeToGroup( tagCode );
}

//----------------------------------------------------------------------------------
/**
 * タグ開始コードを指している文字列から、そのタグのグループ内インデックスを返す
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指している必要がある）
 *
 * @retval  u8    グループ内インデックス
 */
//----------------------------------------------------------------------------------
u8 GetTagIndex( const gfl2::str::STRCODE* sp )
{
  u16 tagCode = gfl2::str::GetTagCode( sp );
  return TagCodeToIndex( tagCode );
}

//----------------------------------------------------------------------------------
/**
 * タググループID、グループ内Index を合成してタグコード化する
 *
 * @param   groupID
 * @param   index
 *
 * @retval  gfl2::str::STRCODE
 */
//----------------------------------------------------------------------------------
gfl2::str::STRCODE CreateTagCode( TagGroup groupID, u8 index )
{
  gfl2::str::STRCODE code = ((groupID & 0xff)<< 8) | (index & 0xff);
  return code;
}


//----------------------------------------------------------------------------------
/**
 * タグ開始コードを指している文字列から、そのタグのタイプを取得
 *
 * @param   sp            文字列ポインタ（タグ開始コードを指している必要がある）
 * @param   paramIndex    パラメータインデックス
 *
 * @retval  u16   タグタイプ
 */
//----------------------------------------------------------------------------------
u16 GetTagParam( const gfl2::str::STRCODE* sp, u32 paramIndex )
{
  return gfl2::str::GetTagParam( sp, paramIndex );
}


//=============================================================================================
/**
 * 数値の文字列化コア部分
 *
 * @param   dst
 * @param   number
 * @param   keta
 * @param   dispType
 * @param   codeType
 */
//=============================================================================================
void strnum_core( gfl2::str::StrBuf* dst, s32 number, bool bMinus, u32 keta, NumberDispType dispType, NumberCodeType codeType )
{
  static const gfl2::str::STRCODE sc_zen[] = {n0_,n1_,n2_,n3_,n4_,n5_,n6_,n7_,n8_,n9_,hate_};
  static const gfl2::str::STRCODE sc_han[] = {h_n0_,h_n1_,h_n2_,h_n3_,h_n4_,h_n5_,h_n6_,h_n7_,h_n8_,h_n9_,h_hate_};

  const gfl2::str::STRCODE* sctbl = (codeType==NUM_CODE_ZENKAKU)? sc_zen : sc_han;

  u32  num_f, num_i, i;

  if( bMinus )
  {
    dst->AddCode( (codeType==NUM_CODE_ZENKAKU)? bou_ : h_bou_ );
  }


  for(i=gNumLower[keta-1]; i>0; i/=10)
  {
    num_i = (u16)( number / i );
    num_f = (u32)( number - ( i * num_i ) );

    if(num_i >= 10)
    {
      num_i = 10;
    }

    if( dispType == NUM_DISP_ZERO )
    {
      dst->AddCode( sctbl[num_i] );
    }
    else if( num_i != 0 || i==1 )
    {
      dispType = NUM_DISP_ZERO;
      dst->AddCode( sctbl[num_i] );
    }
    else if( dispType == NUM_DISP_SPACE )
    {
      dst->AddCode( (codeType==NUM_CODE_ZENKAKU)? spc_ : h_spc_ );
    }

    number = num_f;
  }

}

//----------------------------------------------------------------------------------
/**
 * 数値の文字列化（符号付き32bit整数）
 *
 * @param[out]   dst
 * @param[in]    number   数値
 * @param[in]    keta     最大ケタ数
 * @param[in]    dispType 整形タイプ
 * @param[in]    codeType 文字コードタイプ
 */
//----------------------------------------------------------------------------------
void StrNumber( gfl2::str::StrBuf* dst, s32 number, u32 keta, NumberDispType dispType, NumberCodeType codeType )
{
  bool bMinus;

  if ( number < 0 ){
    bMinus = true;
    number *= -1;
  }
  else
  {
    bMinus = false;
  }

  dst->Clear();
  strnum_core( dst, number, bMinus, keta, dispType, codeType );
}

//----------------------------------------------------------------------------------
/**
 * 数値の文字列化（符号付き double ）
 *
 * @param[out]   dst
 * @param[in]    number    数値
 * @param[in]    keta_int  整数部最大ケタ数
 * @param[in]    keta_dec  小数部ケタ数
 * @param[in]    dispType  整形タイプ
 * @param[in]    codeType  文字コードタイプ
 */
//----------------------------------------------------------------------------------
void StrRealNumber( gfl2::str::StrBuf* dst, double number, u32 keta_int, u32 keta_dec, NumberDispType dispType, NumberCodeType codeType )
{
  s32  integer = static_cast<s32>(number);
  bool bMinus;
  if( number < 0 ){
    bMinus = true;
    integer *= -1;
  }else{
    bMinus = false;
  }

  dst->Clear();
  strnum_core( dst, integer, bMinus, keta_int, dispType, codeType );

  if( keta_dec )
  {
    if( keta_dec < GFL_NELEMS(gNumLower) )
    {
      u32 n = gNumLower[ keta_dec ];

      if( number < 0 ){
        number *= -1;
      }
      s32 decimal = static_cast<s32>(number * n + 0.5f);
      s32 minus;

      minus = ( decimal / n ) * n;
      decimal -= minus;

      dst->AddCode( (codeType==NUM_CODE_ZENKAKU)? period_ : h_period_ );
      strnum_core( dst, decimal, false, keta_dec, NUM_DISP_ZERO, codeType );
    }
    else
    {
      GFL_ASSERT_MSG( 0, "keta_dec=%d", keta_dec );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * レイアウトのテキストボックスに、文字列を設定する（タグ考慮）
 *
 * @param   target
 * @param   str
 * @param   langId 言語ID(EFIGSの縮小対応)sv_config内の言語IDを入れる
 *                 デフォルト引数でMsgDataから自動取得
 */
//----------------------------------------------------------------------------------
static FontTagProc sTagProc;
void SetStringForLytTextBox( gfl2::lyt::LytTextBox * target, const gfl2::str::StrBuf* str , System::MSGLANGID langId )
{
  const gfl2::str::STRCODE* sp = str->GetPtr();
  SetStringForLytTextBox( target , sp , langId );
}

void SetStringForLytTextBox( gfl2::lyt::LytTextBox * target, const wchar_t* str , System::MSGLANGID langId )
{
  SetStringForLytTextBoxEx( target , str , NULL , langId );
}

//----------------------------------------------------------------------------------
/**
 * レイアウトのテキストボックスに、文字列を設定するためのワーク作成
 * 特殊タグ用(右寄せタグ、センタリングタグに必要)
 *
 * @param   gfl::heap::HeapBase *heap ヒープ
 *
 * @retval  LYT_TEXT_EX_WORK*   ワーク
 */
//----------------------------------------------------------------------------------
LYT_TEXT_EX_WORK* CreateLytTextExWork( gfl2::heap::HeapBase *heap )
{
  LYT_TEXT_EX_WORK *work = GFL_NEW(heap)LYT_TEXT_EX_WORK;
  work->tagProc = GFL_NEW(heap) FontTagProc();
  
  return work;
}

//----------------------------------------------------------------------------------
/**
 * レイアウトのテキストボックスに、文字列を設定するためのワーク削除
 * 特殊タグ用(右寄せタグ、センタリングタグに必要)
 *
 * @param   LYT_TEXT_EX_WORK*   ワーク
 */
//----------------------------------------------------------------------------------
void DeleteLytTextExWork( LYT_TEXT_EX_WORK* work )
{
  if( work )
  {
    GFL_SAFE_DELETE( work->tagProc );
    GFL_SAFE_DELETE( work );
  }
}

//----------------------------------------------------------------------------------
/**
 * レイアウトのテキストボックスに、文字列を設定する（特殊タグ考慮）右寄せタグ、センタリングタグに必要
 *
 * @param   target
 * @param   str
 * @param   langId 言語ID(EFIGSの縮小対応)sv_config内の言語IDを入れる
 *                 デフォルト引数でMsgDataから自動取得
 */
//----------------------------------------------------------------------------------
void SetStringForLytTextBoxEx( gfl2::lyt::LytTextBox * target, const gfl2::str::StrBuf* str , LYT_TEXT_EX_WORK *work , System::MSGLANGID langId )
{
  const gfl2::str::STRCODE* sp = str->GetPtr();
  SetStringForLytTextBoxEx( target , sp , work , langId );
}
void SetStringForLytTextBoxEx( gfl2::lyt::LytTextBox * target, const wchar_t* str , LYT_TEXT_EX_WORK *work , System::MSGLANGID langId )
{
  FontTagProc *tagProc;
  if( work )
  {
    tagProc = work->tagProc;
  }
  else
  {
    tagProc = &sTagProc;
  }


  tagProc->ResetParam();
  
  if( work )
  {
    nw::lyt::Size size = target->GetSize();
    tagProc->SetWindowWidth( static_cast<u32>(size.width) );
  }
  else
  {
    tagProc->SetWindowWidth( 0 );
  }

  target->SetTagProcessor( tagProc );

  System::MSGLANGID langIdTemp = langId;
  if( langIdTemp == System::MSGLANGID_MAX )
  {
    langIdTemp = GetMessageLangId();
  }

  const nw::lyt::res::ExtUserData *extData = target->FindExtUserDataByName("g_fw");
  
  if( extData == NULL )
  {
    extData = target->FindExtUserDataByName("g2d_fw");
    if( extData )
    {
      GFL_PRINT("古いg2d_fw定義です。コンバートしなおしてください。\n");
    }
  }
  
  if( extData )
  {

    f32 defWidth = extData->GetFloatArray()[0];
    f32 fontWidth = defWidth;

    if( CheckEnableShortStr( target,langIdTemp ) )
    {
//      fontWidth = defWidth * (PRINT_EFIGS_SCALE);
    }

    int strLen = GetStringWidth( str, target->GetFont(), fontWidth );
    const f32 winWidth = target->GetSize().width;

#if PM_DEBUG
#if 0   // @note デバッグ処理未実装
    //幅チェックの色がリセットされないので、システムチェックの位置を幅チェックの前にしてリセット代わりにする。
    //幅あふれの時は紫、そうでないときは黄緑になる。
    if( GFL_SINGLETON_INSTANCE(xy_debug::DebugWinWork)->mMsgSystemCheck )
    {
      target->SetTextColor(nw::lyt::TEXTCOLOR_TOP    ,nw::ut::Color8::YELLOW );
      target->SetTextColor(nw::lyt::TEXTCOLOR_BOTTOM ,nw::ut::Color8::GREEN );
    }
#endif
#endif

    if( strLen > winWidth )
    {
      GFL_PRINT("[PrintSys] 表示領域をはみ出ました！ strLen=%d, winWidth=%d\n", strLen, (int)(winWidth));

      fontWidth = fontWidth * (winWidth/strLen);
      //はみ出ることがあったので少しサバを読む
      fontWidth -= 0.05f;
#if PM_DEBUG
#if 0   // @note デバッグ処理未実装
      if( GFL_SINGLETON_INSTANCE(xy_debug::DebugWinWork)->mMsgWidthCheck )
      {
        target->SetTextColor(nw::lyt::TEXTCOLOR_TOP    ,nw::ut::Color8::RED );
        target->SetTextColor(nw::lyt::TEXTCOLOR_BOTTOM ,nw::ut::Color8::BLUE );
      }
#endif
#endif
    }

    //GFL_PRINT("len[%d]win[%f]\n",GetStringWidth( str , target->GetFont() , fontWidth ),winWidth);

    nw::lyt::Size size = target->GetFontSize();
    size.width = fontWidth;
    target->SetFontSize(size);
  }
  else
  {
    GFL_PRINT("[PrintSys] Userdata not found!\n");
  }

  if( gfl2::str::StrLen(str) > target->GetStringBufferLength() )
  {
    GFL_PRINT("Layout String Size(%d) < Msg String Size(%d)\n", target->GetStringBufferLength(), gfl2::str::StrLen(str) );
    GFL_ASSERT_MSG(0,"message buffer不足\n");
    target->SetString( L"", 0, 0 ); //空文字列をセットする
  }
  else
  {
    target->SetString( str, 0, gfl2::str::StrLen(str) );
  }
}

//----------------------------------------------------------------------------------
/**
 * 文字列の長さを取得する。(改行解釈済み)
 *
 * @param   str       文字列
 * @param   pane      テキストボックスペイン
 * @param   langId 言語ID(EFIGSの縮小対応)sv_config内の言語IDを入れる
 *                 デフォルト引数で自動取得
 */
//----------------------------------------------------------------------------------
int GetStringWidth( const gfl2::str::StrBuf * str, gfl2::lyt::LytTextBox * pane , System::MSGLANGID langId )
{
  System::MSGLANGID langIdTemp = langId;
  if( langIdTemp == System::MSGLANGID_MAX )
  {
    langIdTemp = GetMessageLangId();
  }

  f32 width = static_cast<f32>( pane->GetFont()->GetWidth() );

  const nw::lyt::res::ExtUserData *extData = pane->FindExtUserDataByName("g_fw");
  if( extData )
  {
    width = extData->GetFloatArray()[0];
  }
  else
  {
    extData = pane->FindExtUserDataByName("g2d_fw");
    if( extData )
    {
      width = extData->GetFloatArray()[0];
      GFL_PRINT("古いg2d_fw定義です。コンバートしなおしてください。\n");
    }
  }
    
  if( CheckEnableShortStr( pane,langIdTemp ) )
  {
//    width = width * (PRINT_EFIGS_SCALE);
  }

  return GetStringWidth( str , pane->GetFont() , width );
}

//----------------------------------------------------------------------------------
/**
 * 文字列の長さを調べる。(改行時、最大行を見る)
 *
 * @param   str       文字列
 * @param   font      フォントファイル
 * @param   fontWidth フォント幅(スケール済みのサイズを入れてください)
 */
//----------------------------------------------------------------------------------
int GetStringWidth( const gfl2::str::StrBuf* str, const nw::font::Font *font, f32 fontWidth, int *retLine )
{
  // デフォルト処理後、領域幅固定タグをストリームウィンドウ用に置換する
  const gfl2::str::STRCODE* sp = (str->GetPtr());
  return GetStringWidth( sp, font, fontWidth, retLine );
}

int GetStringWidth( const wchar_t* str, const nw::font::Font *font, f32 fontWidth, int *retLine )
{
  f32 str_width_max = 0;
  f32 str_width = 0;
  s32 line = 0;

  f32 fontRatio = fontWidth / (f32)(font->GetWidth());

  while( *str != gfl2::str::EOM_CODE )
  {
    // 幅リセット
    if( *str == gfl2::str::CR_CODE )
    {
      // 最大文字幅をチェック
      if( str_width_max < str_width )
      {
        str_width_max = str_width;
      }

      str_width = 0;
      line++;
      ++str;
    }
    // 通常文字
    else if( *str != gfl2::str::TAG_START_CODE )
    {
      // 幅を計算後文字を進める
      const int width = font->GetCharWidth( *str );
      const f32  f_width = (f32)width * fontRatio;

      str_width += f_width;
      ++str;
    }
    // タグ
    else
    {
      // @note タグを解析してより正確な文字幅を計算する必要がある
      // タグをスキップし、幅を計算

      TagGroup  tagGroup = GetTagGroup( str );
      u32       tagIdx   = GetTagIndex( str );
      //改行タグをチェック
      if( tagGroup == TAGGROUP_STREAM_CTRL &&
         (tagIdx == TAGIDX_STREAM_LINE_FEED || tagIdx == TAGIDX_STREAM_PAGE_CLEAR ) )
      {
        if( str_width_max < str_width )
        {
          str_width_max = str_width;
        }
        line++;
        str_width = 0;
      }
      
      str = gfl2::str::SkipTagCode( str );
      //ここで文字幅をたすと、連続でタグが入っているとまずい。
    }
  }

  // 最終行をチェック
  if( str_width_max < str_width )
  {
    str_width_max = str_width;
  }
  
  if( retLine )
  {
    *retLine = line+1;
  }
  
  return static_cast<int>(gfl2::math::FCeil(str_width_max));
}

//----------------------------------------------------------------------------------
/**
 * 文字列のスケール値を取得する(DrawUtil等独自の文字描画をする時用!)
 *
 * @param   langId 言語ID(EFIGSの縮小対応)sv_config内の言語IDを入れる
 *                 デフォルト引数で自動取得
 */
//----------------------------------------------------------------------------------
f32 GetStringScale( System::MSGLANGID langId )
{
  System::MSGLANGID langIdTemp = langId;
  if( langIdTemp == System::MSGLANGID_MAX )
  {
    langIdTemp = GetMessageLangId();
  }
  if( CheckEnableShortStrLang( langIdTemp ) )
  {
    return PRINT_EFIGS_SCALE;
  }
  return 1.0f;
}


//Msgファイル関係
static System::MSGLANGID mMsgLangId;
//----------------------------------------------------------------------------------
/**
 * メッセージの言語IDを設定する。
 *
 * @param   langId       言語ID(ROMの本体の言語IDとは違います！)
 */
//----------------------------------------------------------------------------------
void SetMessageLangId( System::MSGLANGID langId )
{
  mMsgLangId = langId;
}

//----------------------------------------------------------------------------------
/**
 * メッセージの言語IDを取得する。
 *
 * @return  言語ID(ROMの本体の言語IDとは違います！)
 */
//----------------------------------------------------------------------------------
System::MSGLANGID GetMessageLangId(void)
{
  return mMsgLangId ;
}

//----------------------------------------------------------------------------------
/**
 * 通常のメッセージのARCIDを取得する
 *
 * @param   langId       言語ID(ROMの本体の言語IDとは違います！)
 */
//----------------------------------------------------------------------------------
u32 GetMessageArcId( System::MSGLANGID langId )
{
  System::MSGLANGID tempLangId = langId;
  if( tempLangId == System::MSGLANGID_MAX )
  {
    tempLangId = mMsgLangId;
  }
//  OHNO_PRINT("GetMessageArcId %d\n",tempLangId);
  static const u32 arcIdArr[] =
  {
    NIJI_ARCID_NUM( ARCID_MESSAGE_JPN ),
    NIJI_ARCID_NUM( ARCID_MESSAGE_KANJI ),
    NIJI_ARCID_NUM( ARCID_MESSAGE_ENG ),
    NIJI_ARCID_NUM( ARCID_MESSAGE_FRE ),
    NIJI_ARCID_NUM( ARCID_MESSAGE_ITA ),
    NIJI_ARCID_NUM( ARCID_MESSAGE_GER ),
    NIJI_ARCID_NUM( ARCID_MESSAGE_SPA ),
    NIJI_ARCID_NUM( ARCID_MESSAGE_KOR ),
    NIJI_ARCID_NUM( ARCID_MESSAGE_CHN ),
    NIJI_ARCID_NUM( ARCID_MESSAGE_TWN ),
  };
  GFL_ASSERT_STOP( tempLangId < GFL_NELEMS(arcIdArr) );
  return arcIdArr[tempLangId];
}

//----------------------------------------------------------------------------------
/**
 * デバッグ用のメッセージのARCIDを取得する
 *
 * @param   langId       言語ID(ROMの本体の言語IDとは違います！)
 */
//----------------------------------------------------------------------------------
#if PM_DEBUG
u32 GetMessageArcId_Debug( System::MSGLANGID langId )
{
  System::MSGLANGID tempLangId = langId;
  if( tempLangId == System::MSGLANGID_MAX )
  {
    tempLangId = mMsgLangId;
  }
  static const u32 arcIdArr[] =
  {
    NIJI_ARCID_NUM( ARCID_DEBUG_MESSAGE_JPN ),
    NIJI_ARCID_NUM( ARCID_DEBUG_MESSAGE_KANJI ),
    NIJI_ARCID_NUM( ARCID_DEBUG_MESSAGE_ENG ),
    NIJI_ARCID_NUM( ARCID_DEBUG_MESSAGE_FRE ),
    NIJI_ARCID_NUM( ARCID_DEBUG_MESSAGE_ITA ),
    NIJI_ARCID_NUM( ARCID_DEBUG_MESSAGE_GER ),
    NIJI_ARCID_NUM( ARCID_DEBUG_MESSAGE_SPA ),
    NIJI_ARCID_NUM( ARCID_DEBUG_MESSAGE_KOR ),
    NIJI_ARCID_NUM( ARCID_DEBUG_MESSAGE_CHN ),
    NIJI_ARCID_NUM( ARCID_DEBUG_MESSAGE_TWN ),
  };
  return arcIdArr[tempLangId];
}
#endif  // PM_DEBUG

//----------------------------------------------------------------------------------
/**
 * スクリプト用のメッセージのARCIDを取得する
 *
 * @param   langId       言語ID(ROMの本体の言語IDとは違います！)
 */
//----------------------------------------------------------------------------------
u32 GetMessageArcId_Script( System::MSGLANGID langId )
{
  System::MSGLANGID tempLangId = langId;
  if( tempLangId == System::MSGLANGID_MAX )
  {
    tempLangId = mMsgLangId;
  }
  static const u32 arcIdArr[] =
  {
    NIJI_ARCID_NUM( ARCID_SCRIPT_MESSAGE_JPN ),
    NIJI_ARCID_NUM( ARCID_SCRIPT_MESSAGE_KANJI ),
    NIJI_ARCID_NUM( ARCID_SCRIPT_MESSAGE_ENG ),
    NIJI_ARCID_NUM( ARCID_SCRIPT_MESSAGE_FRE ),
    NIJI_ARCID_NUM( ARCID_SCRIPT_MESSAGE_ITA ),
    NIJI_ARCID_NUM( ARCID_SCRIPT_MESSAGE_GER ),
    NIJI_ARCID_NUM( ARCID_SCRIPT_MESSAGE_SPA ),
    NIJI_ARCID_NUM( ARCID_SCRIPT_MESSAGE_KOR ),
    NIJI_ARCID_NUM( ARCID_SCRIPT_MESSAGE_CHN ),
    NIJI_ARCID_NUM( ARCID_SCRIPT_MESSAGE_TWN ),
  };
  return arcIdArr[tempLangId];
}


//内部用
//----------------------------------------------------------------------------------
/**
 * 文字列を縮めるかを判定する
 *
 * @param   target      テキストボックスペイン
 * @param   langId      言語ID(sv_config)
 */
//----------------------------------------------------------------------------------
bool CheckEnableShortStr( gfl2::lyt::LytTextBox * target, System::MSGLANGID langId )
{
  //レイアウトの拡張データの名前
  const char *const FONT_NO_SHORT_STR = "n_sh";

  int noShortSetiing = 0;
  
  if( target )
  {
    const nw::lyt::res::ExtUserData *extData = target->FindExtUserDataByName(FONT_NO_SHORT_STR);
    if( extData )
    {
      noShortSetiing = extData->GetIntArray()[0];
    }
    else
    {
      //古い定義対応
      const char *const FONT_NO_SHORT_STR_TEMP = "no_short";
      extData = target->FindExtUserDataByName(FONT_NO_SHORT_STR_TEMP);
      if( extData )
      {
        noShortSetiing = extData->GetIntArray()[0];
        GFL_PRINT("古いno_short定義です。データをコンバートしなおしてください。\n");
      }
    }
  }

  if( noShortSetiing == 0 )
  {
    return CheckEnableShortStrLang( langId );
  }

  return false;
}

bool CheckEnableShortStrLang( System::MSGLANGID langId )
{
  if( langId == System::MSGLANGID_ENGLISH ||
      langId == System::MSGLANGID_FRENCH  ||
      langId == System::MSGLANGID_ITALIAN ||
      langId == System::MSGLANGID_GERMAN  ||
      langId == System::MSGLANGID_SPANISH )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END( print )
