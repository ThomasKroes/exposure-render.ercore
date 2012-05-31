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

#ifdef UAH
	char gSegmentation[]	= "C://uah_segmentation.mhd";
	char gRiskNerves[]		= "C://uah_risk_nerves.mhd";
	char gRiskVeins[]		= "C://uah_risk_veins.mhd";
	char gRiskArteries[]	= "C://uah_risk_arteries.mhd";
#else
	char gIntensityVolumeFileName[]		= "C://Volumes//manix.mhd";
#endif

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
//	CreateObjects(Tracer);
	SetTransferFunction(Tracer);
	CreateCamera(Renderer);

	Tracer->SetDensityScale(10);

	const float StepSize = 2.0f;

	Tracer->SetStepFactorPrimary(StepSize);
	Tracer->SetStepFactorShadow(2.0f * StepSize);
	Tracer->SetShadingMode(ExposureRender::Enums::PhaseFunctionOnly);

	Tracer->Update();

	vtkSmartPointer<vtkVolume> Volume = vtkSmartPointer<vtkVolume>::New();
	
	Volume->Update();
	Volume->SetMapper(Tracer);

	Renderer->AddVolume(Volume);
}

void LoadDistanceField(vtkErTracer* Tracer, const char* pFileName, const float& Hue, const int& VolumeID)
{
	vtkSmartPointer<vtkMetaImageReader> Reader = vtkSmartPointer<vtkMetaImageReader>::New();

	if (Reader->CanReadFile(pFileName) == 0)
	{
		printf("Can't read distance field (%s)!\n", pFileName);
		exit(EXIT_FAILURE);
	}
	else
	{
		Reader->SetFileName(pFileName);
	}
	
	Reader->Update();
	
	vtkSmartPointer<vtkImageCast> ImageCast = vtkSmartPointer<vtkImageCast>::New();

	ImageCast->SetOutputScalarTypeToUnsignedShort();
	ImageCast->SetClampOverflow(1);
	ImageCast->SetInputConnection(0, Reader->GetOutputPort());
	ImageCast->Update();

	vtkSmartPointer<vtkErVolume> Volume = vtkSmartPointer<vtkErVolume>::New();

	Volume->SetInputConnection(0, ImageCast->GetOutputPort());
	Volume->SetFilterMode(ExposureRender::Enums::Linear);
	Volume->SetAcceleratorType(ExposureRender::Enums::NoAcceleration);
	Volume->Update();

	Tracer->AddInputConnection(vtkErTracer::VolumesPort, Volume->GetOutputPort());

	vtkSmartPointer<vtkPiecewiseFunction> Opacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	
	const float MaxDistance = 10.0f;

	Opacity->AddPoint(0, 1);
	Opacity->AddPoint(MaxDistance, 0);
	
	Tracer->SetOpacity(Opacity, VolumeID);
	
	vtkSmartPointer<vtkColorTransferFunction> Emission = vtkSmartPointer<vtkColorTransferFunction>::New();
	
	const float EmissionStrength = 2.0f;

	Emission->AddRGBPoint(0, EmissionStrength, 0, 0);
	Emission->AddRGBPoint(MaxDistance, 0, EmissionStrength, 0);
	Emission->AddRGBPoint(MaxDistance + 0.01f, 0, 0, 0);

	Tracer->SetEmission(Emission, VolumeID);
}

