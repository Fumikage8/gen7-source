using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using gfl2.clr.math;

namespace ModelViewer
{
	public class BattleCamera : ICamera
	{
		private readonly Vector[,] m_EyePositions;
		private readonly Vector[,] m_FocusPositions;

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
		private readonly Vector m_upDirection;				//!< @brief 上方向ベクトル
		public float CameraInterval
		{
			get { return (m_EyePositions[m_FrontSizeIndex, m_BackSizeIndex] - m_FocusPositions[m_FrontSizeIndex, m_BackSizeIndex]).Length3(); }
			set { /* 空実装 */ }
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
		private CameraControlForm.Viewpoint m_Viewpoint;
		public CameraControlForm.Viewpoint Viewpoint
		{
			set { m_Viewpoint = value; }
		}
		private Int32 m_FrontSizeIndex;
		private Int32 m_BackSizeIndex;

		public BattleCamera(float screenWidth, float screenHeight, float fieldOfView)
		{
			Int32 size = (Int32)CameraControlForm.PokemonSize.NumberOf;
			m_EyePositions = new Vector[size, size];
			m_EyePositions[0, 0] = new Vector(125.0f, 50.0f, 360.0f);
			m_EyePositions[0, 1] = new Vector(155.0f, 56.0f, 380.0f);
			m_EyePositions[0, 2] = new Vector(185.0f, 62.0f, 400.0f);
			m_EyePositions[1, 0] = new Vector(236.0f, 110.0f, 460.0f);
			m_EyePositions[1, 1] = new Vector(256.0f, 106.0f, 465.0f);
			m_EyePositions[1, 2] = new Vector(276.0f, 102.0f, 470.0f);
			m_EyePositions[2, 0] = new Vector(220.0f, 150.0f, 470.0f);
			m_EyePositions[2, 1] = new Vector(240.0f, 140.0f, 485.0f);
			m_EyePositions[2, 2] = new Vector(260.0f, 130.0f, 500.0f);
			m_FocusPositions = new Vector[size, size];
			m_FocusPositions[0, 0] = new Vector(-50.0f, 50.0f, -40.0f);
			m_FocusPositions[0, 1] = new Vector(-45.0f, 56.0f, -40.0f);
			m_FocusPositions[0, 2] = new Vector(-40.0f, 62.0f, -40.0f);
			m_FocusPositions[1, 0] = new Vector(-50.0f, 60.0f, -20.0f);
			m_FocusPositions[1, 1] = new Vector(-45.0f, 70.0f, -20.0f);
			m_FocusPositions[1, 2] = new Vector(-40.0f, 80.0f, -20.0f);
			m_FocusPositions[2, 0] = new Vector(0.0f, 130.0f, 90.0f);
			m_FocusPositions[2, 1] = new Vector(0.0f, 130.0f, 90.0f);
			m_FocusPositions[2, 2] = new Vector(0.0f, 130.0f, 90.0f);

			m_Viewpoint = CameraControlForm.Viewpoint.FRONT_SIDE;
			m_FrontSizeIndex = 0;
			m_BackSizeIndex = 0;

			m_upDirection = Vector.GetYUnit();

			m_View = Matrix.GetLookAt(
				m_EyePositions[m_FrontSizeIndex, m_BackSizeIndex],
				m_FocusPositions[m_FrontSizeIndex, m_BackSizeIndex],
				m_upDirection);

			m_NearClip = 16.0f;
			m_FarClip = 6000.0f;

			m_FieldOfView = fieldOfView;
			m_Projection = gfl2.clr.math.Matrix.GetPerspective(
				gfl2.clr.math.Util.ConvDegToRad(fieldOfView),
				screenWidth / screenHeight,
				m_NearClip,
				m_FarClip);
		}

		public void SetPokemonSize(
			CameraControlForm.PokemonSize frontSize,
			CameraControlForm.PokemonSize backSize)
		{
			m_FrontSizeIndex = (Int32)frontSize;
			m_BackSizeIndex = (Int32)backSize;
		}

		public void ResetToDefaultPosition(PokemonModelDataManager modelDataManager)
		{

		}

    public void Reset(PokemonModelDataManager modelDataManager)
		{

		}

		public void Update()
		{
			Vector eyePosition = new Vector(m_EyePositions[m_FrontSizeIndex, m_BackSizeIndex]);
			Vector focusPosition = new Vector(m_FocusPositions[m_FrontSizeIndex, m_BackSizeIndex]);
			if (m_Viewpoint == CameraControlForm.Viewpoint.BACK_SIDE)
			{
				eyePosition.x = -eyePosition.x;
				eyePosition.z = -eyePosition.z;
				focusPosition.x = -focusPosition.x;
				focusPosition.z = -focusPosition.z;
			}

			m_View = Matrix.GetLookAt(
				eyePosition,
				focusPosition,
				m_upDirection);
		}
	}
}
