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

template<class T>
class EXPOSURE_RENDER_DLL Buffer3D : public Buffer<T>
{
public:
	HOST Buffer3D(const char* pName = "Buffer3D", const Enums::MemoryType& MemoryType = Enums::Host, const Enums::FilterMode& FilterMode = Enums::Linear, const Enums::AddressMode& AddressMode = Enums::Wrap) :
		Buffer<T>(pName, MemoryType, FilterMode, AddressMode),
		Resolution(0)
	{
		DebugLog("%s: %s", __FUNCTION__, this->GetFullName());
	}

	HOST Buffer3D(const Buffer3D& Other) :
		Buffer<T>(),
		Resolution(0)
	{
		DebugLog("%s: Other = %s", __FUNCTION__, Other.GetFullName());
		
		*this = Other;
	}

	HOST virtual ~Buffer3D(void)
	{
		DebugLog(__FUNCTION__);
		this->Free();
	}

	HOST Buffer3D& operator = (const Buffer3D& Other)
	{
		Buffer<T>::operator = (Other);

		DebugLog("%s: this = %s, Other = %s", __FUNCTION__, this->GetFullName(), Other.GetFullName());
		
		if (this->TimeStamp != Other.TimeStamp)
		{
			this->Set(Other.MemoryType, Other.Resolution, Other.Data);
			this->TimeStamp = Other.TimeStamp;
		}
		
		sprintf_s(this->Name, MAX_CHAR_SIZE, "Copy of %s", Other.Name);

		return *this;
	}

	HOST void Free(void)
	{
		DebugLog("%s: %s", __FUNCTION__, this->GetFullName());

		char MemoryString[MAX_CHAR_SIZE];
		
		this->GetMemoryString(MemoryString, Enums::MegaByte);

		if (this->Data)
		{
			switch (this->MemoryType)
			{
				case Enums::Host:
				{
					free(this->Data);
					this->Data = NULL;
					DebugLog("Freed %s on host", MemoryString);
					break;
				}

#ifdef __CUDA_ARCH__
				case Enums::Device:
				{
					Cuda::Free(this->Data);
					DebugLog("Freed %s on device", MemoryString);
					break;
				}
#endif
			}
		}
				
		this->Resolution	= Vec3i(0);
		this->NoElements	= 0;

		this->TimeStamp.Modified();
	}

	HOST void Reset(void)
	{
		DebugLog("%s: %s", __FUNCTION__, this->GetFullName());
		
		if (this->GetNoElements() <= 0)
			return;
		
		switch (this->MemoryType)
		{
			case Enums::Host:
			{
				memset(this->Data, 0, this->GetNoBytes());
				break;
			}

#ifdef __CUDA_ARCH__
			case Enums::Device:
			{
				Cuda::MemSet(this->Data, 0, this->GetNoElements());
				break;
			}
#endif
		}

		this->TimeStamp.Modified();
	}

	HOST void Resize(const Vec3i& Resolution)
	{
		DebugLog("%s", __FUNCTION__);
		
		if (this->Resolution == Resolution)
			return;
		else
			this->Free();
		
		this->Resolution = Resolution;
		
		DebugLog("Resolution = [%d x %d x %d]", this->Resolution[0], this->Resolution[1], this->Resolution[2]);

		this->NoElements = this->Resolution[0] * this->Resolution[1] * this->Resolution[2];
		
		if (this->NoElements <= 0)
			return;
		
		DebugLog("No. Elements = %d", this->NoElements);

		char MemoryString[MAX_CHAR_SIZE];
		
		this->GetMemoryString(MemoryString, Enums::MegaByte);

		switch (this->MemoryType)
		{
			case Enums::Host:
			{
				this->Data = (T*)malloc(this->GetNoBytes());
				DebugLog("Allocated %s on host", MemoryString);
				break;
			}

#ifdef __CUDA_ARCH__
			case Enums::Device:
			{
				Cuda::Allocate(this->Data, this->GetNoElements());
				DebugLog("Allocated %s on device", MemoryString);
				break;
			}
#endif
		}

		this->Reset();
	}

	HOST void Set(const Enums::MemoryType& MemoryType, const Vec3i& Resolution, T* Data)
	{
		DebugLog("%s: %s, %d x %d x %d", __FUNCTION__, this->GetFullName(), Resolution[0], Resolution[1], Resolution[2]);

		this->Resize(Resolution);

		if (this->NoElements <= 0)
			return;
		
		switch (this->MemoryType)
		{
			case Enums::Host:
			{
				switch (MemoryType)
				{
					case Enums::Host:
					{
						memcpy(this->Data, Data, this->GetNoBytes());
						break;
					}
				
#ifdef __CUDA_ARCH__
					case Enums::Device:
					{
						Cuda::MemCopyDeviceToHost(Data, this->Data, this->GetNoElements());
						break;
					}
#endif
				}
				
				break;
			}

#ifdef __CUDA_ARCH__
			case Enums::Device:
			{
				switch (MemoryType)
				{
					case Enums::Host:
					{
						Cuda::MemCopyHostToDevice(Data, this->Data, this->GetNoElements());
						break;
					}

					case Enums::Device:
					{
						Cuda::MemCopyDeviceToDevice(Data, this->Data, this->GetNoElements());
						break;
					}
				}

				break;
			}
#endif
		}
	}

	HOST_DEVICE T& operator()(const int& X = 0, const int& Y = 0, const int& Z = 0) const
	{
		const Vec3i ClampedXYZ(Clamp(X, 0, this->Resolution[0] - 1), Clamp(Y, 0, this->Resolution[1] - 1), Clamp(Z, 0, this->Resolution[2] - 1));
		return this->Data[ClampedXYZ[2] * this->Resolution[0] * this->Resolution[1] + ClampedXYZ[1] * this->Resolution[0] + ClampedXYZ[0]];
	}

	HOST_DEVICE T& operator()(const Vec3i& XYZ) const
	{
		const Vec3i ClampedXYZ(Clamp(XYZ[0], 0, this->Resolution[0] - 1), Clamp(XYZ[1], 0, this->Resolution[1] - 1), Clamp(XYZ[2], 0, this->Resolution[2] - 1));
		return this->Data[ClampedXYZ[2] * this->Resolution[0] * this->Resolution[1] + ClampedXYZ[1] * this->Resolution[0] + ClampedXYZ[0]];
	}
	
	HOST_DEVICE T operator()(const Vec3f& XYZ, const bool Normalized = false) const
	{
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

	HOST_DEVICE T& operator[](const int& ID) const
	{
		const int ClampedID = Clamp(ID, 0, this->NoElements - 1);
		return this->Data[ClampedID];
	}

	HOST_DEVICE Vec3i GetResolution() const { return this->Resolution; }

protected:
	Vec3i	Resolution;

};

}
