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

#include "vector.h"

#include "wrapper.cuh"

namespace ExposureRender
{

/*! \class Buffer
 * \brief Base buffer class
 */
template<class T, int NoDimensions = 1>
class EXPOSURE_RENDER_DLL Buffer
{
public:
	/*! Constructor
		@param[in] pName Buffer name
		@param[in] MemoryType Place where the memory resides, can be host or device
		@param[in] FilterMode Type of filtering
		@param[in] AddressMode Type of addressing near edges
	*/
	HOST Buffer(const char* pName = "Untitled", const Enums::MemoryType MemoryType = Enums::Host, Enums::FilterMode FilterMode = Enums::Linear, Enums::AddressMode AddressMode = Enums::Wrap) :
		MemoryType(MemoryType),
		FilterMode(FilterMode),
		AddressMode(AddressMode),
		Data(NULL),
		Resolution()
	{
		this->SetName(pName);
	}
	
	/*! Copy constructor */
	HOST Buffer(const Buffer& Other)
	{
		*this = Other;
	}

	/*! Destructor */
	HOST virtual ~Buffer(void)
	{
		this->Free();
	}
	
	/*! Assignment operator
		@param[in] Other Buffer to copy from
		* \return Copied buffer by reference
	*/
	HOST Buffer& operator = (const Buffer& Other)
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
		if (this->Data)
		{
			switch (this->MemoryType)
			{
				case Enums::Host:
				{
					free(this->Data);
					this->Data = NULL;
					break;
				}

				case Enums::Device:
				{
#ifdef __CUDACC__
					Cuda::Free(this->Data);
#endif
					break;
				}
			}
		}

		this->Resolution = Vec<int, NoDimensions>();

		this->TimeStamp.Modified();
	}
	
	/*! Resets the memory owned by the buffer */
	HOST void Reset(void)
	{
		if (this->Resolution.CumulativeProduct() <= 0)
			return;

		switch (this->MemoryType)
		{
			case Enums::Host:
			{
				memset(this->Data, 0, this->GetNoBytes());
				break;
			}
			
			case Enums::Device:
			{
#ifdef __CUDACC__
				Cuda::MemSet(this->Data, 0, this->Resolution.CumulativeProduct());
#endif
				break;
			}
		}

		this->TimeStamp.Modified();
	}
	
	/*! Resize the buffer
		@param[in] Resolution Resolution of the buffer
	*/
	HOST void Resize(const Vec<int, NoDimensions>& Resolution)
	{
		if (this->Resolution == Resolution)
			return;
		else
			this->Free();

		this->Resolution = Resolution;

		if (this->Resolution.CumulativeProduct() <= 0)
			return;

		if (this->MemoryType == Enums::Host)
		{
			this->Data = (T*)malloc(this->GetNoBytes());
		}

#ifdef __CUDACC__
		if (this->MemoryType == Enums::Device)
		{
			Cuda::Allocate(this->Data, this->Resolution.CumulativeProduct());
		}
#endif

		this->Reset();
	}

	/*! Set the buffer
		@param[in] MemoryType Type of memory, host/device
		@param[in] Resolution Resolution of the buffer
		@param[in] Data Pointer to raw data
	*/
	HOST void Set(const Enums::MemoryType& MemoryType, const Vec<int, NoDimensions>& Resolution, T* Data)
	{
		this->Resize(Resolution);

		if (this->Resolution.CumulativeProduct() <= 0)
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
					
					case Enums::Device:
					{
#ifdef __CUDACC__
						Cuda::MemCopyDeviceToHost(Data, this->Data, this->Resolution.CumulativeProduct());
#endif
						break;
					}
				}

