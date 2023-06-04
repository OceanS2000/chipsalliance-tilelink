#include "Simulator.hpp"
#include "Master.hpp"
#include "Slave.hpp"
#include "Crossbar.hpp"

using MasterFactory = sparta::ResourceFactory<Master, Master::MasterParameterSet>;
using SlaveFactory = sparta::ResourceFactory<Slave, Slave::SlaveParameterSet>;
using CrossbarFactory = sparta::ResourceFactory<Crossbar, Crossbar::CrossbarParameterSet>;

Simulator::Simulator(sparta::Scheduler &scheduler)
  : sparta::app::Simulation("tlmodel", &scheduler) {
  
  getResourceSet()->addResourceFactory<MasterFactory>();
  getResourceSet()->addResourceFactory<SlaveFactory>();
  getResourceSet()->addResourceFactory<CrossbarFactory>();
}

Simulator::~Simulator() {
  getRoot()->enterTeardown();
}

void Simulator::buildTree_() {
  // Create crossbar
  sparta::ResourceTreeNode *crossbar = new sparta::ResourceTreeNode(
      getRoot(), "crossbar", sparta::TreeNode::GROUP_NAME_NONE,
      sparta::TreeNode::GROUP_IDX_NONE, "Crossbar node",
      getResourceSet()->getResourceFactory(Crossbar::name));
  to_delete_.emplace_back(crossbar);

  auto params = dynamic_cast<Crossbar::CrossbarParameterSet *>(crossbar->getParameterSet());
  sparta_assert(params != nullptr);
  auto master_cnt = params->sinks.getValue();
  auto slave_cnt = params->sources.getValue();

  // Create master
  for(size_t i = 0; i < master_cnt; ++i) {
    sparta::ResourceTreeNode *master = new sparta::ResourceTreeNode(
        getRoot(), "master_" + std::to_string(i), sparta::TreeNode::GROUP_NAME_NONE,
        sparta::TreeNode::GROUP_IDX_NONE, "Master node " + std::to_string(i),
        getResourceSet()->getResourceFactory(Master::name));
    to_delete_.emplace_back(master);
  }

  // Create slave
  for(size_t i = 0; i < slave_cnt; ++i) {
    sparta::ResourceTreeNode *slave = new sparta::ResourceTreeNode(
        getRoot(), "slave_" + std::to_string(i), sparta::TreeNode::GROUP_NAME_NONE,
        sparta::TreeNode::GROUP_IDX_NONE, "Slave node " + std::to_string(i),
        getResourceSet()->getResourceFactory(Slave::name));
    to_delete_.emplace_back(slave);
  }
}

void Simulator::configureTree_() {
  // Nothing to configure
}

void Simulator::bindTree_() {
  sparta::TreeNode* root = getRoot();

  auto crossbar = root->getChild("crossbar")->getResourceAs<Crossbar>();
  
  for(size_t i = 0; i < crossbar->get_sink_num(); ++i) {
    auto master = root->getChild("master_" + std::to_string(i))->getResourceAs<Master>();
    crossbar->bind_sink(i, *master->get_port());
  }
  for(size_t i = 0; i < crossbar->get_source_num(); ++i) {
    auto slave = root->getChild("slave_" + std::to_string(i))->getResourceAs<Slave>();
    crossbar->bind_src(i, *slave->get_port());
  }
}