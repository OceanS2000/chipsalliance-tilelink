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
  sparta::DataInPort<Payload> data{pset, "data" };
  sparta::SignalOutPort accept{pset, "accept" };

  void bind(TLChannelSource<Payload> &ano) {
    ano.bind(*this);
  }
};

template<HasSize Payload>
struct TLChannelSource{
  sparta::PortSet *pset;
  sparta::DataOutPort<Payload> data{pset, "data" };
  sparta::SignalInPort accept{pset, "accept" };

  void bind(TLChannelSink<Payload> &ano) {
    sparta::bind(data, ano.data);
    sparta::bind(accept, ano.accept);
  }
};

template<typename Types = DefaultTypes>
struct TLBundleSink {
  sparta::PortSet *pset;
  TLChannelSink<TLABMsg<Types>> a{pset};
  TLChannelSource<TLABMsg<Types>> b{pset};
  TLChannelSink<TLCMsg<Types>> c{pset};
  TLChannelSource<TLDMsg<Types>> d{pset};
  TLChannelSink<TLEMsg<Types>> e{pset};

  void bind(TLBundleSource<Types> &ano) {
    ano.bind(*this);
  }
};

template<typename Types>
struct TLBundleSource {
  sparta::PortSet *pset;
  TLChannelSource<TLABMsg<Types>> a{pset};
  TLChannelSink<TLABMsg<Types>> b{pset};
  TLChannelSource<TLCMsg<Types>> c{pset};
  TLChannelSink<TLDMsg<Types>> d{pset};
  TLChannelSource<TLEMsg<Types>> e{pset};

  void bind(TLBundleSink<Types> &ano) {
    a.bind(ano.a);
    b.bind(ano.b);
    c.bind(ano.c);
    d.bind(ano.d);
    e.bind(ano.e);
  }
};

#endif // __LINK__