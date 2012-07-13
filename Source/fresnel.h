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

namespace ExposureRender
{

class Fresnel
{
public:
	HOST_DEVICE Fresnel(void)
	{
	}

	HOST_DEVICE Fresnel(const float& EtaI, const float& EtaT) :
		EtaI(EtaI),
		EtaT(EtaT)
	{
	}

	HOST_DEVICE ColorXYZf Evaluate(float CosI)
	{
		CosI = Clamp(CosI, -1.0f, 1.0f);

		const bool Entering = CosI > 0.0f;

		float EtaI = this->EtaI, EtaT = this->EtaT;

		if (!Entering)
			Swap(EtaI, EtaT);

		const float SinT = EtaI / EtaT * sqrtf(max(0.0f, 1.0f - CosI * CosI));

		if (SinT >= 1.0f)
		{
			return 1.0f;
		}
		else
		{
			const float CosT = sqrtf(max(0.0f, 1.0f - SinT * SinT));
			return FresnelDielectric(fabsf(CosI), CosT, EtaI, EtaT);
		}
	}

	HOST_DEVICE ColorXYZf FresnelDielectric(const float& CosI, const float& CosT, const float& EtaI, const float& EtaT)
	{
		const ColorXYZf Rparl = ((EtaT * CosI) - (EtaI * CosT)) / ((EtaT * CosI) + (EtaI * CosT));
		const ColorXYZf Rperp = ((EtaI * CosI) - (EtaT * CosT)) / ((EtaI * CosI) + (EtaT * CosT));
		return 0.5f * (Rparl * Rparl + Rperp * Rperp);
	}

	HOST_DEVICE Fresnel& operator = (const Fresnel& Other)
	{
		this->EtaI = Other.EtaI;
		this->EtaT = Other.EtaT;

		return *this;
	}

	float	EtaI;
	float	EtaT;
};

}
