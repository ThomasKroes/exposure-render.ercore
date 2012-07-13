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

#pragma once

#include "samplecamera.cuh"
#include "samplelight.cuh"
#include "sampleshader.cuh"
#include "dvr.cuh"
#include "filtering.cuh"
#include "estimate.cuh"
#include "tonemap.cuh"
#include "blendrgbauc.cuh"
#include "gaussianfilterrgbauc.cuh"
#include "gaussianfilterxyzaf.cuh"

#include <thrust/remove.h>

#define SAMPLE_LIGHT
#define SAMPLE_SHADER

namespace ExposureRender
{

struct IsInvalid
{
	HOST_DEVICE bool operator()(const int& Value)
	{
		return Value < 0;
	}
};

void RemoveRedundantSamples(Tracer& Tracer, int& NoSamples)
{
	thrust::device_ptr<int> DevicePtr(Tracer.FrameBuffer.IDs.GetData()); 
	thrust::device_ptr<int> DevicePtrEnd = thrust::remove_if(DevicePtr, DevicePtr + Tracer.FrameBuffer.IDs.GetNoElements(), IsInvalid());

	NoSamples = DevicePtrEnd - DevicePtr;
}

void Render(Tracer& Tracer, Statistics& Statistics)
{
	Tracer.FrameBuffer.DisplayEstimate.Reset();

	switch (Tracer.RenderMode)
	{
		case Enums::StandardRayCasting:
		{
			if (Tracer.NoEstimates > 0)
				return;
			
			Dvr(Tracer, Statistics);
			GaussianFilterRGBAuc(Statistics, 1, Tracer.FrameBuffer.DVR);
			BlendRGBAuc(Statistics, Tracer.FrameBuffer.DisplayEstimate, Tracer.FrameBuffer.DVR);

			break;
		}
		
		case Enums::StochasticRayCasting:
		{
			SampleCamera(Tracer, Statistics);

			int NoSamples = 0;

			RemoveRedundantSamples(Tracer, NoSamples);

			for (int i = 0; i < 1; i++)
			{
				if (NoSamples > 0)
				{
#ifdef SAMPLE_LIGHT
					SampleLight(Tracer, Statistics, i + 1, NoSamples);
#endif

#ifdef SAMPLE_SHADER
					SampleShader(Tracer, Statistics, i + 1, NoSamples);
#endif
				}
			}

			GaussianFilterXYZAf(Statistics, 1, Tracer.FrameBuffer.FrameEstimate);
			ComputeEstimate(Tracer, Statistics);
			ToneMap(Tracer, Statistics);
			GaussianFilterRGBAuc(Statistics, 1, Tracer.FrameBuffer.RunningEstimateRGB);
			BlendRGBAuc(Statistics, Tracer.FrameBuffer.DisplayEstimate, Tracer.FrameBuffer.RunningEstimateRGB);

			break;
		}
	}
}

}
