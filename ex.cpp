#include "ex.hpp"

#include <cmath>
#include <enulib/action.hpp>
#include <enulib/asset.hpp>
#include "enu.token.hpp"

using namespace enumivo;
using namespace std;

void ex::buy(const currency::transfer &transfer) {

  if (transfer.to != _self) {
    return;
  }

  auto to = transfer.from;

  auto quantity = asset(transfer.quantity.amount, ELN_SYMBOL);

  action(permission_level{_self, N(active)}, N(eln.coin), N(transfer),
         std::make_tuple(_self, to, quantity,
                         std::string("LTS to ELN")))
      .send();
}

void ex::sell(const currency::transfer &transfer) {

  if (transfer.to != _self) {
    return;
  }

  auto to = transfer.from;

  auto quantity = asset(transfer.quantity.amount, LTS_SYMBOL);

  action(permission_level{_self, N(active)}, N(ltsonenumivo), N(transfer),
         std::make_tuple(_self, to, quantity,
                         std::string("ELN to LTS")))
      .send();
}

void ex::apply(account_name contract, action_name act) {
  if (contract == N(ltsonenumivo) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();
    enumivo_assert(transfer.quantity.symbol == LTS_SYMBOL,
                 "must send LTS");
    buy(transfer);
    return;
  }

  if (contract == N(eln.coin) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();
    enumivo_assert(transfer.quantity.symbol == ELN_SYMBOL,
                 "must send ELN");
    sell(transfer);
    return;
  }

  if (act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();
    enumivo_assert(false, "must send ELN or LTS");
    sell(transfer);
    return;
  }

  if (contract != _self) return;
}

extern "C" {
[[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  ex ltseln(receiver);
  ltseln.apply(code, action);
  enumivo_exit(0);
}
}
