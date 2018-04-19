#ifndef HiggsTauTau_HTT2016Studies_h
#define HiggsTauTau_HTT2016Studies_h
#include <string>
#include <cstdint>
#include "Math/Vector4D.h"
#include "Math/Vector4Dfwd.h"
#include "RooWorkspace.h"
#include "RooFunctor.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "Core/interface/TreeEvent.h"
#include "Core/interface/ModuleBase.h"
#include "Objects/interface/Candidate.hh"
#include "Objects/interface/CompositeCandidate.hh"
#include "Objects/interface/TriggerObject.hh"
#include "HiggsTauTau/interface/HTTPairGenInfo.h"

namespace ic {

struct CandidateTreeVars {
  float pt = 0.;
  float eta = 0.;
  float phi = 0.;
  float m = 0.;
  int q = 0;

  virtual void SetTree(TTree *t, TString pre, TString post);
  virtual void SetVals(ic::Candidate const* c);
};


struct WeightSetVars {
  std::vector<float> wts_;
  std::vector<std::string> strs_;
  unsigned start_ = 0;
  unsigned end_ = 0;

  virtual void SetTree(TTree *t, unsigned start, unsigned end, TString pre,
                       TString post);
  virtual void SetVals(ic::EventInfo const* info);
};


bool HLTPathCheck(ic::TreeEvent* event, std::string const& label,
                  std::string const& path);

//union ui64 {
//   uint64_t one;
//   int16_t four[4];
// };

std::set<int16_t> GetTriggerTypes(ic::TriggerObject* obj);

bool SortByIsoMT(CompositeCandidate const* c1, CompositeCandidate const* c2);
bool SortMM(CompositeCandidate const* c1, CompositeCandidate const* c2);

void CorrectMETForShift(ic::Met * met, ROOT::Math::PxPyPzEVector const& shift);

double TopQuarkPtWeight(std::vector<GenParticle *> const& parts);

ROOT::Math::PtEtaPhiEVector BuildGenBoson(std::vector<GenParticle *> const& parts);

class WJetsStudy : public ModuleBase {
 private:
  // CLASS_MEMBER(WJetsStudy, ic::channel, channel)
  CLASS_MEMBER(WJetsStudy, fwlite::TFileService*, fs)
  CLASS_MEMBER(WJetsStudy, std::string, genparticle_label)
  CLASS_MEMBER(WJetsStudy, std::string, sample_name)

  TTree * tree_;
  CandidateTreeVars lepton_;
  WeightSetVars scale_wts_;
  WeightSetVars pdf_wts_;
  float mt_1_ = 0;
  float wt_ = 1.0;
  unsigned n_jets_ = 0;
  bool do_lhe_weights_ = true;


 public:
  WJetsStudy(std::string const& name);
  virtual ~WJetsStudy();

  virtual int PreAnalysis();
  virtual int Execute(TreeEvent *event);
  virtual int PostAnalysis();
  virtual void PrintInfo();
};

class SelectMVAMET : public ModuleBase {
 private:
  CLASS_MEMBER(SelectMVAMET, std::string, pairs_label)
  CLASS_MEMBER(SelectMVAMET, std::string, met_label)
  CLASS_MEMBER(SelectMVAMET, std::string, met_target)
  CLASS_MEMBER(SelectMVAMET, std::string, correct_for_lepton1)
  CLASS_MEMBER(SelectMVAMET, std::string, correct_for_lepton2)

  public:
   SelectMVAMET(std::string const& name);
   virtual ~SelectMVAMET();

   virtual int Execute(TreeEvent *event);
};

class TheoryTreeProducer : public ModuleBase {
 private:
  CLASS_MEMBER(TheoryTreeProducer, fwlite::TFileService*, fs)
  TTree *outtree_;

  float pt_h;
  float pt_ditau;
  float pt_taup;
  float pt_taum;
  float pt_vistaup;
  float pt_vistaum;
  int njets;
  float mjj;

 public:
  TheoryTreeProducer(std::string const& name);
  virtual ~TheoryTreeProducer();

