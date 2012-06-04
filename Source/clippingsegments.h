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
	HOST_DEVICE ClippingSegments() :
		NoSegments(0),
		NoSegmentNodes(0)
	{
	}

	HOST_DEVICE ClippingSegments& operator = (const ClippingSegments& Other)
	{
		for (int i = 0; i < MAX_NO_CLIPPING_SEGMENTS; i++)
			this->Segments[i] = Other.Segments[i];

		this->NoSegments = Other.NoSegments;

		return *this;
	}

	HOST_DEVICE void Add(const Segment& Segment)
	{
		if (this->NoSegments >= MAX_NO_CLIPPING_SEGMENTS)
			return;

		this->Segments[this->NoSegments] = Segment;

		this->NoSegments++;

		this->SegmentNodes[this->NoSegmentNodes][0]	= Segment.Min;
		this->SegmentNodes[this->NoSegmentNodes][1]	= 1.0f;

		this->NoSegmentNodes++;

		this->SegmentNodes[this->NoSegmentNodes][0]	= Segment.Max;
		this->SegmentNodes[this->NoSegmentNodes][1]	= 0.0f;

		this->NoSegmentNodes++;
	}

	HOST_DEVICE void Build()
	{
		if (this->NoSegmentNodes <= 0)
			return;

		Vec2f TempSegmentNodes[2 * MAX_NO_CLIPPING_SEGMENTS];
		
		for (int i = 0; i < this->NoSegments; i++)
			TempSegmentNodes[i] = this->SegmentNodes[i];

		for (int i = 0; i < this->NoSegmentNodes; i++)
		{
			float Current = FLT_MAX;

			int ID = 0;

			for (int j = 0; j < this->NoSegmentNodes; j++)
			{
				if (this->SegmentNodes[j][0] < Current && this->SegmentNodes[j][1] != -1)
				{
					ID = j;
					Current = this->SegmentNodes[j][0];
				}
			}

			TempSegmentNodes[i] = this->SegmentNodes[ID];
			this->SegmentNodes[ID][1] = -1;
		}

		this->NoSegmentNodes	= 0;
		float LastValue			= 0.0f;
		this->SegmentNodes[0]	= TempSegmentNodes[0];
		this->NoSegmentNodes	= 1;

		for (int i = 1; i < this->NoSegmentNodes; i++)
		{
			if (TempSegmentNodes[i][1] != LastValue)
				this->SegmentNodes[this->NoSegmentNodes++] = TempSegmentNodes[i];				
		}
	}

	HOST_DEVICE bool Inside(const float& T, float& MaxT) const
	{
		bool Inside = false;
		float TempMaxT = 0.0f;

		for (int i = 0; i < this->NoSegments; i++)
		{
			float LocalMaxT = 0.0f;

			if (this->Segments[i].Inside(T, LocalMaxT) && LocalMaxT > TempMaxT)
			{
				Inside = true;
				TempMaxT = LocalMaxT;
			}
		}

		if (Inside)
			MaxT = TempMaxT;

		return Inside;

		/*
		bool Inside = false;

		float TempMaxT = 0.0f;

		for (int i = 1; i < NoSegmentNodes; i++)
		{
			if (T > this->SegmentNodes[i - 1][0] && T > this->SegmentNodes[i][0] && this->SegmentNodes[i - 1][1] == 1.0f)
			{
				MaxT = this->SegmentNodes[i][0];
				return true;
			}
		}

		return false;
		*/
	}

	Segment		Segments[MAX_NO_CLIPPING_SEGMENTS];
	int			NoSegments;
	Vec2f		SegmentNodes[2 * MAX_NO_CLIPPING_SEGMENTS];
	int			NoSegmentNodes;
};

}
