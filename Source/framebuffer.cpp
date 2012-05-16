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

#include "framebuffer.h"

namespace ExposureRender
{

FrameBuffer::FrameBuffer(void) :
	Resolution(),
	FrameEstimate(Enums::Device, "Frame Estimate XYZA"),
	FrameEstimateTemp(Enums::Device, "Temp Frame Estimate XYZA"),
	AccumulationXyza(Enums::Device, "Accumulation XYZA"),
	Weight(Enums::Device, "Weight"),
	RunningEstimateXyza(Enums::Device, "Running Estimate Xyza"),
	DisplayEstimate(Enums::Device, "Display Estimate RGBA"),
	DisplayEstimateTemp(Enums::Device, "Temp Display Estimate RGBA"),
	DisplayEstimateFiltered(Enums::Device, "Filtered Display Estimate RGBA"),
	RandomSeeds1(Enums::Device, "Random Seeds 1"),
	RandomSeeds2(Enums::Device, "Random Seeds 2"),
	RandomSeedsCopy1(Enums::Device, "Random Seeds 1 Copy"),
	RandomSeedsCopy2(Enums::Device, "Random Seeds 2 Copy"),
	HostDisplayEstimate(Enums::Host, "Display Estimate RGBA")
{
}

void FrameBuffer::Resize(const Vec2i& Resolution)
{
	if (this->Resolution == Resolution)
		return;
	
	this->Resolution = Resolution;

	this->FrameEstimate.Resize(this->Resolution);
	this->FrameEstimateTemp.Resize(this->Resolution);
	this->AccumulationXyza.Resize(this->Resolution);
	this->Weight.Resize(this->Resolution);
	this->RunningEstimateXyza.Resize(this->Resolution);
	this->DisplayEstimate.Resize(this->Resolution);
	this->DisplayEstimateTemp.Resize(this->Resolution);
	this->DisplayEstimateFiltered.Resize(this->Resolution);
	this->RandomSeeds1.Resize(this->Resolution);
	this->RandomSeeds2.Resize(this->Resolution);
	this->HostDisplayEstimate.Resize(this->Resolution);

	this->RandomSeedsCopy1 = this->RandomSeeds1;
	this->RandomSeedsCopy2 = this->RandomSeeds2;
}

}
