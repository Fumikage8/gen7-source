//==============================================================================
/**
 @file    gfl2_FsWin32Func.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   Win32ファイル便利関数
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// fsのインクルード
#include <fs/include/gfl2_FsStr.h>
#include "gfl2_FsWin32Func.h"


#if   defined(GF_PLATFORM_WIN32)


#include <Windows.h>  // MultiByteToWideChar, CreateDirectory
#include <stdio.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


Result Win32Func::CreateDir(const char* fullName)
{
  Result res;

  char path[Define::FILE_NAME_FULL_LEN_MAX];
  res |= Str::CopySafely(path, Define::FILE_NAME_FULL_LEN_MAX, fullName);
  if(res.IsSuccess())
  {
    u32 slashCount = 0;
    u32 pathLen = 0;
    u32 endlessLoopCount = 0;
    for ever
    {
      pathLen = Str::GetPathLenBySlash(path, slashCount);
      if(pathLen > 0)
      {
        char replacedChar = path[pathLen];
        path[pathLen] = '\0';
        {
          TCHAR pathWide[Define::FILE_NAME_FULL_LEN_MAX];
          s32 r0 = ::MultiByteToWideChar(CP_ACP, 0, path, -1, pathWide, Define::FILE_NAME_FULL_LEN_MAX);
          if(r0 == 0)
          {
            res |= Result::Bit::MULTI_BYTE_CHAR_TO_WIDE_CHAR_FAIL;
            break;
          }
          else
          {
            BOOL r1 = ::CreateDirectory(pathWide, NULL);  // Win32の関数で戻り値はBOOL型。
              // CreateDirectoryをテストして分かったこと
              //     \でも/でもOK。
              //     存在しないフォルダを含んだパスを指定すると、目的のフォルダがつくられない。確認していないが多分エラーになっている。
            if(r1 == 0)
            {
              // 既存フォルダをつくろうとしてエラーになっているかもしれないので、このエラーは無視する
            }
          }
        }
        path[pathLen] = replacedChar;
        ++slashCount;
      }
      else
      {
        break;
      }

      if(++endlessLoopCount >= Define::ENDLESS_LOOP_COUNT_MAX)
      {
        GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
        break;
      }
    }
  }

  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_WIN32

