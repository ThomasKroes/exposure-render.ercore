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

#include "segment.h"

namespace ExposureRender
{

class ClippingSegments
{	
public:
	HOST_DEVICE ClippingSegments()
	{
	}

	HOST_DEVICE ClippingSegments& operator = (const ClippingSegments& Other)
	{
		for (int i = 0; i < MAX_NO_CLIPPING_SEGMENTS; i++)
			this->Segments[i] = Other.Segments[i];

		this->NoSegments = Other.NoSegments;

		return *this;
	}

	HOST_DEVICE bool Inside(const float& T, float& MaxT) const
	{
		bool Inside = false;

		float TempMaxT = 0.0f;

		for (int i = 0; i < MAX_NO_CLIPPING_SEGMENTS; i++)
		{
			if (this->Segments[i].Inside(T, TempMaxT) && TempMaxT > MaxT)
			{
				Inside = true;
				MaxT = TempMaxT;
			}
		}

		return Inside;
	}

	HOST_DEVICE void Add(const Segment& Segment)
	{
		if (this->NoSegments >= MAX_NO_CLIPPING_SEGMENTS)
			return;

		this->Segments[this->NoSegments++] = Segment;

		this->Values[this->NoValues][0]	= Segment.Min;
		this->Values[this->NoValues][1]	= 1.0f;

		this->NoValues++;

		this->Values[this->NoValues][0]	= Segment.Max;
		this->Values[this->NoValues][1]	= 0.0f;

		this->NoValues++;
	}

	HOST_DEVICE void Build()
	{
		Vec2f Values[2 * MAX_NO_CLIPPING_SEGMENTS];
		
		for (int i = 0; i < this->NoValues; i++)
			Values[i] = this->Values[i];

		for (int i = 0; i < this->NoValues; i++)
		{
			float Current = FLT_MAX;
			int ID = 0;

			for (int j = 0; j < this->NoValues; j++)
			{
				if (this->Values[j][0] < Current && this->Values[j][1] != -1)
					ID = j;
			}

			Values[i] = this->Values[ID];
			this->Values[ID][1] = -1;
		}

		int NoValues;

		for (int i = 0; i < this->NoValues; i++)
		{
			
		}
	}

	HOST_DEVICE bool ShouldClip(const float& T)
	{
		/*
		for (int i = 0; i < this->NoSegments; i++)
			if (Segments[i].Inside(T))
				return true;
		*/

		return false;
	}

	Segment		Segments[MAX_NO_CLIPPING_SEGMENTS];
	int			NoSegments;
	Vec2f		Values[2 * MAX_NO_CLIPPING_SEGMENTS];
	int			NoValues;
};

}