  virtual int PreAnalysis();
  virtual int Execute(TreeEvent *event);
  virtual int PostAnalysis();
  virtual void PrintInfo();

};

class ZmmTreeProducer : public ModuleBase {
 private:
  CLASS_MEMBER(ZmmTreeProducer, fwlite::TFileService*, fs)
  CLASS_MEMBER(ZmmTreeProducer, std::string, sf_workspace)
  CLASS_MEMBER(ZmmTreeProducer, bool, do_zpt_reweighting)
  CLASS_MEMBER(ZmmTreeProducer, bool, do_top_reweighting)
  TTree *outtree_;
  std::shared_ptr<RooWorkspace> ws_;
  std::map<std::string, std::shared_ptr<RooFunctor>> fns_;

  float wt;
  float wt_pu;
  float wt_trk;
  float wt_id;
  float wt_iso;
  float wt_trg;
  float wt_zpt;
  float wt_top;

  int n_vtx;

  bool os;

  float mvamet_et;

  float pt_1;
  float eta_1;
  float phi_1;
  float iso_1;
  int gen_1;

  float pt_2;
  float eta_2;
  float phi_2;
  float iso_2;
  int gen_2;

  float m_ll;
  float pt_ll;
  float dr_ll;

  bool trg_IsoMu22;
  bool trg_IsoTkMu22;

  bool e_veto;
  bool m_veto;

  int n_jets;
  int n_bjets;

 public:
  ZmmTreeProducer(std::string const& name);
  virtual ~ZmmTreeProducer();

  virtual int PreAnalysis();
  virtual int Execute(TreeEvent *event);
  virtual int PostAnalysis();
  virtual void PrintInfo();

};

class ZeeTreeProducer : public ModuleBase {
 private:
  CLASS_MEMBER(ZeeTreeProducer, fwlite::TFileService*, fs)
  CLASS_MEMBER(ZeeTreeProducer, std::string, sf_workspace)
  TTree *outtree_;
  std::shared_ptr<RooWorkspace> ws_;
  std::map<std::string, std::shared_ptr<RooFunctor>> fns_;

  float wt;
  float wt_trk;
  float wt_pu;
  float wt_id;
  float wt_iso;
  float wt_trg;

  int n_vtx;

  bool os;

  float pt_1;
  float eta_1;
  float phi_1;
  float iso_1;

  float pt_2;
  float eta_2;
  float phi_2;
  float iso_2;

  float m_ll;
  float pt_ll;
  float dr_ll;

  bool trg_Ele25eta2p1WPTight;

 public:
  ZeeTreeProducer(std::string const& name);
  virtual ~ZeeTreeProducer();

  virtual int PreAnalysis();
  virtual int Execute(TreeEvent *event);
  virtual int PostAnalysis();
  virtual void PrintInfo();

};

class ZmmTPTreeProducer : public ModuleBase {
 private:
  CLASS_MEMBER(ZmmTPTreeProducer, fwlite::TFileService*, fs)
  CLASS_MEMBER(ZmmTPTreeProducer, std::string, sf_workspace)
  TTree *outtree_;
  std::shared_ptr<RooWorkspace> ws_;

  float wt;

  int n_vtx;
  int run;

  float pt_t;
  float eta_t;
  float phi_t;
  bool id_t;
  float iso_t;

  bool muon_p;
  bool trk_p;
  float pt_p;
  float eta_p;
  float phi_p;
  bool id_p;
  float iso_p;

  float m_ll;

  bool trg_t_IsoMu22;
  bool trg_t_IsoMu19Tau; // Did something fire the L1 tau?
  bool trg_p_IsoMu22;
  bool trg_p_IsoTkMu22;
  bool trg_p_PFTau120;
  bool trg_p_IsoMu19TauL1;
  bool trg_p_IsoMu19Tau;

 public:
  ZmmTPTreeProducer(std::string const& name);
  virtual ~ZmmTPTreeProducer();

  virtual int PreAnalysis();
  virtual int Execute(TreeEvent *event);
  virtual int PostAnalysis();
  virtual void PrintInfo();

};

class ZeeTPTreeProducer : public ModuleBase {
 private:
  CLASS_MEMBER(ZeeTPTreeProducer, fwlite::TFileService*, fs)
  CLASS_MEMBER(ZeeTPTreeProducer, std::string, sf_workspace)
  TTree *outtree_;
  std::shared_ptr<RooWorkspace> ws_;

  float wt;

