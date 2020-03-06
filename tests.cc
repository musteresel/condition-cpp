#include "condition-cpp.hh"
using namespace condition;


#include "catch2/catch.hpp"




TEST_CASE("Handler alone is not invoked", "[handler]") {
  struct some_condition {};
  bool invoked = false;
  {
    fn_for<some_condition> handler([&] (some_condition) {
                                     invoked = true;
                                   });
  }
  CHECK(invoked == false);
}


TEST_CASE("Condition does not invoke unrelated handlers, simply returns",
          "[handler][signal]") {
  struct some_condition {};
  struct some_other_condition {};
  bool invoked = false;
  bool returned = false;
  {
    fn_for<some_condition> handler([&] (some_condition) {
                                     invoked = true;
                                   });
    signal<some_other_condition>({});
    returned = true;
  }
  CHECK(invoked == false);
  CHECK(returned == true);
}
