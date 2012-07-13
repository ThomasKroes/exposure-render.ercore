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

#include "randomseedbuffer.h"

namespace ExposureRender
{

/*! \class 2D Random seed buffer
 * \brief Two-dimensional random seed buffer
 */
class RandomSeedBuffer2D : public RandomSeedBuffer<2>
{
public:
	/*! Constructor
		@param[in] pName Buffer name
		@param[in] MemoryType Place where the memory resides, can be host or device
	*/
	HOST RandomSeedBuffer2D(const char* pName = "2D Random seed buffer", const Enums::MemoryType& MemoryType = Enums::Device) :
		RandomSeedBuffer<2>(pName, MemoryType)
	{
	}
	
	/*! Get buffer element at discrete position \a X, \a Y
		@param[in] X X position in buffer
		@param[in] Y Y position in buffer
		@return Element at \a X, \a Y
	*/
	HOST_DEVICE unsigned int& operator()(const int& X = 0, const int& Y = 0) const
	{
		const Vec2i ClampedXY(Clamp(X, 0, this->Resolution[0] - 1), Clamp(Y, 0, this->Resolution[1] - 1));
		return this->Data[ClampedXY[1] * this->Resolution[0] + ClampedXY[0]];
	}
};

}
