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

FIND_PACKAGE(CUDA)

# Supported streaming architectures
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_10,code=sm_10;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_10,code=compute_10;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_11,code=sm_11;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_11,code=compute_11;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_12,code=sm_12;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_12,code=compute_12;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_13,code=sm_13;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_13,code=compute_13;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_20,code=sm_20;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_20,code=compute_20;${CUDA_NVCC_FLAGS}")
#SET(CUDA_NVCC_FLAGS "-OPT:Olimit=99999;${CUDA_NVCC_FLAGS}")

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CUDA_SDK_ROOT_DIR}/common/inc
	${CUDA_TOOLKIT_INCLUDE}
)

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
	volumes.h
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
	buffer1d.h
	buffer2d.h
	buffer3d.h
	boundingbox.h
	transferfunction.h
	rendersettings.h
	timing.h
	exception.h
	enums.h
	color.h
	exposurerender.h
	exposurerender.cpp
	tracer.h
	volume.h
	light.h
	scatterevent.h
	intersection.h
	object.h
	clippingobject.h
	texture.h
	bitmap.h
	buffer.h
	framebuffer.h
	pf.h
	plf.h
	pcf.h
	singlescattering.h
)

SOURCE_GROUP("General" FILES ${General})

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

CUDA_ADD_LIBRARY(ErCore ${General} ${Shapes} ${Bindable} ${Cuda} SHARED)

IF(NOT VTK_BINARY_DIR)
	FIND_PACKAGE(VTK)
	IF(NOT VTK_DIR)
	 MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
	ENDIF(NOT VTK_DIR)
	INCLUDE(${VTK_USE_FILE})
ENDIF(NOT VTK_BINARY_DIR)

INCLUDE(CMakeOptions.cmake)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_EXPORTING")

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_EXPORTING")

SET(General
	vtkErBitmap.h
	vtkErBitmap.cpp
	vtkErClippingObject.h
	vtkErClippingObject.cpp
	vtkErLight.h
	vtkErLight.cpp
	vtkErLocatable.h
	vtkErLocatable.cpp
	vtkErObject.h
	vtkErObject.cpp
	vtkErShapable.h
	vtkErShapable.cpp
	vtkErVolume.h
	vtkErVolume.cpp
	vtkErTracer.h
	vtkErTracer.cpp
	vtkErTexture.h
	vtkErTexture.cpp
	vtkErVolumeProperty.h
	vtkErVolumeProperty.cpp
	vtkErBindable.h
	vtkErBindable.cpp
	vtkErStable.h
	vtkErStable.cpp
	vtkErConfigure.h
	vtkErDll.h
)

SOURCE_GROUP("General" FILES ${General})

SET(WrapPython
	vtkErBitmap.h
	vtkErBitmap.cpp
	vtkErClippingObjectSource.h
	vtkErClippingObjectSource.cpp
	vtkErLightSource.h
	vtkErLightSource.cpp
	vtkErLocatable.h
	vtkErLocatable.cpp
	vtkErObjectSource.h
	vtkErObjectSource.cpp
	vtkErShapable.h
	vtkErShapable.cpp
	vtkErVolume.h
	vtkErVolume.cpp
	vtkErVolumeMapper.h
	vtkErVolumeMapper.cpp
	vtkErTextureSource.h
	vtkErTextureSource.cpp
)

ADD_LIBRARY(VtkEr ${General})
TARGET_LINK_LIBRARIES(VtkEr vtkCommon vtkRendering vtkGraphics vtkVolumeRendering ErCore)

if (MSVC)
   set_target_properties(VtkEr PROPERTIES COMPILE_FLAGS "/YuvtkErStable.h")
   set_source_files_properties(vtkErStable.cpp PROPERTIES COMPILE_FLAGS "/YcvtkErStable.h")
endif(MSVC)

IF(VTK_WRAP_PYTHON)
	INCLUDE_DIRECTORIES("${PYTHON_INCLUDE_PATH}")
	VTK_WRAP_PYTHON3(vtkErPython vtkErPython_SRCS "${WrapPython}")
	ADD_LIBRARY(vtkErPythonD ${vtkErPython_SRCS})
	PYTHON_ADD_MODULE(vtkErPython vtkErPythonInit.cxx)
	TARGET_LINK_LIBRARIES(vtkErPython vtkErPythonD)
	TARGET_LINK_LIBRARIES(vtkErPythonD VtkEr vtkCommonPythonD vtkRenderingPythonD vtkGraphicsPythonD vtkVolumeRenderingPythonD)
ENDIF(VTK_WRAP_PYTHON)

ADD_EXECUTABLE(vtkErExample vtkErExample.cpp)
TARGET_LINK_LIBRARIES(vtkErExample VtkEr)




