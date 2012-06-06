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
#include <vtkPNGReader.h>
#include <vtkImageCast.h>
#include <vtkCommand.h>
#include <vtkImageGaussianSmooth.h>

#include "vtkErVolume.h"
#include "vtkErLight.h"
#include "vtkErTexture.h"
#include "vtkErTracer.h"
#include "vtkErCamera.h"
#include "vtkErObject.h"
#include "vtkErClippingObject.h"
#include "vtkErBitmap.h"
#include "vtkErTimerCallback.h"
#include "vtkErVolumeProperty.h"

char gVolumeFile[] = "C:\\Volumes\\manix.mhd";

#define BACK_PLANE_ON
//#define KEY_LIGHT_ON
#define ENVIRONMENT_ON

#ifdef BACK_PLANE_ON
	char gBackPlaneBitmap[] = "C:\\Bitmaps\\back_plane.png";
#endif

#ifdef ENVIRONMENT_ON
	char gEnvironmentBitmap[] = "C:\\Bitmaps\\environment.png";
#endif

void ConfigureER(vtkRenderer* Renderer);
void LoadVolume(vtkErTracer* Tracer);
void CreateCamera(vtkRenderer* Renderer);
void CreateVolumeProperty(vtkErTracer* Tracer);
void CreateLighting(vtkErTracer* Tracer);
void CreateObjects(vtkErTracer* Tracer);
void CreateClippingObjects(vtkErTracer* Tracer);

using namespace ExposureRender;

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

	Renderer->ResetCamera();

	RenderWindowInteractor->Start();

	return EXIT_SUCCESS;
}

void ConfigureER(vtkRenderer* Renderer)
{
	vtkSmartPointer<vtkErTracer> Tracer = vtkSmartPointer<vtkErTracer>::New();
	
	LoadVolume(Tracer);
	CreateVolumeProperty(Tracer);
	CreateLighting(Tracer);
	CreateObjects(Tracer);
	CreateClippingObjects(Tracer);
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
	
	const float StepSize = 5.0f;

	VolumeProperty->SetStepFactorPrimary(StepSize);
	VolumeProperty->SetStepFactorShadow(2.0f * StepSize);
	VolumeProperty->SetShadingMode(Enums::BrdfOnly);
	VolumeProperty->SetDensityScale(100);
	VolumeProperty->SetGradientFactor(0);

	vtkSmartPointer<vtkPiecewiseFunction> Opacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	
	Opacity->AddPoint(0, 0);
	Opacity->AddPoint(1, 0);
	Opacity->AddPoint(2, 0.5);
	Opacity->AddPoint(1024, 1);
	
	VolumeProperty->SetOpacity(Opacity);

	vtkSmartPointer<vtkColorTransferFunction> Diffuse = vtkSmartPointer<vtkColorTransferFunction>::New();
	
	const float DiffuseLevel = 0.0f;
	/*
	for (int i = 0; i < 50; i++)
	{
		Diffuse->AddHSVPoint(i, rand() / (float)RAND_MAX, 1.0f, 1.0f);
	}
	*/
	
	Diffuse->AddRGBPoint(0, DiffuseLevel, DiffuseLevel, DiffuseLevel);
	Diffuse->AddRGBPoint(2048, DiffuseLevel, DiffuseLevel, DiffuseLevel);
	

	VolumeProperty->SetDiffuse(Diffuse);

	vtkSmartPointer<vtkColorTransferFunction> Specular = vtkSmartPointer<vtkColorTransferFunction>::New();
	
	const float SpecularLevel = 1.0f;

	Specular->AddRGBPoint(0, SpecularLevel, SpecularLevel, SpecularLevel);
	Specular->AddRGBPoint(2048, SpecularLevel, SpecularLevel, SpecularLevel);

	VolumeProperty->SetSpecular(Specular);
	
	vtkSmartPointer<vtkPiecewiseFunction> Glossiness = vtkSmartPointer<vtkPiecewiseFunction>::New();
	
	const float GlossinessLevel = 1.0f;

	Glossiness->AddPoint(0, GlossinessLevel);
	Glossiness->AddPoint(2048, GlossinessLevel);
	
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
	Volume->SetFilterMode(Enums::Linear);
	Volume->SetAcceleratorType(Enums::NoAcceleration);

	Tracer->SetInputConnection(vtkErTracer::VolumesPort, Volume->GetOutputPort());
}

