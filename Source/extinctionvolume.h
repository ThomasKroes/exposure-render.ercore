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

#include "volume.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL ExtinctionVolume : public Volume
{
public:
	HOST ExtinctionVolume() :
		Volume(),
	{
		DebugLog(__FUNCTION__);
	}

	HOST Volume(const Volume& Other) :
		Volume()
	{
		DebugLog(__FUNCTION__);
		*this = Other;
	}

	HOST Volume(const ErVolume& Other) :
		BoundingBox(),
		GradientDeltaX(),
		GradientDeltaY(),
		GradientDeltaZ(),
		Spacing(1.0f),
		InvSpacing(1.0f),
		Size(1.0f),
		InvSize(1.0f),
		MinStep(1.0f),
		Voxels("Device Voxels", Enums::Device)
	{
		DebugLog(__FUNCTION__);
		*this = Other;
	}

	HOST virtual ~Volume(void)
	{
		DebugLog(__FUNCTION__);
	}
	
	HOST Volume& Volume::operator = (const Volume& Other)
	{
		DebugLog(__FUNCTION__);

		this->BoundingBox		= Other.BoundingBox;
		this->Spacing			= Other.Spacing;
		this->InvSpacing		= Other.InvSpacing;
		this->Size				= Other.Size;
		this->InvSize			= Other.InvSize;
		this->MinStep			= Other.MinStep;
		this->Voxels			= Other.Voxels;

		return *this;
	}

	HOST Volume& Volume::operator = (const ErVolume& Other)
	{
		DebugLog(__FUNCTION__);

		this->Voxels = Other.Voxels;

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


		return *this;
	}

	HOST_DEVICE unsigned short operator()(const Vec3f& XYZ = Vec3f(0.0f)) const
	{
		const Vec3f Offset = XYZ - this->BoundingBox.MinP;
		
		const Vec3f LocalXYZ = Offset * this->InvSize * Vec3f(this->Voxels.GetResolution()[0], this->Voxels.GetResolution()[1], this->Voxels.GetResolution()[2]);

		return this->Voxels(Vec3i(LocalXYZ[0], LocalXYZ[1], LocalXYZ[2]));
	}

	HOST_DEVICE float GetIntensity(const Vec3f& P)
	{
		return (*this)(P);
	}

	HOST_DEVICE Vec3f GradientCD(const Vec3f& P)
	{
		const float Intensity[3][2] = 
		{
			{ GetIntensity(P + Vec3f(this->Spacing[0], 0.0f, 0.0f)), GetIntensity(P - Vec3f(this->Spacing[0], 0.0f, 0.0f)) },
			{ GetIntensity(P + Vec3f(0.0f, this->Spacing[1], 0.0f)), GetIntensity(P - Vec3f(0.0f, this->Spacing[1], 0.0f)) },
			{ GetIntensity(P + Vec3f(0.0f, 0.0f, this->Spacing[2])), GetIntensity(P - Vec3f(0.0f, 0.0f, this->Spacing[2])) }
		};

		return Vec3f(Intensity[0][1] - Intensity[0][0], Intensity[1][1] - Intensity[1][0], Intensity[2][1] - Intensity[2][0]);
	}

	HOST_DEVICE Vec3f GradientFD(const Vec3f& P)
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

	HOST_DEVICE Vec3f GradientFiltered(const Vec3f& P)
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

	HOST_DEVICE Vec3f Gradient(const Vec3f& P, const Enums::GradientMode& GradientMode)
	{
		switch (GradientMode)
		{
			case Enums::ForwardDifferences:		return GradientFD(P);
			case Enums::CentralDifferences:		return GradientCD(P);
			case Enums::Filtered:				return GradientFiltered(P);
		}

		return GradientFD(P);
	}

	HOST_DEVICE Vec3f NormalizedGradient(const Vec3f& P, const Enums::GradientMode& GradientMode)
	{
		return Normalize(Gradient(P, GradientMode));
	}

	HOST_DEVICE float GradientMagnitude(const Vec3f& P)
	{
		Vec3f Pts[3][2];

		Pts[0][0] = P + Vec3f(this->Spacing[0], 0.0f, 0.0f);
		Pts[0][1] = P - Vec3f(this->Spacing[0], 0.0f, 0.0f);
		Pts[1][0] = P + Vec3f(0.0f, this->Spacing[1], 0.0f);
		Pts[1][1] = P - Vec3f(0.0f, this->Spacing[1], 0.0f);
		Pts[2][0] = P + Vec3f(0.0f, 0.0f, this->Spacing[2]);
		Pts[2][1] = P - Vec3f(0.0f, 0.0f, this->Spacing[2]);

		float D = 0.0f, Sum = 0.0f;

		for (int i = 0; i < 3; i++)
		{
			D = GetIntensity(Pts[i][1]) - GetIntensity(Pts[i][0]);
			D *= 0.5f / this->Spacing[i];
			Sum += D * D;
		}

		return sqrtf(Sum);
	}

	BoundingBox					BoundingBox;
	Vec3f						Spacing;
	Vec3f						InvSpacing;
	Vec3f						Size;
	Vec3f						InvSize;
	float						MinStep;
	Buffer3D<unsigned short>	Voxels;
};

}