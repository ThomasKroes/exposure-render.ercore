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

#include "vtkErVolume.h"
#include "vtkErLight.h"
#include "vtkErTexture.h"
#include "vtkErTracer.h"
#include "vtkErCamera.h"
#include "vtkErObject.h"

char gFileName[] = "C://Volumes//manix.mhd";

int main(int, char *[])
{
	vtkSmartPointer<vtkMetaImageReader> Reader = vtkSmartPointer<vtkMetaImageReader>::New();

	Reader->SetFileName(gFileName);
	
	if (Reader->CanReadFile(gFileName) == 0)
	{
		printf("can't read file!");
		return EXIT_FAILURE;
	}

	Reader->Update();

	vtkSmartPointer<vtkErVolume> ErVolume = vtkSmartPointer<vtkErVolume>::New();

	vtkSmartPointer<vtkErLight> ErLight = vtkSmartPointer<vtkErLight>::New();
	vtkSmartPointer<vtkErLight> ErLight2 = vtkSmartPointer<vtkErLight>::New();
	
	ErLight->SetAlignmentType(ExposureRender::Enums::Spherical);
	ErLight->SetElevation(45.0f);
	ErLight->SetAzimuth(135.0f);
	ErLight->SetOffset(1.0f);
	ErLight->SetMultiplier(20.0f);
	ErLight->SetSize(0.1f, 0.1f, 0.1f);

	ErLight2->SetMultiplier(20.0f);
	ErLight2->SetOffset(2.0f);

	vtkSmartPointer<vtkErObject> ErObject = vtkSmartPointer<vtkErObject>::New();

	ErObject->SetAlignmentType(Enums::AxisAlign);
	ErObject->SetAxis(Enums::Y);
	ErObject->SetPosition(0.0f, -0.5f, 0.0f);
	ErObject->SetSize(10.0f, 10.0f, 10.0f);

	vtkSmartPointer<vtkErTexture> ErTexture = vtkSmartPointer<vtkErTexture>::New();

	ErLight->SetInputConnection(ErTexture->GetOutputPort());
	ErLight2->SetInputConnection(ErTexture->GetOutputPort());
	
	ErObject->SetInputConnection(0, ErTexture->GetOutputPort());
	ErObject->SetInputConnection(1, ErTexture->GetOutputPort());
	ErObject->SetInputConnection(2, ErTexture->GetOutputPort());

	ErTexture->SetProceduralType(Enums::Checker);
	ErTexture->SetRepeat(2.0f, 2.0f);
	ErTexture->SetCheckerColor1(0.8f, 0.8f, 0.8f);
	ErTexture->SetCheckerColor2(0.3f, 0.3f, 0.3f);

	vtkSmartPointer<vtkErTracer> VolumeMapper = vtkSmartPointer<vtkErTracer>::New();
	
	vtkSmartPointer<vtkErCamera> Camera = vtkSmartPointer<vtkErCamera>::New();

	Camera->SetClippingRange(0, 100000);
	Camera->SetExposure(1);
	
	vtkSmartPointer<vtkPiecewiseFunction> Opacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	
	Opacity->AddPoint(0, 0);
	Opacity->AddPoint(100, 0);
	Opacity->AddPoint(1000, 1);

	VolumeMapper->SetOpacity(Opacity);

	vtkSmartPointer<vtkColorTransferFunction> Diffuse = vtkSmartPointer<vtkColorTransferFunction>::New();

	Diffuse->AddRGBPoint(0, 1, 1, 1);
	Diffuse->AddRGBPoint(32750, 1, 0, 0);
	Diffuse->AddRGBPoint(35750, 0, 1, 0);

	VolumeMapper->SetDiffuse(Diffuse);

	vtkSmartPointer<vtkColorTransferFunction> Emission = vtkSmartPointer<vtkColorTransferFunction>::New();

	Emission->AddRGBPoint(0, 1, 1, 1);
	Emission->AddRGBPoint(65000, 1, 1, 1);

//	VolumeMapper->SetEmission(Emission);

	ErVolume->SetInputConnection(0, Reader->GetOutputPort());
		
	ErVolume->Update();

//	vtkOutpu
	VolumeMapper->SetInputConnection(0, ErVolume->GetOutputPort());
	VolumeMapper->AddInputConnection(1, ErLight->GetOutputPort());
	VolumeMapper->AddInputConnection(1, ErLight2->GetOutputPort());
	VolumeMapper->SetDensityScale(1000.0f);
	VolumeMapper->SetStepFactorPrimary(5);
	VolumeMapper->SetStepFactorShadow(5);

	VolumeMapper->Update();

	vtkSmartPointer<vtkVolume> Volume = vtkSmartPointer<vtkVolume>::New();
	Volume->Update();
	

	Volume->SetMapper(VolumeMapper);

	VolumeMapper->SetInputConnection(2, ErObject->GetOutputPort());

	vtkSmartPointer<vtkRenderer> Renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();

	RenderWindow->AddRenderer(Renderer);
	
	vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

	RenderWindowInteractor->SetRenderWindow(RenderWindow);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); //like paraview
 
	RenderWindowInteractor->SetInteractorStyle( style );

	Renderer->AddVolume(Volume);
	Renderer->SetActiveCamera(Camera);
	Renderer->ResetCamera();

	RenderWindow->Render();
	RenderWindow->SetSize(512, 512);
	RenderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
