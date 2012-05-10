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
#include "vtkErLocatable.h"

class vtkErLightData : public vtkDataObject, public vtkErBindableLight
{
public:
	static vtkErLightData* New();
	vtkTypeRevisionMacro(vtkErLightData, vtkDataObject);
	
protected:
	vtkErLightData() {};
	virtual ~vtkErLightData() {};

private:
	vtkErLightData(const vtkErLightData& Other);	// Not implemented.
    void operator = (const vtkErLightData& Other);	// Not implemented.
};

class VTK_ER_EXPORT vtkErLight : public vtkErLocatable
{
public:
	static vtkErLight* New();
	vtkTypeRevisionMacro(vtkErLight, vtkAlgorithm);

	virtual int ProcessRequest(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);

	vtkGetMacro(Visible, bool);
	vtkSetMacro(Visible, bool);

	vtkGetMacro(OneSided, bool);
	vtkSetMacro(OneSided, bool);

	vtkGetMacro(ShapeType, Enums::ShapeType);
	vtkSetMacro(ShapeType, Enums::ShapeType);

	vtkGetVector3Macro(Size, float);
	vtkSetVector3Macro(Size, float);
	
	vtkGetMacro(InnerRadius, float);
	vtkSetMacro(InnerRadius, float);

	vtkGetMacro(OuterRadius, float);
	vtkSetMacro(OuterRadius, float);

	vtkGetMacro(Multiplier, float);
	vtkSetMacro(Multiplier, float);

	vtkGetMacro(EmissionUnit, Enums::EmissionUnit);
	vtkSetMacro(EmissionUnit, Enums::EmissionUnit);

protected:
	vtkErLight();
	virtual ~vtkErLight();

	virtual int FillInputPortInformation(int Port, vtkInformation* Info);
	virtual int FillOutputPortInformation(int Port, vtkInformation* Info);

	virtual int RequestDataObject(vtkInformation* vtkNotUsed(Request), vtkInformationVector** vtkNotUsed(InputVector), vtkInformationVector* OutputVector);
	virtual int RequestInformation(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);
	virtual int RequestData(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);
	virtual int RequestUpdateExtent(vtkInformation* vtkNotUsed(Request), vtkInformationVector** InputVector, vtkInformationVector* vtkNotUsed(OutputVector));

private:
	vtkErLight(const vtkErLight& Other);		// Not implemented
    void operator = (const vtkErLight& Other);  // Not implemented

	bool					Visible;
	bool					OneSided;
	Enums::ShapeType		ShapeType;
	float					Size[3];
	float					InnerRadius;
	float					OuterRadius;
	float					Multiplier;
	Enums::EmissionUnit		EmissionUnit;
	int						TextureID;
};