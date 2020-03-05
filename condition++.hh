#ifndef CONDITION_PLUS_PLUS_HEADER
#define CONDITION_PLUS_PLUS_HEADER 1




#include <cassert>
#include <functional>
#include <vector>




namespace condition {




  namespace intern {




    struct base_handler;
    struct base_restart;




    inline std::vector<base_handler *> & handler_register();
    inline std::vector<base_restart *> & restart_register();



    struct base_handler {


      base_handler() {
        handler_register().push_back(this);
      }


      virtual ~base_handler() {
        assert(handler_register().size() > 0);
        assert(handler_register().back() == this);
        handler_register().pop_back();
      }
    };



    struct base_restart {


      base_restart() {
        restart_register().push_back(this);
      }


      virtual ~base_restart() {
        assert(restart_register().size() > 0);
        assert(restart_register().back() == this);
        restart_register().pop_back();
      }
    };




    inline std::vector<base_handler *> & handler_register() {
      static thread_local std::vector<base_handler *> handlers;
      return handlers;
    }




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




}




#endif // CONDITION_PLUS_PLUS_HEADER
