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
class EXPOSURE_RENDER_DLL Buffer1D : public Buffer<T>
{
public:
	HOST Buffer1D(const char* pName = "Buffer1D", const Enums::MemoryType& MemoryType = Enums::Host, const Enums::FilterMode& FilterMode = Enums::Linear, const Enums::AddressMode& AddressMode = Enums::Wrap) :
		Buffer<T>(pName, MemoryType, FilterMode, AddressMode),
		Resolution(0)
	{
	}

	HOST Buffer1D(const Buffer1D& Other) :
		Buffer<T>(),
		Resolution(0)
	{
		*this = Other;
	}

	HOST virtual ~Buffer1D(void)
	{
		this->Free();
	}

	HOST Buffer1D& operator = (const Buffer1D& Other)
	{
		Buffer<T>::operator = (Other);

		if (this->TimeStamp != Other.TimeStamp)
		{
			this->Set(Other.MemoryType, Other.Resolution, Other.Data);
			this->TimeStamp = Other.TimeStamp;
		}

		return *this;
	}

	HOST void Free(void)
	{
		char MemoryString[MAX_CHAR_SIZE];
		
		this->GetMemoryString(MemoryString, Enums::MegaByte);

		if (this->Data)
		{
			if (this->MemoryType == Enums::Host)
			{
				free(this->Data);
				this->Data = NULL;
			}

#ifdef __CUDACC__
			if (this->MemoryType == Enums::Device)
			{
				Cuda::Free(this->Data);
			}
#endif
		}
				
		this->Resolution	= 0;
		this->NoElements	= 0;
		
		this->TimeStamp.Modified();
	}

	HOST void Reset(void)
	{
		if (this->GetNoElements() <= 0)
			return;
		
		if (this->MemoryType == Enums::Host)
			memset(this->Data, 0, this->GetNoBytes());

#ifdef __CUDACC__
		if (this->MemoryType == Enums::Device)
			Cuda::MemSet(this->Data, 0, this->GetNoElements());
#endif
		
		this->TimeStamp.Modified();
	}

	HOST void Resize(const int& Resolution)
	{
		if (this->Resolution == Resolution)
			return;
		else
			this->Free();

		this->Resolution = Resolution;

		this->NoElements = this->Resolution;

		if (this->NoElements <= 0)
			return;
		
		char MemoryString[MAX_CHAR_SIZE];
		
		this->GetMemoryString(MemoryString, Enums::MegaByte);

		if (this->MemoryType == Enums::Host)
		{
			this->Data = (T*)malloc(this->GetNoBytes());
		}

#ifdef __CUDACC__
		if (this->MemoryType == Enums::Device)
		{
			Cuda::Allocate(this->Data, this->GetNoElements());
		}
#endif

		this->Reset();
	}

	HOST void Set(const Enums::MemoryType& MemoryType, const int& Resolution, T* Data)
	{
		this->Resize(Resolution);

		if (this->NoElements <= 0)
			return;

		if (this->MemoryType == Enums::Host)
		{
			if (MemoryType == Enums::Host)
				memcpy(this->Data, Data, this->GetNoBytes());
			
#ifdef __CUDACC__
			if (MemoryType == Enums::Device)
				Cuda::MemCopyDeviceToHost(Data, this->Data, this->GetNoElements());
#endif
		}

#ifdef __CUDACC__
		if (this->MemoryType == Enums::Device)
		{
			if (MemoryType == Enums::Host)
				Cuda::MemCopyHostToDevice(Data, this->Data, this->GetNoElements());

			if (MemoryType == Enums::Device)
				Cuda::MemCopyDeviceToDevice(Data, this->Data, this->GetNoElements());
		}
#endif
	}

	HOST_DEVICE T& operator()(const int& X) const
	{
		const int ClampedX = Clamp(X, 0, this->Resolution - 1);
		return this->Data[ClampedX];
	}

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

	HOST_DEVICE T& operator[](const int& ID) const
	{
		const int ClampedID = Clamp(ID, 0, this->NoElements - 1);
		return this->Data[ClampedID];
	}

	HOST_DEVICE int GetResolution() const { return this->Resolution; }

protected:
	int	Resolution;
};

}
