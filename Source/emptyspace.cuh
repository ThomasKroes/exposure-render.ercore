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

#include "geometry.h"
#include "utilities.h"

namespace ExposureRender
{

KERNEL void KrnlEmptySpace()
{
	KERNEL_3D(gpTracer->EmptySpace.GetResolution()[0], gpTracer->EmptySpace.GetResolution()[1], gpTracer->EmptySpace.GetResolution()[2])

	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[0]];

	int Range[3][2];

	const Vec3i Offset(IDx * 8, IDy * 8, IDz * 8);

	Range[0][0] = Offset[0];
	Range[0][1] = min(Offset[0] + 8, Volume.Voxels.GetResolution()[0] - 1);
	Range[1][0] = Offset[1];
	Range[1][1] = min(Offset[1] + 8, Volume.Voxels.GetResolution()[1] - 1);
	Range[2][0] = Offset[2];
	Range[2][1] = min(Offset[2] + 8, Volume.Voxels.GetResolution()[2] - 1);

	gpTracer->EmptySpace(IDx, IDy, IDz) = 0;

	for (int x = Range[0][0]; x <= Range[0][1]; x++)
	{
		for (int y = Range[1][0]; y <= Range[1][1]; y++)
		{
			for (int z = Range[2][0]; z <= Range[2][1]; z++)
			{
				if (gpTracer->GetOpacity(Volume(x, y, z)) > 0.0f)
					gpTracer->EmptySpace(IDx, IDy, IDz) = 1;
			}
		}
	}
}

void EmptySpace(Tracer& Tracer, Statistics& Statistics)
{
	int	Interval = 8;

	const Vec3i VolRes = gVolumes[gVolumesHashMap[Tracer.VolumeIDs[0]]].Voxels.GetResolution();

	const Vec3i Resolution((int)ceilf(VolRes[0] / 8.0f), (int)ceilf(VolRes[1] / 8.0f), (int)ceilf(VolRes[2] / 8.0f));

	Tracer.EmptySpace.Resize(Resolution);

	LAUNCH_DIMENSIONS(Resolution[0], Resolution[1], Resolution[2], 8, 8, 8)


	LAUNCH_CUDA_KERNEL_TIMED((KrnlEmptySpace<<<GridDim, BlockDim>>>()), "Composite");
}

}
