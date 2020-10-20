//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		PhotoSelect.cpp
 *	@brief  写真選択アプレット
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
 *   @brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
PhotoSelect::PhotoSelect(
  const Description& description
)
{
  // 使用するメモリサイズが変更されていないか確認する
  GFL_ASSERT( WORK_BUFFER_SIZE == nn::phtsel::CTR::GetWorkBufferSize() );  //@check

  // ファイルパス文字列最大値が変更されていないか確認する
  STATIC_ASSERT( FILE_PATH_LENGTH_MAX == nn::phtsel::CTR::PhtselOutput::FILE_PATH_VALUE_MAX_NUM +1 );

  // パラメータ
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
  // 何もしない
}

//-----------------------------------------------------------------------------
/**
 *   @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
PhotoSelect::~PhotoSelect()
{
  // 何もしない
}

//-----------------------------------------------------------------------------
/**
 *   @brief  起動
 */
//-----------------------------------------------------------------------------
void PhotoSelect::Start(
  gfl::heap::HeapBase* heap_memory
)
{
  void* buffer = GflHeapAllocMemoryAlign(heap_memory, nn::phtsel::CTR::GetWorkBufferSize(), 4);


  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレットコール前処理
   */
  //-----------------------------------------------------------------------------
  base::Applet::AppletCallBefore();

  nn::applet::AppletWakeupState state = nn::phtsel::CTR::StartPhtsel(
      &m_parameter,
      buffer
  );
  GflHeapFreeMemory(buffer);

  //switch(state)  // @note ここでエラーが起こることもあるか？その場合の対応は？→nn::applet::CTR::WaitForStartingの説明を読む限りではIsExpectedToCloseApplicationで対処すればいいようだ。
  //{
  //case WAKEUP_SKIP:
  //  break;
  //case :
  //  break;
  //default:
  //  break;
  //}

  b32 is_close = nn::applet::CTR::IsExpectedToCloseApplication();  
      // @note この関数は定期的に(通常、毎フレーム)呼び出してください。 
      // また、WaitForStarting() の待ちうけ関数から戻った直後に必ず調べてください。nn::applet::CTR::Enable() の後も調べてください。
  if(is_close)
  {
    // 終了への遷移は、ソフトウェアに任せる（絶対に用意されている）
    /*
    nn::applet::CTR::PrepareToCloseApplication();
    nn::applet::CTR::CloseApplication();
    // ここには到達しません
    */
    return ;
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレットコール後処理
   */
  //-----------------------------------------------------------------------------
  base::Applet::AppletCallAfter();
}

//-----------------------------------------------------------------------------
/**
 *   @brief  終了後に、終了結果を取得する
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
  //case nn::phtsel::CTR::RETURN_CODE_CANCEL: res = RESULT_CANCEL; break;  // 「もどる」を選んだときはCANCELではなくSUCCESSが返ってくる。だから文字数で判別することにした。
  }
  return res;
}
//-----------------------------------------------------------------------------
/**
 *   @brief  終了後に、選択された写真のファイルパスを取得する
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
 *   @brief  終了後に、選択された写真のファイルパスを取得する
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
 *   @brief  終了後に、選択された写真のファイルパスの長さを取得する
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
 *   @brief  終了後に、リターンコードを取得する
 */
//-----------------------------------------------------------------------------
nn::phtsel::CTR::ReturnCode PhotoSelect::GetReturnCode() const
{
  const nn::phtsel::CTR::PhtselOutput& output = m_parameter.output;
  return output.GetReturnCode();
}


} // namespace std
} // namespace gfl

