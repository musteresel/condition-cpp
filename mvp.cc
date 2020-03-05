#include <cassert>
#include <vector>

#include <iostream>

struct base_handler;
inline std::vector<base_handler *> & handler_register();

/// Base handler - every handler is an instance of this class.
///
struct base_handler {
  /// Create a new handler and register it.
  /// 
  base_handler() {
    std::cout << "Register handler: " << (void*)this << std::endl;
    handler_register().push_back(this);
  }

  /// Destroy the handler after unregistering it.
  ///
  virtual ~base_handler() {
    std::cout << "Unregister handler: " << (void*)this << std::endl;
    assert(handler_register().size() > 0);
    assert(handler_register().back() == this);
    handler_register().pop_back();
  }

  /// Function to handle a condition if object is able to.
  ///
  /// A base_handler is not capable of handling any condition, objects
  /// of type handler_for<T> however are capable of handling a
  /// condition of type T.
  ///
  template<typename C>
  void handle(C condition);

  /// Apply all available handlers to the given condition.
  ///
  /// Iterate over the currently established handlers, from most
  /// recent to least recent, and let every hanlder try to handle the
  /// condition.
  ///
  template<typename C>
  static void apply_handlers(C condition) {
    for (auto it = handler_register().rbegin(); it != handler_register().rend(); ++it) {
      (*it)->handle(condition);
    }
  }
};

inline std::vector<base_handler *> & handler_register() {
  /// Thread local container with currently established handlers.
  ///
  /// This static class member holds a list of currently established
  /// handlers.  It is managed by this class and must not be touched.
  ///
  static thread_local std::vector<base_handler *> handlers;
  return handlers;
}


/// Interface for a handler which is able to handle a specific
/// condition.
///
template<typename Condition>
struct handler_for : public base_handler {
  virtual void operator()(Condition const &) = 0;
};


template<typename C>
void base_handler::handle(C condition) {
  handler_for<C> * const me = dynamic_cast<handler_for<C> *>(this);
  if (me) {
    (*me)(condition);
  }
}


template<typename T>
void signal(T condition) {
  base_handler::apply_handlers(condition);
}

#include <functional>

template<typename Condition>
struct fn_for : public handler_for<Condition> {
  std::function<void(Condition const &)> fn;
  template<typename Fn>
  fn_for(Fn && f) : fn(f) {}
  void operator()(Condition const & c) override {
    fn(c);
  }
};




/*

template<typename T>
void restart(std::string name, T) {
  // call the restart with the name, making sure the types match?
}

*/

struct some_condition {};

/*
struct warn : public restart { warn() : restart("warn") {} };
struct ignore : public restart { ignore() : restart("ignore") {} };

*/
void foo() {
  signal(some_condition{});
}

#include <iostream>


struct restart_base {
  restart_base();
  virtual ~restart_base();
};

inline std::vector<restart_base *> & restart_register() {
  static thread_local std::vector<restart_base *> restarts;
  return restarts;
}

restart_base::restart_base() {
  std::cout << "Registering restart: " << (void*) this << std::endl;
  restart_register().push_back(this);
}
restart_base::~restart_base() {
  std::cout << "Unregistering restart: " << (void*) this << std::endl;
  assert(restart_register().size() > 0);
  assert(restart_register().back() == this);
  restart_register().pop_back();
}

template<typename Restart>
struct restart_with : public restart_base {
  void restart(Restart const & r) {
    throw r;
  }
};

template<typename Restart>
class register_restart {
  restart_with<Restart> restart;
};

inline std::vector<restart_base *> const & get_restarts() {
  return restart_register();
}

struct warn{};
struct ignore{};

/*
template<typename... Conditions>
struct lambdas_for : public handler_for<Conditions>... {
  std::tuple<std::function<void(Conditions)>...> lambdas;
  };*/

int main() {
  fn_for<some_condition> o([] (auto) {
                             std::cout << "not printed" << std::endl;
                           });
  fn_for<some_condition> a([] (some_condition) {
                             std::cout << "This handler tries to issue a warning"
                                       << std::endl;
                             for (auto r : get_restarts()) {
                               std::cout << "  Trying restart: " << (void*)r
                                         << std::endl;
                               restart_with<warn> * w =
                                 dynamic_cast<restart_with<warn> *>(r);
                               if (w) {
                                 std::cout << "    => Works, RESTART!"
                                           << std::endl;
                                 w->restart({});
                               }
                             }
                           });
  fn_for<some_condition> b([] (auto) {
                             std::cout << "This handler does nothing"
                                       << std::endl;
                           });
  try {
    register_restart<warn> restarts;
    register_restart<ignore> restarts2;
    foo();
  } catch (ignore const &) {
  } catch (warn const &) {
    std::cout << "WARNING" << std::endl;
  }
}
