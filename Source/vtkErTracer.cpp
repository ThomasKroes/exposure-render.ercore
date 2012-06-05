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

#include "vtkErStable.h"
#include "vtkErTracer.h"
#include "vtkErCamera.h"
#include "vtkErVolume.h"
#include "vtkErLight.h"
#include "vtkErObject.h"
#include "vtkErClippingObject.h"

#include "vtkgl.h"

vtkStandardNewMacro(vtkErTracerData);
vtkCxxRevisionMacro(vtkErTracerData, "$Revision: 1.0 $");

vtkStandardNewMacro(vtkErTracer);
vtkCxxRevisionMacro(vtkErTracer, "$Revision: 1.0 $");

vtkErTracer::vtkErTracer(void)
{
	this->SetNumberOfInputPorts(4);
	this->SetNumberOfOutputPorts(0);

	glGenTextures(1, &TextureID);

	this->ImageBuffer		= NULL;
	this->RenderSize[0]		= 0;
	this->RenderSize[1]		= 0;
	this->CameraTimeStamp	= 0;

	for (int i = 0; i < MAX_NO_VOLUMES; i++)
	{
		this->VolumeProperties[i]			= vtkSmartPointer<vtkErVolumeProperty>::New();
		this->VolumePropertiesTimeStamp[i]	= this->VolumeProperties[i]->GetMTime();
	}

	this->Tracer.SetDirty();
}

vtkErTracer::~vtkErTracer(void)
{
	delete[] this->ImageBuffer;
}

int vtkErTracer::FillInputPortInformation(int Port, vtkInformation* Info)
{
	switch (Port)
	{
		case VolumesPort:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErVolumeData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
			return 1;
		}

		case LightsPort:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErLightData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
			return 1;
		}

		case ObjectsPort:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErObjectData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
			return 1;
		}

		case ClippingObjectsPort:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErClippingObjectData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
			return 1;
		}
	}

	return 0;
}

int vtkErTracer::FillOutputPortInformation(int Port, vtkInformation* Info)
{
	return 1;
}

