/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "vtkErDll.h"
#include "vtkErBindable.h"

class vtkErObjectData : public vtkDataObject, public vtkErBindableObject
{
public:
	static vtkErObjectData* New();
	vtkTypeRevisionMacro(vtkErObjectData, vtkDataObject);
	
protected:
	vtkErObjectData() {};
	virtual ~vtkErObjectData() {};

private:
	vtkErObjectData(const vtkErObjectData& Other);		// Not implemented.
    void operator = (const vtkErObjectData& Other);		// Not implemented.
};

class VTK_ER_EXPORT vtkErObject : public vtkAlgorithm
{
public:
	static vtkErObject* New();
	vtkTypeRevisionMacro(vtkErObject, vtkAlgorithm);

	virtual int ProcessRequest(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);

protected:
	vtkErObject();
	virtual ~vtkErObject();

	virtual int FillInputPortInformation(int Port, vtkInformation* Info);
	virtual int FillOutputPortInformation(int Port, vtkInformation* Info);

	virtual int RequestDataObject(vtkInformation* vtkNotUsed(Request), vtkInformationVector** vtkNotUsed(InputVector), vtkInformationVector* OutputVector);
	virtual int RequestInformation(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);
	virtual int RequestData(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);
	virtual int RequestUpdateExtent(vtkInformation* vtkNotUsed(Request), vtkInformationVector** InputVector, vtkInformationVector* vtkNotUsed(OutputVector));

private:
	vtkErObject(const vtkErObject& Other);			// Not implemented
    void operator = (const vtkErObject& Other);		// Not implemented
};