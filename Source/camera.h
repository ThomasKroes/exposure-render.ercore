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

#include "montecarlo.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL Camera
{
public:
	HOST Camera() :
		FilmSize(0, 0),
		Pos(1.0f),
		Target(0.0f),
		Up(0.0f, 1.0f, 0.0f),
		FocusMode(Enums::AutoFocus),
		FocusUV(0.5f),
		FocalDistance(0.0f),
		ApertureShape(Enums::Polygon),
		ApertureSize(0.0f),
		NoApertureBlades(6),
		ApertureAngle(0.0f),
		ClipNear(0.0f),
		ClipFar(10000.0f),
		Exposure(1.0f),
		Gamma(2.2f),
		FOV(35.0f),
		N(0.0f),
		U(0.0f),
		V(0.0f),
		InvExposure(1.0f),
		InvGamma(1.0f / 2.2f)
	{
	}

	HOST Camera(const Camera& Other) :
		FilmSize(0, 0),
		Pos(1.0f),
		Target(0.0f),
		Up(0.0f, 1.0f, 0.0f),
		FocusMode(Enums::AutoFocus),
		FocusUV(0.5f),
		FocalDistance(0.0f),
		ApertureShape(Enums::Polygon),
		ApertureSize(0.0f),
		NoApertureBlades(6),
		ApertureAngle(0.0f),
		ClipNear(0.0f),
		ClipFar(10000.0f),
		Exposure(1.0f),
		Gamma(2.2f),
		FOV(35.0f),
		N(0.0f),
		U(0.0f),
		V(0.0f),
		InvExposure(1.0f),
		InvGamma(1.0f / 2.2f)
	{
		*this = Other;
	}

	HOST Camera& Camera::operator = (const Camera& Other)
	{
		this->FilmSize			= Other.FilmSize;
		this->Pos				= Other.Pos;
		this->Target			= Other.Target;
		this->Up				= Other.Up;
		this->FocusMode			= Other.FocusMode;
		this->FocusUV			= Other.FocusUV;
		this->FocalDistance		= Other.FocalDistance;
		this->ApertureShape		= Other.ApertureShape;
		this->ApertureSize		= Other.ApertureSize;
		this->NoApertureBlades	= Other.NoApertureBlades;
		this->ApertureAngle		= Other.ApertureAngle;
		this->ClipNear			= Other.ClipNear;
		this->ClipFar			= Other.ClipFar;
		this->Exposure			= Other.Exposure;
		this->Gamma				= Other.Gamma;
		this->FOV				= Other.FOV;
		
		this->Update();

		return *this;
	}

	HOST void Update()
	{
		this->InvExposure	= this->Exposure == 0.0f ? 0.0f : 1.0f / this->Exposure;
		this->InvGamma		= this->Gamma == 0.0f ? 0.0f : 1.0f / this->Gamma;
		
		this->N = Normalize(this->Target - this->Pos);
		this->U = Normalize(Cross(this->N, this->Up));
		this->V = Normalize(Cross(this->N, this->U));

		if (this->FocalDistance == -1.0f)
			this->FocalDistance = Length(this->Target, this->Pos);

		float Scale = 0.0f;

		Scale = tanf((0.5f * this->FOV / RAD_F));

		const float AspectRatio = (float)this->FilmSize[1] / (float)this->FilmSize[0];

		if (AspectRatio > 1.0f)
		{
			this->Screen[0][0] = -Scale;
			this->Screen[0][1] = Scale;
			this->Screen[1][0] = -Scale * AspectRatio;
			this->Screen[1][1] = Scale * AspectRatio;
		}
		else
		{
			this->Screen[0][0] = -Scale / AspectRatio;
			this->Screen[0][1] = Scale / AspectRatio;
			this->Screen[1][0] = -Scale;
			this->Screen[1][1] = Scale;
		}

		this->InvScreen[0] = (this->Screen[0][1] - this->Screen[0][0]) / (float)this->FilmSize[0];
		this->InvScreen[1] = (this->Screen[1][1] - this->Screen[1][0]) / (float)this->FilmSize[1];
	}

	DEVICE void Sample(Ray& R, const Vec2i& UV, RNG& RNG)
	{
		Vec2f ScreenPoint;

		R.ImageUV[0] = UV[0] + RNG.Get1();
		R.ImageUV[1] = UV[1] + RNG.Get1();

		ScreenPoint[0] = this->Screen[0][0] + (this->InvScreen[0] * R.ImageUV[0]);
		ScreenPoint[1] = this->Screen[1][0] + (this->InvScreen[1] * R.ImageUV[1]);

		R.O		= this->Pos;
		R.D		= Normalize(this->N + (ScreenPoint[0] * this->U) - (ScreenPoint[1] * this->V));
		R.MinT	= this->ClipNear;
		R.MaxT	= this->ClipFar;
		
		if (this->ApertureSize != 0.0f)
		{
			Vec2f LensUV;

			switch (this->ApertureShape)
			{
				case Enums::Circular:
				{
					LensUV = this->ApertureSize * ConcentricSampleDisk(RNG.Get2());
					break;
				}

				case Enums::Polygon:
				{
					const float LensY		= RNG.Get1() * this->NoApertureBlades;
					const int Side			= (int)LensY;
					const float Offset		= (float) LensY - Side;
					const float Distance	= (float) sqrtf(RNG.Get1());
					const float A0 			= (float) (Side * PI_F * 2.0f / this->NoApertureBlades + this->ApertureAngle);
					const float A1 			= (float) ((Side + 1.0f) * PI_F * 2.0f / this->NoApertureBlades + this->ApertureAngle);
					const float EyeX 		= (float) ((cos(A0) * (1.0f - Offset) + cos(A1) * Offset) * Distance);
					const float EyeY 		= (float) ((sin(A0) * (1.0f - Offset) + sin(A1) * Offset) * Distance);
					
					LensUV[0] = EyeX * this->ApertureSize;
					LensUV[1] = EyeY * this->ApertureSize;
					break;
				}
			}
			
			const Vec3f LI = this->U * LensUV[0] + this->V * LensUV[1];

			R.O += LI;
			R.D = Normalize(R.D * this->FocalDistance - LI);
		}
	}

	HOST_DEVICE bool ProjectPointToFilmPlane(const Vec3f& P, Vec2f& FilmUV) const
	{
		const Vec3f D = P - this->Pos;
		const Vec3f Dn = Normalize(P - this->Pos);

		const float L = Dot(Dn, this->N);

		const Vec3f FilmP = this->Pos + Dn * (1.0f * L);

		const Vec2f CamUV(Dot(FilmP, this->U), -Dot(FilmP, this->V));

		if (CamUV[0] < this->Screen[0][0] || CamUV[0] > this->Screen[0][1])
			return false;

		if (CamUV[1] < this->Screen[1][0] || CamUV[1] > this->Screen[1][1])
			return false;

		const Vec2f ScreenSize(this->Screen[0][1] - this->Screen[0][0], this->Screen[1][1] - this->Screen[1][0]);

		Vec2f Offset = CamUV - Vec2f(this->Screen[0][0], this->Screen[1][0]);
		
		Offset /= ScreenSize;
		
		FilmUV[0] = (float)this->FilmSize[0] * Offset[0];
		FilmUV[1] = (float)this->FilmSize[1] * Offset[1];

		return true;
	}

	Vec2i					FilmSize;
	Vec3f					Pos;
	Vec3f					Target;
	Vec3f					Up;
	Enums::FocusMode		FocusMode;
	Vec2f					FocusUV;
	float					FocalDistance;
	Enums::ApertureShape	ApertureShape;	
	float					ApertureSize;
	int						NoApertureBlades;
	float					ApertureAngle;
	float					ClipNear;
	float					ClipFar;
	float					Exposure;
	float					Gamma;
	float					FOV;
	Vec3f					N;
	Vec3f					U;
	Vec3f					V;
	float					Screen[2][2];
	float					InvScreen[2];
	float					InvExposure;
	float					InvGamma;
};

}
