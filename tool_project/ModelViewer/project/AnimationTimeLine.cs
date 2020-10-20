using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

using gfl2.clr.renderingengine.scenegraph.resource;
using gfl2.clr.animation;
using gfl2.clr.poke_3d_lib;

namespace ModelViewer
{
	/**
	 * @brief タイムラインデータ
	 */
	public class TimeLineData
	{
		/**
		 * @brief コンストラクタ
		 */
		public TimeLineData()
		{
			Initialize();
		}

		/**
		 * @brief 初期化
		 */
		public void Initialize()
		{
			m_MotionIndex = -1;
			m_StartFrame = 0;
			m_EndFrame = 0;
			m_FrameSize = 0;
			m_InterpSize = 0;		// 補間フレームの最小値は1(0フレーム補間というのは有り得ない)
			m_IsAvailable = false;
		}

		/**
		 * @brief コンストラクタ
		 * 
		 * @param motionIndex モーションの番号
		 * @param startFrame 開始フレーム
		 * @param endFrame 終了フレーム
		 * @param frameSize アニメーションのフレームの大きさ
		 * @param interpSize 補間フレーム数
		 */
		public void SetData(Int32 motionIndex, decimal startFrame, decimal endFrame, UInt32 frameSize, UInt32 interpSize)
		{
			Debug.Assert(interpSize >= 0);
			m_MotionIndex = motionIndex;
			m_StartFrame = startFrame;
			m_EndFrame = endFrame;
			m_FrameSize = frameSize;
			m_InterpSize = interpSize;
		}

		private Int32 m_MotionIndex;		//!< @brief モーション番号
		/**
		 * @brief m_MotionIndexのget
		 */
		public Int32 MotionIndex
		{
			get { return this.m_MotionIndex; }
		}

		private decimal m_StartFrame;		//!< @brief 開始フレーム
		/**
		 * @brief m_StartFrameのget/set
		 */
		public decimal StartFrame
		{
			get { return this.m_StartFrame; }
			set { this.m_StartFrame = value; }
		}

		private decimal m_EndFrame;		//!< @brief 終了フレーム
		/**
		 * @brief m_EndFrameのget/set
		 */
		public decimal EndFrame
		{
			get { return this.m_EndFrame; }
			set { this.m_EndFrame = value; }
		}

		private bool m_IsAvailable;		//!< @brief 再生する/しない
		/**
		 * @brief m_IsAvailableのget/set
		 */
		public bool IsAvailable
		{
			get { return m_IsAvailable; }
			set { m_IsAvailable = value; }
		}

		/**
		 * @brief タイムラインの長さ
		 */
		public decimal Length
		{
			get { return (m_EndFrame - m_StartFrame + 1.0m); }
		}

		private UInt32 m_FrameSize;		//!< @brief モーションのフレームの総数
		/**
		 * @brief m_FrameSizeのget
		 */
		public UInt32 FrameSize
		{
			get { return this.m_FrameSize; }
		}

		private UInt32 m_InterpSize;		//!< @brief 補間フレーム数
		/**
		 * @brief m_InterpSizeのget/set
		 */
		public UInt32 InterpSize
		{
			get 
      {
        return this.m_InterpSize;
      }
			set 
			{
				Debug.Assert(value >= 0);
				this.m_InterpSize = value;
			}
		}
	}

	/**
	 * @brief アニメーションのタイムライン
	 */
	public class AnimationTimeLine
	{
		/**
		 * @brief コンストラクタ
		 * @param motionNum タイムライン上で扱うモーションの最大数
		 * @param 再生時のフレーム分解能
		 * @param ループON/OFF
		 */
		public AnimationTimeLine(Int32 motionNum, decimal frameResolution, bool loopEnable)
		{
			m_TimeLineDataArray = new TimeLineData[motionNum];
      m_LoopPlayFrame = new decimal[LoopAnimationBit.Length];
			Initialize(frameResolution, loopEnable);
		}

		/*+
		 * @brief 初期化
		 */
		public void Initialize(decimal frameResolution, bool loopEnable)
		{
			m_State = STATE.STOP;
      m_StateOld = m_State;
			m_CurrentFrame = 0;
			m_LoopEnable = loopEnable;
			m_FrameResolutionCount = 0;
			m_FrameRateAutoSettingEnable = false;
			m_FrameResolution = frameResolution;				// モーションデータが30fps基準なので2イント標準
			m_Speed = 1.0m;
			for (Int32 i = 0; i < m_TimeLineDataArray.Length; ++i)
			{
				m_TimeLineDataArray[i] = new TimeLineData();
			}
		}

