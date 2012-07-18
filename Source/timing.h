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

#include "defines.h"
#include "enums.h"

#include <vector>

using namespace std;

namespace ExposureRender
{

/*! Timing class */
class EXPOSURE_RENDER_DLL Statistic
{
public:
	/*! Default constructor */
	HOST Statistic() :
		Duration(0.0f),
		NoDurations(0)
	{
		sprintf_s(this->Name, MAX_CHAR_SIZE, "Untitled");

		this->Reset();
	}

	/*! Constructor
		@param[in] Name Timing name
		@param[in] Duration Duration of the event
	*/
	HOST Statistic(const char* Name, const float& Duration) :
		Duration(0.0f),
		NoDurations(0)
	{
		sprintf_s(this->Name, MAX_CHAR_SIZE, Name);

		this->Reset();
		this->AddDuration(Duration);
	}
	
	/*! Copy constructor
		@param[in] Other Timing to copy
	*/
	HOST Statistic(const Statistic& Other) :
		Duration(0.0f),
		NoDurations(0)
	{
		*this = Other;
	}
	
	/*! Assignment operator
		@param[in] Other Timing to copy
		@return Copied timing
	*/
	HOST Statistic& operator = (const Statistic& Other)
	{
		sprintf_s(this->Name, MAX_CHAR_SIZE, Other.Name);
		
		this->Duration = Other.Duration;
		
		for (int i = 0; i < MAX_NO_TIMINGS; i++)
			this->AddDuration(Other.Durations[i]);
		
		this->NoDurations = Other.NoDurations;

		return *this;
	}
	
	/*! Adds a duration
		@param[in] Duration Duration of the event
	*/
	HOST void AddDuration(const float& Duration)
	{
		if (this->NoDurations > 1)
		{
			for (int i = this->NoDurations - 1; i > 0; i--)
				Durations[i] = Durations[i - 1];
		}

		Durations[0] = Duration;
		
		this->NoDurations++;
		
		this->NoDurations = Clamp(this->NoDurations, 0, MAX_NO_TIMING_SAMPLES - 1);

		float Sum = 0.0f;

		for (int i = 0; i < this->NoDurations; i++)
			Sum += this->Durations[i];
		
		this->Duration = Sum / (float)this->NoDurations;
	}
	
	/*! Resets the timing */
	HOST void Reset()
	{
		this->Duration = 0.0f;

		for (int i = 0; i < MAX_NO_TIMINGS; i++)
			this->Durations[i] = 0.0f;

		this->NoDurations = 0;
	}

	/*! Get the name of the timing
		@return Name
	*/
	HOST const char* GetName() const
	{
		return this->Name;
	}

	GET_MACRO(HOST, Duration, float)

protected:
	char	Name[MAX_CHAR_SIZE];					/*! Name */
	float	Duration;								/*! Smoothed duration */
	float	Durations[MAX_NO_TIMING_SAMPLES];		/*! Durations */
	int		NoDurations;							/*! Number of durations */
};

}
