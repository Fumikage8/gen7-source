using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using gfl2.clr.math;
using gfl2.clr.animation;

namespace ModelViewer
{
	public partial class CameraControlForm : Form
	{
		public enum Viewpoint
		{
			FRONT_SIDE,
			BACK_SIDE,
			NumberOf
		}

		public enum PokemonSize
		{
			S,
			M,
			L,
			NumberOf
		}

    private static float MODEL_SETTING_INTERVAL = 100.0f;	//!< @brief モデル同士の間隔

		private readonly List<PokemonModelDataManager> m_PokemonModelDataManagerList;
		private Int32 m_SelectedModelDatamanagerIndex;
		public Int32 SelectedIndexOfPokemonModelDataManagerList
		{
			set { m_SelectedModelDatamanagerIndex = value; }
		}

		private readonly RenderForm m_RenderForm;	//!< @todo: カメラノードだけを受け取れるようにすべき
		private readonly TransformControlForm m_TransformControlForm;
		private Viewpoint m_ViewPoint;
		private Viewpoint ViewPoint
		{
			set
			{
				m_ViewPoint = value;
				m_RenderForm.BattleCamera.Viewpoint = m_ViewPoint;
			}
		}
		private PokemonSize m_FrontSidePokemonSize;
		private PokemonSize m_BackSidePokemonSize;
		private PokemonSize FrontSidePokemonSize
		{
			set
			{
				m_FrontSidePokemonSize = value;
				m_RenderForm.BattleCamera.SetPokemonSize(m_FrontSidePokemonSize, m_BackSidePokemonSize);
			}
		}
		private PokemonSize BackSidePokemonSize
		{
			set
			{
				m_BackSidePokemonSize = value;
				m_RenderForm.BattleCamera.SetPokemonSize(m_FrontSidePokemonSize, m_BackSidePokemonSize);
			}
		}

		private RadioButton[] m_FrontSideSizeRadioButtons;
		private RadioButton[] m_BackSideSizeRadioButtons;

		private readonly Vector[, ,] m_PokemonBattlePositions;
		private readonly float[, ,] m_PokemonBattleRotations;
    private readonly AlignModelDataManagerList[] m_AlignModelManagerListFunctions;
		private readonly ChangeCamera[] m_ChangeCameraFunctions;