void CreateCamera(vtkRenderer* Renderer)
{
	vtkSmartPointer<vtkErCamera> Camera = vtkSmartPointer<vtkErCamera>::New();

	Camera->SetExposure(1);
	Camera->SetApertureShape(Enums::Polygon);
	Camera->SetApertureSize(0.0f);
	Camera->SetNoApertureBlades(6);
	Camera->SetApertureAngle(0.0f);
	Camera->SetClippingRange(0, 1000000);
	
	Renderer->SetActiveCamera(Camera);
}

void CreateLighting(vtkErTracer* Tracer)
{
#ifdef KEY_LIGHT_ON
	vtkSmartPointer<vtkErLight> KeyLight = vtkSmartPointer<vtkErLight>::New();

	const float KeyLightSize = 0.2;

	KeyLight->SetAlignmentType(Enums::Spherical);
	KeyLight->SetShapeType(Enums::Plane);
	KeyLight->SetOneSided(true);
	KeyLight->SetVisible(false);
	KeyLight->SetElevation(45.0f);
	KeyLight->SetAzimuth(-45.0f);
	KeyLight->SetOffset(0.8f);
	KeyLight->SetMultiplier(2.0f);
	KeyLight->SetSize(KeyLightSize, KeyLightSize, KeyLightSize);
	KeyLight->SetEmissionUnit(Enums::Lux);
	KeyLight->SetRelativeToCamera(1);
	KeyLight->SetUseCameraFocalPoint(1);
	KeyLight->SetEnabled(true);

	Tracer->AddInputConnection(vtkErTracer::LightsPort, KeyLight->GetOutputPort());

	vtkSmartPointer<vtkErTexture> KeyLightTexture = vtkSmartPointer<vtkErTexture>::New();

	KeyLightTexture->SetTextureType(Enums::Procedural);
	KeyLightTexture->SetProceduralType(Enums::Uniform);
	
	KeyLight->SetInputConnection(vtkErLight::TexturePort, KeyLightTexture->GetOutputPort());
#endif

#ifdef ENVIRONMENT_ON
	vtkSmartPointer<vtkErLight> EnvironmentLight = vtkSmartPointer<vtkErLight>::New();
	
	EnvironmentLight->SetAlignmentType(Enums::AxisAlign);
	EnvironmentLight->SetAxis(ExposureRender::Enums::X);
	EnvironmentLight->SetPosition(0, 0, 0);
	EnvironmentLight->SetShapeType(Enums::Sphere);
	EnvironmentLight->SetOneSided(false);
	EnvironmentLight->SetRadius(100.0f);
	EnvironmentLight->SetMultiplier(5.0f);
	EnvironmentLight->SetEmissionUnit(Enums::Lux);
	EnvironmentLight->SetEnabled(true);

	Tracer->AddInputConnection(vtkErTracer::LightsPort, EnvironmentLight->GetOutputPort());

	vtkSmartPointer<vtkErTexture> EnvironmentLightTexture = vtkSmartPointer<vtkErTexture>::New();

	vtkSmartPointer<vtkPNGReader> ImageReader = vtkSmartPointer<vtkPNGReader>::New();

	if (ImageReader->CanReadFile(gEnvironmentBitmap))
	{
		EnvironmentLightTexture->SetTextureType(Enums::Bitmap);

		ImageReader->SetFileName(gEnvironmentBitmap);
		ImageReader->Update();

		vtkSmartPointer<vtkImageGaussianSmooth> ImageGaussianSmooth = vtkSmartPointer<vtkImageGaussianSmooth>::New();
		
		ImageGaussianSmooth->SetInputConnection(ImageReader->GetOutputPort());
		ImageGaussianSmooth->SetRadiusFactor(2);
		ImageGaussianSmooth->SetStandardDeviation(2);
		ImageGaussianSmooth->Update();

		vtkSmartPointer<vtkErBitmap> Bitmap = vtkSmartPointer<vtkErBitmap>::New();
	
		Bitmap->SetFilterMode(Enums::Linear);
		Bitmap->SetInputConnection(vtkErBitmap::ImageDataPort, ImageGaussianSmooth->GetOutputPort());

		EnvironmentLightTexture->SetInputConnection(vtkErLight::TexturePort, Bitmap->GetOutputPort());
	}
	else
	{
		printf("%s cannot be loaded, reverting to gradient background.", gEnvironmentBitmap);

		EnvironmentLightTexture->SetTextureType(Enums::Procedural);
		EnvironmentLightTexture->SetProceduralType(Enums::Uniform);

		vtkSmartPointer<vtkColorTransferFunction> Gradient = vtkSmartPointer<vtkColorTransferFunction>::New();
		
		Gradient->AddRGBPoint(0.0f, 0.0025f, 0.0025f, 0.0025f);
		Gradient->AddRGBPoint(0.5f, 0.01f, 0.01f, 0.01f);
		Gradient->AddRGBPoint(0.50001f, 0.9f, 0.8f, 0.5f);
		Gradient->AddRGBPoint(0.51f, 0.9f, 0.6f, 0.2f);
		Gradient->AddHSVPoint(1.0f, 0.6f, 0.9f, 0.8f);

		EnvironmentLightTexture->SetGradient(Gradient);
	}

	EnvironmentLight->SetInputConnection(vtkErLight::TexturePort, EnvironmentLightTexture->GetOutputPort());
#endif
}

