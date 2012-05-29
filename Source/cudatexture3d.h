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

#include "buffer3d.h"

namespace ExposureRender
{

template<class T>
class EXPOSURE_RENDER_DLL CudaTexture3D
{
public:
	HOST CudaTexture3D() :
		Resolution(0),
		Array(0),
		Normalized(true),
		FilterMode(Enums::Linear),
		AddressMode(Enums::Wrap)
	{
	}

	HOST virtual ~CudaTexture3D(void)
	{
		this->Free();
	}

	HOST CudaTexture3D& operator = (const CudaTexture3D& Other)
	{
		throw (Exception(Enums::Error, "Not implemented yet!"));
	}

	HOST CudaTexture3D<T>& operator = (const Buffer3D<T>& Other)
	{
		this->Resize(Other.GetResolution());

		const int NoElementes = this->Resolution[0] * this->Resolution[1] * this->Resolution[2];

		if (NoElementes <= 0)
			return *this;

		cudaExtent CudaExtent;
		
		CudaExtent.width	= this->Resolution[0];
		CudaExtent.height	= this->Resolution[1];
		CudaExtent.depth	= this->Resolution[2];

		cudaMemcpy3DParms CopyParams = {0};
		
		CopyParams.srcPtr		= make_cudaPitchedPtr((void*)Other.GetData(), CudaExtent.width * sizeof(unsigned short), CudaExtent.width, CudaExtent.height);
		CopyParams.dstArray		= this->Array;
		CopyParams.extent		= CudaExtent;
		CopyParams.kind			= cudaMemcpyHostToDevice;
		
		Cuda::Memcpy3D(&CopyParams);

		return *this;
	}

	HOST void Free(void)
	{
		Cuda::FreeArray(this->Array);
	}

	HOST void Resize(const Vec3i& Resolution)
	{
		if (this->Resolution == Resolution)
			return;
		else
			this->Free();
		
		this->Resolution = Resolution;
		
		const int NoElementes = this->Resolution[0] * this->Resolution[1] * this->Resolution[2];

		if (NoElementes <= 0)
			throw (Exception(Enums::Error, "No. elements is zero!"));

		Cuda::Malloc3DArray(&this->Array, cudaCreateChannelDesc<T>(), this->Resolution);
	}

	HOST void Bind(textureReference* TextureReference)
	{
		TextureReference->normalized		= this->Normalized;
		TextureReference->filterMode		= (cudaTextureFilterMode)this->FilterMode;
		TextureReference->addressMode[0]	= (cudaTextureAddressMode)this->AddressMode;
		TextureReference->addressMode[1]	= (cudaTextureAddressMode)this->AddressMode;
		TextureReference->addressMode[2]	= (cudaTextureAddressMode)this->AddressMode;

		const cudaChannelFormatDesc ChannelFormatDescription = cudaCreateChannelDesc<T>();
		Cuda::BindTextureToArray(TextureReference, this->Array, &ChannelFormatDescription);
	}

	DEVICE float operator()(const Vec3f& XYZ) const
	{
		return tex3D(VolumeTexture, XYZ[0], XYZ[1], XYZ[2]);
	}

	HOST_DEVICE Vec3i GetResolution() const
	{
		return this->Resolution;
	}

protected:
	Vec3i					Resolution;
	cudaArray*				Array;
	bool					Normalized;
	Enums::FilterMode		FilterMode;
	Enums::AddressMode		AddressMode;
			
};

}
