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

namespace ExposureRender
{

/*! \class Piecewise function node
 * \brief Piecewise function node class
 */
template<class T>
class EXPOSURE_RENDER_DLL PiecewiseFunctionNode
{
public:
	/*! Default constructor */
	HOST_DEVICE PiecewiseFunctionNode() :
		Position(),
		Value()
	{
	}

	/*! Constructor with \a Position and \a Value
		@param[in] Position Node position
		@param[in] Value Node value
	*/
	HOST_DEVICE PiecewiseFunctionNode(const float& Position, const T& Value) :
		Position(Position),
		Value(Value)
	{
	}
	
	/*! Destructor */
	HOST_DEVICE ~PiecewiseFunctionNode()
	{
	}
	
	/*! Copy constructor
		@param[in] Other Piecewise function node to copy
	*/
	HOST_DEVICE PiecewiseFunctionNode(const PiecewiseFunctionNode& Other)
	{
		*this = Other;
	}
	
	/*! Assignment operator
		@param[in] Other Piecewise function node to copy
		@result Reference to piecewise function node
	*/
	HOST_DEVICE PiecewiseFunctionNode& operator = (const PiecewiseFunctionNode& Other)
	{
		this->Position	= Other.Position;
		this->Value		= Other.Value;

		return *this;
	}
	
	/*! Resets the content of the piecewise function node */
	HOST_DEVICE void Reset()
	{
		this->Position	= 0.0f;
		this->Value		= T();
	}

	/*! Gets the node position
		@result Node position
	*/
	HOST_DEVICE float GetPosition() const
	{
		return this->Position;
	}

	/*! Sets the node position
		@param[in] Position Node position
	*/
	HOST_DEVICE void SetPosition(const float& Position)
	{
		this->Position = Position;
	}

	/*! Gets the node value
		@result Node value
	*/
	HOST_DEVICE T GetValue() const
	{
		return this->Value;
	}

	/*! Sets the node value
		@param[in] Value Node value
	*/
	HOST_DEVICE void SetValue(const T& Value)
	{
		this->Value = Value;
	}

protected:
	float	Position;	/*! Node position */
	T		Value;		/*! Node value */
};

}
