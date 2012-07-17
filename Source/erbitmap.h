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

#include "erbindable.h"
#include "color.h"
#include "buffer2d.h"

namespace ExposureRender
{

/*! Exposure Render API bitmap class */
class EXPOSURE_RENDER_DLL ErBitmap : public ErBindable
{
public:
	/*! Default constructor */
	HOST ErBitmap() :
		ErBindable(),
		Pixels("Host Pixels", Enums::Host)
	{
	}
	
	/*! Copy constructor
		@param[in] Other Bitmap to copy
	*/
	HOST ErBitmap(const ErBitmap& Other) :
		ErBindable(),
		Pixels("Host Pixels", Enums::Host)
	{
		*this = Other;
	}
	
	/*! Assignment operator
		@param[in] Other Bitmap to copy
		@return Copied bitmap
	*/
	HOST ErBitmap& operator = (const ErBitmap& Other)
	{
		ErBindable::operator = (Other);
		
		this->Pixels = Other.Pixels;
		
		return *this;
	}
	
	/*! Binds pixels to the bitmap
		@param[in] Resolution Resolution of the bitmap
		@param[in] Pixels Pointer to RGBA pixels
	*/
	HOST void BindPixels(const Vec2i& Resolution, ColorRGBAuc* Pixels)
	{
		this->Pixels.Set(Enums::Host, Resolution, Pixels);
	}

	GET_MACRO(HOST, Pixels, Buffer2D<ColorRGBAuc>)
	GET_REF_MACRO(HOST, Pixels, Buffer2D<ColorRGBAuc>)

protected:
	Buffer2D<ColorRGBAuc>	Pixels;		/*! Pixel buffer */
};

}
