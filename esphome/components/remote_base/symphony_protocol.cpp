#include "symphony_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.symphony";

// This is based on https://github.com/crankyoldgit/IRremoteESP8266/blob/master/src/ir_Symphony.cpp
// Licensed under the LGPL-2.1 license (https://github.com/crankyoldgit/IRremoteESP8266/blob/master/LICENSE.txt)

static const uint16_t ZERO_MARK = 400;
static const uint16_t ZERO_SPACE = 1250;
static const uint16_t ONE_MARK = ZERO_SPACE;
static const uint16_t ONE_SPACE = ZERO_MARK;

void SymphonyProtocol::encode(RemoteTransmitData *dst, const SymphonyData &data) {
  dst->set_carrier_frequency(38000);
  dst->reserve(2 + data.nbits * 2u);

  for (uint32_t mask = 1UL << (data.nbits - 1); mask != 0; mask >>= 1) {
    if (data.data & mask) {
      dst->item(ONE_MARK, ONE_SPACE);
    } else {
      dst->item(ZERO_MARK, ZERO_SPACE);
    }
  }
}
optional<SymphonyData> SymphonyProtocol::decode(RemoteReceiveData src) {
  SymphonyData out{
      .data = 0,
      .nbits = 0,
  };

  for (; out.nbits < 20; out.nbits++) {
    uint32_t bit;
    if (src.expect_mark(ONE_MARK)) {
      bit = 1;
    } else if (src.expect_mark(ZERO_MARK)) {
      bit = 0;
    } else if (out.nbits == 12 || out.nbits == 15) {
      return out;
    } else {
      return {};
    }

    out.data = (out.data << 1UL) | bit;
    if (src.expect_space(ZERO_SPACE)) {
      // nothing needs to be done
    } else if (src.peek_space_at_least(ONE_SPACE)) {
      out.nbits += 1;
      if (out.nbits == 12 || out.nbits == 15 || out.nbits == 20)
        return out;
      return {};
    } else {
      return {};
    }
  }

  return out;
}
void SymphonyProtocol::dump(const SymphonyData &data) {
  ESP_LOGD(TAG, "Received Symphony: data=0x%08X, nbits=%d", data.data, data.nbits);
}

}  // namespace remote_base
}  // namespace esphome
