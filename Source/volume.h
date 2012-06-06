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

#include "ervolume.h"
#include "boundingbox.h"
#include "octree.h"
#include "cudatexture3d.h"
#include "utilities.h"
#include "transform.h"

#include "gradientmagnitude.cuh"

namespace ExposureRender
{

class Volume
{
public:
	HOST Volume() :
		Transform(),
		BoundingBox(),
		Spacing(1.0f),
		InvSpacing(1.0f),
		Size(1.0f),
		InvSize(1.0f),
		MinStep(1.0f),
		Voxels(),
		AcceleratorType(Enums::Octree),
		Octree(),
		MaxGradientMagnitude(0.0f)
	{
	}

	HOST Volume(const ErVolume& Other) :
		Transform(),
		BoundingBox(),
		Spacing(1.0f),
		InvSpacing(1.0f),
		Size(1.0f),
		InvSize(1.0f),
		MinStep(1.0f),
		Voxels(),
		AcceleratorType(Enums::Octree),
		Octree(),
		MaxGradientMagnitude(0.0f)
	{
		*this = Other;
	}

	HOST Volume& Volume::operator = (const ErVolume& Other)
	{
		this->Transform			= Other.Alignment.GetTransform();
		this->Voxels			= Other.Voxels;
		this->AcceleratorType	= Other.AcceleratorType;

		const int NoElements = this->Voxels.GetResolution()[0] * this->Voxels.GetResolution()[1] * this->Voxels.GetResolution()[2];

		if (NoElements > 0)
		{
			float Scale = 0.0f;

			if (Other.NormalizeSize)
			{
				const Vec3f PhysicalSize = Vec3f((float)this->Voxels.GetResolution()[0], (float)this->Voxels.GetResolution()[1], (float)this->Voxels.GetResolution()[2]) * Other.Spacing;
				Scale = 1.0f / max(PhysicalSize[0], max(PhysicalSize[1], PhysicalSize[2]));
			}

			this->Spacing		= Scale * Other.Spacing;
			this->InvSpacing	= 1.0f / this->Spacing;
			this->Size			= Vec3f((float)this->Voxels.GetResolution()[0] * this->Spacing[0], (float)this->Voxels.GetResolution()[1] *this->Spacing[1], (float)this->Voxels.GetResolution()[2] * this->Spacing[2]);
			this->InvSize		= 1.0f / this->Size;

			this->BoundingBox.SetMinP(-0.5 * Size);
			this->BoundingBox.SetMaxP(0.5f * Size);

			this->MinStep = min(this->Spacing[0], min(this->Spacing[1], this->Spacing[2]));

			this->MaxGradientMagnitude = Other.GetMaximumGradientMagnitude();
		}

		return *this;
	}

	DEVICE unsigned short operator()(const Vec3f& XYZ, const int& TextureID = 0)
	{
		const Vec3f NormalizedXYZ = (XYZ - this->BoundingBox.MinP) * this->InvSize;

		switch (TextureID)
		{
			case 0:
				return (float)USHRT_MAX * tex3D(TexVolume0, NormalizedXYZ[0], NormalizedXYZ[1], NormalizedXYZ[2]);

			case 1:
				return (float)USHRT_MAX * tex3D(TexVolume1, NormalizedXYZ[0], NormalizedXYZ[1], NormalizedXYZ[2]);

			case 2:
				return (float)USHRT_MAX * tex3D(TexVolume2, NormalizedXYZ[0], NormalizedXYZ[1], NormalizedXYZ[2]);

			case 3:
				return (float)USHRT_MAX * tex3D(TexVolume3, NormalizedXYZ[0], NormalizedXYZ[1], NormalizedXYZ[2]);
		}

		return 0; 
	}

	DEVICE float GetIntensity(const Vec3f& P)
	{
		switch (this->AcceleratorType)
		{
			case Enums::NoAcceleration:
			{
				return (*this)(P);
			}

			case Enums::Octree:
			{
				return this->Octree.GetIntensity(P);
			}

			default:
			{
				return (*this)(P);
			}
		}
	}

	DEVICE Vec3f GradientCD(const Vec3f& P)
	{
		const float Intensity[3][2] = 
		{
			{ GetIntensity(P + Vec3f(this->Spacing[0], 0.0f, 0.0f)), GetIntensity(P - Vec3f(this->Spacing[0], 0.0f, 0.0f)) },
			{ GetIntensity(P + Vec3f(0.0f, this->Spacing[1], 0.0f)), GetIntensity(P - Vec3f(0.0f, this->Spacing[1], 0.0f)) },
			{ GetIntensity(P + Vec3f(0.0f, 0.0f, this->Spacing[2])), GetIntensity(P - Vec3f(0.0f, 0.0f, this->Spacing[2])) }
		};

		return Vec3f(Intensity[0][1] - Intensity[0][0], Intensity[1][1] - Intensity[1][0], Intensity[2][1] - Intensity[2][0]);
	}

