#ifndef CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_RND_VIEW_HPP
#define CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_RND_VIEW_HPP

#include <exception>
#include <random>
#include <ranges>
#include <stdexcept>

class rnd_view : public std::ranges::view_interface<rnd_view> { // CRTP in action
private:
  struct Sentinel;

  struct Iterator {
  public:
    // using iterator_category = std::forward_iterator_tag;
    using iterator_category =
        std::bidirectional_iterator_tag; // can do reverse iteration but much slower
    using value_type = int;
    using difference_type = std::ptrdiff_t;

    Iterator() = default;

    explicit Iterator(unsigned int seed, unsigned long long rank)
        : m_seed(seed), m_rank(rank), m_engine(seed) {
      m_engine.discard(m_rank);
      update();
    }

    constexpr value_type operator*() const noexcept { return m_current; }

    Iterator &operator++() {
      update();
      return *this;
    }

    Iterator operator++(int) {
      auto tmp = *this;
      update();
      return tmp;
    }

    Iterator &operator+=(difference_type n) {
      if (n > 0) {
        m_rank += (n - 1);
        m_engine.discard(n - 1);
        update();
      } else if (n < 0) {
        if (m_rank < -n)
          throw std::out_of_range{"rnd_view reverse exceeded"};
        m_rank += n;
        force_update();
      }
      return *this;
    }

    Iterator &operator--() {
      reverse_update();
      return *this;
    }

    Iterator operator--(int) {
      auto tmp = *this;
      reverse_update();
      return tmp;
    }

    Iterator &operator-=(difference_type n) { return (*this) += (-n); }

    friend constexpr bool operator==(const Iterator &x, const Iterator &y) {
      return x.m_seed == y.m_seed && x.m_rank == y.m_rank;
    }

  private:
    void update() {
      m_current = m_dist(m_engine);
      ++m_rank;
    }

    void reverse_update() {
      if (m_rank <= 1)
        throw std::out_of_range{"rnd_view reverse exceeded"};
      m_rank -= 1;
      force_update();
    }

    void force_update() {
      m_engine.seed(m_seed);
      m_engine.discard(m_rank -= 1);
      update();
    }

  private:
    unsigned int m_seed = {};
    unsigned long long m_rank = {};
    std::mt19937 m_engine;
    std::uniform_int_distribution<int> m_dist;
    int m_current = {};
  };

  struct Sentinel {
    friend constexpr bool operator==(const Iterator &x, const Sentinel &y) {
      return false; // unreachable sentinel
    }
  };

public:
  rnd_view() : m_seed(std::random_device{}()) {}
  constexpr explicit rnd_view(unsigned int seed) : m_seed(seed) {}
  Iterator begin() const { return Iterator{m_seed, 0}; }
  constexpr auto end() const { return Sentinel{}; }

private:
  unsigned int m_seed = {};
};

namespace custom_views {

struct RndFactory {
  constexpr auto operator()(unsigned int seed) const { return rnd_view{seed}; }
};

inline constexpr RndFactory rnd{}; // factory object

} // namespace custom_views

#endif // CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_RND_VIEW_HPP
