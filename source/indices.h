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

/*! \class Indices
 * \brief Vector of shorts 
 */
template<int Size = 64>
class EXPOSURE_RENDER_DLL Indices : public Vec<short, Size>
{
public:
	/*! Default constructor */
	HOST_DEVICE Indices() :
		NoIndices(0)
	{
		this->Reset();
	}
	
	/*! Assignment operator
		@param[in] Other Indices to copy from
		@return Copied indices by reference
	*/
	HOST_DEVICE Indices& operator = (const Indices& Other)
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] = Other.D[i];

		this->NoIndices = Other.NoIndices;

		return *this;
	}
	
	/*! Obtain number of indices set
		@return Number of used indices
	*/
	HOST_DEVICE int GetNoIndices() const
	{
		return this->NoIndices;
	}

	/*! Set number of indices
		@param[in] NoIndices Number of indices
	*/
	HOST_DEVICE void SetNoIndices(const int& NoIndices)
	{
		this->NoIndices = NoIndices;
	}

	/*! Resets the indices */
	HOST_DEVICE void Reset()
	{
		for (int i = 0; i < Size; ++i)
			this->D[i] = -1;

		this->NoIndices = 0;
	}

	/*! Adds an index
		@param[in] Index Index
	*/
	HOST_DEVICE void Add(const int& Index)
	{
		if (this->GetNoIndices() >= Size)
			return;

		this->D[this->NoIndices] = Index;
		this->NoIndices++;
	}

protected:
	int		NoIndices;	/*! Number of indices */
};

}
