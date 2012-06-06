/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or witDEVut modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software witDEVut specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT DEVLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT DEVLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) DEVWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "ertracer.h"
#include "framebuffer.h"
#include "cudatexture1d.h"

#include <map>

using namespace std;

namespace ExposureRender
{

class Tracer
{
public:
	HOST Tracer() :
		VolumeProperty(),
		Camera(),
		VolumeIDs(),
		LightIDs(),
		ObjectIDs(),
		ClippingObjectIDs(),
		FrameBuffer(),
		NoEstimates(0)
	{
	}

	HOST Tracer(const ErTracer& Other) :
		VolumeProperty(),
		Camera(),
		VolumeIDs(),
		LightIDs(),
		ObjectIDs(),
		ClippingObjectIDs(),
		FrameBuffer(),
		NoEstimates(0)
	{
		*this = Other;
	}

	HOST Tracer& Tracer::operator = (const ErTracer& Other)
	{
		this->VolumeProperty	= Other.VolumeProperty;
		this->Camera			= Other.Camera;

		this->VolumeIDs.Count = 0;

		for (int i = 0; i < Other.VolumeIDs.Count; i++)
		{
			if (gVolumesHashMap.find(Other.VolumeIDs[i]) != gVolumesHashMap.end())
				this->VolumeIDs[this->VolumeIDs.Count++] = gVolumesHashMap[Other.VolumeIDs[i]];
			else
				throw(Exception(Enums::Fatal, "Volume not found!"));
		}
		
		this->LightIDs.Count = 0;

		for (int i = 0; i < Other.LightIDs.Count; i++)
		{
			if (gLightsHashMap.find(Other.LightIDs[i]) != gLightsHashMap.end())
				this->LightIDs[this->LightIDs.Count++] = gLightsHashMap[Other.LightIDs[i]];
			else
				throw(Exception(Enums::Fatal, "Light not found!"));
		}
		
		this->ObjectIDs.Count = 0;

		for (int i = 0; i < Other.ObjectIDs.Count; i++)
		{
			if (gObjectsHashMap.find(Other.ObjectIDs[i]) != gObjectsHashMap.end())
				this->ObjectIDs[this->ObjectIDs.Count++] = gObjectsHashMap[Other.ObjectIDs[i]];
			else
				throw(Exception(Enums::Fatal, "Object not found!"));
		}

		this->ClippingObjectIDs.Count = 0;

		for (int i = 0; i < Other.ClippingObjectIDs.Count; i++)
		{
			if (gClippingObjectsHashMap.find(Other.ClippingObjectIDs[i]) != gClippingObjectsHashMap.end())
				this->ClippingObjectIDs[this->ClippingObjectIDs.Count++] = gClippingObjectsHashMap[Other.ClippingObjectIDs[i]];
			else
				throw(Exception(Enums::Fatal, "Clipping object not found!"));
		}

		this->FrameBuffer.Resize(Other.Camera.FilmSize);

		if (Other.GetDirty())
		{
			this->NoEstimates = 0;

			this->FrameBuffer.RandomSeedsCopy1.Modified();
			this->FrameBuffer.RandomSeedsCopy2.Modified();

			this->FrameBuffer.RandomSeeds1 = this->FrameBuffer.RandomSeedsCopy1;
			this->FrameBuffer.RandomSeeds2 = this->FrameBuffer.RandomSeedsCopy2;
		}

		return *this;
	}

	VolumeProperty				VolumeProperty;
	CudaTexture1D<ColorRGBAuc>	TexDiffuse1D;
	CudaTexture1D<ColorRGBAuc>	TexSpecular1D;
	CudaTexture1D<ColorRGBAuc>	TexEmission1D;
	Camera						Camera;
	Indices						VolumeIDs;
	Indices						LightIDs;
	Indices						ObjectIDs;
	Indices						ClippingObjectIDs;
	FrameBuffer					FrameBuffer;
	int							NoEstimates;
};

}