		/**
		 * @brief 更新
		 * @param model 再生するモデル
     *
     * @return タプル(Itme1 再生が完了したかどうか, Item2 加算したフレーム数)
		 */
		public System.Tuple<bool, decimal> Update(PokemonModel model)
		{
      var result = UpdateCore(model);

      m_StateOld = m_State;

      return result;
		}
		private System.Tuple<bool, decimal> UpdateCore(PokemonModel model)
    {
			if (!IsPlayingAnimation())
			{
        return System.Tuple.Create(false, 0.0m);
			}

			m_FrameResolutionCount += 2;
			if (m_FrameResolutionCount < m_FrameResolution)
			{
        return System.Tuple.Create(false, 0.0m);
			}

			decimal frame = (m_FrameResolution / 2.0m) * m_Speed;
			if ((m_State == STATE.REVERSE) || (m_State == STATE.BACKWARD_FRAME))
			{
				frame = -frame;
			}
			bool isFinished = AddFrame(frame, model);
      bool isStopped = isFinished && !m_LoopEnable;

      // 停止状態に入るなら, 総再生時間を0にする.
      if (isStopped && (m_State != STATE.BACKWARD_FRAME))
      {
        Stop();
      }

      // 1コマ送りならポーズに戻す
      if ((m_State == STATE.FORWARD_FRAME) || (m_State == STATE.BACKWARD_FRAME))
      {
        m_State = STATE.PAUSE;
      }

      return System.Tuple.Create(isFinished, frame);
    }

		/**
		 * @brief 逆再生
		 */
		public void Reverse()
		{
			if (m_CurrentFrame == 0.0m)
			{// 0フレーム目だったら終端のフレームに移行する
				AddFrame(-m_Speed, m_Model);
			}
			m_State = STATE.REVERSE;
		}

		/**
		 * @brief 再生
		 */
		public void Play()
		{
			m_State = STATE.PLAY;
		}

		/**
		 * @brief 停止
		 */
		public void Stop()
		{
			m_State = STATE.STOP;
			Reset();
		}

		/**
		 * @brief 一時停止
		 */
		public void Pause()
		{
			m_State = STATE.PAUSE;
		}

		/**
		 * @brief リセット(再生開始前の状態に戻す)
		 */
		public void Reset()
		{
			m_CurrentFrame = 0.0m;
			m_FrameResolutionCount = 0.0m;
      for (Int32 i = 0; i < AnimationTimeLine.LoopAnimationBit.Length; ++i)
      {
        SetLoopPlayFrame(AnimationTimeLine.LoopAnimationBit[i], 0.0m);
      }
		}

		/**
		 * @brief フレームを進める
		 * @return bool 成功/失敗
		 */
		public void ForwardFrame()
		{
      m_State = STATE.FORWARD_FRAME;
		}

		/**
		 * @brief フレームを戻す
		 * @return bool 成功/失敗
		 */
		public void BackwardFrame()
		{
      m_State = STATE.BACKWARD_FRAME;
		}

