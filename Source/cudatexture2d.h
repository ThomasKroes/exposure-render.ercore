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

#include "buffer1d.h"

namespace ExposureRender
{

template<class T>
class EXPOSURE_RENDER_DLL CudaTexture1D
{
public:
	HOST CudaTexture1D(const bool& Normalized = true, const Enums::FilterMode& FilterMode = Enums::Linear, const Enums::AddressMode& AddressMode = Enums::Clamp) :
		Resolution(0),
		Array(NULL),
		Normalized(Normalized),
		FilterMode(FilterMode),
		AddressMode(AddressMode)
	{
	}

	HOST virtual ~CudaTexture1D(void)
	{
		this->Free();
	}

	HOST CudaTexture1D& operator = (const CudaTexture1D& Other)
	{
		throw (Exception(Enums::Error, "Not implemented yet!"));
	}

	HOST CudaTexture1D<T>& operator = (const Buffer1D<T>& Other)
	{
		this->Resize(Other.GetResolution());
		
		this->FilterMode	= Other.GetFilterMode();
		this->AddressMode	= Other.GetAddressMode();

		const int NoElements = this->Resolution;

		if (NoElements <= 0)
			return *this;

		Cuda::MemcpyToArray(this->Array, 0, 0, Other.GetData(), Other.GetNoBytes(), cudaMemcpyHostToDevice);

		return *this;
	}

	HOST void Free(void)
	{
		Cuda::FreeArray(this->Array);
	}

	HOST void Resize(const int& Resolution)
	{
		if (this->Resolution == Resolution)
			return;
		else
			this->Free();
		
		this->Resolution = Resolution;
		
		const int NoElements = this->Resolution;

		if (NoElements <= 0)
			throw (Exception(Enums::Error, "No. elements is zero!"));

		Cuda::MallocArray(&this->Array, cudaCreateChannelDesc<T>(), Vec2i(NoElements, 1));
	}

	HOST void Bind(textureReference& TextureReference)
	{
		if (this->Resolution == 0)
			return;

		if (this->Array == NULL)
			return;

		TextureReference.normalized		= this->Normalized;
		TextureReference.filterMode		= cudaFilterModeLinear;//(cudaTextureFilterMode)this->FilterMode;
		TextureReference.addressMode[0]	= cudaAddressModeClamp;//(cudaTextureAddressMode)this->AddressMode;

		const cudaChannelFormatDesc ChannelFormatDescription = cudaCreateChannelDesc<T>();

		Cuda::BindTextureToArray(&TextureReference, this->Array, &ChannelFormatDescription);
	}

	HOST_DEVICE int GetResolution() const
	{
		return this->Resolution;
	}

protected:
	int						Resolution;
	cudaArray*				Array;
	bool					Normalized;
	Enums::FilterMode		FilterMode;
	Enums::AddressMode		AddressMode;
			
};

}
