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

#include "defines.h"
#include "enums.h"
#include "buffer3d.h"
#include <vector>

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL Octree
{
protected:
	class OctreeNode
	{
	public:
		HOST OctreeNode(const Vec3f& Min, const Vec3f& Max) :
			Min(Min),
			Max(Max),
			Children(NULL)
		{

		}

		HOST OctreeNode(const OctreeNode& Other)
		{
			*this = Other;
		}

		HOST OctreeNode& operator = (const OctreeNode& Other)
		{
			this->Min		= Other.Min;
			this->Max		= Other.Max;
			this->Children	= Other.Children;

			return *this;
		}

		HOST void SubDivide()
		{
			// initialize children
			std::vector<OctreeNode> Children(8, OctreeNode(this->Min, this->Max));

			// get middle point
			Vec3f Mid = (this->Min + this->Max) / 2;

			// create children
			Children[0].Min = Vec3f(this->Min[0],	this->Min[1],	this->Min[2]);
			Children[0].Max = Vec3f(Mid[0],			Mid[1],			Mid[2]);

			Children[1].Min = Vec3f(this->Min[0],	Mid[1],			this->Min[2]);
			Children[1].Max = Vec3f(Mid[0],			this->Max[1],	Mid[2]);

			Children[2].Min = Vec3f(Mid[0],			this->Min[1],	this->Min[2]);
			Children[2].Max = Vec3f(this->Max[0],	Mid[1],			Mid[2]);

			Children[3].Min = Vec3f(Mid[0],			Mid[1],			this->Min[2]);
			Children[3].Max = Vec3f(this->Max[0],	this->Max[2],	Mid[2]);

			Children[4].Min = Vec3f(this->Min[0],	this->Min[1],	Mid[2]);
			Children[4].Max = Vec3f(Mid[0],			Mid[1],			this->Max[2]);

			Children[5].Min = Vec3f(this->Min[0],	Mid[1],			Mid[2]);
			Children[5].Max = Vec3f(Mid[0],			this->Max[1],	this->Max[2]);

			Children[6].Min = Vec3f(Mid[0],			this->Min[1],	Mid[2]);
			Children[6].Max = Vec3f(this->Max[0],	Mid[1],			this->Max[2]);

			Children[7].Min = Vec3f(Mid[0],			Mid[1],			Mid[2]);
			Children[7].Max = Vec3f(this->Max[0],	this->Max[2],	this->Max[2]);

			// pass on voxels to children

			// clear this node's voxels

			// store children
			this->Children	= &Children[0];
		}

		Vec3f Min;
		Vec3f Max;
		OctreeNode* Children;
	};

public:
	HOST Octree(const int& MaxDepth = 12) :
		MaxDepth(MaxDepth),
		RootNode(NULL)
	{
	}

	HOST Octree(const Octree& Other)
	{
		*this = Other;
	}

	HOST Octree& operator = (const Octree& Other)
	{
		this->MaxDepth	= Other.MaxDepth;
		this->RootNode	= Other.RootNode;

		return *this;
	}

	HOST_DEVICE unsigned short GetIntensity(const Vec3f& P) const
	{
		return 150;
	}

	HOST void Build(const Buffer3D<unsigned short>& Voxels)
	{
		Vec3i res = Voxels.GetResolution(); // this is (0,0,0) ??
		//unsigned short val = Voxels[0];  runtime exception
	}

	int				MaxDepth;
	OctreeNode*		RootNode;
};

}