// Copyright (c) 2020. All rights reserved.
// Author: lisilin013@163.com(Silin Li) on 2020/9/26.

#pragma once

#include <algorithm>
#include <chrono>
#include <random>
#include <set>
#include <vector>

#include <glog/logging.h>

#include "common/log_out.h"
#include "long_term_relocalization/utils/constants.h"

namespace long_term_relocalization {

std::string GetCurrentPackagePath();
std::string GetDataDirectoryPath();
std::string GetMapDirectoryPath();
std::string GetConfigFilePath();
std::string GetRS80AngleFilePath();

struct Color {
  Color(float red, float green, float blue) : r(red), g(green), b(blue) {}
  float r;
  float g;
  float b;
};

// Return index.
template <typename T>
int FindClosestValue(const std::vector<T> &data /*MUST be sorted*/, const T &val) {
  int found = 0;
  if (data.front() < data.back()) {
    found = std::upper_bound(data.begin(), data.end(), val) - data.begin();
  } else {
    found = data.rend() - std::upper_bound(data.rbegin(), data.rend(), val);
  }
  if (found == 0) {
    return found;
  }
  if (found == data.size()) {
    return found - 1;
  }
  const int diff_next = std::abs(data[found] - val);
  const int diff_prev = std::abs(val - data[found - 1]);
  return diff_next < diff_prev ? found : found - 1;
}

template <typename T>
std::vector<int> FindClosestValues(const std::vector<T> &data /*MUST be sorted*/, const T &val,
                                   int num_closest) {
  // CHECK_GE(data.size(), num_closest);
  const int cloest_ind = FindClosestValue(data, val);
  const int left = std::max<int>(cloest_ind - num_closest + 1, 0);
  const int right = std::min<int>(cloest_ind + num_closest - 1, data.size() - 1);

  // NOTE: we didn't use `map` here, cause using double/float as key is not stable.
  std::vector<std::pair<int /*index*/, T /*value*/>> closest_candidates;
  const int closest_candidates_size = right - left + 1;
  closest_candidates.reserve(closest_candidates_size);
  for (int i = left; i <= right; ++i) {
    closest_candidates.emplace_back(i, data[i]);
  }
  std::sort(closest_candidates.begin(), closest_candidates.end(),
            [](const std::pair<int, T> &lhs, const std::pair<int, T> &rhs) {
              return lhs.second < rhs.second;
            });

  const int closest_indices_size = std::min(closest_candidates_size, num_closest);
  std::vector<int> closest_indices(closest_indices_size);
  for (int i = 0; i < closest_indices_size; ++i) {
    closest_indices[i] = closest_candidates[i].first;
  }
  return closest_indices;
}

std::set<int> RandomDownsamplingArray(int max_size, double remain_ratio);

} // namespace long_term_relocalization
