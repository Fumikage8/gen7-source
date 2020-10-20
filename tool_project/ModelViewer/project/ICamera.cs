using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using gfl2.clr.math;

namespace ModelViewer
{
	public interface ICamera
	{
		void Update();
    void Reset(PokemonModelDataManager modelDataManager);
		Matrix View { get; }
		Matrix Projection { get; }
		float FieldOfView { get; }
		float NearClip { get; }
		float FarClip { get; }
		float CameraRotationX { get; set; }
		float CameraRotationY { get; set; }
		float CameraTranslationX { get; set; }
		float CameraTranslationY { get; set; }
		float CameraInterval { get; set; }
		void ResetToDefaultPosition(PokemonModelDataManager modelDataManager);
	}
}
