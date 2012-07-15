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

#include "timestamp.h"

namespace ExposureRender
{

/*! \class Transfer function
 * \brief Base transfer function class
 */
class EXPOSURE_RENDER_DLL TransferFunction : public TimeStamp
{
public:
	/*! Constructor
		@param[in] Name Name
	*/
	HOST_DEVICE TransferFunction(const char* Name) :
		TimeStamp()
	{
		this->SetName(Name);
	}
	
	/*! Copy constructor */
	HOST_DEVICE TransferFunction(const TransferFunction& Other)
	{
		*this = Other;
	}

	/*! Destructor */
	HOST_DEVICE virtual ~TransferFunction(void)
	{
	}
	
	/*! Assignment operator
		@param[in] Other Transfer function to copy
		@result Reference to the copied transfer function
	*/
	HOST_DEVICE TransferFunction& operator = (const TransferFunction& Other)
	{
		TimeStamp::operator = (Other);

		this->SetName(Other.GetName());
		
		return *this;
	}
	
	/*! Gets the name of the transfer function
		@result Name of the transfer function
	*/
	HOST_DEVICE const char* GetName() const
	{
		return this->Name;
	}

	/*! Sets the transfer function name
		@param[in] pName Name of the transfer function
	*/
	HOST_DEVICE void SetName(const char* Name)
	{
#ifndef __CUDACC__
		sprintf_s(this->Name, MAX_CHAR_SIZE, "%s", Name);
#endif
	}
	
protected:
	char	Name[MAX_CHAR_SIZE];	/*! Name */
};

}
