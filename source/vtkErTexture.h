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

#include "kelvin.h"

class vtkErTextureData : public vtkDataObject, public vtkHostBindableTexture
{
public:
	static vtkErTextureData* New();
	vtkTypeRevisionMacro(vtkErTextureData, vtkDataObject);
	
protected:
	vtkErTextureData() {};
	virtual ~vtkErTextureData() {};

private:
	vtkErTextureData(const vtkErTextureData& Other);		// Not implemented.
    void operator = (const vtkErTextureData& Other);		// Not implemented.
};

class VTK_ER_EXPORT vtkErTexture : public vtkAlgorithm
{
public:
	static vtkErTexture* New();
	vtkTypeRevisionMacro(vtkErTexture, vtkAlgorithm);

	enum Ports
	{
		ImageDataPort = 0
	};

	virtual int ProcessRequest(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);

	vtkGetMacro(Name, vtkStdString);
	vtkSetMacro(Name, vtkStdString);

	vtkGetMacro(TextureType, Enums::TextureType);
	vtkSetMacro(TextureType, Enums::TextureType);

	vtkGetMacro(OutputLevel, float);
	vtkSetMacro(OutputLevel, float);

	vtkGetMacro(ProceduralType, Enums::ProceduralType);
	vtkSetMacro(ProceduralType, Enums::ProceduralType);

	vtkGetVector3Macro(UniformColor, float);
	vtkSetVector3Macro(UniformColor, float);
	void SetUniformColor(const float& Temperature) { const ColorRGBf RGB = ExposureRender::KelvinToColorRGBf(Temperature) / 255.0f; this->SetUniformColor(RGB[0], RGB[1], RGB[2]); }

	vtkGetVector3Macro(CheckerColor1, float);
	vtkSetVector3Macro(CheckerColor1, float);
	void SetCheckerColor1(const float& Temperature) { const ColorRGBf RGB = ExposureRender::KelvinToColorRGBf(Temperature) / 255.0f; this->SetCheckerColor1(RGB[0], RGB[1], RGB[2]); }

	vtkGetVector3Macro(CheckerColor2, float);
	vtkSetVector3Macro(CheckerColor2, float);
	void SetCheckerColor2(const float& Temperature) { const ColorRGBf RGB = ExposureRender::KelvinToColorRGBf(Temperature) / 255.0f; this->SetCheckerColor2(RGB[0], RGB[1], RGB[2]); }

	vtkColorTransferFunction* GetGradient() { return this->Gradient; }
	void SetGradient(vtkColorTransferFunction* Gradient) { this->Gradient->DeepCopy(Gradient); }

	vtkGetVector2Macro(Offset, float);
	vtkSetVector2Macro(Offset, float);

	vtkGetVector2Macro(Repeat, float);
	vtkSetVector2Macro(Repeat, float);

	vtkGetVector2Macro(Flip, int);
	vtkSetVector2Macro(Flip, int);

protected:
	vtkErTexture();
	virtual ~vtkErTexture();

	virtual int FillInputPortInformation(int Port, vtkInformation* Info);
	virtual int FillOutputPortInformation(int Port, vtkInformation* Info);

	virtual int RequestDataObject(vtkInformation* vtkNotUsed(Request), vtkInformationVector** vtkNotUsed(InputVector), vtkInformationVector* OutputVector);
	virtual int RequestInformation(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);
	virtual int RequestData(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector);
	virtual int RequestUpdateExtent(vtkInformation* vtkNotUsed(Request), vtkInformationVector** InputVector, vtkInformationVector* vtkNotUsed(OutputVector));

private:
	vtkErTexture(const vtkErTexture& Other);			// Not implemented
    void operator = (const vtkErTexture& Other);		// Not implemented

	vtkStdString								Name;
	Enums::TextureType							TextureType;
	float										OutputLevel;
	Enums::ProceduralType						ProceduralType;
	float										UniformColor[3];
	float										CheckerColor1[3];
	float										CheckerColor2[3];
	vtkSmartPointer<vtkColorTransferFunction>	Gradient;
	float										Offset[2];
	float										Repeat[2];
	int											Flip[2];
	int											BitmapID;
};