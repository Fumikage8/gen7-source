//=============================================================================
/**
 * @file    PrintSystem.cpp
 * @brief   ������`��V�X�e��
 * @author  taya
 * @author  Hiroyuki Nakamura
 * @date    2011.03.08
 *
 * @li sango��printsys.cpp���ڐA
 */
//=============================================================================
#include <str/include/gfl2_StrSys.h>
#include <str/include/gfl2_StrBuf.h>
#include <debug/include/gfl2_Assert.h>

#include <../include/PrintSystem.h>
#include <../include/MessageWindow.h>

#include <arc_def_index/arc_def.h>


#if defined(DEBUG_ONLY_FOR_tamada) || defined(DEBUG_ONLY_FOR_matsushima) || defined(DEBUG_ONLY_FOR_ito_hiroto) || defined(DEBUG_ONLY_FOR_hiro_nakamura)
#define ASSERT_LAYOUT_BUFFER_OVER
#endif

GFL_NAMESPACE_BEGIN( print )

/**
 *  �����R�[�h�V���{���iUTF-16�j
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
 *  �P�^�����Ƃ̍Œ�l
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


//EFIGS���ɉ������ɂԂ��{��
//�f�t�H���g��0.64�{�B�Œ�l��0.5�{�B
const f32 PRINT_EFIGS_SCALE = 0.78125f; // 0.5/0.64

// �e�L�X�g�{�b�N�X�y�C���Ɏw�肳��Ă��镶���̃X�P�[���l
// DrawUtil�ŕ�����`�悷��ۂɎg�p���Ă�������
const f32 LAYOUT_STRING_SCALE_X = 0.64f;    ///< ������X�X�P�[���l
const f32 LAYOUT_STRING_SCALE_Y = 0.68f;    ///< ������Y�X�P�[���l
const f32 LAYOUT_NUMBER_SCALE_X = 0.56f;    ///< ������X�X�P�[���l�i�����݂̂̏ꍇ�͂�����j
const f32 LAYOUT_NUMBER_SCALE_Y = 0.68f;    ///< ������Y�X�P�[���l�i�����݂̂̏ꍇ�͂�����j


void strnum_core( gfl2::str::StrBuf* dst, s32 number, bool bMinus, u32 keta, NumberDispType dispType, NumberCodeType codeType );

//----------------------------------------------------------------------------------
/**
 * �^�O�J�n�R�[�h���w���Ă��镶���񂩂�A���̃^�O�̃O���[�v���擾
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 *
 * @retval  u8    �^�O�O���[�v
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
 * �^�O�R�[�h����^�O�O���[�v���擾
 *
 * @param   tagCode   �^�O�R�[�h
 *
 * @retval  TagGroup    �^�O�O���[�v
 */
//----------------------------------------------------------------------------------
TagGroup TagCodeToGroup( const gfl2::str::STRCODE tagCode )
{
  return static_cast<TagGroup>(((tagCode)>>8)&0xff);
}
//----------------------------------------------------------------------------------
/**
 * �^�O�R�[�h����^�OIndex���擾
 *
 * @param   tagCode   �^�O�R�[�h
 *
 * @retval  u8        �^�OIndex�i�O���[�v���̔ԍ��j
 */
//----------------------------------------------------------------------------------
u8 TagCodeToIndex( const gfl2::str::STRCODE tagCode )
{
  return (tagCode&0xff);
}

//----------------------------------------------------------------------------------
/**
 * �^�O�J�n�R�[�h���w���Ă��镶���񂩂�A���̃^�O�̃O���[�v���擾
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 *
 * @retval  u8    �^�O�O���[�v
 */
//----------------------------------------------------------------------------------
TagGroup GetTagGroup( const gfl2::str::STRCODE* sp )
{
  u16 tagCode = gfl2::str::GetTagCode( sp );
  return TagCodeToGroup( tagCode );
}

//----------------------------------------------------------------------------------
/**
 * �^�O�J�n�R�[�h���w���Ă��镶���񂩂�A���̃^�O�̃O���[�v���C���f�b�N�X��Ԃ�
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 *
 * @retval  u8    �O���[�v���C���f�b�N�X
 */
//----------------------------------------------------------------------------------
u8 GetTagIndex( const gfl2::str::STRCODE* sp )
{
  u16 tagCode = gfl2::str::GetTagCode( sp );
  return TagCodeToIndex( tagCode );
}

