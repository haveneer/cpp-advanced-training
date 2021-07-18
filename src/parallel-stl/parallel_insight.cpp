#include <version>
#ifndef __cpp_lib_parallel_algorithm
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_parallel_algorithm)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Colorization]
#include <iomanip>
#include <iostream>
#if __has_include("colorize.hpp")
#include "colorize.hpp"
#else
template <typename T> struct Colorize { T x; };
template <typename T>
std::ostream &operator<<(std::ostream &o, const Colorize<T> &t) {
  return o << t.x;
}
template <typename T> Colorize(T &&t) -> Colorize<T>;
#endif
//#endregion

//#region [Many includes]
#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <execution>
#include <iostream>
#include <limits>
#include <map>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <vector>
//#endregion

//#region [TBB config]
#ifdef WITH_TBB
#include <tbb/global_control.h>
#endif
//#endregion

struct SyncCout {
  SyncCout() : oss{} {
    std::ostream o(oss.rdbuf()); // safe RAII flags restore
    o << std::hex << "[" << Colorize{std::this_thread::get_id()} << "] ";
  }
  ~SyncCout() {
    oss << '\n';
    std::scoped_lock lock(m_mutex);
    std::cout << oss.str();
  }
  template <typename T> auto &operator<<(T &&val) { return oss << val; }
  std::ostringstream oss;
  static inline std::mutex m_mutex;
};

template <typename T> struct Inspector {
  static constexpr bool want_detail = false;

  Inspector(const T *origin) : origin{origin}, start{now()} {}
  Inspector(const Inspector &that) : origin{that.origin}, start{now()} {}
  Inspector(Inspector &&that) : origin{that.origin}, start{now()} {}
  Inspector &operator=(const Inspector &) = delete;
  Inspector &operator=(Inspector &&) = delete;
  ~Inspector() {
    if (first != last) {
      if constexpr (want_detail) {
        auto diff = std::chrono::high_resolution_clock::now() - start;
        SyncCout{}
            << std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
            << "_us on range [" << first << ":" << last
            << "] = " << (last - first + 1);
      }
      {
        std::lock_guard guard(id_mutex);
        auto [inserter, is_new_entry] =
            id_map.insert({std::this_thread::get_id(),
                           {0, std::numeric_limits<std::ptrdiff_t>::max(), 0, 0}});
        ChunkData &data = inserter->second;
        const auto size = last - first + 1;
        data.call_count++;
        data.min_size = std::min(data.min_size, size);
        data.max_size = std::max(data.max_size, size);
        data.cumul_size += size;
      }
    }
  }
  void inspect(const T &i) const {
    std::ptrdiff_t pos = &i - origin;
    if (first == std::numeric_limits<std::ptrdiff_t>::max())
      first = pos;
    last = pos;
  }

  const T *origin = nullptr;
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> now() {
    if constexpr (want_detail)
      return std::chrono::high_resolution_clock::now();
    else
      return {};
  }

  mutable std::ptrdiff_t first = std::numeric_limits<std::ptrdiff_t>::max();
  mutable std::ptrdiff_t last = std::numeric_limits<std::ptrdiff_t>::max();

  struct ChunkData {
    std::size_t call_count;
    std::ptrdiff_t min_size;
    std::ptrdiff_t max_size;
    std::size_t cumul_size;
  };
  static inline std::mutex id_mutex;
  static inline std::map<std::thread::id, ChunkData> id_map;
};

int main() {
#ifdef WITH_TBB
  // Init TBB Threads
  int pstl_num_threads = tbb::global_control::active_value(
      tbb::global_control::max_allowed_parallelism);
  if (auto pstl_num_threads_str = std::getenv("PSTL_NUM_THREADS")) {
    std::istringstream iss(pstl_num_threads_str);
    iss >> pstl_num_threads;
  }
  SyncCout{} << "PSTL_NUM_THREADS: " << pstl_num_threads;
  // Thanks to
  // https://docs.oneapi.io/versions/latest/onetbb/tbb_userguide/Migration_Guide/Task_Scheduler_Init.html
  tbb::global_control global_limit(tbb::global_control::max_allowed_parallelism,
                                   pstl_num_threads);
#endif

  std::vector<double> v(100'000'000, 0);

  //#region [Make random data]
  std::mt19937 engine(/* seed = */ 42);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  // not parallel since generator is not thread safe
  std::generate(std::begin(v), std::end(v), [&] { return dist(engine); });
  //#endregion

  auto start = std::chrono::high_resolution_clock::now();

  //  std::sort(std::execution::par, std::begin(v), std::end(v));

  std::transform(std::execution::par, //
                 std::begin(v), std::end(v), std::begin(v),
                 [inspector = Inspector{std::data(v)}](const auto &v) {
                   inspector.inspect(v); // HINT comment this to disable analysis
                   return std::log(std::abs(std::sin(v) * std::cos(v)) + 1);
                 });

  //#region [Do stats]
  auto diff = std::chrono::high_resolution_clock::now() - start;
  SyncCout{} << "Total : "
             << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
             << "_ms";
  // safe not concurrent area
  using InspectorT = Inspector<decltype(v)::value_type>;
  using ChunkData = InspectorT::ChunkData;
  const auto &id_map = InspectorT::id_map;
  if (!id_map.empty()) {
    auto [min_chunk_element, max_chunk_element] =
        std::minmax_element(id_map.begin(), id_map.end(), [](auto a, auto b) {
          const ChunkData &da = a.second;
          const ChunkData &db = b.second;
          return da.call_count < db.call_count;
        });
    auto min_chunk_size =
        std::min_element(id_map.begin(), id_map.end(), [](auto a, auto b) {
          const ChunkData &da = a.second;
          const ChunkData &db = b.second;
          return da.min_size < db.min_size;
        })->second.min_size;
    auto max_chunk_size =
        std::min_element(id_map.begin(), id_map.end(), [](auto a, auto b) {
          const ChunkData &da = a.second;
          const ChunkData &db = b.second;
          return da.max_size < db.max_size;
        })->second.max_size;
    auto chunk_count =
        std::accumulate(id_map.begin(), id_map.end(), 0, [](auto acc, auto v) {
          const ChunkData &dv = v.second;
          return acc + dv.call_count;
        });
    SyncCout{} << "using " << id_map.size() << " threads called ["
               << min_chunk_element->second.call_count << ":"
               << max_chunk_element->second.call_count << "] times with sizes in ["
               << min_chunk_size << ":" << max_chunk_size << "~"
               << v.size() / chunk_count << "]";
  }
  //#endregion
}

#endif