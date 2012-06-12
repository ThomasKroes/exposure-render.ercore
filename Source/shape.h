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

#include "shapes.h"
#include "alignment.h"
#include "clippingsegments.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL Shape
{
public:
	HOST_DEVICE Shape() :
		Type(Enums::Plane),
		Plane(),
		Disk(),
		Ring(),
		Sphere(),
		Box(),
		Alignment(),
		Transform(),
		Area(0.0f)
	{
	}

	HOST_DEVICE Shape(const Shape& Other)
	{
		*this = Other;
	}
	
	HOST_DEVICE Shape& operator = (const Shape& Other)
	{
		this->Type			= Other.Type;
		this->Plane			= Other.Plane;
		this->Disk			= Other.Disk;
		this->Ring			= Other.Ring;
		this->Sphere		= Other.Sphere;
		this->Box			= Other.Box;
		this->Alignment		= Other.Alignment;
		this->Transform		= Other.Transform;
		this->Area			= Other.Area;

		this->Update();

		return *this;
	}

	HOST_DEVICE void Update()
	{
		switch (this->Type)
		{
			case Enums::Plane:		this->Area = Plane.GetArea();		break;
			case Enums::Disk:		this->Area = Disk.GetArea();		break;
			case Enums::Ring:		this->Area = Ring.GetArea();		break;
			case Enums::Box:		this->Area = Box.GetArea();			break;
			case Enums::Sphere:		this->Area = Sphere.GetArea();		break;
//			case Enums::Cylinder:	this->Area = Cylinder.GetArea();	break;
		}

		this->Transform = this->Alignment.GetTransform();
	}

	HOST_DEVICE bool Intersects(const Ray& R) const
	{
		const Ray LocalShapeR = TransformRay(this->Transform.InvTM, R);

		switch (this->Type)
		{
			case Enums::Plane:		return Plane.Intersects(LocalShapeR);
			case Enums::Disk:		return Disk.Intersects(LocalShapeR);
			case Enums::Ring:		return Ring.Intersects(LocalShapeR);
			case Enums::Box:		return Box.Intersects(LocalShapeR);
			case Enums::Sphere:		return Sphere.Intersects(LocalShapeR);
//			case Enums::Cylinder:	return Plane.Intersects(LocalShapeR);
		}

		return false;
	}

	HOST_DEVICE bool Intersect(const Ray& R, Intersection& NI) const
	{
		const Ray LocalShapeR = TransformRay(this->Transform.InvTM, R);

		bool Intersects = false;

		switch (this->Type)
		{
			case Enums::Plane:		Intersects = Plane.Intersect(LocalShapeR, NI);		break;
			case Enums::Disk:		Intersects = Disk.Intersect(LocalShapeR, NI);		break;
			case Enums::Ring:		Intersects = Ring.Intersect(LocalShapeR, NI);		break;
			case Enums::Box:		Intersects = Box.Intersect(LocalShapeR, NI);		break;
			case Enums::Sphere:		Intersects = Sphere.Intersect(LocalShapeR, NI);		break;
//			case Enums::Cylinder:	Intersects = Plane.Intersect(LocalShapeR, NI);		break;
		}

		if (Intersects)
		{
			NI.P	= TransformPoint(this->Transform.TM, NI.P);
			NI.N	= TransformVector(this->Transform.TM, NI.N);
			NI.T	= (NI.P - R.O).Length();
		}
		
		return Intersects;
	}

	HOST_DEVICE void Sample(SurfaceSample& SS, const Vec3f& UVW) const
	{
		switch (this->Type)
		{
			case Enums::Plane:		Plane.Sample(SS, UVW);		break;
			case Enums::Disk:		Disk.Sample(SS, UVW);		break;
			case Enums::Ring:		Ring.Sample(SS, UVW);		break;
			case Enums::Box:		Box.Sample(SS, UVW);		break;
			case Enums::Sphere:		Sphere.Sample(SS, UVW);		break;
//			case Enums::Cylinder:	Cylinder.Sample(SS, UVW);	break;
		}

		SS.P = TransformPoint(this->Transform.TM, SS.P);
		SS.N = TransformVector(this->Transform.TM, SS.N);
	}

	HOST_DEVICE bool GetOneSided() const
	{
		switch (this->Type)
		{
			case Enums::Plane:		return this->Plane.GetOneSided();
			case Enums::Disk:		return this->Disk.GetOneSided();
			case Enums::Ring:		return this->Ring.GetOneSided();
			case Enums::Box:		return this->Box.GetOneSided();
			case Enums::Sphere:		return this->Sphere.GetOneSided();
//			case Enums::Cylinder:	return this->Cylinder.GetOneSided();
		}

		return false;
	}

	HOST_DEVICE bool Inside(const Vec3f& P) const
	{
		const Vec3f LocalP = TransformPoint(this->Transform.InvTM, P);

		switch (this->Type)
		{
			case Enums::Plane:		return this->Plane.Inside(LocalP);
			case Enums::Disk:		return this->Disk.Inside(LocalP);
			case Enums::Ring:		return this->Ring.Inside(LocalP);
			case Enums::Box:		return this->Box.Inside(LocalP);
			case Enums::Sphere:		return this->Sphere.Inside(LocalP);
//			case Enums::Cylinder:	return this->Cylinder.Inside(LocalP);
		}

		return false;
	}

	
	HOST_DEVICE void ClipRange(const Ray& R, ClippingSegment& CS) const
	{
		Ray LocalR = TransformRay(this->Transform.InvTM, R);
		
		LocalR.MinT = 0.0f;
		LocalR.MaxT = 1000;

		Intersection Int;

		switch (this->Type)
		{
			case Enums::Plane:
			{
				if (this->Plane.Intersect(LocalR, Int))
				{
					Int.P	= TransformPoint(this->Transform.TM, Int.P);
					Int.N	= TransformVector(this->Transform.TM, Int.N);
					Int.T	= (Int.P - R.O).Length();

					if (Dot(R.D, Int.N) < 0.0f)
						CS = ClippingSegment(Vec2f(R.MinT, Int.T), Int.P, Int.N);
					else
						CS = ClippingSegment(Vec2f(Int.T, R.MaxT), Int.P, Int.N);
				}
				else
				{
					if (LocalR.O[2] > 0.0f && LocalR.D[2] > 0.0f)
					{
//						CS.Range[0] = R.MinT;
						CS.Range[1] = FLT_MAX;
					}
				}

				break;
			}
			
			/*
			case Enums::Box:		return this->Box.Inside(LocalP, LocalD, T);
			case Enums::Sphere:		return this->Sphere.Inside(LocalP, LocalD, T);
//			case Enums::Cylinder:	return this->Cylinder.Inside(LocalP, LocalD, T);
			*/
		}
	}

	Enums::ShapeType	Type;
	Plane				Plane;
	Disk				Disk;
	Ring				Ring;
	Sphere				Sphere;
	Box					Box;
	Alignment			Alignment;
	Transform			Transform;
	float				Area;
};

}