		public CameraControlForm(
			RenderForm renderForm,
			TransformControlForm transformControlForm,
			List<PokemonModelDataManager> pokemonModelDataManagerList)
		{
			InitializeComponent();
			m_RenderForm = renderForm;
			m_TransformControlForm = transformControlForm;
			m_PokemonModelDataManagerList = pokemonModelDataManagerList;
			m_SelectedModelDatamanagerIndex = -1;
			m_ViewPoint = Viewpoint.FRONT_SIDE;
			m_FrontSidePokemonSize = PokemonSize.S;
			m_BackSidePokemonSize = PokemonSize.S;
			Int32 size = (Int32)PokemonSize.NumberOf;
			Int32 viewpoint = (Int32)Viewpoint.NumberOf;
			
			m_PokemonBattlePositions = new Vector[viewpoint, size, size];
			m_PokemonBattlePositions[0, 0, 0] = new Vector(0.0f, 0.0f, 200.0f);
			m_PokemonBattlePositions[1, 0, 0] = new Vector(0.0f, 0.0f, -200.0f);
			m_PokemonBattlePositions[0, 0, 1] = new Vector(0.0f, 0.0f, 200.0f);
			m_PokemonBattlePositions[1, 0, 1] = new Vector(0.0f, 0.0f, -200.0f);
			m_PokemonBattlePositions[0, 0, 2] = new Vector(0.0f, 0.0f, 200.0f);
			m_PokemonBattlePositions[1, 0, 2] = new Vector(0.0f, 0.0f, -200.0f);
			m_PokemonBattlePositions[0, 1, 0] = new Vector(0.0f, 0.0f, 200.0f);
			m_PokemonBattlePositions[1, 1, 0] = new Vector(0.0f, 0.0f, -200.0f);
			m_PokemonBattlePositions[0, 1, 1] = new Vector(0.0f, 0.0f, 200.0f);
			m_PokemonBattlePositions[1, 1, 1] = new Vector(0.0f, 0.0f, -200.0f);
			m_PokemonBattlePositions[0, 1, 2] = new Vector(0.0f, 0.0f, 200.0f);
			m_PokemonBattlePositions[1, 1, 2] = new Vector(0.0f, 0.0f, -200.0f);
			m_PokemonBattlePositions[0, 2, 0] = new Vector(0.0f, 0.0f, 200.0f);
			m_PokemonBattlePositions[1, 2, 0] = new Vector(0.0f, 0.0f, -200.0f);
			m_PokemonBattlePositions[0, 2, 1] = new Vector(0.0f, 0.0f, 200.0f);
			m_PokemonBattlePositions[1, 2, 1] = new Vector(0.0f, 0.0f, -200.0f);
			m_PokemonBattlePositions[0, 2, 2] = new Vector(0.0f, 0.0f, 200.0f);
			m_PokemonBattlePositions[1, 2, 2] = new Vector(0.0f, 0.0f, -200.0f);
			m_PokemonBattleRotations = new float[viewpoint, size, size];
			m_PokemonBattleRotations[0, 0, 0] = 180.0f;
			m_PokemonBattleRotations[1, 0, 0] = 0.0f;
			m_PokemonBattleRotations[0, 0, 1] = 180.0f;
			m_PokemonBattleRotations[1, 0, 1] = 0.0f;
			m_PokemonBattleRotations[0, 0, 2] = 180.0f;
			m_PokemonBattleRotations[1, 0, 2] = 0.0f;
			m_PokemonBattleRotations[0, 1, 0] = 180.0f;
			m_PokemonBattleRotations[1, 1, 0] = 0.0f;
			m_PokemonBattleRotations[0, 1, 1] = 180.0f;
			m_PokemonBattleRotations[1, 1, 1] = 0.0f;
			m_PokemonBattleRotations[0, 1, 2] = 180.0f;
			m_PokemonBattleRotations[1, 1, 2] = 0.0f;
			m_PokemonBattleRotations[0, 2, 0] = 180.0f;
			m_PokemonBattleRotations[1, 2, 0] = 0.0f;
			m_PokemonBattleRotations[0, 2, 1] = 180.0f;
			m_PokemonBattleRotations[1, 2, 1] = 0.0f;
			m_PokemonBattleRotations[0, 2, 2] = 180.0f;
			m_PokemonBattleRotations[1, 2, 2] = 0.0f;

      AlignModelDataManagerList[] alignModelDataManagerListFunctions = 
      {
        AlignModelDataManagerListForDefaultCamera,
        AlignModelDataManagerListForStatusCamera,
        AlignModelDataManagerListForKwCamera,
        AlignModelDataManagerListForBattleCamera
      };
      m_AlignModelManagerListFunctions = alignModelDataManagerListFunctions;

      ChangeCamera[] changeCamerafunctions =
      {
        SetDefaultCamera,
        SetStatusCamera,
        SetKwCamera,
        SetBattleCamera
      };
			m_ChangeCameraFunctions = changeCamerafunctions;

			m_FrontSideSizeRadioButtons = new RadioButton[(Int32)PokemonSize.NumberOf];
			m_BackSideSizeRadioButtons = new RadioButton[(Int32)PokemonSize.NumberOf];
		}

		/**
		 * @brief ウインドウプロシージャ上書き
		 * @param ref Message メッセージ
		 */
		protected override void WndProc(ref Message m)
		{
			const Int32 WM_NCLBUTTONDBLCLK = 0x00A3;

			if (m.Msg == WM_NCLBUTTONDBLCLK)
			{// タイトルバーのダブルクリックをシカト
				return;
			}

			base.WndProc(ref m);
		}

		private void nearRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			ViewPoint = Viewpoint.FRONT_SIDE;
		}