void LoadVolume(vtkErTracer* Tracer)
{
#ifdef UAH
	vtkSmartPointer<vtkImageCast> SegmentationVolumeImageCast = vtkSmartPointer<vtkImageCast>::New();
	SegmentationVolumeImageCast->SetOutputScalarTypeToUnsignedShort();
	SegmentationVolumeImageCast->SetClampOverflow(1);

	vtkSmartPointer<vtkMetaImageReader> SegmentationVolumeReader = vtkSmartPointer<vtkMetaImageReader>::New();
	
	if (SegmentationVolumeReader->CanReadFile(gSegmentation) == 0)
	{
		printf("Can't read segmentation volume!");
		exit(EXIT_FAILURE);
	}
	else
	{
		SegmentationVolumeReader->SetFileName(gSegmentation);
	}
	
	SegmentationVolumeReader->Update();

	SegmentationVolumeImageCast->SetInputConnection(0, SegmentationVolumeReader->GetOutputPort());
	SegmentationVolumeImageCast->Update();

	vtkSmartPointer<vtkErVolume> SegmentationVolume = vtkSmartPointer<vtkErVolume>::New();

	SegmentationVolume->SetInputConnection(0, SegmentationVolumeImageCast->GetOutputPort());
	SegmentationVolume->SetFilterMode(ExposureRender::Enums::NearestNeighbour);
	SegmentationVolume->SetAcceleratorType(ExposureRender::Enums::NoAcceleration);
	SegmentationVolume->Update();

	Tracer->AddInputConnection(vtkErTracer::VolumesPort, SegmentationVolume->GetOutputPort());
	
//	LoadDistanceField(Tracer, gRiskNerves, 10, 1);
//	LoadDistanceField(Tracer, gRiskVeins, 10, 1);
	LoadDistanceField(Tracer, gRiskArteries, 10, 1);


#else
	vtkSmartPointer<vtkMetaImageReader> Reader	= vtkSmartPointer<vtkMetaImageReader>::New();
	
	Reader->SetFileName(gIntensityVolumeFileName);
	
	if (Reader->CanReadFile(gIntensityVolumeFileName) == 0)
	{
		printf("Can't read intensity volume!");
		exit(EXIT_FAILURE);
	}

	Reader->Update();

	

	

	Volume->SetInputConnection(0, ImageCast->GetOutputPort());
	Volume->SetFilterMode(ExposureRender::Enums::Linear);
	Volume->SetAcceleratorType(ExposureRender::Enums::NoAcceleration);

	Volume->SetAlignmentType(ExposureRender::Enums::AxisAlign);
//	Volume->SetAxis(ExposureRender::Enums::X);
	Volume->SetPosition(0, 0, 0);

	

	Tracer->SetInputConnection(vtkErTracer::VolumesPort, Volume->GetOutputPort());
#endif
}

void CreateCamera(vtkRenderer* Renderer)
{
	vtkSmartPointer<vtkErCamera> Camera = vtkSmartPointer<vtkErCamera>::New();

	Camera->SetExposure(1);
	
	Camera->SetApertureShape(ExposureRender::Enums::Polygon);
	Camera->SetApertureSize(0.0f);
	Camera->SetNoApertureBlades(5);
	Camera->SetApertureAngle(0.0f);

	Renderer->SetActiveCamera(Camera);
	Renderer->ResetCamera();

	Camera->SetClippingRange(0, 1000000);
}

