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

#include "hosttracer.h"
#include "framebuffer.h"
#include "buffer3d.h"
#include "gaussian.h"

#include <map>

using namespace std;

namespace ExposureRender
{

/*! Tracer class */
class Tracer : public TimeStamp
{
public:
	/*! Default constructor */
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
		GaussianFilterTables()
	{
	}
	
	/*! Copy constructor
		@param[in] Other Tracer to copy
	*/
	HOST Tracer(const HostTracer& Other) :
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
		GaussianFilterTables()
	{
		*this = Other;
	}
	
	/*! Assignment operator
		@param[in] Other Tracer to copy
		@return Copied tracer
	*/
	HOST Tracer& Tracer::operator = (const HostTracer& Other)
	{
		TimeStamp::operator = (Other);

		this->RenderMode		= Other.GetRenderMode();
		this->Camera			= Other.GetCamera();

		this->VolumeIDs.Reset();

		for (int i = 0; i < Other.GetVolumeIDs().GetNoIndices(); i++)
		{
			if (gVolumesHashMap.find(Other.GetVolumeIDs()[i]) != gVolumesHashMap.end())
				this->VolumeIDs.Add(gVolumesHashMap[Other.GetVolumeIDs()[i]]);
			else
				throw(Exception(Enums::Fatal, "Volume not found!"));
		}
		
		this->LightIDs.Reset();

		for (int i = 0; i < Other.GetLightIDs().GetNoIndices(); i++)
		{
			if (gObjectsHashMap.find(Other.GetLightIDs()[i]) != gObjectsHashMap.end())
				this->LightIDs.Add(gObjectsHashMap[Other.GetLightIDs()[i]]);
			else
				throw(Exception(Enums::Fatal, "Emitter object not found!"));
		}
		
		this->ObjectIDs.Reset();

		for (int i = 0; i < Other.GetObjectIDs().GetNoIndices(); i++)
		{
			if (gObjectsHashMap.find(Other.GetObjectIDs()[i]) != gObjectsHashMap.end())
				this->ObjectIDs.Add(gObjectsHashMap[Other.GetObjectIDs()[i]]);
			else
				throw(Exception(Enums::Fatal, "Object not found!"));
		}

		this->ClippingObjectIDs.Reset();

		for (int i = 0; i < Other.GetClippingObjectIDs().GetNoIndices(); i++)
		{
			if (gClippingObjectsHashMap.find(Other.GetClippingObjectIDs()[i]) != gClippingObjectsHashMap.end())
				this->ClippingObjectIDs.Add(gClippingObjectsHashMap[Other.GetClippingObjectIDs()[i]]);
			else
				throw(Exception(Enums::Fatal, "Clipping object not found!"));
		}

		if (this->FrameBuffer.Resolution != Other.GetCamera().GetFilmSize() || Other.GetDirty())
		{
			this->FrameBuffer.Resize(Other.GetCamera().GetFilmSize());

			this->NoEstimates = 0;
			
//			this->FrameBuffer.RunningEstimateXYZ.Reset();
			
			/*
			this->FrameBuffer.RandomSeedsCopy1.Modified();
			this->FrameBuffer.RandomSeedsCopy2.Modified();
			this->FrameBuffer.RandomSeeds1 = this->FrameBuffer.RandomSeedsCopy1;
			this->FrameBuffer.RandomSeeds2 = this->FrameBuffer.RandomSeedsCopy2;
			*/
		}

		this->NoiseReduction = Other.GetNoiseReduction();
		this->VolumeProperty = Other.GetVolumeProperty();

		return *this;
	}

	Enums::RenderMode			RenderMode;					/*! Type of rendering */
	VolumeProperty				VolumeProperty;				/*! Volume property */
	Camera						Camera;						/*! Camera */
	Indices<64>					VolumeIDs;					/*! Volume IDs */
	Indices<64>					LightIDs;					/*! Light IDs */
	Indices<64>					ObjectIDs;					/*! Object IDs */
	Indices<64>					ClippingObjectIDs;			/*! Clipping object IDs */
	FrameBuffer					FrameBuffer;				/*! Frame buffer */
	int							NoEstimates;				/*! Number of estimates rendered so far */
	bool						NoiseReduction;				/*! Whether noise reduction is on/off */
	GaussianFilterTables		GaussianFilterTables;		/*! Pre-computed Gaussian filter weights */
};

}
