#include <algorithm>
#include <map>
#include "TMath.h"
#include "boost/functional/hash.hpp"
#include "boost/lexical_cast.hpp"
#include "RooRealVar.h"
#include "HiggsTauTau/interface/HTT2016Studies.h"
#include "UserCode/ICHiggsTauTau/interface/PFJet.hh"
#include "UserCode/ICHiggsTauTau/interface/PFCandidate.hh"
#include "UserCode/ICHiggsTauTau/interface/SecondaryVertex.hh"
#include "UserCode/ICHiggsTauTau/interface/Track.hh"
#include "UserCode/ICHiggsTauTau/interface/Met.hh"
#include "UserCode/ICHiggsTauTau/interface/EventInfo.hh"
#include "Utilities/interface/FnPredicates.h"
#include "Utilities/interface/FnPairs.h"
#include "Utilities/interface/FnRootTools.h"
#include "HiggsTauTau/interface/HTTPairGenInfo.h"

namespace ic {


  void CandidateTreeVars::SetTree(TTree *t, TString pre, TString post) {
    t->Branch(pre+"pt"+post, &pt);
    t->Branch(pre+"eta"+post, &eta);
    t->Branch(pre+"phi"+post, &phi);
    t->Branch(pre+"m"+post, &m);
    t->Branch(pre+"q"+post, &q);
  }

  void CandidateTreeVars::SetVals(ic::Candidate const* c) {
    pt = c->pt();
    eta = c->eta();
    phi = c->phi();
    m = c->M();
    q = c->charge();
  }

  void WeightSetVars::SetTree(TTree *t, unsigned start, unsigned end, TString pre,
                       TString post) {
    if (start > end) return;
    start_ = start;
    end_ = end;
    wts_.resize((end_ - start_) + 1);
    strs_.resize((end_ - start_) + 1);
    for (unsigned i = 0; i < wts_.size(); ++i) {
      t->Branch(pre+boost::lexical_cast<std::string>(i)+post, &(wts_[i]));
      strs_[i] = boost::lexical_cast<std::string>(i+start_);
    };
  }

  void WeightSetVars::SetVals(ic::EventInfo const* info) {
    for (unsigned i = 0; i < wts_.size(); ++i) {
      wts_[i] = info->weight(strs_[i]);
    }
  }

  WJetsStudy::WJetsStudy(std::string const& name)
      : ModuleBase(name), fs_(nullptr), genparticle_label_("genParticles") {}

  WJetsStudy::~WJetsStudy() { ; }

  int WJetsStudy::PreAnalysis() {
    if (fs_) {
      tree_ = fs_->make<TTree>("wjetsstudy", "wjetsstudy");
      lepton_.SetTree(tree_, "", "_1");
      if (sample_name_ == "WJetsToLNuNLO") {
        scale_wts_.SetTree(tree_, 1001, 1009, "scale_wt_", "");
        pdf_wts_.SetTree(tree_, 2001, 2102, "pdf_wt_", ""); // 2101 and 2102 are alpha_s
      } else if (sample_name_ == "WJetsToLNuLO") {
        scale_wts_.SetTree(tree_, 1, 9, "scale_wt_", "");
        pdf_wts_.SetTree(tree_, 11, 110, "pdf_wt_", ""); // no alpha_s in LO sample
      } else {
        std::cout << "[WJetsStudy] Warning, sample_name was not recognised, LHE weights will not be saved\n";
        do_lhe_weights_ = false;
      }
      tree_->Branch("mt_1", &mt_1_);
      tree_->Branch("wt", &wt_);
      tree_->Branch("n_jets", &n_jets_);
    }
    return 0;
  }

  int WJetsStudy::Execute(TreeEvent* event) {
    auto muons = event->GetPtrVec<ic::Muon>("muons");
    auto sel_muons = ic::copy_keep_if(muons, [](ic::Muon *m) {
      return
        m->pt()                 > 19.0    &&
        fabs(m->eta())          < 2.1     &&
        fabs(m->dxy_vertex())   < 0.045   &&
        fabs(m->dz_vertex())    < 0.2     &&
        MuonMedium(m);
    });

    if (sel_muons.size() == 0) return 1;

    auto pfmet = event->GetPtrVec<ic::Met>("pfMet");
    // auto genparticles = event->GetPtrVec<ic::GenParticle>(genparticle_label_);

    // auto gen_muon = ic::copy_keep_if(genparticles, [](ic::GenParticle* p) {
    //   return (std::abs(p->pdgid()) == 13) &&
    //          p->statusFlags()[ic::FromHardProcess] && p->status() == 1;
    // });
    // auto gen_nu = ic::copy_keep_if(genparticles, [](ic::GenParticle* p) {
    //   return (std::abs(p->pdgid()) == 14) &&
    //          p->statusFlags()[ic::FromHardProcess] && p->status() == 1;
    // });

    // if (gen_muon.size() == 0) return 1;
    // if (gen_muon.size() > 1) {
    //   std::cout << "More than one muon!\n";
    //   return 1;
    // }
    // if (gen_nu.size() != 1) {
    //   std::cout << "No muon neutrino!\n";
    //   return 1;
    // }

    lepton_.SetVals(sel_muons[0]);
    mt_1_ = ic::MT(sel_muons[0], pfmet[0]);

    auto info = event->GetPtr<ic::EventInfo>("eventInfo");
    wt_ = info->total_weight();

    if (do_lhe_weights_) {
      scale_wts_.SetVals(info);
      pdf_wts_.SetVals(info);
    }

    auto jets = event->GetPtrVec<ic::PFJet>("ak4PFJetsCHS");

    auto sel_jets = ic::copy_keep_if(jets, [&](ic::PFJet *p) {
      return
        p->pt()           > 20.0    &&
        fabs(p->eta())    < 2.4     &&
        ic::PFJetID2015(p)          &&
        ic::MinDRToCollection(p, sel_muons, 0.5);
    });

    n_jets_ = sel_jets.size();
    /*
    0 muR=1.0 muF=1.0 <-- just the nominal
    1 muR=1.0 muF=2.0
    2 muR=1.0 muF=0.5
    3 muR=2.0 muF=1.0
    4 muR=2.0 muF=2.0
    5 muR=2.0 muF=0.5 <-- not this one
    6 muR=0.5 muF=1.0
    7 muR=0.5 muF=2.0 <-- not this one
    8 muR=0.5 muF=0.5
    */

    tree_->Fill();
    return 0;
  }
  int WJetsStudy::PostAnalysis() {
    std::cout << "*** " << this->ModuleName() << "\n";
    return 0;
  }

  void WJetsStudy::PrintInfo() {}


  SelectMVAMET::SelectMVAMET(std::string const& name)
      : ModuleBase(name),
        pairs_label_("ditau"),
        met_label_("pfMVAMetVector"),
        met_target_("mvamet"),
        correct_for_lepton1_(""),
        correct_for_lepton2_("") {};

  SelectMVAMET::~SelectMVAMET() { ; }

  int SelectMVAMET::Execute(TreeEvent *event) {
    // Get the MVA MET
    Met *mvamet = nullptr;
    auto pairs = event->GetPtrVec<CompositeCandidate>(pairs_label_);
    auto const& met_map = event->GetIDMap<Met>(met_label_);
    std::size_t id = 0;
    boost::hash_combine(id, pairs.at(0)->at(0)->id());
    boost::hash_combine(id, pairs.at(0)->at(1)->id());
    std::size_t id_inv = 0;
    boost::hash_combine(id_inv, pairs.at(0)->at(1)->id());
    boost::hash_combine(id_inv, pairs.at(0)->at(0)->id());
    auto it = met_map.find(id);
    auto it_inv = met_map.find(id_inv);
    if (it != met_map.end()) {
      mvamet = it->second;
      // event->Add("pfMVAMet", mva_met);
    } else if (it_inv != met_map.end()){
      //lepton1 <--> lepton2 (needed for fully hadronic)
      mvamet = it_inv->second;
      // event->Add("pfMVAMet", mva_met);
    } else {
      throw::std::runtime_error("Could not find MVA MET!");
    }

    typedef std::map<std::size_t, ROOT::Math::PxPyPzEVector> map_id_vec;
    if (correct_for_lepton1_ != "") {
      auto const& es_shifts1 = event->Get<map_id_vec>(correct_for_lepton1_);
      CorrectMETForShift(mvamet, es_shifts1.at(pairs.at(0)->at(0)->id()));
    }
    if (correct_for_lepton2_ != "") {
      auto const& es_shifts2 = event->Get<map_id_vec>(correct_for_lepton2_);
      CorrectMETForShift(mvamet, es_shifts2.at(pairs.at(0)->at(1)->id()));
    }

    event->Add(met_target_, mvamet);
    return 0;
  }


  TheoryTreeProducer::TheoryTreeProducer(std::string const& name)
      : ModuleBase(name) {
      fs_ = nullptr;
  }

  TheoryTreeProducer::~TheoryTreeProducer() {
    ;
  }

  int TheoryTreeProducer::PreAnalysis() {
    if (fs_) {
      outtree_ = fs_->make<TTree>("Theory","Theory");
      outtree_->Branch("pt_h",         &pt_h);
      outtree_->Branch("pt_ditau",     &pt_ditau);
      outtree_->Branch("pt_taup",      &pt_taup);
      outtree_->Branch("pt_taum",      &pt_taum);
      outtree_->Branch("pt_vistaup",   &pt_vistaup);
      outtree_->Branch("njets", &njets);
      outtree_->Branch("mjj",   &mjj);
    }
    return 0;
  }

