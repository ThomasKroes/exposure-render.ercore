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

#include "piecewiselinearfunction.h"

namespace ExposureRender
{

/*! \class Transfer function
 * \brief Base buffer class
 */
template<class T, int NoDimensions = 1>
class EXPOSURE_RENDER_DLL TransferFunction
{
public:
	/*! Constructor
		@param[in] pName Buffer name
		@param[in] MemoryType Place where the memory resides, can be host or device
		@param[in] FilterMode Type of filtering
		@param[in] AddressMode Type of addressing near edges
	*/
	HOST TransferFunction(const char* pName = "Untitled")
	{
		this->SetName(pName);
	}
	
	/*! Copy constructor */
	HOST TransferFunction(const Buffer& Other)
	{
		*this = Other;
	}

	/*! Destructor */
	HOST virtual ~TransferFunction(void)
	{
		this->Free();
	}
	
	/*! Assignment operator
		@param[in] Other Buffer to copy from
		@result Copied buffer by reference
	*/
	HOST TransferFunction& operator = (const TransferFunction& Other)
	{
		this->SetName(Other.GetName());

		this->FilterMode	= Other.FilterMode;
		this->AddressMode	= Other.AddressMode;
		
		if (this->TimeStamp != Other.TimeStamp)
		{
			this->Set(Other.MemoryType, Other.Resolution, Other.Data);
			this->TimeStamp = Other.TimeStamp;
		}

		return *this;
	}
	
	/*! Frees the memory owned by the buffer */
	HOST void Free(void)
	{
		this->TimeStamp.Modified();
	}
	
	/*! Resets the memory owned by the buffer */
	HOST void Reset(void)
	{
		this->TimeStamp.Modified();
	}
	
	/*! Gets the buffer name
		@result Name of the buffer
	*/
	HOST const char* GetName() const
	{
		return this->Name;
	}

	/*! Sets the buffer name
		@param[in] pName Name of name
		@result Size of the memory
	*/
	HOST void SetName(const char* pName)
	{
		sprintf_s(this->Name, MAX_CHAR_SIZE, "%s", pName);
		this->UpdateFullName();
	}
	
	/*! Gets the full name
		@result Full name
	*/
	HOST const char* GetFullName() const
	{
		return this->FullName;
	}
	
protected:
	char										Name[MAX_CHAR_SIZE];			/*! Name  */
	char										FullName[MAX_CHAR_SIZE];		/*! Full buffer name  */
	PiecewiseLinearFunction<MAX_NO_TF_NODES>	PLF;							/*! Full buffer name  */
	CudaTexture1D<T>							Texture1D;
	CudaTexture2D<T>							Texture2D;
	CudaTexture3D<t>							Texture3D;
};

}
