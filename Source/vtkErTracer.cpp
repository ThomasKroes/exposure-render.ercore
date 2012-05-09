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

#include "vtkgl.h"

vtkStandardNewMacro(vtkErTracerData);
vtkCxxRevisionMacro(vtkErTracerData, "$Revision: 1.0 $");

vtkStandardNewMacro(vtkErTracer);
vtkCxxRevisionMacro(vtkErTracer, "$Revision: 1.0 $");

vtkErTracer::vtkErTracer(void)
{
	this->ImageBuffer = NULL;

	this->RenderSize[0]	= 0;
	this->RenderSize[1]	= 0;

	this->SetNumberOfInputPorts(4);
	this->SetNumberOfOutputPorts(0);

	this->Opacity		= vtkPiecewiseFunction::New();
	this->Diffuse[0]	= vtkPiecewiseFunction::New();
	this->Diffuse[1]	= vtkPiecewiseFunction::New();
	this->Diffuse[2]	= vtkPiecewiseFunction::New();
	this->Specular[0]	= vtkPiecewiseFunction::New();
	this->Specular[1]	= vtkPiecewiseFunction::New();
	this->Specular[2]	= vtkPiecewiseFunction::New();
	this->Glossiness	= vtkPiecewiseFunction::New();
	this->Emission[0]	= vtkPiecewiseFunction::New();
	this->Emission[1]	= vtkPiecewiseFunction::New();
	this->Emission[2]	= vtkPiecewiseFunction::New();

	double Min = 0, Max = 255;

	Opacity->AddPoint(Min, 0, 0.5, 0);
	Opacity->AddPoint(Max, 1, 0.5, 0);

	for (int i = 0; i < 3; i++)
	{
		Diffuse[i]->AddPoint(Min, 0.0, 0.5, 0);
		Diffuse[i]->AddPoint(Max, 1.0, 0.5, 0);
	}

	for (int i = 0; i < 3; i++)
	{
		Specular[i]->AddPoint(Min, 0, 0.5, 0);
		Specular[i]->AddPoint(Max, 0, 0.5, 0);
	}

	Glossiness->AddPoint(Min, 1, 0.5, 0);
	Glossiness->AddPoint(Max, 1, 0.5, 0);

	for (int i = 0; i < 3; i++)
	{
		Emission[i]->AddPoint(Min, 10000000, 0.5, 0);
		Emission[i]->AddPoint(Max, 1000, 0.5, 0);
	}

	this->SetStepFactorPrimary(1.0f);
	this->SetStepFactorShadow(1.0f);
	this->SetShadows(true);
	this->SetMaxShadowDistance(2.0f);
	this->SetShadingType(1);
	this->SetDensityScale(100.1f);
	this->SetOpacityModulated(true);
	this->SetGradientComputation(1);
	this->SetGradientThreshold(0.5f);
	this->SetGradientFactor(1.0f);

	glGenTextures(1, &TextureID);
}

vtkErTracer::~vtkErTracer(void)
{
	delete[] this->ImageBuffer;
}

int vtkErTracer::FillInputPortInformation(int Port, vtkInformation* Info)
{
	switch (Port)
	{
		case 0:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErVolumeData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
			return 1;
		}

		case 1:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErLightData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
			return 1;
		}

		case 2:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErObjectData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
			return 1;
		}

		case 3:
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

