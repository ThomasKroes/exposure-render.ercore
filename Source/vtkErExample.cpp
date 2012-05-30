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

// char gFileName[] = "C://model_segmentation.mhd";
// char gFileName[] = "C://Volumes//manix.mhd";
char gFileName[] = "c://manix.mhd";

void ConfigureER(vtkRenderer* Renderer);
void LoadVolume(vtkErTracer* Tracer);
void CreateCamera(vtkRenderer* Renderer);
void CreateLighting(vtkErTracer* Tracer);
void CreateObjects(vtkErTracer* Tracer);
void SetTransferFunction(vtkErTracer* Tracer);

int main(int, char *[])
{
	vtkSmartPointer<vtkRenderWindow> RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();

	vtkSmartPointer<vtkRenderer> Renderer = vtkSmartPointer<vtkRenderer>::New();

	vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

	RenderWindowInteractor->SetRenderWindow(RenderWindow);
	
	RenderWindow->AddRenderer(Renderer);

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

	RenderWindow->Render();
	RenderWindow->SetSize(512, 512);

	ConfigureER(Renderer);

	RenderWindowInteractor->Start();

	return EXIT_SUCCESS;
}

void ConfigureER(vtkRenderer* Renderer)
{
	vtkSmartPointer<vtkErTracer> Tracer = vtkSmartPointer<vtkErTracer>::New();
	
	LoadVolume(Tracer);
	CreateLighting(Tracer);
	CreateObjects(Tracer);
	SetTransferFunction(Tracer);
	CreateCamera(Renderer);

	Tracer->SetDensityScale(100);

	const float StepSize = 4.0f;

	Tracer->SetStepFactorPrimary(StepSize);
	Tracer->SetStepFactorShadow(2.0f * StepSize);
	Tracer->SetShadingMode(ExposureRender::Enums::BrdfOnly);

	Tracer->Update();

	vtkSmartPointer<vtkVolume> Volume = vtkSmartPointer<vtkVolume>::New();
	
	Volume->Update();
	Volume->SetMapper(Tracer);

	Renderer->AddVolume(Volume);
}
 
void LoadVolume(vtkErTracer* Tracer)
{
	vtkSmartPointer<vtkMetaImageReader> Reader	= vtkSmartPointer<vtkMetaImageReader>::New();
	
	Reader->SetFileName(gFileName);
	
	if (Reader->CanReadFile(gFileName) == 0)
	{
		printf("can't read file!");
		exit(EXIT_FAILURE);
	}

	Reader->Update();

	vtkSmartPointer<vtkImageCast> ImageCast	= vtkSmartPointer<vtkImageCast>::New();

	ImageCast->SetInputConnection(0, Reader->GetOutputPort());
	ImageCast->SetOutputScalarTypeToUnsignedShort();
	ImageCast->Update();

	vtkSmartPointer<vtkErVolume> Volume = vtkSmartPointer<vtkErVolume>::New();

	Volume->SetInputConnection(0, ImageCast->GetOutputPort());
	Volume->SetFilterMode(ExposureRender::Enums::NearestNeighbour);
	Volume->SetAcceleratorType(ExposureRender::Enums::NoAcceleration);

	Volume->SetAlignmentType(ExposureRender::Enums::AxisAlign);
	Volume->SetAxis(ExposureRender::Enums::Z);
	Volume->SetPosition(0, 0, 0);

	Volume->Update();

	Tracer->SetInputConnection(vtkErTracer::VolumePort, Volume->GetOutputPort());
}

void CreateCamera(vtkRenderer* Renderer)
{
	vtkSmartPointer<vtkErCamera> Camera = vtkSmartPointer<vtkErCamera>::New();

	Camera->SetExposure(1.0f);
	Camera->SetFocalDisk(0.02f);

	Renderer->SetActiveCamera(Camera);
	Renderer->ResetCamera();

	Camera->SetClippingRange(0, 1000000);
}

