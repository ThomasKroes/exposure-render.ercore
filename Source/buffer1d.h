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

#include "buffer.h"

namespace ExposureRender
{

/*! \class One-dimensional buffer
 * \brief One-dimensional memory container class for both host and device (CUDA) linear memory
 */
template<class T>
class EXPOSURE_RENDER_DLL Buffer1D : public Buffer<T, 1>
{
public:
	/*! Constructor
		@param[in] pName Buffer name
		@param[in] MemoryType Place where the memory resides, can be host or device
		@param[in] FilterMode Type of filtering
		@param[in] AddressMode Type of addressing near edges
	*/
	HOST Buffer1D(const char* pName = "Buffer1D", const Enums::MemoryType& MemoryType = Enums::Host, const Enums::FilterMode& FilterMode = Enums::Linear, const Enums::AddressMode& AddressMode = Enums::Wrap) :
		Buffer<T, 1>(pName, MemoryType, FilterMode, AddressMode)
	{
	}
	
	/*! Get buffer element at discrete location \a X
		@param[in] X Integer position in buffer
		@return Element at \a X
	*/
	HOST_DEVICE T& operator()(const int& X) const
	{
		return this->Data[Clamp(X, 0, this->Resolution - 1)];
	}
	
	/*! Get buffer element at (normalized) floating point position \a X
		@param[in] X Floating point position
		@param[in] Normalized Whether \a X is normalized or not
		@return Interpolated value at \a X
	*/
	HOST_DEVICE T operator()(const float& X, const bool Normalized = false) const
	{
		if (!this->Data)
			return T();

		const float U = Normalized ? X * (float)this->Resolution : X;

		switch (this->FilterMode)
		{
			case Enums::NearestNeighbour:
			{
				return (*this)((int)floorf(U));
			}

			case Enums::Linear:
			{
				int Coord[2] =
				{
					(int)floorf(U), (int)ceilf(U)
				};

				const float DU = U - Coord[0];

				Coord[0] = min(max(Coord[0], 0), this->Resolution - 1);
				Coord[1] = min(max(Coord[1], 0), this->Resolution - 1);
				
				T Values[2] = 
				{
					T((*this)(Coord[0])),
					T((*this)(Coord[1])),
				};

				return Lerp(DU, Values[0], Values[1]);
			}

			default:
				return T();
		}
	}
};

}