void CreateObjects(vtkErTracer* Tracer)
{
#ifdef BACK_PLANE_ON
	vtkSmartPointer<vtkErObject> Object = vtkSmartPointer<vtkErObject>::New();

	const float BackPlaneSize = 10;

	Object->SetAlignmentType(Enums::AxisAlign);
	Object->SetAxis(Enums::Z);
	Object->SetPosition(0.0f, 0.0f, -0.5f);
	Object->SetShapeType(Enums::Plane);
	Object->SetSize(BackPlaneSize, BackPlaneSize, BackPlaneSize);
	Object->SetRelativeToCamera(1);
	Object->SetUseCameraFocalPoint(1);
	Object->SetElevation(0.0f);
	Object->SetAzimuth(0.0f);
	Object->SetOffset(-0.0f);
	Object->SetEnabled(true);

	vtkSmartPointer<vtkErTexture> DiffuseTexture = vtkSmartPointer<vtkErTexture>::New();

	DiffuseTexture->SetOutputLevel(0.3f);
	DiffuseTexture->SetRepeat(3, 3);

	vtkSmartPointer<vtkPNGReader> ImageReader = vtkSmartPointer<vtkPNGReader>::New();

	if (ImageReader->CanReadFile(gBackPlaneBitmap))
	{
		DiffuseTexture->SetTextureType(Enums::Bitmap);

		ImageReader->SetFileName(gBackPlaneBitmap);
		ImageReader->Update();

		vtkSmartPointer<vtkErBitmap> Bitmap = vtkSmartPointer<vtkErBitmap>::New();
	
		Bitmap->SetFilterMode(Enums::Linear);
		Bitmap->SetInputConnection(vtkErBitmap::ImageDataPort, ImageReader->GetOutputPort());

		DiffuseTexture->SetInputConnection(vtkErLight::TexturePort, Bitmap->GetOutputPort());
	}
	else
	{
		printf("%s cannot be loaded, reverting to checker background.", gBackPlaneBitmap);

		DiffuseTexture->SetTextureType(Enums::Procedural);
		DiffuseTexture->SetProceduralType(Enums::Checker);
		DiffuseTexture->SetRepeat(10, 10);
	}

	Object->SetInputConnection(vtkErObject::DiffuseTexturePort, DiffuseTexture->GetOutputPort());
//	Object->SetInputConnection(vtkErObject::SpecularTexturePort, DiffuseTexture->GetOutputPort());
//	Object->SetInputConnection(vtkErObject::GlossinessTexturePort, DiffuseTexture->GetOutputPort());

	Tracer->AddInputConnection(vtkErTracer::ObjectsPort, Object->GetOutputPort());
#endif
}

void CreateClippingObjects(vtkErTracer* Tracer)
{
	return;

	for (int i = 0; i < 1; i++)
	{
		vtkSmartPointer<vtkErClippingObject> ClippingObject = vtkSmartPointer<vtkErClippingObject>::New();

		ClippingObject->SetAlignmentType(Enums::AxisAlign);
		ClippingObject->SetAzimuth(0);
		ClippingObject->SetSize(100, 100, 100);
		ClippingObject->SetOffset(0.2f);
		ClippingObject->SetPosition(0, -0.01, 0);
		ClippingObject->SetAutoFlip(true);

		Tracer->AddInputConnection(vtkErTracer::ClippingObjectsPort, ClippingObject->GetOutputPort());
	}
}
