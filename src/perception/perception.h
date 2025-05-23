#pragma once

#include <atomic>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <vector>

#define ORT_NO_EXCEPTIONS
#include <onnxruntime_cxx_api.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/videoio.hpp>

namespace simulo {

class Perception {
public:
   struct Keypoint {
      float x;
      float y;
      float visibility;
   };

   struct Detection {
      float x;
      float y;
      float width;
      float height;
      float confidence;
      std::vector<Keypoint> points;
   };

   Perception(std::shared_ptr<const Ort::Env> ort_env, int id);

   ~Perception();

   void set_running(bool run);

   std::vector<Detection> latest_detections() {
      std::shared_lock lock(detection_lock_);
      return latest_detections_;
   }

   bool is_calibrated() const {
      return calibrated_;
   }

   void debug_window();

private:
   void detect();
   bool detect_calibration_marker(cv::Mat &frame);
   void apply_calibration_transform(Detection &detection);

   int id_;
   cv::VideoCapture capture_;
   std::shared_ptr<const Ort::Env> ort_env_;
   Ort::Session ort_session_;
   Ort::MemoryInfo ort_mem_info_;
   Ort::Allocator ort_allocator_;
   Ort::Value ort_input_;
   Ort::Value ort_output_;
   std::atomic<bool> running_;
   std::thread thread_;

   std::shared_mutex detection_lock_;
   cv::Mat latest_frame_;
   std::vector<Detection> latest_detections_;

   std::atomic<bool> calibrated_{false};
   cv::Mat perspective_transform_;
};

} // namespace simulo
