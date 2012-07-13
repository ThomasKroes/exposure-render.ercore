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

#include "buffers.h"

namespace ExposureRender
{

/*! \class Cuda texture
 * \brief Cuda texture template base class
 */
template<class T, int NoDimensions = 1>
class EXPOSURE_RENDER_DLL CudaTexture
{
public:
	/*! Constructor
		@param[in] Normalized Normalized element access
		@param[in] FilterMode Type of filtering
		@param[in] AddressMode Type of addressing near edges
	*/
	HOST CudaTexture(const bool& Normalized = true, const Enums::FilterMode& FilterMode = Enums::Linear, const Enums::AddressMode& AddressMode = Enums::Clamp) :
		Resolution(),
		Array(NULL),
		Data(NULL),
		Normalized(Normalized),
		FilterMode(FilterMode),
		AddressMode(AddressMode)
	{
	}
	
	/*! Destructor */
	HOST virtual ~CudaTexture(void)
	{
		this->Free();
	}
	
	/*! Assignment operator
		@param[in] Other Cuda texture to copy from
		@result Copied cuda texture by reference
	*/
	HOST CudaTexture& operator = (const CudaTexture& Other)
	{
		throw (Exception(Enums::Error, "Not implemented yet!"));
	}
	
	/*! Free dynamic data owned by the texture */
	HOST void Free(void)
	{
#ifdef __CUDACC__
		Cuda::FreeArray(this->Array);
#endif
	}
	
	/*! Binds the texture to the data
		@param[in] TextureReference Texture reference to bind to
	*/
	HOST void Bind(textureReference& TextureReference)
	{
		if (this->Resolution.CumulativeProduct() <= 0)
			return;

		if (this->Array == NULL)
			return;

		TextureReference.normalized		= this->Normalized;
		TextureReference.filterMode		= (cudaTextureFilterMode)this->FilterMode;
		TextureReference.addressMode[0]	= (cudaTextureAddressMode)this->AddressMode;
		TextureReference.addressMode[1]	= (cudaTextureAddressMode)this->AddressMode;
		TextureReference.addressMode[2]	= (cudaTextureAddressMode)this->AddressMode;

		const cudaChannelFormatDesc ChannelFormatDescription = cudaCreateChannelDesc<T>();

#ifdef __CUDACC__
		Cuda::BindTextureToArray(&TextureReference, this->Array, &ChannelFormatDescription);
#endif
	}

	/*! Gets the resolution
		@result Resolution
	*/
	HOST_DEVICE Vec<int, NoDimensions> GetResolution() const
	{
		return this->Resolution;
	}

protected:
	Vec<int, NoDimensions>	Resolution;			/*! Texture resolution */
	cudaArray*				Array;				/*! Cuda array, in case of pitched texture memory */
	T*						Data;				/*! Data array, in case of linear memory */
	bool					Normalized;			/*! Whether texture access is in normalized texture coordinates */
	Enums::FilterMode		FilterMode;			/*! Type of filtering  */
	Enums::AddressMode		AddressMode;		/*! Type of addressing  */
			
};

}