void CreateLighting(vtkErTracer* Tracer)
{
	vtkSmartPointer<vtkErTexture> KeyLightTexture = vtkSmartPointer<vtkErTexture>::New();

	KeyLightTexture->SetTextureType(ExposureRender::Enums::Procedural);
	KeyLightTexture->SetProceduralType(ExposureRender::Enums::Uniform);
	KeyLightTexture->SetUniformColor(0.9, 0.6, 0.2);

	vtkSmartPointer<vtkErTexture> RimLightTexture = vtkSmartPointer<vtkErTexture>::New();

	RimLightTexture->SetTextureType(ExposureRender::Enums::Procedural);
	RimLightTexture->SetProceduralType(ExposureRender::Enums::Gradient);
	RimLightTexture->SetOutputLevel(1.0f);

	vtkSmartPointer<vtkColorTransferFunction> Gradient = vtkSmartPointer<vtkColorTransferFunction>::New();
	
	Gradient->AddRGBPoint(0.0f, 0.0025f, 0.0025f, 0.0025f);
	Gradient->AddRGBPoint(0.5f, 0.01f, 0.01f, 0.01f);
	Gradient->AddRGBPoint(0.50001f, 0.9f, 0.8f, 0.5f);
	Gradient->AddRGBPoint(0.51f, 0.9f, 0.6f, 0.2f);
	Gradient->AddHSVPoint(1.0f, 0.6f, 0.9f, 0.8f);

	RimLightTexture->SetGradient(Gradient);

	
	vtkSmartPointer<vtkErTexture> ImageTexture = vtkSmartPointer<vtkErTexture>::New();

	ImageTexture->SetTextureType(ExposureRender::Enums::Bitmap);

	vtkSmartPointer<vtkJPEGReader> JpegReader = vtkSmartPointer<vtkJPEGReader>::New();

	JpegReader->SetFileName("C:\\Users\\Thomas\\Desktop\\panorama.jpg");
	JpegReader->Update();

	vtkSmartPointer<vtkErBitmap> Bitmap = vtkSmartPointer<vtkErBitmap>::New();
	
	Bitmap->SetFilterMode(ExposureRender::Enums::Linear);
	Bitmap->SetInputConnection(0, JpegReader->GetOutputPort());

	ImageTexture->SetInputConnection(0, Bitmap->GetOutputPort());
	/**/

	vtkSmartPointer<vtkErLight> KeyLight = vtkSmartPointer<vtkErLight>::New();
	
	const float KeyLightSize = 100.0f;

	KeyLight->SetAlignmentType(ExposureRender::Enums::Spherical);
	KeyLight->SetShapeType(ExposureRender::Enums::Disk);
	KeyLight->SetOuterRadius(0.5f);
	KeyLight->SetOneSided(false);
	KeyLight->SetElevation(45.0f);
	KeyLight->SetAzimuth(145.0f);
	KeyLight->SetOffset(3.0f);
	KeyLight->SetMultiplier(100.0f);
	KeyLight->SetSize(KeyLightSize, KeyLightSize, KeyLightSize);
	KeyLight->SetEmissionUnit(ExposureRender::Enums::Lux);
	KeyLight->SetInputConnection(KeyLightTexture->GetOutputPort());
	KeyLight->SetEnabled(true);

	vtkSmartPointer<vtkErLight> RimLight = vtkSmartPointer<vtkErLight>::New();
	
	const float RimLightSize = 2.0f;

	RimLight->SetAlignmentType(ExposureRender::Enums::AxisAlign);
	RimLight->SetAxis(ExposureRender::Enums::Y);
	RimLight->SetPosition(0.0f, -0.5f, 0.0f);
	RimLight->SetShapeType(ExposureRender::Enums::Sphere);
	RimLight->SetOneSided(false);
	RimLight->SetOuterRadius(5.0f);
	RimLight->SetElevation(45.0f);
	RimLight->SetAzimuth(125.0f);
	RimLight->SetOffset(3.0f);
	RimLight->SetMultiplier(1000.0f);
	RimLight->SetSize(RimLightSize, RimLightSize, RimLightSize);
	RimLight->SetEmissionUnit(ExposureRender::Enums::Power);
	RimLight->SetInputConnection(ImageTexture->GetOutputPort());
	RimLight->SetEnabled(true);

	Tracer->AddInputConnection(vtkErTracer::LightsPort, KeyLight->GetOutputPort());
	Tracer->AddInputConnection(vtkErTracer::LightsPort, RimLight->GetOutputPort());
}

