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

#include "vtkErDll.h"
#include "vtkErBindable.h"
#include "vtkErVolumeProperty.h"

#include <vtkDataObject.h>
#include <vtkVolumeMapper.h>
#include <vtkSmartPointer.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkTextActor.h>

using namespace ExposureRender;

class vtkErTracerData : public vtkDataObject, public vtkErBindableTracer
{
public:
	static vtkErTracerData* New();
	vtkTypeRevisionMacro(vtkErTracerData, vtkDataObject);
	
protected:
	vtkErTracerData() {};
	virtual ~vtkErTracerData() {};

private:
	vtkErTracerData(const vtkErTracerData& Other);		// Not implemented.
    void operator = (const vtkErTracerData& Other);		// Not implemented.
};

class VTK_ER_EXPORT vtkErTracer : public vtkAbstractVolumeMapper
{
public:
	static vtkErTracer* New();
	vtkTypeRevisionMacro(vtkErTracer, vtkAbstractVolumeMapper);

	enum Ports
	{
		VolumesPort = 0,
		ObjectsPort,
		ClippingObjectsPort
	};

	vtkErVolumeProperty* GetVolumeProperty()						{	return this->VolumeProperty.GetPointer();										};
	void SetVolumeProperty(vtkErVolumeProperty* VolumeProperty)		{	this->VolumeProperty = VolumeProperty; this->VolumeProperty->Modified();		};

	vtkGetMacro(NoiseReduction, bool);
	vtkSetMacro(NoiseReduction, bool);

	vtkGetMacro(ShowStatistics, bool);
	vtkSetMacro(ShowStatistics, bool);

protected:
	vtkErTracer();
    virtual ~vtkErTracer();

	virtual int FillInputPortInformation(int Port, vtkInformation* Info);
	virtual int FillOutputPortInformation(int Port, vtkInformation* Info);

	virtual void BeforeRender(vtkRenderer* Renderer, vtkVolume* Volume);
	virtual void Render(vtkRenderer* Renderer, vtkVolume* Volume);
	virtual void AfterRender(vtkRenderer* Renderer, vtkVolume* Volume);

private:
	unsigned int							TextureID;
	ExposureRender::ColorRGBAuc*			ImageBuffer;
	Vec2i									LastRenderSize;
	unsigned long							CameraTimeStamp;
	vtkSmartPointer<vtkErVolumeProperty>	VolumeProperty;
	unsigned long							VolumePropertyTimeStamp;
	ExposureRender::ErTracer				Tracer;
	bool									NoiseReduction;
	bool									ShowStatistics;
	vtkSmartPointer<vtkTextActor>			NameTextActor;
	vtkSmartPointer<vtkTextActor>			DurationTextActor;
	ExposureRender::Statistics				Statistics;
};
