using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;

namespace ModelViewer
{
  public partial class AnimationTimeLineControlForm : Form, IModelMotionResourceChangeListener
  {
    private readonly AnimationDataSetForm m_AnimationDataSetForm;
    private PokemonModelDataManager m_PokemonModelDataManager;	//!< @brief UI上で扱うPokeonModelDatamanager
    readonly ChangeControl[] m_ChangeControl;										//!< @brief 再生/停止/一時停止のUI変更用delegate

    /**
     * @brief コンストラクタ
     * @param configFilePath コンフィグファイルのパス(60fpsモード解禁などに必要)
     */
    public AnimationTimeLineControlForm(
      AnimationDataSetForm animationDataSetForm,
      string configFilePath)
    {
      InitializeComponent();
      m_AnimationDataSetForm = animationDataSetForm;
      ChangeControl[] changeControl = { ChangeControlToStopMode, ChangeControlToPauseMode, ChangeControlToPlayMode, ChangeControlToReverseMode, ChangeControlToPauseMode, ChangeControlToPauseMode };
      m_ChangeControl = changeControl;

      LoadConfigFile(configFilePath);
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

    /**
     * @brief フォーム初期化
     * @param sender
     * @param e
     */
    private void AnimationTimeLineControlForm_Load(object sender, EventArgs e)
    {
      loopCheckBox.Enabled = false;
      frameRateComboBox.Enabled = false;

      animationPlayButton.Enabled = false;
      animationStopButton.Enabled = false;
      animationPauseButton.Enabled = false;
      nextFrameButton.Enabled = false;
      prevFrameButton.Enabled = false;
      // フレームレートのコンボボックスの初期値を30fpsにする
      // todo: インデックスでは分かりにくいので別途列挙型か何かを定義
      frameRateComboBox.SelectedIndex = 1;
    }

    /**
     * @brief 設定ファイルの読み込み
     * @param ファイルパス
     * @return bool 成功/失敗
     */
    private bool LoadConfigFile(string filePath)
    {
      if (!System.IO.File.Exists(filePath))
      {
        return false;
      }

      XmlDocument document = new XmlDocument();
      document.Load(filePath);
      XmlNodeList xmlNode = document.GetElementsByTagName("FrameRate");
      XmlElement xmlName = (XmlElement)xmlNode.Item(0);
      string strEnable60fps = xmlName.GetAttribute("enable60fps");
      if (strEnable60fps == "true")
      {
        frameRateComboBox.Items.Add("60fps");
      }

      return true;
    }

    /**
     * @brief アニメーション逆再生
     */
    private void Reverse()
    {
      m_PokemonModelDataManager.AnimationTimeLine.Reverse();
    }

    /**
     * @brief アニメーション再生
     */
    private void Play()
    {
      m_PokemonModelDataManager.AnimationTimeLine.Play();
    }

    /**
     * @brief アニメーション停止
     */
    private void Stop()
    {
      m_PokemonModelDataManager.AnimationTimeLine.Stop();
    }

    /**
     * @brief アニメーション一時停止
     */
    private void Pause()
    {
      m_PokemonModelDataManager.AnimationTimeLine.Pause();
    }

    /**
     * @brief 1フレーム進める
     */
    private void ForwardFrame()
    {
      m_PokemonModelDataManager.AnimationTimeLine.ForwardFrame();
    }

    /**
     * @brief 1フレーム戻す
     */
    private void BackwardFrame()
    {
      m_PokemonModelDataManager.AnimationTimeLine.BackwardFrame();
    }

    /**
     * @brief アニメーション逆再生ボタン押下
     * @param object
     * @param EventArgs
     */
    private void animationReverseButton_Click(object sender, EventArgs e)
    {
      Reverse();
    }

    /**
     * @brief アニメーション再生ボタン押下
     * @param object
     * @param EventArgs
     */
    private void animationPlayButton_Click(object sender, EventArgs e)
    {
      Play();
    }

    /**
     * @brief アニメーション停止ボタン押下
     * @param object
     * @param EventArgs
     */
    private void animationStopButton_Click(object sender, EventArgs e)
    {
      Stop();
    }

    /**
     * @brief アニメーション一時停止ボタン押下
     * @param object
     * @param EventArgs
     */
    private void animationPauseButton_Click(object sender, EventArgs e)
    {
      Pause();
    }

    /**
     * @brief 次のフレームへ移行ボタン押下
     * @param object
     * @param EventArgs
     */
    private void nextFrameButton_Click(object sender, EventArgs e)
    {
      ForwardFrame();
    }

    /**
     * @brief 前のフレームへ移行ボタン押下
     * @param object
     * @param EventArgs
     */
    private void prevFrameButton_Click(object sender, EventArgs e)
    {
      BackwardFrame();
    }

    /**
     * @brief ループ再生チェックボックスの変更
     * @param object
     * @param EventArgs
     */
    private void loopCheckBox_CheckedChanged(object sender, EventArgs e)
    {
      if (m_PokemonModelDataManager == null)
      {
        return;
      }

      AnimationTimeLine timeLine = m_PokemonModelDataManager.AnimationTimeLine;
      timeLine.LoopEnable = this.loopCheckBox.Checked;

      // ループが有効化になった場合は, タイムラインのフレームを末尾から1フレーム削る
      TimeLineData[] timeLineDataArray = timeLine.TimeLineDataArray;
      for (Int32 i = 0; i < timeLineDataArray.Length; ++i)
      {
        if (!timeLineDataArray[i].IsAvailable)
        {
          continue;
        }

        decimal endFrame = timeLineDataArray[i].FrameSize - 1;
        if (timeLine.LoopEnable)
        {
          endFrame -= 1;
        }
        timeLineDataArray[i].EndFrame = endFrame;
        m_AnimationDataSetForm.InputEndFrame(i, endFrame);
      }
    }

    /**
     * @brief フレームレート設定コンボボックスの変更
     * @param object
     * @param EventArgs
     */
    private void frameRateComboBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (m_PokemonModelDataManager != null)
      {
        UInt32 frameResolution = (UInt32)(4 >> (frameRateComboBox.SelectedIndex));
        m_PokemonModelDataManager.AnimationTimeLine.FrameResolution = frameResolution;
      }
    }

