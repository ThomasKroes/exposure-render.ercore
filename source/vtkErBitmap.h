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

#include "vtkErDll.h"
#include "vtkErBindable.h"

#include <vtkDataObject.h>
#include <vtkAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkColorTransferFunction.h>

class vtkErBitmapData : public vtkDataObject, public vtkHostBindableBitmap
{
public:
	static vtkErBitmapData* New();
	vtkTypeRevisionMacro(vtkErBitmapData, vtkDataObject);
	
protected:
	vtkErBitmapData() {};
	virtual ~vtkErBitmapData() {};

private:
	vtkErBitmapData(const vtkErBitmapData& Other);		// Not implemented.
    void operator = (const vtkErBitmapData& Other);		// Not implemented.
};

class VTK_ER_EXPORT vtkErBitmap : public vtkAlgorithm
{
public:
	static vtkErBitmap* New();
	vtkTypeRevisionMacro(vtkErBitmap, vtkAlgorithm);

	enum Ports
	{
		ImageDataPort = 0
	};

	vtkGetMacro(FilterMode, Enums::FilterMode);
	vtkSetMacro(FilterMode, Enums::FilterMode);

	virtual int ProcessRequest(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);

	vtkGetMacro(Name, vtkStdString);
	vtkSetMacro(Name, vtkStdString);

	virtual int FillInputPortInformation(int Port, vtkInformation* Info);
	virtual int FillOutputPortInformation(int Port, vtkInformation* Info);

	virtual int RequestDataObject(vtkInformation* vtkNotUsed(Request), vtkInformationVector** vtkNotUsed(InputVector), vtkInformationVector* OutputVector);
	virtual int RequestInformation(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);
	virtual int RequestData(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);
	virtual int RequestUpdateExtent(vtkInformation* vtkNotUsed(Request), vtkInformationVector** InputVector, vtkInformationVector* vtkNotUsed(OutputVector));

protected:
	vtkErBitmap();
	virtual ~vtkErBitmap();

	Enums::FilterMode			FilterMode;

private:
	vtkErBitmap(const vtkErBitmap& Other);			// Not implemented
    void operator = (const vtkErBitmap& Other);		// Not implemented

	vtkStdString			Name;
};