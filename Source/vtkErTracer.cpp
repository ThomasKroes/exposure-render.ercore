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
	this->TracerTimeStamp			= 0;

	this->NameTextActor				= NULL;
	this->ValueTextActor				= NULL;
	this->UnitTextActor				= NULL;

	this->SetRenderMode(Enums::StochasticRayCasting);
	this->SetNoiseReduction(true);
	this->SetShowStatistics(true);

	this->Tracer.Modified();
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
	this->Tracer.SetRenderMode(this->GetRenderMode());

	if (this->VolumePropertyTimeStamp < this->VolumeProperty->GetMTime())
	{
		this->VolumeProperty->RequestData(this->Tracer.GetVolumeProperty());
		
		this->VolumePropertyTimeStamp = this->VolumeProperty->GetMTime();
		this->Tracer.Modified();
	}

	const Vec2i CurrentRenderSize = Vec2i(Renderer->GetRenderWindow()->GetSize()[0], Renderer->GetRenderWindow()->GetSize()[1]);

	if (this->LastRenderSize != CurrentRenderSize)
	{
		this->LastRenderSize = CurrentRenderSize;

		delete[] this->ImageBuffer;
		this->ImageBuffer = new ExposureRender::ColorRGBAuc[this->LastRenderSize[0] * this->LastRenderSize[1]];

		this->Tracer.GetCamera().SetFilmSize(this->LastRenderSize);
		this->Tracer.Modified();
	}

	vtkErCamera* Camera = dynamic_cast<vtkErCamera*>(Renderer->GetActiveCamera());
	
	if (Camera && Camera->GetMTime() != this->CameraTimeStamp)
	{
		Camera->RequestData(this->Tracer.GetCamera());

		this->CameraTimeStamp = Camera->GetMTime();
		this->Tracer.Modified();
	}
	
	const int NoVolumes = this->GetNumberOfInputConnections(VolumesPort);

	this->Tracer.GetVolumeIDs().Reset();

	for (int i = 0; i < NoVolumes; i++)
	{
		vtkErVolumeData* VolumeData = vtkErVolumeData::SafeDownCast(this->GetInputDataObject(VolumesPort, i));

		if (VolumeData)
			this->Tracer.GetVolumeIDs().Add(VolumeData->Bindable.ID);
	}

	const int NoObjects = this->GetNumberOfInputConnections(ObjectsPort);

	this->Tracer.GetObjectIDs().Reset();
	this->Tracer.GetLightIDs().Reset();
	this->Tracer.GetClippingObjectIDs().Reset();

	for (int i = 0; i < NoObjects; i++)
	{
		vtkErObjectData* ObjectData = vtkErObjectData::SafeDownCast(this->GetInputDataObject(ObjectsPort, i));

		if (ObjectData && ObjectData->Bindable.GetEnabled())
		{
			this->Tracer.GetObjectIDs().Add(ObjectData->Bindable.ID);

			if (ObjectData->Bindable.GetEmitter())
				this->Tracer.GetLightIDs().Add(ObjectData->Bindable.ID);

			if (ObjectData->Bindable.GetClip())
				this->Tracer.GetClippingObjectIDs().Add(ObjectData->Bindable.ID);

			ObjectData->Object->GetCameraOffset(Camera, ObjectData->Bindable.GetShape().GetAlignment().GetOffsetTM());
			ObjectData->Bind();
		}
	}

//	if (this->Tracer.GetNoiseReduction() != this->NoiseReduction)
//		this->Tracer.Modified();

	this->Tracer.SetNoiseReduction(this->NoiseReduction);

	if (this->TracerTimeStamp != this->Tracer.GetModifiedTime())
	{
		ER_CALL(ExposureRender::BindTracer(this->Tracer));
		this->TracerTimeStamp = this->Tracer.GetModifiedTime();
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
	
	if (this->ValueTextActor.GetPointer() == NULL)
	{
		this->ValueTextActor = vtkSmartPointer<vtkTextActor>::New();

		this->ValueTextActor->GetTextProperty()->SetFontSize(10);
		this->ValueTextActor->SetPosition(200, 20);

		Renderer->AddActor2D(this->ValueTextActor);

		this->ValueTextActor->GetTextProperty()->SetColor(0.9, 0.6, 0.2);
		this->ValueTextActor->GetTextProperty()->SetLineSpacing(1.3);
	}

	if (this->UnitTextActor.GetPointer() == NULL)
	{
		this->UnitTextActor = vtkSmartPointer<vtkTextActor>::New();

		this->UnitTextActor->GetTextProperty()->SetFontSize(10);
		this->UnitTextActor->SetPosition(250, 20);

		Renderer->AddActor2D(this->UnitTextActor);

		this->UnitTextActor->GetTextProperty()->SetColor(0.9, 0.6, 0.2);
		this->UnitTextActor->GetTextProperty()->SetLineSpacing(1.3);
	}
	
	if (this->ShowStatistics)
	{
		std::string NameString, ValueString, UnitString;
	
		for (int i = 0; i < this->Statistics.GetCount(); i++)
		{
			Statistic Stat = this->Statistics.GetStatistic(i);

			NameString.append(Stat.GetName());
			NameString.append("\n");

			char Value[MAX_CHAR_SIZE];
						
			sprintf_s(Value, MAX_CHAR_SIZE, Stat.GetValueFormat(), Stat.GetValue());

			ValueString.append(Value);
			ValueString.append("\n");

			UnitString.append(Stat.GetUnit());
			UnitString.append("\n");
		}
		
		this->NameTextActor->SetInput(NameString.c_str());
		this->ValueTextActor->SetInput(ValueString.c_str());
		this->UnitTextActor->SetInput(UnitString.c_str());
	}
}