    /**
     * @brief PokemonModelDataManagerの変更通知受け取り
     * @param modelDataManager モデルデータマネージャ
     */
    public void ReceiveNotification(PokemonModelDataManager modelDataManager)
    {
      if (m_PokemonModelDataManager != modelDataManager)
      {// 参照するモデルデータが変わったらその都度変更
        m_PokemonModelDataManager = modelDataManager;
        if (modelDataManager == null)
        {
          ChangeControlToDisableMode();
          motionSpeedLabel.Text = "1.0";
          return;
        }

        loopCheckBox.Enabled = true;
        loopCheckBox.Checked = modelDataManager.AnimationTimeLine.LoopEnable;
        frameRateComboBox.Enabled = true;
        frameRateComboBox.SelectedIndex = modelDataManager.AnimationTimeLine.GetFrameRateComboBoxIndex();
      }

      AnimationTimeLine animationTimeLine = modelDataManager.AnimationTimeLine;
      motionSpeedLabel.Text = animationTimeLine.Speed.ToString();
      if (animationTimeLine.IsEmpty())
      {
        ChangeControlToEmptyMode();
        return;
      }

      Int32 stateIndex = (Int32)modelDataManager.AnimationTimeLine.State;
      m_ChangeControl[stateIndex]();
    }

    delegate void ChangeControl();

    private void ChangeControlToDisableMode()
    {
      prevFrameButton.Enabled = false;
      animationReverseButton.Enabled = false;
      animationPlayButton.Enabled = false;
      animationStopButton.Enabled = false;
      animationPauseButton.Enabled = false;
      nextFrameButton.Enabled = false;
      frameRateComboBox.Enabled = true;
      loopCheckBox.Enabled = false;
      loopCheckBox.Checked = false;
      decrementMotionSpeedButton.Enabled = false;
      incrementMotionSpeedButton.Enabled = false;
    }

    private void ChangeControlToEmptyMode()
    {
      prevFrameButton.Enabled = false;
      animationReverseButton.Enabled = false;
      animationPlayButton.Enabled = false;
      animationStopButton.Enabled = false;
      animationPauseButton.Enabled = false;
      nextFrameButton.Enabled = false;
      frameRateComboBox.Enabled = true;
      loopCheckBox.Enabled = true;
      decrementMotionSpeedButton.Enabled = true;
      incrementMotionSpeedButton.Enabled = true;
    }

    private void ChangeControlToReverseMode()
    {
      prevFrameButton.Enabled = false;
      animationReverseButton.Enabled = false;
      animationPlayButton.Enabled = false;
      animationStopButton.Enabled = true;
      animationPauseButton.Enabled = true;
      nextFrameButton.Enabled = false;
      frameRateComboBox.Enabled = true;
      loopCheckBox.Enabled = true;
      decrementMotionSpeedButton.Enabled = true;
      incrementMotionSpeedButton.Enabled = true;
    }

    private void ChangeControlToPlayMode()
    {
      prevFrameButton.Enabled = false;
      animationReverseButton.Enabled = false;
      animationPlayButton.Enabled = false;
      animationStopButton.Enabled = true;
      animationPauseButton.Enabled = true;
      nextFrameButton.Enabled = false;
      frameRateComboBox.Enabled = true;
      loopCheckBox.Enabled = true;
      decrementMotionSpeedButton.Enabled = true;
      incrementMotionSpeedButton.Enabled = true;
    }

    private void ChangeControlToStopMode()
    {
      prevFrameButton.Enabled = true;
      animationReverseButton.Enabled = true;
      animationPlayButton.Enabled = true;
      animationStopButton.Enabled = false;
      animationPauseButton.Enabled = false;
      nextFrameButton.Enabled = true;
      frameRateComboBox.Enabled = true;
      loopCheckBox.Enabled = true;
      decrementMotionSpeedButton.Enabled = true;
      incrementMotionSpeedButton.Enabled = true;
    }

    private void ChangeControlToPauseMode()
    {
      prevFrameButton.Enabled = true;
      animationReverseButton.Enabled = true;
      animationPlayButton.Enabled = true;
      animationStopButton.Enabled = true;
      animationPauseButton.Enabled = false;
      nextFrameButton.Enabled = true;
      frameRateComboBox.Enabled = true;
      loopCheckBox.Enabled = true;
      decrementMotionSpeedButton.Enabled = true;
      incrementMotionSpeedButton.Enabled = true;
    }

    private void decrementMotionSpeedButton_Click(object sender, EventArgs e)
    {
      m_PokemonModelDataManager.AnimationTimeLine.AddSpeed(-0.1m);
    }

    private void incrementMotionSpeedButton_Click(object sender, EventArgs e)
    {
      m_PokemonModelDataManager.AnimationTimeLine.AddSpeed(0.1m);
    }
  }
}
