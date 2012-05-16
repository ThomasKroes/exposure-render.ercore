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
class EXPOSURE_RENDER_DLL Texture3D : public Buffer<T>
{
public:
	HOST Texture3D(const char* pName = "Texture3D", const Enums::MemoryType& MemoryType = Enums::Device, const Enums::DeviceType& DeviceType = Enums::Cuda) :
		Buffer<T>(MemoryType, pName),
		Resolution(0)
	{
	}

	HOST Texture3D(const Texture3D& Other) :
		Buffer<T>(),
		Resolution(0)
	{
		*this = Other;
	}

	HOST virtual ~Buffer3D(void)
	{
		this->Free();
	}

	HOST Buffer3D& operator = (const Buffer3D& Other)
	{
		DebugLog("%s: this = %s, Other = %s", __FUNCTION__, this->GetFullName(), Other.GetFullName());
		
		if (Other.Dirty)
		{
			this->Set(Other.MemoryType, Other.Resolution, Other.Data);
			Other.Dirty = false;
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
			if (this->MemoryType == Enums::Host)
			{
				free(this->Data);
				this->Data = NULL;
				DebugLog("Freed %s on host", MemoryString);
			}

#ifdef __CUDA_ARCH__
			if (this->MemoryType == Enums::Device)
			{
				Cuda::Free(this->Data);
				DebugLog("Freed %s on device", MemoryString);
			}
#endif
		}
				
		this->Resolution	= Vec3i(0);
		this->NoElements	= 0;
		this->Dirty			= true;
	}


private:
#ifdef __CUDA_ARCH__
	cudaArray*	CudaArray
#endif
};

}
