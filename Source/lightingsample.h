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

#include "shadersample.h"
#include "lightsample.h"

namespace ExposureRender
{

/*! Lighting sample class */
class LightingSample
{
public:
	/*! Default constructor */
	HOST_DEVICE LightingSample(void) :
		ShaderSample(),
		LightSample(),
		LightNum(0.0f)
	{
	}
	
	/*! Constructor
		@param[in,out] RNG Random number generator
	*/
	HOST_DEVICE LightingSample(RNG& RNG)
	{
		this->LargeStep(RNG);
	}
	
	/*! Assignment operator
		@param[in] Other Lighting sample to copy
		@return Lighting sample
	*/
	HOST_DEVICE LightingSample& LightingSample::operator=(const LightingSample& Other)
	{
		this->ShaderSample	= Other.ShaderSample;
		this->LightSample	= Other.LightSample;
		this->LightNum		= Other.LightNum;
		
		return *this;
	}
	
	/*! Large step mutation
		@param[in,out] RNG Random number generator
	*/
	HOST_DEVICE void LargeStep(RNG& RNG)
	{
		this->ShaderSample.LargeStep(RNG);
		this->LightSample.LargeStep(RNG);
		this->LightNum = RNG.Get1();
	}
	
	/*! Mutation
		@param[in,out] RNG Random number generator
	*/
	HOST_DEVICE void Mutate(RNG& RNG)
	{
		this->ShaderSample.Mutate(RNG);
		this->LightSample.Mutate(RNG);
		Mutate1(this->LightNum, RNG);
	}

	GET_MACRO(HOST_DEVICE, ShaderSample, ShaderSample)
	GET_MACRO(HOST_DEVICE, LightSample, LightSample)
	GET_MACRO(HOST_DEVICE, LightNum, float)

protected:
	ShaderSample	ShaderSample;		/*! Shader sample */
	LightSample 	LightSample;		/*! Light sample */
	float			LightNum;			/*! Light number */
};

}