		/**
		 * @brief フレームを加算
		 * @param frame 進めるフレーム数
		 * return bool 成功/失敗
		 */
		private bool AddFrame(decimal frame, PokemonModel model)
		{
			if (frame == 0.0m)
			{// わざわざ0.0fで呼び出すことはないはずだが念のため
				return false;
			}

			if (m_Model != null)
			{
				if (m_Model == model)
				{
					m_Model.ClearKeySoundMatchFlags();
				}
			}

			// タイムライン全体を管理するフレームの更新
			bool isFinished = false;
			m_CurrentFrame = m_CurrentFrame + frame;
			decimal totalFrame = CalculateTimeLineTotalLength();

      // ループモーションにジョイントが含まれていたら, ループ再生は全て一定のスピードにする
      var loopMotionMax = (uint)(OverridePartsType.Enum.Loop04 - OverridePartsType.Enum.Loop01) + 1;
      bool hasJointAnimationInLoopMotion = false;
      for (uint loopMotionNo = 0; loopMotionNo < loopMotionMax; ++loopMotionNo)
      {
        if (m_Model.HasJointAnimationInLoopMotion(loopMotionNo))
        {
          hasJointAnimationInLoopMotion = true;
          break;
        }
      }

      // ループモーションがジョイントを持っている場合は, スピードをジョイントに合わせる
      decimal loopMotionSpeed = frame;
      if (hasJointAnimationInLoopMotion)
      {
        loopMotionSpeed = (frame > 0.0m) ? m_Speed : -m_Speed;
      }

      // ループ用の再生フレームも更新. ただ必ずスピードは一定にする
      for (Int32 i = 0; i < LoopAnimationBit.Length; ++i)
      {
        decimal loopPlayFrame = GetLoopPlayFrame(LoopAnimationBit[i]);
        loopPlayFrame += loopMotionSpeed;
        SetLoopPlayFrame(AnimationTimeLine.LoopAnimationBit[i], loopPlayFrame);
      }

			// 上限, 下限のチェック. 一周していたら0に戻す(PokeViewerの仕様と同様)
      decimal upperLimit = (LoopEnable) ? totalFrame - m_Speed : totalFrame - 1;
      decimal currentFrame = (LoopEnable) ? Math.Floor(m_CurrentFrame) : m_CurrentFrame;
			if (m_CurrentFrame < 0.0m)
			{
        m_CurrentFrame = upperLimit;
        if (m_StateOld != STATE.STOP)
        {
          isFinished = true;
        }
			}
      else if (currentFrame > upperLimit)
			{
        m_CurrentFrame -= (upperLimit + m_Speed);
        if (m_CurrentFrame < 1.0m)
        {
          // PokeViewerは0フレに戻しているので同じ挙動にしておく
          m_CurrentFrame = 0.0m;
        }
				isFinished = true;
			}

			m_FrameResolutionCount = 0.0m;

			return isFinished;
		}

		/**
		 * @brief 現在のモーションの補間モーションと補間比率を取得
		 * @param[out] prevMotionIndex 一つ前のモーション番号
		 * @param[out] prevMotionFrame 一つ前のモーションのフレーム数
		 * @param[out] interpRatio 補間フレーム比率
		 * @return Int32 タイムライン上のインデックス
		 */
		public Int32 GetCurrentBlendMotion(
			out Int32 prevMotionIndex,
			out decimal prevMotionFrame,
			out decimal interpRatio)
		{
			prevMotionIndex = -1;				// 登録されているモーションのインデックス
			prevMotionFrame = 0.0m;
			interpRatio = -1.0m;
			decimal currentFrame = m_CurrentFrame;

      Int32 availableMotionNum = Array.FindAll(m_TimeLineDataArray, data => data.IsAvailable).Length;
			Int32 availableMotionCount = 0;
			for (Int32 i = 0; i < m_TimeLineDataArray.Length; ++i)
			{
				if (!m_TimeLineDataArray[i].IsAvailable)
				{
					continue;
				}

				if ((m_TimeLineDataArray[i].EndFrame <= currentFrame) && (currentFrame < (m_TimeLineDataArray[i].EndFrame + m_TimeLineDataArray[i].InterpSize)))
				{
					prevMotionIndex = m_TimeLineDataArray[i].MotionIndex;
					prevMotionFrame = currentFrame;
					if (prevMotionFrame > (m_TimeLineDataArray[i].FrameSize - 1))
					{
						prevMotionFrame -= (m_TimeLineDataArray[i].FrameSize);
            // スローモーション設定の場合はマイナス値になる可能性があるため0以上になるようにする
            prevMotionFrame = Math.Max(0.0m, prevMotionFrame);
					}
          if (availableMotionNum != (availableMotionCount))
          {// タイムライン上で最後のモーションでなければ補間が必要なので算出
            interpRatio = (currentFrame - m_TimeLineDataArray[i].EndFrame) / m_TimeLineDataArray[i].InterpSize;
          }
					return i;
				}

				currentFrame -= m_TimeLineDataArray[i].Length;
				if ((availableMotionCount < availableMotionNum) && (m_TimeLineDataArray[i].InterpSize > 0))
				{
					currentFrame += 1.0m;
				}
				++availableMotionCount;
			}

			return -1;
		}

