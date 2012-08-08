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
#include "shape.h"

namespace ExposureRender
{

/*! Exposure Render host object class */
class EXPOSURE_RENDER_DLL HostObject : public HostBase
{
public:
	/*! Default constructor */
	HOST HostObject() :
	  	HostBase(),
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
	
	/*! Copy constructor
		@param[in] Other Host object to copy
	*/
	HOST HostObject(const HostObject& Other) :
	  	HostBase(),
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
	
	/*! Assignment operator
		@param[in] Other Host object to copy
		@return Copied host object
	*/
	HOST HostObject& operator = (const HostObject& Other)
	{
		HostBase::operator = (Other);

		this->Visible				= Other.Visible;
		this->Shape					= Other.Shape;
		this->DiffuseTextureID		= Other.DiffuseTextureID;
		this->SpecularTextureID		= Other.SpecularTextureID;
		this->GlossinessTextureID	= Other.GlossinessTextureID;
		this->EmissionTextureID		= Other.EmissionTextureID;
		this->Multiplier			= Other.Multiplier;
		this->EmissionUnit			= Other.EmissionUnit;
		this->Clip					= Other.Clip;

		this->Shape.Update();

		return *this;
	}

	GET_SET_MACRO(HOST, Shape, Shape)
	GET_REF_MACRO(HOST, Shape, Shape)
	GET_SET_MACRO(HOST, Visible, bool)
	GET_SET_MACRO(HOST, DiffuseTextureID, int)
	GET_SET_MACRO(HOST, SpecularTextureID, int)
	GET_SET_MACRO(HOST, GlossinessTextureID, int)
	GET_SET_MACRO(HOST, EmissionTextureID, int)
	GET_SET_MACRO(HOST, IndexOfReflection, float)
	GET_SET_MACRO(HOST, Emitter, bool)
	GET_SET_MACRO(HOST, Multiplier, float)
	GET_SET_MACRO(HOST, EmissionUnit, Enums::EmissionUnit)
	GET_SET_MACRO(HOST, Clip, bool)

protected:
	Shape					Shape;						/*! Object shape */
	bool					Visible;					/*! Determines object visibility */
	int						DiffuseTextureID;			/*! Diffuse texture ID */
	int						SpecularTextureID;			/*! Specular texture ID */
	int						GlossinessTextureID;		/*! Glossiness texture ID */
	int						EmissionTextureID;			/*! Emission texture ID */
	float					IndexOfReflection;			/*! Index of reflection */
	bool					Emitter;					/*! Determines if the object acts as an emitter */
	float					Multiplier;					/*! Emitter multiplier */
	Enums::EmissionUnit		EmissionUnit;				/*! Emission unit */
	bool					Clip;						/*! Determines whether the object acts as a clipping object */
};

}
