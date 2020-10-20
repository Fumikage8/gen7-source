//==============================================================================
/**
 @file    gfl2_FsResult.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステムにおける結果
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>

// fsのインクルード
#include <fs/include/gfl2_FsResult.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


Result::Result(void)
{
  m_bitFlag[0] = Bit::SUCCESS;
  m_bitFlag[1] = Bit::SUCCESS;
}

bool Result::IsSuccess(void) const
{
  return (m_bitFlag[0] == Bit::SUCCESS) && (m_bitFlag[1] == Bit::SUCCESS);
}

void Result::Clear(void)
{
  m_bitFlag[0] = Bit::SUCCESS;
  m_bitFlag[1] = Bit::SUCCESS;
}

void Result::Print(void) const
{
  GFL_PRINT("gfl2::fs::Result  IsSuccess()=%d  Tag0=0x%X  Tag1=0x%X\n", this->IsSuccess(), this->m_bitFlag[0], this->m_bitFlag[1]);
  GFL_PRINT("Tag0: ");
  for(s32 i=Bit::TAG0_MAX; i>=0; --i)
  {
    GFL_PRINT("%d ", (this->m_bitFlag[0]&(1<<i)) !=0);
  }
  GFL_PRINT("\n");
  GFL_PRINT("Tag1: ");
  for(s32 i=Bit::TAG1_MAX; i>=0; --i)
  {
    GFL_PRINT("%d ", (this->m_bitFlag[1]&(1<<i)) != 0);
  }
  GFL_PRINT("\n");
}


#if   defined(GF_PLATFORM_CTR)

#define GFL_FS_NN_RESULT_PRINT_BLOCK(funcName,nnRes) \
  if( nn::fs::funcName::Includes(nnRes) ) \
  { \
    GFL_PRINT(#funcName); \
  } \
  else

void Result::PrintNnResult(nn::Result nnRes)
{
#if GFL_DEBUG
  GFL_PRINT("ERROR: nn::Result = 0x%X\n", nnRes.GetPrintableBits());
  GFL_PRINT("    Level       = %d\n", nnRes.GetLevel()       );
  GFL_PRINT("    Summary     = %d\n", nnRes.GetSummary()     );
  GFL_PRINT("    Module      = %d\n", nnRes.GetModule()      );
  GFL_PRINT("    Description = %d\n", nnRes.GetDescription() );
  
  GFL_PRINT("    nn::fs::Result = ");
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultNotFound             , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultMediaNotFound        , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultAlreadyExists        , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultNotEnoughSpace       , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultArchiveInvalidated   , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultOperationDenied      , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultWriteProtected       , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultMediaAccessError     , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultNotFormatted         , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultBadFormat            , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultVerificationFailed   , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultOutOfResource        , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultAccessDenied         , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultInvalidArgument      , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultNotInitialized       , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultAlreadyInitialized   , nnRes )
  GFL_FS_NN_RESULT_PRINT_BLOCK( ResultUnsupportedOperation , nnRes )
  {
    GFL_PRINT("FATAL");  // その他のエラー
  }
  GFL_PRINT("\n");
#endif  // GFL_DEBUG
}

#undef GFL_FS_NN_RESULT_PRINT_BLOCK

#endif  // GF_PLATFORM_CTR


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

