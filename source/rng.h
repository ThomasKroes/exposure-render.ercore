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

#include "geometry.h"

namespace ExposureRender
{

/*! Random number generator class */
class RNG
{
public:
	/*! Constructor
		@param[in] Seed0 First seed value
		@param[in] Seed1 Second seed value
	*/
	HOST_DEVICE RNG(unsigned int* Seed0, unsigned int* Seed1)
	{
		this->Seed0 = Seed0;
		this->Seed1 = Seed1;
	}

	/*! Gets a single random float
		@return Random float
	*/
	HOST_DEVICE float Get1()
	{
		*this->Seed0 = 36969 * ((*Seed0) & 65535) + ((*Seed0) >> 16);
		*this->Seed1 = 18000 * ((*Seed1) & 65535) + ((*Seed1) >> 16);

		unsigned int ires = ((*Seed0) << 16) + (*Seed1);

		union
		{
			float f;
			unsigned int ui;
		} res;

		res.ui = (ires & 0x007fffff) | 0x40000000;

		return (res.f - 2.f) / 2.f;
	}

	/*! Gets a two-dimensional random vector
		@return Two-dimensional random vector
	*/
	HOST_DEVICE Vec2f Get2()
	{
		return Vec2f(Get1(), Get1());
	}
	
	/*! Gets a three-dimensional random vector
		@return Three-dimensional random vector
	*/
	HOST_DEVICE Vec3f Get3(void)
	{
		return Vec3f(Get1(), Get1(), Get1());
	}

private:
	unsigned int*	Seed0;		/*! First random seed value */
	unsigned int*	Seed1;		/*! Second random seed value */
};

}