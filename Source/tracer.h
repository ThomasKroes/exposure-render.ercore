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
#include "buffer3d.h"
#include "gaussian.h"

#include <map>

using namespace std;

namespace ExposureRender
{

class Tracer : public TimeStamp
{
public:
	HOST Tracer() :
		TimeStamp(),
		RenderMode(Enums::StochasticRayCasting),
		VolumeProperty(),
		Camera(),
		VolumeIDs(),
		LightIDs(),
		ObjectIDs(),
		ClippingObjectIDs(),
		FrameBuffer(),
		NoEstimates(0),
		NoiseReduction(true),
		EmptySpace("Empty space", Enums::Device),
		GaussianFilterTables()
	{
	}

	HOST Tracer(const ErTracer& Other) :
		TimeStamp(),
		RenderMode(Enums::StochasticRayCasting),
		VolumeProperty(),
		Camera(),
		VolumeIDs(),
		LightIDs(),
		ObjectIDs(),
		ClippingObjectIDs(),
		FrameBuffer(),
		NoEstimates(0),
		NoiseReduction(true),
		EmptySpace("Empty space", Enums::Device),
		GaussianFilterTables()
	{
		*this = Other;
	}

	HOST Tracer& Tracer::operator = (const ErTracer& Other)
	{
		TimeStamp::operator = (Other);

		this->RenderMode		= Other.RenderMode;
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
			if (gObjectsHashMap.find(Other.LightIDs[i]) != gObjectsHashMap.end())
				this->LightIDs[this->LightIDs.Count++] = gObjectsHashMap[Other.LightIDs[i]];
			else
				throw(Exception(Enums::Fatal, "Emitter object not found!"));
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

		if (this->FrameBuffer.Resolution != Other.Camera.FilmSize || Other.GetDirty())
		{
			this->FrameBuffer.Resize(Other.Camera.FilmSize);

			this->NoEstimates = 0;
			
//			this->FrameBuffer.RunningEstimateXYZ.Reset();

			this->FrameBuffer.RandomSeedsCopy1.Modified();
			this->FrameBuffer.RandomSeedsCopy2.Modified();
			this->FrameBuffer.RandomSeeds1 = this->FrameBuffer.RandomSeedsCopy1;
			this->FrameBuffer.RandomSeeds2 = this->FrameBuffer.RandomSeedsCopy2;
		}

		this->NoiseReduction = Other.NoiseReduction;

		this->UpdateEmptySpace = this->VolumeProperty.Opacity1D != Other.VolumeProperty.Opacity1D;

		this->VolumeProperty = Other.VolumeProperty;

		return *this;
	}

	DEVICE float GetOpacity(const unsigned short& Intensity)				{	return this->VolumeProperty.Opacity1D.Evaluate(Intensity);				}
	DEVICE ColorXYZf GetDiffuse(const unsigned short& Intensity)			{	return this->VolumeProperty.Diffuse1D.Evaluate(Intensity);				}
	DEVICE ColorXYZf GetSpecular(const unsigned short& Intensity)			{	return this->VolumeProperty.Specular1D.Evaluate(Intensity);				}
	DEVICE float GetGlossiness(const unsigned short& Intensity)				{	return this->VolumeProperty.Glossiness1D.Evaluate(Intensity);			}
	DEVICE float GetIndexOfReflection(const unsigned short& Intensity)		{	return this->VolumeProperty.IndexOfReflection1D.Evaluate(Intensity);	}
	DEVICE ColorXYZf GetEmission(const unsigned short& Intensity)			{	return this->VolumeProperty.Emission1D.Evaluate(Intensity);				}

	Enums::RenderMode			RenderMode;
	VolumeProperty				VolumeProperty;
	Camera						Camera;
	Indices						VolumeIDs;
	Indices						LightIDs;
	Indices						ObjectIDs;
	Indices						ClippingObjectIDs;
	FrameBuffer					FrameBuffer;
	int							NoEstimates;
	bool						NoiseReduction;
	Buffer3D<unsigned char>		EmptySpace;
	bool						UpdateEmptySpace;
	GaussianFilterTables		GaussianFilterTables;
};

}
