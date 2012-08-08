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

#include <vtkOpenGLCamera.h>

class vtkRenderer;

using namespace ExposureRender;

class VTK_ER_EXPORT vtkErCamera : public vtkOpenGLCamera
{
public:
	static vtkErCamera *New();
	vtkTypeRevisionMacro(vtkErCamera, vtkOpenGLCamera);

	void RequestData(ExposureRender::Camera& Camera);

	vtkGetMacro(Renderer, vtkRenderer*);
	vtkSetMacro(Renderer, vtkRenderer*);

	vtkGetMacro(FocalDistance, float);
	vtkSetMacro(FocalDistance, float);
	
	vtkGetMacro(Exposure, float);
	vtkSetMacro(Exposure, float);

	vtkGetMacro(Gamma, float);
	vtkSetMacro(Gamma, float);

	vtkGetMacro(ApertureShape, Enums::ApertureShape);
	vtkSetMacro(ApertureShape, Enums::ApertureShape);

	vtkGetMacro(ApertureSize, float);
	vtkSetMacro(ApertureSize, float);
	
	vtkGetMacro(NoApertureBlades, int);
	vtkSetMacro(NoApertureBlades, int);

	vtkGetMacro(ApertureAngle, float);
	vtkSetMacro(ApertureAngle, float);

protected:
	vtkErCamera();
	virtual ~vtkErCamera();

	//BTX
	vtkRenderer*			Renderer;
	float					FocalDistance;
	float					Exposure;
	float					Gamma;
	Enums::ApertureShape	ApertureShape;
	float					ApertureSize;
	int						NoApertureBlades;
	float					ApertureAngle;
	//ETX
};