/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or witDEVut modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software witDEVut specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT DEVLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT DEVLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) DEVWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "vector.h"

namespace ExposureRender
{

class Segment
{	
public:
	HOST_DEVICE Segment(const float& Min = 0.0f, const float& Max = 0.0f) :
		Min(Min),
		Max(max(Min, Max))
	{
	}

	HOST_DEVICE Segment& operator = (const Segment& Other)
	{
		this->Min	= Other.Min;
		this->Max	= Other.Max;

		return *this;
	}

	HOST_DEVICE void Set(const float& Min, const float& Max)
	{
		this->Min	= Min;
		this->Max	= max(Min, Max);
	}

	HOST_DEVICE bool Inside(const float& T, float& MaxT) const
	{
		if (T >= this->Min && T < this->Max)
		{
			MaxT = this->Max;
			return true;
		}

		return false;
	}

	/*
	HOST_DEVICE bool operator < (const float& Other)
	{
		return Other < this->Min;
	};

	HOST_DEVICE bool operator > (const float& Other)
	{
		return Other > this->Max;
	};

	HOST_DEVICE bool Overlaps(const Segment& Segment, OverlapType& OverlapType)
	{
		if (Segment.Min < this->Min && Segment.Max > this->Max)
		{
			OverlapType = Complete;
			return true;
		}

		if (Segment.Min > this->Min && Segment.Min < this->Max)
		{
			OverlapType = Right;
			return true;
		}

		if (Segment.Max > this->Min && Segment.Max < this->Max)
		{
			OverlapType = Left;
			return true;
		}

		if (Segment.Min == this->Min && Segment.Max == this->Max)
		{
			OverlapType = Exact;
			return true;
		}

		return false;
	}
	*/

	float 	Min;
	float 	Max;
};

}
