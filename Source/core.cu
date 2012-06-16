/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <map>

using namespace std;

texture<unsigned short, 3, cudaReadModeNormalizedFloat> TexVolume0;

#include "color.h"

texture<float, 1, cudaReadModeElementType>			Opacity1D;
texture<float4, 1, cudaReadModeElementType> 		Diffuse1D;
texture<float4, 1, cudaReadModeElementType> 		Specular1D;
texture<float, 1, cudaReadModeElementType> 			Glossiness1D;
texture<float, 1, cudaReadModeElementType> 			IndexOfReflection1D;
texture<float4, 1, cudaReadModeElementType> 		Emission1D;

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
#include "light.h"
#include "object.h"
#include "clippingobject.h"
#include "texture.h"
#include "bitmap.h"

DEVICE ExposureRender::Tracer*			gpTracer			= NULL;
DEVICE ExposureRender::Volume* 			gpVolumes			= NULL;
DEVICE ExposureRender::Object*			gpObjects			= NULL;
DEVICE ExposureRender::ClippingObject*	gpClippingObjects	= NULL;
DEVICE ExposureRender::Texture*			gpTextures			= NULL;
DEVICE ExposureRender::Bitmap*			gpBitmaps			= NULL;

#include "list.cuh"

ExposureRender::Cuda::List<ExposureRender::Tracer, ExposureRender::ErTracer>					gTracers("gpTracer");
ExposureRender::Cuda::List<ExposureRender::Volume, ExposureRender::ErVolume>					gVolumes("gpVolumes");
ExposureRender::Cuda::List<ExposureRender::Object, ExposureRender::ErObject>					gObjects("gpObjects");
ExposureRender::Cuda::List<ExposureRender::ClippingObject, ExposureRender::ErClippingObject>	gClippingObjects("gpClippingObjects");
ExposureRender::Cuda::List<ExposureRender::Texture, ExposureRender::ErTexture>					gTextures("gpTextures");
ExposureRender::Cuda::List<ExposureRender::Bitmap, ExposureRender::ErBitmap>					gBitmaps("gpBitmaps");

#include "autofocus.cuh"
#include "singlescattering.cuh"
#include "filtering.cuh"
#include "estimate.cuh"
#include "tonemap.cuh"
#include "composite.cuh"
#include "emptyspace.cuh"

namespace ExposureRender
{

EXPOSURE_RENDER_DLL void BindTracer(const ErTracer& Tracer, const bool& Bind /*= true*/)
{
	if (Bind)
		gTracers.Bind(Tracer);
	else
		gTracers.Unbind(Tracer);

	gTracersHashMap = gTracers.HashMap;
}

EXPOSURE_RENDER_DLL void BindVolume(const ErVolume& Volume, const bool& Bind /*= true*/)
{
	if (Bind)
		gVolumes.Bind(Volume);
	else
		gVolumes.Unbind(Volume);

	gVolumesHashMap = gVolumes.HashMap;
}

EXPOSURE_RENDER_DLL void BindObject(const ErObject& Object, const bool& Bind /*= true*/)
{
	if (Bind)
		gObjects.Bind(Object);
	else
		gObjects.Unbind(Object);

	gObjectsHashMap = gObjects.HashMap;
}

EXPOSURE_RENDER_DLL void BindClippingObject(const ErClippingObject& ClippingObject, const bool& Bind /*= true*/)
{
	if (Bind)
		gClippingObjects.Bind(ClippingObject);
	else
		gClippingObjects.Unbind(ClippingObject);

	gClippingObjectsHashMap = gClippingObjects.HashMap;
}

EXPOSURE_RENDER_DLL void BindTexture(const ErTexture& Texture, const bool& Bind /*= true*/)
{
	if (Bind)
		gTextures.Bind(Texture);
	else
		gTextures.Unbind(Texture);

	gTexturesHashMap = gTextures.HashMap;
}

EXPOSURE_RENDER_DLL void BindBitmap(const ErBitmap& Bitmap, const bool& Bind /*= true*/)
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

	const float DensityScale		= Tracer.VolumeProperty.DensityScale;
	const float StepFactorPrimary	= gVolumes[gVolumesHashMap[Tracer.VolumeIDs[0]]].MinStep * Tracer.VolumeProperty.StepFactorPrimary;
	const float StepFactorShadow	= gVolumes[gVolumesHashMap[Tracer.VolumeIDs[0]]].MinStep * Tracer.VolumeProperty.StepFactorShadow;
	
	Cuda::HostToConstantDevice(&DensityScale, "gDensityScale");
	Cuda::HostToConstantDevice(&StepFactorPrimary, "gStepFactorPrimary");
	Cuda::HostToConstantDevice(&StepFactorShadow, "gStepFactorShadow");

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

	gTracers.Synchronize(TracerID);

	if (Tracer.VolumeIDs[0] >= 0)
		gVolumes[Tracer.VolumeIDs[0]].Voxels.Bind(TexVolume0);

	SingleScattering(Tracer, Statistics);
	GaussianFilterFrameEstimate(Tracer, Statistics);
	ComputeEstimate(Tracer, Statistics);
	ToneMap(Tracer, Statistics);
	GaussianFilterRunningEstimate(Tracer, Statistics);
	
	if (Tracer.NoiseReduction)
		BilateralFilterRunningEstimate(Tracer, Statistics);
	
	Composite(Tracer, Statistics);

	Tracer.NoEstimates++;

	Cuda::HandleCudaError(cudaEventRecord(EventStop, 0));
	Cuda::HandleCudaError(cudaEventSynchronize(EventStop));
																							
	float TimeDelta = 0.0f;
																							
	Cuda::HandleCudaError(cudaEventElapsedTime(&TimeDelta, EventStart, EventStop), "cudaEventElapsedTime");
	
	Statistics.FPS = 1000.0f / TimeDelta;
														
	Cuda::HandleCudaError(cudaEventDestroy(EventStart));
	Cuda::HandleCudaError(cudaEventDestroy(EventStop));										

}

EXPOSURE_RENDER_DLL void GetDisplayEstimate(int TracerID, ColorRGBAuc* pData)
{
	FrameBuffer& FB = gTracers[TracerID].FrameBuffer;

	Cuda::MemCopyDeviceToHost(FB.DisplayEstimate.GetData(), (ColorRGBAuc*)pData, FB.DisplayEstimate.GetNoElements());
}

}
