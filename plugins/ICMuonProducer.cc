#include "UserCode/ICHiggsTauTau/plugins/ICMuonProducer.hh"
#include <string>
#include <vector>
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "UserCode/ICHiggsTauTau/interface/StaticTree.hh"
#include "UserCode/ICHiggsTauTau/interface/Muon.hh"
#include "UserCode/ICHiggsTauTau/interface/city.h"
#include "UserCode/ICHiggsTauTau/plugins/PrintConfigTools.h"

ICMuonProducer::IsoTags::IsoTags(edm::ParameterSet const& pset)
    : charged_all(pset.getParameter<edm::InputTag>("chargedAll")),
      charged(pset.getParameter<edm::InputTag>("charged")),
      neutral(pset.getParameter<edm::InputTag>("neutral")),
      gamma(pset.getParameter<edm::InputTag>("gamma")),
      pu(pset.getParameter<edm::InputTag>("pu")) {}

ICMuonProducer::ICMuonProducer(const edm::ParameterSet& config)
    : input_(config.getParameter<edm::InputTag>("input")),
      branch_(config.getParameter<std::string>("branch")),
      is_pf_(config.getParameter<bool>("isPF")),
      input_vertices_(config.getParameter<edm::InputTag>("inputVertices")),
      do_vertex_ip_(config.getParameter<bool>("includeVertexIP")),
      input_beamspot_(config.getParameter<edm::InputTag>("inputBeamspot")),
      do_beamspot_ip_(config.getParameter<bool>("includeBeamspotIP")),
      pf_iso_03_(config.getParameterSet("pfIso03")),
      pf_iso_04_(config.getParameterSet("pfIso04")),
      do_pf_iso_03_(config.getParameter<bool>("includePFIso03")),
      do_pf_iso_04_(config.getParameter<bool>("includePFIso04"))  {
  muons_ = new std::vector<ic::Muon>();

  edm::ParameterSet pset_floats =
      config.getParameter<edm::ParameterSet>("includeFloats");
  std::vector<std::string> vec =
      pset_floats.getParameterNamesForType<edm::InputTag>();
  for (unsigned i = 0; i < vec.size(); ++i) {
    input_vmaps_.push_back(std::make_pair(
        vec[i], pset_floats.getParameter<edm::InputTag>(vec[i])));
  }
  PrintHeaderWithProduces(config, input_, branch_);
  PrintOptional(1, is_pf_, "isPF");
  PrintOptional(1, do_vertex_ip_, "includeVertexIP");
  PrintOptional(1, do_beamspot_ip_, "includeBeamspotIP");
  PrintOptional(1, do_pf_iso_03_, "includePFIso03");
  PrintOptional(1, do_pf_iso_04_, "includePFIso04");
}

ICMuonProducer::~ICMuonProducer() { delete muons_; }

void ICMuonProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
  edm::Handle<edm::View<reco::Muon> > muons_handle;
  edm::Handle<edm::View<reco::PFCandidate> > pfs_handle;
  unsigned n_muons = 0;
  if (is_pf_) {
    event.getByLabel(input_, pfs_handle);
    n_muons = pfs_handle->size();
  } else {
    event.getByLabel(input_, muons_handle);
    n_muons = muons_handle->size();
  }

  edm::Handle<edm::View<reco::Vertex> > vertices_handle;
  if (do_vertex_ip_) event.getByLabel(input_vertices_, vertices_handle);

  edm::Handle<reco::BeamSpot> beamspot_handle;
  if (do_beamspot_ip_) event.getByLabel(input_beamspot_, beamspot_handle);

  std::vector<edm::Handle<edm::ValueMap<float> > > float_handles(
      input_vmaps_.size());
  for (unsigned i = 0; i < float_handles.size(); ++i) {
    event.getByLabel(input_vmaps_[i].second, float_handles[i]);
  }

  edm::Handle<edm::ValueMap<double> > charged_all_03;
  edm::Handle<edm::ValueMap<double> > charged_03;
  edm::Handle<edm::ValueMap<double> > neutral_03;
  edm::Handle<edm::ValueMap<double> > gamma_03;
  edm::Handle<edm::ValueMap<double> > pu_03;
  if (do_pf_iso_03_) {
    event.getByLabel(pf_iso_03_.charged_all, charged_all_03);
    event.getByLabel(pf_iso_03_.charged, charged_03);
    event.getByLabel(pf_iso_03_.neutral, neutral_03);
    event.getByLabel(pf_iso_03_.gamma, gamma_03);
    event.getByLabel(pf_iso_03_.pu, pu_03);
  }

  edm::Handle<edm::ValueMap<double> > charged_all_04;
  edm::Handle<edm::ValueMap<double> > charged_04;
  edm::Handle<edm::ValueMap<double> > neutral_04;
  edm::Handle<edm::ValueMap<double> > gamma_04;
  edm::Handle<edm::ValueMap<double> > pu_04;
  if (do_pf_iso_04_) {
    event.getByLabel(pf_iso_04_.charged_all, charged_all_04);
    event.getByLabel(pf_iso_04_.charged, charged_04);
    event.getByLabel(pf_iso_04_.neutral, neutral_04);
    event.getByLabel(pf_iso_04_.gamma, gamma_04);
    event.getByLabel(pf_iso_04_.pu, pu_04);
  }

  // Prepare output collection
  muons_->clear();
  muons_->resize(n_muons, ic::Muon());

  for (unsigned i = 0; i < n_muons; ++i) {
    ic::Muon & dest = muons_->at(i);

    reco::MuonRef muon_ref;
    edm::RefToBase<reco::Muon> muon_base_ref;
    edm::RefToBase<reco::PFCandidate> pf_base_ref;
    if (is_pf_) {
      reco::PFCandidate const& pf_src = pfs_handle->at(i);
      dest.set_id(pf_hasher_(&pf_src));
      dest.set_pt(pf_src.pt());
      dest.set_eta(pf_src.eta());
      dest.set_phi(pf_src.phi());
      dest.set_energy(pf_src.energy());
      dest.set_charge(pf_src.charge());
      muon_ref = pf_src.muonRef();
      pf_base_ref = pfs_handle->refAt(i);
    } else {
      reco::Muon const& reco_src = muons_handle->at(i);
      dest.set_id(muon_hasher_(&reco_src));
      dest.set_pt(reco_src.pt());
      dest.set_eta(reco_src.eta());
      dest.set_phi(reco_src.phi());
      dest.set_energy(reco_src.energy());
      dest.set_charge(reco_src.charge());
      muon_base_ref = muons_handle->refAt(i);
    }

    reco::Muon const& src =
        is_pf_ ? *(pfs_handle->at(i).muonRef()) : *muons_handle->refAt(i);

    dest.set_dr03_tk_sum_pt(src.isolationR03().sumPt);
    dest.set_dr03_ecal_rechit_sum_et(src.isolationR03().emEt);
    dest.set_dr03_hcal_tower_sum_et(src.isolationR03().hadEt);


    dest.set_is_standalone(src.isStandAloneMuon());
    dest.set_is_global(src.isGlobalMuon());
    dest.set_is_tracker(src.isTrackerMuon());
    dest.set_is_calo(src.isCaloMuon());
    dest.set_matched_stations(src.numberOfMatchedStations());

    if (src.isGlobalMuon() && src.globalTrack().isNonnull()) {
      dest.set_gt_normalized_chi2(src.globalTrack()->normalizedChi2());
      dest.set_gt_valid_muon_hits(
          src.globalTrack()->hitPattern().numberOfValidMuonHits());
    }

    if (src.isTrackerMuon() && src.innerTrack().isNonnull()) {
      dest.set_it_pixel_hits(
          src.innerTrack()->hitPattern().numberOfValidPixelHits());
      dest.set_it_tracker_hits(
          src.innerTrack()->hitPattern().numberOfValidTrackerHits());
      dest.set_it_layers_with_measurement(
          src.innerTrack()->hitPattern().trackerLayersWithMeasurement());
    }

    dest.set_vx(src.vx());
    dest.set_vy(src.vy());
    dest.set_vz(src.vz());

    for (unsigned v = 0; v < float_handles.size(); ++v) {
      if (is_pf_) {
        dest.SetIdIso(input_vmaps_[v].first,
                    (*(float_handles[v]))[pf_base_ref]);
      } else {
        dest.SetIdIso(input_vmaps_[v].first,
                    (*(float_handles[v]))[muon_base_ref]);
      }
    }

    if (is_pf_) {
      if (do_pf_iso_03_) {
        dest.set_dr03_pfiso_charged_all((*charged_all_03)[pf_base_ref]);
        dest.set_dr03_pfiso_charged((*charged_03)[pf_base_ref]);
        dest.set_dr03_pfiso_neutral((*neutral_03)[pf_base_ref]);
        dest.set_dr03_pfiso_gamma((*gamma_03)[pf_base_ref]);
        dest.set_dr03_pfiso_pu((*pu_03)[pf_base_ref]);
      }
      if (do_pf_iso_04_) {
        dest.set_dr04_pfiso_charged_all((*charged_all_04)[pf_base_ref]);
        dest.set_dr04_pfiso_charged((*charged_04)[pf_base_ref]);
        dest.set_dr04_pfiso_neutral((*neutral_04)[pf_base_ref]);
        dest.set_dr04_pfiso_gamma((*gamma_04)[pf_base_ref]);
        dest.set_dr04_pfiso_pu((*pu_04)[pf_base_ref]);
      }
    } else {
      if (do_pf_iso_03_) {
        dest.set_dr03_pfiso_charged_all((*charged_all_03)[muon_base_ref]);
        dest.set_dr03_pfiso_charged((*charged_03)[muon_base_ref]);
        dest.set_dr03_pfiso_neutral((*neutral_03)[muon_base_ref]);
        dest.set_dr03_pfiso_gamma((*gamma_03)[muon_base_ref]);
        dest.set_dr03_pfiso_pu((*pu_03)[muon_base_ref]);
      }
      if (do_pf_iso_04_) {
        dest.set_dr04_pfiso_charged_all((*charged_all_04)[muon_base_ref]);
        dest.set_dr04_pfiso_charged((*charged_04)[muon_base_ref]);
        dest.set_dr04_pfiso_neutral((*neutral_04)[muon_base_ref]);
        dest.set_dr04_pfiso_gamma((*gamma_04)[muon_base_ref]);
        dest.set_dr04_pfiso_pu((*pu_04)[muon_base_ref]);
      }
    }

    if (do_vertex_ip_ && vertices_handle->size() > 0 &&
        src.innerTrack().isNonnull()) {
        reco::Vertex const& vtx = vertices_handle->at(0);
        dest.set_dz_vertex(src.innerTrack()->dz(vtx.position()));
        dest.set_dxy_vertex(src.innerTrack()->dxy(vtx.position()));
    }
    if (do_beamspot_ip_&& src.innerTrack().isNonnull()) {
      dest.set_dxy_beamspot(src.innerTrack()->dxy(*beamspot_handle));
    }
  }
}

void ICMuonProducer::beginJob() {
  ic::StaticTree::tree_->Branch(branch_.c_str(), &muons_);
}

void ICMuonProducer::endJob() {}

DEFINE_FWK_MODULE(ICMuonProducer);