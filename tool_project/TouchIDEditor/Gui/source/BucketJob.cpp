#include "../include/BucketJob.h"
#include "../include/GLUtil.h"
#include "../include/MainWindow.h"
#include <string>
#include <vector>

#include "../include/MultiTextureShader.h"
#include "../include/BrushShader.h"

using namespace gfl::maya;
using namespace gfl::maya::plugins::Shader;

namespace Gui { 

  BucketJob::Info BucketJob::s_Info;
  BucketJob::RGBA32 BucketJob::s_StartColor;
  BucketJob::RGBA32 BucketJob::s_ReplaceColor;
  bool *BucketJob::s_fChecked = NULL;

  //-----------------------------------------------------------------------------
  /// @brief	
  //-----------------------------------------------------------------------------
  bool BucketJob::RGBA32::CheckRGB( RGBA32 color, int tolerance )
  {
    int r_len = abs( this->r - color.r );
    int g_len = abs( this->g - color.g );
    int b_len = abs( this->b - color.b );

    r_len -= tolerance;
    g_len -= tolerance;
    b_len -= tolerance;

    //０以上ということは許容オーバー
    if ( (r_len <= 0) && (g_len <= 0) && (b_len <= 0) )
    {
      return true;
    }

    return false;
  }

  //-----------------------------------------------------------------------------
  /// @brief	
  //-----------------------------------------------------------------------------
  void BucketJob::RGBA32::SetRGB( RGBA32 color )
  {
    this->r = color.r;
    this->g = color.g;
    this->b = color.b;
  }

  //-----------------------------------------------------------------------------
  /// @brief	
  //-----------------------------------------------------------------------------
  BucketJob::BucketJob()
  {

  }

  //-----------------------------------------------------------------------------
  /// @brief	
  //-----------------------------------------------------------------------------
  BucketJob::~BucketJob()
  {

  }

  //-----------------------------------------------------------------------------
  /// @brief	
  //-----------------------------------------------------------------------------
  int BucketJob::GetImagePos( int x, int y )
  {
    if ( x < 0 || y < 0 || x >= s_Info.m_ImageSize.GFL_VECTOR_X || y >= s_Info.m_ImageSize.GFL_VECTOR_Y )
    {
      return -1;
    }

    int pos = y * s_Info.m_ImageSize.GFL_VECTOR_X;
    pos += x;

    return pos;
  }

  //-----------------------------------------------------------------------------
  /// @brief	
  //-----------------------------------------------------------------------------
  void BucketJob::DoIt( Info &rInfo )
  {
    s_Info.m_MousePos = rInfo.m_MousePos;
    s_Info.m_Color = rInfo.m_Color;
    s_Info.m_pImageData = rInfo.m_pImageData;
    s_Info.m_pDstImageData = rInfo.m_pDstImageData;
    s_Info.m_ImageSize.Set( rInfo.m_ImageSize );
    s_Info.m_Tolerance = rInfo.m_Tolerance;

    {
      int startPos = BucketJob::GetImagePos( s_Info.m_MousePos.x(), s_Info.m_MousePos.y() );
      s_StartColor = s_Info.m_pImageData[startPos];

      s_ReplaceColor.r = s_Info.m_Color.red();
      s_ReplaceColor.g = s_Info.m_Color.green();
      s_ReplaceColor.b = s_Info.m_Color.blue();
      s_ReplaceColor.a = 0xFF;

      BucketJob::s_fChecked = DbgNew bool[ s_Info.m_ImageSize.GFL_VECTOR_X * s_Info.m_ImageSize.GFL_VECTOR_Y ];
      int size = sizeof(bool) * s_Info.m_ImageSize.GFL_VECTOR_X * s_Info.m_ImageSize.GFL_VECTOR_Y ;

      memset( BucketJob::s_fChecked, 0, size );
    }

    BucketJob job;

    job.Draw( s_Info.m_MousePos.x(), s_Info.m_MousePos.y() );

    delete[] ( BucketJob::s_fChecked );
  }

  //-----------------------------------------------------------------------------
  /// @brief	
  //-----------------------------------------------------------------------------
  void BucketJob::Draw( int x, int y )
  {
    int pos = BucketJob::GetImagePos( x, y );

    if ( pos == -1 )
    {
      return;
    }

    if ( BucketJob::s_fChecked[ pos ] == true )
    {
      return;
    }

    BucketJob::s_fChecked[ pos ] = true;

    if ( s_Info.m_pImageData[ pos ].CheckRGB( BucketJob::s_StartColor, s_Info.m_Tolerance ) )
    {
      s_Info.m_pDstImageData[ pos ].idata = BucketJob::s_ReplaceColor.idata;

      this->Draw( x + 1, y );
      this->Draw( x - 1, y );
      this->Draw( x, y + 1 );
      this->Draw( x, y - 1 );
    }
  }

}