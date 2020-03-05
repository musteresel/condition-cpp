/// \file condition-cpp.hh
/// \brief A lightweight condition system for C++, inspired by Common Lisp
///
/// This header file contains the code for a lightweight condition
/// system for C++.  Its design is inspired by the condition system
/// which is part of the Common Lisp language.  C++ exceptions divide
/// control flow in two parts: One throwing an exception and one
/// catching it and handling it.  This condition system divides
/// control flow in three parts: One signaling a condition, one
/// providing a way to handle it and one deciding whether / how to
/// handle it.  This makes it possible for high level code to decide
/// on a way to handle a condition which was signaled in low level
/// code without knowing *how* to actually handle it, because that was
/// provided by mid level code.
///
#ifndef CONDITION_CPP_HEADER
#define CONDITION_CPP_HEADER 1




#include <cassert>
#include <functional>
#include <vector>




namespace condition {




  namespace intern {




    struct base_handler;
    struct base_restart;




    inline std::vector<base_handler *> & handler_register();
    inline std::vector<base_restart *> & restart_register();



    /// \brief Base class for all handlers, takes care of handler registry.
    ///
    /// This is the base class from which all condition handlers
    /// derive.  It's main purpose is to take care of the registry of
    /// currently installed handlers.
    ///
    struct base_handler {


      /// \brief Construction of a handler automatically registers it
      /// with the handler registry.
      ///
      base_handler() {
        handler_register().push_back(this);
      }


      /// \brief Destruction of a handler removes it from the handler
      /// registry.
      ///
      /// \internal
      ///
      /// The destructor is `virtual` in order to allow down casts to
      /// derving handler classes.
      ///
      virtual ~base_handler() {
        assert(handler_register().size() > 0);
        assert(handler_register().back() == this);
        handler_register().pop_back();
      }
    };



    /// \brief Base class for all restart markers.
    ///
    /// A restart can be any user defined type, but in order to
    /// register possible restarts there has to be a type hierarchy.
    /// This class is the root of that hierarchy.
    ///
    struct base_restart {


      /// \brief Construction of a restart marker automatically
      /// registers the restart in the registry of currently installed
      /// restarts.
      ///
      base_restart() {
        restart_register().push_back(this);
      }


      /// \brief Destruction of the restart marker removes it from the
      /// registry of currently installed restarts.
      ///
      virtual ~base_restart() {
        assert(restart_register().size() > 0);
        assert(restart_register().back() == this);
        restart_register().pop_back();
      }
    };




    /// \brief Gives access to the thread local static handler
    /// registry.
    ///
    inline std::vector<base_handler *> & handler_register() {
      static thread_local std::vector<base_handler *> handlers;
      return handlers;
    }




    /// \brief Gives access to the thread local static restart
    /// registry.
    ///
    inline std::vector<base_restart *> & restart_register() {
      static thread_local std::vector<base_restart *> restarts;
      return restarts;
    }




  }




  template<typename Condition>
  struct handler_for : public intern::base_handler {


    virtual void operator()(Condition const &) = 0;
  };




  template<typename Restart>
  struct restart_with : public intern::base_restart {
    // TODO: Hide constructor from users.


    void restart(Restart const & r) {
      throw r;
    }
  };




  template<typename Restart>
  class register_restart {
    // TODO: Sanitize Restart type


    restart_with<Restart> restart;
  };




  template<typename Condition>
  void signal(Condition && condition) {
    for (auto it = intern::handler_register().rbegin();
         it != intern::handler_register().rend();
         ++it) {
      handler_for<Condition> * const handler =
        dynamic_cast<handler_for<Condition> *>(*it);
      if (handler) {
        // TODO: Perfect forwarding?
        (*handler)(condition);
      }
    }
  }




  inline std::vector<intern::base_restart *> const & get_restarts() {
    // TODO: vector with pointers to intern(al) types?  Maybe change
    // that.
    return intern::restart_register();
  }



  template<typename Condition>
  struct fn_for : public handler_for<Condition> {


    std::function<void(Condition const &)> fn;


    template<typename Fn>
    fn_for(Fn && f) : fn(f) {}


    void operator()(Condition const & c) override {
      fn(c);
    }
  };




  template<typename Restart, typename Code, typename RestartCode>
  auto with_restart(Code && code, RestartCode && restart) {
    try {
      register_restart<Restart> reg;
      return code();
    } catch (Restart const & r) {
      return restart(r);
    }
  }




}




#endif // CONDITION_CPP_HEADER
