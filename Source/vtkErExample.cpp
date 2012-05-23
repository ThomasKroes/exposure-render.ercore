/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or witDEVut modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software witDEVut specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT DEVLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT DEVLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) DEVWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <vtkConeSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkMetaImageReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkJPEGReader.h>
#include <vtkImageCast.h>
#include <vtkCommand.h>

#include "vtkErVolume.h"
#include "vtkErLight.h"
#include "vtkErTexture.h"
#include "vtkErTracer.h"
#include "vtkErCamera.h"
#include "vtkErObject.h"
#include "vtkErBitmap.h"
#include "vtkErTimerCallback.h"

char gFileName[] = "C://Volumes//engine.mhd";

vtkSmartPointer<vtkMetaImageReader> Reader	= vtkSmartPointer<vtkMetaImageReader>::New();
vtkSmartPointer<vtkImageCast> ImageCast		= vtkSmartPointer<vtkImageCast>::New();

int main(int, char *[])
{
	Reader->SetFileName(gFileName);
	
	if (Reader->CanReadFile(gFileName) == 0)
	{
		printf("can't read file!");
		return EXIT_FAILURE;
	}

	Reader->Update();

	ImageCast->SetInputConnection(0, Reader->GetOutputPort());
	ImageCast->SetOutputScalarTypeToUnsignedShort();
	ImageCast->Update();

	vtkSmartPointer<vtkErVolume> ErVolume = vtkSmartPointer<vtkErVolume>::New();

	ErVolume->SetInputConnection(0, ImageCast->GetOutputPort());
	ErVolume->SetFilterMode(ExposureRender::Enums::Linear);
	ErVolume->Update();

	vtkSmartPointer<vtkErTexture> KeyLightTexture = vtkSmartPointer<vtkErTexture>::New();

	KeyLightTexture->SetTextureType(ExposureRender::Enums::Procedural);
	KeyLightTexture->SetProceduralType(ExposureRender::Enums::Uniform);
	KeyLightTexture->SetUniformColor(1, 1, 1);

	vtkSmartPointer<vtkErTexture> RimLightTexture = vtkSmartPointer<vtkErTexture>::New();

	RimLightTexture->SetTextureType(ExposureRender::Enums::Procedural);
	RimLightTexture->SetProceduralType(ExposureRender::Enums::Uniform);
	RimLightTexture->SetUniformColor(1, 1, 1);

	vtkSmartPointer<vtkErLight> KeyLight = vtkSmartPointer<vtkErLight>::New();
	
	const float KeyLightSize = 1.0f;

	KeyLight->SetAlignmentType(ExposureRender::Enums::Spherical);
	KeyLight->SetElevation(-25.0f);
	KeyLight->SetAzimuth(-55.0f);
	KeyLight->SetOffset(5.0f);
	KeyLight->SetMultiplier(5.0f);
	KeyLight->SetSize(KeyLightSize, KeyLightSize, KeyLightSize);
	KeyLight->SetEmissionUnit(ExposureRender::Enums::Lux);
	KeyLight->SetInputConnection(KeyLightTexture->GetOutputPort());
	KeyLight->SetEnabled(false);

	vtkSmartPointer<vtkErLight> RimLight = vtkSmartPointer<vtkErLight>::New();
	
	const float RimLightSize = 1.0f;

	RimLight->SetAlignmentType(ExposureRender::Enums::AxisAlign);
	RimLight->SetPosition(0.1f, 0.1f, 0.1f);
	RimLight->SetShapeType(ExposureRender::Enums::Sphere);
	RimLight->SetOneSided(false);
	RimLight->SetOuterRadius(10000.0f);
	RimLight->SetElevation(45.0f);
	RimLight->SetAzimuth(135.0f);
	RimLight->SetOffset(0.0f);
	RimLight->SetMultiplier(0.1f);
	RimLight->SetSize(RimLightSize, RimLightSize, RimLightSize);
	RimLight->SetEmissionUnit(ExposureRender::Enums::Power);
	RimLight->SetInputConnection(RimLightTexture->GetOutputPort());
	RimLight->SetEnabled(true);

	vtkSmartPointer<vtkErTracer> Tracer = vtkSmartPointer<vtkErTracer>::New();
	
	vtkSmartPointer<vtkErCamera> Camera = vtkSmartPointer<vtkErCamera>::New();

	Camera->SetClippingRange(0, 100000);
	Camera->SetExposure(0.1f);
	
	vtkSmartPointer<vtkPiecewiseFunction> Opacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	
	Opacity->AddPoint(0, 0);
	Opacity->AddPoint(20, 0);
	Opacity->AddPoint(21, 1);

	Tracer->SetOpacity(Opacity);

	vtkSmartPointer<vtkColorTransferFunction> Diffuse = vtkSmartPointer<vtkColorTransferFunction>::New();

	/*
	for (int i = 0; i < 50; i++)
	{
		float H = (float)rand() / RAND_MAX;
		Diffuse->AddHSVPoint(i, H, 0.75, 0.75);
		Diffuse->AddHSVPoint(i + 1, H, 0.75, 0.75);
	}
	*/

	Diffuse->AddRGBPoint(0, 1, 1, 1);
	Diffuse->AddRGBPoint(1, 1, 1, 1);

	Tracer->SetDiffuse(Diffuse);

	vtkSmartPointer<vtkColorTransferFunction> Specular = vtkSmartPointer<vtkColorTransferFunction>::New();

	Specular->AddRGBPoint(0, 0, 0, 0);
	Specular->AddRGBPoint(32750, 0, 0, 0);

	Tracer->SetSpecular(Specular);

	vtkSmartPointer<vtkColorTransferFunction> Emission = vtkSmartPointer<vtkColorTransferFunction>::New();

	Emission->AddRGBPoint(0, 0, 0, 0);
	Emission->AddRGBPoint(1, 0, 0, 0);

	Tracer->SetEmission(Emission);

	Tracer->SetInputConnection(0, ErVolume->GetOutputPort());
	Tracer->AddInputConnection(1, KeyLight->GetOutputPort());
	Tracer->AddInputConnection(1, RimLight->GetOutputPort());
	Tracer->SetDensityScale(1000.0f);
	Tracer->SetStepFactorPrimary(3);
	Tracer->SetStepFactorShadow(15);

	Tracer->Update();

	vtkSmartPointer<vtkVolume> Volume = vtkSmartPointer<vtkVolume>::New();
	Volume->Update();
	
	Volume->SetMapper(Tracer);

	vtkSmartPointer<vtkRenderer> Renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();

	RenderWindow->AddRenderer(Renderer);
	
	vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

	RenderWindowInteractor->SetRenderWindow(RenderWindow);
	
	// Create and apply timer callback
	vtkSmartPointer<vtkErTimerCallback> TimerCallback = vtkSmartPointer<vtkErTimerCallback>::New();
	TimerCallback->SetRenderWindowInteractor(RenderWindowInteractor);
	
	// Create and apply interactor style
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> InteractorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	InteractorStyle->SetMotionFactor(10);
		
	RenderWindowInteractor->Initialize();
	RenderWindowInteractor->CreateRepeatingTimer(1);
	RenderWindowInteractor->AddObserver(vtkCommand::TimerEvent, TimerCallback);
	RenderWindowInteractor->SetInteractorStyle(InteractorStyle);

	Renderer->AddVolume(Volume);
	Renderer->SetActiveCamera(Camera);
	Renderer->ResetCamera();

	RenderWindow->Render();
	RenderWindow->SetSize(512, 512);
	RenderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