//----------------------------------------------------------------------------------
/**
 * �^�O�O���[�vID�A�O���[�v��Index ���������ă^�O�R�[�h������
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
 * �^�O�J�n�R�[�h���w���Ă��镶���񂩂�A���̃^�O�̃^�C�v���擾
 *
 * @param   sp            ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 * @param   paramIndex    �p�����[�^�C���f�b�N�X
 *
 * @retval  u16   �^�O�^�C�v
 */
//----------------------------------------------------------------------------------
u16 GetTagParam( const gfl2::str::STRCODE* sp, u32 paramIndex )
{
  return gfl2::str::GetTagParam( sp, paramIndex );
}


//=============================================================================================
/**
 * ���l�̕����񉻃R�A����
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
 * ���l�̕����񉻁i�����t��32bit�����j
 *
 * @param[out]   dst
 * @param[in]    number   ���l
 * @param[in]    keta     �ő�P�^��
 * @param[in]    dispType ���`�^�C�v
 * @param[in]    codeType �����R�[�h�^�C�v
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
 * ���l�̕����񉻁i�����t�� double �j
 *
 * @param[out]   dst
 * @param[in]    number    ���l
 * @param[in]    keta_int  �������ő�P�^��
 * @param[in]    keta_dec  �������P�^��
 * @param[in]    dispType  ���`�^�C�v
 * @param[in]    codeType  �����R�[�h�^�C�v
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
 * ���C�A�E�g�̃e�L�X�g�{�b�N�X�ɁA�������ݒ肷��i�^�O�l���j
 *
 * @param   target
 * @param   str
 * @param   langId ����ID(EFIGS�̏k���Ή�)sv_config���̌���ID������
 *                 �f�t�H���g������MsgData���玩���擾
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
 * ���C�A�E�g�̃e�L�X�g�{�b�N�X�ɁA�������ݒ肷�邽�߂̃��[�N�쐬
 * ����^�O�p(�E�񂹃^�O�A�Z���^�����O�^�O�ɕK�v)
 *
 * @param   gfl::heap::HeapBase *heap �q�[�v
 *
 * @retval  LYT_TEXT_EX_WORK*   ���[�N
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
 * ���C�A�E�g�̃e�L�X�g�{�b�N�X�ɁA�������ݒ肷�邽�߂̃��[�N�폜
 * ����^�O�p(�E�񂹃^�O�A�Z���^�����O�^�O�ɕK�v)
 *
 * @param   LYT_TEXT_EX_WORK*   ���[�N
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
 * ���C�A�E�g�̃e�L�X�g�{�b�N�X�ɁA�������ݒ肷��i����^�O�l���j�E�񂹃^�O�A�Z���^�����O�^�O�ɕK�v
 *
 * @param   target
 * @param   str
 * @param   langId ����ID(EFIGS�̏k���Ή�)sv_config���̌���ID������
 *                 �f�t�H���g������MsgData���玩���擾
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
      GFL_PRINT("�Â�g2d_fw��`�ł��B�R���o�[�g���Ȃ����Ă��������B\n");
    }
  }
  
  if( extData )
  {

    f32 defWidth = extData->GetFloatArray()[0];
    f32 fontWidth = defWidth;

    if( CheckEnableShortStr( target,langIdTemp ) )
    {
      fontWidth = defWidth * (PRINT_EFIGS_SCALE);
    }

    int strLen = GetStringWidth( str, target->GetFont(), fontWidth );
    const f32 winWidth = target->GetSize().width;

#if PM_DEBUG
#if 0   // @todo �f�o�b�O����������
    //���`�F�b�N�̐F�����Z�b�g����Ȃ��̂ŁA�V�X�e���`�F�b�N�̈ʒu�𕝃`�F�b�N�̑O�ɂ��ă��Z�b�g����ɂ���B
    //�����ӂ�̎��͎��A�����łȂ��Ƃ��͉��΂ɂȂ�B
    if( GFL_SINGLETON_INSTANCE(xy_debug::DebugWinWork)->mMsgSystemCheck )
    {
      target->SetTextColor(nw::lyt::TEXTCOLOR_TOP    ,nw::ut::Color8::YELLOW );
      target->SetTextColor(nw::lyt::TEXTCOLOR_BOTTOM ,nw::ut::Color8::GREEN );
    }
#endif
#endif

    if( strLen > winWidth )
    {
      GFL_PRINT("[PrintSys] �\���̈���͂ݏo�܂����I strLen=%d, winWidth=%d\n", strLen, (int)(winWidth));

      fontWidth = fontWidth * (winWidth/strLen);
      //�͂ݏo�邱�Ƃ��������̂ŏ����T�o��ǂ�
      fontWidth -= 0.05f;
#if PM_DEBUG
#if 0   // @todo �f�o�b�O����������
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
#ifdef  ASSERT_LAYOUT_BUFFER_OVER
    GFL_ASSERT(0);
#endif
    target->SetString( L"", 0, 0 ); //�󕶎�����Z�b�g����
  }
  else
  {
    target->SetString( str, 0, gfl2::str::StrLen(str) );
  }
}

//----------------------------------------------------------------------------------
/**
 * ������̒������擾����B(���s���ߍς�)
 *
 * @param   str       ������
 * @param   pane      �e�L�X�g�{�b�N�X�y�C��
 * @param   langId ����ID(EFIGS�̏k���Ή�)sv_config���̌���ID������
 *                 �f�t�H���g�����Ŏ����擾
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
      GFL_PRINT("�Â�g2d_fw��`�ł��B�R���o�[�g���Ȃ����Ă��������B\n");
    }
  }
    
  if( CheckEnableShortStr( pane,langIdTemp ) )
  {
    width = width * (PRINT_EFIGS_SCALE);
  }

  return GetStringWidth( str , pane->GetFont() , width );
}

//----------------------------------------------------------------------------------
/**
 * ������̒����𒲂ׂ�B(���s���A�ő�s������)
 *
 * @param   str       ������
 * @param   font      �t�H���g�t�@�C��
 * @param   fontWidth �t�H���g��(�X�P�[���ς݂̃T�C�Y�����Ă�������)
 */
