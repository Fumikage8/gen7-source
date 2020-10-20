#if defined(GF_PLATFORM_WIN32)//Windowsでしか提供するつもりなし

#include <string.h>
#include <iostream>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>
#include <util/include/gfl2_FnvHash.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")  

using namespace rapidxml;

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

  //-----------------------------------------------------------------------------
  /// @brief	ファイル削除関数
  //-----------------------------------------------------------------------------
  void ParserUtil::RemoveFile( const char* filename )
  {
    remove( filename );
  }

  //-----------------------------------------------------------------------------
  /// @brief	シフトJISをUTF-16に変換
  //-----------------------------------------------------------------------------
  std::wstring ParserUtil::ShitJIStoUTF16( const std::string& str )
  {
    int len = ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, NULL, 0 );

    if ( len > 0 ) { 
      std::vector<wchar_t> utf16( len );

      if ( ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, &utf16[0], len ) ){
        return std::wstring( &utf16[0] );
      }
    }

    return NULL;
  }

  //-----------------------------------------------------------------------------
  /// @brief	UTF-16をUTF-8に変換
  //-----------------------------------------------------------------------------
  std::string ParserUtil::UTF16toUTF8( const std::wstring& str )
  {
    int len = ::WideCharToMultiByte( CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL );

    if ( len > 0 ) { 
      std::vector<char> utf8( len );

      if ( ::WideCharToMultiByte( CP_UTF8, 0, str.c_str(), -1, &utf8[0], len, NULL, NULL ) ){
        return std::string( &utf8[0] );
      }
    }

    return NULL;
  }

  //-----------------------------------------------------------------------------
  /// @brief	UTF-16をSJISに変換
  //-----------------------------------------------------------------------------
  std::string ParserUtil::UTF16toShitJIS( const std::wstring& wstr )
  {
    int len = ::WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL );

    if ( len>0 ) {
      std::vector<char> sjis( len );

      if ( ::WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, &sjis[0], len, NULL, NULL ) ){
        return std::string( &sjis[0] );
      }
    }

    return NULL;
  }

  //-----------------------------------------------------------------------------
  /// @brief	UTF-8をUTF-16に変換
  //-----------------------------------------------------------------------------
  std::wstring ParserUtil::UTF8toUTF16( const std::string& str )
  {

    int len = ::MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, NULL, 0 );

    if ( len > 0 ) { 
      std::vector<wchar_t> utf16( len );

      if ( ::MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, &utf16[0], len ) ){
        return std::wstring( &utf16[0] );
      }
    }

    return NULL;
  } 

  //-----------------------------------------------------------------------------
  /// @brief	ファイル生成関数
  //-----------------------------------------------------------------------------
  void ParserUtil::FileCreate( const char* filename, const char* p_data, int size )
  {
    FILE		*hFile;

    char					filePath[GFL_MAX_FILEPATH];

    ParserUtil::GetFilePath( filePath, filename );

    if ( PathIsDirectory(filePath) == false )
    {
      CreateDirectory( filePath, NULL );
    }

    if(fopen_s( &hFile, filename, "wb" ) != 0)
    {
      std::cerr << "出力先ファイルを開けません" << std::endl;
      exit(1);
    }

    fwrite( p_data, size, 1 , hFile );
    fclose( hFile );
  }

  //-----------------------------------------------------------------------------
  /// @brief	ファイルパスの取得
  //-----------------------------------------------------------------------------
  void ParserUtil::GetFilePath( char* p_out, const char* filename )
  {
    int			size;
    bool		fHasDrive = false;

    size = (int)strlen( filename );

    if ( filename[1] == ':' )
      fHasDrive = true;

    while( size ){
      if ( filename[size] == '/' )		break;
      if ( filename[size] == '\\' )		break;
      --size;
    }

    if ( size )
    {
      memcpy( p_out, filename, size + 1 );
      p_out[ size + 1 ] = 0;//term code
    }
    else
    {
      const char		*tail = "\\";
      GetCurrentDirectory(128,p_out);
      strcat_s( p_out, 128, tail );
    }
  }

  //-----------------------------------------------------------------------------
  /// @brief	ファイルパスの取得
  //-----------------------------------------------------------------------------
  std::string ParserUtil::GetFilePathStr( const char* filename )
  {  
    char					filePath[GFL_MAX_FILEPATH];
    memset( filePath, 0, sizeof(filePath) );

    GetFilePath( filePath, filename );

    return filePath;
  }

  c8* ParserUtil::ReadFile( const char* filename, s32* p_size, s32 align )
  {
    s32			fileSize;
    return ParserUtil::ReadFile( filename, &fileSize, p_size, align );
  }

  c8* ParserUtil::ReadFile( const char* filename, s32* p_file, s32* p_size, s32 align )
  {
    FILE		*hFile;
    int			file_size;
    char		*read_buff;

    fopen_s( &hFile, filename, "rb" );

    if ( hFile == NULL ){
      return NULL;
    }

    fseek( hFile, 0, SEEK_END );
    file_size = ftell( hFile );
    fseek( hFile, 0, SEEK_SET );

    *p_file = file_size;

    file_size += align;//必ずNULLが入るようにしたい。
    u32 buf_size = file_size + ((-file_size) & (align-1));

    *p_size = buf_size;

    read_buff = new c8[ buf_size ];
    memset( read_buff, 0, buf_size );
    fread( read_buff, file_size, 1, hFile );
    fclose( hFile );

    return( read_buff );
  }

  void ParserUtil::ReplaceStr(std::string& str, const std::string& from, const std::string& to)
  {
    std::string::size_type pos = 0;

    while(pos = str.find(from, pos), pos != std::string::npos) {
      str.replace(pos, from.length( ), to);
      pos += to.length();
    }
  }

  std::vector<std::string> ParserUtil::SplitStr( std::string& SrcCode, const std::string& separate )
  {
    std::vector<std::string>			StrArray;
    int				read_pos = 0;
    int				tag_head = 0;
    int				tag_tail = 0;

    while( read_pos < static_cast<int>( SrcCode.length() ) )
    {
      //前回の検索結果位置からseparateを検索
      tag_head = static_cast<int>( SrcCode.find( separate, read_pos ) );

      if ( tag_head != -1 )
      {
        if ( (tag_head - read_pos) > 0 )
        {//ここまでの文字列を出力
          std::string message = SrcCode.substr(read_pos, tag_head - read_pos);
          StrArray.push_back( message );
        }
        else
        {
          std::string message = "";
          StrArray.push_back( message );
        }

        tag_tail = tag_head + 1;
        read_pos = tag_tail;//ここまで読み込んだ
      }
      else
      {//もうseparateはない。
        break;
      }
    }

    if ( (SrcCode.length() - read_pos) > 0 )
    {//残りの文字を出力
      std::string message = SrcCode.substr(read_pos, SrcCode.length() - read_pos);
      StrArray.push_back( message );
    }

    return StrArray;
  }

  inline float FastAtof(char *str)
  {
    int pos = 0;
    int numTotal = 0;
    double total = 0;
    bool sign = false;

    bool foundDecimal = false;
    int mult = 10;

    if(str[0] == '-')
    {
      pos++;
      sign=true;
    }

    int div = 10;

    while(str[pos]!=0)
    {
      int v = str[pos++] - '0';

      if(v == '.' - '0') 
      {
        foundDecimal = true;			
        break;
      }

      numTotal *= (double)mult;		

      if(v) numTotal += (double)v;

    }

    // Decimal Part:
    while(str[pos]!=0)
    {
      int v = str[pos++] - '0';

      if(v)
      {		
        double inc =  (double)(v) / div;
        total+= inc;		
      }
      div*=10;
    }

    double finalTotal = (double)numTotal+total;
    if(sign) finalTotal = -finalTotal;

    return((float)finalTotal);
  }

  void ParserUtil::ParseFloatArray( std::vector<math::Vector4> *p_out, const xml_node<>* float_array_node, int stride, float padding )
  {
    char*			pFloatDataLine = float_array_node->value();
    u32				readMax = static_cast<u32>( strlen( pFloatDataLine ) );

    char *str_value = new char[readMax+1];

    memcpy(str_value,pFloatDataLine,readMax);
    str_value[readMax] = 0; // 0 Terminate string

    u32 strValueStart = 0;

    u32						readPos = 0;
    u32						writePos = 0;
    s32						elementCnt = 0;
    math::Vector4	value;

    while( readPos < readMax )
    {
      writePos++; readPos++;

      if ( pFloatDataLine[readPos] == ' ' )
      {
        str_value[ writePos ] = 0;

        value[elementCnt] = FastAtof(str_value+strValueStart);

        strValueStart = writePos+1;

        ++elementCnt;

        if ( elementCnt == stride )
        {
          p_out->push_back( value );
          elementCnt = 0;
        }
      }

    }

    str_value[ writePos ] = 0;
    value[elementCnt] = FastAtof(str_value+strValueStart);

    p_out->push_back( value );

    delete[] str_value;
  }

  void ParserUtil::ParseNameArray( std::vector<std::string> *p_out, const xml_node<>* name_array_node )
  {
    std::string					nameDataLine( name_array_node->value() );
    std::vector<std::string>	nameArray;

    *p_out = SplitStr( nameDataLine, " " );
  }

  int ParserUtil::GetFileName( c8* p_out, const c8* filename )
  {
    int			tail;
    int			len;

    tail = len = (int)strlen( filename );

    while( tail ){
      if ( filename[tail] == '/' )
      {
        ++tail;
        break;
      }

      if ( filename[tail] == '\\' )
      {
        ++tail;
        break;
      }

      --tail;
    }

    int			copy_size = len - tail;

    if ( p_out )
    {
      memcpy( p_out, &filename[tail], copy_size );
      p_out[ copy_size ] = 0;//term code
    }

    return copy_size;
  }

  std::string ParserUtil::GetFileNameStr( const char* filename )
  {
    char					fileNameType[GFL_MAX_FILEPATH];
    std::string		fileNameTypeStr;

    GetFileName( fileNameType, filename );
    fileNameTypeStr = fileNameType;

    return fileNameTypeStr;
  }

  void ParserUtil::GetNodeName( c8* p_out, s32 bufferSize, const c8* filename )
  {
    const char *p = strrchr(filename, '|');
    if(p == NULL)
    {
      p = filename;
    }
    else
    {
      p++;
    }
    strcpy_s(p_out, bufferSize, p);
  }



}}}}


#endif