		/**
		 * @brief 現在のモーションのフレーム数, モーション番号, 補間比率を取得
		 * @param[out] motionIndex モーション番号
		 * @param[out] motionFrame モーションのフレーム数
		 * @return Int32 タイムライン上のインデックス
		 */
		public Int32 GetCurrentMotion(out Int32 motionIndex, out decimal motionFrame)
		{
			Int32 index = -1;
			motionIndex = -1;
			motionFrame = 0;

			Int32 availableMotionNum = Array.FindAll(m_TimeLineDataArray, data => data.IsAvailable).Length;

			decimal currentFrame = m_CurrentFrame;
			Int32 availableMotionCount = 0;
			for (Int32 i = 0; i < m_TimeLineDataArray.Length; ++i)
			{
				if (!m_TimeLineDataArray[i].IsAvailable)
				{
					continue;
				}

				decimal upperLimit = m_TimeLineDataArray[i].Length;
				if ((availableMotionCount < availableMotionNum) && (m_TimeLineDataArray[i].InterpSize > 0))
				{
					upperLimit -= 1.0m;
				}
				if ((0.0m <= currentFrame) && (currentFrame < upperLimit))
				{
					motionIndex = m_TimeLineDataArray[i].MotionIndex;
					motionFrame = (m_TimeLineDataArray[i].StartFrame + currentFrame);
					if (motionFrame > m_TimeLineDataArray[i].FrameSize - 1)
					{
						motionFrame -= (m_TimeLineDataArray[i].FrameSize - 1);
					}
					index = i;
					break;
				}
				currentFrame -= upperLimit;
				++availableMotionCount;
			}

			return index;
		}

		/**
		 * @brief アニメーションの設定
		 * @param pokemonModel ポケモンモデル
		 * @param index TimeLine上のインデックス
		 * @param motionIndex PokemonModelに格納されているモーション番号
		 */
		public void SetMotion(PokemonModel pokemonModel, Int32 index, Int32 motionIndex)
		{
      int slotIndex = 0;
      PokemonModel.AnimationBit animationBit = PokemonModel.AnimationBit.ANIMATION_BIT_ALL;

			ResourceNode motionResourceRootNode = pokemonModel.GetMotionResourceNode(motionIndex);
      pokemonModel.ChangeAnimation(motionResourceRootNode, slotIndex, animationBit);
			pokemonModel.SetAnimationFrame(0.0f, slotIndex, animationBit);
			pokemonModel.SetAnimationStepFrame(0.0f, slotIndex, animationBit);
			pokemonModel.SetKeyCallBackInfo();

			UInt32 frameCount = (UInt32)pokemonModel.GetAnimationEndFrame(slotIndex, animationBit);
			m_TimeLineDataArray[index].SetData(motionIndex, 0, frameCount, frameCount + 1, 0);
		}

		/**
		 * @brief アニメーションの有効/無効の切替
		 * @param pokemonModel ポケモンモデル
		 * @param enable ループモーション有効/無効
		 * @param pFaceMotionResourceRootNodes 表情操作用リソース
		 * @todo: 表情操作用のリソースもネイティブ側で持っておくこと.
		 */
		private void SetEnabledLoopMotion(PokemonModel pokemonModel, bool enable, ResourceNode[] pFaceMotionResourceRootNodes)
		{
			for (Int32 loopMotionIndex = (Int32)gfl2.clr.animation.OverridePartsType.Enum.Loop01;
								loopMotionIndex <= (Int32)gfl2.clr.animation.OverridePartsType.Enum.Loop04;
								++loopMotionIndex)
			{
        // GFModelViewerはnijiと異なり,
        // 0 : 通常のモーション
        // 1～10 : eye01～eye03, mouth01～mouth03, loop01～loop04
        // となっているため, インデックスを + 1している
        // (nijiのPokeModelクラスでは,
        // 0 : 通常のモーション
        // 1 : バトル時の登場のOrigin移動モーション
        // 2～11 : eye01～eye03, mouth01～mouth03, loop01～loop04
        // となっている)
        int slotIndex = loopMotionIndex + 1;
        PokemonModel.AnimationBit animationBit = PokemonModel.AnimationBit.ANIMATION_BIT_ALL;

				pokemonModel.ChangeAnimation(
					(enable) ? pFaceMotionResourceRootNodes[loopMotionIndex] : null,
          slotIndex,
          animationBit);
			}
		}

