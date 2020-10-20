using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace ModelViewer
{
	public class Keyboard
	{
		private bool[] m_KeyPressed;
		private bool[] m_KeyPushed;

		public Keyboard()
		{
			Int32 length = Enum.GetNames(typeof(Key)).Length;
			m_KeyPressed = new bool[length];
			m_KeyPushed = new bool[length];
			m_KeyPressed = Array.ConvertAll(m_KeyPressed, data => data = false);
			m_KeyPushed = Array.ConvertAll(m_KeyPushed, data => data = false);
		}

		public bool IsKeyPressed(Key key)
		{
			return m_KeyPressed[(Int32)key];
		}

		public bool IsKeyPushed(Key key)
		{
			return m_KeyPushed[(Int32)key];
		}

		public void Update()
		{
			for (Int32 i = 1; i < m_KeyPushed.Length; ++i)
			{
				m_KeyPushed[i] = (!m_KeyPressed[i]) && System.Windows.Input.Keyboard.IsKeyDown((Key)i);
			}

			for (Int32 i = 1; i < m_KeyPressed.Length; ++i)
			{
				m_KeyPressed[i] = System.Windows.Input.Keyboard.IsKeyDown((Key)i);
			}
		}
	}
}
