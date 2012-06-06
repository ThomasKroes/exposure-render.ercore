/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or witDEVut modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software witDEVut specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT DEVLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT DEVLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) DEVWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "erbindable.h"
#include "vector.h"
#include "buffer3d.h"
#include "alignment.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL ErVolume : public ErBindable
{
public:
	HOST ErVolume() :
		ErBindable(),
		Alignment(),
		Voxels("Host Voxels", Enums::Host),
		NormalizeSize(false),
		Spacing(1.0f),
		AcceleratorType(Enums::Octree)
	{
	}

	HOST ErVolume(const ErVolume& Other) :
		ErBindable(),
		Alignment(),
		Voxels("Host Voxels", Enums::Host),
		NormalizeSize(false),
		Spacing(1.0f),
		AcceleratorType(Enums::Octree)
	{
		*this = Other;
	}

	HOST ErVolume& ErVolume::operator = (const ErVolume& Other)
	{
		ErBindable::operator=(Other);

		this->Alignment			= Other.Alignment;
		this->Voxels			= Other.Voxels;
		this->NormalizeSize		= Other.NormalizeSize;
		this->Spacing			= Other.Spacing;
		this->AcceleratorType	= Other.AcceleratorType;

		return *this;
	}

	HOST void BindVoxels(const Vec3i& Resolution, const Vec3f& Spacing, unsigned short* Voxels, const bool& NormalizeSize = false)
	{
		this->Voxels.Set(Enums::Host, Resolution, Voxels);

		this->NormalizeSize	= NormalizeSize;
		this->Spacing		= Spacing;
	}

	HOST float GetMaximumGradientMagnitude() const
	{
		float MaximumGradientMagnitude = 0.0f;

		const Vec3i Resolution = this->Voxels.GetResolution();
		
		Vec3f Pts[3][2];
		Vec3f P;
		
		
		
		for (int x = 0; x < Resolution[0] - 1; x += 3)
		{
			for (int y = 0; y < Resolution[1] - 1; y += 3)
			{
				for (int z = 0; z < Resolution[2] - 1; z += 3)
				{
					float Sum = 0.0f, D = 0.0f;

					D = (float)(this->Voxels(x + 1, y, z) - this->Voxels(x, y, z));
					D *= 0.5f / this->Spacing[0];
					Sum += D * D;

					D = (float)(this->Voxels(x, y + 1, z) - this->Voxels(x, y, z));
					D *= 0.5f / this->Spacing[1];
					Sum += D * D;

					D = (float)(this->Voxels(x, y, z + 1) - this->Voxels(x, y, z));
					D *= 0.5f / this->Spacing[2];
					Sum += D * D;

					const float GradientMagnitude = sqrtf(Sum);

					if (GradientMagnitude > MaximumGradientMagnitude)
						MaximumGradientMagnitude = GradientMagnitude;
					/*
					Pts[0][0] = P + Vec3f(this->Spacing[0], 0.0f, 0.0f);
					Pts[0][1] = P - Vec3f(this->Spacing[0], 0.0f, 0.0f);
					Pts[1][0] = P + Vec3f(0.0f, this->Spacing[1], 0.0f);
					Pts[1][1] = P - Vec3f(0.0f, this->Spacing[1], 0.0f);
					Pts[2][0] = P + Vec3f(0.0f, 0.0f, this->Spacing[2]);
					Pts[2][1] = P - Vec3f(0.0f, 0.0f, this->Spacing[2]);

					float D = 0.0f, Sum = 0.0f;

					for (int i = 0; i < 3; i++)
					{
						D = (float)(this->Voxels(Pts[i][1], false) - this->Voxels(Pts[i][0], false));
						D *= 0.5f / this->Spacing[i];
						Sum += D * D;
					}

					const float GradientMagnitude = sqrtf(Sum);

					if (GradientMagnitude > MaximumGradientMagnitude)
						MaximumGradientMagnitude = GradientMagnitude;*/
				}
			}
		}
		
		printf("%0.4f", MaximumGradientMagnitude);

		return MaximumGradientMagnitude;
	}

	Alignment					Alignment;
	Buffer3D<unsigned short>	Voxels;
	bool						NormalizeSize;
	Vec3f						Spacing;
	Enums::AcceleratorType		AcceleratorType;
};

}
