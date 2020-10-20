using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using gfl2.clr.math;

namespace ModelViewer
{
	public class KwCamera : ICamera
	{
		private Vector m_EyePosition;
		private Vector m_FocusPosition;
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
		private readonly float m_FarClip;
		public float FarClip
		{
			get { return m_FarClip; }
		}
		private readonly Vector m_upDirection;				//!< @brief 上方向ベクトル
		private float m_cameraInterval;			//!< @brief カメラの視点-注視点間の距離
		public float CameraInterval
		{
			get { return m_cameraInterval; }
			set { m_cameraInterval = Math.Min(Math.Max(1.0f, value), 5000.0f); }
		}
		public float CameraTranslationX { get; set; }
		public float CameraTranslationY { get; set; }
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
		public KwCamera(float screenWidth, float screenHeight, float fieldOfView)
		{
			m_EyePosition = new Vector(0.0f, 56.52f, 147.72f);
			m_FocusPosition = new Vector(0.0f, 30.0f, 0.0f);
			m_upDirection = Vector.GetYUnit();

			m_View = Matrix.GetLookAt(m_EyePosition, m_FocusPosition, m_upDirection);

			m_NearClip = 32.0f;
			m_FarClip = 6000.0f;
			m_FieldOfView = fieldOfView;
			m_Projection = gfl2.clr.math.Matrix.GetPerspective(
				gfl2.clr.math.Util.ConvDegToRad(fieldOfView),
				screenWidth / screenHeight,
				m_NearClip,
				m_FarClip);
		}

		public void ResetToDefaultPosition(PokemonModelDataManager modelDataManager)
		{
			float length = 250.0f;
			float height = 50.0f;

			if (modelDataManager != null)
			{
				length *= (modelDataManager.DefaultAdjustHeight / 100.0f);
				height *= (modelDataManager.DefaultAdjustHeight / 100.0f);
			}

			float theta = 10.0f / 180.0f * (float)Math.PI;
			float cos = length * (float)Math.Cos(theta);
			float sin = length * (float)Math.Sin(theta);
			m_EyePosition.x = 0.0f;
			m_EyePosition.y = sin;
			m_EyePosition.z = cos;
			m_FocusPosition.x = 0.0f;
			m_FocusPosition.y = 0.0f;
			m_FocusPosition.z = 0.0f;

			m_cameraRotationX = 0.0f;
			m_cameraRotationY = -(float)Math.Atan2(sin, cos);

			CameraInterval = (m_EyePosition - m_FocusPosition).Length3();

			CameraTranslationX = 0.0f;
			CameraTranslationY = -height;
		}

    public void Reset(PokemonModelDataManager modelDataManager)
    {
      ResetToDefaultPosition(modelDataManager);
    }

		public void Update()
		{
			gfl2.clr.math.Vector unitZ = gfl2.clr.math.Vector.GetZUnit();

			gfl2.clr.math.Matrix mtxRotX = gfl2.clr.math.Matrix.GetRotationY(CameraRotationX);
			gfl2.clr.math.Matrix mtxRotY = gfl2.clr.math.Matrix.GetRotationX(CameraRotationY);
			m_EyePosition = mtxRotX * mtxRotY * unitZ;

			m_EyePosition.x *= CameraInterval;
			m_EyePosition.y *= CameraInterval;
			m_EyePosition.z *= CameraInterval;

			m_View = Matrix.GetLookAt(m_EyePosition, m_FocusPosition, m_upDirection);
			Matrix mtxTranslateX = Matrix.GetTranslation(new gfl2.clr.math.Vector(CameraTranslationX, 0.0f, 0.0f, 1.0f));
			Matrix mtxTranslateY = Matrix.GetTranslation(new gfl2.clr.math.Vector(0.0f, CameraTranslationY, 0.0f, 1.0f));
			m_View = mtxTranslateX * m_View * mtxTranslateY;
		}
	}
}
