using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using gfl2.clr.math;

namespace SimpleViewer
{
  class BottomCamera : ICamera
  {
    private Matrix m_Projection;				//!< @brief 射影行列
    public Matrix Projection
    {
      get { return m_Projection; }
    }
    private Matrix m_View;							//!< @brief ビュー行列
    public Matrix View
    {
      get { return m_View; }
    }
    private float m_FieldOfView;
    public float FieldOfView
    {
      get { return m_FieldOfView; }
    }
    private float m_NearClip;
    public float NearClip
    {
      get { return m_NearClip; }
    }
    private float m_FarClip;
    public float FarClip
    {
      get { return m_FarClip; }
    }
    private Vector m_eyePosition;				//!< @brief 視点
    private Vector m_focusPosition;			//!< @brief 注視点
    private Vector m_upDirection;				//!< @brief 上方向ベクトル
    private float m_cameraInterval;			//!< @brief カメラの視点-注視点間の距離
    public float CameraInterval
    {
      get { return m_cameraInterval; }
      set { m_cameraInterval = Math.Min(Math.Max(1.0f, value), 5000.0f); }
    }

    private float m_cameraRotationX;		//!< @brief マウス操作時のカメラの横方向回転
    public float CameraRotationX
    {
      get { return m_cameraRotationX; }
      set
      {
        m_cameraRotationX = value;
        if (m_cameraRotationX > gfl2.clr.math.Util.Pai)
        {
          m_cameraRotationX -= 2.0f * gfl2.clr.math.Util.Pai;
        }
        else if (m_cameraRotationX < -gfl2.clr.math.Util.Pai)
        {
          m_cameraRotationX += 2.0f * gfl2.clr.math.Util.Pai;
        }
      }
    }

    private float m_cameraRotationY;		//!< @brief マウス操作時のカメラの縦方向回転
    public float CameraRotationY
    {
      get { return m_cameraRotationY; }
      set
      {
        float verticalLimit = (gfl2.clr.math.Util.Pai - 0.01f) / 2.0f;
        m_cameraRotationY = Math.Min(Math.Max(-verticalLimit, value), verticalLimit);
      }
    }

    private float m_cameraTranslationX;	//!< @brief マウス操作時のカメラの横方向移動
    public float CameraTranslationX
    {
      get { return m_cameraTranslationX; }
      set { m_cameraTranslationX = value; }
    }

    private float m_cameraTranslationY;	//!< @brief マウス操作時のカメラの縦方向移動
    public float CameraTranslationY
    {
      get { return m_cameraTranslationY; }
      set { m_cameraTranslationY = value; }
    }

    public BottomCamera(float screenWidth, float screenHeight, float fieldOfView)
    {
      m_eyePosition = new gfl2.clr.math.Vector(1.0f, 0.0f, 0.0f, 1.0f);
      m_focusPosition = new gfl2.clr.math.Vector(0.0f, 0.0f, 0.0f, 1.0f);
      m_upDirection = new gfl2.clr.math.Vector(0.0f, 1.0f, 0.0f, 1.0f);

      ResetToDefaultPosition(null);

      m_NearClip = 32.0f;
      m_FarClip = 6000.0f;

      m_FieldOfView = fieldOfView;
      m_Projection = gfl2.clr.math.Matrix.GetPerspective(
        gfl2.clr.math.Util.ConvDegToRad(fieldOfView),
        screenWidth / screenHeight,
        m_NearClip,
        m_FarClip);

      m_View = Matrix.GetLookAt(m_eyePosition, m_focusPosition, m_upDirection);
      Matrix mtxTranslateX = Matrix.GetTranslation(new gfl2.clr.math.Vector(m_cameraTranslationX, 0.0f, 0.0f, 1.0f));
      Matrix mtxTranslateY = Matrix.GetTranslation(new gfl2.clr.math.Vector(0.0f, m_cameraTranslationY, 0.0f, 1.0f));
      m_View = mtxTranslateX * m_View * mtxTranslateY;

    }

    public void Reset(PokemonModelDataManager modelDataManager)
    {
      // 何もしない
    }

    public void ResetToDefaultPosition(PokemonModelDataManager modelDataManager)
    {
      m_cameraRotationX = 0.0f;
      m_cameraRotationY = 0.5f * (float)Math.PI;
      m_cameraTranslationX = 0.0f;
      m_cameraTranslationY = -60.0f;
      m_cameraInterval = 150.0f;
    }

    public void Update()
    {
      gfl2.clr.math.Vector unitZ = gfl2.clr.math.Vector.GetZUnit();

      gfl2.clr.math.Matrix mtxRotX = gfl2.clr.math.Matrix.GetRotationY(m_cameraRotationX);
      gfl2.clr.math.Matrix mtxRotY = gfl2.clr.math.Matrix.GetRotationX(m_cameraRotationY);
      m_eyePosition = mtxRotX * mtxRotY * unitZ;

      m_eyePosition.x *= m_cameraInterval;
      m_eyePosition.y *= m_cameraInterval;
      m_eyePosition.z *= m_cameraInterval;

      m_View = Matrix.GetLookAt(m_eyePosition, m_focusPosition, m_upDirection);
      Matrix mtxTranslateX = Matrix.GetTranslation(new gfl2.clr.math.Vector(m_cameraTranslationX, 0.0f, 0.0f, 1.0f));
      Matrix mtxTranslateY = Matrix.GetTranslation(new gfl2.clr.math.Vector(0.0f, m_cameraTranslationY, 0.0f, 1.0f));
      m_View = mtxTranslateX * m_View * mtxTranslateY;
    }

  }
}
