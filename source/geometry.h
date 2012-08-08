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

#include "color.h"
#include "ray.h"
#include "matrix.h"

using namespace std;

namespace ExposureRender
{

/*! Transform a vector by a matrix
	@param[in] TM Transformation matrix
	@param[in] V Vector to transform
	@return Transformed vector
*/
HOST_DEVICE inline Vec3f TransformVector(const Matrix44& TM, const Vec3f& V)
{
	Vec3f Vt;

	const float x = V[0], y = V[1], z = V[2];

	Vt[0] = TM.GetElement(0, 0) * x + TM.GetElement(0, 1) * y + TM.GetElement(0, 2) * z;
	Vt[1] = TM.GetElement(1, 0) * x + TM.GetElement(1, 1) * y + TM.GetElement(1, 2) * z;
	Vt[2] = TM.GetElement(2, 0) * x + TM.GetElement(2, 1) * y + TM.GetElement(2, 2) * z;

	return Vt;
}

/*! Transform a point by a matrix
	@param[in] TM Transformation matrix
	@param[in] P Point to transform
	@return Transformed point
*/
HOST_DEVICE inline Vec3f TransformPoint(const Matrix44& TM, const Vec3f& P)
{
	const float x = P[0], y = P[1], z = P[2];
    
	const float Px = TM.GetElement(0, 0) * x + TM.GetElement(0, 1) * y + TM.GetElement(0, 2) * z + TM.GetElement(0, 3);
    const float Py = TM.GetElement(1, 0) * x + TM.GetElement(1, 1) * y + TM.GetElement(1, 2) * z + TM.GetElement(1, 3);
    const float Pz = TM.GetElement(2, 0) * x + TM.GetElement(2, 1) * y + TM.GetElement(2, 2) * z + TM.GetElement(2, 3);
	
	return Vec3f(Px, Py, Pz);
}

/*! Transform a ray by a matrix
	@param[in] TM Transformation matrix
	@param[in] R Ray to transform
	@return Transformed ray
*/
HOST_DEVICE inline Ray TransformRay(const Matrix44& TM, const Ray& R)
{
	Ray Rt;

	Vec3f P		= TransformPoint(TM, R.O);
	Vec3f MinP	= TransformPoint(TM, R(R.MinT));
	Vec3f MaxP	= TransformPoint(TM, R(R.MaxT));

	Rt.O	= P;
	Rt.D	= Normalize(MaxP - Rt.O);
	Rt.MinT	= Length(MinP, Rt.O);
	Rt.MaxT	= Length(MaxP, Rt.O);

	return Rt;
}

/*! Computes the spherical theta of \a W
	@param[in] W Input vector
	@return Spherical theta
*/
HOST_DEVICE inline float SphericalTheta(const Vec3f& W)
{
	return acosf(Clamp(W[1], -1.0f, 1.0f));
}

/*! Computes the spherical phi of \a W
	@param[in] W Input vector
	@return Spherical phi
*/
HOST_DEVICE inline float SphericalPhi(const Vec3f& W)
{
	float p = atan2f(W[2], W[0]);
	return (p < 0.0f) ? p + 2.0f * PI_F : p;
}

/*! Transforms spherical coordinates into normalized UV coordinates
	@param[in] W Input vector
	@return UV coordinates theta
*/
HOST_DEVICE inline Vec2f SphericalToUV(const Vec3f& W)
{
	const Vec3f V = Normalize(W);
	return Vec2f(INV_TWO_PI_F * SphericalPhi(V), 1.0f - (INV_PI_F * SphericalTheta(V)));
}

/*! Linearly interpolate between two values \a A and \a B
	@param[in] LerpC Interpolation coefficient
	@param[in] A Value A
	@param[in] B Value B
	@return Interpolation between \a A and \a B
*/
template<class T>
HOST_DEVICE inline T Lerp(const float& LerpC, const T& A, const T& B)
{
	return (1.0f - LerpC) * A + LerpC * B;
}

/*! Get a random float between 0.0f and 1.0f 
	@return Random float between 0.0f and 1.0f 
*/
inline float RandomFloat()
{
	return (float)rand() / RAND_MAX;
}

}
