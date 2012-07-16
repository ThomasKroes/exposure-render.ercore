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

/*! Procedural class */
	class EXPOSURE_RENDER_DLL Procedural : public TimeStamp
{
public:
	/*! Default constructor */
	HOST_DEVICE Procedural() :
		TimeStamp(),
		Type(Enums::Uniform),
		UniformColor(0.5f),
		CheckerColor1(0.5f),
		CheckerColor2(0.5f),
		Gradient()
	{
	}
	
	/*! Destructor */
	HOST_DEVICE virtual ~Procedural()
	{
	}

	/*! Copy constructor
		@param[in] Other Procedural to copy
	*/
	HOST_DEVICE Procedural(const Procedural& Other)
	{
		*this = Other;
	}
	
	/*! Assignment operator
		@param[in] Other Procedural to copy
		@result Procedural
	*/
	HOST_DEVICE Procedural& operator = (const Procedural& Other)
	{
		TimeStamp::operator = (Other);

		this->Type				= Other.Type;
		this->UniformColor		= Other.UniformColor;
		this->CheckerColor1		= Other.CheckerColor1;
		this->CheckerColor2		= Other.CheckerColor2;
		this->Gradient			= Other.Gradient;

		return *this;
	}
	
	/*! Evaluates the procedural at \a UV
		@param[in] UVW Texture coordinates to evaluate at
		@result Color
	*/
	HOST_DEVICE ColorXYZf Evaluate(const Vec2f& UV) const
	{
		switch (this->Type)
		{
			case Enums::Uniform:
			{
				return this->UniformColor;
			}

			case Enums::Checker:
			{
				const int CheckerUV[2] =
				{
					(int)(UV[0] * 2.0f),
					(int)(UV[1] * 2.0f)
				};

				if (CheckerUV[0] % 2 == 0)
				{
					if (CheckerUV[1] % 2 == 0)
						return this->CheckerColor1;
					else
						return this->CheckerColor2;
				}
				else
				{
					if (CheckerUV[1] % 2 == 0)
						return this->CheckerColor2;
					else
						return this->CheckerColor1;
				}
			}

			case Enums::Gradient:
			{
				return this->Gradient.Evaluate(UV[1]);
			}
		}

		return ColorXYZf::Black();
	}
	
	GET_SET_TS_MACRO(HOST_DEVICE, Type, Enums::ProceduralType)
	GET_SET_TS_MACRO(HOST_DEVICE, UniformColor, ColorXYZf)
	GET_SET_TS_MACRO(HOST_DEVICE, CheckerColor1, ColorXYZf)
	GET_SET_TS_MACRO(HOST_DEVICE, CheckerColor2, ColorXYZf)
	GET_REF_SET_TS_MACRO(HOST_DEVICE, Gradient, ColorTransferFunction1D)

protected:
	Enums::ProceduralType		Type;				/*! Type of procedural */
	ColorXYZf					UniformColor;		/*! Uniform color */
	ColorXYZf					CheckerColor1;		/*! First checker color */
	ColorXYZf					CheckerColor2;		/*! Second checker color */
	ColorTransferFunction1D		Gradient;			/*! Gradient */
};

}