		private void farRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			ViewPoint = Viewpoint.BACK_SIDE;
		}

		private void frontSideSizeSRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			FrontSidePokemonSize = PokemonSize.S;
		}

		private void frontSideSizeMRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			FrontSidePokemonSize = PokemonSize.M;
		}

		private void frontSideSizeLRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			FrontSidePokemonSize = PokemonSize.L;
		}

		private void backSideSizeSRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			BackSidePokemonSize = PokemonSize.S;
		}

		private void backSideSizeMRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			BackSidePokemonSize = PokemonSize.M;
		}

		private void backSideSizeLRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			BackSidePokemonSize = PokemonSize.L;
		}

		private void battleCameraSetButton_Click(object sender, EventArgs e)
		{
			
		}

		private void CameraControlForm_Load(object sender, EventArgs e)
		{
			m_FrontSideSizeRadioButtons[(Int32)PokemonSize.S] = frontSideSizeSRadioButton;
			m_FrontSideSizeRadioButtons[(Int32)PokemonSize.M] = frontSideSizeMRadioButton;
			m_FrontSideSizeRadioButtons[(Int32)PokemonSize.L] = frontSideSizeLRadioButton;

			m_BackSideSizeRadioButtons[(Int32)PokemonSize.S] = backSideSizeSRadioButton;
			m_BackSideSizeRadioButtons[(Int32)PokemonSize.M] = backSideSizeMRadioButton;
			m_BackSideSizeRadioButtons[(Int32)PokemonSize.L] = backSideSizeLRadioButton;

			freeCameraRadioButton.Checked = true;
			farRadioButton.Enabled = false;
			nearRadioButton.Enabled = false;
			frontSideSizeSRadioButton.Enabled = false;
			frontSideSizeMRadioButton.Enabled = false;
			frontSideSizeLRadioButton.Enabled = false;
			backSideSizeSRadioButton.Enabled = false;
			backSideSizeMRadioButton.Enabled = false;
			backSideSizeLRadioButton.Enabled = false;
		}

		private void freeCameraRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			if (!freeCameraRadioButton.Checked)
			{
				return;
			}

      AlignModelDataManagerListForDefaultCamera();
			SetDefaultCamera();
			fieldOfViewLabel.Text = m_RenderForm.GetCurrentFieldOfView().ToString();
		}

		private void statusCameraRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			if (!statusCameraRadioButton.Checked)
			{
				return;
			}

      AlignModelDataManagerListForStatusCamera();
			SetStatusCamera();
			fieldOfViewLabel.Text = m_RenderForm.GetCurrentFieldOfView().ToString();
		}

		private void kwCameraRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			if (!kwCameraRadioButton.Checked)
			{
				return;
			}

      AlignModelDataManagerListForKwCamera();
			SetKwCamera();
			fieldOfViewLabel.Text = m_RenderForm.GetCurrentFieldOfView().ToString();
		}

		private void battleCameraRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			farRadioButton.Enabled = battleCameraRadioButton.Checked;
			nearRadioButton.Enabled = battleCameraRadioButton.Checked;
			frontSideSizeSRadioButton.Enabled = battleCameraRadioButton.Checked;
			frontSideSizeMRadioButton.Enabled = battleCameraRadioButton.Checked;
			frontSideSizeLRadioButton.Enabled = battleCameraRadioButton.Checked;
			backSideSizeSRadioButton.Enabled = battleCameraRadioButton.Checked;
			backSideSizeMRadioButton.Enabled = battleCameraRadioButton.Checked;
			backSideSizeLRadioButton.Enabled = battleCameraRadioButton.Checked;
			if (!battleCameraRadioButton.Checked)
			{
				return;
			}

      AlignModelDataManagerListForBattleCamera();
			SetBattleCamera();
			fieldOfViewLabel.Text = m_RenderForm.GetCurrentFieldOfView().ToString();
		}

    public void ResetAlignment()
    {
      CameraType cameraType = m_RenderForm.CameraType;
      if (cameraType == CameraType.DEFAULT)
      {
        return;
      }
      m_AlignModelManagerListFunctions[(Int32)cameraType]();
    }

		public void ResetCamera()
		{
			CameraType cameraType = m_RenderForm.CameraType;
			m_ChangeCameraFunctions[(Int32)cameraType]();
		}

    delegate void AlignModelDataManagerList();

    private void AlignModelDataManagerListForDefaultCamera()
    {
      // 選択されているモデルが真ん中に来るように調整
      float startPositionX = -MODEL_SETTING_INTERVAL * (float)m_SelectedModelDatamanagerIndex;

      for (Int32 i = 0; i < m_PokemonModelDataManagerList.Count; ++i)
      {
        m_PokemonModelDataManagerList[i].SetVisible(true);
        m_PokemonModelDataManagerList[i].Position.x = startPositionX + MODEL_SETTING_INTERVAL * (float)i;
        m_PokemonModelDataManagerList[i].Position.y = 0.0f;
        m_PokemonModelDataManagerList[i].Position.z = 0.0f;
        m_PokemonModelDataManagerList[i].EulerRotation.x = 0.0f;
        m_PokemonModelDataManagerList[i].EulerRotation.y = 0.0f;
        m_PokemonModelDataManagerList[i].EulerRotation.z = 0.0f;
      }
    }

    private void AlignModelDataManagerListForStatusCamera()
    {
      const float DEFAULT_ROTATION = 25.21f;    //!< @brief ステータス画面での回転量
      // 選択されているモデルが真ん中に来るように調整
      float startPositionX = -MODEL_SETTING_INTERVAL * (float)m_SelectedModelDatamanagerIndex;
      for (Int32 i = 0; i < m_PokemonModelDataManagerList.Count; ++i)
      {
        m_PokemonModelDataManagerList[i].SetVisible(i == m_SelectedModelDatamanagerIndex);
        m_PokemonModelDataManagerList[i].Position.x = startPositionX + MODEL_SETTING_INTERVAL * (float)i;
        m_PokemonModelDataManagerList[i].Position.y = 0.0f;
        m_PokemonModelDataManagerList[i].Position.z = 0.0f;
        m_PokemonModelDataManagerList[i].EulerRotation.x = 0.0f;
        m_PokemonModelDataManagerList[i].EulerRotation.y = (i == m_SelectedModelDatamanagerIndex) ? DEFAULT_ROTATION : 0.0f;
        m_PokemonModelDataManagerList[i].EulerRotation.z = 0.0f;
      }
    }

    private void AlignModelDataManagerListForKwCamera()
    {
      // 選択されているモデルが真ん中に来るように調整
      float startPositionX = -MODEL_SETTING_INTERVAL * (float)m_SelectedModelDatamanagerIndex;

      for (Int32 i = 0; i < m_PokemonModelDataManagerList.Count; ++i)
      {
        m_PokemonModelDataManagerList[i].SetVisible(i == m_SelectedModelDatamanagerIndex);
        m_PokemonModelDataManagerList[i].Position.x = startPositionX + MODEL_SETTING_INTERVAL * (float)i;
        m_PokemonModelDataManagerList[i].Position.y = 0.0f;
        m_PokemonModelDataManagerList[i].Position.z = 0.0f;
        m_PokemonModelDataManagerList[i].EulerRotation.x = 0.0f;
        m_PokemonModelDataManagerList[i].EulerRotation.y = 0.0f;
        m_PokemonModelDataManagerList[i].EulerRotation.z = 0.0f;
      }
    }

    private void AlignModelDataManagerListForBattleCamera()
    {
      Int32 viewpointIndex = (Int32)m_ViewPoint;
      Int32 frontIndex = (Int32)m_FrontSidePokemonSize;
      Int32 backIndex = (Int32)m_BackSidePokemonSize;
      Int32 selectedIndex = m_SelectedModelDatamanagerIndex;
      if (selectedIndex < 0)
      {
        return;
      }

      // まず全てのモデルを見えなくしてから, 該当のモデルだけ見えるようにする
      m_PokemonModelDataManagerList.ForEach(data => data.SetVisible(false));

      PokemonModelDataManager pokemonModelDataManager = m_PokemonModelDataManagerList[selectedIndex];
      pokemonModelDataManager.SetVisible(true);
      pokemonModelDataManager.Position = new Vector(m_PokemonBattlePositions[viewpointIndex, frontIndex, backIndex]);
      float rotation = m_PokemonBattleRotations[viewpointIndex, frontIndex, backIndex];
      pokemonModelDataManager.EulerRotation = new Vector(0.0f, rotation, 0.0f);
      m_TransformControlForm.ReceiveSRTChangeNotification(pokemonModelDataManager);

      ++selectedIndex;
      if (selectedIndex > m_PokemonModelDataManagerList.Count - 1)
      {
        return;
      }

      viewpointIndex ^= 1;
      pokemonModelDataManager = m_PokemonModelDataManagerList[selectedIndex];
      pokemonModelDataManager.SetVisible(true);
      pokemonModelDataManager.Position = new Vector(m_PokemonBattlePositions[viewpointIndex, frontIndex, backIndex]);
      rotation = m_PokemonBattleRotations[viewpointIndex, frontIndex, backIndex];
      pokemonModelDataManager.EulerRotation = new Vector(0.0f, rotation, 0.0f);


    }

		delegate void ChangeCamera();

		private void SetDefaultCamera()
		{
			m_RenderForm.CameraType = CameraType.DEFAULT;
			for (Int32 i = 0; i < (Int32)PokemonSize.NumberOf; ++i)
			{
				m_FrontSideSizeRadioButtons[i].BackColor = Color.White;
				m_BackSideSizeRadioButtons[i].BackColor = Color.White;
			}

      PokemonModelDataManager pokemonModelDataManager = null;
			if (m_SelectedModelDatamanagerIndex >= 0)
			{
        pokemonModelDataManager = m_PokemonModelDataManagerList[m_SelectedModelDatamanagerIndex];
			}

      m_RenderForm.ResetCamera(pokemonModelDataManager);
      m_TransformControlForm.ReceiveSRTChangeNotification(pokemonModelDataManager);
		}

		private void SetStatusCamera()
		{
			m_RenderForm.CameraType = CameraType.STATUS;
			for (Int32 i = 0; i < (Int32)PokemonSize.NumberOf; ++i)
			{
				m_FrontSideSizeRadioButtons[i].BackColor = Color.White;
				m_BackSideSizeRadioButtons[i].BackColor = Color.White;
			}

      PokemonModelDataManager pokemonModelDataManager = null;
      if (m_SelectedModelDatamanagerIndex >= 0)
      {
        pokemonModelDataManager = m_PokemonModelDataManagerList[m_SelectedModelDatamanagerIndex];
      }

      m_RenderForm.ResetCamera(pokemonModelDataManager);
      m_TransformControlForm.ReceiveSRTChangeNotification(pokemonModelDataManager);
		}

		private void SetKwCamera()
		{
			m_RenderForm.CameraType = CameraType.KW;
			for (Int32 i = 0; i < (Int32)PokemonSize.NumberOf; ++i)
			{
				m_FrontSideSizeRadioButtons[i].BackColor = Color.White;
				m_BackSideSizeRadioButtons[i].BackColor = Color.White;
			}

      PokemonModelDataManager pokemonModelDataManager = null;
      if (m_SelectedModelDatamanagerIndex >= 0)
      {
        pokemonModelDataManager = m_PokemonModelDataManagerList[m_SelectedModelDatamanagerIndex];
      }

      m_RenderForm.ResetCamera(pokemonModelDataManager);
      m_TransformControlForm.ReceiveSRTChangeNotification(pokemonModelDataManager);
		}

		private void SetBattleCamera()
		{
			m_RenderForm.CameraType = CameraType.BATTLE;

			Int32 viewpointIndex = (Int32)m_ViewPoint;
			Int32 frontIndex = (Int32)m_FrontSidePokemonSize;
			Int32 backIndex = (Int32)m_BackSidePokemonSize;
			Int32 selectedIndex = m_SelectedModelDatamanagerIndex;
			if (selectedIndex < 0)
			{
				return;
			}

      PokemonModelDataManager pokemonModelDataManager = m_PokemonModelDataManagerList[selectedIndex];
      for (Int32 i = 0; i < (Int32)PokemonSize.NumberOf; ++i)
			{
				Int32 s = (Int32)pokemonModelDataManager.Size;
				m_FrontSideSizeRadioButtons[i].BackColor = (i == s) ? Color.Red : Color.White;
			}

			// 2体以上いるなら2体目を相手側に配置
			if (selectedIndex >= m_PokemonModelDataManagerList.Count - 1)
			{
				for (Int32 i = 0; i < (Int32)PokemonSize.NumberOf; ++i)
				{
					m_BackSideSizeRadioButtons[i].BackColor = Color.White;
				}
				return;
			}

      pokemonModelDataManager = m_PokemonModelDataManagerList[selectedIndex + 1];
      for (Int32 i = 0; i < (Int32)PokemonSize.NumberOf; ++i)
			{
				Int32 s = (Int32)pokemonModelDataManager.Size;
				m_BackSideSizeRadioButtons[i].BackColor = (i == s) ? Color.Red : Color.White;
			}
		}
	}
}
