// *****************************************************************************
//
// Copyright (c) 2017, Southwest Research Institute® (SwRI®)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Southwest Research Institute® (SwRI®) nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *****************************************************************************

#include <ros/ros.h>
#include <swri_image_util/blend_images_util.h>

namespace swri_image_util
{
  void blendImages(
      const cv::Mat& base_image,
      const cv::Mat& top_image,
      const double alpha,
      cv::Mat& dest_image)
  {
    // All images must have the same shape. Return without modifying anything
    // if this is not the case
    if ((base_image.rows != top_image.rows)
        || (base_image.cols != top_image.cols)
        || (base_image.rows != dest_image.rows)
        || (base_image.cols != dest_image.cols))
    {
      ROS_ERROR("Images to blend had incorrect shapes");
      return;
    }

    // Make sure all the image have the same type before modifying anything
    if ((base_image.type() != top_image.type())
        || (base_image.type() != dest_image.type()))
    {
      ROS_ERROR("Images to blend must have the same type");
      return;
    }

    // Sanity check the alpha value to make sure it is reasonable. Do not
    // modify anything if it is outside the expected range
    if ((alpha < 0.0) || (alpha > 1.0))
    {
      ROS_ERROR("Alpha value must be in the range [0, 1]");
      return;
    }

    cv::addWeighted(base_image, 1.0 - alpha, top_image, alpha, 0, dest_image);
  }
}