void CreateObjects(vtkErTracer* Tracer)
{
	vtkSmartPointer<vtkErTexture> DiffuseTexture = vtkSmartPointer<vtkErTexture>::New();

	DiffuseTexture->SetTextureType(ExposureRender::Enums::Procedural);
	DiffuseTexture->SetProceduralType(ExposureRender::Enums::Uniform);
	DiffuseTexture->SetUniformColor(1, 1, 1);
	DiffuseTexture->SetOutputLevel(0.1f);

	/*
	vtkSmartPointer<vtkJPEGReader> JpegReader = vtkSmartPointer<vtkJPEGReader>::New();

	JpegReader->SetFileName("C:\\Users\\Thomas\\Desktop\\graph.jpg");
	JpegReader->Update();

	vtkSmartPointer<vtkErBitmap> Bitmap = vtkSmartPointer<vtkErBitmap>::New();
	
	Bitmap->SetFilterMode(ExposureRender::Enums::NearestNeighbour);
	Bitmap->SetInputConnection(0, JpegReader->GetOutputPort());

	DiffuseTexture->SetInputConnection(0, Bitmap->GetOutputPort());
	*/

	vtkSmartPointer<vtkErObject> Object = vtkSmartPointer<vtkErObject>::New();

	Object->SetAlignmentType(ExposureRender::Enums::AxisAlign);
	Object->SetAxis(ExposureRender::Enums::Y);
	Object->SetPosition(0.0f, -0.5f, 0.0f);
	Object->SetShapeType(ExposureRender::Enums::Plane);
	Object->SetSize(10.0f, 10.0f, 10.0f);
	Object->SetEnabled(true);

	Object->SetInputConnection(vtkErObject::DiffuseTexturePort, DiffuseTexture->GetOutputPort());
//	Object->SetInputConnection(vtkErObject::SpecularTexturePort, DiffuseTexture->GetOutputPort());
//	Object->SetInputConnection(vtkErObject::GlossinessTexturePort, DiffuseTexture->GetOutputPort());

	Tracer->AddInputConnection(vtkErTracer::ObjectsPort, Object->GetOutputPort());
}

void SetTransferFunction(vtkErTracer* Tracer)
{
	vtkSmartPointer<vtkPiecewiseFunction> Opacity = vtkSmartPointer<vtkPiecewiseFunction>::New();

	Opacity->AddPoint(32900, 0.1);
	Opacity->AddPoint(40000, 1.0);

	Tracer->SetOpacity(Opacity);

	vtkSmartPointer<vtkColorTransferFunction> Diffuse = vtkSmartPointer<vtkColorTransferFunction>::New();

	/*
	for (int i = 0; i < 1024; i++)
	{
		float H = (float)rand() / RAND_MAX;
		Diffuse->AddHSVPoint(i, H, 0.75, 0.75);
		Diffuse->AddHSVPoint(i + 1, H, 0.75, 0.75);
	}
	*/

	Diffuse->AddRGBPoint(0, 1.0f, 0.2f, 0.2f);
	Diffuse->AddRGBPoint(32900, 0.8f, 0.3f, 0.2f);
	Diffuse->AddRGBPoint(40000, 0.9f, 0.8f, 0.5f);

	Tracer->SetDiffuse(Diffuse);

	vtkSmartPointer<vtkColorTransferFunction> Specular = vtkSmartPointer<vtkColorTransferFunction>::New();

	Specular->AddRGBPoint(0, 1, 1, 1);
	Specular->AddRGBPoint(100, 0, 0, 0);
	Specular->AddRGBPoint(200, 1, 1, 1);

	Tracer->SetSpecular(Specular);

	vtkSmartPointer<vtkPiecewiseFunction> Glossiness = vtkSmartPointer<vtkPiecewiseFunction>::New();

	Glossiness->AddPoint(32900, 0.2);
	Glossiness->AddPoint(40000, 0.3);

	Tracer->SetGlossiness(Glossiness);

	vtkSmartPointer<vtkColorTransferFunction> Emission = vtkSmartPointer<vtkColorTransferFunction>::New();

	Emission->AddRGBPoint(60, 0, 0, 0);
	Emission->AddRGBPoint(200, 0, 0, 0);

	Tracer->SetEmission(Emission);
}
