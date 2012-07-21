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

#include "accelerator.h"
#include "volume.h"
#include "transferfunction1d.h"
#include "buffer1d.h"

namespace ExposureRender
{

#define GRID_EPSILON (0.000001f)

/*! Grid accelerator class */
class EXPOSURE_RENDER_DLL Grid : public TimeStamp
{
public:
	/*! Default constructor */
	HOST Grid() :
		TimeStamp(),
		MacroCellSize(8),
		Spacing(),
		Voxels(false, Enums::NearestNeighbour)
	{
	}
	
	/*! Copy constructor
		@param[in] Other Grid to copy
	*/
	HOST Grid(const Grid& Other) :
		TimeStamp(),
		MacroCellSize(8),
		Spacing(),
		Voxels(false, Enums::NearestNeighbour)
	{
		*this = Other;
	}

	/*! Assignment operator
		@param[in] Other Grid to copy
		@return Copied grid
	*/
	HOST Grid& Grid::operator = (const Grid& Other)
	{
		TimeStamp::operator = (Other);

		this->MacroCellSize		= Other.GetMacroCellSize();
		this->Spacing			= Other.GetSpacing();
//		this->Voxels			= Other.GetVoxels();

		return *this;
	}

	HOST void Build(Volume& Volume, const ScalarTransferFunction1D& Opacity1D)
	{
		this->BoundingBox	= Volume.BoundingBox;
		this->Spacing		= (float)this->MacroCellSize * Volume.Spacing;

		Buffer3D<int> EmptySpace("Empty space", Enums::Host);

		Vec3i Resolution = Volume.Voxels.GetResolution();

		Vec3i GridSize;
		
		for (int i = 0; i < 3; ++i)
			GridSize[i] = (int)ceil((float)Resolution[i] / (float)this->GetMacroCellSize());
		
		EmptySpace.Resize(GridSize);

		for (int GridX = 0; GridX < GridSize[0]; ++GridX)
		{
			for (int GridY = 0; GridY < GridSize[1]; ++GridY)
			{
				for (int GridZ = 0; GridZ < GridSize[2]; ++GridZ)
				{
					EmptySpace(Vec3i(GridX, GridY, GridZ)) = 1;

					for (int VoxelX = 0; VoxelX < this->GetMacroCellSize(); ++VoxelX)
					{
						for (int VoxelY = 0; VoxelY < this->GetMacroCellSize(); ++VoxelY)
						{
							for (int VoxelZ = 0; VoxelZ < this->GetMacroCellSize(); ++VoxelZ)
							{
								Vec3i VoxelCoord;

								VoxelCoord[0] = GridX * this->GetMacroCellSize() + VoxelX;
								VoxelCoord[1] = GridY * this->GetMacroCellSize() + VoxelY;
								VoxelCoord[2] = GridZ * this->GetMacroCellSize() + VoxelZ;

								if (Opacity1D.Evaluate(Volume.HostVoxels(VoxelCoord)) > 0.0f)
									EmptySpace(Vec3i(GridX, GridY, GridZ)) = 0;
							}
						}
					}
				}
			}
		}

		this->Voxels = EmptySpace;
	}

	DEVICE void GetNextBoundary(const Vec3f& Position, const Vec3f& Direction, float& NexT)
	{
		const Vec3f CellIndex;
		
		for (int i = 0; i < 3; ++i)
			floor((Position[i] - this->BoundingBox.GetMinP()[i]) / this->Spacing[i]);

		Vec3f T;

		for (int i = 0; i < 3; ++i)
		{
			if (Direction[i] > GRID_EPSILON)
			{
				T[i] = (this->BoundingBox.GetMinP()[i] + (CellIndex[i] + 1.0f) * this->Spacing[i] - Position[i]) / Direction[i];
			}
			else if (Direction[i] < -GRID_EPSILON)
			{
				T[i] = (this->BoundingBox.GetMinP()[i] + CellIndex[i] * this->Spacing[i] - Position[i]) / Direction[i];
			}
			else
			{
				T[i] = 1000.0f;
			}
		}

		NexT = min(T[0], min(T[1], T[2]));
	}

	GET_SET_MACRO(HOST_DEVICE, MacroCellSize, int)
	GET_SET_MACRO(HOST_DEVICE, Spacing, Vec3f)
	
	CudaTexture3D<int>		Voxels;				/*! Voxel texture */

protected:
	int						MacroCellSize;		/*! Macro cell size */
	Vec3f					Spacing;			/*! Spacing */
	BoundingBox				BoundingBox;		/*! Bounding box */
};

}