//----------------------------------------------------------------------------------
int GetStringWidth( const gfl2::str::StrBuf* str, const nw::font::Font *font, f32 fontWidth, int *retLine )
{
  // �f�t�H���g������A�̈敝�Œ�^�O���X�g���[���E�B���h�E�p�ɒu������
  const gfl2::str::STRCODE* sp = (str->GetPtr());
  return GetStringWidth( sp, font, fontWidth, retLine );
}

int GetStringWidth( const wchar_t* str, const nw::font::Font *font, f32 fontWidth, int *retLine )
{
  bool  bCalcMode = false;
  f32 str_width_max = 0;
  f32 str_width = 0;
  s32 line = 0;

  f32 fontRatio = fontWidth / (f32)(font->GetWidth());

  while( *str != gfl2::str::EOM_CODE )
  {
    // �����Z�b�g
    if( *str == gfl2::str::CR_CODE )
    {
      // �ő啶�������`�F�b�N
      if( str_width_max < str_width )
      {
        str_width_max = str_width;
      }

      str_width = 0;
      line++;
      ++str;
    }
    // �ʏ핶��
    else if( *str != gfl2::str::TAG_START_CODE )
    {
      // �����v�Z�㕶����i�߂�
      const int width = font->GetCharWidth( *str );
      const f32  f_width = (f32)width * fontRatio;

      str_width += f_width;
      ++str;
    }
    // �^�O
    else
    {
      // @todo �^�O����͂��Ă�萳�m�ȕ��������v�Z����K�v������
      // �^�O���X�L�b�v���A�����v�Z

      TagGroup  tagGroup = GetTagGroup( str );
      u32       tagIdx   = GetTagIndex( str );
      //���s�^�O���`�F�b�N
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
      //�����ŕ������������ƁA�A���Ń^�O�������Ă���Ƃ܂����B
    }
  }

  // �ŏI�s���`�F�b�N
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
 * ������̃X�P�[���l���擾����(DrawUtil���Ǝ��̕����`������鎞�p!)
 *
 * @param   langId ����ID(EFIGS�̏k���Ή�)sv_config���̌���ID������
 *                 �f�t�H���g�����Ŏ����擾
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


//Msg�t�@�C���֌W
static System::MSGLANGID mMsgLangId;
//----------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�̌���ID��ݒ肷��B
 *
 * @param   langId       ����ID(ROM�̖{�̂̌���ID�Ƃ͈Ⴂ�܂��I)
 */
//----------------------------------------------------------------------------------
void SetMessageLangId( System::MSGLANGID langId )
{
  mMsgLangId = langId;
}

//----------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�̌���ID���擾����B
 *
 * @return  ����ID(ROM�̖{�̂̌���ID�Ƃ͈Ⴂ�܂��I)
 */
//----------------------------------------------------------------------------------
System::MSGLANGID GetMessageLangId(void)
{
  return mMsgLangId ;
}

//----------------------------------------------------------------------------------
/**
 * �ʏ�̃��b�Z�[�W��ARCID���擾����
 *
 * @param   langId       ����ID(ROM�̖{�̂̌���ID�Ƃ͈Ⴂ�܂��I)
 */
//----------------------------------------------------------------------------------
u32 GetMessageArcId( System::MSGLANGID langId )
{
  System::MSGLANGID tempLangId = langId;
  if( tempLangId == System::MSGLANGID_MAX )
  {
    tempLangId = mMsgLangId;
  }
  static const u32 arcIdArr[] =
  {
    ARCID_MESSAGE_JPN,
    ARCID_MESSAGE_KANJI,
    ARCID_MESSAGE_ENG,
    ARCID_MESSAGE_FRE,
    ARCID_MESSAGE_ITA,
    ARCID_MESSAGE_GER,
    ARCID_MESSAGE_SPA,
    ARCID_MESSAGE_KOR,
  };
  GFL_ASSERT_STOP( tempLangId < GFL_NELEMS(arcIdArr) );
  return arcIdArr[tempLangId];
}

//----------------------------------------------------------------------------------
/**
 * �f�o�b�O�p�̃��b�Z�[�W��ARCID���擾����
 *
 * @param   langId       ����ID(ROM�̖{�̂̌���ID�Ƃ͈Ⴂ�܂��I)
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
    ARCID_DEBUG_MESSAGE_JPN,
    ARCID_DEBUG_MESSAGE_KANJI,
    ARCID_DEBUG_MESSAGE_ENG,
    ARCID_DEBUG_MESSAGE_FRE,
    ARCID_DEBUG_MESSAGE_ITA,
    ARCID_DEBUG_MESSAGE_GER,
    ARCID_DEBUG_MESSAGE_SPA,
    ARCID_DEBUG_MESSAGE_KOR,
  };
  return arcIdArr[tempLangId];
}
#endif  // PM_DEBUG

//----------------------------------------------------------------------------------
/**
 * �X�N���v�g�p�̃��b�Z�[�W��ARCID���擾����
 *
 * @param   langId       ����ID(ROM�̖{�̂̌���ID�Ƃ͈Ⴂ�܂��I)
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
    ARCID_SCRIPT_MESSAGE_JPN,
    ARCID_SCRIPT_MESSAGE_KANJI,
    ARCID_SCRIPT_MESSAGE_ENG,
    ARCID_SCRIPT_MESSAGE_FRE,
    ARCID_SCRIPT_MESSAGE_ITA,
    ARCID_SCRIPT_MESSAGE_GER,
    ARCID_SCRIPT_MESSAGE_SPA,
    ARCID_SCRIPT_MESSAGE_KOR,
  };
  return arcIdArr[tempLangId];
}


//�����p
//----------------------------------------------------------------------------------
/**
 * ��������k�߂邩�𔻒肷��
 *
 * @param   target      �e�L�X�g�{�b�N�X�y�C��
 * @param   langId      ����ID(sv_config)
 */
//----------------------------------------------------------------------------------
bool CheckEnableShortStr( gfl2::lyt::LytTextBox * target, System::MSGLANGID langId )
{
  //���C�A�E�g�̊g���f�[�^�̖��O
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
      //�Â���`�Ή�
      const char *const FONT_NO_SHORT_STR_TEMP = "no_short";
      extData = target->FindExtUserDataByName(FONT_NO_SHORT_STR_TEMP);
      if( extData )
      {
        noShortSetiing = extData->GetIntArray()[0];
        GFL_PRINT("�Â�no_short��`�ł��B�f�[�^���R���o�[�g���Ȃ����Ă��������B\n");
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
