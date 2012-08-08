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
#include "rng.h"

namespace ExposureRender
{

/*! Cosine of theta
	@param[in] W Input vector
	@return Cosine of theta
*/
HOST_DEVICE_NI inline float CosTheta(const Vec3f& W)
{
	return W[2];
}

/*! Absolute cosine of theta
	@param[in] W Input vector
	@return Absolute Cosine of theta
*/
HOST_DEVICE_NI inline float AbsCosTheta(const Vec3f& W)
{
	return fabsf(CosTheta(W));
}

/*! Sine of theta
	@param[in] W Input vector
	@return Sine of theta
*/
HOST_DEVICE_NI inline float SinTheta(const Vec3f& W)
{
	return sqrtf(max(0.0f, 1.0f - W[2] * W[2]));
}

/*! Sine of theta 2
	@param[in] W Input vector
	@return Sine of theta 2
*/
HOST_DEVICE_NI inline float SinTheta2(const Vec3f& W)
{
	return 1.0f - CosTheta(W) * CosTheta(W);
}

/*! Cosine of phi
	@param[in] W Input vector
	@return Cosine of phi
*/
HOST_DEVICE_NI inline float CosPhi(const Vec3f& W)
{
	return W[0] / SinTheta(W);
}

/*! Sine of phi
	@param[in] W Input vector
	@return Sine of phi
*/
HOST_DEVICE_NI inline float SinPhi(const Vec3f& W)
{
	return W[1] / SinTheta(W);
}

/*! Determine whether \a W1 and \a W2 are in the same hemisphere
	@param[in] W1 Input vector 1
	@param[in] W2 Input vector 2
	@return If \a W1 and \a W2 are in the same hemisphere
*/
HOST_DEVICE_NI inline bool SameHemisphere(const Vec3f& W1, const Vec3f& W2)
{
   return (W1[2] * W2[2]) > 0.0f;
}

/*! Sample a disk
	@param[in] U Input random vector
	@return Sample
*/
HOST_DEVICE_NI Vec2f inline ConcentricSampleDisk(const Vec2f& U)
{
	float r, theta;
	// Map uniform random numbers to $[-1,1]^2$
	float sx = 2 * U[0] - 1;
	float sy = 2 * U[1] - 1;
	// Map square to $(r,\theta)$
	// Handle degeneracy at the origi
	
	if (sx == 0.0 && sy == 0.0)
	{
		return Vec2f(0.0f);
	}

	if (sx >= -sy)
	{
		if (sx > sy)
		{
			// Handle first region of disk
			r = sx;
			if (sy > 0.0)
				theta = sy/r;
			else
				theta = 8.0f + sy/r;
		}
		else
		{
			// Handle second region of disk
			r = sy;
			theta = 2.0f - sx/r;
		}
	}
	else
	{
		if (sx <= sy)
		{
			// Handle third region of disk
			r = -sx;
			theta = 4.0f - sy/r;
		}
		else
		{
			// Handle fourth region of disk
			r = -sy;
			theta = 6.0f + sx/r;
		}
	}
	
	theta *= PI_F / 4.f;

	return Vec2f(r*cosf(theta), r*sinf(theta));
}

/*! Cosine weighted hemisphere
	@param[in] U Input random vector
	@return Sample
*/
HOST_DEVICE_NI inline Vec3f CosineWeightedHemisphere(const Vec2f& U)
{
	const Vec2f ret = ConcentricSampleDisk(U);
	return Vec3f(ret[0], ret[1], sqrtf(max(0.f, 1.f - ret[0] * ret[0] - ret[1] * ret[1])));
}

/*! Generates a spherical direction
	@param[in] SinTheta Sine of theta
	@param[in] CosTheta Cosine of theta
	@param[in] Phi Phi
	@return Vector
*/
HOST_DEVICE_NI inline Vec3f SphericalDirection(const float& SinTheta, const float& CosTheta, const float& Phi)
{
	return Vec3f(SinTheta * cosf(Phi), SinTheta * sinf(Phi), CosTheta);
}

/*! Uniformly sample the upper hemisphere
	@param[in] U Input random vector
	@return Random vector in upper hemisphere
*/
HOST_DEVICE_NI inline Vec3f UniformSampleHemisphere(const Vec2f& U)
{
	float z = U[0];
	float r = sqrtf(max(0.f, 1.f - z*z));
	float phi = 2 * PI_F * U[1];
	float x = r * cosf(phi);
	float y = r * sinf(phi);
	return Vec3f(x, y, z);
}

/*! Uniformly sample a unit sphere
	@param[in] U Input random vector
	@return Random vector in unit sphere
*/
HOST_DEVICE_NI inline Vec3f UniformSampleSphere(const Vec2f& U)
{
	const float Z	= 1.0f - 2.0f * U[0];
	const float R	= sqrtf(max(0.0f, 1.0f - Z * Z));
	const float Phi	= 2.0f * PI_F * U[1];
	const float X	= R * cosf(Phi);
	const float Y	= R * sinf(Phi);

	return Vec3f(X, Y, Z);
}

}
