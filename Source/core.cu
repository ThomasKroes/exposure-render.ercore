/*
*	@file
*	@author  Thomas Kroes <t.kroes at tudelft.nl>
*	@version 1.0
*	
*	@section LICENSE
*	
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <map>

using namespace std;

texture<unsigned short, 3, cudaReadModeNormalizedFloat> TexVolume0;
texture<unsigned short, 3, cudaReadModeNormalizedFloat> TexVolume1;

#include "color.h"

map<int, int> gTracersHashMap;
map<int, int> gVolumesHashMap;
map<int, int> gObjectsHashMap;
map<int, int> gClippingObjectsHashMap;
map<int, int> gTexturesHashMap;
map<int, int> gBitmapsHashMap;

CONSTANT_DEVICE float gDensityScale			= 0.0f;
CONSTANT_DEVICE float gStepFactorPrimary	= 0.0f;
CONSTANT_DEVICE float gStepFactorShadow		= 0.0f;

#include "statistics.h"
#include "tracer.h"
#include "volume.h"
#include "object.h"
#include "texture.h"
#include "bitmap.h"

DEVICE ExposureRender::Tracer*			gpTracer			= NULL;
DEVICE ExposureRender::Volume* 			gpVolumes			= NULL;
DEVICE ExposureRender::Object*			gpObjects			= NULL;
DEVICE ExposureRender::Texture*			gpTextures			= NULL;
DEVICE ExposureRender::Bitmap*			gpBitmaps			= NULL;

#include "list.cuh"

ExposureRender::Cuda::List<ExposureRender::Tracer, ExposureRender::HostTracer>					gTracers("gpTracer");
ExposureRender::Cuda::List<ExposureRender::Volume, ExposureRender::HostVolume>					gVolumes("gpVolumes");
ExposureRender::Cuda::List<ExposureRender::Object, ExposureRender::HostObject>					gObjects("gpObjects");
ExposureRender::Cuda::List<ExposureRender::Texture, ExposureRender::HostTexture>				gTextures("gpTextures");
ExposureRender::Cuda::List<ExposureRender::Bitmap, ExposureRender::HostBitmap>					gBitmaps("gpBitmaps");

#include "autofocus.cuh"
#include "render.cuh"

namespace ExposureRender
{

EXPOSURE_RENDER_DLL void BindTracer(const HostTracer& Tracer, const bool& Bind /*= true*/)
{
	if (Bind)
		gTracers.Bind(Tracer);
	else
		gTracers.Unbind(Tracer);

	gTracersHashMap = gTracers.HashMap;
}

EXPOSURE_RENDER_DLL void BindVolume(const HostVolume& Volume, const bool& Bind /*= true*/)
{
	if (Bind)
		gVolumes.Bind(Volume);
	else
		gVolumes.Unbind(Volume);

	gVolumesHashMap = gVolumes.HashMap;
}

EXPOSURE_RENDER_DLL void BindObject(const HostObject& Object, const bool& Bind /*= true*/)
{
	if (Bind)
		gObjects.Bind(Object);
	else
		gObjects.Unbind(Object);

	gObjectsHashMap = gObjects.HashMap;
}

EXPOSURE_RENDER_DLL void BindTexture(const HostTexture& Texture, const bool& Bind /*= true*/)
{
	if (Bind)
		gTextures.Bind(Texture);
	else
		gTextures.Unbind(Texture);

	gTexturesHashMap = gTextures.HashMap;
}

EXPOSURE_RENDER_DLL void BindBitmap(const HostBitmap& Bitmap, const bool& Bind /*= true*/)
{
	if (Bind)
		gBitmaps.Bind(Bitmap);
	else
		gBitmaps.Unbind(Bitmap);

	gBitmapsHashMap = gBitmaps.HashMap;
}

EXPOSURE_RENDER_DLL void Render(int TracerID, Statistics& Statistics)
{
	cudaEvent_t EventStart, EventStop;

	Cuda::HandleCudaError(cudaEventCreate(&EventStart));
	Cuda::HandleCudaError(cudaEventCreate(&EventStop));
	Cuda::HandleCudaError(cudaEventRecord(EventStart, 0));

	Tracer& Tracer = gTracers[TracerID];

	const float DensityScale		= Tracer.VolumeProperty.GetDensityScale();
	const float StepFactorPrimary	= gVolumes[gVolumesHashMap[Tracer.VolumeIDs[0]]].MinStep * Tracer.VolumeProperty.GetStepFactorPrimary();
	const float StepFactorShadow	= gVolumes[gVolumesHashMap[Tracer.VolumeIDs[0]]].MinStep * Tracer.VolumeProperty.GetStepFactorShadow();
	
	Cuda::HostToConstantDevice(&DensityScale, "gDensityScale");
	Cuda::HostToConstantDevice(&StepFactorPrimary, "gStepFactorPrimary");
	Cuda::HostToConstantDevice(&StepFactorShadow, "gStepFactorShadow");
	
	/*
	if (Tracer.NoEstimates == 0)
	{
		if (Tracer.Camera.FocusMode == Enums::AutoFocus)
		{
			float AutoFocusDistance = -1.0f;

			const Vec2i FilmUV((int)(Tracer.Camera.FocusUV[0] * (float)Tracer.FrameBuffer.Resolution[0]), (int)(Tracer.Camera.FocusUV[1] * (float)Tracer.FrameBuffer.Resolution[1]));
			ComputeAutoFocusDistance(FilmUV, AutoFocusDistance, Statistics);

			if (AutoFocusDistance >= 0.0f)
				Tracer.Camera.FocalDistance = AutoFocusDistance;
		}
	}
	*/

	gTracers.Synchronize(TracerID);

	if (Tracer.VolumeIDs[0] >= 0)
		gVolumes[Tracer.VolumeIDs[0]].Voxels.Bind(TexVolume0);

	if (Tracer.VolumeIDs[1] >= 0)
		gVolumes[Tracer.VolumeIDs[1]].Voxels.Bind(TexVolume1);

	Render(Tracer, Statistics);
		
	if (Tracer.NoiseReduction)
		BilateralFilterRunningEstimate(Tracer, Statistics);
	
	Tracer.NoEstimates++;

	Cuda::HandleCudaError(cudaEventRecord(EventStop, 0));
	Cuda::HandleCudaError(cudaEventSynchronize(EventStop));
																							
	float TimeDelta = 0.0f;
																							
	Cuda::HandleCudaError(cudaEventElapsedTime(&TimeDelta, EventStart, EventStop), "cudaEventElapsedTime");
	
	Statistics = Timing("FPS", 1000.0f / TimeDelta);
														
	Cuda::HandleCudaError(cudaEventDestroy(EventStart));
	Cuda::HandleCudaError(cudaEventDestroy(EventStop));										

}

EXPOSURE_RENDER_DLL void GetDisplayEstimate(int TracerID, ColorRGBAuc* pData)
{
	FrameBuffer& FB = gTracers[TracerID].FrameBuffer;

	Cuda::MemCopyDeviceToHost(FB.DisplayEstimate.GetData(), (ColorRGBAuc*)pData, FB.DisplayEstimate.GetNoElements());
}

}
