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

#include "geometry.h"
#include "wrapper.cuh"

using namespace ExposureRender::Enums;

namespace ExposureRender
{

template<class T>
class EXPOSURE_RENDER_DLL Buffer
{
public:
	HOST Buffer(const char* pName = "Untitled", const Enums::MemoryType& MemoryType = Host, const Enums::DeviceType DeviceType = Cuda) :
		MemoryType(MemoryType),
		DeviceType(DeviceType),
		Data(NULL),
		NoElements(0),
		Dirty(false)
	{
		this->SetName(pName);
	}

	HOST Buffer(const Buffer& Other)
	{
		this->SetName(Other.GetName());
	}

	HOST const char* GetName() const
	{
		return this->Name;
	}

	HOST void SetName(const char* pName)
	{
		sprintf_s(this->Name, MAX_CHAR_SIZE, "%s", pName);
		this->UpdateFullName();
	}
	
	HOST const char* GetFullName() const
	{
		return this->FullName;
	}

	HOST void UpdateFullName()
	{
		char MemoryTypeName[MAX_CHAR_SIZE];

		switch (this->MemoryType)
		{
			case Host:
				sprintf_s(MemoryTypeName, MAX_CHAR_SIZE, "%s", "H");
				break;
			
			case Device:
				sprintf_s(MemoryTypeName, MAX_CHAR_SIZE, "%s", "D");
				break;

			default:
				sprintf_s(MemoryTypeName, MAX_CHAR_SIZE, "%s", "U");
				break;
		}

		sprintf_s(this->FullName, MAX_CHAR_SIZE, "['%s', %s]", this->Name, MemoryTypeName);
	}

	HOST virtual int GetNoBytes() const
	{
		return 0;
	}

	HOST virtual float GetMemorySize(const MemoryUnit& MemoryUnit) const
	{
		switch (MemoryUnit)
		{
			case KiloByte:
				return (float)this->GetNoBytes() / (1024.0f);

			case MegaByte:
				return (float)this->GetNoBytes() / (1024.0f * 1024.0f);
			
			case GigaByte:
				return (float)this->GetNoBytes() / (1024.0f * 1024.0f * 1024.0f);
		}

		return 0.0f;
	}

	HOST virtual void GetMemoryString(char* pMemoryString, const MemoryUnit& MemoryUnit = MegaByte) const
	{
		switch (MemoryUnit)
		{
			case KiloByte:	sprintf_s(pMemoryString, MAX_CHAR_SIZE, "%0.2f KB", this->GetMemorySize(KiloByte));		break;
			case MegaByte:	sprintf_s(pMemoryString, MAX_CHAR_SIZE, "%0.2f MB", this->GetMemorySize(MegaByte));		break;
			case GigaByte:	sprintf_s(pMemoryString, MAX_CHAR_SIZE, "%0.2f GB", this->GetMemorySize(GigaByte));		break;
		}
	}

	MemoryType		MemoryType;
	DeviceType		DeviceType;
	char			Name[MAX_CHAR_SIZE];
	char			FullName[MAX_CHAR_SIZE];
	T*				Data;
	int				NoElements;
	mutable bool	Dirty;
};

}
