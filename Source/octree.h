/*
*	@file
*	@author  Thomas Kroes <t.kroes at tudelft.nl>
*	@version 1.0
*	
*	@section LICENSE
*	
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "defines.h"
#include "enums.h"
#include "buffer3d.h"
#include "boundingbox.h"
#include <vector>

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL Octree
{
protected:
	class OctreeNode
	{
	public:
		HOST OctreeNode(const BoundingBox BoundingBox = ExposureRender::BoundingBox()) :
			BoundingBox(BoundingBox),
			FirstChild(-1)
		{
		}

		HOST OctreeNode(const OctreeNode& Other)
		{
			*this = Other;
		}

		HOST OctreeNode& operator = (const OctreeNode& Other)
		{
			this->BoundingBox	= Other.BoundingBox;
			this->FirstChild	= Other.FirstChild;

			return *this;
		}
		
		HOST static bool ShouldStop(const int& Depth, const int& MaxDepth)
		{
			if (Depth >= MaxDepth)
				return true;

			return false;
		}

		HOST void SubDivide(const Buffer3D<unsigned short>& Voxels, std::vector<OctreeNode>& Nodes, const int& Index, const int& Depth, const int& MaxDepth)
		{
			// test if we should stop subdividing
			if (OctreeNode::ShouldStop(Depth, MaxDepth))
				return;

			// get the index of the first child
			this->FirstChild = Nodes.size();

			// new depth
			int NewDepth = Depth + 1;

			// calculate halfsize of bounding box
			Vec3f HalfSize = this->BoundingBox.Size / 2;

			// initialize children and append to vector
			OctreeNode Children[8];
			Nodes.insert(Nodes.end(), &Children[0], &Children[8]);

			// subdivide children
			for (int i = 0; i < 8; i++)
			{
				// IMPORTANT: Nodes has been resized so "this" became unusable
				OctreeNode* ThisNode = &Nodes[Index];

				// get child index
				int ChildIndex = ThisNode->FirstChild + i;
				OctreeNode* ChildNode = &Nodes[ChildIndex];

				// calculate child boundingbox
				int a = i & 1;
				int b = (i >> 1) & 1;
				int c = (i >> 2) & 1;
				ChildNode->BoundingBox.SetMinP(Vec3f(
					ThisNode->BoundingBox.MinP[0] + a * HalfSize[0],
					ThisNode->BoundingBox.MinP[1] + b * HalfSize[1],
					ThisNode->BoundingBox.MinP[2] + c * HalfSize[2]
				));
				ChildNode->BoundingBox.SetMaxP(ChildNode->BoundingBox.MinP + HalfSize);
				ChildNode->BoundingBox.Update();

				// recurse
				ChildNode->SubDivide(Voxels, Nodes, ChildIndex, NewDepth, MaxDepth);
			}
		}

		BoundingBox		BoundingBox;
		int				FirstChild;
	};

public:
	HOST Octree(const int& MaxDepth = 4) :
		MaxDepth(MaxDepth),
		RootNode(NULL),
		Sigma(5.0f)
	{
	}

	HOST Octree(const Octree& Other)
	{
		*this = Other;
	}
	
	HOST ~Octree()
	{
		this->Free();
	}

	HOST Octree& operator = (const Octree& Other)
	{
		this->MaxDepth	= Other.MaxDepth;
		this->RootNode	= Other.RootNode;

		return *this;
	}

	// Device runtime
	DEVICE unsigned short GetIntensity(const Vec3f& P) const
	{
//		RootNode[RootNode[0]]
		return 0;

//		const Vec3f NormalizedXYZ = (XYZ - this->BoundingBox.MinP) * this->InvSize;
//		return (float)USHRT_MAX * tex3D(VolumeTexture, NormalizedXYZ[0], NormalizedXYZ[1], NormalizedXYZ[2]);
	}

	HOST void Build(const Buffer3D<unsigned short>& Voxels, const BoundingBox& BoundingBox)
	{
		std::vector<OctreeNode> Nodes;

		int Depth = 0;

		OctreeNode Root(BoundingBox);
		Nodes.push_back(Root);

		Nodes[0].SubDivide(Voxels, Nodes, 0, Depth, this->MaxDepth);

		this->Free();

		Cuda::Allocate(this->RootNode, Nodes.size());
		Cuda::MemCopyHostToDevice(&Nodes[0], this->RootNode, Nodes.size()); 
	}

	HOST void Free()
	{
		Cuda::Free(this->RootNode);
	}

	int				MaxDepth;
	OctreeNode*		RootNode;
	float			Sigma;
};

}