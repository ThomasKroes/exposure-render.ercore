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

#include "filter.h"

namespace ExposureRender
{

class GaussianFilter : public Filter
{
public:
	HOST_DEVICE GaussianFilter(const Vec2f& Size = Vec2f(2.0f), const float& Alpha = 2.0f) :
		Filter(Size),
		Alpha(Alpha),
		ExpX(expf(-Alpha * Size[0] * Size[0])),
		ExpY(expf(-Alpha * Size[1] * Size[1]))
	{
	}
	
	HOST_DEVICE float Evaluate(const float& X, const float& Y) const
	{
		return this->Gaussian(X * this->InvSize[0], this->ExpX) * this->Gaussian(Y * this->InvSize[1], this->ExpY);
	}
	
private:
	float Alpha;
	float ExpX;
	float ExpY;
	
	HOST_DEVICE float Gaussian(const float& D, const float& ExpV) const
	{
		return max(0.0f, expf(-this->Alpha * D * D) - ExpV);
	}
};

template<int FilterRadius>
class GaussianFilterTable
{	
public:
	HOST GaussianFilterTable()
	{
		this->Generate();
	}

	HOST GaussianFilterTable(const GaussianFilterTable& Other)
	{
		*this = Other;
	}

	HOST GaussianFilterTable& operator = (const GaussianFilterTable& Other)
	{
		for (int i = 0; i < FilterRadius * 2 + 1; i++)
			for (int j = 0; j < FilterRadius * 2 + 1; j++)
				this->Weights[i][j] = Other.Weights[i][j];

		return *this;
	}

	HOST void Generate()
	{
		Vec2f UV;

		for (int i = 0; i < FilterRadius * 2 + 1; i++)
		{
			for (int j = 0; j < FilterRadius * 2 + 1; j++)
			{
				UV[0] = -(FilterRadius + 0.5f) + (float)i + 0.5f;
				UV[1] = -(FilterRadius + 0.5f) + (float)j + 0.5f;

				this->Weights[i][j] = Gauss2D(0.75f * FilterRadius, UV[0], UV[1]);
			}
		}

		this->PrintSelf();
	}

	HOST void PrintSelf()
	{
		std::string Message;

		for (int j = 0; j < FilterRadius * 2 + 1; j++)
		{
			for (int i = 0; i < FilterRadius * 2 + 1; i++)
			{
				char Weight[MAX_CHAR_SIZE];

				if (i == FilterRadius * 2)
					sprintf_s(Weight, MAX_CHAR_SIZE, "%0.2f\n", this->Weights[i][j]);
				else
					sprintf_s(Weight, MAX_CHAR_SIZE, "%0.2f, ", this->Weights[i][j]);

				Message.append(Weight);
			}
		}

		Message.append("\n");

		printf(Message.c_str());
	}

	float	Weights[FilterRadius * 2 + 1][FilterRadius * 2 + 1];
};

class GaussianFilterTables
{
public:
	GaussianFilterTable<1> Gaussian3x3;
	GaussianFilterTable<2> Gaussian5x5;
	GaussianFilterTable<3> Gaussian7x7;
};

}