  int TheoryTreeProducer::Execute(TreeEvent *event) {

    auto parts = event->GetPtrVec<GenParticle>("genParticles");
    ic::GenParticle *last_h = nullptr;
    ic::GenParticle *last_taup = nullptr;
    ic::GenParticle *last_taum = nullptr;



    for (auto p : parts) {
      auto const& flags = p->statusFlags();
      if (std::abs(p->pdgid()) == 25 && flags[IsLastCopy]) {
        if (!last_h) {
          last_h = p;
        } else {
          std::cout << ">> Found duplicate last Higgs\n";
        }
      }
      // p->Print();
      // std::cout << "  isLastCopy: " << p->statusFlags()[GenStatusBits::IsLastCopy] << "\n";
    }
    auto h_daughters = ExtractDaughtersRecursive(last_h, parts);
    for (auto p : h_daughters) {
      auto const& flags = p->statusFlags();
      if ((p->pdgid()) == +15 && flags[IsLastCopy]) {
        if (!last_taup) {
          last_taup = p;
        } else {
          std::cout << ">> Found duplicate last taup\n";
        }
      }
      if ((p->pdgid()) == -15 && flags[IsLastCopy]) {
        if (!last_taum) {
          last_taum = p;
        } else {
          std::cout << ">> Found duplicate last taum\n";
        }
      }
    };
    ic::CompositeCandidate ditau;
    ditau.AddCandidate("taup", last_taup);
    ditau.AddCandidate("taum", last_taum);

    auto taup_stable_daughters = ExtractStableDaughters(last_taup, h_daughters);
    auto taum_stable_daughters = ExtractStableDaughters(last_taum, h_daughters);
    ic::erase_if(taup_stable_daughters, [](GenParticle *p) {
      return (abs(p->pdgid()) == 12 || abs(p->pdgid()) == 14 || abs(p->pdgid()) == 16);
    });
    ic::erase_if(taum_stable_daughters, [](GenParticle *p) {
      return (abs(p->pdgid()) == 12 || abs(p->pdgid()) == 14 || abs(p->pdgid()) == 16);
    });
    ic::CompositeCandidate visp;
    for (auto p : taup_stable_daughters) {
      visp.AddCandidate(boost::lexical_cast<std::string>(p->index()), p);
    }
    ic::CompositeCandidate vism;
    for (auto p : taum_stable_daughters) {
      vism.AddCandidate(boost::lexical_cast<std::string>(p->index()), p);
    }

    auto genjets = event->GetPtrVec<GenJet>("genJets");
    std::vector<Candidate *> vis_taus{&visp, &vism};
    ic::keep_if(genjets, [&](GenJet *j) {
      return MinPtMaxEta(j, 30., 4.7) && MinDRToCollection(j, vis_taus, 0.5);
    });
    std::sort(genjets.begin(), genjets.end(), [](GenJet *j1, GenJet *j2) {
      return j1->pt() > j2->pt();
    });
    pt_h = last_h->pt();
    pt_ditau = ditau.pt();
    pt_taup = last_taup->pt();
    pt_taum = last_taum->pt();
    pt_vistaup = visp.pt();
    pt_vistaum = vism.pt();

    njets = genjets.size();
    if (njets >= 2) {
      mjj = (genjets[0]->vector() + genjets[1]->vector()).M();
    } else {
      mjj = 0.;
    }
    outtree_->Fill();
    return 0;
  }

  int TheoryTreeProducer::PostAnalysis() {
    return 0;
  }

  void TheoryTreeProducer::PrintInfo() { }


  ZmmTreeProducer::ZmmTreeProducer(std::string const& name)
      : ModuleBase(name),
        do_zpt_reweighting_(false),
        do_top_reweighting_(false) {
      fs_ = nullptr;
  }

  ZmmTreeProducer::~ZmmTreeProducer() {
    ;
  }

