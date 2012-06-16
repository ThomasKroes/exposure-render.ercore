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

#include "macros.cuh"
#include "utilities.h"
#include "transport.h"
#include "camera.h"

#include "mitchell.h"
#include "gaussian.h"
#include "sinc.h"
#include "triangle.h"

namespace ExposureRender
{

DEVICE ScatterEvent NearestIntersection(const Ray& R, RNG& RNG)
{
	ScatterEvent SE[3] = { ScatterEvent(Enums::Volume), ScatterEvent(Enums::Light), ScatterEvent(Enums::Object) };
	
	IntersectVolume(R, RNG, SE[0]);
	IntersectLights(R, SE[1], true);
	IntersectObjects(R, SE[2]);
	
	float T = FLT_MAX;

	ScatterEvent NearestRS(Enums::Volume);

	for (int i = 0; i < 3; i++)
	{
		if (SE[i].Valid && SE[i].T < T)
		{
			NearestRS = SE[i];
			T = SE[i].T;
		}
	}

	return NearestRS;
}

DEVICE ColorXYZAf SingleScattering(Tracer* pTracer, const Vec2i& PixelCoord)
{
	

	/*
	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(PixelCoord[0], PixelCoord[1]), &gpTracer->FrameBuffer.RandomSeeds2(PixelCoord[0], PixelCoord[1]));

	ColorXYZf L = ColorXYZf::Black();

	MetroSample Sample(RNG);

	Ray R;

	SampleCamera(gpTracer->Camera, R, PixelCoord[0], PixelCoord[1], Sample.CameraSample);

	ColorRGBf RGB;

	ScatterEvent SE;

	SE = NearestIntersection(R, RNG);

	if (SE.T > 0.0f)
	{
		switch (SE.Type)
		{
			case Enums::Volume:
			case Enums::Object:
			case Enums::ClippingObject:
			{
				L += UniformSampleOneLight(SE, RNG, Sample.LightingSample);
				break;
			}

			case Enums::Light:
			{
				L += SE.Le;
				break;
			}
		}
	}

	return ColorXYZAf(L[0], L[1], L[2], SE.T > 0.0f ? 1.0f : 0.0f);
	*/
}

}
