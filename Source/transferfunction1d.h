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

#include "transferfunction.h"
#include "piecewiselinearfunction.h"
#include "cudatexture1d.h"
#include "color.h"

namespace ExposureRender
{

/*! \class TransferFunction1D
 * \brief One-dimensional transfer function base template class
 */
template<class T>
class EXPOSURE_RENDER_DLL TransferFunction1D : public TransferFunction
{
public:
	/*! Default constructor */
	HOST TransferFunction1D() :
		TransferFunction("Untitled"),
		PLF("Untitled")
	{
	}

	/*! Constructor
		@param[in] pName Name
	*/
	HOST TransferFunction1D(const char* pName) :
		TransferFunction(pName),
		PLF(pName)
	{
	}
	
	/*! Copy constructor */
	HOST TransferFunction1D(const TransferFunction1D& Other) :
		TransferFunction("Untitled"),
		PLF("Untitled")
	{
		*this = Other;
	}

	/*! Destructor */
	HOST virtual ~TransferFunction1D(void)
	{
	}
	
	/*! Assignment operator
		@param[in] Other Transfer function to copy
		@result Reference to the copied transfer function
	*/
	HOST TransferFunction1D& operator = (const TransferFunction1D& Other)
	{
		TransferFunction::operator = (Other);

		this->PLF		= Other.PLF;
//		this->Texture	= Other.Texture;

		return *this;
	}

	/*! Adds a node with \a Position and \a Value
		@param[in] Position Position of the node
		@param[in] Value Value of the node
	*/
	HOST void AddNode(const float& Position, const T& Value)
	{
		this->PLF.AddNode(Position, Value);
	}

	/*! Resets the content of the piecewise linear function */
	HOST void Reset()
	{
		this->PLF.Reset();
	}

	/*! Evaluates the transfer function at \a Position
		@param[in] Position Position to evaluate
		@result Value at \a Position
	*/
	HOST_DEVICE T Evaluate(const float& Position) const
	{
		return this->PLF.Evaluate(Position);
	}

protected:
	PiecewiseLinearFunction<T>		PLF;		/*! Piecewise linear function */
	CudaTexture1D<T>				Texture;	/*! One-dimensional CUDA texture */
};

typedef TransferFunction1D<float>		ScalarTransferFunction1D;
typedef TransferFunction1D<ColorXYZf>	ColorTransferFunction1D;

}
