#pragma once

#include "esphome/core/component.h"
#include "remote_base.h"

namespace esphome {
namespace remote_base {

struct SymphonyData {
  uint64_t data;
  uint16_t nbits;

  bool operator==(const SymphonyData &rhs) const { return data == rhs.data && nbits == rhs.nbits; }
};

class SymphonyProtocol : public RemoteProtocol<SymphonyData> {
 public:
  void encode(RemoteTransmitData *dst, const SymphonyData &data) override;
  optional<SymphonyData> decode(RemoteReceiveData src) override;
  void dump(const SymphonyData &data) override;
};

DECLARE_REMOTE_PROTOCOL(Symphony)

template<typename... Ts> class SymphonyAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint64_t, data)
  TEMPLATABLE_VALUE(uint16_t, nbits)
  
  void encode(RemoteTransmitData *dst, Ts... x) override {
    SymphonyData data{};
    data.data = this->data_.value(x...);
    data.nbits = this->nbits_.value(x...);
    SymphonyProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome