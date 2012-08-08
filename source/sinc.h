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

namespace ExposureRender
{

/*! Lanczos-Sinc filter */
class LanczosSincFilter : public Filter
{
public:
	/*! Constructor
		@param Size Size of the filter
		@param Tau Filter setting
	*/
	HOST_DEVICE LanczosSincFilter(const Vec2f& Size = Vec2f(4.0f), const float& Tau = 3.0f) :
		Filter(Size),
		Tau(Tau)
	{
	}
	
	/*! Evaluates the Lanczos-Sinc filter
		@param[in] X X position
		@param[in] Y Y position
		@return Lanczos-Sinc filter weight
	*/
	HOST_DEVICE float Evaluate(const float& X, const float& Y) const
	{
		return Sinc1D(X * this->InvSize[0]) * Sinc1D(Y * this->InvSize[1]);
	}

protected:
	/*! Evaluates the one-dimensional Sinc filter
		@param[in] X Position
		@return Sinc filter weight
	*/
	HOST_DEVICE float Sinc1D(const float& X) const
	{
		float x = fabsf(X);

		if (x < 1e-5)
			return 1.0f;
		if (x > 1.0)
			return 0.0f;

		x *= PI_F;

		const float Sinc		= sinf(x * this->Tau) / (x * this->Tau);
		const float Lanczos		= sinf(x) / x;

		return Sinc * Lanczos;
	}

	float Tau;		/*! Tau filter setting */
};

}
