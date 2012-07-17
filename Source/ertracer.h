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

#include "erbindable.h"
#include "camera.h"
#include "volumeproperty.h"
#include "rendersettings.h"

#include <map>

using namespace std;

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL ErTracer : public ErBindable
{
public:
	HOST ErTracer() :
		ErBindable(),
		RenderMode(Enums::StochasticRayCasting),
		VolumeProperty(),
		Camera(),
		VolumeIDs(),
		LightIDs(),
		ObjectIDs(),
		ClippingObjectIDs(),
		NoiseReduction(true)
	{
	}

	HOST ErTracer(const ErTracer& Other) :
		ErBindable(),
		RenderMode(Enums::StochasticRayCasting),
		VolumeProperty(),
		Camera(),
		VolumeIDs(),
		LightIDs(),
		ObjectIDs(),
		ClippingObjectIDs(),
		NoiseReduction(true)
	{
		*this = Other;
	}

	HOST ErTracer& ErTracer::operator = (const ErTracer& Other)
	{
		ErBindable::operator = (Other);

		this->RenderMode			= Other.RenderMode;
		this->VolumeProperty		= Other.VolumeProperty;
		this->Camera				= Other.Camera;
		this->VolumeIDs				= Other.VolumeIDs;
		this->LightIDs				= Other.LightIDs;
		this->ObjectIDs				= Other.ObjectIDs;
		this->ClippingObjectIDs		= Other.ClippingObjectIDs;
		this->NoiseReduction		= Other.NoiseReduction;

		return *this;
	}

	Enums::RenderMode	RenderMode;
	VolumeProperty		VolumeProperty;
	Camera				Camera;
	Indices<64>			VolumeIDs;
	Indices<64>			LightIDs;
	Indices<64>			ObjectIDs;
	Indices<64>			ClippingObjectIDs;
	bool				NoiseReduction;
};

}