void vtkErTracer::BeforeRender(vtkRenderer* Renderer, vtkVolume* Volume)
{
	for (int i = 0; i < MAX_NO_VOLUMES; i++)
	{
		if (this->VolumePropertiesTimeStamp[i] >= this->VolumeProperties[i]->GetMTime())
			continue;

		vtkErVolumeProperty* VolumeProperty = this->VolumeProperties[i].GetPointer();
		
		ExposureRender::VolumeProperty& ErVolumeProperty = this->Tracer.VolumeProperties[i];

		ErVolumeProperty.Opacity1D.Reset();
		
		for (int j = 0; j < VolumeProperty->GetOpacity()->GetSize(); j++)
		{
			double NodeValue[4];
			VolumeProperty->GetOpacity()->GetNodeValue(j, NodeValue);
			ErVolumeProperty.Opacity1D.AddNode(ExposureRender::ScalarNode(NodeValue[0], NodeValue[1]));
		}
		
		ErVolumeProperty.Diffuse1D.Reset();

		for (int j = 0; j < VolumeProperty->GetDiffuse()->GetSize(); j++)
		{
			double NodeValue[6];
			VolumeProperty->GetDiffuse()->GetNodeValue(j, NodeValue);
			ErVolumeProperty.Diffuse1D.AddNode(ExposureRender::ColorNode::FromRGB(NodeValue[0], ExposureRender::ColorRGBf(NodeValue[1], NodeValue[2], NodeValue[3])));
		}
		
		ErVolumeProperty.Specular1D.Reset();

		for (int j = 0; j < VolumeProperty->GetSpecular()->GetSize(); j++)
		{
			double NodeValue[6];
			VolumeProperty->GetSpecular()->GetNodeValue(j, NodeValue);
			ErVolumeProperty.Specular1D.AddNode(ExposureRender::ColorNode::FromRGB(NodeValue[0], ExposureRender::ColorRGBf(NodeValue[1], NodeValue[2], NodeValue[3])));
		}

		ErVolumeProperty.Glossiness1D.Reset();
		
		for (int j = 0; j < VolumeProperty->GetGlossiness()->GetSize(); j++)
		{
			double NodeValue[4];
			VolumeProperty->GetGlossiness()->GetNodeValue(j, NodeValue);
			ErVolumeProperty.Glossiness1D.AddNode(ExposureRender::ScalarNode(NodeValue[0], NodeValue[1]));
		}

		ErVolumeProperty.IndexOfReflection1D.Reset();
		
		for (int j = 0; j < VolumeProperty->GetIndexOfReflection()->GetSize(); j++)
		{
			double NodeValue[4];
			VolumeProperty->GetIndexOfReflection()->GetNodeValue(j, NodeValue);
			ErVolumeProperty.IndexOfReflection1D.AddNode(ExposureRender::ScalarNode(NodeValue[0], NodeValue[1]));
		}

		ErVolumeProperty.Emission1D.Reset();

		for (int j = 0; j < VolumeProperty->GetEmission()->GetSize(); j++)
		{
			double NodeValue[6];
			VolumeProperty->GetEmission()->GetNodeValue(j, NodeValue);
			ErVolumeProperty.Emission1D.AddNode(ExposureRender::ColorNode::FromRGB(NodeValue[0], ExposureRender::ColorRGBf(NodeValue[1], NodeValue[2], NodeValue[3])));
		}

		ErVolumeProperty.StepFactorPrimary 	= VolumeProperty->GetStepFactorPrimary();
		ErVolumeProperty.StepFactorShadow	= VolumeProperty->GetStepFactorShadow();
		ErVolumeProperty.Shadows			= VolumeProperty->GetShadows();
		ErVolumeProperty.ShadingType		= VolumeProperty->GetShadingMode();
		ErVolumeProperty.DensityScale		= VolumeProperty->GetDensityScale();
		ErVolumeProperty.OpacityModulated	= VolumeProperty->GetOpacityModulated();
		ErVolumeProperty.GradientMode		= VolumeProperty->GetGradientMode();
		ErVolumeProperty.GradientThreshold	= VolumeProperty->GetGradientThreshold();
		ErVolumeProperty.GradientFactor		= VolumeProperty->GetGradientFactor();

		this->VolumePropertiesTimeStamp[i] = this->VolumeProperties[i]->GetMTime();
	}
	
	vtkCamera* Camera = Renderer->GetActiveCamera();
	
	if (Camera->GetMTime() != this->CameraTimeStamp)
	{
		this->Tracer.SetDirty();
		this->CameraTimeStamp = Camera->GetMTime();

		if (Camera)
		{
			this->Tracer.Camera.FilmSize		= Vec2i(Renderer->GetRenderWindow()->GetSize()[0], Renderer->GetRenderWindow()->GetSize()[1]);
			this->Tracer.Camera.Pos				= Vec3f(Camera->GetPosition()[0], Camera->GetPosition()[1], Camera->GetPosition()[2]);
			this->Tracer.Camera.Target			= Vec3f(Camera->GetFocalPoint()[0], Camera->GetFocalPoint()[1], Camera->GetFocalPoint()[2]);
			this->Tracer.Camera.Up				= Vec3f(Camera->GetViewUp()[0], Camera->GetViewUp()[1], Camera->GetViewUp()[2]);
			this->Tracer.Camera.ClipNear		= 0.0f;//Camera->GetClippingRange()[0];
			this->Tracer.Camera.ClipFar			= 10000.0f;//Camera->GetClippingRange()[1];
			this->Tracer.Camera.FOV				= Camera->GetViewAngle();
		}

		vtkErCamera* ErCamera = dynamic_cast<vtkErCamera*>(Camera);

		if (ErCamera)
		{
			this->Tracer.Camera.FocalDistance		= ErCamera->GetFocalDistance();
			this->Tracer.Camera.Exposure			= ErCamera->GetExposure();
			this->Tracer.Camera.Gamma				= ErCamera->GetGamma();
			this->Tracer.Camera.ApertureShape		= ErCamera->GetApertureShape();
			this->Tracer.Camera.ApertureSize		= ErCamera->GetApertureSize();
			this->Tracer.Camera.NoApertureBlades	= ErCamera->GetNoApertureBlades();
			this->Tracer.Camera.ApertureAngle		= ErCamera->GetApertureAngle();
		}
	}
	
	const int NoVolumes = this->GetNumberOfInputConnections(VolumesPort);

	this->Tracer.VolumeIDs.Count = 0;

	for (int i = 0; i < NoVolumes; i++)
	{
		vtkErVolumeData* VolumeData = vtkErVolumeData::SafeDownCast(this->GetInputDataObject(VolumesPort, i));

		if (VolumeData)
		{
			this->Tracer.VolumeIDs[this->Tracer.VolumeIDs.Count] = VolumeData->Bindable.ID;
			this->Tracer.VolumeIDs.Count++;
		}
	}

	const int NoLights = this->GetNumberOfInputConnections(LightsPort);

	this->Tracer.LightIDs.Count = 0;

	for (int i = 0; i < NoLights; i++)
	{
		vtkErLightData* LightData = vtkErLightData::SafeDownCast(this->GetInputDataObject(LightsPort, i));

		if (LightData && LightData->Bindable.Enabled)
		{
			this->Tracer.LightIDs[this->Tracer.LightIDs.Count] = LightData->Bindable.ID;
			this->Tracer.LightIDs.Count++;

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					LightData->Bindable.Shape.Alignment.OffsetTM.NN[i][j] = Camera->GetViewTransformMatrix()->GetElement(i, j);
			
			if (LightData->Light->GetUseCameraFocalPoint())
			{
				LightData->Bindable.Shape.Alignment.OffsetTM.NN[0][3] = Camera->GetFocalPoint()[0];
				LightData->Bindable.Shape.Alignment.OffsetTM.NN[1][3] = Camera->GetFocalPoint()[1];
				LightData->Bindable.Shape.Alignment.OffsetTM.NN[2][3] = Camera->GetFocalPoint()[2];
			}

			LightData->Bind();
		}
	}

	const int NoObjects = this->GetNumberOfInputConnections(ObjectsPort);

	this->Tracer.ObjectIDs.Count = 0;
	
	for (int i = 0; i < NoObjects; i++)
	{
		vtkErObjectData* ObjectData = vtkErObjectData::SafeDownCast(this->GetInputDataObject(ObjectsPort, i));

		if (ObjectData && ObjectData->Bindable.Enabled)
		{
			this->Tracer.ObjectIDs[this->Tracer.ObjectIDs.Count] = ObjectData->Bindable.ID;
			this->Tracer.ObjectIDs.Count++;

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					ObjectData->Bindable.Shape.Alignment.OffsetTM.NN[i][j] = Camera->GetViewTransformMatrix()->GetElement(i, j);

			if (ObjectData->Object->GetUseCameraFocalPoint())
			{
				ObjectData->Bindable.Shape.Alignment.OffsetTM.NN[0][3] = Camera->GetFocalPoint()[0];
				ObjectData->Bindable.Shape.Alignment.OffsetTM.NN[1][3] = Camera->GetFocalPoint()[1];
				ObjectData->Bindable.Shape.Alignment.OffsetTM.NN[2][3] = Camera->GetFocalPoint()[2];
			}

			ObjectData->Bind();
		}
	}

	const int NoClippingObjects = this->GetNumberOfInputConnections(ClippingObjectsPort);

	this->Tracer.ClippingObjectIDs.Count = 0;
	
	for (int i = 0; i < NoClippingObjects; i++)
	{
		vtkErClippingObjectData* ClippingObjectData = vtkErClippingObjectData::SafeDownCast(this->GetInputDataObject(ClippingObjectsPort, i));

		if (ClippingObjectData && ClippingObjectData->Bindable.Enabled)
		{
			this->Tracer.ClippingObjectIDs[this->Tracer.ClippingObjectIDs.Count] = ClippingObjectData->Bindable.ID;
			this->Tracer.ClippingObjectIDs.Count++;

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					ClippingObjectData->Bindable.Shape.Alignment.OffsetTM.NN[i][j] = Camera->GetViewTransformMatrix()->GetElement(i, j);

			if (ClippingObjectData->ClippingObject->GetUseCameraFocalPoint())
			{
				ClippingObjectData->Bindable.Shape.Alignment.OffsetTM.NN[0][3] = Camera->GetFocalPoint()[0];
				ClippingObjectData->Bindable.Shape.Alignment.OffsetTM.NN[1][3] = Camera->GetFocalPoint()[1];
				ClippingObjectData->Bindable.Shape.Alignment.OffsetTM.NN[2][3] = Camera->GetFocalPoint()[2];
			}

			ClippingObjectData->Bind();
		}
	}

	if (this->Tracer.GetDirty())
	{
		ER_CALL(ExposureRender::BindTracer(this->Tracer));
		this->Tracer.SetDirty(false);
	}
}

