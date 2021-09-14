#pragma once
// Inspired by Nicolai M. Josuttis and Bartłomiej Filipek

#include <cassert>
#include <iostream>
#include <memory_resource>
#include <string>

class logger_resource : public std::pmr::memory_resource {
private:
  std::pmr::memory_resource *upstream; // wrapped memory resource
  std::string name{};

public:
  explicit logger_resource(std::string p, std::pmr::memory_resource *us =
                                              std::pmr::get_default_resource())
      : upstream{us}, name{std::move(p)} { }

private:
  void *do_allocate(size_t bytes, size_t alignment) override {
    std::cout << name << " allocate " << bytes << " Bytes\n";
    void *ret = upstream->allocate(bytes, alignment);
    return ret;
  }

  void do_deallocate(void *ptr, size_t bytes, size_t alignment) override {
    std::cout << name << " deallocate " << bytes << " Bytes\n";
    upstream->deallocate(ptr, bytes, alignment);
  }

  bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
    // Verify that this equivalence is sufficient
    const auto *op = dynamic_cast<const logger_resource *>(&other);
    assert(op == nullptr || op->name != name || !upstream->is_equal(other));
    return (this == &other);
  }
};
