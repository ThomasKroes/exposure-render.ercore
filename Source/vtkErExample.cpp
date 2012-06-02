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
#include "vtkErVolumeProperty.h"

char gVolumeFile[]		= "C://Volumes//manix.mhd";

void ConfigureER(vtkRenderer* Renderer);
void LoadVolume(vtkErTracer* Tracer);
void CreateCamera(vtkRenderer* Renderer);
void CreateVolumeProperty(vtkErTracer* Tracer);
void CreateLighting(vtkErTracer* Tracer);
void CreateObjects(vtkErTracer* Tracer);

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
	CreateVolumeProperty(Tracer);
	CreateLighting(Tracer);
//	CreateObjects(Tracer);
	CreateCamera(Renderer);

	Tracer->Update();

	vtkSmartPointer<vtkVolume> Volume = vtkSmartPointer<vtkVolume>::New();
	
	Volume->Update();
	Volume->SetMapper(Tracer);

	Renderer->AddVolume(Volume);
}

void CreateVolumeProperty(vtkErTracer* Tracer)
{
	vtkSmartPointer<vtkErVolumeProperty> VolumeProperty = vtkSmartPointer<vtkErVolumeProperty>::New();
	
	const float StepSize = 2.0f;

	VolumeProperty->SetStepFactorPrimary(StepSize);
	VolumeProperty->SetStepFactorShadow(2.0f * StepSize);
	VolumeProperty->SetShadingMode(ExposureRender::Enums::BrdfOnly);
	VolumeProperty->SetDensityScale(100);

	vtkSmartPointer<vtkPiecewiseFunction> Opacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	
	Opacity->AddPoint(100, 1);
	Opacity->AddPoint(1024, 1);
	
	VolumeProperty->SetOpacity(Opacity);

	vtkSmartPointer<vtkColorTransferFunction> Diffuse = vtkSmartPointer<vtkColorTransferFunction>::New();
	
	Diffuse->AddRGBPoint(0, 1, 1, 1);
	Diffuse->AddRGBPoint(2048, 1, 1, 1);

	VolumeProperty->SetDiffuse(Diffuse);

	vtkSmartPointer<vtkColorTransferFunction> Specular = vtkSmartPointer<vtkColorTransferFunction>::New();
	
	Specular->AddRGBPoint(0, 1, 1, 1);
	Specular->AddRGBPoint(2048, 1, 1, 1);

	VolumeProperty->SetSpecular(Specular);

	vtkSmartPointer<vtkPiecewiseFunction> Glossiness = vtkSmartPointer<vtkPiecewiseFunction>::New();
	
	Glossiness->AddPoint(0, 0.8);
	Glossiness->AddPoint(2048, 0.8);
	
	VolumeProperty->SetGlossiness(Glossiness);

	Tracer->SetVolumeProperty(VolumeProperty);
}

void LoadVolume(vtkErTracer* Tracer)
{
	vtkSmartPointer<vtkMetaImageReader> Reader	= vtkSmartPointer<vtkMetaImageReader>::New();
	
	Reader->SetFileName(gVolumeFile);
	
	if (Reader->CanReadFile(gVolumeFile) == 0)
	{
		printf("Can't read %s, aborting!\n", gVolumeFile);
		exit(EXIT_FAILURE);
	}

	Reader->Update();

	vtkSmartPointer<vtkImageCast> ImageCast = vtkSmartPointer<vtkImageCast>::New();
	
	ImageCast->SetOutputScalarTypeToUnsignedShort();
	ImageCast->SetClampOverflow(1);
	ImageCast->SetInputConnection(0, Reader->GetOutputPort());
	ImageCast->Update();

	vtkSmartPointer<vtkErVolume> Volume	= vtkSmartPointer<vtkErVolume>::New();

	Volume->SetInputConnection(0, ImageCast->GetOutputPort());
	Volume->SetFilterMode(ExposureRender::Enums::Linear);
	Volume->SetAcceleratorType(ExposureRender::Enums::NoAcceleration);

	Tracer->SetInputConnection(vtkErTracer::VolumesPort, Volume->GetOutputPort());
}

void CreateCamera(vtkRenderer* Renderer)
{
	vtkSmartPointer<vtkErCamera> Camera = vtkSmartPointer<vtkErCamera>::New();

	Camera->SetExposure(10);
	Camera->SetApertureShape(ExposureRender::Enums::Polygon);
	Camera->SetApertureSize(0.0f);
	Camera->SetNoApertureBlades(4);
	Camera->SetApertureAngle(0.0f);

	Renderer->SetActiveCamera(Camera);
	Renderer->ResetCamera();

	Camera->SetClippingRange(0, 1000000);
}