		/**
		 * @brief 開始フレームの取得
		 * @param index モーション番号
		 * @return UInt32 開始フレーム
		 */
		public decimal GetStartFrame(UInt32 index)
		{
			Debug.Assert(m_TimeLineDataArray[index] != null);
			return m_TimeLineDataArray[index].StartFrame;
		}

		/**
		 * @brief 開始フレームの設定
		 * @param index モーション番号
		 * @param string 開始フレーム(文字列. UIから呼び出される前提の設計. 他で呼び出すことは想定していない)
		 * @return bool 成功/失敗
		 */
		public bool SetStartFrame(UInt32 index, string strFrame)
		{
			Int32 frame = 0;
			bool isSucceeded = Int32.TryParse(strFrame, out frame);
			if (!isSucceeded)
			{
				return false;
			}

			if (frame < 0)
			{
				return false;
			}

			if (frame >= m_TimeLineDataArray[index].EndFrame)
			{
				return false;
			}

			if (frame >= m_TimeLineDataArray[index].FrameSize)
			{
				return false;
			}

			m_TimeLineDataArray[index].StartFrame = (decimal)frame;
			CalculateTimeLineTotalLength();

			return true;
		}

		/**
		 * @brief 終了フレームの取得
		 * @param index モーション番号
		 * @return UInt32 終了フレーム
		 */
		public decimal GetEndFrame(UInt32 index)
		{
			Debug.Assert(m_TimeLineDataArray[index] != null);
			return m_TimeLineDataArray[index].EndFrame;
		}

		/**
		 * @brief 終了フレームの設定
		 * @param index モーション番号
		 * @param string 終了フレーム(文字列. UIから呼び出される前提の設計. 他で呼び出すことは想定していない)
		 * @return bool 成功/失敗
		 */
		public bool SetEndFrame(UInt32 index, string strFrame)
		{
			Int32 frame = 0;
			bool isSucceeded = Int32.TryParse(strFrame, out frame);
			if (!isSucceeded)
			{
				return false;
			}

			if (frame <= m_TimeLineDataArray[index].StartFrame)
			{
				return false;
			}

			if (frame >= m_TimeLineDataArray[index].FrameSize)
			{
				return false;
			}

			m_TimeLineDataArray[index].EndFrame = (decimal)frame;

			if (m_FrameRateAutoSettingEnable)
			{
				Int32 prevIndex = Array.FindIndex(m_TimeLineDataArray, data => data.IsAvailable == true);

				for (Int32 i = prevIndex + 1; i < m_TimeLineDataArray.Length - 1; ++i)
				{
					if (!m_TimeLineDataArray[i].IsAvailable)
					{
						continue;
					}

					Int32 motionBlendFrame = (Int32)m_TimeLineDataArray[i].InterpSize;
					if (frame > m_TimeLineDataArray[prevIndex].FrameSize - 1 - motionBlendFrame)
					{
						return false;
					}
				}
			}

			return true;
		}

		/**
		 * @brief 補間フレームの取得
		 * @param index モーション番号
		 * @return UInt32 補間フレーム
		 */
		public UInt32 GetBlendFrame(UInt32 index)
		{
			Debug.Assert(m_TimeLineDataArray[index] != null);
			return m_TimeLineDataArray[index].InterpSize;
		}

