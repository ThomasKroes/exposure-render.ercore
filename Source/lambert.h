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

#include "montecarlo.h"

namespace ExposureRender
{

/*! Lambert class */
class Lambert
{
public:
	/*! Default constructor */
	HOST_DEVICE Lambert(void)
	{
	}
	
	/*! Constructor
		@param[in] Kd Diffuse color
	*/
	HOST_DEVICE Lambert(const ColorXYZf& Kd)
	{
		this->Kd = Kd;
	}
	
	/*! Computes the reflectance given \a Wo and \a Wi
		@param[in] Wo Outgoing direction
		@param[out] Wi Incoming direction
		@return Reflectance
	*/
	HOST_DEVICE ColorXYZf F(const Vec3f& Wo, const Vec3f& Wi)
	{
		return Kd * INV_PI_F;
	}
	
	/*! Samples a random direction with importance sampling
		@param[in] Wo Outgoing direction
		@param[out] Wi Incoming direction
		@param[out] Pdf Probability of sampling \a Wi
		@param[in] U Random sample
		@return Reflectance, given \a Wo and \a Wi
	*/
	HOST_DEVICE ColorXYZf SampleF(const Vec3f& Wo, Vec3f& Wi, float& Pdf, const Vec2f& U)
	{
		Wi = CosineWeightedHemisphere(U);

		if (Wo[2] < 0.0f)
			Wi[2] *= -1.0f;

		Pdf = this->Pdf(Wo, Wi);

		return this->F(Wo, Wi);
	}
	
	/*! Computes the probability giving vector \a Wo and \a Wi
		@param[in] Wo Outgoing direction
		@param[in] Wi Incoming direction
		@return Probability
	*/
	HOST_DEVICE float Pdf(const Vec3f& Wo, const Vec3f& Wi)
	{
		return SameHemisphere(Wo, Wi) ? AbsCosTheta(Wi) * INV_PI_F : 0.0f;
	}
	
	/*! Assignment operator
		@param[in] Other Lambert to copy
		@result Lambert
	*/
	HOST_DEVICE Lambert& operator = (const Lambert& Other)
	{
		this->Kd = Other.Kd;

		return *this;
	}

	ColorXYZf	Kd;		/*! Diffuse color */
};

}
