// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/quic/platform/impl/quic_chromium_clock.h"

#include "base/check_op.h"
#include "base/no_destructor.h"
#include "base/time/time.h"

namespace quic {

#if BUILDFLAG(IS_COBALT)
namespace {
QuicTime s_approximate_now{QuicTime::Zero()};
}  // namespace
#endif

QuicChromiumClock* QuicChromiumClock::GetInstance() {
  static base::NoDestructor<QuicChromiumClock> instance;
  return instance.get();
}

QuicChromiumClock::QuicChromiumClock() = default;

QuicChromiumClock::~QuicChromiumClock() = default;

#if BUILDFLAG(IS_COBALT)
void QuicChromiumClock::ZeroApproximateNow() {
  s_approximate_now = QuicTime::Zero();
}
#endif

QuicTime QuicChromiumClock::ApproximateNow() const {
  // At the moment, Chrome does not have a distinct notion of ApproximateNow().
  // We should consider implementing this using MessageLoop::recent_time_.
#if BUILDFLAG(IS_COBALT)
  if (s_approximate_now.IsInitialized()) {
    return s_approximate_now;
  }
#endif
  return Now();
}

QuicTime QuicChromiumClock::Now() const {
  int64_t ticks = (base::TimeTicks::Now() - base::TimeTicks()).InMicroseconds();
  DCHECK_GE(ticks, 0);
#if BUILDFLAG(IS_COBALT)
  s_approximate_now = CreateTimeFromMicroseconds(ticks);
  return s_approximate_now;
#else
  return CreateTimeFromMicroseconds(ticks);
#endif
}

QuicWallTime QuicChromiumClock::WallNow() const {
  const base::TimeDelta time_since_unix_epoch =
      base::Time::Now() - base::Time::UnixEpoch();
  int64_t time_since_unix_epoch_micro = time_since_unix_epoch.InMicroseconds();
  DCHECK_GE(time_since_unix_epoch_micro, 0);
  return QuicWallTime::FromUNIXMicroseconds(time_since_unix_epoch_micro);
}

// static
base::TimeTicks QuicChromiumClock::QuicTimeToTimeTicks(QuicTime quic_time) {
  // QuicChromiumClock defines base::TimeTicks() as equal to
  // quic::QuicTime::Zero(). See QuicChromiumClock::Now() above.
  QuicTime::Delta offset_from_zero = quic_time - QuicTime::Zero();
  int64_t offset_from_zero_us = offset_from_zero.ToMicroseconds();
  return base::TimeTicks() + base::Microseconds(offset_from_zero_us);
}

}  // namespace quic
