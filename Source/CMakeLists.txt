#	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
#	All rights reserved.
#
#	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
#	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#	
#	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 2.8)

PROJECT(ExposureRender)

# Use CUDA
FIND_PACKAGE(CUDA)

# Supported streaming architectures, uncomment the lines that pertain to the hardware your compiling for

# Tesla architecture: sm 1.0 and above
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_10,code=sm_10;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_10,code=compute_10;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_11,code=sm_11;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_11,code=compute_11;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_12,code=sm_12;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_12,code=compute_12;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_13,code=sm_13;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_13,code=compute_13;${CUDA_NVCC_FLAGS}")

# Fermi architecture: sm 2.0 and above
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_20,code=sm_20;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_20,code=compute_20;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_21,code=sm_21;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_21,code=compute_21;${CUDA_NVCC_FLAGS}")

# Kepler architecture: sm 3.0 and above
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_30,code=sm_30;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_30,code=compute_30;${CUDA_NVCC_FLAGS}")

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CUDA_SDK_ROOT_DIR}/common/inc
	${CUDA_TOOLKIT_INCLUDE}
)

# Make sure classes are exported
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_EXPORTING")

SOURCE_GROUP("Core" FILES ${vtkErCoreSources})

SET(General
	device.h
	utilities.h
	geometry.h
	transport.h
	raymarching.h
	shader.h
	sample.h
	rng.h
	montecarlo.h
	ray.h
	lights.h
	objects.h
	clippingobjects.h
	textures.h
	filter.h
	log.h
	defines.h
	camera.h
	procedural.h
	shape.h
	matrix.h
	vector.h
	boundingbox.h
	transferfunction.h
	rendersettings.h
	timing.h
	exception.h
	timestamp.h
	kelvin.h
	enums.h
	color.h
	exposurerender.h
	exposurerender.cpp
	tracer.h
	volume.h
	extinctionvolume.h
	light.h
	scatterevent.h
	intersection.h
	object.h
	clippingobject.h
	texture.h
	bitmap.h
	pf.h
	plf.h
	pcf.h
	singlescattering.h
	shadeevent.h
	alignment.h
	transform.h
	octree.h
)

SOURCE_GROUP("General" FILES ${General})

SET(Buffer
	buffer.h
	buffer1d.h
	buffer2d.h
	buffer3d.h
	cudatexture.h
	cudatexture3d.h
	randomseedbuffer2d.h
	randomseedbuffer3d.h
	framebuffer.h
)

SOURCE_GROUP("Buffer" FILES ${Buffer})

SET(Shapes
	shapes.h
	plane.h
	box.h
	disk.h
	ring.h
	sphere.h
	cylinder.h
)

SOURCE_GROUP("Shapes" FILES ${Shapes})

SET(Bindable
	erbindable.h
	ertracer.h
	ervolume.h
	erlight.h
	erobject.h
	erclippingobject.h
	ertexture.h
	erbitmap.h
)

SOURCE_GROUP("Bindable" FILES ${Bindable})

SET(Cuda
	singlescattering.cuh
	estimate.cuh
	gradientmagnitude.cuh
	filterrunningestimate.cuh
	filterframeestimate.cuh
	tonemap.cuh
	autofocus.cuh
	list.cuh
	wrapper.cuh
	macros.cuh
	core.cu
)

SOURCE_GROUP("Cuda" FILES ${Cuda})

# Create the ErCore project as a shared library (DLL)
CUDA_ADD_LIBRARY(ErCore ${General} ${Buffer} ${Shapes} ${Bindable} ${Cuda} SHARED)

# Use vtk
IF(NOT VTK_BINARY_DIR)
	FIND_PACKAGE(VTK)
	IF(NOT VTK_DIR)
	 MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
	ENDIF(NOT VTK_DIR)
	INCLUDE(${VTK_USE_FILE})
ENDIF(NOT VTK_BINARY_DIR)

INCLUDE(CMakeOptions.cmake)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Make sure classes are exported
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_EXPORTING")

SET(General
	vtkErLight.h
	vtkErLight.cpp
	vtkErObject.h
	vtkErObject.cpp
	vtkErVolume.h
	vtkErVolume.cpp
	vtkErTracer.h
	vtkErTracer.cpp
	vtkErTexture.h
	vtkErTexture.cpp
	vtkErBindable.h
	vtkErBindable.cpp
	vtkErCamera.h
	vtkErCamera.cpp
	vtkErAlignment.h
	vtkErAlignment.cpp
	vtkErBitmap.h
	vtkErBitmap.cpp
	vtkErShape.h
	vtkErShape.cpp
	vtkErTimerCallback.h
	vtkErTimerCallback.cpp
	vtkErStable.h
	vtkErStable.cpp
	vtkErConfigure.h
	vtkErDll.h
	vtkErUtilities.h
)

SOURCE_GROUP("General" FILES ${General})

# Sources that will be wrapped by python
SET(WrapPython
	vtkErShape.h
	vtkErShape.cpp
	vtkErAlignment.h
	vtkErAlignment.cpp
	vtkErLight.h
	vtkErLight.cpp
	vtkErObject.h
	vtkErObject.cpp
	vtkErVolume.h
	vtkErVolume.cpp
	vtkErTracer.h
	vtkErTracer.cpp
	vtkErTexture.h
	vtkErTexture.cpp
	vtkErCamera.h
	vtkErCamera.cpp
	vtkErBitmap.h
	vtkErBitmap.cpp
)

# Create VtkEr wrapping library
ADD_LIBRARY(VtkEr ${General})

# Link against ErCore
TARGET_LINK_LIBRARIES(VtkEr vtkCommon vtkRendering vtkGraphics vtkVolumeRendering ErCore OpenGL32.lib)

# Make use of precompiled headers to speed up compilation
if (MSVC)
   set_target_properties(VtkEr PROPERTIES COMPILE_FLAGS "/YuvtkErStable.h")
   set_source_files_properties(vtkErStable.cpp PROPERTIES COMPILE_FLAGS "/YcvtkErStable.h")
endif(MSVC)

# Python wrapping
IF(VTK_WRAP_PYTHON)
	INCLUDE_DIRECTORIES("${PYTHON_INCLUDE_PATH}")
	VTK_WRAP_PYTHON3(vtkErPython vtkErPython_SRCS "${WrapPython}")
	ADD_LIBRARY(vtkErPythonD ${vtkErPython_SRCS})
	PYTHON_ADD_MODULE(vtkErPython vtkErPythonInit.cxx)
	
	# Create the VtkEr Python wrapper
	TARGET_LINK_LIBRARIES(vtkErPython vtkErPythonD)
	
	# Create the VtkEr Python debug wrapper
	TARGET_LINK_LIBRARIES(vtkErPythonD VtkEr vtkCommonPythonD vtkRenderingPythonD vtkGraphicsPythonD vtkVolumeRenderingPythonD)
ENDIF(VTK_WRAP_PYTHON)

# Create an example project
ADD_EXECUTABLE(vtkErExample vtkErExample.cpp)

# Link it against the VtkEr wrapper
TARGET_LINK_LIBRARIES(vtkErExample VtkEr)
