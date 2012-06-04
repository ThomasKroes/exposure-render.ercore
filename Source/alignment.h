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

#include "transform.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL Alignment
{
public:
	HOST_DEVICE Alignment() :
		Type(Enums::Spherical),
		Axis(Enums::X),
		AutoFlip(true),
		Position(0.0f),
		Target(0.0f),
		Up(0.0f, 1.0f, 0.0f),
		Elevation(45.0f),
		Azimuth(0.0f),
		Offset(0.0f),
		ManualTM()
	{
	}

	HOST_DEVICE Alignment(const Alignment& Other)
	{
		*this = Other;
	}
	
	HOST_DEVICE Alignment& operator = (const Alignment& Other)
	{
		this->Type			= Other.Type;
		this->Axis			= Other.Axis;
		this->AutoFlip		= Other.AutoFlip;
		this->Position		= Other.Position;
		this->Target		= Other.Target;
		this->Up			= Other.Up;
		this->Elevation		= Other.Elevation;
		this->Azimuth		= Other.Azimuth;
		this->Offset		= Other.Offset;
		this->ManualTM		= Other.ManualTM;

		return *this;
	}

	HOST_DEVICE Transform GetTransform() const
	{
		Transform Result;

		Matrix44 Offset, Translation, Rotation;

		switch (this->Type)
		{
			case Enums::AxisAlign:
			{
				Translation.NN[0][3] = this->Position[0];
				Translation.NN[1][3] = this->Position[1];
				Translation.NN[2][3] = this->Position[2];

				switch (this->Axis)
				{
					case Enums::X:
					{
						Rotation.NN[0][0] = 0.0f;
						Rotation.NN[1][0] = 0.0f;
						Rotation.NN[2][0] = 1.0f;

						Rotation.NN[0][1] = 0.0f;
						Rotation.NN[1][1] = 1.0f;
						Rotation.NN[2][1] = 0.0f;

						Rotation.NN[0][2] = this->AutoFlip ? (this->Position[0] > 0.0f ? -1.0f : 1.0f) : 1.0f;
						Rotation.NN[1][2] = 0.0f;
						Rotation.NN[2][2] = 0.0f;
						
						return Transform(Offset * Translation * Rotation);
					}

					case Enums::Y:
					{
						Rotation.NN[0][0] = 1.0f;
						Rotation.NN[1][0] = 0.0f;
						Rotation.NN[2][0] = 0.0f;

						Rotation.NN[0][1] = 0.0f;
						Rotation.NN[1][1] = 0.0f;
						Rotation.NN[2][1] = 1.0f;

						Rotation.NN[0][2] = 0.0f;
						Rotation.NN[1][2] = this->AutoFlip ? (this->Position[1] > 0.0f ? -1.0f : 1.0f) : 1.0f;
						Rotation.NN[2][2] = 0.0f;
						
						return Transform(Offset * Translation * Rotation);
					}

					case Enums::Z:
					{
						Rotation.NN[0][0] = 1.0f;
						Rotation.NN[1][0] = 0.0f;
						Rotation.NN[2][0] = 0.0f;

						Rotation.NN[0][1] = 0.0f;
						Rotation.NN[1][1] = 1.0f;
						Rotation.NN[2][1] = 0.0f;

						Rotation.NN[0][2] = 0.0f;
						Rotation.NN[1][2] = 0.0f;
						Rotation.NN[2][2] = this->AutoFlip ? (this->Position[2] > 0.0f ? -1.0f : 1.0f) : 1.0f;
						
						return Transform(Offset * Translation * Rotation);
					}
				}
			}

			case Enums::LookAt:
			{
				return Transform(Offset * Matrix44::CreateLookAt(this->Position, this->Target, this->Up));
			}

			case Enums::Spherical:
			{
				const float ElevationRad	= this->Elevation / RAD_F;
				const float AzimuthRad		= this->Azimuth / RAD_F;

				const Vec3f Direction(cosf(ElevationRad) * sinf(AzimuthRad), sinf(ElevationRad), cosf(ElevationRad) * cosf(AzimuthRad));

				const Vec3f P = this->Offset * Direction;
				const Vec3f U = Normalize(Cross(-Direction, Vec3f(0.0f, 1.0f, 0.0f)));
				const Vec3f V = -Normalize(Cross(-Direction, U));
				const Vec3f W = Normalize(-Direction);

				Translation.NN[0][3] = P[0];
				Translation.NN[1][3] = P[1];
				Translation.NN[2][3] = P[2];

				Rotation.NN[0][0] = U[0];
				Rotation.NN[1][0] = U[1];
				Rotation.NN[2][0] = U[2];

				Rotation.NN[0][1] = V[0];
				Rotation.NN[1][1] = V[1];
				Rotation.NN[2][1] = V[2];

				Rotation.NN[0][2] = W[0];
				Rotation.NN[1][2] = W[1];
				Rotation.NN[2][2] = W[2];

				return Transform(Offset * Translation * Rotation);
			}

			case Enums::Manual:
			{
				return Transform(Offset * this->ManualTM);
			}
		}

		return Offset;
	}

	Enums::AlignmentType	Type;
	Enums::Axis				Axis;
	bool					AutoFlip;
	Vec3f					Position;
	Vec3f					Target;
	Vec3f					Up;
	float					Elevation;
	float					Azimuth;
	float					Offset;
	Matrix44				ManualTM;
};

}
