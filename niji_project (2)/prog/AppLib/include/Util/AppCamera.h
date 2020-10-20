#if !defined(APP_UTIL_APPCAMERA_H_INCLUDED)
#define APP_UTIL_APPCAMERA_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    AppCamera.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.01.22
 @brief   カメラ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


//==============================================================================
/**
 @class     AppCamera
 @brief     カメラ
 */
//==============================================================================
class AppCamera
{
  GFL_FORBID_COPY_AND_ASSIGN(AppCamera);

public:
  class RenderPlaceType
  {
  public:
    enum Tag
    {
      UPPER_DISPLAY,  //!< 上画面(上画面左、上画面右)
      LOWER_DISPLAY,  //!< 下画面
    };
  };

  class ProjectionType
  {
  public:
    enum Tag
    {
      PERSPECTIVE,  //!< 透視射影
      ORTHO,        //!< 正射影
    };
  };

public:
  AppCamera(void);
  virtual ~AppCamera() {}

  ProjectionType::Tag GetProjectionType(void) const { return m_projType; }
  const gfl2::math::Matrix44& GetProjectionMatrix(void) const { return m_projMtx; }
  const gfl2::math::Matrix34& GetViewMatrix(void) const { return m_viewMtx; }

  void SetProjectionMatrix(const gfl2::math::Matrix44& projMtx, ProjectionType::Tag projType) { m_projMtx = projMtx;  m_projType = projType; }
  void SetViewMatrix(const gfl2::math::Matrix34& viewMtx) { m_viewMtx = viewMtx; }

  //! @brief  正射影カメラとしてセットアップする
  //! @param[in] scale    scale=1.0fのとき、(x,y)=(1.0f,1.0f)は画面上の(1.0f,1.0f)となる。
  //!                     scale=0.5fのとき、(x,y)=(1.0f,1.0f)は画面上の(0.5f,0.5f)となる。
  //!                     scale=2.0fのとき、(x,y)=(1.0f,1.0f)は画面上の(2.0f,2.0f)となる。
  //! @param[in] front    rear<=z<=frontが描画される。(カメラz位置=front、ニアクリップ面までの距離=0)
  //! @param[in] rear                                 (ファークリップ面までの距離=front-rear)
  //! @note  Z軸上にカメラを置き、XY平面を写す。
  void SetupOrthoCamera(
      RenderPlaceType::Tag renderPlace,
      f32 scale, f32 front, f32 rear
  );

  //! @brief  ビューマトリクスを設定する
  //! @param[in] position        カメラの位置
  //! @param[in] targetPosition  注視点
  //! @param[in] twist           カメラの回転[degree]
  void SetViewMatrix(
      const gfl2::math::Vector3&  position,
      const gfl2::math::Vector3&  targetPosition,
      f32                         twist
  );

  //! @brief  パースペクティブプロジェクションマトリクスを設定する(透視射影)
  //void SetPerspectiveProjectionMatrix() { m_projType = ProjectionType::PERSPECTIVE; }

  //! @brief  オルソプロジェクションマトリクスを設定する(正射影)
  //! @param[in] aspectRatio  アスペクト比
  //! @param[in] height       画像の縦サイズ
  //! @param[in] near         ニア
  //! @param[in] far          ファー
  //! @param[in] isPivot      画面を回転させるときtrue
  //!                         GF_PLATFORM_CTRならtrue。GF_PLATFORM_WIN32ならfalse。
  //!                         CTRでも正射影でない場合はfalseになるが、この関数は正射影用なので、CTRならtrue。
  void SetOrthoProjectionMatrix(
      f32                         aspectRatio,
      f32                         height,
      f32                         near,
      f32                         far,
      bool                        isPivot = false
  );

private:
  gfl2::math::Matrix34  m_viewMtx;
  gfl2::math::Matrix44  m_projMtx;
  ProjectionType::Tag   m_projType;
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // APP_UTIL_APPCAMERA_H_INCLUDED

