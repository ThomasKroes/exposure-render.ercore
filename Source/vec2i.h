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

#include "vec.h"

namespace ExposureRender
{											

class EXPOSURE_RENDER_DLL Vec2i : public Vec<int, 2>
{
public:
	/*! Constructor with initializing values */
	HOST_DEVICE Vec2i(const int& V1, const int& V2)
	{
		this->D[0] = V1;
		this->D[1] = V2;
	}

	/*! Copy constructor */
	HOST_DEVICE Vec2i(const Vec<int, 2>& Other)
	{
		for (int i = 0; i < 2; ++i)
			this->D[i] = Other[i];
	}
};

static inline HOST_DEVICE Vec2i operator * (const Vec2i& V, const int& I)					{ return Vec2i(V[0] * I, V[1] * I);											};
static inline HOST_DEVICE Vec2i operator * (const int& I, const Vec2i& V)					{ return Vec2i(V[0] * I, V[1] * I);											};

}