  int ZmmTreeProducer::PreAnalysis() {
    if (fs_) {
      outtree_ = fs_->make<TTree>("Zmm","Zmm");

      outtree_->Branch("wt",          &wt);
      outtree_->Branch("wt_pu",       &wt_pu);
      outtree_->Branch("wt_trk",      &wt_trk);
      outtree_->Branch("wt_id",       &wt_id);
      outtree_->Branch("wt_iso",      &wt_iso);
      outtree_->Branch("wt_trg",      &wt_trg);
      outtree_->Branch("wt_zpt",      &wt_zpt);
      outtree_->Branch("wt_top",      &wt_top);
      outtree_->Branch("n_vtx",       &n_vtx);
      outtree_->Branch("os",          &os);
      outtree_->Branch("pt_1",        &pt_1);
      outtree_->Branch("eta_1",       &eta_1);
      outtree_->Branch("phi_1",       &phi_1);
      outtree_->Branch("iso_1",       &iso_1);
      outtree_->Branch("gen_1",       &gen_1);
      outtree_->Branch("pt_2",        &pt_2);
      outtree_->Branch("eta_2",       &eta_2);
      outtree_->Branch("phi_2",       &phi_2);
      outtree_->Branch("iso_2",       &iso_2);
      outtree_->Branch("gen_2",       &gen_2);
      outtree_->Branch("m_ll",        &m_ll);
      outtree_->Branch("pt_ll",       &pt_ll);
      outtree_->Branch("dr_ll",       &dr_ll);
      outtree_->Branch("mvamet_et",   &mvamet_et);
      outtree_->Branch("trg_IsoMu22",       &trg_IsoMu22);
      outtree_->Branch("trg_IsoTkMu22",     &trg_IsoTkMu22);
      outtree_->Branch("e_veto",       &e_veto);
      outtree_->Branch("m_veto",       &m_veto);
      outtree_->Branch("n_jets",       &n_jets);
      outtree_->Branch("n_bjets",      &n_bjets);
    }

    TFile f(sf_workspace_.c_str());
    ws_ = std::shared_ptr<RooWorkspace>((RooWorkspace*)gDirectory->Get("w"));
    f.Close();
    fns_["m_trk_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_trk_ratio")->functor(ws_->argSet("m_eta")));
    fns_["m_id_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_id_ratio")->functor(ws_->argSet("m_pt,m_eta")));
    fns_["m_iso_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_iso_ratio")->functor(ws_->argSet("m_pt,m_eta")));
    fns_["m_trgOR_data"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_trgOR_data")->functor(ws_->argSet("m_pt,m_eta")));
    fns_["m_idiso0p15_desy_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_idiso0p15_desy_ratio")->functor(ws_->argSet("m_pt,m_eta")));
    fns_["m_trgIsoMu22orTkIsoMu22_desy_data"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_trgIsoMu22orTkIsoMu22_desy_data")->functor(ws_->argSet("m_pt,m_eta")));
    fns_["zpt_weight"] = std::shared_ptr<RooFunctor>(
      ws_->function("zpt_weight")->functor(ws_->argSet("z_gen_mass,z_gen_pt")));
    return 0;
  }

  int ZmmTreeProducer::Execute(TreeEvent *event) {
    typedef std::vector<double> v_double;
    auto pairs = event->GetPtrVec<CompositeCandidate>("dimuon");
    auto info = event->GetPtr<EventInfo>("eventInfo");

    // // Take the pair closest to the Z mass
    // std::sort(pairs.begin(), pairs.end(), [=](CompositeCandidate *c1, CompositeCandidate *c2) {
    //   return std::fabs(c1->M() - 91.1876) < std::fabs(c2->M() - 91.1876);
    // });
    auto const* pair = pairs.at(0);

    // Sort the pair by descending pT
    if (pair->at(0)->pt() < pair->at(1)->pt()) {
      std::cout << "Something has gone wrong here!\n";
    }
    // std::vector<Candidate *> leptons = pair->AsVector();
    // std::sort(leptons.begin(), leptons.end(),
    //     bind(&Candidate::pt, _1) > bind(&Candidate::pt, _2));

    Muon const* lep_1 = dynamic_cast<Muon const*>(pair->at(0));
    Muon const* lep_2 = dynamic_cast<Muon const*>(pair->at(1));

    gen_1 = 0;
    gen_2 = 0;
    if (!info->is_data()) {
      gen_1 = MCOrigin2UInt(event->Get<ic::mcorigin>("gen_match_1"));
      gen_2 = MCOrigin2UInt(event->Get<ic::mcorigin>("gen_match_2"));
    }

    // Get the MVA MET so we can calculate MT
    Met *mva_met = nullptr;
    auto const& met_map = event->GetIDMap<Met>("pfMVAMetVector");
    std::size_t id = 0;
    boost::hash_combine(id, lep_1->id());
    boost::hash_combine(id, lep_2->id());
    std::size_t id_inv = 0;
    boost::hash_combine(id_inv, lep_2->id());
    boost::hash_combine(id_inv, lep_1->id());
    auto it = met_map.find(id);
    auto it_inv = met_map.find(id_inv);
    if (it != met_map.end()) {
      mva_met = it->second;
    } else if (it_inv != met_map.end()){
      mva_met = it_inv->second;
    } else {
      throw::std::runtime_error("Could not find MVA MET!");
    };

    typedef std::map<std::size_t, ROOT::Math::PxPyPzEVector> map_id_vec;
    auto const& mu_es_shifts = event->Get<map_id_vec>("shifts_MuonEnergyScale");
    CorrectMETForShift(mva_met, mu_es_shifts.at(lep_1->id()));
    CorrectMETForShift(mva_met, mu_es_shifts.at(lep_2->id()));

    mvamet_et = mva_met->pt();

    n_vtx = info->good_vertices();

    // Fill tree info
    os = (pair->charge() == 0);
    pt_1 = lep_1->pt();
    eta_1 = lep_1->eta();
    phi_1 = lep_1->phi();
    iso_1 = PF04IsolationVal(lep_1, 0.5, 0);

    pt_2 = lep_2->pt();
    eta_2 = lep_2->eta();
    phi_2 = lep_2->phi();
    iso_2 = PF04IsolationVal(lep_2, 0.5, 0);

    m_ll = pair->M();
    pt_ll = pair->pt();
    dr_ll = ROOT::Math::VectorUtil::DeltaR(lep_1->vector(), lep_2->vector());

    trg_IsoMu22 = false;
    trg_IsoTkMu22 = false;

    if (info->is_data()) {
      bool path_fired_IsoMu22 = HLTPathCheck(event, "triggerPaths", "HLT_IsoMu22_v");
      bool path_fired_IsoTkMu22 = HLTPathCheck(event, "triggerPaths", "HLT_IsoTkMu22_v");

      std::string filter_IsoMu22 = "hltL3crIsoL1sMu20L1f0L2f10QL3f22QL3trkIsoFiltered0p09";
      auto const& trg_objs_IsoMu22 = event->GetPtrVec<TriggerObject>("triggerObjects_IsoMu22");
      trg_IsoMu22 =
          path_fired_IsoMu22 &&
          IsFilterMatched(lep_1, trg_objs_IsoMu22, filter_IsoMu22, 0.5);

      std::string filter_IsoTkMu22 = "hltL3fL1sMu20L1f0Tkf22QL3trkIsoFiltered0p09";
      auto const& trg_objs_IsoTkMu22 = event->GetPtrVec<TriggerObject>("triggerObjects_IsoTkMu22");
      trg_IsoTkMu22 =
          path_fired_IsoTkMu22 &&
          IsFilterMatched(lep_1, trg_objs_IsoTkMu22, filter_IsoTkMu22, 0.5);
    } else {
      trg_IsoMu22 = true;
      trg_IsoTkMu22 = true;
    }

    wt_trk = 1.0;
    wt_id = 1.0;
    wt_iso = 1.0;
    wt_trg = 1.0;
    auto args_1 = std::vector<double>{lep_1->pt(), lep_1->eta()};
    auto args_2 = std::vector<double>{lep_2->pt(), lep_2->eta()};
    if (!info->is_data()) {
      float eff_1 = fns_["m_trgIsoMu22orTkIsoMu22_desy_data"]->eval(args_1.data());
      // float eff_2 = fns_["m_trg_data"]->eval(args_2.data());
      // wt_trg = eff_1 + eff_2 - eff_1 * eff_2;
      wt_trg = eff_1;
      wt_trk = fns_["m_trk_ratio"]->eval(v_double{lep_1->eta()}.data()) *
              fns_["m_trk_ratio"]->eval(v_double{lep_2->eta()}.data());
      wt_id = fns_["m_idiso0p15_desy_ratio"]->eval(args_1.data()) *
              fns_["m_idiso0p15_desy_ratio"]->eval(args_2.data());
      // wt_iso = fns_["m_iso_ratio"]->eval(args_1.data()) *
      //          fns_["m_iso_ratio"]->eval(args_2.data());
    }
    info->set_weight("trk", wt_trk);
    info->set_weight("trg", wt_trg);
    info->set_weight("id", wt_id);
    info->set_weight("iso", wt_iso);
    wt = info->total_weight();
    wt_pu = info->weight_defined("pileup") ? info->weight("pileup") : 1.0;

    // DY reweighting
    wt_zpt = 1.0;
    if (do_zpt_reweighting_) {
      auto const& parts = event->GetPtrVec<GenParticle>("genParticles");
      auto gen_boson = BuildGenBoson(parts);
      wt_zpt = fns_["zpt_weight"]->eval(v_double{gen_boson.M(), gen_boson.pt()}.data());
    }

    // Top quark pT reweighting
    wt_top = 1.0;
    if (do_top_reweighting_) {
      auto const& parts = event->GetPtrVec<GenParticle>("genParticles");
      wt_top = TopQuarkPtWeight(parts);
    }

    e_veto = event->Get<bool>("extra_elec_veto");
    m_veto = event->Get<bool>("extra_muon_veto");

    auto jets = event->GetPtrVec<PFJet>("ak4PFJetsCHS");
    ic::keep_if(jets, [&](PFJet * j) {
      return ic::MinDRToCollection(j, pair->AsVector(), 0.5);
    });
    auto bjets = jets;
    ic::keep_if(jets, [&](PFJet *j) {
      return MinPtMaxEta(j, 30., 4.7);
    });
    ic::keep_if(bjets, [&](PFJet* j) {
      return MinPtMaxEta(j, 20., 2.4) &&
             j->GetBDiscriminator(
                 "pfCombinedInclusiveSecondaryVertexV2BJetTags") > 0.8;
    });
    n_jets = jets.size();
    n_bjets = bjets.size();

    outtree_->Fill();
    return 0;
  }

  int ZmmTreeProducer::PostAnalysis() {
    return 0;
  }

  void ZmmTreeProducer::PrintInfo() { }


  ZeeTreeProducer::ZeeTreeProducer(std::string const& name)
      : ModuleBase(name) {
      fs_ = nullptr;
  }

  ZeeTreeProducer::~ZeeTreeProducer() {
    ;
  }

  int ZeeTreeProducer::PreAnalysis() {
    if (fs_) {
      outtree_ = fs_->make<TTree>("Zee","Zee");

      outtree_->Branch("wt",          &wt);
      outtree_->Branch("wt_pu",       &wt_pu);
      outtree_->Branch("wt_trk",      &wt_trk);
      outtree_->Branch("wt_id",       &wt_id);
      outtree_->Branch("wt_iso",      &wt_iso);
      outtree_->Branch("wt_trg",      &wt_trg);
      outtree_->Branch("n_vtx",       &n_vtx);
      outtree_->Branch("os",          &os);
      outtree_->Branch("pt_1",        &pt_1);
      outtree_->Branch("eta_1",       &eta_1);
      outtree_->Branch("phi_1",       &phi_1);
      outtree_->Branch("iso_1",       &iso_1);
      outtree_->Branch("pt_2",        &pt_2);
      outtree_->Branch("eta_2",       &eta_2);
      outtree_->Branch("phi_2",       &phi_2);
      outtree_->Branch("iso_2",       &iso_2);
      outtree_->Branch("m_ll",        &m_ll);
      outtree_->Branch("pt_ll",       &pt_ll);
      outtree_->Branch("dr_ll",       &dr_ll);
      outtree_->Branch("trg_Ele25eta2p1WPTight",       &trg_Ele25eta2p1WPTight);
    }

    TFile f(sf_workspace_.c_str());
    ws_ = std::shared_ptr<RooWorkspace>((RooWorkspace*)gDirectory->Get("w"));
    fns_["e_trk_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("e_trk_ratio")->functor(ws_->argSet("e_pt,e_eta")));
    fns_["e_id_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("e_id_ratio")->functor(ws_->argSet("e_pt,e_eta")));
    fns_["e_iso_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("e_iso_ratio")->functor(ws_->argSet("e_pt,e_eta")));
    fns_["e_trg_data"] = std::shared_ptr<RooFunctor>(
      ws_->function("e_trg_data")->functor(ws_->argSet("e_pt,e_eta")));
    f.Close();
    return 0;
  }

  int ZeeTreeProducer::Execute(TreeEvent *event) {
    auto pairs = event->GetPtrVec<CompositeCandidate>("dielec");
    auto info = event->GetPtr<EventInfo>("eventInfo");

    // Take the pair closest to the Z mass
    std::sort(pairs.begin(), pairs.end(), [=](CompositeCandidate *c1, CompositeCandidate *c2) {
      return std::fabs(c1->M() - 91.1876) < std::fabs(c2->M() - 91.1876);
    });
    auto const* pair = pairs.at(0);

    // Sort the pair by descending pT
    std::vector<Candidate *> leptons = pair->AsVector();
    std::sort(leptons.begin(), leptons.end(),
        bind(&Candidate::pt, _1) > bind(&Candidate::pt, _2));

    Electron const* lep_1 = dynamic_cast<Electron const*>(leptons[0]);
    Electron const* lep_2 = dynamic_cast<Electron const*>(leptons[1]);

    n_vtx = info->good_vertices();

    // Fill tree info
    os = (pair->charge() == 0);
    pt_1 = lep_1->pt();
    eta_1 = lep_1->eta();
    phi_1 = lep_1->phi();
    iso_1 = PF03IsolationVal(lep_1, 0.5, 0);

    pt_2 = lep_2->pt();
    eta_2 = lep_2->eta();
    phi_2 = lep_2->phi();
    iso_2 = PF03IsolationVal(lep_2, 0.5, 0);

    m_ll = pair->M();
    pt_ll = pair->pt();
    dr_ll = ROOT::Math::VectorUtil::DeltaR(lep_1->vector(), lep_2->vector());

    trg_Ele25eta2p1WPTight = false;

    if (info->is_data()) {
      bool path_fired_Ele25eta2p1WPTight = HLTPathCheck(event, "triggerPaths", "HLT_Ele25_eta2p1_WPTight_Gsf_v");

      std::string filter_Ele25eta2p1WPTight = "hltEle25erWPTightGsfTrackIsoFilter";
      auto const& trg_objs_Ele25eta2p1WPTight = event->GetPtrVec<TriggerObject>("triggerObjects_Ele25_eta2p1_WPTight_Gsf");
      trg_Ele25eta2p1WPTight = path_fired_Ele25eta2p1WPTight &&
        (IsFilterMatched(lep_1, trg_objs_Ele25eta2p1WPTight, filter_Ele25eta2p1WPTight, 0.5) ||
         IsFilterMatched(lep_2, trg_objs_Ele25eta2p1WPTight, filter_Ele25eta2p1WPTight, 0.5));
    } else {
      trg_Ele25eta2p1WPTight = true;
    }

    wt_trk = 1.0;
    wt_id = 1.0;
    wt_iso = 1.0;
    wt_trg = 1.0;
    auto args_1 = std::vector<double>{lep_1->pt(), lep_1->sc_eta()};
    auto args_2 = std::vector<double>{lep_2->pt(), lep_2->sc_eta()};
    if (!info->is_data()) {
      float eff_1 = fns_["e_trg_data"]->eval(args_1.data());
      float eff_2 = fns_["e_trg_data"]->eval(args_2.data());
      wt_trg = eff_1 + eff_2 - eff_1 * eff_2;
      wt_trk = fns_["e_trk_ratio"]->eval(args_1.data()) *
               fns_["e_trk_ratio"]->eval(args_2.data());
      wt_id = fns_["e_id_ratio"]->eval(args_1.data()) *
              fns_["e_id_ratio"]->eval(args_2.data());
      wt_iso = fns_["e_iso_ratio"]->eval(args_1.data()) *
               fns_["e_iso_ratio"]->eval(args_2.data());
    }
    info->set_weight("trk", wt_trk);
    info->set_weight("trg", wt_trg);
    info->set_weight("id", wt_id);
    info->set_weight("iso", wt_iso);
    wt = info->total_weight();
    wt_pu = info->weight_defined("pileup") ? info->weight("pileup") : 1.0;

    outtree_->Fill();
    return 0;
  }

  int ZeeTreeProducer::PostAnalysis() {
    return 0;
  }

  void ZeeTreeProducer::PrintInfo() { }



  ZmmTPTreeProducer::ZmmTPTreeProducer(std::string const& name)
      : ModuleBase(name) {
      fs_ = nullptr;
  }

  ZmmTPTreeProducer::~ZmmTPTreeProducer() {
    ;
  }

  int ZmmTPTreeProducer::PreAnalysis() {
    if (fs_) {
      outtree_ = fs_->make<TTree>("ZmmTP","ZmmTP");

      outtree_->Branch("wt",          &wt);
      outtree_->Branch("n_vtx",       &n_vtx);
      outtree_->Branch("run",         &run);
      outtree_->Branch("pt_t",        &pt_t);
      outtree_->Branch("eta_t",       &eta_t);
      outtree_->Branch("phi_t",       &phi_t);
      outtree_->Branch("id_t",        &id_t);
      outtree_->Branch("iso_t",       &iso_t);
      outtree_->Branch("muon_p",      &muon_p);
      outtree_->Branch("trk_p",       &trk_p);
      outtree_->Branch("pt_p",        &pt_p);
      outtree_->Branch("eta_p",       &eta_p);
      outtree_->Branch("id_p",        &id_p);
      outtree_->Branch("iso_p",       &iso_p);
      outtree_->Branch("m_ll",        &m_ll);
      outtree_->Branch("trg_t_IsoMu22",       &trg_t_IsoMu22);
      outtree_->Branch("trg_t_IsoMu19Tau",    &trg_t_IsoMu19Tau);
      outtree_->Branch("trg_p_IsoMu22",       &trg_p_IsoMu22);
      outtree_->Branch("trg_p_IsoTkMu22",     &trg_p_IsoTkMu22);
      outtree_->Branch("trg_p_PFTau120",      &trg_p_PFTau120);
      outtree_->Branch("trg_p_IsoMu19TauL1",  &trg_p_IsoMu19TauL1);
      outtree_->Branch("trg_p_IsoMu19Tau",    &trg_p_IsoMu19Tau);
    }

    TFile f(sf_workspace_.c_str());
    ws_ = std::shared_ptr<RooWorkspace>((RooWorkspace*)gDirectory->Get("w"));
    f.Close();
    return 0;
  }

  int ZmmTPTreeProducer::Execute(TreeEvent *event) {
    auto pairs = event->GetPtrVec<CompositeCandidate>("dimuon");
    auto info = event->GetPtr<EventInfo>("eventInfo");
    run = info->run();

    n_vtx = info->good_vertices();
    wt = info->total_weight();

    for (auto pair : pairs) {
      Muon const* mu_t = dynamic_cast<Muon const*>(pair->At(0));
      Muon const* mu_p = dynamic_cast<Muon const*>(pair->At(1));
      pt_t = mu_t->pt();
      eta_t = mu_t->eta();
      phi_t = mu_t->phi();
      id_t = MuonMediumHIPsafe(mu_t) && fabs(mu_t->dxy_vertex()) < 0.045 && fabs(mu_t->dz_vertex()) < 0.2;
      iso_t = PF04IsolationVal(mu_t, 0.5, 0);
      pt_p = mu_p->pt();
      eta_p = mu_p->eta();
      phi_p = mu_p->phi();
      id_p = MuonMediumHIPsafe(mu_p) && fabs(mu_p->dxy_vertex()) < 0.045 && fabs(mu_p->dz_vertex()) < 0.2;
      iso_p = PF04IsolationVal(mu_p, 0.5, 0);
      m_ll = pair->M();

      trg_t_IsoMu22 = false;
      trg_t_IsoMu19Tau = false;
      trg_p_IsoMu22 = false;
      trg_p_IsoTkMu22 = false;
      trg_p_PFTau120 = false;
      trg_p_IsoMu19TauL1 = false;
      trg_p_IsoMu19Tau = false;
      if (info->is_data()) {
        bool path_fired_IsoMu22 = HLTPathCheck(event, "triggerPaths", "HLT_IsoMu22_v");
        bool path_fired_IsoTkMu22 = HLTPathCheck(event, "triggerPaths", "HLT_IsoTkMu22_v");
        bool path_fired_PFTau120 = HLTPathCheck(event, "triggerPaths", "HLT_VLooseIsoPFTau120_Trk50_eta2p1_v");

        std::string filter_IsoMu22 = "hltL3crIsoL1sMu20L1f0L2f10QL3f22QL3trkIsoFiltered0p09";
        auto const& trg_objs_IsoMu22 = event->GetPtrVec<TriggerObject>("triggerObjects_IsoMu22");

        trg_t_IsoMu22 = path_fired_IsoMu22 && IsFilterMatched(mu_t, trg_objs_IsoMu22, filter_IsoMu22, 0.5);
        trg_p_IsoMu22 = path_fired_IsoMu22 && IsFilterMatched(mu_p, trg_objs_IsoMu22, filter_IsoMu22, 0.5);

        std::string filter_IsoTkMu22 = "hltL3fL1sMu20L1f0Tkf22QL3trkIsoFiltered0p09";
        auto const& trg_objs_IsoTkMu22 = event->GetPtrVec<TriggerObject>("triggerObjects_IsoTkMu22");
        trg_p_IsoTkMu22 = path_fired_IsoTkMu22 && IsFilterMatched(mu_p, trg_objs_IsoTkMu22, filter_IsoTkMu22, 0.5);

        std::string filter_PFTau120  = "hltPFTau120TrackPt50LooseAbsOrRelVLooseIso";
        auto const& trg_objs_PFTau120  = event->GetPtrVec<TriggerObject>("triggerObjects_VLooseIsoPFTau120_Trk50_eta2p1");
        trg_p_PFTau120 = path_fired_PFTau120 && IsFilterMatched(mu_p, trg_objs_PFTau120, filter_PFTau120, 0.5);

        std::string filter_IsoMu19TauL1  = "hltL3crIsoL1sSingleMu18erIorSingleMu20erL1f0L2f10QL3f19QL3trkIsoFiltered0p09";
        auto const& trg_objs_IsoMu19TauL1  = event->GetPtrVec<TriggerObject>("triggerObjects_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1");
        trg_p_IsoMu19TauL1 = IsFilterMatched(mu_p, trg_objs_IsoMu19TauL1, filter_IsoMu19TauL1, 0.5);

        // For the mu-tau L1 cross seed we have to take a bit more care. Need to check that something other than the probe muon
        // fired the L1 tau part
        std::string filter_IsoMu19Tau  = "hltL3crIsoL1sMu18erTauJet20erL1f0L2f10QL3f19QL3trkIsoFiltered0p09";
        auto const& trg_objs_IsoMu19Tau  = event->GetPtrVec<TriggerObject>("triggerObjects_IsoMu19_eta2p1_LooseIsoPFTau20");
        trg_t_IsoMu19Tau = false;
        for (auto obj : trg_objs_IsoMu19Tau) {
          // First skip objects that don't come from the L1
          auto const& filters = obj->filters();
          if (std::find(filters.begin(), filters.end(),
                        CityHash64("hltL1sMu18erTau20er")) == filters.end()) {
            continue;
          }
          // Then skip L1 objects that aren't a tau
          auto types = GetTriggerTypes(obj);
          if (types.count(-100) == 0) continue;

          // Finally check if the object is separated from the probe muon
          if (DR(obj, mu_p) > 0.5) {
            trg_t_IsoMu19Tau = true;
            break;
          }
        }
        trg_p_IsoMu19Tau = trg_t_IsoMu19Tau && IsFilterMatched(mu_p, trg_objs_IsoMu19Tau, filter_IsoMu19Tau, 0.5);
      } else {
        trg_t_IsoMu22 = true;
        trg_t_IsoMu19Tau = true;
        trg_p_IsoMu22 = true;
        trg_p_IsoTkMu22 = true;
        trg_p_PFTau120 = true;
      }
      muon_p = true;
      // This is a hack for now: we'll pretend muons with pT < 20 are track probes
      // for the purpose of the ID measurement until we get the J/Psi up and running
      if (pt_p <= 20.) {
        trk_p = true;
      } else {
        trk_p = false;
      }
      outtree_->Fill();
    }

    auto tpairs = event->GetPtrVec<CompositeCandidate>("mutrk");
    auto probe_muons = event->GetPtrVec<Muon>("probe_muons");

    for (auto pair : tpairs) {
      Muon const* mu_t = dynamic_cast<Muon const*>(pair->At(0));
      Candidate const* mu_p = dynamic_cast<Candidate const*>(pair->At(1));
      pt_t = mu_t->pt();
      eta_t = mu_t->eta();
      phi_t = mu_t->phi();
      id_t = MuonMediumHIPsafe(mu_t) && fabs(mu_t->dxy_vertex()) < 0.045 &&
             fabs(mu_t->dz_vertex()) < 0.2;
      iso_t = PF04IsolationVal(mu_t, 0.5, 0);
      pt_p = mu_p->pt();
      eta_p = mu_p->eta();
      phi_p = mu_p->phi();
      auto mu_matches = MatchByDR(std::vector<ic::Candidate const*>{mu_p},
                                  probe_muons, 0.3, true, true);
      id_p = false;
      if (mu_matches.size() > 0) {
        auto match_mu = dynamic_cast<ic::Muon *>(mu_matches[0].second);
        if (MuonMediumHIPsafe(match_mu) &&
            fabs(match_mu->dxy_vertex()) < 0.045 &&
            fabs(match_mu->dz_vertex()) < 0.2) {
          id_p = true;
        }
      }
      iso_p = false;
      m_ll = pair->M();

      trg_t_IsoMu22 = false;
      trg_t_IsoMu19Tau = false;
      trg_p_IsoMu22 = false;
      trg_p_IsoTkMu22 = false;
      trg_p_PFTau120 = false;
      trg_p_IsoMu19TauL1 = false;
      trg_p_IsoMu19Tau = false;
      if (info->is_data()) {
        bool path_fired_IsoMu22 = HLTPathCheck(event, "triggerPaths", "HLT_IsoMu22_v");
        std::string filter_IsoMu22 = "hltL3crIsoL1sMu20L1f0L2f10QL3f22QL3trkIsoFiltered0p09";
        auto const& trg_objs_IsoMu22 = event->GetPtrVec<TriggerObject>("triggerObjects_IsoMu22");
        trg_t_IsoMu22 = path_fired_IsoMu22 && IsFilterMatched(mu_t, trg_objs_IsoMu22, filter_IsoMu22, 0.5);
      } else {
        trg_t_IsoMu22 = true;
        trg_t_IsoMu19Tau = true;
        trg_p_IsoMu22 = true;
        trg_p_IsoTkMu22 = true;
        trg_p_PFTau120 = true;
      }
      muon_p = false;
      trk_p = true;
      // Temporary hack: for now skip the case with track pT < 20 GeV,
      // not possible to measure this low for the ID in Z->mumu events,
      // need to wait for the J/Psi
      if (pt_p <= 20.) continue;
      outtree_->Fill();
    }
    return 0;
  }

  int ZmmTPTreeProducer::PostAnalysis() {
    return 0;
  }

  void ZmmTPTreeProducer::PrintInfo() { }


  ZeeTPTreeProducer::ZeeTPTreeProducer(std::string const& name)
      : ModuleBase(name) {
    fs_ = nullptr;
  }

  ZeeTPTreeProducer::~ZeeTPTreeProducer() {
    ;
  }

  int ZeeTPTreeProducer::PreAnalysis() {
    if (fs_) {
      outtree_ = fs_->make<TTree>("ZeeTP","ZeeTP");

      outtree_->Branch("wt",          &wt);
      outtree_->Branch("run",         &run);
      outtree_->Branch("n_vtx",       &n_vtx);
      outtree_->Branch("pt_t",        &pt_t);
      outtree_->Branch("eta_t",       &eta_t);
      outtree_->Branch("phi_t",       &phi_t);
      outtree_->Branch("id_t",        &id_t);
      outtree_->Branch("iso_t",       &iso_t);
      outtree_->Branch("pt_p",        &pt_p);
      outtree_->Branch("eta_p",       &eta_p);
      outtree_->Branch("sc_eta_p",    &sc_eta_p);
      outtree_->Branch("phi_p",       &phi_p);
      outtree_->Branch("id_p",        &id_p);
      outtree_->Branch("iso_p",       &iso_p);
      outtree_->Branch("m_ll",        &m_ll);
      outtree_->Branch("trg_t_Ele25eta2p1WPTight", &trg_t_Ele25eta2p1WPTight);
      outtree_->Branch("trg_t_Ele24Tau",           &trg_t_Ele24Tau);
      outtree_->Branch("trg_p_Ele25eta2p1WPTight", &trg_p_Ele25eta2p1WPTight);
      outtree_->Branch("trg_p_PFTau120",           &trg_p_PFTau120);
      outtree_->Branch("trg_p_Ele24TauL1",         &trg_p_Ele24TauL1);
      outtree_->Branch("trg_p_Ele24Tau",           &trg_p_Ele24Tau);
    }

    TFile f(sf_workspace_.c_str());
    ws_ = std::shared_ptr<RooWorkspace>((RooWorkspace*)gDirectory->Get("w"));
    f.Close();
    return 0;
  }

  int ZeeTPTreeProducer::Execute(TreeEvent *event) {
    auto pairs = event->GetPtrVec<CompositeCandidate>("dielec");
    auto info = event->GetPtr<EventInfo>("eventInfo");
    run = info->run();


    n_vtx = info->good_vertices();
    wt = info->total_weight();

    for (auto pair : pairs) {
      Electron const* el_t = dynamic_cast<Electron const*>(pair->At(0));
      Electron const* el_p = dynamic_cast<Electron const*>(pair->At(1));
      pt_t = el_t->pt();
      eta_t = el_t->eta();
      phi_t = el_t->phi();
      id_t = ElectronHTTIdSpring15(el_t, false) && fabs(el_t->dxy_vertex()) < 0.045 && fabs(el_t->dz_vertex()) < 0.2;
      iso_t = PF03IsolationVal(el_t, 0.5, 0);
      pt_p = el_p->pt();
      eta_p = el_p->eta();
      sc_eta_p = el_p->sc_eta();
      phi_p = el_p->phi();
      id_p = ElectronHTTIdSpring15(el_p, false) && fabs(el_p->dxy_vertex()) < 0.045 && fabs(el_p->dz_vertex()) < 0.2;
      iso_p = PF03IsolationVal(el_p, 0.5, 0);
      m_ll = pair->M();

      trg_t_Ele25eta2p1WPTight = false;
      trg_t_Ele24Tau = false;
      trg_p_Ele25eta2p1WPTight = false;
      trg_p_PFTau120 = false;
      trg_p_Ele24TauL1 = false;
      trg_p_Ele24Tau = false;
      if (info->is_data()) {
        bool path_fired_Ele25eta2p1WPTight = HLTPathCheck(event, "triggerPaths", "HLT_Ele25_eta2p1_WPTight_Gsf_v");
        bool path_fired_PFTau120 = HLTPathCheck(event, "triggerPaths", "HLT_VLooseIsoPFTau120_Trk50_eta2p1_v");

        std::string filter_Ele25eta2p1WPTight = "hltEle25erWPTightGsfTrackIsoFilter";
        auto const& trg_objs_Ele25eta2p1WPTight = event->GetPtrVec<TriggerObject>("triggerObjects_Ele25_eta2p1_WPTight_Gsf");

        trg_t_Ele25eta2p1WPTight = path_fired_Ele25eta2p1WPTight && IsFilterMatched(el_t, trg_objs_Ele25eta2p1WPTight, filter_Ele25eta2p1WPTight, 0.5);
        trg_p_Ele25eta2p1WPTight = path_fired_Ele25eta2p1WPTight && IsFilterMatched(el_p, trg_objs_Ele25eta2p1WPTight, filter_Ele25eta2p1WPTight, 0.5);

        std::string filter_PFTau120  = "hltPFTau120TrackPt50LooseAbsOrRelVLooseIso";
        auto const& trg_objs_PFTau120  = event->GetPtrVec<TriggerObject>("triggerObjects_VLooseIsoPFTau120_Trk50_eta2p1");
        trg_p_PFTau120 = path_fired_PFTau120 && IsFilterMatched(el_p, trg_objs_PFTau120, filter_PFTau120, 0.5);

        std::string filter_Ele24TauL1  = "hltEle24WPLooseL1SingleIsoEG22erGsfTrackIsoFilter";
        auto const& trg_objs_Ele24TauL1  = event->GetPtrVec<TriggerObject>("triggerObjects_Ele24_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_SingleL1");
        trg_p_Ele24TauL1 = IsFilterMatched(el_p, trg_objs_Ele24TauL1, filter_Ele24TauL1, 0.5);

        // For the e-tau L1 cross seed we have to take a bit more care. Need to check that something other than the probe
        // electron fired the L1 tau part
        std::string filter_Ele24Tau  = "hltEle24WPLooseL1IsoEG22erTau20erGsfTrackIsoFilter";
        auto const& trg_objs_Ele24Tau  = event->GetPtrVec<TriggerObject>("triggerObjects_Ele24_eta2p1_WPLoose_Gsf_LooseIsoPFTau20");
        trg_t_Ele24Tau = false;
        for (auto obj : trg_objs_Ele24Tau) {
          // First skip objects that don't come from the L1
          auto const& filters = obj->filters();
          if (std::find(filters.begin(), filters.end(),
                        CityHash64("hltL1sIsoEG22erTau20erdEtaMin0p2")) == filters.end()) {
            continue;
          }
          // Then skip L1 objects that aren't a tau
          auto types = GetTriggerTypes(obj);
          if (types.count(-100) == 0) continue;

          // Finally check if the object is separated from the probe muon
          if (DR(obj, el_p) > 0.5) {
            trg_t_Ele24Tau = true;
            break;
          }
        }
        trg_p_Ele24Tau = trg_t_Ele24Tau && IsFilterMatched(el_p, trg_objs_Ele24Tau, filter_Ele24Tau, 0.5);

      } else {
        trg_t_Ele25eta2p1WPTight = true;
        trg_t_Ele24Tau = true;
        trg_p_Ele25eta2p1WPTight = true;
        trg_p_PFTau120 = true;
        trg_p_Ele24TauL1 = true;
        trg_p_Ele24Tau = true;
      }
      outtree_->Fill();
    }

    return 0;
  }

  int ZeeTPTreeProducer::PostAnalysis() {
    return 0;
  }

  void ZeeTPTreeProducer::PrintInfo() { }


  // ZmtTP
  ZmtTPTreeProducer::ZmtTPTreeProducer(std::string const& name)
      : ModuleBase(name),
        do_zpt_reweighting_(false),
        do_top_reweighting_(false) {
    fs_ = nullptr;
  }

  ZmtTPTreeProducer::~ZmtTPTreeProducer() {
    ;
  }

  int ZmtTPTreeProducer::PreAnalysis() {
    if (fs_) {
      outtree_ = fs_->make<TTree>("ZmtTP", "ZmtTP");

      outtree_->Branch("wt",          &wt);
      outtree_->Branch("wt_pu_hi",    &wt_pu_hi);
      outtree_->Branch("wt_mfr_l",    &wt_mfr_l);
      outtree_->Branch("wt_mfr_t",    &wt_mfr_t);
      outtree_->Branch("wt_zpt",      &wt_zpt);
      outtree_->Branch("wt_top",      &wt_top);
      outtree_->Branch("wt_tauid",    &wt_tauid);
      outtree_->Branch("n_vtx",       &n_vtx);
      outtree_->Branch("rho",         &rho);
      outtree_->Branch("pt_m",        &pt_m);
      outtree_->Branch("eta_m",       &eta_m);
      outtree_->Branch("mt_m",        &mt_m);
      outtree_->Branch("pzeta",       &pzeta);
      outtree_->Branch("pt_t",        &pt_t);
      outtree_->Branch("eta_t",       &eta_t);
      outtree_->Branch("dm_t",        &dm_t);
      outtree_->Branch("tot_ch_t",    &tot_ch_t);
      outtree_->Branch("anti_e_vl_t",   &anti_e_vl_t);
      outtree_->Branch("anti_e_t_t",    &anti_e_t_t);
      outtree_->Branch("anti_m_t_t",    &anti_m_t_t);
      outtree_->Branch("anti_m_l_t",    &anti_m_l_t);
      outtree_->Branch("mva_vl_t",    &mva_vl_t);
      outtree_->Branch("mva_l_t",     &mva_l_t);
      outtree_->Branch("mva_m_t",     &mva_m_t);
      outtree_->Branch("mva_t_t",     &mva_t_t);
      outtree_->Branch("mva_vt_t",    &mva_vt_t);
      outtree_->Branch("mva_vvt_t",   &mva_vvt_t);
      outtree_->Branch("cmb_l_t",     &cmb_l_t);
      outtree_->Branch("cmb_m_t",     &cmb_m_t);
      outtree_->Branch("cmb_t_t",     &cmb_t_t);
      outtree_->Branch("cbiso_t",     &cbiso_t);
      outtree_->Branch("cbiso_0p5_t", &cbiso_0p5_t);
      outtree_->Branch("cbiso_1p0_t", &cbiso_1p0_t);
      outtree_->Branch("cbiso_1p5_t", &cbiso_1p5_t);
      outtree_->Branch("cbiso_2p0_t", &cbiso_2p0_t);
      outtree_->Branch("chiso_t",     &chiso_t);
      outtree_->Branch("ntiso_t",     &ntiso_t);
      outtree_->Branch("ntiso_0p5_t", &ntiso_0p5_t);
      outtree_->Branch("ntiso_1p0_t", &ntiso_1p0_t);
      outtree_->Branch("ntiso_1p5_t", &ntiso_1p5_t);
      outtree_->Branch("ntiso_2p0_t", &ntiso_2p0_t);
      outtree_->Branch("puiso_t",     &puiso_t);
      outtree_->Branch("pho_out_t",   &pho_out_t);
      outtree_->Branch("pho_out_0p0_t",   &pho_out_0p0_t);
      outtree_->Branch("pho_out_0p5_t",   &pho_out_0p5_t);
      outtree_->Branch("pho_out_1p0_t",   &pho_out_1p0_t);
      outtree_->Branch("pho_out_1p5_t",   &pho_out_1p5_t);
      outtree_->Branch("pho_out_2p0_t",   &pho_out_2p0_t);
      outtree_->Branch("nt_density_0p0_0p1",   &nt_density_0p0_0p1);
      outtree_->Branch("nt_density_0p1_0p2",   &nt_density_0p1_0p2);
      outtree_->Branch("nt_density_0p2_0p3",   &nt_density_0p2_0p3);
      outtree_->Branch("nt_density_0p3_0p4",   &nt_density_0p3_0p4);
      outtree_->Branch("nt_density_0p4_0p5",   &nt_density_0p4_0p5);
      outtree_->Branch("po_density_0p0_0p1",   &po_density_0p0_0p1);
      outtree_->Branch("po_density_0p1_0p2",   &po_density_0p1_0p2);
      outtree_->Branch("po_density_0p2_0p3",   &po_density_0p2_0p3);
      outtree_->Branch("po_density_0p3_0p4",   &po_density_0p3_0p4);
      outtree_->Branch("po_density_0p4_0p5",   &po_density_0p4_0p5);
      outtree_->Branch("n_iso_ph_0p5", &n_iso_ph_0p5);
      outtree_->Branch("n_sig_ph_0p5", &n_sig_ph_0p5);
      outtree_->Branch("n_iso_ph_1p0", &n_iso_ph_1p0);
      outtree_->Branch("n_sig_ph_1p0", &n_sig_ph_1p0);
      outtree_->Branch("n_iso_ph_1p5", &n_iso_ph_1p5);
      outtree_->Branch("n_sig_ph_1p5", &n_sig_ph_1p5);
      outtree_->Branch("n_iso_ph_2p0", &n_iso_ph_2p0);
      outtree_->Branch("n_sig_ph_2p0", &n_sig_ph_2p0);
      outtree_->Branch("trg_m_IsoMu19TauL1",    &trg_m_IsoMu19TauL1);
      outtree_->Branch("trg_t_IsoMu19TauL1",    &trg_t_IsoMu19TauL1);
      outtree_->Branch("trg_m_IsoMu19Tau ",     &trg_m_IsoMu19Tau);
      outtree_->Branch("trg_t_IsoMu19Tau ",     &trg_t_IsoMu19Tau);
      outtree_->Branch("gen_1",       &gen_1);
      outtree_->Branch("gen_2",       &gen_2);
      outtree_->Branch("e_veto",       &e_veto);
      outtree_->Branch("m_veto",       &m_veto);
      outtree_->Branch("n_bjets",     &n_bjets);
      outtree_->Branch("n_uncorr_bjets", &n_uncorr_bjets);
      outtree_->Branch("os",          &os);
      outtree_->Branch("m_ll",        &m_ll);

    }

    TFile f(sf_workspace_.c_str());
    ws_ = std::shared_ptr<RooWorkspace>((RooWorkspace*)gDirectory->Get("w"));
    f.Close();
    fns_["m_trk_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_trk_ratio")->functor(ws_->argSet("m_eta")));
    fns_["m_id_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_id_ratio")->functor(ws_->argSet("m_pt,m_eta")));
    fns_["m_iso_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_iso_ratio")->functor(ws_->argSet("m_pt,m_eta")));
    fns_["m_trgOR_data"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_trgOR_data")->functor(ws_->argSet("m_pt,m_eta")));
    fns_["m_idiso0p15_desy_ratio"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_idiso0p15_desy_ratio")->functor(ws_->argSet("m_pt,m_eta")));
    fns_["m_trgIsoMu22orTkIsoMu22_desy_data"] = std::shared_ptr<RooFunctor>(
      ws_->function("m_trgIsoMu22orTkIsoMu22_desy_data")->functor(ws_->argSet("m_pt,m_eta")));
    fns_["zpt_weight"] = std::shared_ptr<RooFunctor>(
      ws_->function("zpt_weight")->functor(ws_->argSet("z_gen_mass,z_gen_pt")));
    fns_["t_muonfr_loose"] = std::shared_ptr<RooFunctor>(
      ws_->function("t_muonfr_loose")->functor(ws_->argSet("t_pt,t_eta")));
    fns_["t_muonfr_tight"] = std::shared_ptr<RooFunctor>(
      ws_->function("t_muonfr_tight")->functor(ws_->argSet("t_pt,t_eta")));
    fns_["t_iso_mva_m_pt30_sf"] = std::shared_ptr<RooFunctor>(
      ws_->function("t_iso_mva_m_pt30_sf")->functor(ws_->argSet("t_pt,t_eta,t_dm")));
    return 0;
  }

  int ZmtTPTreeProducer::Execute(TreeEvent *event) {
    typedef std::vector<double> v_double;

    auto info = event->GetPtr<EventInfo>("eventInfo");
    auto pairs = event->GetPtrVec<CompositeCandidate>("ditau");

    if (info->is_data()) {
      bool path_IsoMu22 = HLTPathCheck(event, "triggerPaths", "HLT_IsoMu22_v");
      bool path_IsoTkMu22 = HLTPathCheck(event, "triggerPaths", "HLT_IsoTkMu22_v");

      std::string filt_IsoMu22 = "hltL3crIsoL1sMu20L1f0L2f10QL3f22QL3trkIsoFiltered0p09";
      auto const& objs_IsoMu22 = event->GetPtrVec<TriggerObject>("triggerObjects_IsoMu22");
      std::string filt_IsoTkMu22 = "hltL3fL1sMu20L1f0Tkf22QL3trkIsoFiltered0p09";
      auto const& objs_IsoTkMu22 = event->GetPtrVec<TriggerObject>("triggerObjects_IsoTkMu22");

      ic::keep_if(pairs, [&](ic::CompositeCandidate const* c) {
        return (path_IsoMu22 && IsFilterMatched(c->At(0), objs_IsoMu22, filt_IsoMu22, 0.5)) ||
               (path_IsoTkMu22 && IsFilterMatched(c->At(0), objs_IsoTkMu22, filt_IsoTkMu22, 0.5));
      });
    } else {
      // Nothing at the moment...
    }
    if (pairs.size() == 0) return 1;

    // std::sort(pairs.begin(), pairs.end(), SortByIsoMT);
    // event->Add("sorted_ditau", pairs);

    Muon const* muon = dynamic_cast<Muon const*>(pairs[0]->At(0));
    Tau const* tau = dynamic_cast<Tau const*>(pairs[0]->At(1));

    pt_m = muon->pt();
    eta_m = muon->eta();

    pt_t = tau->pt();
    eta_t = tau->eta();
    dm_t = tau->decay_mode();
    float cone = TMath::Max(TMath::Min(0.1, 3.0/pt_t), 0.05);

    tot_ch_t  = tau->sig_charged_cands().size() + tau->iso_charged_cands().size();
    anti_e_vl_t  = tau->GetTauID("againstElectronVLooseMVA6") > 0.5;
    anti_m_t_t  = tau->GetTauID("againstMuonTight3") > 0.5;
    anti_e_t_t  = tau->GetTauID("againstElectronTightMVA6") > 0.5;
    anti_m_l_t  = tau->GetTauID("againstMuonLoose3") > 0.5;
    mva_vl_t  = tau->GetTauID("byVLooseIsolationMVArun2v1DBoldDMwLT") > 0.5;
    mva_l_t   = tau->GetTauID("byLooseIsolationMVArun2v1DBoldDMwLT") > 0.5;
    mva_m_t   = tau->GetTauID("byMediumIsolationMVArun2v1DBoldDMwLT") > 0.5;
    mva_t_t   = tau->GetTauID("byTightIsolationMVArun2v1DBoldDMwLT") > 0.5;
    mva_vt_t  = tau->GetTauID("byVTightIsolationMVArun2v1DBoldDMwLT") > 0.5;
    mva_vvt_t = tau->GetTauID("byVVTightIsolationMVArun2v1DBoldDMwLT") > 0.5;
    cmb_l_t   = tau->GetTauID("byLooseCombinedIsolationDeltaBetaCorr3Hits") > 0.5;
    cmb_m_t   = tau->GetTauID("byMediumCombinedIsolationDeltaBetaCorr3Hits") > 0.5;
    cmb_t_t   = tau->GetTauID("byTightCombinedIsolationDeltaBetaCorr3Hits") > 0.5;
    cbiso_t   = tau->GetTauID("byCombinedIsolationDeltaBetaCorrRaw3Hits");
    chiso_t   = tau->GetTauID("chargedIsoPtSum");
    ntiso_t   = tau->GetTauID("neutralIsoPtSum");
    puiso_t   = tau->GetTauID("puCorrPtSum");
    pho_out_t = tau->GetTauID("photonPtSumOutsideSignalCone");

    auto pfcands = event->GetIDMap<PFCandidate>("pfCandIDMap", "pfCandidates");
    // auto tracks = event->GetIDMap<Track>("trackIDMap", "tracks");
    auto const& iso_gammas = tau->iso_gamma_cands();
    auto const& sig_gammas = tau->sig_gamma_cands();
    auto const& sig_charged = tau->sig_charged_cands();
    ic::Candidate charged;
    for (auto id : sig_charged) {
      charged.set_vector(charged.vector() + pfcands[id]->vector());
    }
    n_iso_ph_0p5 = 0;
    n_sig_ph_0p5 = 0;
    n_iso_ph_1p0 = 0;
    n_sig_ph_1p0 = 0;
    n_iso_ph_1p5 = 0;
    n_sig_ph_1p5 = 0;
    n_iso_ph_2p0 = 0;
    n_sig_ph_2p0 = 0;

    ntiso_0p5_t = 0.;
    ntiso_1p0_t = 0.;
    ntiso_1p5_t = 0.;
    ntiso_2p0_t = 0.;
    pho_out_0p0_t = 0.;
    pho_out_0p5_t = 0.;
    pho_out_1p0_t = 0.;
    pho_out_1p5_t = 0.;
    pho_out_2p0_t = 0.;
    nt_density_0p0_0p1 = 0.;
    nt_density_0p1_0p2 = 0.;
    nt_density_0p2_0p3 = 0.;
    nt_density_0p3_0p4 = 0.;
    nt_density_0p4_0p5 = 0.;
    po_density_0p0_0p1 = 0.;
    po_density_0p1_0p2 = 0.;
    po_density_0p2_0p3 = 0.;
    po_density_0p3_0p4 = 0.;
    po_density_0p4_0p5 = 0.;
    for (auto id : iso_gammas) {
      double et = pfcands[id]->vector().Et();
      double pt = pfcands[id]->pt();
      double dr_c = DR(pfcands[id], &charged);
      if (et > 0.5) {
        ++n_iso_ph_0p5;
        ntiso_0p5_t += pt;
      }
      if (et > 1.0) {
        ++n_iso_ph_1p0;
        ntiso_1p0_t += pt;
      }
      if (et > 1.5) {
        ++n_iso_ph_1p5;
        ntiso_1p5_t += pt;
      }
      if (et > 2.0) {
        ++n_iso_ph_2p0;
        ntiso_2p0_t += pt;
      }
      if (dr_c >= 0.0 && dr_c < 0.1) nt_density_0p0_0p1 += pt;
      if (dr_c >= 0.1 && dr_c < 0.2) nt_density_0p1_0p2 += pt;
      if (dr_c >= 0.2 && dr_c < 0.3) nt_density_0p2_0p3 += pt;
      if (dr_c >= 0.3 && dr_c < 0.4) nt_density_0p3_0p4 += pt;
      if (dr_c >= 0.4 && dr_c < 0.5) nt_density_0p4_0p5 += pt;
    }
    nt_density_0p0_0p1 /= (TMath::Pi() * 0.1 * 0.1);
    nt_density_0p1_0p2 /= (TMath::Pi() * (0.2 * 0.2 - 0.1 * 0.1));
    nt_density_0p2_0p3 /= (TMath::Pi() * (0.3 * 0.3 - 0.2 * 0.2));
    nt_density_0p3_0p4 /= (TMath::Pi() * (0.4 * 0.4 - 0.3 * 0.3));
    nt_density_0p4_0p5 /= (TMath::Pi() * (0.5 * 0.5 - 0.4 * 0.4));

    for (auto id : sig_gammas) {
      double et = pfcands[id]->vector().Et();
      double pt = pfcands[id]->pt();
      double dr = DR(pfcands[id], tau);
      double dr_c = DR(pfcands[id], &charged);
      if (dr > cone) {
        pho_out_0p0_t += pt;
      }
      if (et > 0.5) {
        ++n_sig_ph_0p5;
        if (dr > cone) {
          pho_out_0p5_t += pt;
        }
      }
      if (et > 1.0) {
        ++n_sig_ph_1p0;
        if (dr > cone) {
          pho_out_1p0_t += pt;
        }
      }
      if (et > 1.5) {
        ++n_sig_ph_1p5;
        if (dr > cone) {
          pho_out_1p5_t += pt;
        }
      }
      if (et > 2.0) {
        ++n_sig_ph_2p0;
        if (dr > cone) {
          pho_out_2p0_t += pt;
        }
      }
      if (dr_c >= 0.0 && dr_c < 0.1) po_density_0p0_0p1 += pt;
      if (dr_c >= 0.1 && dr_c < 0.2) po_density_0p1_0p2 += pt;
      if (dr_c >= 0.2 && dr_c < 0.3) po_density_0p2_0p3 += pt;
      if (dr_c >= 0.3 && dr_c < 0.4) po_density_0p3_0p4 += pt;
      if (dr_c >= 0.4 && dr_c < 0.5) po_density_0p4_0p5 += pt;
    }
    po_density_0p0_0p1 /= (TMath::Pi() * 0.1 * 0.1);
    po_density_0p1_0p2 /= (TMath::Pi() * (0.2 * 0.2 - 0.1 * 0.1));
    po_density_0p2_0p3 /= (TMath::Pi() * (0.3 * 0.3 - 0.2 * 0.2));
    po_density_0p3_0p4 /= (TMath::Pi() * (0.4 * 0.4 - 0.3 * 0.3));
    po_density_0p4_0p5 /= (TMath::Pi() * (0.5 * 0.5 - 0.4 * 0.4));

    cbiso_0p5_t = chiso_t + TMath::Max(0., ntiso_0p5_t - 0.2 * puiso_t);
    cbiso_1p0_t = chiso_t + TMath::Max(0., ntiso_1p0_t - 0.2 * puiso_t);
    cbiso_1p5_t = chiso_t + TMath::Max(0., ntiso_1p5_t - 0.2 * puiso_t);
    cbiso_2p0_t = chiso_t + TMath::Max(0., ntiso_2p0_t - 0.2 * puiso_t);
    // std::cout << "dm: " << dm_t << " nSigCh: " << tau->sig_charged_cands().size()
    //           << " nSigNt: " << tau->sig_gamma_cands().size()
    //           << " ntiso_0p5: " << ntiso_0p5_t << " ntiso: " << ntiso_t
    //           << " cbiso_0p5: " << cbiso_0p5_t
    //           << " cbiso: " << cbiso_t
    //           << " pho_out_0p5: " << pho_out_0p5_t
    //           << " pho_out: " << pho_out_t << "\n";

    trg_m_IsoMu19TauL1 = false;
    trg_t_IsoMu19TauL1 = false;
    trg_m_IsoMu19Tau = false;
    trg_t_IsoMu19Tau = false;

    if (info->is_data()) {
      std::string filter_m_IsoMu19TauL1  = "hltL3crIsoL1sSingleMu18erIorSingleMu20erL1f0L2f10QL3f19QL3trkIsoFiltered0p09";
      std::string filter_t_IsoMu19TauL1  = "hltPFTau20TrackLooseIsoAgainstMuon";
      auto const& trg_objs_IsoMu19TauL1  = event->GetPtrVec<TriggerObject>("triggerObjects_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1");
      trg_m_IsoMu19TauL1 = IsFilterMatched(muon, trg_objs_IsoMu19TauL1, filter_m_IsoMu19TauL1, 0.5);
      trg_t_IsoMu19TauL1 = IsFilterMatched(tau, trg_objs_IsoMu19TauL1, filter_t_IsoMu19TauL1, 0.5);

      std::string filter_m_IsoMu19Tau  = "hltL3crIsoL1sMu18erTauJet20erL1f0L2f10QL3f19QL3trkIsoFiltered0p09";
      std::string filter_t_IsoMu19Tau  = "hltPFTau20TrackLooseIsoAgainstMuon";
      auto const& trg_objs_IsoMu19Tau  = event->GetPtrVec<TriggerObject>("triggerObjects_IsoMu19_eta2p1_LooseIsoPFTau20");
      trg_m_IsoMu19Tau = false;
      trg_t_IsoMu19Tau = false;
      bool trg_t_IsoMU19Tau_L1matched = false;
      bool trg_m_IsoMU19Tau_L1other = false;
      for (auto obj : trg_objs_IsoMu19Tau) {
        // First skip objects that don't come from the L1
        auto const& filters = obj->filters();
        if (std::find(filters.begin(), filters.end(),
                      CityHash64("hltL1sMu18erTau20er")) == filters.end()) {
          continue;
        }
        // Then skip L1 objects that aren't a tau
        auto types = GetTriggerTypes(obj);
        if (types.count(-100) == 0) continue;

        // Finally check if the object is separated from the muon *and*
        // the probe tau
        if (DR(obj, muon) > 0.5 && DR(obj, tau) > 0.5) {
          trg_m_IsoMU19Tau_L1other = true;
        }
        if (DR(obj, tau) < 0.5) {
          trg_t_IsoMU19Tau_L1matched = true;
        }
      }
      // Did the L1 x-seed fire (but with the tau part due to some other object in the event)
      // Might not be necessary if the IsoMu22 firing guarantees the IsoMu19 part
      trg_m_IsoMu19Tau = trg_m_IsoMU19Tau_L1other && IsFilterMatched(muon, trg_objs_IsoMu19Tau, filter_m_IsoMu19Tau, 0.5);
      // Did the tau fire the L1 and HLT parts too?
      trg_t_IsoMu19Tau = trg_t_IsoMU19Tau_L1matched && IsFilterMatched(tau, trg_objs_IsoMu19Tau, filter_t_IsoMu19Tau, 0.5);
    } else {
      trg_m_IsoMu19TauL1 = true;
      trg_t_IsoMu19TauL1 = true;
      trg_m_IsoMu19Tau = true;
      trg_t_IsoMu19Tau = true;
    }

    n_vtx = info->good_vertices();
    rho = info->jet_rho();

    // Get the MVA MET so we can calculate MT
    Met *mva_met = event->GetPtr<Met>("mvamet");
    // auto const& met_map = event->GetIDMap<Met>("pfMVAMetVector");
    // std::size_t id = 0;
    // boost::hash_combine(id, muon->id());
    // boost::hash_combine(id, tau->id());
    // std::size_t id_inv = 0;
    // boost::hash_combine(id_inv, tau->id());
    // boost::hash_combine(id_inv, muon->id());
    // auto it = met_map.find(id);
    // auto it_inv = met_map.find(id_inv);
    // if (it != met_map.end()) {
    //   mva_met = it->second;
    //   // event->Add("pfMVAMet", mva_met);
    // } else if (it_inv != met_map.end()){
    //   //lepton1 <--> lepton2 (needed for fully hadronic)
    //   mva_met = it_inv->second;
    //   // event->Add("pfMVAMet", mva_met);
    // } else {
    //   throw::std::runtime_error("Could not find MVA MET!");
    // }

    // typedef std::map<std::size_t, ROOT::Math::PxPyPzEVector> map_id_vec;
    // auto const& tau_es_shifts = event->Get<map_id_vec>("shifts_TauEnergyScale");
    // CorrectMETForShift(mva_met, tau_es_shifts.at(tau->id()));

    mt_m = MT(muon, mva_met);
    pzeta = PZeta(pairs[0], mva_met, 0.85);
    os = (pairs[0]->charge() == 0);
    m_ll = pairs[0]->M();

    auto bjets = event->GetPtrVec<PFJet>("ak4PFJetsCHS");
    ic::keep_if(bjets, [&](PFJet *j) {
      return j->pt()          > 20. &&
             fabs(j->eta())   < 2.4 &&
             PFJetID2016(j) &&
             MinDRToCollection(j, pairs[0]->AsVector(), 0.5);
    });
    auto retag_bjets = bjets;
    ic::keep_if(bjets, [&](PFJet *j) {
      return j->GetBDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > 0.8;
    });
    n_bjets = bjets.size();
    n_uncorr_bjets = n_bjets;
    if (!info->is_data()) {
      auto const& retag_result = event->Get<std::map<std::size_t,bool>>("retag_result");
      ic::keep_if(retag_bjets, [&](PFJet *j) {
        return IsReBTagged(j, retag_result);
      });
      n_uncorr_bjets = retag_bjets.size();
    }

    n_bjets = bjets.size();


    gen_1 = 0;
    gen_2 = 0;
    double wt_trk = 1.0;
    double wt_id = 1.0;
    double wt_iso = 1.0;
    double wt_trg = 1.0;
    wt_mfr_l = 1.0;
    wt_mfr_t = 1.0;
    wt_pu_hi = 1.0;
    wt_tauid = 1.0;
    auto args = std::vector<double>{muon->pt(), muon->eta()};
    if (!info->is_data()) {
      wt_trg = fns_["m_trgIsoMu22orTkIsoMu22_desy_data"]->eval(args.data());
      // wt_trg = fns_["m_trgOR_data"]->eval(args.data());
      wt_trk = fns_["m_trk_ratio"]->eval(v_double{muon->eta()}.data());
      // wt_id = fns_["m_id_ratio"]->eval(args.data());
      // wt_iso = fns_["m_iso_ratio"]->eval(args.data());
      wt_id = fns_["m_idiso0p15_desy_ratio"]->eval(args.data());
      // wt_id = fns_["m_id_ratio"]->eval(args.data());
      // wt_iso = fns_["m_iso_ratio"]->eval(args.data());
      gen_1 = MCOrigin2UInt(event->Get<ic::mcorigin>("gen_match_1"));
      gen_2 = MCOrigin2UInt(event->Get<ic::mcorigin>("gen_match_2"));
      if (info->weight("pileup") > 0.) {
        wt_pu_hi = info->weight("pileup_hi") / info->weight("pileup");
      }
      if (gen_2 == 2 || gen_2 == 4) {
        wt_mfr_l = fns_["t_muonfr_loose"]->eval(v_double{tau->pt(), tau->eta()}.data());
        wt_mfr_t = fns_["t_muonfr_tight"]->eval(v_double{tau->pt(), tau->eta()}.data());
      }
      if (gen_2 == 5) {
        wt_tauid = fns_["t_iso_mva_m_pt30_sf"]->eval(v_double{tau->pt(), tau->eta(), double(tau->decay_mode())}.data());
      }
    }
    info->set_weight("trk", wt_trk);
    info->set_weight("trg", wt_trg);
    info->set_weight("id", wt_id);
    info->set_weight("iso", wt_iso);

    e_veto = event->Get<bool>("extra_elec_veto");
    m_veto = event->Get<bool>("extra_muon_veto");

    // DY reweighting
    wt_zpt = 1.0;
    if (do_zpt_reweighting_) {
      auto const& parts = event->GetPtrVec<GenParticle>("genParticles");
      auto gen_boson = BuildGenBoson(parts);
      wt_zpt = fns_["zpt_weight"]->eval(v_double{gen_boson.M(), gen_boson.pt()}.data());
    }

    // Top quark pT reweighting
    wt_top = 1.0;
    if (do_top_reweighting_) {
      auto const& parts = event->GetPtrVec<GenParticle>("genParticles");
      wt_top = TopQuarkPtWeight(parts);
    }

    wt = info->total_weight();

    outtree_->Fill();

    return 0;
  }

  int ZmtTPTreeProducer::PostAnalysis() {
    return 0;
  }


  bool HLTPathCheck(ic::TreeEvent* event, std::string const& label,
                    std::string const& path) {
    bool path_fired = false;
    auto const& trg_paths = event->GetPtrVec<TriggerPath>(label);
    for (auto trg : trg_paths) {
      if (trg->id() == CityHash64(path) && trg->accept()) {
        path_fired = true;
        break;
      }
    }
    return path_fired;
  }

  std::set<int16_t> GetTriggerTypes(ic::TriggerObject* obj) {
    std::set<int16_t> types;
    ui64 packed_type;
    packed_type.one = obj->id();

    for (unsigned i = 0; i < 4; ++i) {
      int16_t type = packed_type.four[i];
      if (type != 0) types.insert(type);
    }
    return types;
  }


  void CorrectMETForShift(ic::Met * met, ROOT::Math::PxPyPzEVector const& shift) {
    double metx = met->vector().px() - shift.px();
    double mety = met->vector().py() - shift.py();
    double metet = sqrt(metx*metx + mety*mety);
    ROOT::Math::PxPyPzEVector new_met(metx, mety, 0, metet);
    met->set_vector(ROOT::Math::PtEtaPhiEVector(new_met));
  }

  double TopQuarkPtWeight(std::vector<GenParticle *> const& parts) {
    double top_wt = 1.0;
    // double top_wt_up = 1.0;
    // double top_wt_down = 1.0;
    for (unsigned i = 0; i < parts.size(); ++i) {
      std::vector<bool> status_flags = parts[i]->statusFlags();
      unsigned id = abs(parts[i]->pdgid());
      if (id == 6 && status_flags[FromHardProcess] &&
          status_flags[IsLastCopy]) {
        double pt = parts[i]->pt();
        pt = std::min(pt, 400.);
        top_wt *= std::exp(0.156 - 0.00137 * pt);
      }
    }
    top_wt = std::sqrt(top_wt);
    return top_wt;
    // top_wt_up = top_wt * top_wt;
    // top_wt_down = 1.0;
    // event->Add("wt_tquark_up", top_wt_up / top_wt);
    // event->Add("wt_tquark_down", top_wt_down / top_wt);
    // eventInfo->set_weight("topquark_weight", top_wt);
  }

  ROOT::Math::PtEtaPhiEVector BuildGenBoson(
      std::vector<GenParticle*> const& parts) {
    ROOT::Math::PtEtaPhiEVector gen_boson;
    for (unsigned i = 0; i < parts.size(); ++i) {
      std::vector<bool> status_flags = parts[i]->statusFlags();
      unsigned id = abs(parts[i]->pdgid());
      unsigned status = abs(parts[i]->status());
      if ((id >= 11 && id <= 16 && status_flags[FromHardProcess] &&
           status == 1) ||
          status_flags[IsDirectHardProcessTauDecayProduct]) {
        gen_boson += parts[i]->vector();
      }
    }
    return gen_boson;
  }

  bool SortByIsoMT(CompositeCandidate const* c1, CompositeCandidate const* c2) {
    Muon const* m1 = static_cast<Muon const*>(c1->At(0));
    Muon const* m2 = static_cast<Muon const*>(c2->At(0));
    double m_iso1 = PF04IsolationVal(m1, 0.5, 0);
    double m_iso2 = PF04IsolationVal(m2, 0.5, 0);
    if (m_iso1 != m_iso2) return m_iso1 < m_iso2;
    if (m1->pt() != m2->pt()) return m1->pt() > m2->pt();
    Tau const* t1 = static_cast<Tau const*>(c1->At(1));
    Tau const* t2 = static_cast<Tau const*>(c2->At(1));
    double t_iso1 = t1->GetTauID("byIsolationMVArun2v1DBoldDMwLTraw");
    double t_iso2 = t2->GetTauID("byIsolationMVArun2v1DBoldDMwLTraw");
    if (t_iso1 != t_iso2) return t_iso1 > t_iso2;
    return (t1->pt() > t2->pt());
  }

  bool SortMM(CompositeCandidate const* c1, CompositeCandidate const* c2) {
    Muon const* m1c1 = static_cast<Muon const*>(c1->At(0));
    Muon const* m1c2 = static_cast<Muon const*>(c2->At(0));
    Muon const* m2c1 = static_cast<Muon const*>(c1->At(1));
    Muon const* m2c2 = static_cast<Muon const*>(c2->At(1));
    double c1min = std::min(PF04IsolationVal(m1c1, 0.5), PF04IsolationVal(m2c1, 0.5));
    double c1max = std::max(PF04IsolationVal(m1c1, 0.5), PF04IsolationVal(m2c1, 0.5));
    double c2min = std::min(PF04IsolationVal(m1c2, 0.5), PF04IsolationVal(m2c2, 0.5));
    double c2max = std::max(PF04IsolationVal(m1c2, 0.5), PF04IsolationVal(m2c2, 0.5));
    if (c1min != c2min) return c1min < c2min;
    if (c1max != c2max) return c1max < c2max;
    if (m1c1->pt() != m1c2->pt()) return m1c1->pt() > m1c2->pt();
    return m2c1->pt() > m2c2->pt();
  }
}
