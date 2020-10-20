using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ModelViewer
{
	public class Mouse
	{
		private System.Drawing.Point m_lastMousePosition;			//!< @brief 前回のマウスの座標
		private Int32 m_MoveOffsetX;
		private Int32 m_MoveOffsetY;

		public Mouse()
		{
			m_lastMousePosition = Control.MousePosition;
		}

		public bool IsButtonPressed(MouseButtons button)
		{
			return ((Control.MouseButtons & button) == button);
		}

		public void GetMoveOffset(out Int32 x, out Int32 y)
		{
			x = m_MoveOffsetX;
			y = m_MoveOffsetY;
		}

		public void Update()
		{
			System.Drawing.Point mousePosition = Control.MousePosition;
			m_MoveOffsetX = this.m_lastMousePosition.X - mousePosition.X;
			m_MoveOffsetY = this.m_lastMousePosition.Y - mousePosition.Y;
			m_lastMousePosition = mousePosition;
		}
	}
}