bool vtkErTracer::BeforeRender(vtkRenderer* Renderer, vtkVolume* Volume)
{
	this->Tracer.Opacity1D.Reset();
	
	for (int i = 0; i < this->Opacity->GetSize(); i++)
	{
		double NodeValue[4];
		this->Opacity->GetNodeValue(i, NodeValue);
		this->Tracer.Opacity1D.AddNode(ExposureRender::ScalarNode(NodeValue[0], NodeValue[1]));
	}

	this->Tracer.Diffuse1D.Reset();

	for (int i = 0; i < this->Diffuse[0]->GetSize(); i++)
	{
		double NodeValue[3][4];
		this->Diffuse[0]->GetNodeValue(i, NodeValue[0]);
		this->Diffuse[1]->GetNodeValue(i, NodeValue[1]);
		this->Diffuse[2]->GetNodeValue(i, NodeValue[2]);
		this->Tracer.Diffuse1D.AddNode(ExposureRender::ColorNode::FromRGB(NodeValue[0][0], ExposureRender::ColorRGBf(NodeValue[0][1], NodeValue[1][1], NodeValue[2][1])));
	}
	
	this->Tracer.Specular1D.Reset();

	for (int i = 0; i < this->Specular[0]->GetSize(); i++)
	{
		double NodeValue[3][4];
		this->Specular[0]->GetNodeValue(i, NodeValue[0]);
		this->Specular[1]->GetNodeValue(i, NodeValue[1]);
		this->Specular[2]->GetNodeValue(i, NodeValue[2]);
		this->Tracer.Specular1D.AddNode(ExposureRender::ColorNode::FromRGB(NodeValue[0][0], ExposureRender::ColorRGBf(NodeValue[0][1], NodeValue[1][1], NodeValue[2][1])));
	}

	this->Tracer.Emission1D.Reset();

	for (int i = 0; i < this->Emission[0]->GetSize(); i++)
	{
		double NodeValue[3][4];
		this->Emission[0]->GetNodeValue(i, NodeValue[0]);
		this->Emission[1]->GetNodeValue(i, NodeValue[1]);
		this->Emission[2]->GetNodeValue(i, NodeValue[2]);
		this->Tracer.Emission1D.AddNode(ExposureRender::ColorNode::FromRGB(NodeValue[0][0], ExposureRender::ColorRGBf(NodeValue[0][1], NodeValue[1][1], NodeValue[2][1])));
	}

	this->Tracer.RenderSettings.Traversal.StepFactorPrimary 	= 5;//this->StepFactorPrimary;
	this->Tracer.RenderSettings.Traversal.StepFactorShadow		= 5;//this->StepFactorShadow;
	this->Tracer.RenderSettings.Traversal.Shadows				= this->Shadows;
	this->Tracer.RenderSettings.Traversal.MaxShadowDistance		= this->MaxShadowDistance;

	this->Tracer.RenderSettings.Shading.Type				= (ExposureRender::Enums::ShadingMode)this->ShadingType;
	this->Tracer.RenderSettings.Shading.DensityScale		= this->DensityScale;
	this->Tracer.RenderSettings.Shading.OpacityModulated	= this->OpacityModulated;
	this->Tracer.RenderSettings.Shading.GradientComputation	= this->GradientComputation;
	this->Tracer.RenderSettings.Shading.GradientThreshold	= this->GradientThreshold;
	this->Tracer.RenderSettings.Shading.GradientFactor		= this->GradientFactor;

	vtkCamera* Camera = Renderer->GetActiveCamera();
	
	if (Camera)
	{
		this->Tracer.Camera.FilmSize		= Vec2i(Renderer->GetRenderWindow()->GetSize()[0], Renderer->GetRenderWindow()->GetSize()[1]);
		this->Tracer.Camera.Pos				= Vec3f(Camera->GetPosition()[0], Camera->GetPosition()[1], Camera->GetPosition()[2]);
		this->Tracer.Camera.Target			= Vec3f(Camera->GetFocalPoint()[0], Camera->GetFocalPoint()[1], Camera->GetFocalPoint()[2]);
		this->Tracer.Camera.Up				= Vec3f(Camera->GetViewUp()[0], Camera->GetViewUp()[1], Camera->GetViewUp()[2]);
		this->Tracer.Camera.ApertureSize	= 0.0f;//Camera->GetFocalDisk();
		this->Tracer.Camera.ClipNear		= 0.0f;//Camera->GetClippingRange()[0];
		this->Tracer.Camera.ClipFar			= 1000.0f;//Camera->GetClippingRange()[1];
		this->Tracer.Camera.FOV				= 45;//Camera->GetViewAngle();
	}

	vtkErCamera* ErCamera = dynamic_cast<vtkErCamera*>(Camera);

	if (ErCamera)
	{
		this->Tracer.Camera.FocalDistance	= 1.0f;//ErCamera->GetFocalDistance();
		this->Tracer.Camera.Exposure		= 100.0f;//ErCamera->GetExposure();
		this->Tracer.Camera.Gamma			= 2.2f;//ErCamera->GetGamma();
	}
	
	vtkErVolumeData* VolumeData = dynamic_cast<vtkErVolumeData*>(this->GetInputDataObject(0, 0));

	this->Tracer.VolumeID = VolumeData->Bindable.ID;
	
	ExposureRender::BindTracer(this->Tracer);

	return true;
}

