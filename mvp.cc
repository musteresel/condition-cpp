#include "condition-cpp.hh"
using namespace condition;
#include <iostream>


struct some_condition {};


void foo() {
  signal(some_condition{});
}

#include <iostream>



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
                             std::cout << "Some condition came up! "
                                       << "Ignore?"
                                       << std::endl;
                             char c;
                             std::cin >> c;
                             for (auto r : get_restarts()) {
                               std::cout << "  Trying restart: " << (void*)r
                                         << std::endl;
                               restart_with<warn> * w =
                                 dynamic_cast<restart_with<warn> *>(r);
                               restart_with<ignore> * i =
                                 dynamic_cast<restart_with<ignore> *>(r);
                               if ((c != 'y') && w) {
                                 std::cout << "    => RESTART with warn!"
                                           << std::endl;
                                 w->restart({});
                               } else if ((c == 'y') && i) {
                                 std::cout << "    => RESTART with ignore!"
                                           << std::endl;
                                 i->restart({});
                               }
                             }
                             std::cout << "Hm .. that's bad" << std::endl;
                             assert(false);
                           });
  fn_for<some_condition> b([] (auto) {
                             std::cout << "This handler does nothing"
                                       << std::endl;
                           });
  with_restart<ignore>([]() {
                         with_restart<warn>([]() {
                                              foo();
                                            },
                           [](auto) {
                             std::cout << "WARNING" << std::endl;
                           });
                       },
    [](auto) { /* do nothing */ });
}
