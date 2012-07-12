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

#include "utilities.h"

namespace ExposureRender
{

/*! \class Vec
 * \brief Base vector template class
 */
template<class T, int Size = 1>
class EXPOSURE_RENDER_DLL Vec
{
public:
	/*! Default constructor */
	HOST_DEVICE Vec()
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] = T();
	}
	
	/*! Copy constructor */
	HOST_DEVICE Vec(const Vec& Other)
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] = Other[i];
	}
	
	/*! Construct and initialize with default value
		@param[in] Default Default value
	*/
	HOST_DEVICE Vec(const T& V)
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] = V;
	}
	
	/*! Construct and initialize with array
		@param[in] V Data array
	*/
	HOST_DEVICE Vec(const T V[Size])
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] = V[i];
	}
	
	/*! Subscript operator
		@param[in] i Element from array
		* \return Return element by value
	*/
	HOST_DEVICE T operator[](const int& i) const
	{
		return this->D[i];
	}

	/*! Subscript operator
		@param[in] i Array element index
		* \return Return element by reference
	*/
	HOST_DEVICE T& operator[](const int& i)
	{
		return this->D[i];
	}

	/*! Assignment operator
		@param[in] Other Vector to copy from
		* \return Copied vector by reference
	*/
	HOST_DEVICE Vec<T, Size>& operator = (const Vec<T, Size>& Other)
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] = Other[i];
	
		return *this;
	}
	
	/*! Add operator
		@param[in] V Vector to add
		* \return Added vector by value
	*/
	HOST_DEVICE Vec<T, Size> operator + (const Vec<T, Size>& V) const
	{
		Vec<T, Size> Result;
		
		for (int i = 0; i < Size; ++i)
			Result[i] = this->D[i] + V[i];
		
		return Result;
	}
	
	/*! Add + assignment operator
		@param[in] V Vector to add
		* \return Added vector by reference
	*/
	HOST_DEVICE Vec<T, Size>& operator += (const Vec<T, Size>& V)
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] += V[i];
		
		return *this;
	}
		
	/*! Subtract operator
		@param[in] V Vector to subtract
		* \return Subtracted vector by value
	*/
	HOST_DEVICE Vec<T, Size> operator - (const Vec<T, Size>& V) const
	{
		Vec<T, Size> Result;
		
		for (int i = 0; i < Size; ++i)
			Result[i] = this->D[i] - V[i];
		
		return Result;
	}

	/*! Subtract + assignment operator
		@param[in] V Vector to subtract
		* \return Subtracted vector by reference
	*/
	HOST_DEVICE Vec<T, Size>& operator -= (const Vec<T, Size>& V)
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] -= V[i];
		
		return *this;
	}

	/*! Multiply operator
		@param[in] V Vector to multiply by
		* \return Multiplied vector by value
	*/
	HOST_DEVICE Vec<T, Size> operator * (const Vec<T, Size>& V) const
	{
		Vec<T, Size> Result;
		
		for (int i = 0; i < Size; ++i)
			Result[i] = this->D[i] * V[i];
		
		return Result;
	}

	/*! Multiply + assignment operator
		@param[in] V Vector to multiply by
		* \return Multiplied vector by reference
	*/
	HOST_DEVICE Vec<T, Size>& operator *= (const Vec<T, Size>& V)
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] *= V[i];
		
		return *this;
	}

	/*! Divide operator
		@param[in] V Vector to divide by
		* \return Divided vector by value
	*/
	HOST_DEVICE Vec<T, Size> operator / (const Vec<T, Size>& V) const
	{
		Vec<T, Size> Result;
		
		for (int i = 0; i < Size; ++i)
			Result[i] = this->D[i] / V[i];
		
		return Result;
	}

	/*! Divide + assignment operator
		@param[in] V Vector to divide by
		* \return Divided vector by reference
	*/
	HOST_DEVICE Vec<T, Size>& operator /= (const Vec<T, Size>& V)
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] /= V[i];
		
		return *this;
	}
	
	/*! Negate operator
		* \return Negated vector by value
	*/
	HOST_DEVICE Vec<T, Size> operator - () const
	{
		Vec<T, Size> Result;
		
		for (int i = 0; i < Size; ++i)
			Result[i] = -this->D[i];
		
		return Result;
	}

	/*! Less than operator
		@param[in] V Vector to check against
	*/
	HOST_DEVICE bool operator < (const Vec<T, Size>& V) const
	{
		for (int i = 0; i < Size; ++i)
		{
			if (this->D[i] >= V[i])
				return false;
		}
		
		return true;
	}
	
	/*! Less than or equal operator
		@param[in] V Vector to check against
	*/
	HOST_DEVICE bool operator <= (const Vec<T, Size>& V) const
	{
		for (int i = 0; i < Size; ++i)
		{
			if (this->D[i] > V[i])
				return false;
		}
		
		return true;
	}
	
	/*! Larger than operator
		@param[in] V Vector to check against
	*/
	HOST_DEVICE bool operator > (const Vec<T, Size>& V) const
	{
		for (int i = 0; i < Size; ++i)
		{
			if (this->D[i] <= V[i])
				return false;
		}
	
		return true;
	}

	/*! Larger than or equal to operator
		@param[in] V Vector to check against
	*/
	HOST_DEVICE bool operator >= (const Vec<T, Size>& V) const
	{
		for (int i = 0; i < Size; ++i)
		{
			if (this->D[i] < V[i])
				return false;
		}
		
		return true;
	}

	/*! Equality operator
		@param[in] V Vector to check against
	*/
	HOST_DEVICE bool operator == (const Vec<T, Size>& V) const
	{
		for (int i = 0; i < Size; ++i)
		{
			if (this->D[i] != V[i])
				return false;
		}
	
		return true;
	}

	/*! Inequality operator
		@param[in] V Vector to check against
	*/
	HOST_DEVICE bool operator != (const Vec<T, Size>& V) const
	{
		for (int i = 0; i < Size; ++i)
		{
			if (this->D[i] != V[i])
				return true;
		}
		
		return false;
	}
	
	/*! Minimum element
		* \return Minimum element in range
	*/
	HOST_DEVICE T Min()
	{
		T Min = D[0];
		
		for (int i = 1; i < Size; ++i)
		{
			if (D[i] < Min)
				Min = this->D[i];
		}
		
		return Min;
	}

	/*! Maximum element
		* \return Maximum element in range
	*/
	HOST_DEVICE T Max()
	{
		T Max = D[0];
		
		for (int i = 1; i < Size; ++i)
		{
			if (D[i] > Max)
				Max = this->D[i];
		}
		
		return Max;
	}

	/*! Minimum vector
		* \return Minimum vector
	*/
	HOST_DEVICE Vec<T, Size> Min(const Vec<T, Size>& Other) const
	{
		Vec<T, Size> Result;
		
		for (int i = 0; i < Size; ++i)
			Result[i] = min(this->D[i], Other[i]);
		
		return Result;
	}

	/*! Maximum vector
		* \return Maximum vector
	*/
	HOST_DEVICE Vec<T, Size> Max(const Vec<T, Size>& Other) const
	{
		Vec<T, Size> Result;
	
		for (int i = 0; i < Size; ++i)
			Result[i] = max(this->D[i], Other[i]);
		
		return Result;
	}

	/*! Clamp to single value */
	HOST_DEVICE void Clamp(const T& Min, const T& Max)
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] = max(Min, min(this->D[i], Max));
	}

	/*! Clamp to vector */
	HOST_DEVICE void Clamp(const Vec<T, Size>& Min, const Vec<T, Size>& Max)
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] = max(Min[i], min(this->D[i], Max[i]));
	}
	
	/*! Get pointer to data array */
	HOST_DEVICE T* GetData()
	{
		return this->D;
	}

// protected: FIXME
	T	D[Size];	/*! Vector data */
};

}
