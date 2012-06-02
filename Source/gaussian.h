/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "filter.h"

namespace ExposureRender
{

class GaussianFilter : public Filter
{
public:
	HOST_DEVICE GaussianFilter(const Vec2f& Size = Vec2f(2.0f), const float& Alpha = 2.0f) :
		Filter(Size),
		Alpha(Alpha),
		ExpX(expf(-Alpha * Size[0] * Size[0])),
		ExpY(expf(-Alpha * Size[1] * Size[1]))
	{
	}
	
	HOST_DEVICE float Evaluate(const float& X, const float& Y) const
	{
		return this->Gaussian(X * this->InvSize[0], this->ExpX) * this->Gaussian(Y * this->InvSize[1], this->ExpY);
	}
	
private:
	float Alpha;
	float ExpX;
	float ExpY;
	
	HOST_DEVICE float Gaussian(const float& D, const float& ExpV) const
	{
		return max(0.0f, expf(-this->Alpha * D * D) - ExpV);
	}
};

}
