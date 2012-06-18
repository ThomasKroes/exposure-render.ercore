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

#include "buffer2d.h"
#include "randomseedbuffer2d.h"
#include "sample.h"

namespace ExposureRender
{

class FrameBuffer
{
public:
	HOST FrameBuffer(void) :
		Resolution(),
		FrameEstimate("Frame Estimate", Enums::Device),
		TempFrameEstimate("Temp Frame Estimate", Enums::Device),
		RunningEstimateXYZ("Running estimate XYZ", Enums::Device),
		RunningEstimateRGB("Running estimate RGB", Enums::Device),
		TempRunningEstimateRGB("Temp running estimate RGB", Enums::Device),
		DisplayEstimate("Display Estimate", Enums::Device),
		RandomSeeds1("Random Seeds 1", Enums::Device),
		RandomSeeds2("Random Seeds 2", Enums::Device),
		RandomSeedsCopy1("Random Seeds 1 Copy", Enums::Device),
		RandomSeedsCopy2("Random Seeds 2 Copy", Enums::Device),
		HostDisplayEstimate("Display Estimate", Enums::Host),
		IDs("IDs", Enums::Device),
		Samples("Samples", Enums::Device)
	{
	}

	HOST void Resize(const Vec2i& Resolution)
	{
		if (this->Resolution == Resolution)
			return;
		
		this->Resolution = Resolution;

		this->FrameEstimate.Resize(this->Resolution);
		this->TempFrameEstimate.Resize(this->Resolution);
		this->RunningEstimateXYZ.Resize(this->Resolution);
		this->RunningEstimateRGB.Resize(this->Resolution);
		this->TempRunningEstimateRGB.Resize(this->Resolution);
		this->TempRunningEstimateRGB.Resize(this->Resolution);
		this->DisplayEstimate.Resize(this->Resolution);
		this->RandomSeeds1.Resize(this->Resolution);
		this->RandomSeeds2.Resize(this->Resolution);
		
		this->IDs.Resize(this->Resolution);
		this->Samples.Resize(this->Resolution);

		this->HostDisplayEstimate.Resize(this->Resolution);

		this->RandomSeedsCopy1 = this->RandomSeeds1;
		this->RandomSeedsCopy2 = this->RandomSeeds2;
	}

	Vec2i					Resolution;
	Buffer2D<ColorXYZAf>	FrameEstimate;
	Buffer2D<ColorXYZAf>	TempFrameEstimate;
	Buffer2D<ColorXYZAf>	RunningEstimateXYZ;
	Buffer2D<ColorRGBAuc>	RunningEstimateRGB;
	Buffer2D<ColorRGBAuc>	TempRunningEstimateRGB;
	Buffer2D<ColorRGBAuc>	DisplayEstimate;
	RandomSeedBuffer2D		RandomSeeds1;
	RandomSeedBuffer2D		RandomSeeds2;
	RandomSeedBuffer2D		RandomSeedsCopy1;
	RandomSeedBuffer2D		RandomSeedsCopy2;
	Buffer2D<ColorRGBAuc>	HostDisplayEstimate;

	Buffer2D<int>			IDs;
	Buffer2D<Sample>		Samples;
};

}
