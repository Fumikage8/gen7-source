//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl_Env2.cpp
 *  @brief  2D描画環境情報
 *  @author tomoya takahashi
 *  @date   2010.10.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <layout/include/gfl2_G2dEnv.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

namespace gfl2 {
namespace lyt {

//-----------------------------------------------------------------------------
/**
 *          定数宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          公開
 */
//-----------------------------------------------------------------------------
// 2D環境デフォルト　パラメータ
const ProjectionParam Env::DefaultProjection[lyt::DISPLAY_NORMAL_MAX] = {
  {
    PROJECTION_ORTHO,
    { -200, 200, -120, 120 },
    0.10f,
    800.0f,
    gfl2::math::PIVOT_UPSIDE_TO_TOP
  },
  {
    PROJECTION_ORTHO,
    { -160, 160, -120, 120 },
    0.10f,
    800.0f,
    gfl2::math::PIVOT_UPSIDE_TO_TOP
  },
};
const ViewParam Env::DefaultView[lyt::DISPLAY_NORMAL_MAX] = {
  {
    gfl2::math::Vector3(0,0,1),
    gfl2::math::Vector3(0,1,0),
    gfl2::math::Vector3(0,0,0)
  },
  {
    gfl2::math::Vector3(0,-290,1),
    gfl2::math::Vector3(0,1,0),
    gfl2::math::Vector3(0,-290,0)
  },
};

bool                        Env::m_sStereoCameraReady = false;
#if defined(GF_PLATFORM_CTR)
nn::ulcd::CTR::StereoCamera Env::m_sStereoCamera; //!< 視差を考慮したカメラ(CTRのみ)
#endif

//----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ    デフォルト設定
 */
//-----------------------------------------------------------------------------
Env::Env(void)
{
  SetCameraParam( CAMERA_NORMAL, DefaultProjection[lyt::DISPLAY_UPPER], DefaultView[lyt::DISPLAY_UPPER] );
  SetCameraParam( CAMERA_RIGHT, DefaultProjection[lyt::DISPLAY_UPPER], DefaultView[lyt::DISPLAY_UPPER] );

  m_parallax.parallax_half = 0.0f;
  m_parallax.dLevel        = 0.0f;
  m_parallax.addDist       = 0.0f;

  // StereoCameraの準備
  if( !m_sStereoCameraReady ){
#if defined(GF_PLATFORM_CTR)  //!< 視差を考慮したカメラ(CTRのみ)
    m_sStereoCamera.Initialize();
    m_sStereoCamera.SetBaseFrustum(
        -0.04f,
         0.04f,
        -0.04f * static_cast<f32>(DISPLAY_UPPER_HEIGHT) / static_cast<f32>(DISPLAY_UPPER_WIDTH),
         0.04f * static_cast<f32>(DISPLAY_UPPER_HEIGHT) / static_cast<f32>(DISPLAY_UPPER_WIDTH),
         0.2f,
         3000.0f);
#endif
    m_sStereoCameraReady = true;
  }

  // FragmentOperationはコンストラクタ内で設定されるデフォルト値を使用
}



//----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
Env::~Env()
{
}


//----------------------------------------------------------------------------
/**
 *  @brief  カメラパラメータを設定
 *
 *  @param  type    カメラタイプ
 *  @param  proj    射影設定
 *  @param  view    View設定
 */
//-----------------------------------------------------------------------------
void Env::SetCameraParam( Env::CameraType type, const ProjectionParam& proj, const ViewParam& view )
{
  if( type < CAMERA_NUM )
  {
    m_camera[type].SetUpProjectionParam( proj );
    m_camera[type].SetUpViewParam( view );
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ViewPortの設定
 *
 *  @param  use       true　ビューポート設定を使用する。 false  フレームバッファのサイズ＝ViewPortと扱う。
 *  @param  ViewPort  設定情報
 */
//-----------------------------------------------------------------------------
void Env::SetViewPort( bool use, const EnvViewPort* viewport )
{
  m_viewPortUse = use;
  if(viewport){
    m_viewPort    = *viewport;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  視差の設定
 *
 *  @param  parallax    視差の調整の値（StereoCameraと同じ値）
 *  @param  dLevel      基準面への距離（StereoCameraと同じ値）
 *  @param  addDist     カメラとの距離に追加する値 
 */
//-----------------------------------------------------------------------------
void Env::SetParallax( f32 parallax, f32 dLevel, f32 addDist )
{
#if defined( GF_PLATFORM_CTR )  //!< 視差を考慮したカメラ
#if 0   // @note GraphicSystemを使用しているのでコメントアウト。使えるようにしてもnijiでは意味がない
  m_sStereoCamera.SetBaseCamera(&m_camera[CAMERA_NORMAL].GetViewMtx());

  f32 l_parallax = 0.0f;
  if( GraphicsSystem::IsGameStereoFlag() ){
    l_parallax = parallax;
  }

  nn::math::MTX44 projL, projR;
  nn::math::MTX34 viewL, viewR;
  m_sStereoCamera.CalculateMatrices(
      &projL,
      &viewL,
      &projR,
      &viewR,
      dLevel,
      l_parallax,
      nn::math::PIVOT_UPSIDE_TO_TOP);
  
  f32 wkParallax = m_sStereoCamera.GetCoefficientForParallax();

  m_parallax.parallax_half  = wkParallax * static_cast<f32>(DISPLAY_UPPER_WIDTH);
  m_parallax.dLevel         = dLevel;
  m_parallax.addDist        = addDist;
#endif
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  視差情報の取得
 */
//-----------------------------------------------------------------------------
const Parallax& Env::GetParallax( void ) const
{
  return m_parallax;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ViewPortの取得
 *
 *  @retval  ViewPort情報
 */
//-----------------------------------------------------------------------------
const EnvViewPort& Env::GetViewPort( void ) const
{
  return m_viewPort;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  カメラ情報を取得
 */
//-----------------------------------------------------------------------------
const Camera& Env::GetCamera( Env::CameraType type ) const 
{
  if( type < CAMERA_NUM )
  {
    return m_camera[type];
  }
  GFL_ASSERT(0);
  return m_camera[0];
}
Camera* Env::GetCameraPtr( Env::CameraType type )
{
  if( type < CAMERA_NUM )
  {
    return &m_camera[type];
  }
  GFL_ASSERT(0);
  return &m_camera[0];
}


} /* end of namespace lyt */
} /* end of namespace gfl2 */
