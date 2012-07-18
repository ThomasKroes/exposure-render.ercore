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

#include "vec.h"

namespace ExposureRender
{

/*! Hysteresis class */
template<class T, int NoSamples>
class EXPOSURE_RENDER_DLL Hysteresis : public Vec<T, NoSamples>
{
public:
	/*! Default constructor */
	HOST_DEVICE Hysteresis() :
		Vec<T, NoSamples>()
	{
	}

	/*! Copy constructor */
	HOST_DEVICE Hysteresis(const Hysteresis& Other)
	{
		*this = Other;
	}
	
	/*! Assignment operator
		@param[in] Other Host Hysteresis to copy
		@return Copied hysteresis
	*/
	HOST Hysteresis& operator = (const Hysteresis& Other)
	{
		Vec<T, NoSamples>::operator = (Other);

		this->FilteredValue	= Other.FilteredValue;
		this->NoValues		= Other.NoValues;

		return *this;
	}

	/*! Adds a value and computes the new filtered value
		@param[in] Value Value to add
	*/
	HOST void AddValue(const T& Value)
	{
		if (this->NoValues > 1)
		{
			for (int i = this->NoValues - 1; i > 0; i--)
				D[i] = D[i - 1];
		}

		D[0] = Value;
		
		this->NoValues++;
		
		this->NoValues = ExposureRender::Clamp(this->NoValues, 0, NoSamples);

		T Sum = 0.0f;

		for (int i = 0; i < this->NoValues; i++)
			Sum += this->D[i];
		
		this->FilteredValue = Sum / (float)this->NoValues;
	}
	
	/*! Resets the hysteresis */
	HOST void Reset()
	{
		this->FilteredValue = T();

		for (int i = 0; i < NoSamples; i++)
			this->D[i] = T();

		this->NoValues = 0;
	}

	GET_MACRO(HOST_DEVICE, FilteredValue, T);

protected:
	T		FilteredValue;			/*! Filtered value */
	int		NoValues;				/*! Number of values */
};

}
