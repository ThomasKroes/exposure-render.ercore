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

#include "erobject.h"

namespace ExposureRender
{

class Object : public TimeStamp
{
public:
	HOST Object() :
		TimeStamp(),
		Visible(true),
		Shape(),
		DiffuseTextureID(-1),
		SpecularTextureID(-1),
		GlossinessTextureID(-1),
		EmissionTextureID(-1),
		Emitter(false),
		Multiplier(1.0f),
		EmissionUnit(Enums::Power),
		Clip(false)
	{
	}

	HOST Object(const ErObject& Other) :
		TimeStamp(),
		Visible(true),
		Shape(),
		DiffuseTextureID(-1),
		SpecularTextureID(-1),
		GlossinessTextureID(-1),
		EmissionTextureID(-1),
		Emitter(false),
		Multiplier(1.0f),
		EmissionUnit(Enums::Power),
		Clip(false)
	{
		*this = Other;
	}

	HOST Object& operator = (const ErObject& Other)
	{
		TimeStamp::operator = (Other);

		this->Visible	= Other.Visible;
		this->Shape		= Other.Shape;

		if (Other.DiffuseTextureID >= 0)
		{
			if (gTexturesHashMap.find(Other.DiffuseTextureID) != gTexturesHashMap.end())
				this->DiffuseTextureID = gTexturesHashMap[Other.DiffuseTextureID];
			else
				throw(Exception(Enums::Fatal, "Diffuse texture not found!"));
		}
		else
		{
			this->DiffuseTextureID = -1;
		}

		if (Other.SpecularTextureID >= 0)
		{
			if (gTexturesHashMap.find(Other.SpecularTextureID) != gTexturesHashMap.end())
				this->SpecularTextureID = gTexturesHashMap[Other.SpecularTextureID];
			else
				throw(Exception(Enums::Fatal, "Specular texture not found!"));
		}
		else
		{
			this->SpecularTextureID = -1;
		}

		if (Other.GlossinessTextureID >= 0)
		{
			if (gTexturesHashMap.find(Other.GlossinessTextureID) != gTexturesHashMap.end())
				this->GlossinessTextureID = gTexturesHashMap[Other.GlossinessTextureID];
			else
				throw(Exception(Enums::Fatal, "Glossiness texture not found!"));
		}
		else
		{
			this->GlossinessTextureID = -1;
		}

		if (Other.EmissionTextureID >= 0)
		{
			if (gTexturesHashMap.find(Other.EmissionTextureID ) != gTexturesHashMap.end())
				this->EmissionTextureID  = gTexturesHashMap[Other.EmissionTextureID ];
			else
				throw(Exception(Enums::Fatal, "Emission texture not found!"));
		}
		else
		{
			this->EmissionTextureID = -1;
		}

		this->Emitter		= Other.Emitter;
		this->Multiplier	= Other.Multiplier;
		this->EmissionUnit	= Other.EmissionUnit;
		this->Clip			= Other.Clip;

		return *this;
	}

	bool					Visible;
	Shape					Shape;
	int						DiffuseTextureID;
	int						SpecularTextureID;
	int						GlossinessTextureID;
	int						EmissionTextureID;
	bool					Emitter;
	float					Multiplier;
	Enums::EmissionUnit		EmissionUnit;
	bool					Clip;
};

}
