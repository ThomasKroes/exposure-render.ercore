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

#include <map>

using namespace std;

namespace ExposureRender
{

class Tracer
{
public:
	HOST Tracer() :
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
		for (int i = 0; i < MAX_NO_VOLUMES; i++)
			this->VolumeProperties[i] = Other.VolumeProperties[i];

		this->Camera				= Other.Camera;
		this->VolumeIDs				= Other.VolumeIDs;
		this->LightIDs				= Other.LightIDs;
		this->ObjectIDs				= Other.ObjectIDs;
		this->ClippingObjectIDs		= Other.ClippingObjectIDs;

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

	HOST void UpdateIDs(Indices SourceIDs, Indices& TargetIDs, map<int, int> HashMap)
	{
		for (int i = 0; i < SourceIDs.Count; i++)
			TargetIDs[i] = HashMap[SourceIDs[i]];

		TargetIDs.Count = SourceIDs.Count;
	}

	HOST void UpdateVolumeIDs(map<int, int> HashMap)
	{
		this->UpdateIDs(this->VolumeIDs, this->VolumeIDs, HashMap);
	}

	HOST void UpdateLightIDs(map<int, int> HashMap)
	{
		this->UpdateIDs(this->LightIDs, this->LightIDs, HashMap);
	}

	HOST void UpdateObjectIDs(map<int, int> HashMap)
	{
		this->UpdateIDs(this->ObjectIDs, this->ObjectIDs, HashMap);
	}

	HOST void UpdateClippingObjectIDs(map<int, int> HashMap)
	{
		this->UpdateIDs(this->ClippingObjectIDs, this->ClippingObjectIDs, HashMap);
	}

	VolumeProperty	VolumeProperties[MAX_NO_VOLUMES];
	Camera			Camera;
	Indices			VolumeIDs;
	Indices			LightIDs;
	Indices			ObjectIDs;
	Indices			ClippingObjectIDs;
	FrameBuffer		FrameBuffer;
	int				NoEstimates;
};

}