void vtkErTracer::Render(vtkRenderer* Renderer, vtkVolume* Volume)
{
	if (!this->BeforeRender(Renderer, Volume))
		return;

	int* WindowSize = Renderer->GetRenderWindow()->GetSize();

	if (WindowSize[0] != this->RenderSize[0] || WindowSize[1] != this->RenderSize[1])
	{
		RenderSize[0] = WindowSize[0];
		RenderSize[1] = WindowSize[1];

		delete[] this->ImageBuffer;
		this->ImageBuffer = new unsigned char[4 * this->RenderSize[0] * this->RenderSize[1]];
	}
	

	ExposureRender::RenderEstimate(this->Tracer.ID);
	ExposureRender::GetEstimate(this->Tracer.ID, this->ImageBuffer);

	glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, TextureID);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, RenderSize[0], RenderSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, this->ImageBuffer);
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
		glTexCoord2i(1,1);
		glVertex4dv(coordinatesA);
		glTexCoord2i(0,1);
		glVertex4dv(coordinatesB);
		glTexCoord2i(0,0);
		glVertex4dv(coordinatesC);
		glTexCoord2i(1,0);
		glVertex4dv(coordinatesD);
	glEnd();

	glPopAttrib();
}

void vtkErTracer::SetOpacity(vtkPiecewiseFunction* Opacity)
{
	if (this->Opacity != Opacity)
	{
		this->Opacity = Opacity;
		this->Modified();
	}
}

vtkPiecewiseFunction* vtkErTracer::GetOpacity(void)
{
	return Opacity.GetPointer();
}

void vtkErTracer::SetDiffuse(int Index, vtkPiecewiseFunction* Diffuse)
{
	if (this->Diffuse[Index] != Diffuse)
	{
		this->Diffuse[Index] = Diffuse;
		this->Modified();
	}
}

vtkPiecewiseFunction* vtkErTracer::GetDiffuse(int Index)
{
	return Diffuse[Index];
}

void vtkErTracer::SetSpecular(int Index, vtkPiecewiseFunction* Specular)
{
	if (this->Specular[Index] != Specular)
	{
		this->Specular[Index] = Specular;
		this->Modified();
	}
}

vtkPiecewiseFunction* vtkErTracer::GetSpecular(int Index)
{
	return Specular[Index];
}

void vtkErTracer::SetGlossiness(vtkPiecewiseFunction* Glossiness)
{
	if (this->Glossiness != Glossiness)
	{
		this->Glossiness = Glossiness;
		this->Modified();
	}
}

vtkPiecewiseFunction* vtkErTracer::GetGlossiness(void)
{
	return Glossiness.GetPointer();
}

void vtkErTracer::SetEmission(int Index, vtkPiecewiseFunction* Emission)
{
	if (this->Emission[Index] != Emission)
	{
		this->Emission[Index] = Emission;
		this->Modified();
	}
}

vtkPiecewiseFunction* vtkErTracer::GetEmission(int Index)
{
	return Emission[Index];
}
