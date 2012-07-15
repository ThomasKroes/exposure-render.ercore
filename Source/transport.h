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

#include "vector.h"
										
namespace ExposureRender
{

/*! Computes the geometric factor
	@param[in] P1 Point 1
	@param[in] N1 Normal 1
	@param[in] P2 Point 2
	@param[in] N2 Normal 2
	@result Geometric factor
*/
HOST_DEVICE inline float GeometricFactor(const Vec3f& P1, const Vec3f& N1, const Vec3f& P2, const Vec3f& N2)
{
	const Vec3f W = Normalize(P2 - P1);
	return (ClampedDot(W, N1) * ClampedDot(-1.0f * W, N2)) / LengthSquared(P1, P2);
}

/*! Computes MIS weight based on power heuristic
	@param[in] Nf Nf
	@param[in] PdfF Pdf F
	@param[in] Ng Ng
	@param[in] PdfG Pdf G
	@result MIS weight
*/
HOST_DEVICE_NI inline float PowerHeuristic(const int& Nf, const float& PdfF, const int& Ng, const float& PdfG)
{
	const float F = Nf * PdfF;
	const float G = Ng * PdfG;

	return (F * F) / (F * F + G * G); 
}

}
