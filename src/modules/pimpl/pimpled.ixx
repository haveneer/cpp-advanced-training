export module pimpled;
struct Impl;

export class S {
public:
  S();
  ~S();
  //#region [Delete copy and move ops]
  S(const S &) = delete;
  S(S &&) = delete;
  S &operator=(const S &) = delete;
  S &operator=(const S &&) = delete;
  //#endregion

  void do_stuff();
  Impl *get() const { return m_impl; }

private:
  Impl *m_impl; // FIXME prefer std::unique_ptr when available
};

module : private; // Everything beyond this point is not available to importers

struct Impl {
  void do_stuff() {}
};

S::S() : m_impl{new Impl{}} {}

S::~S() { delete m_impl; }

void S::do_stuff() { get()->do_stuff(); }