  int n_vtx;
  int run;

  float pt_t;
  float eta_t;
  float phi_t;
  bool id_t;
  float iso_t;

  float pt_p;
  float eta_p;
  float sc_eta_p;
  float phi_p;
  bool id_p;
  float iso_p;

  float m_ll;

  bool trg_t_Ele25eta2p1WPTight;
  bool trg_t_Ele24Tau; // Did something fire the L1 tau?
  bool trg_p_Ele25eta2p1WPTight;
  bool trg_p_PFTau120;
  bool trg_p_Ele24TauL1;
  bool trg_p_Ele24Tau;

 public:
  ZeeTPTreeProducer(std::string const& name);
  virtual ~ZeeTPTreeProducer();

  virtual int PreAnalysis();
  virtual int Execute(TreeEvent *event);
  virtual int PostAnalysis();
  virtual void PrintInfo();

};

class ZmtTPTreeProducer : public ModuleBase {
 private:
  CLASS_MEMBER(ZmtTPTreeProducer, fwlite::TFileService*, fs)
  CLASS_MEMBER(ZmtTPTreeProducer, std::string, sf_workspace)
  CLASS_MEMBER(ZmtTPTreeProducer, bool, do_zpt_reweighting)
  CLASS_MEMBER(ZmtTPTreeProducer, bool, do_top_reweighting)
  TTree *outtree_;
  std::shared_ptr<RooWorkspace> ws_;
  std::map<std::string, std::shared_ptr<RooFunctor>> fns_;

  float wt;
  float wt_pu_hi;
  float wt_mfr_l;
  float wt_mfr_t;
  float wt_zpt;
  float wt_top;
  float wt_tauid;

  int n_vtx;
  float rho;

  float pt_m;
  float eta_m;
  float mt_m;
  float pzeta;

  float pt_t;
  float eta_t;
  int dm_t;
  int tot_ch_t;
  bool anti_e_vl_t;
  bool anti_e_t_t;
  bool anti_m_t_t;
  bool anti_m_l_t;
  bool mva_vl_t;
  bool mva_l_t;
  bool mva_m_t;
  bool mva_t_t;
  bool mva_vt_t;
  bool mva_vvt_t;
  bool cmb_l_t;
  bool cmb_m_t;
  bool cmb_t_t;
  float cbiso_t;
  float cbiso_0p5_t;
  float cbiso_1p0_t;
  float cbiso_1p5_t;
  float cbiso_2p0_t;
  float chiso_t;
  float ntiso_t;
  float ntiso_0p5_t;
  float ntiso_1p0_t;
  float ntiso_1p5_t;
  float ntiso_2p0_t;
  float puiso_t;
  float pho_out_t;
  float pho_out_0p0_t;
  float pho_out_0p5_t;
  float pho_out_1p0_t;
  float pho_out_1p5_t;
  float pho_out_2p0_t;

  float nt_density_0p0_0p1;
  float nt_density_0p1_0p2;
  float nt_density_0p2_0p3;
  float nt_density_0p3_0p4;
  float nt_density_0p4_0p5;

  float po_density_0p0_0p1;
  float po_density_0p1_0p2;
  float po_density_0p2_0p3;
  float po_density_0p3_0p4;
  float po_density_0p4_0p5;

  unsigned n_iso_ph_0p5;
  unsigned n_sig_ph_0p5;
  unsigned n_iso_ph_1p0;
  unsigned n_sig_ph_1p0;
  unsigned n_iso_ph_1p5;
  unsigned n_sig_ph_1p5;
  unsigned n_iso_ph_2p0;
  unsigned n_sig_ph_2p0;

  bool trg_m_IsoMu19TauL1;
  bool trg_t_IsoMu19TauL1;
  bool trg_m_IsoMu19Tau;
  bool trg_t_IsoMu19Tau;

  unsigned gen_1;
  unsigned gen_2;

  bool e_veto;
  bool m_veto;

  int n_bjets;
  int n_uncorr_bjets;

  bool os;
  float m_ll;

 public:
  ZmtTPTreeProducer(std::string const& name);
  virtual ~ZmtTPTreeProducer();

  virtual int PreAnalysis();
  virtual int Execute(TreeEvent *event);
  virtual int PostAnalysis();
};

}


#endif
