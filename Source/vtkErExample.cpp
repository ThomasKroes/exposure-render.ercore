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

#include "vtkErVolume.h"
#include "vtkErLight.h"
#include "vtkErTexture.h"
#include "vtkErTracer.h"
#include "vtkErCamera.h"

char gFileName[] = "C://Volumes//engine.mhd";

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
	
	vtkSmartPointer<vtkErTexture> ErTexture = vtkSmartPointer<vtkErTexture>::New();

	ErLight->SetInputConnection(ErTexture->GetOutputPort());

	vtkSmartPointer<vtkErTracer> VolumeMapper = vtkSmartPointer<vtkErTracer>::New();
	
	vtkSmartPointer<vtkErCamera> Camera = vtkSmartPointer<vtkErCamera>::New();

	Camera->SetClippingRange(0, 100000);
	Camera->SetExposure(0.01);

	vtkSmartPointer<vtkPiecewiseFunction> Opacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	
	Opacity->AddPoint(0, 0);
	Opacity->AddPoint(50, 0);
	Opacity->AddPoint(150, 1);
	Opacity->AddPoint(255, 1);

	VolumeMapper->SetOpacity(Opacity);

	ErVolume->SetInputConnection(0, Reader->GetOutputPort());
		
	ErVolume->Update();

//	vtkOutpu
	VolumeMapper->SetInputConnection(0, ErVolume->GetOutputPort());
	VolumeMapper->SetInputConnection(1, ErLight->GetOutputPort());

	VolumeMapper->Update();

	vtkSmartPointer<vtkVolume> Volume = vtkSmartPointer<vtkVolume>::New();
	Volume->Update();
	

	Volume->SetMapper(VolumeMapper);

	vtkSmartPointer<vtkRenderer> Renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();

	RenderWindow->AddRenderer(Renderer);
	
	vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

	RenderWindowInteractor->SetRenderWindow(RenderWindow);

	Renderer->AddVolume(Volume);
	Renderer->SetActiveCamera(Camera);
	Renderer->ResetCamera();

	RenderWindow->Render();
	RenderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