void CreateLighting(vtkErTracer* Tracer)
{
	vtkSmartPointer<vtkErTexture> KeyLightTexture = vtkSmartPointer<vtkErTexture>::New();

	KeyLightTexture->SetTextureType(ExposureRender::Enums::Procedural);
	KeyLightTexture->SetProceduralType(ExposureRender::Enums::Checker);
	KeyLightTexture->SetUniformColor(1, 1, 1);
	KeyLightTexture->SetRepeat(3, 3);

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

	JpegReader->SetFileName("C:\\Users\\Thomas Kroes\\Desktop\\radissonpano1.jpg");
	JpegReader->Update();

	vtkSmartPointer<vtkErBitmap> Bitmap = vtkSmartPointer<vtkErBitmap>::New();
	
	Bitmap->SetFilterMode(ExposureRender::Enums::Linear);
	Bitmap->SetInputConnection(0, JpegReader->GetOutputPort());

	ImageTexture->SetInputConnection(0, Bitmap->GetOutputPort());
	/**/

	vtkSmartPointer<vtkErLight> KeyLight = vtkSmartPointer<vtkErLight>::New();
	
	const float KeyLightSize = 2.1f;

	KeyLight->SetAlignmentType(ExposureRender::Enums::Spherical);
	KeyLight->SetShapeType(ExposureRender::Enums::Plane);
	KeyLight->SetOuterRadius(0.01f);
	KeyLight->SetOneSided(true);
	KeyLight->SetElevation(60.0f);
	KeyLight->SetAzimuth(250.0f);
	KeyLight->SetOffset(1.5f);
	KeyLight->SetMultiplier(10.0f);
	KeyLight->SetSize(KeyLightSize, KeyLightSize, KeyLightSize);
	KeyLight->SetEmissionUnit(ExposureRender::Enums::Lux);
	KeyLight->SetInputConnection(KeyLightTexture->GetOutputPort());
	KeyLight->SetEnabled(false);

	vtkSmartPointer<vtkErLight> RimLight = vtkSmartPointer<vtkErLight>::New();
	
	const float RimLightSize = 0.1f;

	RimLight->SetAlignmentType(ExposureRender::Enums::AxisAlign);
	RimLight->SetAxis(ExposureRender::Enums::Y);
	RimLight->SetPosition(0.0f, 0.0f, 0.0f);
	RimLight->SetShapeType(ExposureRender::Enums::Sphere);
	RimLight->SetOneSided(false);
	RimLight->SetOuterRadius(20.0f);
	RimLight->SetElevation(45.0f);
	RimLight->SetAzimuth(125.0f);
	RimLight->SetOffset(30.0f);
	RimLight->SetMultiplier(20.0f);
	RimLight->SetSize(RimLightSize, RimLightSize, RimLightSize);
	RimLight->SetEmissionUnit(ExposureRender::Enums::Lux);
	RimLight->SetInputConnection(RimLightTexture->GetOutputPort());
	RimLight->SetEnabled(true);

	Tracer->AddInputConnection(vtkErTracer::LightsPort, KeyLight->GetOutputPort());
	Tracer->AddInputConnection(vtkErTracer::LightsPort, RimLight->GetOutputPort());
}

void CreateObjects(vtkErTracer* Tracer)
{
	vtkSmartPointer<vtkErTexture> DiffuseTexture = vtkSmartPointer<vtkErTexture>::New();

	DiffuseTexture->SetTextureType(ExposureRender::Enums::Procedural);
	DiffuseTexture->SetProceduralType(ExposureRender::Enums::Uniform);
	DiffuseTexture->SetRepeat(4, 4);
	DiffuseTexture->SetUniformColor(1, 1, 1);
	DiffuseTexture->SetOutputLevel(0.01f);

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
	Object->SetSize(100.0f, 100.0f, 100.0f);
	Object->SetEnabled(true);

	Object->SetInputConnection(vtkErObject::DiffuseTexturePort, DiffuseTexture->GetOutputPort());
//	Object->SetInputConnection(vtkErObject::SpecularTexturePort, DiffuseTexture->GetOutputPort());
//	Object->SetInputConnection(vtkErObject::GlossinessTexturePort, DiffuseTexture->GetOutputPort());

	Tracer->AddInputConnection(vtkErTracer::ObjectsPort, Object->GetOutputPort());
}
