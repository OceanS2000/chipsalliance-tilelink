#ifndef __MASTER_H__
#define __MASTER_H__

#include "sparta/simulation/Unit.hpp"
#include "sparta/ports/PortSet.hpp"
#include "sparta/ports/DataPort.hpp"
#include "sparta/ports/SignalPort.hpp"
#include "sparta/events/UniqueEvent.hpp"
#include "sparta/simulation/ParameterSet.hpp"

#include "Type.hpp"
#include "Link.hpp"
#include "Rand.hpp"
#include <random>

class Simulator;

class Master : public sparta::Unit {
public:
  class MasterParameterSet : public sparta::ParameterSet {
  public:
    MasterParameterSet(sparta::TreeNode *n) :
      sparta::ParameterSet(n)
    {}

    const AddrRanges default_range = {{0, 0x100000000}};

    PARAMETER(uint64_t, seed, 0x19260817, "Seed for generating data")
    PARAMETER(AddrRanges, downstreams, default_range, "Valid downstreams")
    PARAMETER(uint64_t, id, 0, "Master id")
  };

  Master(sparta::TreeNode *node, const MasterParameterSet *params);

  static const char *name;

  TLBundleSource<>* get_port(){return &port;}
  
private:
  TLBundleSource<> port{&unit_port_set_};

  sparta::UniqueEvent<> next_a {
    &unit_event_set_, "next_a",
    CREATE_SPARTA_HANDLER(Master, send_a),
    1
  };

  sparta::UniqueEvent<> next_d {
    &unit_event_set_, "next_d",
    CREATE_SPARTA_HANDLER(Master, grant_d),
    1 // FIXME: move this delay to the producer side
  };

  RandGen gen_a;
  uint64_t id;

  std::uniform_int_distribution<size_t> dist_a_downstream;
  std::vector<std::uniform_int_distribution<uint64_t>> dist_a_addrs;
  std::uniform_int_distribution<uint8_t> dist_a_size{ 3, 7 }; // [8, 128]

  void send_a();
  void accept_a();
  void data_d(const TLDMsg<> &msg);
  void grant_d();

};

#endif // __MASTER_H__