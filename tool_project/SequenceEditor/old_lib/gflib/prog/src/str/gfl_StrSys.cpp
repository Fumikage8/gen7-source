//===========================================================================
/**
 * @file    gfl_StrSys.cpp
 * @brief   GAMEFREAK Library  String Print System ( �����񏈗��V�X�e���R�A )
 * @author  GAMEFREAK inc.
 * @date    2010.11.12
 */
//===========================================================================
#include <gfl_Common.h>

#include <str/gfl_StrSys.h>


namespace gfl {
namespace str {

//----------------------------------------------------------------------------------
/**
 * �^�O�J�n�R�[�h�ȍ~�A�X�L�b�v����v�f�����擾
 *
 * @param[in]   sp    �^�O�J�n�R�[�h���w���Ă���|�C���^
 *
 * @retval  u32       �X�L�b�v����v�f��
 */
//----------------------------------------------------------------------------------
static u32 tag_CheckSkipLength( const STRCODE* sp )
{
  GFL_ASSERT((*sp) == TAG_START_CODE);

  u32 num = *( sp+1 );

  return 2 + num;
}

//----------------------------------------------------------------------------------
/**
 * �^�O�v�f���X�L�b�v
 *
 * @param[in]   sp    �^�O�J�n�R�[�h���w���Ă��镶����|�C���^
 *
 * @retval      const STRCODE*    �^�O�v�f��S�ăX�L�b�v������̃|�C���^
 */
//----------------------------------------------------------------------------------
static const STRCODE* tag_Skip( const STRCODE* sp )
{
  GFL_ASSERT((*sp) == TAG_START_CODE);

  u32 len = tag_CheckSkipLength( sp );
  sp += len;

  return sp;
}

//----------------------------------------------------------------------------------
/**
 * �w��s�̐擪�܂Ń|�C���^�ړ�
 *
 * @param[in]   str
 * @param[in]   lineIdx
 *
 * @retval  const STRCODE*
 */
//----------------------------------------------------------------------------------
static const STRCODE* line_Skip( const STRCODE* str, u32 lineIdx )
{
  if( lineIdx )
  {
    while( *str != EOM_CODE )
    {
      if( *str == CR_CODE )
      {
        ++str;
        if( --lineIdx == 0 ){
          break;
        }
        continue;
      }

      if( *str != TAG_START_CODE )
      {
        ++str;
      }
      else
      {
        str = tag_Skip( str );
      }
    }
  }

  if( lineIdx == 0 ){
    return str;
  }

  return NULL;
}

//--------------------------------------------------------------
/**
 * ������̒����i�I�[�������܂܂Ȃ��j��Ԃ�
 *
 * @param[in]    str    ������|�C���^
 *
 * @retval  u32   ������̒���
 */
//--------------------------------------------------------------
u32
StrLen( const STRCODE* str )
{
  u32 len = 0;
  while( *str != EOM_CODE )
  {
    if( *str == TAG_START_CODE )
    {
      const STRCODE* prev = str;

      str = tag_Skip( str );
      len += static_cast<uptr>(str - prev);
    }
    else
    {
      ++str;
      ++len;
    }

    if( len >= WARNING_LENGTH )
    {
      GFL_ASSERT(0);    // �����񂪑z��ȏ�̒����ł�
      break;
    }
  }
  return len;
}

//--------------------------------------------------------------
/**
 * ������R�s�[
 *
 * @param[out]   dst    �R�s�[��̕����z��
 * @param[in]    src    �R�s�[���̕����z��
 *
 * @retval  u32   �R�s�[�����������i�I�[�������܂܂Ȃ��j
 */
//--------------------------------------------------------------
u32
StrCopy( STRCODE* dst, const STRCODE* src )
{
  u32 len = 0;
  while( 1 )
  {
    *dst = *src;

    // �^�O�̓X�L�b�v��܂ł̕���������̂܂܃R�s�[����
    if( *dst == TAG_START_CODE )
    {
      const STRCODE* prev = src;

      src = tag_Skip( src );
      len += static_cast<uptr>(src - prev);

      while( prev != src ){
        *(++dst) = *(++prev);
      }
    }

    if( *dst == EOM_CODE ){
      break;
    }

    ++dst;
    ++src;
    ++len;
    if( len >= WARNING_LENGTH )
    {
      GFL_ASSERT(0);    // �����񂪑z��ȏ�̒����ł�
      --len;
      *dst = EOM_CODE;
      break;
    }
  }
  return len;
}
//--------------------------------------------------------------
/**
 * ������R�s�[�i�ő啶�����̎w�肠��j
 *
 * @param[out]   dst          �R�s�[��̕����z��
 * @param[in]    src          �R�s�[���̕����z��
 * @param[in]    numCharMax   �R�s�[����ő啶�����i�I�[�������܂ށj
 *
 * @retval  u32   �R�s�[�����������i�I�[�������܂܂Ȃ��j
 */
//--------------------------------------------------------------
u32
StrNCopy( STRCODE* dst, const STRCODE* src, u32 numCharMax )
{
  u32 len = 0;


  while( numCharMax )
  {
    *dst = *src;

    if( *dst == TAG_START_CODE )
    {
      u32 skip_len = tag_CheckSkipLength( src );
      if( numCharMax > skip_len )
      {
        numCharMax -= skip_len;
        len += skip_len;
        while( skip_len-- )
        {
          *(++dst) = *(++src);
        }
        continue;
      }
      else{
        *dst = EOM_CODE;
      }
    }

    if( *dst == EOM_CODE ){
      break;
    }

    ++dst;
    ++src;
    ++len;
    --numCharMax;
  }

  if( (numCharMax == 0) && (len>0) )
  {
    --len;
    *(dst-1) = EOM_CODE;
  }

  return len;
}
//--------------------------------------------------------------
/**
 * �������r
 *
 * @param[in]    s1       ��r�p������P
 * @param[in]    s2       ��r�p������Q
 *
 * @retval  bool    s1, s2 �̓��e����v���Ă�����TRUE��Ԃ�
 */
//--------------------------------------------------------------
bool
StrComp( const STRCODE* s1, const STRCODE* s2 )
{
  while( *s1 == *s2 )
  {
    if( *s1 == TAG_START_CODE )
    {
      const STRCODE* s1_next = tag_Skip( s1 );
      while( s1 != s1_next )
      {
        ++s1;
        ++s2;
        if( *s1 != *s2 ){
          return false;
        }
      }
    }

    if( *s1 == EOM_CODE ){
      return true;
    }
    ++s1;
    ++s2;
  }
  return false;
}
//--------------------------------------------------------------
/**
 * �s���J�E���g
 *
 * @param[in]    str      ������|�C���^
 *
 * @retval  u32  �s���i1 origin�j
 */
//--------------------------------------------------------------
u32 CountLines( const STRCODE* str )
{
  u32 count = 1;

  while( *str != EOM_CODE )
  {
    if( *str == CR_CODE )
    {
      ++count;
    }

    if( *str != TAG_START_CODE )
    {
      ++str;
    }
    else
    {
      str = tag_Skip( str );
    }
  }

  return count;
}

//--------------------------------------------------------------
/**
 * �w��s�̕����񒷁i�I�[�����E���s�������܂܂Ȃ��j���擾����
 *
 * @param[in]   str       ������|�C���^
 * @param[in]   lineIdx   �s�ԍ��i0�`�j
 *
 * @retval  u32   lineIdx�Ŏw�肳�ꂽ�s�̕����񒷁i���s�^�I�[�R�[�h���܂܂Ȃ��j
 */
//--------------------------------------------------------------
u32
LineLen( const STRCODE* str, u32 lineIdx )
{
  str = line_Skip( str, lineIdx );

  if( str != NULL )
  {
    const STRCODE* start = str;


    while( (*str!=EOM_CODE) && (*str!=CR_CODE) )
    {
      if( *str != TAG_START_CODE )
      {
        ++str;
      }
      else
      {
        str = tag_Skip( str );
      }
    }

    u32 len = static_cast<uptr>(str - start);
    return len;
  }

  return 0;
}

//--------------------------------------------------------------
/**
 * �w��s�̐擪�|�C���^���擾
 *
 * @param[in]   str       ������|�C���^
 * @param[in]   lineIdx   �s�ԍ��i0�`�j
 *
 * @retval  u32   lineIdx�Ŏw�肳�ꂽ�s�̐擪�����|�C���^
 */
//--------------------------------------------------------------
const STRCODE*
GetLinePtr( const STRCODE* str, u32 lineIdx )
{
  return line_Skip( str, lineIdx );
}

//--------------------------------------------------------------
/**
 * �w��s�̕�������R�s�[
 *
 * @param[out]    dst         �R�s�[��̕����z��
 * @param[in]     dstElems    dst �̗v�f��
 * @param[in]     src         �R�s�[���̕����z��
 * @param[in]     lineIdx     �s�ԍ��i0�`�j
 *
 * @retval  u32   �R�s�[�����������i�I�[�������܂܂Ȃ��j
 */
//--------------------------------------------------------------
u32
LineCopy( STRCODE* dst, u32 dstElems, const STRCODE* src, u32 lineIdx )
{
  const STRCODE* line_top = line_Skip( src, lineIdx );

  if( line_top != NULL )
  {
    u32 len = LineLen( src, lineIdx ) + 1;
    if( len >= dstElems ){
      len = dstElems - 1;
    }
    return StrNCopy( dst, line_top, len );
  }
  return 0;
}

//--------------------------------------------------------------
/**
 * �^�O�J�n�R�[�h�`�����������X�L�b�v����������|�C���^��Ԃ�
 *
 * @param[in]   str   ������|�C���^�i�^�O�J�n�R�[�h���w���Ă��邱�Ɓj
 *
 * @retval  STRCODE*  �^�O�J�n�R�[�h�`�^�O���������X�L�b�v����������|�C���^
 */
//--------------------------------------------------------------
const STRCODE*
SkipTagCode( const STRCODE* str )
{
  if( *str == TAG_START_CODE )
  {
    str = tag_Skip( str );
  }
  return str;
}

//--------------------------------------------------------------
/**
 * ���[�U�[�^�O�R�[�h���擾
 *
 * @param[in]   str       ������|�C���^�i�^�O�J�n�R�[�h���w���Ă��邱�Ɓj
 *
 * @retval  u16   �^�O�R�[�h
 */
//--------------------------------------------------------------
u16
GetTagCode( const STRCODE* str )
{
  if( *str == TAG_START_CODE )
  {
    str += 2;
    return static_cast<u16>(*str);
  }
  else
  {
    GFL_ASSERT(0);
  }

  return 0;
}
//--------------------------------------------------------------
/**
 * �^�O�����̌����擾
 *
 * @param[in]   str   ������|�C���^�i�^�O�J�n�R�[�h���w���Ă��邱�Ɓj
 *
 * @retval  u16   �^�O�����̌�
 */
//--------------------------------------------------------------
u16
GetTagParamCount( const STRCODE* str )
{
  if( *str == TAG_START_CODE )
  {
    u16 numParams = static_cast<u16>( str[1] );
    if( (numParams > 0) && (numParams<NUM_TAG_PARAM_MAX) ){
      return numParams - 1;
    }else{
      GFL_ASSERT_MSG(0, "numParams=%d", numParams);
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
  return 0;
}
//--------------------------------------------------------------
/**
 * �^�O�������擾
 *
 * @param[in]   str       ������|�C���^�i�^�O�J�n�R�[�h���w���Ă��邱�Ɓj
 * @param[in]   paramIdx  �p�����[�^Index�i0�`�j
 *
 * @retval  u16   �^�O����
 */
//--------------------------------------------------------------
u16
GetTagParam( const STRCODE* str, u32 paramIdx )
{
  if( *str == TAG_START_CODE )
  {
    u32 numParams = static_cast<u32>( str[1] );

    if( numParams > NUM_TAG_PARAM_MAX ){
      GFL_ASSERT_MSG( 0, "numParams=%d", numParams );
      return 0;
    }

    if( numParams && (paramIdx <(numParams-1)) )
    {
      return static_cast<u16>( str[ 3 + paramIdx ] );
    }
    else
    {
      GFL_ASSERT_MSG(0, "paramIdx=%d, numParams=%d", paramIdx, numParams);
    }
  }
  else
  {
    GFL_ASSERT(0);
  }

  return 0;
}

#if _GFL_STRCODE == _GFL_STRCODE_UTF16
//--------------------------------------------------------------------
/**
 *  utf16->sjis �ϊ�
 *
 *  @param[in]    code   �ϊ�����UTF-16�R�[�h
 *  @param[out]   dst    �ϊ����sjis�R�[�h���i�[����o�b�t�@
 *
 *  @retval  bool  �ϊ��ł����� true
 */
//--------------------------------------------------------------------
bool utf16_to_sjis( wchar_t code, char* dst )
{
  int numPutCode = 0;

  do {
    // ���8bit == 0 -> ascii
    if( ((code & 0xff00) == 0)
    &&  ((code & 0x00ff) < 0x007f)
    ){
      dst[numPutCode++] = code & 0x00ff;
      break;
    }

    // ������x�A�����������R�[�h�Q����
    {
      static const struct {
        wchar_t   uc_min;
        wchar_t   uc_max;
        u16       sjis_min;
      }convert_cont[] = {
        { 0x3041,  0x3093,  0x829f },   // 3041 ~ 3093�F�Ђ炪�ȁi���`��j
        { 0x30a1,  0x30df,  0x8340 },   // 30a1 ~ 30df�F�J�^�J�i�i�@�`�~�j
        { 0x30e0,  0x30f6,  0x8380 },   // 30e0 ~ 30f6�F�J�^�J�i�i���`���j
        { 0xff10,  0xff19,  0x824f },   // ff10 ~ ff19�F�S�p�����i�O�`�X�j
        { 0xff21,  0xff3a,  0x8260 },   // ff21 ~ ff3a�F�S�p�����i�`�`�y�j
        { 0xff41,  0xff5a,  0x8281 },   // ff41 ~ ff5a�F�S�p�����i���`���j
      };

      for(int i=0; i<GFL_NELEMS(convert_cont); ++i)
      {
        if( (code >= convert_cont[i].uc_min) && (code <= convert_cont[i].uc_max) )
        {
          u16 sjis = convert_cont[i].sjis_min + ( code - convert_cont[i].uc_min );
          dst[numPutCode++] = (sjis >> 8) & 0xff;
          dst[numPutCode++] = sjis & 0xff;
          break;
        }
      }
      if( numPutCode ){
        break;
      }
    }

    // �g�������ȋL���Ȃǌʂ�
    {
      static const struct {
        u16  unicode;
        u16  sjis;
      }convert_indv[] = {
        { 0x3000,  0x8140 },  //
        { 0x3002,  0x8142 },  // �B
        { 0xff1f,  0x8148 },  // �H
        { 0xff01,  0x8149 },  // �I
        { 0x30fc,  0x815b },  // �[
        { 0xff0d,  0x817c },  // �|
        { 0x25cb,  0x819b },  // ��
        { 0x25cf,  0x819c },  // ��
        { 0x25ce,  0x819d },  // ��
        { 0x25c7,  0x819e },  // ��
        { 0x25c6,  0x819f },  // ��
        { 0x25a1,  0x81a0 },  // ��
        { 0x25a0,  0x81a1 },  // ��
        { 0x25b3,  0x81a2 },  // ��
        { 0x25b2,  0x81a3 },  // ��
        { 0x25bd,  0x81a4 },  // ��
        { 0x25bc,  0x81a5 },  // ��
        { 0x2192,  0x81a8 },  // ��
        { 0x2190,  0x81a9 },  // ��
        { 0x2191,  0x81aa },  // ��
        { 0x2193,  0x81ab },  // ��
      };

      for(int i=0; i<GFL_NELEMS(convert_indv); ++i)
      {
        if( convert_indv[i].unicode == code )
        {
          dst[numPutCode++] = (convert_indv[i].sjis >> 8) & 0xff;
          dst[numPutCode++] = convert_indv[i].sjis & 0xff;
          break;
        }
      }
      if( numPutCode ){
        break;
      }
    }

  }while( 0 );


  if( numPutCode ){
    dst[numPutCode] = '\0';
    return true;
  }

  return false;
}
#endif  // #if _GFL_STRCODE == _GFL_STRCODE_UTF16


//--------------------------------------------------------------
/**
 * �����R�[�h Print �i�f�o�b�O�p�j
 *
 * @param[in]     str         ������|�C���^
 * @param[in]     caption     �v�����g����L���v�V����
 */
//--------------------------------------------------------------
#if GFL_DEBUG
void
PrintCode( const STRCODE* str, const char* caption )
{
  if( caption ){
    GFL_PRINT( caption );
  }

  #if _GFL_STRCODE == _GFL_STRCODE_UTF16
  {
    char sjis_buf[ 4 ];

    while( *str != EOM_CODE )
    {
      if( *str != TAG_START_CODE )
      {
        if( utf16_to_sjis(*str, sjis_buf) ){
          GFL_PRINT( sjis_buf );
        }else{
          GFL_PRINT("[%04x]", *str);
        }
        ++str;
      }
      else
      {
        const STRCODE* next = tag_Skip( str );
        GFL_PRINT("[TAG>]");
        while( ++str != next )
        {
          GFL_PRINT("%04x,", *str);
        }
        GFL_PRINT("[<TAG]");
      }
    }
  }
  #else
  GFL_PRINT( str );
  #endif

  GFL_PRINT("\n");
}
#else
// �����[�X�łł͉������Ȃ�
void
PrintCode( const STRCODE* , const char*  )
{
}
#endif

/**
 * �������r�i�^�O���l�����Ȃ��j
 *
 * @param[in]    s1       ��r�p������P
 * @param[in]    s2       ��r�p������Q
 *
 * @retval  bool    s1, s2 �̓��e����v���Ă�����TRUE��Ԃ�
 */
bool    StrComp_J( const char* s1, const char* s2 )         // [SJIS]
{
  while( *s1 == *s2 )
  {
    if( *s1 == '\0' ){
      return true;
    }
    ++s1;
    ++s2;
  }
  return false;
}
bool    StrComp_U( const wchar_t* s1, const wchar_t* s2 )   // [UTF16]
{
  while( *s1 == *s2 )
  {
    if( *s1 == L'\0' ){
      return true;
    }
    ++s1;
    ++s2;
  }
  return false;
}

} /* end of namespace str */
} /* end of namespace gfl */
