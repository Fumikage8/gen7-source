//==============================================================================
/**
 @file    gfl2_FsStr.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステムで扱う文字列
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// fsのインクルード
#include <fs/include/gfl2_FsStr.h>


#include <string.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


Result Str::CreateName(gfl2::heap::HeapBase* heap, u32* outNameLen, char** outName, const char* inName)
{
  Result res;

  u32 l_inNameLen = static_cast<u32>(strlen(inName));
  u32 l_outNameLen = l_inNameLen;

  char* l_outName = static_cast<char*>(GflHeapAllocMemory(heap, l_outNameLen +1));  // 終端の'\0'の分を+1する

  if(l_outName)
  {
    copy(l_outName, l_outNameLen +1, inName);

    *outNameLen = l_outNameLen;
    *outName    = l_outName;
  }
  else
  {
    res |= Result::Bit::HEAP_ALLOC_FAIL;
  }
  
  return res;
}

Result Str::CreateFileName(gfl2::heap::HeapBase* heap, u32* outFileNameLen, char** outFileName, const char* inPrefixName, const char* inFileName)
{
  Result res;

  u32 l_inPrefixNameLen = 0;
  if(inPrefixName)
  {
    l_inPrefixNameLen = static_cast<u32>(strlen(inPrefixName));
  }
  u32 l_inFileNameLen = static_cast<u32>(strlen(inFileName));
  u32 l_outFileNameLen = l_inPrefixNameLen + l_inFileNameLen;

  char* l_outFileName = static_cast<char*>(GflHeapAllocMemory(heap, l_outFileNameLen +1));  // 終端の'\0'の分を+1する

  if(l_outFileName)
  {
    if(inPrefixName)
    {
      copy(l_outFileName, l_outFileNameLen +1, inPrefixName);
    }
    copy(&l_outFileName[l_inPrefixNameLen], l_outFileNameLen +1 -l_inPrefixNameLen, inFileName);
    replaceBackSlashBySlash(l_outFileName, l_outFileNameLen);

    *outFileNameLen = l_outFileNameLen;
    *outFileName    = l_outFileName;
  }
  else
  {
    res |= Result::Bit::HEAP_ALLOC_FAIL;
  }
  
  return res;
}

bool Str::CheckFileNameIsSame(const char* fileName_a, const char* fileName_b)
{
  if(fileName_a == NULL || fileName_b == NULL ) return false;  // どちらかNULLならfalse(両方NULLもfalse)

  bool ret = true;

  // 一文字ずつチェック
  u32 i = 0;
  char c_a = fileName_a[i];
  char c_b = fileName_b[i];
  u32 endlessLoopCount = 0;
  while(c_a!='\0' && c_b!='\0')
  {
    // '\\'を'/'に変更する
    if(c_b=='\\') c_b = '/';  // c_aの区切りは'/'になっている。

    // 比較
    if(c_a != c_b)
    {
      ret = false;
      break;
    }
  
    // 次の文字へ
    ++i;
    c_a = fileName_a[i];
    c_b = fileName_b[i];
    
    if(++endlessLoopCount >= Define::ENDLESS_LOOP_COUNT_MAX)
    {
      GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
      break;
    }
  }

  // 最後の文字が両方とも終端かチェック
  if(c_a!='\0' || c_b!='\0')
  {
    ret = false;
  }

  return ret;
}

bool Str::CheckFileNameIsSameStrictly(const char* fileName_a, const char* fileName_b)
{
  if(fileName_a == NULL || fileName_b == NULL ) return false;  // どちらかNULLならfalse(両方NULLもfalse)
  
  bool ret = true;

  // 一文字ずつチェック
  u32 i = 0;
  char c_a = fileName_a[i];
  char c_b = fileName_b[i];
  u32 endlessLoopCount = 0;
  while(c_a!='\0' && c_b!='\0')
  {
    // '\\'を'/'に変更する
    if(c_a=='\\') c_a = '/';
    if(c_b=='\\') c_b = '/';

    // 比較
    if(c_a != c_b)
    {
      ret = false;
      break;
    }
  
    // 次の文字へ
    ++i;
    c_a = fileName_a[i];
    c_b = fileName_b[i];
   
    if(++endlessLoopCount >= Define::ENDLESS_LOOP_COUNT_MAX)
    {
      GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
      break;
    }
  }

  // 最後の文字が両方とも終端かチェック
  if(c_a!='\0' || c_b!='\0')
  {
    ret = false;
  }

  return ret;
}

Result Str::CreatePath(gfl2::heap::HeapBase* heap, u32* outPathLen, char** outPath, const char* inPath)
{
  Result res;
 
  u32 l_inPathLen;
  u32 l_outPathLen = getLenWithTailSlash(&l_inPathLen, inPath);
 
  char* l_outPath = static_cast<char*>(GflHeapAllocMemory(heap, l_outPathLen +1));  // 終端の'\0'の分を+1する

  if(l_outPath)
  {
    copy(l_outPath, l_outPathLen +1, inPath);
    replaceBackSlashBySlash(l_outPath, l_inPathLen);  // まだ末尾に/を付けていない(もともと付いていたかもしれないが)のでl_inPathLenを使う
    
    res |= Str::addTailSlash(&l_outPathLen, l_outPath, l_outPathLen +1, l_inPathLen);  // 末尾に/を付ける

    if(res.IsSuccess())
    {
      *outPathLen = l_outPathLen;
      *outPath    = l_outPath;
    }
    else
    {
      GFL_SAFE_DELETE_ARRAY(l_outPath);
    }
  }
  else
  {
    res |= Result::Bit::HEAP_ALLOC_FAIL;
  }

  return res;
}

Result Str::GetFullPathFileName(
    u32* fullNameLen, char* fullName, u32 fullNameBufSize,
    u32 archiveNameLen, const char* archiveName,
    u32 pathLen, const char* path,
    const char* fileName
)
{
  Result res;

  u32 fileNameLen = static_cast<u32>(strlen(fileName));

  u32 len = archiveNameLen + pathLen + fileNameLen;
  if(len < fullNameBufSize)
  {
    // パスとファイル名を連結する
    *fullNameLen = len;
    u32 index = 0;
    
    if(archiveName)
    {
      copy(&(fullName[index]), fullNameBufSize-index, archiveName); 
      index += archiveNameLen;
    }
   
    if(path)
    {
      copy(&(fullName[index]), fullNameBufSize-index, path); 
      index += pathLen;
    }

    copy(&(fullName[index]), fullNameBufSize-index, fileName);

    fullName[*fullNameLen] = '\0';
   
    // \を/に置き換える
    replaceBackSlashBySlash(fullName, *fullNameLen);
  }
  else
  {
    res |= Result::Bit::FILE_NAME_LEN_TOO_LONG;
  }

  return res;
}

u32 Str::GetStrLen(const char* str)
{
  u32 len = 0;
  if(str)
  {
    len = static_cast<u32>(strlen(str));
  }
  return len;
}

Result Str::CopySafely(char* dst, u32 dstBufSize, const char* src)
{
  Result res;
  u32 srcLen = GetStrLen(src);
  if(srcLen < dstBufSize)
  {
    copy(dst, dstBufSize, src);
  }
  else
  {
    res |= Result::Bit::FILE_NAME_LEN_TOO_LONG;
  }
  return res;
}

void Str::SetFileNameInBuf(char* dst, u32 dstBufSize, const char* src)
{
  if(dst && dstBufSize>0)
  {
    if(src && dstBufSize>1)
    {
      u32 srcLen = GetStrLen(src);
 
      if(srcLen < dstBufSize)
      {
        // (1) 全部dstBufSizeにおさまるなら、全部コピーする。
        copy(dst, dstBufSize, src);
        return;
      }
      else
      {
        // Ptrはポインタ、Addrはアドレス整数値、Idxは文字列インデックス、Lenは長さ、とする。
        uptr srcAddr = reinterpret_cast<uptr>(src);

        // 拡張子の確認
        const char* dotPtr  = searchCharReversely(src, '.');  // 拡張子があるか否かのフラグとしても使う
        uptr        dotAddr = 0;
        u32         extLen  = 0;  // 拡張子の長さ。終端の'\0'は含まない長さ。.を含んだ長さ。
        if(dotPtr)
        {
          dotAddr = reinterpret_cast<uptr>(dotPtr);
          u32 srcWithoutExtLen = static_cast<u32>(dotAddr - srcAddr);  // .以降なし(.もなし)の長さ
          extLen = srcLen - srcWithoutExtLen;
          if(srcWithoutExtLen < dstBufSize)
          {
            // (2) 拡張子をなくせばdstBufSizeにおさまるなら、拡張子が切れるようなコピーの仕方をする。
            copyByLen(dst, dstBufSize, src, dstBufSize-1);
            return;
          }
        }

        // パスの確認
        const char* slashPtr  = NULL;  // \もしくは/があるか否かのフラグとしても使う
        uptr        slashAddr = 0;
        u32         slashNextIdx = 0;  // \もしくは/の次の文字のインデックス
        {
          const char* normalSlashPtr = searchCharReversely(src, '/');
          const char* backSlashPtr   = searchCharReversely(src, '\\');
          uptr normalSlashAddr = 0;
          uptr backSlashAddr   = 0;
          if(normalSlashPtr) normalSlashAddr = reinterpret_cast<uptr>(normalSlashPtr);
          if(backSlashPtr)   backSlashAddr   = reinterpret_cast<uptr>(backSlashPtr);
          // ファイル名に近いほう(値の大きいほう)を採用する
          if(normalSlashAddr > backSlashAddr)
          {
            slashPtr  = normalSlashPtr;
            slashAddr = normalSlashAddr;
          }
          else
          {
            slashPtr  = backSlashPtr;
            slashAddr = backSlashAddr;
          }
        }
        if(slashPtr)
        {
          slashNextIdx = static_cast<u32>(slashAddr - srcAddr) +1;
          u32 srcWithoutPathLen    = srcLen - slashNextIdx;       // パスなしファイル名の長さ(拡張子はある)
          u32 srcWithoutPathExtLen = srcWithoutPathLen - extLen;  // パスなしファイル名の長さ(拡張子なし(.もなし))
          
          if(srcWithoutPathExtLen < dstBufSize)
          {
            // (3) 拡張子をなくした(.もなくす)「パスなしファイル名」がdstBufSizeにおさまるなら、おさまる分だけパスの後半を含んでコピーする(拡張子はなし(.もなし))
            u32 frontLen = dstBufSize-1 - srcWithoutPathExtLen;  // どれだけの長さ前へ行けるか
            u32 startIdx = slashNextIdx;
            if(slashNextIdx >= frontLen)  // 必ず真のはずだが、念のためチェックしておく。
            {
              startIdx = slashNextIdx - frontLen;
            }
            copyByLen(dst, dstBufSize, &src[startIdx], dstBufSize-1);
            return;
          }
        }
 
        // (4) パスをなくしてコピーする。aaa/bbb/ccc.dddだったら、ccc.dddになるがファイル名の後半が切れる。
        copyByLen(dst, dstBufSize, &src[slashNextIdx], dstBufSize-1);
        return;
      }
    }
    else
    {
      dst[0] = '\0';
      return;
    }
  }
}

u32 Str::GetPathLenBySlash(const char* fileName, u32 slashCount)
{
  u32 len = 0;
  u32 i = 0;
  u32 l_slashCount = 0;
  char prevChar = '\0';
  char currChar = fileName[i];
  while(currChar != '\0')
  {
    if(currChar == '/')
    {
      if(    prevChar != '\0'
          && prevChar != ':'
          && prevChar != '/'
          && prevChar != '.'  )
      {
        if(slashCount == l_slashCount)
        {
          len = i;
          break;
        }
        ++l_slashCount;
      }
    }
    ++i;
    prevChar = currChar;
    currChar = fileName[i];
  }
  return len;
}

void Str::copy(char* dst, u32 dstBufSize, const char* src)
{
#if   defined(GF_PLATFORM_CTR)
  strcpy(dst, src);
#elif defined(GF_PLATFORM_WIN32)
  strcpy_s(dst, dstBufSize, src);  // 終端の'\0'を含めた長さを渡す
#endif  // GF_PLATFORM_
}

void Str::copyByLen(char* dst, u32 dstBufSize, const char* src, u32 srcCopyLen)
{
  u32 copyLen = dstBufSize -1;
  if(srcCopyLen < copyLen) copyLen = srcCopyLen;

#if   defined(GF_PLATFORM_CTR)
  strncpy(dst, src, copyLen);
#elif defined(GF_PLATFORM_WIN32)
  strncpy_s(dst, dstBufSize, src, copyLen);
#endif  // GF_PLATFORM_
  dst[copyLen] = '\0';
}

const char* Str::searchCharReversely(const char* str, char c)
{
  const char* p = strrchr(str, c);
  return p;
}

void Str::replaceBackSlashBySlash(char* path, u32 pathLen)
{
  for(u32 i=0; i<pathLen; ++i)
  {
    if(path[i]=='\\') path[i] = '/';
  }
}

u32 Str::getLenWithTailSlash(u32* pathLen, const char* path)
{
  u32 pathLenOut = 0;
  u32 pathLenIn = static_cast<u32>(strlen(path));
  
  if(pathLenIn > 0)
  {
    if(path[pathLenIn-1]=='\\' || path[pathLenIn-1]=='/')
    {
      pathLenOut = pathLenIn;
    }
    else
    {
      pathLenOut = pathLenIn +1;  // 末尾の「\もしくは/」の分を+1する
    }
  }
  else
  {
    pathLenOut = pathLenIn +1;  // 末尾の「\もしくは/」の分を+1する
  }

  *pathLen = pathLenIn;
  return pathLenOut;
}


Result Str::addTailSlash(u32* pathLenOut, char* path, u32 pathBufSize, u32 pathLenIn)
{
  Result res;

  if(pathLenIn > 0)
  {
    if(path[pathLenIn-1]!='/')
    {
      if(pathBufSize > pathLenIn+1)
      {
        path[pathLenIn] = '/';
        path[pathLenIn+1] = '\0';
        *pathLenOut = pathLenIn+1;
      }
      else
      {
        res |= Result::Bit::FILE_NAME_LEN_TOO_LONG;
      }
    }
    else
    {
      *pathLenOut = pathLenIn;
    }
  }
  else
  {
    if(pathBufSize > pathLenIn+1)
    {
      path[pathLenIn] = '/';
      path[pathLenIn+1] = '\0';
      *pathLenOut = pathLenIn+1;
    }
    else
    {
      res |= Result::Bit::FILE_NAME_LEN_TOO_LONG;
    }
  }

  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

