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

#include "vtkErStable.h"
#include "vtkErTracer.h"
#include "vtkErCamera.h"
#include "vtkErVolume.h"
#include "vtkErObject.h"

#include "vtkgl.h"

vtkStandardNewMacro(vtkErTracerData);
vtkCxxRevisionMacro(vtkErTracerData, "$Revision: 1.0 $");

vtkStandardNewMacro(vtkErTracer);
vtkCxxRevisionMacro(vtkErTracer, "$Revision: 1.0 $");

vtkErTracer::vtkErTracer(void)
{
	this->SetNumberOfInputPorts(2);
	this->SetNumberOfOutputPorts(0);

	glGenTextures(1, &TextureID);

	this->ImageBuffer				= NULL;
	this->LastRenderSize			= Vec2i(0);
	this->CameraTimeStamp			= 0;
	this->VolumeProperty			= vtkSmartPointer<vtkErVolumeProperty>::New();
	this->VolumePropertyTimeStamp	= 0;

	this->SetRenderMode(Enums::StochasticRayCasting);
	this->SetNoiseReduction(true);
	this->SetShowStatistics(true);

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

		case ObjectsPort:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErObjectData");
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
	this->Tracer.RenderMode	= this->GetRenderMode();

	if (this->VolumePropertyTimeStamp < this->VolumeProperty->GetMTime())
	{
		this->VolumeProperty->RequestData(this->Tracer.VolumeProperty);
		
		this->VolumePropertyTimeStamp = this->VolumeProperty->GetMTime();
		this->Tracer.SetDirty();
	}

	const Vec2i CurrentRenderSize = Vec2i(Renderer->GetRenderWindow()->GetSize()[0], Renderer->GetRenderWindow()->GetSize()[1]);

	if (this->LastRenderSize != CurrentRenderSize)
	{
		this->LastRenderSize = CurrentRenderSize;

		delete[] this->ImageBuffer;
		this->ImageBuffer = new ExposureRender::ColorRGBAuc[this->LastRenderSize[0] * this->LastRenderSize[1]];

		this->Tracer.Camera.SetFilmSize(this->LastRenderSize);
		this->Tracer.SetDirty();
	}

	vtkErCamera* Camera = dynamic_cast<vtkErCamera*>(Renderer->GetActiveCamera());
	
	if (Camera && Camera->GetMTime() != this->CameraTimeStamp)
	{
		Camera->RequestData(this->Tracer.Camera);

		this->CameraTimeStamp = Camera->GetMTime();
		this->Tracer.SetDirty();
	}
	
	const int NoVolumes = this->GetNumberOfInputConnections(VolumesPort);

	this->Tracer.VolumeIDs.Reset();

	for (int i = 0; i < NoVolumes; i++)
	{
		vtkErVolumeData* VolumeData = vtkErVolumeData::SafeDownCast(this->GetInputDataObject(VolumesPort, i));

		if (VolumeData)
			this->Tracer.VolumeIDs.Add(VolumeData->Bindable.ID);
	}

	const int NoObjects = this->GetNumberOfInputConnections(ObjectsPort);

	this->Tracer.ObjectIDs.Reset();
	this->Tracer.LightIDs.Reset();
	this->Tracer.ClippingObjectIDs.Reset();

	for (int i = 0; i < NoObjects; i++)
	{
		vtkErObjectData* ObjectData = vtkErObjectData::SafeDownCast(this->GetInputDataObject(ObjectsPort, i));

		if (ObjectData && ObjectData->Bindable.Enabled)
		{
			this->Tracer.ObjectIDs.Add(ObjectData->Bindable.ID);

			if (ObjectData->Bindable.Emitter)
				this->Tracer.LightIDs.Add(ObjectData->Bindable.ID);

			if (ObjectData->Bindable.Clip)
				this->Tracer.ClippingObjectIDs.Add(ObjectData->Bindable.ID);

			ObjectData->Object->GetCameraOffset(Camera, ObjectData->Bindable.Shape.GetAlignment().GetOffsetTM());
			ObjectData->Bind();
		}
	}

	if (this->Tracer.NoiseReduction != this->NoiseReduction)
		this->Tracer.SetDirty();

	this->Tracer.NoiseReduction = this->NoiseReduction;

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

	ER_CALL(ExposureRender::Render(this->Tracer.ID, this->Statistics));
	ER_CALL(ExposureRender::GetDisplayEstimate(this->Tracer.ID, this->ImageBuffer));

	glDrawPixels(this->LastRenderSize[0], this->LastRenderSize[1], GL_RGBA, GL_UNSIGNED_BYTE, this->ImageBuffer);

	this->AfterRender(Renderer, Volume);

	this->InvokeEvent(vtkCommand::VolumeMapperRenderEndEvent,0);
}

void vtkErTracer::AfterRender(vtkRenderer* Renderer, vtkVolume* Volume)
{
	if (this->NameTextActor.GetPointer() == NULL)
	{
		this->NameTextActor = vtkSmartPointer<vtkTextActor>::New();

		this->NameTextActor->GetTextProperty()->SetFontSize(10);
		this->NameTextActor->SetPosition(20, 20);

		Renderer->AddActor2D(this->NameTextActor);

		this->NameTextActor->GetTextProperty()->SetColor(0.9, 0.6, 0.2);
		this->NameTextActor->GetTextProperty()->SetLineSpacing(1.3);
	}
	
	if (this->DurationTextActor.GetPointer() == NULL)
	{
		this->DurationTextActor = vtkSmartPointer<vtkTextActor>::New();

		this->DurationTextActor->GetTextProperty()->SetFontSize(10);
		this->DurationTextActor->SetPosition(250, 20);

		Renderer->AddActor2D(this->DurationTextActor);

		this->DurationTextActor->GetTextProperty()->SetColor(0.9, 0.6, 0.2);
		this->DurationTextActor->GetTextProperty()->SetLineSpacing(1.3);
	}
	
	if (this->ShowStatistics)
	{
		std::string NameString, DurationString;
	
		NameString.append("FPS\n");

		char FPS[256];

		sprintf_s(FPS, 256, "%0.2f\n", this->Statistics.FPS);

		DurationString.append(FPS);

		for (int i = 0; i < this->Statistics.Count; i++)
		{
			NameString.append(this->Statistics.Timings[i].Name);
			NameString.append("\n");

			char Duration[256];

			sprintf_s(Duration, 256, "%0.2f", this->Statistics.Timings[i].Duration);

			DurationString.append(Duration);
			DurationString.append("\n");
		}
		
		this->NameTextActor->SetInput(NameString.c_str());
		this->DurationTextActor->SetInput(DurationString.c_str());
	}
}


