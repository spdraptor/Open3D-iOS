// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "open3d/core/Tensor.h"
#include "open3d/core/hashmap/HashMap.h"
#include "open3d/t/geometry/Geometry.h"
#include "open3d/t/geometry/Image.h"
#include "open3d/t/geometry/PointCloud.h"
#include "open3d/t/geometry/TensorMap.h"
#include "open3d/t/geometry/TriangleMesh.h"

namespace open3d {
namespace t {

namespace io {
class TSDFVoxelGridMetadata;
}

namespace geometry {

/// Scalable voxel grid specialized for TSDF integration.
/// The 3D space is organized in such a way:
/// Space is first coarsely divided into blocks that can be indexed by 3D
/// coordinates.
/// Each block is then further divided into voxels as a Tensor of shape
/// (resolution, resolution, resolution, channel).
/// For pure geometric TSDF voxels, channel = 2 (TSDF + weight).
/// For colored TSDF voxels, channel = 5 (TSDF + weight + color).
/// Users may specialize their own channels that can be reinterpreted from the
/// internal Tensor.
class TSDFVoxelGrid {
public:
    /// \brief Default Constructor.
    TSDFVoxelGrid(std::unordered_map<std::string, core::Dtype> attr_dtype_map =
                          {{"tsdf", core::Float32},
                           {"weight", core::UInt16},
                           {"color", core::UInt16}},
                  float voxel_size = 3.0 / 512.0, /* in meter */
                  float sdf_trunc = 0.04,         /* in meter */
                  int64_t block_resolution = 16,  /* block Tensor resolution */
                  int64_t block_count = 1000,
                  const core::Device &device = core::Device("CPU:0"),
                  const core::HashBackendType &backend =
                          core::HashBackendType::Default);

    ~TSDFVoxelGrid(){};

    /// Depth-only integration.
    void Integrate(const Image &depth,
                   const core::Tensor &intrinsics,
                   const core::Tensor &extrinsics,
                   float depth_scale = 1000.0f,
                   float depth_max = 3.0f);

    /// RGB-D integration.
    void Integrate(const Image &depth,
                   const Image &color,
                   const core::Tensor &intrinsics,
                   const core::Tensor &extrinsics,
                   float depth_scale = 1000.0f,
                   float depth_max = 3.0f);

    enum SurfaceMaskCode {
        None = 0,
        VertexMap = (1 << 0),
        DepthMap = (1 << 1),
        ColorMap = (1 << 2),
        NormalMap = (1 << 3),
        RangeMap = (1 << 4)
    };
    /// Use volumetric ray casting to obtain vertex and color maps, mainly for
    /// dense visual odometry.
    /// intrinsics and extrinsics defines the camera properties for image
    /// generation. width and height defines the image size.
    /// Note: vertex map is
    /// interpolated along the ray, but color map is not trilinearly
    /// interpolated due to performance requirements. Colormap is only used for
    /// a reference now.
    std::unordered_map<SurfaceMaskCode, core::Tensor> RayCast(
            const core::Tensor &intrinsics,
            const core::Tensor &extrinsics,
            int width,
            int height,
            float depth_scale = 1000.0f,
            float depth_min = 0.1f,
            float depth_max = 3.0f,
            float weight_threshold = 3.0f,
            int ray_cast_mask = SurfaceMaskCode::DepthMap |
                                SurfaceMaskCode::ColorMap);

    /// Extract point cloud near iso-surfaces.
    /// Weight threshold is used to filter outliers. By default we use 3.0,
    /// where we assume a reliable surface point comes from the fusion of at
    /// least 3 viewpoints. Use as low as 0.0 to accept all the possible
    /// observations.
    PointCloud ExtractSurfacePoints(
            int estimate_number = -1,
            float weight_threshold = 3.0f,
            int surface_mask = SurfaceMaskCode::VertexMap |
                               SurfaceMaskCode::ColorMap);

    /// Extract mesh near iso-surfaces with Marching Cubes.
    /// Weight threshold is used to filter outliers. By default we use 3.0,
    /// where we assume a reliable surface point comes from the fusion of at
    /// least 3 viewpoints. Use as low as 0.0 to accept all the possible
    /// observations.
    TriangleMesh ExtractSurfaceMesh(
            int estimate_vertices = -1,
            float weight_threshold = 3.0f,
            int surface_mask = SurfaceMaskCode::VertexMap |
                               SurfaceMaskCode::NormalMap |
                               SurfaceMaskCode::ColorMap);

    /// Convert TSDFVoxelGrid to the target device.
    /// \param device The targeted device to convert to.
    /// \param copy If true, a new TSDFVoxelGrid is always created; if false,
    /// the copy is avoided when the original TSDFVoxelGrid is already on the
    /// targeted device.
    TSDFVoxelGrid To(const core::Device &device, bool copy = false) const;

    /// Clone TSDFVoxelGrid on the same device.
    TSDFVoxelGrid Clone() const { return To(GetDevice(), true); }

    float GetVoxelSize() const { return voxel_size_; }

    float GetSDFTrunc() const { return sdf_trunc_; }

    int64_t GetBlockResolution() const { return block_resolution_; }

    int64_t GetBlockCount() const { return block_count_; }

    std::unordered_map<std::string, core::Dtype> GetAttrDtypeMap() const {
        return attr_dtype_map_;
    }

    core::Device GetDevice() const { return device_; }

    std::shared_ptr<core::HashMap> GetBlockHashMap() { return block_hashmap_; }

    std::shared_ptr<core::HashMap> GetBlockHashMap() const {
        return block_hashmap_;
    }

private:
    float voxel_size_;

    float sdf_trunc_;

    int64_t block_resolution_;

    int64_t block_count_;

    std::unordered_map<std::string, core::Dtype> attr_dtype_map_;

    core::Device device_ = core::Device("CPU:0");

    /// Return buf_indices and masks for radius (3) neighbor entries.
    /// We first find all active entries in the hashmap with there coordinates.
    /// We then query these coordinates and their 3^3 neighbors.
    /// buf_indices_nb: indexer used for the internal hashmap to access voxel
    /// block coordinates in the 3^3 neighbors. masks_nb: flag used for hashmap
    /// to indicate whether a query is a success. Currently we preserve a dense
    /// output (27 x active_entries) without compression / reduction.
    std::pair<core::Tensor, core::Tensor> BufferRadiusNeighbors(
            const core::Tensor &active_buf_indices);

    // Global hashmap
    std::shared_ptr<core::HashMap> block_hashmap_;

    // Local hashmap for the `unique` operation of input points
    std::shared_ptr<core::HashMap> point_hashmap_;

    core::Tensor active_block_coords_;
};
}  // namespace geometry
}  // namespace t
}  // namespace open3d
