#ifndef HiggsTauTau_Phys14Plots_h
#define HiggsTauTau_Phys14Plots_h

#include "Core/interface/TreeEvent.h"
#include "Core/interface/ModuleBase.h"
#include "UserCode/ICHiggsTauTau/interface/PFCandidate.hh"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "UserCode/ICHiggsTauTau/interface/Track.hh"
#include "UserCode/ICHiggsTauTau/interface/Vertex.hh"
#include "TH1F.h"
#include "TH2F.h"

#include <string>

namespace ic {

struct EfficiencyPlot1D {
  TH1F* all;
  TH1F* pass;

  EfficiencyPlot1D() {
    all = nullptr;
    pass = nullptr;
  }

  void Fill(double val, bool passes) {
    all->Fill(val);
    if (passes) pass->Fill(val);
  }

  EfficiencyPlot1D(TFileDirectory *dir, std::string const &name, unsigned nbins,
                   double lo, double hi) {
    all = dir->make<TH1F>((name + "_all").c_str(), "", nbins, lo, hi);
    pass = dir->make<TH1F>((name + "_pass").c_str(), "", nbins, lo, hi);
  }
};

struct PFMatchPlot {
  TH1F* all;
  TH1F* hadron;
  TH1F* electron;
  TH1F* muon;
  TH1F* photon;
  TH1F* none;

  PFMatchPlot() {
    all = nullptr;
    hadron = nullptr;
    electron = nullptr;
    muon = nullptr;
    photon = nullptr;
    none = nullptr;
  }

  void Fill(double val, PFType type) {
    all->Fill(val);
    if (type == PFType::h) {
      hadron->Fill(val);
    } else if (type == PFType::e) {
      electron->Fill(val);
    } else if (type == PFType::mu) {
      muon->Fill(val);
    } else if (type == PFType::gamma) {
      photon->Fill(val);
    } else {
      none->Fill(val);
    }
  }

  PFMatchPlot(TFileDirectory *dir, std::string const &name, unsigned nbins,
                   double lo, double hi) {
    all = dir->make<TH1F>((name + "_all").c_str(), "", nbins, lo, hi);
    hadron = dir->make<TH1F>((name + "_hadron").c_str(), "", nbins, lo, hi);
    electron = dir->make<TH1F>((name + "_electron").c_str(), "", nbins, lo, hi);
    muon = dir->make<TH1F>((name + "_muon").c_str(), "", nbins, lo, hi);
    photon = dir->make<TH1F>((name + "_photon").c_str(), "", nbins, lo, hi);
    none = dir->make<TH1F>((name + "_none").c_str(), "", nbins, lo, hi);
  }
};

class Phys14Plots : public ModuleBase {
 private:

  double th_pt_acc;
  double th_eta_acc;

  std::string jets_label_;
  TFileDirectory * dir_;

  TH1F *h_gen_h_pt;
  TH1F *h_gen_h_eta;
  TH1F *h_gen_h_phi;
  TH1F *h_gen_h_mass;

  TH1F *h_gen_th_pt;
  TH1F *h_gen_th_eta;
  TH1F *h_gen_th_mode;

  TH1F *h_n_vtx;
  TH1F *h_n_it_pu;
  TH1F *h_n_ot_pu;

  // tau_h reco+decay mode efficiency
  EfficiencyPlot1D th_mt_eff_vs_pt;
  EfficiencyPlot1D th_rf_mt_eff_vs_pt;
  EfficiencyPlot1D th_dm_rf_eff_vs_pt;

  EfficiencyPlot1D th_dm_eff_vs_pt;
  EfficiencyPlot1D th_dm_eff_vs_eta;
  EfficiencyPlot1D th_dm_eff_vs_nvtx;
  EfficiencyPlot1D th_dm_eff_vs_it_pu;
  EfficiencyPlot1D th_dm_eff_vs_ot_pu;

  EfficiencyPlot1D th0_mt_eff_vs_pt;
  EfficiencyPlot1D th0_rf_mt_eff_vs_pt;
  EfficiencyPlot1D th0_dm_rf_eff_vs_pt;

  EfficiencyPlot1D th0_dm_eff_vs_pt;
  EfficiencyPlot1D th0_dm_eff_vs_eta;
  EfficiencyPlot1D th0_dm_eff_vs_nvtx;
  EfficiencyPlot1D th0_dm_eff_vs_it_pu;
  EfficiencyPlot1D th0_dm_eff_vs_ot_pu;

  EfficiencyPlot1D th1_mt_eff_vs_pt;
  EfficiencyPlot1D th1_rf_mt_eff_vs_pt;
  EfficiencyPlot1D th1_dm_rf_eff_vs_pt;

  EfficiencyPlot1D th1_mt_eff_vs_pt_pi;
  EfficiencyPlot1D th1_jet_eff_vs_pt;
  EfficiencyPlot1D th1_mt_eff_after_jet_vs_pt;
  EfficiencyPlot1D th1_mt_eff_after_jet_pi15_vs_pt;
  EfficiencyPlot1D th1_mt_eff_after_jet_pi20_vs_pt;

  TH1F *th1_jet_ch_had_frac;
  TH1F *th1_jet_nt_had_frac;
  TH1F *th1_jet_photon_frac;
  TH1F *th1_jet_elec_frac;
  TH1F *th1_jet_muon_frac;
  TH1F *th1_jet_tot_frac;

  EfficiencyPlot1D th1_dm_eff_vs_pt;
  EfficiencyPlot1D th1_dm_eff_vs_eta;
  EfficiencyPlot1D th1_dm_eff_vs_nvtx;
  EfficiencyPlot1D th1_dm_eff_vs_it_pu;
  EfficiencyPlot1D th1_dm_eff_vs_ot_pu;

  EfficiencyPlot1D th10_mt_eff_vs_pt;
  EfficiencyPlot1D th10_rf_mt_eff_vs_pt;
  EfficiencyPlot1D th10_dm_rf_eff_vs_pt;

  EfficiencyPlot1D th10_dm_eff_vs_pt;
  EfficiencyPlot1D th10_dm_eff_vs_eta;
  EfficiencyPlot1D th10_dm_eff_vs_nvtx;
  EfficiencyPlot1D th10_dm_eff_vs_it_pu;
  EfficiencyPlot1D th10_dm_eff_vs_ot_pu;

  TH2F *h_th_mode_table;
  TH2F *h_th_mode_table_gen_den;
  TH2F *h_th_mode_table_gen_den_rec_fid;

  TH1F *h_th_pt_resp;
  TH1F *h_th0_pt_resp;
  TH1F *h_th1_pt_resp;
  TH1F *h_th10_pt_resp;

  PFMatchPlot th_pf_match_pt;
  PFMatchPlot th_pf_match_eta;
  PFMatchPlot th0_pf_match_pt;
  PFMatchPlot th0_pf_match_eta;
  PFMatchPlot th1_pf_match_pt;
  PFMatchPlot th1_pf_match_eta;
  PFMatchPlot th10_pf_match_pt;
  PFMatchPlot th10_pf_match_eta;

  CLASS_MEMBER(Phys14Plots, fwlite::TFileService*, fs)

 public:
  Phys14Plots(std::string const& name);
  virtual ~Phys14Plots();

  virtual int PreAnalysis();
  virtual int Execute(TreeEvent *event);
  virtual int PostAnalysis();
  virtual void PrintInfo();
  bool QualityTrack(Track const* trk, Vertex const* vtx);
};

}

#endif