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

#include "defines.h"
#include "enums.h"
#include "vector.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL Matrix44
{
public:
	HOST Matrix44()
	{
		this->Identity();
	}

	HOST ~Matrix44()
	{
	}

	HOST Matrix44(const Matrix44& Other)
	{
		*this = Other;
	}

	HOST Matrix44& operator = (const Matrix44& Other)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				this->NN[i][j] = Other.NN[i][j];

		return *this;
	}

	HOST void Identity()
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				this->NN[i][j] = i == j ? 1.0f : 0.0f;
	}

	HOST Matrix44 operator * (const Matrix44& Other) const	
	{
		Matrix44 Result;

		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				Result.NN[i][j] =	this->NN[i][0] * Other.NN[0][j] +
				          			this->NN[i][1] * Other.NN[1][j] +
				          			this->NN[i][2] * Other.NN[2][j] +
				          			this->NN[i][3] * Other.NN[3][j];

		return Result;
	}

	/*
	HOST Matrix44 operator *= (const Matrix44& Other) const	
	{
		Matrix44 Result;

		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				this->NN[i][j] =	m1->m[i][0] * Other.NN[0][j] +
				          			m1->m[i][1] * Other.NN[1][j] +
				          			m1->m[i][2] * Other.NN[2][j] +
				          			m1->m[i][3] * Other.NN[3][j];

		return Result;
	}
	*/

	HOST bool Invert(Matrix44& Result)
	{
		using std::abs; // use overloaded abs
		float t;
		Matrix44 tmp = *this;

		Result.Identity();

		for (int i = 0; i < 4; ++i)
		{
			int swap = i;

			for (size_t j = i + 1; j < 4; ++j)
			{
				if (abs(tmp.NN[j][i]) > abs(tmp.NN[i][i]))
					swap = j;
			}

			if (swap != i)
			{
				for (size_t k = 0; k < 4; ++k)
				{
					t = tmp.NN[i][k];
					tmp.NN[i][k] = tmp.NN[swap][k];
					tmp.NN[swap][k] = t;

					t = Result.NN[i][k];
					Result.NN[i][k] = Result.NN[swap][k];
					Result.NN[swap][k] = t;
				}
			}

			if (tmp.NN[i][i] == 0)
			{
				return false;
			}

			t = tmp.NN[i][i];

			for (size_t k = 0; k < 4; k++)
			{
				tmp.NN[i][k] /= t;
				Result.NN[i][k] /= t;
			}

			for (size_t j = 0; j < 4; j++)
			{
				if (j != i)
				{
					t = tmp.NN[j][i];

					for (size_t k = 0; k < 4; k++) {
						tmp.NN[j][k] -= tmp.NN[i][k] * t;
						Result.NN[j][k] -= Result.NN[i][k] * t;
					}
				}
			}
		}

		return true;
	}

	HOST void AxisAlign(const Enums::Axis& Axis, const Vec3f& Position, const bool& AutoFlip)
	{
		Matrix44 Offset, Translation, Rotation;

		Translation.NN[0][3] = Position[0];
		Translation.NN[1][3] = Position[1];
		Translation.NN[2][3] = Position[2];

		switch (Axis)
		{
			case Enums::X:
			{
				Rotation.NN[0][0] = 0.0f;
				Rotation.NN[1][0] = 0.0f;
				Rotation.NN[2][0] = 1.0f;

				Rotation.NN[0][1] = 0.0f;
				Rotation.NN[1][1] = 1.0f;
				Rotation.NN[2][1] = 0.0f;

				Rotation.NN[0][2] = AutoFlip ? (Position[0] > 0.0f ? -1.0f : 1.0f) : 1.0f;
				Rotation.NN[1][2] = 0.0f;
				Rotation.NN[2][2] = 0.0f;
				
				*this = Offset * Translation * Rotation;
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
				Rotation.NN[1][2] = AutoFlip ? (Position[1] > 0.0f ? -1.0f : 1.0f) : 1.0f;
				Rotation.NN[2][2] = 0.0f;
				
				*this = Offset * Translation * Rotation;
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
				Rotation.NN[2][2] = AutoFlip ? (Position[2] > 0.0f ? -1.0f : 1.0f) : 1.0f;
				
				*this = Offset * Translation * Rotation;
			}
		}
	}

	float NN[4][4];
};

}
