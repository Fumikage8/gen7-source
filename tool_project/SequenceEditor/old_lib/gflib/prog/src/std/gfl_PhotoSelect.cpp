//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		PhotoSelect.cpp
 *	@brief  �ʐ^�I���A�v���b�g
 *	@author	Koji Kawada
 *	@date		2012.06.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR-SDK
#include <nn.h>
#include <nn/phtsel.h>

// gflib
#include <gfl_Heap.h>
#include <std/gfl_PhotoSelect.h>
#include <base/gfl_Applet.h>


namespace gfl {
namespace std {


//-----------------------------------------------------------------------------
/**
 *   @brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
PhotoSelect::PhotoSelect(
  const Description& description
)
{
  // �g�p���郁�����T�C�Y���ύX����Ă��Ȃ����m�F����
  GFL_ASSERT( WORK_BUFFER_SIZE == nn::phtsel::CTR::GetWorkBufferSize() );  //@check

  // �t�@�C���p�X������ő�l���ύX����Ă��Ȃ����m�F����
  STATIC_ASSERT( FILE_PATH_LENGTH_MAX == nn::phtsel::CTR::PhtselOutput::FILE_PATH_VALUE_MAX_NUM +1 );

  // �p�����[�^
  nn::phtsel::CTR::InitializeParameter(m_parameter);

  m_parameter.config.enableHomeButton = description.enableHomeButton;
  m_parameter.config.enableSoftReset  = description.enableSoftReset;

  nn::phtsel::CTR::Config&       config = m_parameter.config;
  nn::phtsel::CTR::PhtselInput&  input  = m_parameter.input;
  nn::phtsel::CTR::PhtselOutput& output = m_parameter.output;

  // PhtselInput
  input.SetSecStart      ( nn::phtsel::CTR::DateTimeConvert::GetMinSeconds() );
  input.SetSecEnd        ( nn::phtsel::CTR::DateTimeConvert::GetMaxSeconds() );
  input.SetImgKindBit    ( nn::phtsel::CTR::PhtselInput::IMAGE_KIND_BIT_PICTURE );
  input.SetScreenShotType( nn::phtsel::CTR::PhtselInput::SCREEN_SHOT_TYPE_ALL );
  input.SetFaceType      ( nn::phtsel::CTR::PhtselInput::FACE_TYPE_ALL );
  input.SetMinFaceNum    ( 0 );
  input.SetMaxFaceNum    ( nn::phtsel::CTR::PhtselInput::MAX_FACE_INFO_NUM );
  input.ClearTitleUniqueId();
  input.SetDistributeType( nn::phtsel::CTR::PhtselInput::DISTRIBUTE_TYPE_ALL );
  input.SetSoundEnable   ( true );
  input.SetMessage       ( L"" );

  // PhtselOutput
  // �������Ȃ�
}

//-----------------------------------------------------------------------------
/**
 *   @brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
PhotoSelect::~PhotoSelect()
{
  // �������Ȃ�
}

//-----------------------------------------------------------------------------
/**
 *   @brief  �N��
 */
//-----------------------------------------------------------------------------
void PhotoSelect::Start(
  gfl::heap::HeapBase* heap_memory
)
{
  void* buffer = GflHeapAllocMemoryAlign(heap_memory, nn::phtsel::CTR::GetWorkBufferSize(), 4);


  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�v���b�g�R�[���O����
   */
  //-----------------------------------------------------------------------------
  base::Applet::AppletCallBefore();

  nn::applet::AppletWakeupState state = nn::phtsel::CTR::StartPhtsel(
      &m_parameter,
      buffer
  );
  GflHeapFreeMemory(buffer);

  //switch(state)  // @note �����ŃG���[���N���邱�Ƃ����邩�H���̏ꍇ�̑Ή��́H��nn::applet::CTR::WaitForStarting�̐�����ǂތ���ł�IsExpectedToCloseApplication�őΏ�����΂����悤���B
  //{
  //case WAKEUP_SKIP:
  //  break;
  //case :
  //  break;
  //default:
  //  break;
  //}

  b32 is_close = nn::applet::CTR::IsExpectedToCloseApplication();  
      // @note ���̊֐��͒���I��(�ʏ�A���t���[��)�Ăяo���Ă��������B 
      // �܂��AWaitForStarting() �̑҂������֐�����߂�������ɕK�����ׂĂ��������Bnn::applet::CTR::Enable() �̌�����ׂĂ��������B
  if(is_close)
  {
    // �I���ւ̑J�ڂ́A�\�t�g�E�F�A�ɔC����i��΂ɗp�ӂ���Ă���j
    /*
    nn::applet::CTR::PrepareToCloseApplication();
    nn::applet::CTR::CloseApplication();
    // �����ɂ͓��B���܂���
    */
    return ;
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�v���b�g�R�[���㏈��
   */
  //-----------------------------------------------------------------------------
  base::Applet::AppletCallAfter();
}

//-----------------------------------------------------------------------------
/**
 *   @brief  �I����ɁA�I�����ʂ��擾����
 */
//-----------------------------------------------------------------------------
PhotoSelect::Result PhotoSelect::GetResult(void) const
{
  Result res = RESULT_RETURN_CODE;
  const nn::phtsel::CTR::PhtselOutput& output = m_parameter.output;
  nn::phtsel::CTR::ReturnCode code = output.GetReturnCode();
  switch(code)
  {
  case nn::phtsel::CTR::RETURN_CODE_SUCCESS:
    {
      if( output.GetValueLen() > 0 )
      {
        res = RESULT_DECIDE;
      }
      else
      {
        res = RESULT_CANCEL;
      }
      break;
    }
  //case nn::phtsel::CTR::RETURN_CODE_CANCEL: res = RESULT_CANCEL; break;  // �u���ǂ�v��I�񂾂Ƃ���CANCEL�ł͂Ȃ�SUCCESS���Ԃ��Ă���B�����當�����Ŕ��ʂ��邱�Ƃɂ����B
  }
  return res;
}
//-----------------------------------------------------------------------------
/**
 *   @brief  �I����ɁA�I�����ꂽ�ʐ^�̃t�@�C���p�X���擾����
 */
//-----------------------------------------------------------------------------
const wchar_t* PhotoSelect::GetFilePath(void) const
{
  if( GetResult() == RESULT_DECIDE )
  {
    const nn::phtsel::CTR::PhtselOutput& output = m_parameter.output;
    return output.GetValue();
  }
  else
  {
    return NULL;
  }
}
//-----------------------------------------------------------------------------
/**
 *   @brief  �I����ɁA�I�����ꂽ�ʐ^�̃t�@�C���p�X���擾����
 */
//-----------------------------------------------------------------------------
PhotoSelect::Result PhotoSelect::GetFilePath(wchar_t* file_path) const
{
  Result res = GetResult();
  if( res == RESULT_DECIDE )
  {
    const nn::phtsel::CTR::PhtselOutput& output = m_parameter.output;
    wcscpy( file_path, output.GetValue() ); 

    //PhotoSelect::Result PhotoSelect::GetFilePath(wchar_t file_path[FILE_PATH_LENGTH_MAX]) const
    //wcsncpy( file_path, output.GetValue(), FILE_PATH_LENGTH_MAX -1 ); 
    //file_path[FILE_PATH_LENGTH_MAX -1] = 0;
  }
  return res;
}
//-----------------------------------------------------------------------------
/**
 *   @brief  �I����ɁA�I�����ꂽ�ʐ^�̃t�@�C���p�X�̒������擾����
 */
//-----------------------------------------------------------------------------
u16 PhotoSelect::GetFilePathLength(void) const
{
  if( GetResult() == RESULT_DECIDE )
  {
    const nn::phtsel::CTR::PhtselOutput& output = m_parameter.output;
    return output.GetValueLen();
  }
  else
  {
    return 0;
  }
}
//-----------------------------------------------------------------------------
/**
 *   @brief  �I����ɁA���^�[���R�[�h���擾����
 */
//-----------------------------------------------------------------------------
nn::phtsel::CTR::ReturnCode PhotoSelect::GetReturnCode() const
{
  const nn::phtsel::CTR::PhtselOutput& output = m_parameter.output;
  return output.GetReturnCode();
}


} // namespace std
} // namespace gfl