		/**
		 * @brief 補間フレームの設定
		 * @param index 配列上のモーション番号
		 * @param string 補間フレーム(文字列. UIから呼び出される前提の設計. 他で呼び出すことは想定していない)
		 * @return bool 成功/失敗
		 */
		public bool SetBlendFrame(UInt32 index, string strFrame)
		{
			Int32 frame = 0;
			bool isSucceeded = Int32.TryParse(strFrame, out frame);
			if (!isSucceeded)
			{
				return false;
			}

			if (frame <= 0)
			{// モーション補間が0フレームということは有り得ないので弾く
				return false;
			}

			if (m_FrameRateAutoSettingEnable)
			{
				Int32 prevIndex = Array.FindIndex(m_TimeLineDataArray, data => data.IsAvailable == true);

				for (Int32 i = prevIndex + 1; i < m_TimeLineDataArray.Length; ++i)
				{
					if (!m_TimeLineDataArray[i].IsAvailable)
					{
						continue;
					}

					Int32 prevMotionFrameSize = (Int32)m_TimeLineDataArray[prevIndex].FrameSize;
					decimal endFrame = (decimal)(prevMotionFrameSize - 1 - frame);
					if (endFrame <= 0.0m)
					{
						endFrame = 1.0m;
						frame = (Int32)m_TimeLineDataArray[prevIndex].FrameSize - 2;
					}
					m_TimeLineDataArray[prevIndex].EndFrame = endFrame;
					break;
				}
			}

			// 補間フレームは補間後モーションのタイムラインに設定するので, index + 1に設定
			m_TimeLineDataArray[index + 1].InterpSize = (UInt32)frame;
			//m_TotalLength = m_TimeLineDataList.Sum(data => data.Length) - m_TimeLineDataList.Count + 1;

			return true;
		}

		/**
		 * @brief 補間フレーム自動設定
		 * @param enable 有効化/無効化
		 */
		public void SetFrameRateAutoSettingEnable(bool enable)
		{
			m_FrameRateAutoSettingEnable = enable;

			if (!m_FrameRateAutoSettingEnable)
			{
				return;
			}

			Int32 prevIndex = Array.FindIndex(m_TimeLineDataArray, data => data.IsAvailable == true);
			for (Int32 i = prevIndex + 1; i < m_TimeLineDataArray.Length; ++i)
			{
				if (!m_TimeLineDataArray[i].IsAvailable)
				{
					continue;
				}

				m_TimeLineDataArray[i].StartFrame = 0;
				decimal endFrame = (Int32)m_TimeLineDataArray[prevIndex].FrameSize - 1 - (Int32)m_TimeLineDataArray[prevIndex].InterpSize;
				if (endFrame <= 0.0m)
				{
					endFrame = 1.0m;
					m_TimeLineDataArray[i].InterpSize = m_TimeLineDataArray[prevIndex].FrameSize - 2;
				}
				m_TimeLineDataArray[prevIndex].EndFrame = endFrame;
				prevIndex = i;
			}
		}

		/**
		 * @brief アニメーション再生可能かどうかの判定
		 */
		public bool IsAnimationEnable()
		{
			return (Array.Find(m_TimeLineDataArray, data => data.IsAvailable == true) != null);
		}

		/**
		 * @brief タイムラインに何も登録されていないかどうかの判定
		 */
		public bool IsEmpty()
		{
			return m_TimeLineDataArray.All(data => (data.IsAvailable == false));
		}

		/**
		 * @brief 現在アニメーションが再生中かどうかの判定
		 */
		public bool IsPlayingAnimation()
		{
			return (m_State == STATE.PLAY) || (m_State == STATE.REVERSE) || (m_State == STATE.FORWARD_FRAME) || (m_State == STATE.BACKWARD_FRAME);
		}

		/**
		 * @brief 現在のタイムラインの総フレーム数を計算
		 * @return decimal タイムラインの総フレーム数
		 */
		public decimal CalculateTimeLineTotalLength()
		{
			decimal length = 0;
			decimal availableNum = 0;
      for (Int32 i = 0; i < m_TimeLineDataArray.Length; ++i)
      {
        if (!m_TimeLineDataArray[i].IsAvailable)
        {
          continue;
        }
        length += m_TimeLineDataArray[i].Length;
        if (m_TimeLineDataArray[i].InterpSize > 0)
        {
          ++availableNum;
        }
      }

			return length - availableNum;
		}

		/**
		 * @brief 指定したインデックスより後に設定されているデータがあるかのチェック
		 * @param index 指定するインデックス
		 * @return 設定されているインデックス(無ければ-1)
		 */
		public Int32 FindFirstAvailableIndex(Int32 index)
		{
			for (Int32 i = index + 1; i < m_TimeLineDataArray.Length; ++i)
			{
				if (m_TimeLineDataArray[i].IsAvailable)
				{
					return i;
				}
			}
			return -1;
		}

		/**
		 * @brief 指定したインデックスより前に設定されているデータがあるかのチェック
		 * @param index 指定するインデックス
		 * @return 設定されているインデックス(無ければ-1)
		 */
		public Int32 FindLastAvailableIndex(Int32 index)
		{
			for (Int32 i = index - 1; i >= 0; --i)
			{
				if (m_TimeLineDataArray[i].IsAvailable)
				{
					return i;
				}
			}
			return -1;
		}

