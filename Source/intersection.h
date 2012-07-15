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

using namespace std;

namespace ExposureRender
{

/*! Intersection class */
class Intersection
{
public:
	/*! Default constructor */
	HOST_DEVICE Intersection() :
		Valid(false),
		Front(true),
		Wo(),
		T(FLT_MAX),
		P(),
		N(),
		UV(),
		Intensity(),
		ScatterType(Enums::Volume),
		ID(-1)
	{
	}
	
	/*! Copy constructor
		@param[in] Other Intersection to copy
	*/
	HOST_DEVICE Intersection(const Intersection& Other) :
		Valid(false),
		Front(true),
		Wo(),
		T(FLT_MAX),
		P(),
		N(),
		UV(),
		Intensity(),
		ScatterType(Enums::Volume),
		ID(-1)
	{
		*this = Other;
	}

	/*! Assignment operator
		@param[in] Other Intersection to copy
		@result Intersection
	*/
	HOST_DEVICE Intersection& Intersection::operator = (const Intersection& Other)
	{
		this->Valid			= Other.Valid;
		this->Front			= Other.Front;
		this->Wo			= Other.Wo;
		this->T				= Other.T;
		this->P				= Other.P;
		this->N				= Other.N;
		this->UV			= Other.UV;
		this->Intensity		= Other.Intensity;
		this->ScatterType	= Other.ScatterType;
		this->ID			= Other.ID;

		return *this;
	}

	GET_REF_SET_MACRO(HOST_DEVICE, Valid, bool)
	GET_REF_SET_MACRO(HOST_DEVICE, Front, bool)
	GET_REF_SET_MACRO(HOST_DEVICE, Wo, Vec3f)
	GET_REF_SET_MACRO(HOST_DEVICE, T, float)
	GET_REF_SET_MACRO(HOST_DEVICE, P, Vec3f)
	GET_REF_SET_MACRO(HOST_DEVICE, N, Vec3f)
	GET_REF_SET_MACRO(HOST_DEVICE, UV, Vec2f)
	GET_REF_SET_MACRO(HOST_DEVICE, Intensity, float)
	GET_REF_SET_MACRO(HOST_DEVICE, ScatterType, Enums::ScatterType)
	GET_REF_SET_MACRO(HOST_DEVICE, ID, int)

protected:
	bool					Valid;			/*! If there is a valid intersection */
	bool					Front;			/*! Front (true) or back (false) */
	Vec3f					Wo;				/*! Outgoing direction */
	float					T;				/*! Distance along ray */
	Vec3f					P;				/*! Position at intersection */
	Vec3f					N;				/*! Normal at intersection */
	Vec2f					UV;				/*! UV coordinates at intersection */
	float					Intensity;		/*! Intensity at intersection */
	Enums::ScatterType		ScatterType;	/*! Scatter type */
	int						ID;				/*! ID of the entity that was hit */
};

}
