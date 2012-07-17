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

#include "filter.h"

namespace ExposureRender
{

/*! Gaussian filter */
class GaussianFilter : public Filter
{
public:
	/*! Constructor
		@param Size Size of the filter
		@param Alpha Alpha
	*/
	HOST_DEVICE GaussianFilter(const Vec2f& Size = Vec2f(2.0f), const float& Alpha = 2.0f) :
		Filter(Size),
		Alpha(Alpha),
		ExpX(expf(-Alpha * Size[0] * Size[0])),
		ExpY(expf(-Alpha * Size[1] * Size[1]))
	{
	}
	
	/*! Copy constructor
		@param[in] Other Gaussian filter to copy
	*/
	HOST GaussianFilter(const GaussianFilter& Other) :
		Filter(Size),
		Alpha(0.0f),
		ExpX(0.0f),
		ExpY(0.0f)
	{
		*this = Other;
	}
	
	/*! Assignment operator
		@param[in] Other Gaussian filter to copy
		@return Copied Gaussian filter
	*/
	HOST GaussianFilter& operator = (const GaussianFilter& Other)
	{
		Filter::operator = (Other);

		this->Alpha		= Other.Alpha;
		this->ExpX		= Other.ExpX;
		this->ExpY		= Other.ExpY;

		return *this;
	}
	
	/*! Evaluates the Gaussian filter
		@param[in] X X position
		@param[in] Y Y position
		@return Gaussian filter weight
	*/
	HOST_DEVICE float Evaluate(const float& X, const float& Y) const
	{
		return this->Gaussian(X * this->InvSize[0], this->ExpX) * this->Gaussian(Y * this->InvSize[1], this->ExpY);
	}
	
protected:
	/*! Evaluates the one-dimensional Gaussian filter
		@param[in] D D
		@param[in] ExpV Precomputed quantity
		@return Gaussian filter weight
	*/
	HOST_DEVICE float Gaussian(const float& D, const float& ExpV) const
	{
		return max(0.0f, expf(-this->Alpha * D * D) - ExpV);
	}

	float Alpha;			/*! Alpha */
	float ExpX;				/*! Internal precomputed quantity */
	float ExpY;				/*! Internal precomputed quantity */
};

/*! Gaussian filter table class */
template<int FilterRadius>
class GaussianFilterTable
{	
public:
	/*! Default constructor
		@param Size Size of the filter
	*/
	HOST GaussianFilterTable()
	{
		this->Generate();
	}

	/*! Copy constructor
		@param[in] Other Gaussian filter table to copy
	*/
	HOST GaussianFilterTable(const GaussianFilterTable& Other)
	{
		*this = Other;
	}
	
	/*! Assignment operator
		@param[in] Other Gaussian filter table to copy
		@return Copied Gaussian filter table
	*/
	HOST GaussianFilterTable& operator = (const GaussianFilterTable& Other)
	{
		for (int i = 0; i < FilterRadius * 2 + 1; i++)
			for (int j = 0; j < FilterRadius * 2 + 1; j++)
				this->Weight[i][j] = Other.Weight[i][j];

		return *this;
	}
	
	/*! Generates the Gaussian filter table */
	HOST void Generate()
	{
		Vec2f UV;

		for (int i = 0; i < FilterRadius * 2 + 1; i++)
		{
			for (int j = 0; j < FilterRadius * 2 + 1; j++)
			{
				UV[0] = -(FilterRadius + 0.5f) + (float)i + 0.5f;
				UV[1] = -(FilterRadius + 0.5f) + (float)j + 0.5f;

				this->Weight[i][j] = Gauss2D(0.75f * FilterRadius, UV[0], UV[1]);
			}
		}

		this->PrintSelf();
	}

	/*! Prints the Gaussian filter table weights */
	HOST void PrintSelf()
	{
		std::string Message;

		for (int j = 0; j < FilterRadius * 2 + 1; j++)
		{
			for (int i = 0; i < FilterRadius * 2 + 1; i++)
			{
				char Weight[MAX_CHAR_SIZE];

				if (i == FilterRadius * 2)
					sprintf_s(Weight, MAX_CHAR_SIZE, "%0.2f\n", this->Weight[i][j]);
				else
					sprintf_s(Weight, MAX_CHAR_SIZE, "%0.2f, ", this->Weight[i][j]);

				Message.append(Weight);
			}
		}

		Message.append("\n");

		printf(Message.c_str());
	}

	GET_2D_ARRAY_ELEMENT_MACRO(HOST_DEVICE, Weight, float)

protected:
	float	Weight[FilterRadius * 2 + 1][FilterRadius * 2 + 1];		/*! Gaussian filter weights */
};

/*! Gaussian filter tables class */
class GaussianFilterTables
{
public:
	/*! Default constructor */
	HOST_DEVICE GaussianFilterTables() :
		Gaussian3x3(),
		Gaussian5x5(),
		Gaussian7x7(),
		Gaussian9x9(),
		Gaussian11x11()
	{
	}
	
	/*! Get Gaussian filter weight
		@param[in] Radius Radius of the filter
		@param[in] X X position
		@param[in] Y Y position
	*/
	HOST_DEVICE float Weight(const int& Radius, const int& X, const int& Y)
	{
		switch (Radius)
		{
			case 0:		return 0.0f;
			case 1:		return this->Gaussian3x3.GetWeight(X, Y);
			case 2:		return this->Gaussian5x5.GetWeight(X, Y);
			case 3:		return this->Gaussian7x7.GetWeight(X, Y);
			case 4:		return this->Gaussian9x9.GetWeight(X, Y);
			case 5:		return this->Gaussian11x11.GetWeight(X, Y);
		}

		return 0.0f;
	}

protected:
	GaussianFilterTable<1> Gaussian3x3;			/*! 3x3 Gaussian filter */
	GaussianFilterTable<2> Gaussian5x5;			/*! 5x5 Gaussian filter */
	GaussianFilterTable<3> Gaussian7x7;			/*! 7x7 Gaussian filter */
	GaussianFilterTable<3> Gaussian9x9;			/*! 9x9 Gaussian filter */
	GaussianFilterTable<3> Gaussian11x11;		/*! 11x11 Gaussian filter */
};

}