void CreateLighting(vtkErTracer* Tracer)
{
	vtkSmartPointer<vtkErTexture> KeyLightTexture = vtkSmartPointer<vtkErTexture>::New();

	KeyLightTexture->SetTextureType(ExposureRender::Enums::Procedural);
	KeyLightTexture->SetProceduralType(ExposureRender::Enums::Uniform);
	KeyLightTexture->SetUniformColor(1, 1, 1);

	vtkSmartPointer<vtkErTexture> RimLightTexture = vtkSmartPointer<vtkErTexture>::New();

	RimLightTexture->SetTextureType(ExposureRender::Enums::Procedural);
	RimLightTexture->SetProceduralType(ExposureRender::Enums::Uniform);
	RimLightTexture->SetOutputLevel(1.0f);

	vtkSmartPointer<vtkColorTransferFunction> Gradient = vtkSmartPointer<vtkColorTransferFunction>::New();
	
	Gradient->AddRGBPoint(0.0f, 0.0025f, 0.0025f, 0.0025f);
	Gradient->AddRGBPoint(0.5f, 0.01f, 0.01f, 0.01f);
	Gradient->AddRGBPoint(0.50001f, 0.9f, 0.8f, 0.5f);
	Gradient->AddRGBPoint(0.51f, 0.9f, 0.6f, 0.2f);
	Gradient->AddHSVPoint(1.0f, 0.6f, 0.9f, 0.8f);

	RimLightTexture->SetGradient(Gradient);
	
	/*
	vtkSmartPointer<vtkErTexture> ImageTexture = vtkSmartPointer<vtkErTexture>::New();

	ImageTexture->SetTextureType(ExposureRender::Enums::Bitmap);

	vtkSmartPointer<vtkJPEGReader> JpegReader = vtkSmartPointer<vtkJPEGReader>::New();

	JpegReader->SetFileName("C:\\Users\\Thomas\\Desktop\\panorama.jpg");
	JpegReader->Update();

	vtkSmartPointer<vtkErBitmap> Bitmap = vtkSmartPointer<vtkErBitmap>::New();
	
	Bitmap->SetFilterMode(ExposureRender::Enums::Linear);
	Bitmap->SetInputConnection(0, JpegReader->GetOutputPort());

	ImageTexture->SetInputConnection(0, Bitmap->GetOutputPort());
	*/

	vtkSmartPointer<vtkErLight> KeyLight = vtkSmartPointer<vtkErLight>::New();
	
	const float KeyLightSize = 0.1f;

	KeyLight->SetAlignmentType(ExposureRender::Enums::Spherical);
	KeyLight->SetShapeType(ExposureRender::Enums::Plane);
	KeyLight->SetOuterRadius(0.01f);
	KeyLight->SetOneSided(true);
	KeyLight->SetElevation(145.0f);
	KeyLight->SetAzimuth(145.0f);
	KeyLight->SetOffset(1.5f);
	KeyLight->SetMultiplier(10.0f);
	KeyLight->SetSize(KeyLightSize, KeyLightSize, KeyLightSize);
	KeyLight->SetEmissionUnit(ExposureRender::Enums::Power);
	KeyLight->SetInputConnection(KeyLightTexture->GetOutputPort());
	KeyLight->SetEnabled(true);

	vtkSmartPointer<vtkErLight> RimLight = vtkSmartPointer<vtkErLight>::New();
	
	const float RimLightSize = 2.0f;

	RimLight->SetAlignmentType(ExposureRender::Enums::AxisAlign);
	RimLight->SetAxis(ExposureRender::Enums::Y);
	RimLight->SetPosition(0.0f, 0.0f, 0.0f);
	RimLight->SetShapeType(ExposureRender::Enums::Sphere);
	RimLight->SetOneSided(false);
	RimLight->SetOuterRadius(20.0f);
	RimLight->SetElevation(45.0f);
	RimLight->SetAzimuth(125.0f);
	RimLight->SetOffset(30.0f);
	RimLight->SetMultiplier(1000.0f);
	RimLight->SetSize(RimLightSize, RimLightSize, RimLightSize);
	RimLight->SetEmissionUnit(ExposureRender::Enums::Power);
	RimLight->SetInputConnection(RimLightTexture->GetOutputPort());
	RimLight->SetEnabled(false);

	Tracer->AddInputConnection(vtkErTracer::LightsPort, KeyLight->GetOutputPort());
	Tracer->AddInputConnection(vtkErTracer::LightsPort, RimLight->GetOutputPort());
}

void CreateObjects(vtkErTracer* Tracer)
{
	vtkSmartPointer<vtkErTexture> DiffuseTexture = vtkSmartPointer<vtkErTexture>::New();

	DiffuseTexture->SetTextureType(ExposureRender::Enums::Procedural);
	DiffuseTexture->SetProceduralType(ExposureRender::Enums::Checker);
	DiffuseTexture->SetRepeat(4, 4);
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
#ifdef UAH
	vtkSmartPointer<vtkPiecewiseFunction> SegmentationOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	
	SegmentationOpacity->AddSegment(0, 0, 2, 0);
	SegmentationOpacity->AddSegment(2, 1, 35, 1);
	
	Tracer->SetOpacity(SegmentationOpacity, 0);

	vtkSmartPointer<vtkColorTransferFunction> SegmentationDiffuse = vtkSmartPointer<vtkColorTransferFunction>::New();
	
	SegmentationDiffuse->AddRGBSegment(0, 0, 0, 0, 2, 0, 0, 0);

	for (int i = 2; i < 36; i++)
	{
		float H = (float)rand() / RAND_MAX;
//		SegmentationDiffuse->AddHSVPoint(i, H, 0.75, 0.75);
//		SegmentationDiffuse->AddHSVPoint(i + 1, H, 0.75, 0.75);
		SegmentationDiffuse->AddHSVPoint(i + 1, 0, 0, 0.5);
	}

	Tracer->SetDiffuse(SegmentationDiffuse, 0);

	
#else
#endif
}
