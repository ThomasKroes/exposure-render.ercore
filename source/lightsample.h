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

#include "mutate.h"

namespace ExposureRender
{

/*! Light sample class */
class LightSample
{
public:
	/*! Default constructor */
	HOST_DEVICE LightSample(void) :
		SurfaceUVW(0.0f)
	{
	}
	
	/*! Constructor
		@param[in,out] RNG Random number generator
	*/
	HOST_DEVICE LightSample(RNG& RNG)
	{
		this->LargeStep(RNG);
	}

	/*! Assignment operator
		@param[in] Other Light sample to copy
		@return Light sample
	*/
	HOST_DEVICE LightSample& LightSample::operator=(const LightSample& Other)
	{
		this->SurfaceUVW = Other.SurfaceUVW;

		return *this;
	}

	/*! Large step mutation
		@param[in,out] RNG Random number generator
	*/
	HOST_DEVICE void LargeStep(RNG& RNG)
	{
		this->SurfaceUVW = RNG.Get3();
	}
	
	/*! Mutation
		@param[in,out] RNG Random number generator
	*/
	HOST_DEVICE void Mutate(RNG& RNG)
	{
		Mutate3(this->SurfaceUVW, RNG);
	}

	GET_MACRO(HOST_DEVICE, SurfaceUVW, Vec3f)

protected:
	Vec3f 	SurfaceUVW;		/*! Surface UVW random */
};

}
