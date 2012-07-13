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
#include "transferfunction.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL Procedural
{
public:
	HOST Procedural()
	{
		this->Type = Enums::Uniform;
	}
	
	HOST ~Procedural()
	{
	}

	HOST Procedural(const Procedural& Other)
	{
		*this = Other;
	}

	HOST Procedural& operator = (const Procedural& Other)
	{
		this->Type			= Other.Type;
		this->UniformColor	= Other.UniformColor;
		this->CheckerColor1	= Other.CheckerColor1;
		this->CheckerColor2	= Other.CheckerColor2;
		this->Gradient		= Other.Gradient;

		return *this;
	}

	HOST_DEVICE ColorXYZf Evaluate(const Vec2f& UVW) const
	{
		switch (this->Type)
		{
			case Enums::Uniform:
			{
				return this->UniformColor;
			}

			case Enums::Checker:
			{
				const int UV[2] =
				{
					(int)(UVW[0] * 2.0f),
					(int)(UVW[1] * 2.0f)
				};

				if (UV[0] % 2 == 0)
				{
					if (UV[1] % 2 == 0)
						return this->CheckerColor1;
					else
						return this->CheckerColor2;
				}
				else
				{
					if (UV[1] % 2 == 0)
						return this->CheckerColor2;
					else
						return this->CheckerColor1;
				}
			}

			case Enums::Gradient:
			{
				return this->Gradient.Evaluate(UVW[1]);
			}
		}

		return ColorXYZf::Black();
	}

	Enums::ProceduralType		Type;
	ColorXYZf					UniformColor;
	ColorXYZf					CheckerColor1;
	ColorXYZf					CheckerColor2;
	ColorTransferFunction1D		Gradient;
};

}
