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

#include "hostbase.h"
#include "procedural.h"

namespace ExposureRender
{

/*! Exposure Render host texture class */
class EXPOSURE_RENDER_DLL HostTexture : public HostBase
{
public:
	/*! Default constructor */
	HOST HostTexture() :
		HostBase(),
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
		@param[in] Other Host texture to copy
	*/
	HOST HostTexture(const HostTexture& Other) :
		HostBase(),
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
		@param[in] Other Host texture to copy
		@return Copied host texture
	*/
	HOST HostTexture& operator = (const HostTexture& Other)
	{
		HostBase::operator = (Other);

		this->Type			= Other.Type;
		this->OutputLevel	= Other.OutputLevel;
		this->BitmapID		= Other.BitmapID;
		this->Procedural	= Other.Procedural;
		this->Offset		= Other.Offset;
		this->Repeat		= Other.Repeat;
		this->Flip			= Other.Flip;
		
		return *this;
	}
	
	GET_SET_MACRO(HOST, Type, Enums::TextureType)
	GET_SET_MACRO(HOST, OutputLevel, float)
	GET_SET_MACRO(HOST, BitmapID, int)
	GET_MACRO(HOST, Procedural, Procedural)
	GET_REF_MACRO(HOST, Procedural, Procedural)
	SET_MACRO(HOST, Procedural, Procedural)
	GET_SET_MACRO(HOST, Offset, Vec2f)
	GET_SET_MACRO(HOST, Repeat, Vec2f)
	GET_SET_MACRO(HOST, Flip, Vec2i)

protected:
	Enums::TextureType		Type;				/*! Texture type */
	float					OutputLevel;		/*! Output level */
	int						BitmapID;			/*! Bitmap ID */
	Procedural				Procedural;			/*! Procedural */
	Vec2f					Offset;				/*! UV offset coordinates */
	Vec2f					Repeat;				/*! UV repeat coordinates */
	Vec2i					Flip;				/*! UV flip coordinates */
};

}
