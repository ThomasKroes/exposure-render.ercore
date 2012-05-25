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

#include "defines.h"
#include "enums.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL ShadeEvent
{
public:
	HOST ShadeEvent()
	{
	}
	
	HOST ShadeEvent(const ShadeEvent& Other)
	{
		*this = Other;
	}

	HOST ShadeEvent& operator = (const ShadeEvent& Other)
	{
		return *this;
	}

	Vec3f			Ps;					// Position of the scatter event
	Vec3f			Ns;					// Normal at scatter event

	unsigned short	LightID;			// ID of the sampled light
	Vec3f			Pl;					// Light sample point
	ColorXYZf		Le;					// Light emission
	ColorXYZf		LightF;				// Light reflection
	float			MisWeightLight;		// MIS weight for the light

	Vec3f			Wi;					// Ray direction, sampled from BRDF
	ColorXYZf		BrdfF;				// BRDF Reflection
	float			PdfBrdf;			// Probability density for sampling the BRDF
};

}
