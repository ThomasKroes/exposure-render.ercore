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

#include "vec.h"

namespace ExposureRender
{

/*! \class Vec4f
 * \brief Four dimensional float vector
 */
class EXPOSURE_RENDER_DLL Vec4f : public Vec<float, 4>
{
public:
	/*! Default constructor */
	HOST_DEVICE Vec4f()
	{
		for (int i = 0; i < 4; ++i)
			this->D[i] = 0.0f;
	}

	/*! Constructor with initializing values */
	HOST_DEVICE Vec4f(const float& V1, const float& V2, const float& V3, const float& V4)
	{
		this->D[0] = V1;
		this->D[1] = V2;
		this->D[2] = V3;
		this->D[3] = V4;
	}
	
	/*! Copy constructor */
	HOST_DEVICE Vec4f(const Vec4f& Other)
	{
		for (int i = 0; i < 4; ++i)
			this->D[i] = Other[i];
	}

	/*! Copy constructor */
	HOST_DEVICE Vec4f(const Vec<float, 4>& Other)
	{
		for (int i = 0; i < 4; ++i)
			this->D[i] = Other[i];
	}

	/*! Negate operator
		* \return Negated vector by value
	*/
	HOST_DEVICE Vec4f operator - () const
	{
		Vec4f Result;
		
		for (int i = 0; i < 4; ++i)
			Result[i] = -this->D[i];
		
		return Result;
	}
};

}
