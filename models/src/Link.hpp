#ifndef __LINK__
#define __LINK__

#include <limits>
#include <sparta/ports/PortSet.hpp>
#include <string_view>

#include "sparta/simulation/Unit.hpp"
#include "sparta/ports/DataPort.hpp"
#include "sparta/ports/SignalPort.hpp"
#include "sparta/simulation/ParameterSet.hpp"

#include "TLMsg.hpp"

template<HasSize Payload>
struct TLChannelSource;
template<typename Types = DefaultTypes>
struct TLBundleSource;

template<HasSize Payload>
struct TLChannelSink {
  sparta::PortSet *pset;
  std::string channel;
  sparta::DataInPort<Payload> data{pset, channel + "_data_in" };
  sparta::SignalOutPort accept{pset,  channel + "_ack_out" };

  void bind(TLChannelSource<Payload> &ano) {
    ano.bind(*this);
  }
};

template<HasSize Payload>
struct TLChannelSource{
  sparta::PortSet *pset;
  std::string channel;
  sparta::DataOutPort<Payload> data{pset, channel + "_data_out" };
  sparta::SignalInPort accept{pset, channel + "_ack_in" };

  void bind(TLChannelSink<Payload> &ano) {
    sparta::bind(data, ano.data);
    sparta::bind(accept, ano.accept);
  }
};

template<typename Types = DefaultTypes>
struct TLBundleSink {
  sparta::PortSet *pset;
  TLChannelSink<TLABMsg<Types>> a{pset, "a"};
  TLChannelSource<TLABMsg<Types>> b{pset, "b"};
  TLChannelSink<TLCMsg<Types>> c{pset, "c"};
  TLChannelSource<TLDMsg<Types>> d{pset, "d"};
  TLChannelSink<TLEMsg<Types>> e{pset, "e"};

  void bind(TLBundleSource<Types> &ano) {
    ano.bind(*this);
  }
};

template<typename Types>
struct TLBundleSource {
  sparta::PortSet *pset;
  TLChannelSource<TLABMsg<Types>> a{pset, "a"};
  TLChannelSink<TLABMsg<Types>> b{pset, "b"};
  TLChannelSource<TLCMsg<Types>> c{pset, "c"};
  TLChannelSink<TLDMsg<Types>> d{pset, "d"};
  TLChannelSource<TLEMsg<Types>> e{pset, "e"};

  void bind(TLBundleSink<Types> &ano) {
    a.bind(ano.a);
    b.bind(ano.b);
    c.bind(ano.c);
    d.bind(ano.d);
    e.bind(ano.e);
  }
};

#endif // __LINK__