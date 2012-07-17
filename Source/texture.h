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

#include "hosttexture.h"

namespace ExposureRender
{

/*! Texture class */
class Texture : public TimeStamp
{
public:
	/*! Default constructor */
	HOST Texture() :
		TimeStamp(),
		Type(Enums::Procedural),
		OutputLevel(1.0f),
		BitmapID(-1),
		Procedural(),
		Offset(0.0f),
		Repeat(0.0f),
		Flip(0)
	{
	}
	
	/*! Copy constructor
		@param[in] Other Texture to copy
	*/
	HOST Texture(const HostTexture& Other) :
		TimeStamp(),
		Type(Enums::Procedural),
		OutputLevel(1.0f),
		BitmapID(-1),
		Procedural(),
		Offset(0.0f),
		Repeat(0.0f),
		Flip(0)
	{
		*this = Other;
	}
	
	/*! Assignment operator
		@param[in] Other Texture to copy
		@return Copied texture
	*/
	HOST Texture& operator = (const HostTexture& Other)
	{
		TimeStamp::operator = (Other);

		this->Type			= Other.GetType();
		this->OutputLevel	= Other.GetOutputLevel();

		if (Other.GetBitmapID() >= 0)
		{
			if (gBitmapsHashMap.find(Other.GetBitmapID()) != gBitmapsHashMap.end())
				this->BitmapID = gBitmapsHashMap[Other.GetBitmapID()];
			else
				throw(Exception(Enums::Fatal, "Bitmap not found!"));
		}
		else
		{
			this->BitmapID = -1;
		}

		this->Procedural	= Other.GetProcedural();
		this->Offset		= Other.GetOffset();
		this->Repeat		= Other.GetRepeat();
		this->Flip			= Other.GetFlip();
		
		return *this;
	}

	Enums::TextureType		Type;				/*! Texture type */
	float					OutputLevel;		/*! Output level */
	int						BitmapID;			/*! Bitmap ID */
	Procedural				Procedural;			/*! Procedural */
	Vec2f					Offset;				/*! Offset */
	Vec2f					Repeat;				/*! Repeat */
	Vec2i					Flip;				/*! Flip */
};

}