void vtkErTracer::Render(vtkRenderer* Renderer, vtkVolume* Volume)
{
	this->InvokeEvent(vtkCommand::VolumeMapperRenderStartEvent,0);

	this->BeforeRender(Renderer, Volume);

	int* WindowSize = Renderer->GetRenderWindow()->GetSize();

	if (WindowSize[0] != this->RenderSize[0] || WindowSize[1] != this->RenderSize[1])
	{
		RenderSize[0] = WindowSize[0];
		RenderSize[1] = WindowSize[1];

		delete[] this->ImageBuffer;
		this->ImageBuffer = new ExposureRender::ColorRGBAuc[this->RenderSize[0] * this->RenderSize[1]];
	}

	ER_CALL(ExposureRender::Render(this->Tracer.ID));
	ER_CALL(ExposureRender::GetRunningEstimate(this->Tracer.ID, this->ImageBuffer));

	glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, TextureID);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, RenderSize[0], RenderSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)this->ImageBuffer);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	double d = 0.5;
	
    Renderer->SetDisplayPoint(0,0,d);
    Renderer->DisplayToWorld();
    double coordinatesA[4];
    Renderer->GetWorldPoint(coordinatesA);

    Renderer->SetDisplayPoint(RenderSize[0],0,d);
    Renderer->DisplayToWorld();
    double coordinatesB[4];
    Renderer->GetWorldPoint(coordinatesB);

    Renderer->SetDisplayPoint(RenderSize[0], RenderSize[1],d);
    Renderer->DisplayToWorld();
    double coordinatesC[4];
    Renderer->GetWorldPoint(coordinatesC);

    Renderer->SetDisplayPoint(0,RenderSize[1],d);
    Renderer->DisplayToWorld();
    double coordinatesD[4];
    Renderer->GetWorldPoint(coordinatesD);
	
	glPushAttrib(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex4dv(coordinatesA);
		glTexCoord2i(1, 0);
		glVertex4dv(coordinatesB);
		glTexCoord2i(1, 1);
		glVertex4dv(coordinatesC);
		glTexCoord2i(0, 1);
		glVertex4dv(coordinatesD);
	glEnd();

	glPopAttrib();

	this->InvokeEvent(vtkCommand::VolumeMapperRenderEndEvent,0);
}
