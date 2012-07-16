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

#include "piecewisefunctionnode.h"
#include "timestamp.h"

namespace ExposureRender
{

/*! \class Piecewise function
 * \brief Piecewise function base template class
 */
template<class T, int Size>
class EXPOSURE_RENDER_DLL PiecewiseFunction : public TimeStamp
{
public:
	/*! Constructor
		@param[in] Name Name
	*/
	HOST_DEVICE PiecewiseFunction(const char* Name = "Untitled") :
		TimeStamp(),
		NodeRange(FLT_MAX, FLT_MIN),
		Nodes(),
		Count(0)
	{
		this->SetName(Name);
	}
	
	/*! Destructor */
	HOST_DEVICE ~PiecewiseFunction()
	{
	}
	
	/*! Copy constructor
		@param[in] Other Piecewise function to copy
	*/
	HOST_DEVICE PiecewiseFunction(const PiecewiseFunction& Other)
	{
		*this = Other;
	}
	
	/*! Assignment operator
		@param[in] Other Piecewise function to copy
		@return Reference to copied piecewise function
	*/
	HOST_DEVICE PiecewiseFunction& operator = (const PiecewiseFunction& Other)
	{
		TimeStamp::operator = (Other);

		this->NodeRange		= Other.NodeRange;
		this->Nodes			= Other.Nodes;
		this->Count			= Other.Count;

		return *this;
	}
	
	/*! Resets the content of the piecewise function */
	HOST_DEVICE void Reset()
	{
		this->NodeRange	= Vec2f(FLT_MAX, FLT_MIN);
		this->Nodes		= Vec<PiecewiseFunctionNode<T>, 256>();
		this->Count		= 0;
	}

	/*! Gets the name of the transfer function
		@return Name of the transfer function
	*/
	HOST_DEVICE const char* GetName() const
	{
		return this->Name;
	}

	/*! Sets the transfer function name
		@param[in] pName Name of the transfer function
	*/
	HOST_DEVICE void SetName(const char* pName)
	{
#ifndef __CUDACC__
		sprintf_s(this->Name, MAX_CHAR_SIZE, "%s", pName);
#endif
	}

protected:
	char									Name[MAX_CHAR_SIZE];	/*! Name */
	Vec2f									NodeRange;				/*! Range of the nodes */
	Vec<PiecewiseFunctionNode<T>, 256>		Nodes;					/*! Nodes vector */
	int										Count;					/*! Number of active nodes */
};

}
