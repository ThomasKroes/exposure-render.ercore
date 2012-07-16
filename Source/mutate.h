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
#include "rng.h"

namespace ExposureRender
{

/*! Mutates a single metropolis value
	@param[in,out] X Value to mutate
	@param[in] RNG Random number generator
	@param[in] S1 Mutation parameter S1
	@param[in] S2 Mutation parameter S2
*/
HOST_DEVICE inline void Mutate1(float& X, RNG& RNG, const float& S1 = 0.0009765625f, const float& S2 = 0.015625f)
{
	float dx = S2 * exp(-log(S2 / S1) * RNG.Get1());

	if (RNG.Get1() < 0.5f)
	{
		float x1 = X + dx;
		X = (x1 > 1) ? x1 - 1 : x1;
	}
	else
	{
		float x1 = X - dx;
		X = (x1 < 0) ? x1 + 1 : x1;
	}
}

/*! Mutates a two-dimensional metropolis vector
	@param[in,out] V Value to mutate
	@param[in] RNG Random number generator
	@param[in] S1 Mutation parameter S1
	@param[in] S2 Mutation parameter S2
*/
HOST_DEVICE inline void Mutate2(Vec2f& V, RNG& RNG, const float& S1 = 0.0009765625f, const float& S2 = 0.015625f)
{
	Mutate1(V[0], RNG, S1, S2);
	Mutate1(V[1], RNG, S1, S2);
}

/*! Mutates a three-dimensional metropolis vector
	@param[in,out] V Value to mutate
	@param[in] RNG Random number generator
	@param[in] S1 Mutation parameter S1
	@param[in] S2 Mutation parameter S2
*/
HOST_DEVICE inline void Mutate3(Vec3f& V, RNG& RNG, const float& S1 = 0.0009765625f, const float& S2 = 0.015625f)
{
	Mutate1(V[0], RNG, S1, S2);
	Mutate1(V[1], RNG, S1, S2);
	Mutate1(V[2], RNG, S1, S2);
}

}
