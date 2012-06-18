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

#include "plf.h"
#include "color.h"
#include "timestamp.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL ScalarNode
{
public:
	HOST ScalarNode(float Position, float Value) :
		Position(Position),
		Value(Value)
	{
	}

	HOST ScalarNode() :
		Position(0.0f),
		Value(0.0f)
	{
	}

	HOST ScalarNode(const ScalarNode& Other)
	{
		*this = Other;
	}

	HOST ScalarNode& operator = (const ScalarNode& Other)
	{
		this->Position	= Other.Position;
		this->Value		= Other.Value;

		return *this;
	}

	float	Position;
	float	Value;
};

class EXPOSURE_RENDER_DLL ScalarTransferFunction1D : public TimeStamp
{
public:
	HOST ScalarTransferFunction1D() :
		TimeStamp()
	{
	}

	HOST ScalarTransferFunction1D(const ScalarTransferFunction1D& Other)
	{
		*this = Other;
	}

	HOST ScalarTransferFunction1D& operator = (const ScalarTransferFunction1D& Other)
	{	
		this->PLF	= Other.PLF;

		return *this;
	}

	HOST void AddNode(const ScalarNode& Node)
	{
		this->PLF.AddNode(Node.Position, Node.Value);
	}

	HOST void Reset()
	{
		this->PLF.Reset();
	}

	HOST_DEVICE float Evaluate(const float& Intensity) const
	{
		return this->PLF.Evaluate(Intensity);
	}

	PiecewiseLinearFunction<MAX_NO_TF_NODES>	PLF;
};

}
