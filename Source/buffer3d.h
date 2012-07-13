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

#include "buffer.h"

namespace ExposureRender
{

/*! \class Three-dimensional buffer
 * \brief Three-dimensional memory container class for both host and device (CUDA) linear memory
 */
template<class T>
class EXPOSURE_RENDER_DLL Buffer3D : public Buffer<T, 3>
{
public:
	/*! Constructor
		@param[in] pName Buffer name
		@param[in] MemoryType Place where the memory resides, can be host or device
		@param[in] FilterMode Type of filtering
		@param[in] AddressMode Type of addressing near edges
	*/
	HOST Buffer3D(const char* pName = "Buffer3D", const Enums::MemoryType& MemoryType = Enums::Host, const Enums::FilterMode& FilterMode = Enums::Linear, const Enums::AddressMode& AddressMode = Enums::Border) :
		Buffer<T, 3>(pName, MemoryType, FilterMode, AddressMode)
	{
	}
	
	/*! Get buffer element at discrete position \a X, \a Y, \a Z
		@param[in] X X position in buffer
		@param[in] Y Y position in buffer
		@param[in] Z Z position in buffer
		@return Element at \a X, \a Y, \a Z
	*/
	HOST_DEVICE T& operator()(const int& X = 0, const int& Y = 0, const int& Z = 0) const
	{
		const Vec3i ClampedXYZ(Clamp(X, 0, this->Resolution[0] - 1), Clamp(Y, 0, this->Resolution[1] - 1), Clamp(Z, 0, this->Resolution[2] - 1));
		return this->Data[ClampedXYZ[2] * this->Resolution[0] * this->Resolution[1] + ClampedXYZ[1] * this->Resolution[0] + ClampedXYZ[0]];
	}
	
	/*! Get buffer element at position \a XYZ
		@param[in] XYZ XYZ position in buffer
		@return Interpolated value at \a XYZ
	*/
	HOST_DEVICE T& operator()(const Vec3i& XYZ) const
	{
		const Vec3i ClampedXYZ(Clamp(XYZ[0], 0, this->Resolution[0] - 1), Clamp(XYZ[1], 0, this->Resolution[1] - 1), Clamp(XYZ[2], 0, this->Resolution[2] - 1));
		return this->Data[ClampedXYZ[2] * this->Resolution[0] * this->Resolution[1] + ClampedXYZ[1] * this->Resolution[0] + ClampedXYZ[0]];
	}
	
	/*! Get buffer element at (normalized) floating point position \a XYZ
		@param[in] XYZ Floating point position
		@param[in] Normalized Whether \a XYZ is normalized or not
		@return Interpolated value at \a XYZ
	*/
	HOST_DEVICE T operator()(const Vec3f& XYZ, const bool Normalized = false) const
	{
		if (!this->Data)
			return T();

		const Vec3f UVW = Normalized ? XYZ * Vec3f((float)this->Resolution[0], (float)this->Resolution[1], (float)this->Resolution[2]) : XYZ;

		switch (this->FilterMode)
		{
			case Enums::NearestNeighbour:
			{
				return (*this)((int)floorf(UVW[0]), (int)floorf(UVW[1]), (int)floorf(UVW[2]));
			}

			case Enums::Linear:
			{
				const int vx = (int)floorf(UVW[0]);
				const int vy = (int)floorf(UVW[1]);
				const int vz = (int)floorf(UVW[2]);

				const float dx = UVW[0] - vx;
				const float dy = UVW[1] - vy;
				const float dz = UVW[2] - vz;

				const T d00 = Lerp(dx, (*this)(vx, vy, vz), (*this)(vx+1, vy, vz));
				const T d10 = Lerp(dx, (*this)(vx, vy+1, vz), (*this)(vx+1, vy+1, vz));
				const T d01 = Lerp(dx, (*this)(vx, vy, vz+1), (*this)(vx+1, vy, vz+1));
				const T d11 = Lerp(dx, (*this)(vx, vy+1, vz+1), (*this)(vx+1, vy+1, vz+1));
				const T d0	= Lerp(dy, d00, d10);
				const T d1 	= Lerp(dy, d01, d11);

				return Lerp(dz, d0, d1);
			}

			default:
				return T();
		}
	}
};

}