	DEVICE Vec3f GradientFD(const Vec3f& P)
	{
		const float Intensity[4] = 
		{
			GetIntensity(P),
			GetIntensity(P + Vec3f(this->Spacing[0], 0.0f, 0.0f)),
			GetIntensity(P + Vec3f(0.0f, this->Spacing[1], 0.0f)),
			GetIntensity(P + Vec3f(0.0f, 0.0f, this->Spacing[2]))
		};

		return Vec3f(Intensity[0] - Intensity[1], Intensity[0] - Intensity[2], Intensity[0] - Intensity[3]);
	}

	DEVICE Vec3f GradientFiltered(const Vec3f& P)
	{
		Vec3f Offset(Vec3f(this->Spacing[0], 0.0f, 0.0f)[0], Vec3f(0.0f, this->Spacing[1], 0.0f)[1], Vec3f(0.0f, 0.0f, this->Spacing[2])[2]);

		Vec3f G0 = GradientCD(P);
		Vec3f G1 = GradientCD(P + Vec3f(-Offset[0], -Offset[1], -Offset[2]));
		Vec3f G2 = GradientCD(P + Vec3f( Offset[0],  Offset[1],  Offset[2]));
		Vec3f G3 = GradientCD(P + Vec3f(-Offset[0],  Offset[1], -Offset[2]));
		Vec3f G4 = GradientCD(P + Vec3f( Offset[0], -Offset[1],  Offset[2]));
		Vec3f G5 = GradientCD(P + Vec3f(-Offset[0], -Offset[1],  Offset[2]));
		Vec3f G6 = GradientCD(P + Vec3f( Offset[0],  Offset[1], -Offset[2]));
		Vec3f G7 = GradientCD(P + Vec3f(-Offset[0],  Offset[1],  Offset[2]));
		Vec3f G8 = GradientCD(P + Vec3f( Offset[0], -Offset[1], -Offset[2]));
	    
		Vec3f L0 = Lerp(Lerp(G1, G2, 0.5), Lerp(G3, G4, 0.5), 0.5);
		Vec3f L1 = Lerp(Lerp(G5, G6, 0.5), Lerp(G7, G8, 0.5), 0.5);
	    
		return Lerp(G0, Lerp(L0, L1, 0.5), 0.75);
	}

	DEVICE Vec3f Gradient(const Vec3f& P, const Enums::GradientMode& GradientMode)
	{
		switch (GradientMode)
		{
			case Enums::ForwardDifferences:		return GradientFD(P);
			case Enums::CentralDifferences:		return GradientCD(P);
			case Enums::Filtered:				return GradientFiltered(P);
		}

		return GradientFD(P);
	}

	DEVICE Vec3f NormalizedGradient(const Vec3f& P, const Enums::GradientMode& GradientMode)
	{
		return Normalize(Gradient(P, GradientMode));
	}

	DEVICE float GradientMagnitude(const Vec3f& P)
	{
		const Vec3f HalfSpacing = 0.5f / this->Spacing;

		float D = 0.0f, Sum = 0.0f;

		D = (GetIntensity(P + Vec3f(this->Spacing[0], 0.0f, 0.0f)) - GetIntensity(P - Vec3f(this->Spacing[0], 0.0f, 0.0f))) * 0.5f;
		Sum += D * D;

		D = (GetIntensity(P + Vec3f(0.0f, this->Spacing[1], 0.0f)) - GetIntensity(P - Vec3f(0.0f, this->Spacing[1], 0.0f))) * 0.5f;
		Sum += D * D;

		D = (GetIntensity(P + Vec3f(0.0f, 0.0f, this->Spacing[2])) - GetIntensity(P - Vec3f(0.0f, 0.0f, this->Spacing[2]))) * 0.5f;
		Sum += D * D;

		return sqrtf(Sum);
	}
	
	Transform						Transform;
	BoundingBox						BoundingBox;
	Vec3f							Spacing;
	Vec3f							InvSpacing;
	Vec3f							Size;
	Vec3f							InvSize;
	float							MinStep;
	CudaTexture3D<unsigned short>	Voxels;
	Enums::AcceleratorType			AcceleratorType;
	Octree							Octree;
	float							MaxGradientMagnitude;
};

}