		//!< @brief 停止/一時停止/再生/ 巻き戻し
		public enum STATE
		{
			STOP,
			PAUSE,
			PLAY,
			REVERSE,
      FORWARD_FRAME,
      BACKWARD_FRAME,
			NumberOf
		}

		private PokemonModel m_Model;	//< @brief アニメーション操作対象のモデル
		public PokemonModel Model
		{
			set { m_Model = value; }
		}

		private STATE m_State;		//!< @brief 現在の状態
		private STATE m_StateOld;		//!< @brief 前回の状態
		/**
		 * @brief m_Stateのget
		 */
		public STATE State
		{
			get { return m_State; }
		}
		private decimal m_CurrentFrame;						//!< @brief 現在のタイムライン上のフレーム
		public decimal CurrentFrame
		{
			get { return m_CurrentFrame; }
		}

		private decimal m_FrameResolutionCount;		//!< @brief フレームの分解能のカウンタ
		private decimal m_FrameResolution;				//!< @brief フレームの分解能
		/**
		 * @brief m_FrameResolutionのget/set
		 */
		public decimal FrameResolution
		{
			get { return m_FrameResolution; }
			set { m_FrameResolution = value; }
		}

		public Int32 GetFrameRateComboBoxIndex()
		{
			Int32 resolution = (Int32)m_FrameResolution;
			Int32 index = ((4 / resolution) / 2);
			return index;
		}

    //!< @brief ループモーションの種類
    public static PokemonModel.AnimationBit[] LoopAnimationBit = new PokemonModel.AnimationBit[]
    {
      PokemonModel.AnimationBit.ANIMATION_BIT_JOINT,
      PokemonModel.AnimationBit.ANIMATION_BIT_MATERIAL,
      PokemonModel.AnimationBit.ANIMATION_BIT_VISIBILITY,
    };

		private bool m_LoopEnable;							//!< @brief ループ可能/不可

    //!< @brief 各ループモーションの総再生時間
		private decimal[] m_LoopPlayFrame;
    public decimal GetLoopPlayFrame(PokemonModel.AnimationBit loopAnimationBit)
    {
      for (Int32 i = 0; i < LoopAnimationBit.Length; ++i)
      {
        if (loopAnimationBit == LoopAnimationBit[i])
        {
          return m_LoopPlayFrame[i];
        }
      }
      // ループモーションで制御していないアニメーションのビットが来ているので止める
      Debug.Assert(false);
      return -1;
    }

    public void SetLoopPlayFrame(PokemonModel.AnimationBit loopAnimationBit, decimal frame)
    {
      for (Int32 i = 0; i < LoopAnimationBit.Length; ++i)
      {
        if (loopAnimationBit == LoopAnimationBit[i])
        {
          m_LoopPlayFrame[i] = frame;
          return;
        }
      }
      // ループモーションで制御していないアニメーションのビットが来ているので止める
      Debug.Assert(false);
    }

		/**
		 * @brief m_LoopEnableのget/set
		 */
		public bool LoopEnable
		{
			get { return m_LoopEnable; }
			set { m_LoopEnable = value; }
		}

		private decimal m_Speed;
		/**
		 * @brief m_Speedのget/set
		 */
		public decimal Speed
		{
			get { return m_Speed; }
		}
		public void AddSpeed(decimal speed)
		{
			decimal spd = m_Speed + speed;
			m_Speed = Math.Min(Math.Max(0.1m, spd), 2.0m);
		}

		private bool m_FrameRateAutoSettingEnable;	//!< @brief フレームレート自動設定のON/OFF
		/**
		 * @brief m_IsFrameRateAutoSettingEnableのget
		 */
		public bool FrameRateAutoSettingEnable
		{
			get { return m_FrameRateAutoSettingEnable; }
		}

		private TimeLineData[] m_TimeLineDataArray;	//!< アニメーション再生用のデータを保持する配列
		/**
		 * @brief m_TimeLineDataArrayのget
		 */
		public TimeLineData[] TimeLineDataArray
		{
			get { return m_TimeLineDataArray; }
		}
	}
}
