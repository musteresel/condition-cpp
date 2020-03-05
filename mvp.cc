#include "condition++.hh"
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
  with_restart<warn>([]() { foo(); },
                     [](auto) {
                       std::cout << "WARNING" << std::endl;
                     });
}
