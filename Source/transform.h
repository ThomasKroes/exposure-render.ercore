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

#include "matrix.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL Transform
{
public:
	HOST_DEVICE Transform() :
		TM(),
		InvTM()
	{
	}

	HOST_DEVICE Transform(const Matrix44& M) :
		TM(M),
		InvTM(Matrix44::Inverse(M))
	{
	}

	HOST_DEVICE Transform& operator = (const Transform& Other)
	{
		this->TM	= Other.TM;
		this->InvTM	= Other.InvTM;

		return *this;
	}

	HOST_DEVICE void Set(const Matrix44& M)
	{
		this->TM	= M;
		this->InvTM	= Matrix44::Inverse(this->TM);
	}

	HOST_DEVICE Vec3f GetTranslation() const
	{
		return Vec3f(this->TM.NN[0][3], this->TM.NN[1][3], this->TM.NN[2][3]);
	}

	Matrix44	TM;
	Matrix44	InvTM;
};

}
