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

#include "cudatexture.h"

namespace ExposureRender
{

template<class T>
class EXPOSURE_RENDER_DLL CudaTexture2D : public CudaTexture<T, 2>
{
public:
	/*! Constructor
		@param[in] Normalized Normalized element access
		@param[in] FilterMode Type of filtering
		@param[in] AddressMode Type of addressing near edges
	*/
	HOST CudaTexture2D(const bool& Normalized = true, const Enums::FilterMode& FilterMode = Enums::Linear, const Enums::AddressMode& AddressMode = Enums::Clamp) :
		CudaTexture<T, 2>(Normalized, FilterMode, AddressMode)
	{
	}

	/*! Assignment operator
		@param[in] Other Buffer to copy from
		@result Copied cuda texture by reference
	*/
	HOST CudaTexture2D& operator = (const Buffer2D<T>& Other)
	{
		this->Resize(Other.GetResolution());
		
		this->FilterMode	= Other.GetFilterMode();
		this->AddressMode	= Other.GetAddressMode();

		const int NoElements = this->Resolution.CumulativeProduct();

		if (NoElements <= 0)
			return *this;

#ifdef __CUDACC__
		Cuda::MemcpyToArray(this->Array, 0, 0, Other.GetData(), Other.GetNoBytes(), cudaMemcpyHostToDevice);
#endif

		return *this;
	}

	/*! Resize the buffer
		@param[in] Resolution Resolution of the buffer
	*/
	HOST void Resize(const Vec<int, 2>& Resolution)
	{
		if (this->Resolution == Resolution)
			return;
		else
			this->Free();
		
		this->Resolution = Resolution;
		
		const int NoElements = this->Resolution.CumulativeProduct();

		if (NoElements <= 0)
			throw (Exception(Enums::Error, "No. elements is zero!"));

#ifdef __CUDACC__
		Cuda::MallocArray(&this->Array, cudaCreateChannelDesc<T>(), Resolution);
#endif
	}
};

}