				break;
			}

			case Enums::Device:
			{
				switch (MemoryType)
				{
					case Enums::Host:
					{
#ifdef __CUDACC__
						Cuda::MemCopyHostToDevice(Data, this->Data, this->Resolution.CumulativeProduct());
#endif
						break;
					}

					case Enums::Device:
					{
#ifdef __CUDACC__
						Cuda::MemCopyDeviceToDevice(Data, this->Data, this->Resolution.CumulativeProduct());
#endif
						break;
					}
				}

				break;
			}
		}
	}

	/*! Get element at index \a ID
		@param[in] ID Index
		* \return Element at \a ID
	*/
	HOST_DEVICE T& operator[](const int& ID) const
	{
		return this->Data[Clamp(ID, 0, this->GetNoElements() - 1)];
	}

	/*! Gets the buffer name
		* \return Name of the buffer
	*/
	HOST const char* GetName() const
	{
		return this->Name;
	}

	/*! Sets the buffer name
		@param[in] pName Name of name
		* \return Size of the memory
	*/
	HOST void SetName(const char* pName)
	{
		sprintf_s(this->Name, MAX_CHAR_SIZE, "%s", pName);
		this->UpdateFullName();
	}
	
	/*! Gets the full name
		* \return Full name
	*/
	HOST const char* GetFullName() const
	{
		return this->FullName;
	}
	
	/*! Gets the number of bytes
		* \return Number of bytes occupied by the buffer
	*/
	HOST_DEVICE virtual long GetNoBytes(void) const
	{
		return this->Resolution.CumulativeProduct() * sizeof(T);
	} 
	
	/*! Gets the memory string
		@param[in] MemoryUnit Memory unit
		* \return Size of the memory
	*/
	HOST virtual float GetMemorySize(const Enums::MemoryUnit& MemoryUnit) const
	{
		switch (MemoryUnit)
		{
			case Enums::KiloByte:
				return (float)this->GetNoBytes() / (1024.0f);

			case Enums::MegaByte:
				return (float)this->GetNoBytes() / (1024.0f * 1024.0f);
			
			case Enums::GigaByte:
				return (float)this->GetNoBytes() / (1024.0f * 1024.0f * 1024.0f);
		}

		return 0.0f;
	}

	/*! Gets a pointer to the data
		* \return Pointer to raw data
	*/
	HOST_DEVICE T* GetData() const
	{
		return this->Data;
	}

	/*! Gets the memory type
		* \return Memory type
	*/
	HOST Enums::MemoryType GetMemoryType() const
	{
		return this->MemoryType;
	}

	/*! Gets the filter mode
		* \return Filter mode
	*/
	HOST Enums::FilterMode GetFilterMode() const
	{
		return this->FilterMode;
	}

	/*! Sets the filter mode
		@param[in] FilterMode FilterMode
	*/
	HOST void SetFilterMode(const Enums::FilterMode& FilterMode)
	{
		this->FilterMode = FilterMode;
	}

	/*! Gets the address mode 
		* \return Address mode
	*/
	HOST Enums::AddressMode GetAddressMode() const
	{
		return this->AddressMode;
	}

	/*! Flag the buffer as modified */
	void Modified()
	{
		TimeStamp.Modified();
	}

	/*! Gets the buffer's resolution 
		* \return Resolution
	*/
	HOST_DEVICE Vec<int, NoDimensions> GetResolution() const
	{
		return this->Resolution;
	}

	/*! Gets the number of elements in the buffer 
		* \return Number of elements
	*/
	HOST_DEVICE int GetNoElements() const
	{
		return this->Resolution.CumulativeProduct();
	}
	
	TimeStamp					TimeStamp;						/*! Time last modified */

protected:
	/*! Gets the memory string
		@param[out] pMemoryString Memory string output
		@param[in] MemoryUnit Memory unit
	*/
	HOST virtual void GetMemoryString(char* pMemoryString, const Enums::MemoryUnit& MemoryUnit = Enums::MegaByte) const
	{
		switch (MemoryUnit)
		{
			case Enums::KiloByte:	sprintf_s(pMemoryString, MAX_CHAR_SIZE, "%0.2f KB", this->GetMemorySize(Enums::KiloByte));		break;
			case Enums::MegaByte:	sprintf_s(pMemoryString, MAX_CHAR_SIZE, "%0.2f MB", this->GetMemorySize(Enums::MegaByte));		break;
			case Enums::GigaByte:	sprintf_s(pMemoryString, MAX_CHAR_SIZE, "%0.2f GB", this->GetMemorySize(Enums::GigaByte));		break;
		}
	}
	
	/*! Updates the full name */
	HOST void UpdateFullName()
	{
		char MemoryTypeName[MAX_CHAR_SIZE];

		switch (this->MemoryType)
		{
			case Enums::Host:
				sprintf_s(MemoryTypeName, MAX_CHAR_SIZE, "%s", "H");
				break;

			case Enums::Device:
				sprintf_s(MemoryTypeName, MAX_CHAR_SIZE, "%s", "D");
				break;

			default:
				sprintf_s(MemoryTypeName, MAX_CHAR_SIZE, "%s", "U");
				break;
		}

		sprintf_s(this->FullName, MAX_CHAR_SIZE, "['%s', %s]", this->Name, MemoryTypeName);
	}


	Enums::MemoryType			MemoryType;						/*! Type of memory e.g. host/device */
	Enums::FilterMode			FilterMode;						/*! Type of filtering  */
	Enums::AddressMode			AddressMode;					/*! Type of addressing  */
	char						Name[MAX_CHAR_SIZE];			/*! Buffer name  */
	char						FullName[MAX_CHAR_SIZE];		/*! Full buffer name  */
	T*							Data;							/*! Pointer to raw data on host/device */
	Vec<int, NoDimensions>		Resolution;						/*! Buffer resolution */
};

}
