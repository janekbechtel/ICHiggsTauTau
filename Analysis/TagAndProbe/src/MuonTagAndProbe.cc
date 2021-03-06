#include "UserCode/ICHiggsTauTau/Analysis/TagAndProbe/interface/MuonTagAndProbe.h"
#include "UserCode/ICHiggsTauTau/Analysis/Utilities/interface/PileUpFunctions.h"
#include "UserCode/ICHiggsTauTau/Analysis/Utilities/interface/FnPredicates.h"
#include "UserCode/ICHiggsTauTau/Analysis/Utilities/interface/FnPairs.h"
#include "boost/lexical_cast.hpp"
#include "boost/bind.hpp"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "TH1.h"
#include <string>
#include <map>
#include <iostream>
#include "UserCode/ICHiggsTauTau//interface/city.h"

namespace ic {

  MuonTagAndProbe::MuonTagAndProbe(std::string const& name) : ModuleBase(name) {
   min_mass_=0;
   max_mass_=200;
   hist_bins_=200;
   probe_match=false;
   fs_=NULL;
   split_pm_eta_=false;
   run_low_=0;
   run_high_=1000000;
  }

  MuonTagAndProbe::~MuonTagAndProbe() {
  }
    
  bool MuonTagAndProbe::hasL1MET(TreeEvent *event, std::vector<TriggerObject *> &mutau_objs) {
    bool hasL1MET=false;
    if(data_)
    {
        std::string filter="hltL1sL1Mu7erETM26";
        std::size_t hash = CityHash64(filter);
        for (unsigned i = 0; i < mutau_objs.size(); ++i)
        {
            std::vector<std::size_t> const& labels = mutau_objs[i]->filters();
            if (std::find(labels.begin(),labels.end(), hash) != labels.end()) hasL1MET=true;
            std::vector<Candidate *> const& l1met = event->GetPtrVec<Candidate>("l1extraMET");
            if(hasL1MET && (l1met.at(0)->pt() < 26)) hasL1MET=false;

        }
    }
    else
    {
        std::vector<Candidate *> const& l1met = event->GetPtrVec<Candidate>("l1extraMET");
        hasL1MET = l1met.at(0)->pt() > 26.;
    }
    
    return hasL1MET;  
  }
  
  bool MuonTagAndProbe::PassL1Lepton(TreeEvent *event, std::vector<TriggerObject *> &mutau_objs, Muon* probeMuon) {
    ic::erase_if(mutau_objs, !boost::bind(MinPtMaxEta, _1, 8.0, 2.1));
    std::vector<Candidate *> l1muon = event->GetPtrVec<Candidate>("l1extraMuons");
    ic::erase_if(l1muon, !boost::bind(MinPtMaxEta, _1, 7.0, 2.1));
    std::vector<Candidate *> mu_as_vector;
    mu_as_vector.push_back(probeMuon);
    bool leg1_l1_match = MatchByDR(mu_as_vector, l1muon, 0.5, false, false).size() > 0;
    return leg1_l1_match;
  }


  int MuonTagAndProbe::PreAnalysis() {
    std::string dataormc;
    if(data_) dataormc="data";
    if(!data_) dataormc="MC";
    //fs_ = new fwlite::TFileService("eeTandP"+dataormc+era_+".root");
    hists_ = new DynamicHistoSet(fs_->mkdir("/"));

    // Set up necessary histograms depending on mode. These histograms are named such
    // that the fitting code will still work.
    std::string label;
    if(mode_==0 || mode_==10) label="id_";          //0=ID, 10=ID in finer bins
    if(mode_==1 || mode_==11) label="iso_";         //1=Iso, 11=iso in finer bins
    if(mode_==2 || mode_==12) label="idiso_";       //2=IDIso, 12=IDIso in finer bins
    if(mode_==3) label="";                          //3=Trigger efficiencies for mutau channel
    if(mode_==4) label="";                          //4=Trigger efficiencies for mutau+met channel
    if(mode_==5) label="";                          //5=Trigger efficiencies for IsoMu24 trigger

    if(mode_==0 || mode_==1 || mode_==2 || mode_==3 || mode_==4 || mode_==5)
    {
        for(unsigned i=0; i<pt_bins_.size()-1; i++)
        {
            std::string s=boost::lexical_cast<std::string>(i+1);
            if(!split_pm_eta_)
            {
                hists_->Create(label+"h_TP_"+s+"B", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TF_"+s+"B", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TP_"+s+"E", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TF_"+s+"E", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TP_"+s+"Eb", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TF_"+s+"Eb", hist_bins_, min_mass_, max_mass_);
            }
            else if (split_pm_eta_)
            {
                hists_->Create(label+"h_TP_"+s+"Bplus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TF_"+s+"Bplus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TP_"+s+"Eplus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TF_"+s+"Eplus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TP_"+s+"Ebplus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TF_"+s+"Ebplus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TP_"+s+"Bminus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TF_"+s+"Bminus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TP_"+s+"Eminus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TF_"+s+"Eminus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TP_"+s+"Ebminus", hist_bins_, min_mass_, max_mass_);
                hists_->Create(label+"h_TF_"+s+"Ebminus", hist_bins_, min_mass_, max_mass_);
            }
        }
    }
    if(mode_==10 || mode_==11 || mode_==12)
    {
        for(unsigned i=0; i<pt_bins_.size()-1; i++)
        {
            std::string s=boost::lexical_cast<std::string>(i+1);
            hists_->Create(label+"h_TP_"+s+"pt", hist_bins_, min_mass_, max_mass_);
            hists_->Create(label+"h_TF_"+s+"pt", hist_bins_, min_mass_, max_mass_);
        }
        for(unsigned i=0; i<eta_bins_.size()-1; i++)
        {
            std::string s=boost::lexical_cast<std::string>(i+1);
            hists_->Create(label+"h_TP_"+s+"eta", hist_bins_, min_mass_, max_mass_);
            hists_->Create(label+"h_TF_"+s+"eta", hist_bins_, min_mass_, max_mass_);
        }
        for(unsigned i=0; i<vtx_bins_.size()-1; i++)
        {
            std::string s=boost::lexical_cast<std::string>(i+1);
            hists_->Create(label+"h_TP_"+s+"vtx", hist_bins_, min_mass_, max_mass_);
            hists_->Create(label+"h_TF_"+s+"vtx", hist_bins_, min_mass_, max_mass_);
        }
    }
    if(mode_==0) hists_->Create("h_PUWeights", 200, 0, 10);
   
    outFile.open((output_name_+"_"+dataormc+".txt").c_str());


    return 0;
  }

  int MuonTagAndProbe::Execute(TreeEvent *event) {
    
    EventInfo const* eventInfo = event->GetPtr<EventInfo>("eventInfo");
    std::vector<PileupInfo *> puInfo;
    if(!data_) { puInfo= event->GetPtrVec<PileupInfo>("pileupInfo"); }
    int run = eventInfo->run();                 

    std::string tap_obj_label;
    std::string tag_filter_muA;
    std::string tag_filter_muB;
    std::string probe_filter_mu;
    std::string trigger_name_mu;
    std::string mutau_obj_label;
    std::string mutau_filter;
    std::string mutau_filter_A;
    std::string mutau_filter_B;

    //Set trigger filter labels
    //Set etau trigger names if measuring trigger efficiencies.
    //In 2011 data the triggers used are split by run range. To make a correct efficiency measurement
    //the user must ensure only events within the run range are used.
    //for MC the trigger labels are fixed by "era".

    // 2011 Triggers
    if ( (data_ && run >= 160404 && run <= 163869) /*|| (!data_ && era_=="2011AMay10")*/)
    {
        mutau_obj_label = "triggerObjectsIsoMu12LooseTau10";
        mutau_filter = "hltSingleMuIsoL3IsoFiltered12";
    }
    if ( (data_ && run >= 165088 && run <= 173198)|| (!data_ && (era_=="2011APrompt6" || era_=="2011BPrompt1")) ) 
    {
        mutau_obj_label = "triggerObjectsIsoMu15LooseTau15";
        mutau_filter = "hltSingleMuIsoL3IsoFiltered15";
    }
    if ( (data_ && run >= 173236 && run <= 180252)) 
    {
        mutau_obj_label = "triggerObjectsIsoMu15LooseTau20";
        mutau_filter = "hltSingleMuIsoL1s14L3IsoFiltered15eta2p1";
    }
    // 2012 Triggers
    if ((data_ && run >= 190456 && run <= 193751) || (!data_ && era_=="2012A"))
    {
        mutau_obj_label = "triggerObjectsIsoMu18LooseTau20";
        mutau_filter = "hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10";
    }
    if ((data_ && run >= 193752/* && run <= xxxxx*/) ||
            (!data_ && (era_=="2012B"||era_=="2012C"||era_=="2012D" ) ))
    {
        mutau_obj_label = "triggerObjectsIsoMu17LooseTau20";
        mutau_filter = "hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15";
    }
    if(mode_==4 && data_)
    {    
        mutau_obj_label = "triggerObjectsIsoMu8LooseTau20L1ETM26";
        mutau_filter = "hltL3crIsoL1sMu7Eta2p1L1f0L2f7QL3f8QL3crIsoRhoFiltered0p15";
    }
    if(mode_==4 && !data_)
    {    
        mutau_obj_label = "triggerObjectsMu8";
        mutau_filter = "hltL3fL1sMu3L3Filtered8";
    }
    if(mode_==5)
    {    
        mutau_obj_label = "triggerObjectsIsoMu24";
        mutau_filter_A="hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f24QL3crIsoFiltered10";
        mutau_filter_B="hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15";
        mutau_filter="";
    }


    //set tag and probe trigger names and filter labels
    if(data_ && run >= 190456)
    {
        trigger_name_mu="HLT_IsoMu24_eta2p1_v";
        tag_filter_muA="hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f24QL3crIsoFiltered10";
        tag_filter_muB="hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15";
        tap_obj_label="triggerObjectsIsoMu24";
    } 
    else if(data_ && run >= 160329 && run <= 163869)
    {
        trigger_name_mu="HLT_IsoMu24_v";
        tag_filter_muA="hltSingleMuL2QualIsoL3IsoFiltered24";
        tag_filter_muB="hltSingleMuIsoL3IsoFiltered24";
        tap_obj_label="triggerObjectsIsoMu24";
    }
    else if(data_ && run >= 165071 && run <= 168437)
    {
        trigger_name_mu="HLT_IsoMu24_v";
        tag_filter_muA="hltSingleMuL2QualIsoL3IsoFiltered24";
        tag_filter_muB="hltSingleMuIsoL3IsoFiltered24";
        tap_obj_label="triggerObjectsIsoMu24";
    }
    else if(data_ && run >= 170053 && run <= 172619)
    {
        trigger_name_mu="HLT_IsoMu24_v";
        tag_filter_muA="hltSingleMuL2QualIsoL3IsoFiltered24";
        tag_filter_muB="hltSingleMuIsoL3IsoFiltered24";
        tap_obj_label="triggerObjectsIsoMu24";
    }
    else if(data_ && run >= 172620 && run <= 175770)
    {
        trigger_name_mu="HLT_IsoMu24_v";
        tag_filter_muA="hltSingleMuL2QualIsoL3IsoFiltered24";
        tag_filter_muB="hltSingleMuIsoL3IsoFiltered24";
        tap_obj_label="triggerObjectsIsoMu24";
    }
    else if(data_ && run >= 175832 && run <= 180296)
    {
        trigger_name_mu="HLT_IsoMu24_v";
        tag_filter_muA="hltSingleMuL2QualIsoL3IsoFiltered24";
        tag_filter_muB="hltSingleMuIsoL3IsoFiltered24";
        tap_obj_label="triggerObjectsIsoMu24";
    }

    std::vector<Muon *> & muons = event->GetPtrVec<Muon>("muonsPFlow");
    std::vector<TriggerObject *> objs;
    if(data_) objs = event->GetPtrVec<TriggerObject>(tap_obj_label);
    std::vector<TriggerObject *> mutau_objs = event->GetPtrVec<TriggerObject>(mutau_obj_label);
    std::vector<TriggerPath *> triggerPathPtrVec;
    if(data_) triggerPathPtrVec = event->GetPtrVec<TriggerPath>("triggerPathPtrVec" , "triggerPaths");
    double vtx = eventInfo->good_vertices();
   
   //Calculate PU weight for each event (if MC).

    double w=1.0;
    if(!data_)
    {
        w = eventInfo->weight("pileup");
    }
    bool trigger=false;
    // int prescalenum;
    
    //See if trigger has fired (if data).

    if(data_)
    {
        for(unsigned k=0; k<triggerPathPtrVec.size(); k++)
        {
            std::string name=(triggerPathPtrVec[k])->name();
            if(name.find(trigger_name_mu)!=name.npos)
            {
                trigger=true;
       //         prescalenum=triggerPathPtrVec[k]->prescale();
            }
        }
    }
    else trigger=true;
    
    std::vector<Muon *> tag_vector; 
    std::vector<Muon *> probe_vector; 
    bool good_tag=false;
    //Make vectors of tag and probe candidates.
    
    if(muons.size()>1 && trigger && ((data_ && run>=run_low_ && run<=run_high_) || !data_) )
    {
   //     prescales[run]+=prescalenum; 
     //   nrun[run]+=1; 
        for(unsigned e1=0; e1<muons.size(); e1++)
        {
            if( tag_predicate_((muons[e1]))
                    && (!data_ || (data_ && (IsFilterMatched((muons)[e1], objs, tag_filter_muA, 0.5)
                         || IsFilterMatched((muons)[e1], objs, tag_filter_muB, 0.5))) ))
            { 
                tag_vector.push_back((muons)[e1]);
                good_tag=true;
            } 
            if( probe_predicate_((muons[e1])) 
                    && (!data_ || !probe_match || (data_ && IsFilterMatched((muons)[e1], objs, probe_filter_mu, 0.5)))
                    && (!(mode_==4) || hasL1MET(event, mutau_objs)))
            {
                probe_vector.push_back((muons)[e1]);
            }
        }
    }
  
    if(good_tag)
    {
        hists_->Fill("h_PUWeights",w);
    }

    //Save good tag/probe pairs.
    bool flag=false; 
    std::vector<std::pair <Muon *, Muon *> > pairs;
    for(unsigned i=0; i<tag_vector.size(); i++)
    {
        for(unsigned j=0; j<probe_vector.size(); j++)
        {
            if(((tag_vector[i]->vector()) + (probe_vector[j]->vector())).M()>60
                && ((tag_vector[i]->vector()) + (probe_vector[j]->vector())).M()<120
                && tag_vector[i]->charge()*probe_vector[j]->charge()<0)
            { 
                std::pair<Muon*,Muon*> p(tag_vector[i], probe_vector[j]);
                pairs.push_back(p);
                flag=true;
            }
        }
    }

    double mass;
    std::string label;
    if(mode_==0 || mode_==10) label="id_";
    if(mode_==1 || mode_==11) label="iso_";
    if(mode_==2 || mode_==12) label="idiso_";
    if(mode_==3) label="";
    if(mode_==4) label="";
    if(mode_==5) label="";
    
    
    //Fill passing and failing histograms.
    if(flag)
    {
        for(unsigned k_final=0; k_final<pairs.size(); k_final++)
        { 
            mass=((pairs[k_final].first)->vector()+(pairs[k_final].second)->vector()).M();
            for(unsigned i=0; i<pt_bins_.size(); i++)
            {
                std::string s=boost::lexical_cast<std::string>(i+1);
                if((!(mode_==3) && !(mode_==4) && !(mode_==5) && passprobe_predicate_(pairs[k_final].second)) ||
                        ((mode_==3) && IsFilterMatched(pairs[k_final].second,mutau_objs,mutau_filter,0.5 )) ||
                        ((mode_==4) && IsFilterMatched(pairs[k_final].second,mutau_objs,mutau_filter,0.5 ) && PassL1Lepton(event, mutau_objs, pairs[k_final].second)) 
                        || ((mode_==5) &&
                        (IsFilterMatched(pairs[k_final].second,mutau_objs,mutau_filter_A,0.5 ) || IsFilterMatched(pairs[k_final].second,mutau_objs,mutau_filter_B,0.5 ))  ) )
                {
                    //if(run==206542 && eventInfo->event() > 350000000 && eventInfo->event()< 400000000 ) std::cout << run << ":  " << eventInfo->event() << std::endl;
                    if(mode_==0 || mode_==1 || mode_==2 || mode_==3 || mode_==4 || mode_==5)
                    {
                        if(!split_pm_eta_)
                        {
                            if(fabs((pairs[k_final].second)->eta()) < eta_bins_[0] ) 
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TP_"+s+"B"), mass, w);
                                }
                            }
                            if(fabs((pairs[k_final].second)->eta()) > eta_bins_[0] && 
                                fabs((pairs[k_final].second)->eta()) <= eta_bins_[1] )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TP_"+s+"E"), mass, w);
                                }
                            }
                            if(fabs((pairs[k_final].second)->eta()) >= eta_bins_[1] )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TP_"+s+"Eb"), mass, w);
                                }
                            }
                        } 
                        else if(split_pm_eta_)
                        {
                            if((pairs[k_final].second)->eta()>0 &&
                                (pairs[k_final].second)->eta() <= eta_bins_[0] ) 
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TP_"+s+"Bplus"), mass, w);
                                }
                            }
                            if((pairs[k_final].second)->eta() > eta_bins_[0] && 
                                (pairs[k_final].second)->eta() <= eta_bins_[1] )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TP_"+s+"Eplus"), mass, w);
                                }
                            }
                            if((pairs[k_final].second)->eta() >= eta_bins_[1] )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TP_"+s+"Ebplus"), mass, w);
                                }
                            }
                            if((pairs[k_final].second)->eta()<0 &&
                                (pairs[k_final].second)->eta() >= (eta_bins_[0])*(-1) ) 
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TP_"+s+"Bminus"), mass, w);
                                }
                            }
                            if((pairs[k_final].second)->eta() < (eta_bins_[0])*(-1) && 
                                (pairs[k_final].second)->eta() >= (eta_bins_[1])*(-1) )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TP_"+s+"Eminus"), mass, w);
                                }
                            }
                            if((pairs[k_final].second)->eta() <= (eta_bins_[1])*(-1) )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TP_"+s+"Ebminus"), mass, w);
                                }
                            }
                        } 

                    }
                    if(mode_==10 || mode_==11 || mode_==12)
                    {
                        if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                        {
                            hists_->Fill((label+"h_TP_"+s+"pt"), mass, w);
                        } 
                    }
                }
                if (((!(mode_==3) && !(mode_==4)) && !(mode_==5) && !passprobe_predicate_(pairs[k_final].second))||
                        ((mode_==3) && !IsFilterMatched(pairs[k_final].second,mutau_objs,mutau_filter,0.5 )) ||
                        ((mode_==4) && (!IsFilterMatched(pairs[k_final].second,mutau_objs,mutau_filter,0.5 ) || !PassL1Lepton(event, mutau_objs, pairs[k_final].second))) 
                        || ((mode_==5) &&
                        (!IsFilterMatched(pairs[k_final].second,mutau_objs,mutau_filter_A,0.5 ) && !IsFilterMatched(pairs[k_final].second,mutau_objs,mutau_filter_B,0.5 ))  ) )
                {
                    //if(run==206542 && eventInfo->event() > 350000000 && eventInfo->event()< 400000000 ) std::cout << run << ":  " << eventInfo->event() << std::endl;
                    if(mode_==0 || mode_==1 || mode_==2 || mode_==3 || mode_==4 || mode_==5)
                    {
                        if(!split_pm_eta_)
                        {
                            if(fabs((pairs[k_final].second)->eta()) < eta_bins_[0] ) 
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TF_"+s+"B"), mass, w);
                                }
                            }
                            if(fabs((pairs[k_final].second)->eta()) > eta_bins_[0] && 
                                fabs((pairs[k_final].second)->eta()) <= eta_bins_[1] )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TF_"+s+"E"), mass, w);
                                }
                            }
                            if(fabs((pairs[k_final].second)->eta()) >= eta_bins_[1] )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TF_"+s+"Eb"), mass, w);
                                }
                            }
                        }
                        else if(split_pm_eta_)
                        {
                            if((pairs[k_final].second)->eta()>0 &&
                                (pairs[k_final].second)->eta() <= eta_bins_[0] ) 
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TF_"+s+"Bplus"), mass, w);
                                }
                            }
                            if((pairs[k_final].second)->eta() > eta_bins_[0] && 
                                (pairs[k_final].second)->eta() <= eta_bins_[1] )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TF_"+s+"Eplus"), mass, w);
                                }
                            }
                            if((pairs[k_final].second)->eta() >= eta_bins_[1] )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TF_"+s+"Ebplus"), mass, w);
                                }
                            }
                            if((pairs[k_final].second)->eta()<0 &&
                                (pairs[k_final].second)->eta() >= (eta_bins_[0])*(-1) ) 
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TF_"+s+"Bminus"), mass, w);
                                }
                            }
                            if((pairs[k_final].second)->eta() < (eta_bins_[0])*(-1) && 
                                (pairs[k_final].second)->eta() >= (eta_bins_[1])*(-1) )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TF_"+s+"Eminus"), mass, w);
                                }
                            }
                            if((pairs[k_final].second)->eta() <= (eta_bins_[1])*(-1) )
                            {
                                if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                        && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                                {
                                    hists_->Fill((label+"h_TF_"+s+"Ebminus"), mass, w);
                                }
                            }
                        }
                    }
                    if(mode_==10 || mode_==11 || mode_==12)
                    {
                        if((pairs[k_final].second)->pt()>pt_bins_[i] 
                                && (pairs[k_final].second)->pt()<=pt_bins_[i+1])
                        {
                            hists_->Fill((label+"h_TF_"+s+"pt"), mass, w);
                        } 
                    }
                }
            }
            if(mode_==10 || mode_==11 || mode_==12)
            {
                for(unsigned i=0; i<eta_bins_.size(); i++)
                {
                    std::string s=boost::lexical_cast<std::string>(i+1);
                    if(passprobe_predicate_(pairs[k_final].second) )
                    {
                        if((pairs[k_final].second)->eta()>eta_bins_[i] 
                                && (pairs[k_final].second)->eta()<=eta_bins_[i+1])
                        {
                            hists_->Fill((label+"h_TP_"+s+"eta"), mass, w);
                        } 
                    }
                    else
                    {
                        if((pairs[k_final].second)->eta()>eta_bins_[i] 
                                && (pairs[k_final].second)->eta()<=eta_bins_[i+1])
                        {
                            hists_->Fill((label+"h_TF_"+s+"eta"), mass, w);
                        } 
                    }
                }
                for(unsigned i=0; i<vtx_bins_.size(); i++)
                {
                    std::string s=boost::lexical_cast<std::string>(i+1);
                    if(passprobe_predicate_(pairs[k_final].second) )
                    {
                        if(vtx>vtx_bins_[i] && vtx<=vtx_bins_[i+1])
                        {
                            hists_->Fill((label+"h_TP_"+s+"vtx"), mass, w);
                        } 
                    }
                    else
                    {
                        if(vtx>vtx_bins_[i] && vtx<=vtx_bins_[i+1])
                        {
                            hists_->Fill((label+"h_TF_"+s+"vtx"), mass, w);
                        } 
                    }
                }
            }
        }
    }

    return 0;
  }
  int MuonTagAndProbe::PostAnalysis() {
    std::string label;
    if(mode_==0 || mode_==10) label="id_";
    if(mode_==1 || mode_==11) label="iso_";
    if(mode_==2 || mode_==12) label="idiso_";
    if(mode_==3) label="";
    if(mode_==4) label="";
    if(mode_==5) label="";
     
    if(mode_==0 || mode_==1 || mode_==2 || mode_==3 || mode_==4 || mode_==5)
    {
         std::cout << "====================="+label+"====================" << std::endl;
         for(unsigned i=0; i<pt_bins_.size()-1; i++)
         {
             std::string s=boost::lexical_cast<std::string>(i+1);
             if(!split_pm_eta_)
             {
                 outFile << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " |eta|<0.8: " << ((hists_->Get_Histo(label+"h_TP_"+s+"B")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"B")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"B")->Integral())) << std::endl;
                 outFile << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " 0.8<|eta|<1.2: " << ((hists_->Get_Histo(label+"h_TP_"+s+"E")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"E")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"E")->Integral())) << std::endl;
                 outFile << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " 1.2<|eta|: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Eb")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Eb")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Eb")->Integral())) << std::endl;

                 std::cout << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " |eta|<0.8: " << ((hists_->Get_Histo(label+"h_TP_"+s+"B")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"B")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"B")->Integral())) << std::endl;
                 std::cout << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " 0.8<|eta|<1.2: " << ((hists_->Get_Histo(label+"h_TP_"+s+"E")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"E")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"E")->Integral())) << std::endl;
                 std::cout << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " 1.2<|eta|: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Eb")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Eb")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Eb")->Integral())) << std::endl;
            }
            else
            {
                 outFile << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " 0<eta<0.8: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Bplus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Bplus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Bplus")->Integral())) << std::endl;
                 outFile << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " 0.8<eta<1.2: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Eplus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Eplus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Eplus")->Integral())) << std::endl;
                 outFile << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " 1.2<eta: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Ebplus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Ebplus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Ebplus")->Integral())) << std::endl;
                 outFile << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " -0.8<eta<0: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Bminus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Bminus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Bminus")->Integral())) << std::endl;
                 outFile << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " -1.2<eta<-0.8: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Eminus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Eminus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Eminus")->Integral())) << std::endl;
                 outFile << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " eta<-1.2: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Ebminus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Ebminus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Ebminus")->Integral())) << std::endl;
            
            
                 std::cout << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " 0<eta<0.8: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Bplus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Bplus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Bplus")->Integral())) << std::endl;
                 std::cout << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " 0.8<eta<1.2: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Eplus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Eplus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Eplus")->Integral())) << std::endl;
                 std::cout << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " 1.2<eta: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Ebplus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Ebplus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Ebplus")->Integral())) << std::endl;
                 std::cout << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " -0.8<eta<0: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Bminus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Bminus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Bminus")->Integral())) << std::endl;
                 std::cout << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " -1.2<eta<-0.8: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Eminus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Eminus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Eminus")->Integral())) << std::endl;
                 std::cout << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " eta<-1.2: " << ((hists_->Get_Histo(label+"h_TP_"+s+"Ebminus")->Integral())) /
                        ( (hists_->Get_Histo(label+"h_TP_"+s+"Ebminus")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"Ebminus")->Integral())) << std::endl;
            }
         }
             std::cout << "================================================" << std::endl;
    } 
    if(mode_==10 || mode_==11 || mode_==12)
    {
         std::cout << "====================="+label+"====================" << std::endl;
         for(unsigned i=0; i<pt_bins_.size()-1; i++)
         {
             std::string s=boost::lexical_cast<std::string>(i+1);
             outFile << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " all eta: " << ((hists_->Get_Histo(label+"h_TP_"+s+"pt")->Integral())) /
                    ( (hists_->Get_Histo(label+"h_TP_"+s+"pt")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"pt")->Integral())) << std::endl;

             std::cout << pt_bins_[i] << " < pt < " << pt_bins_[i+1] << " all eta: " << ((hists_->Get_Histo(label+"h_TP_"+s+"pt")->Integral())) /
                    ( (hists_->Get_Histo(label+"h_TP_"+s+"pt")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"pt")->Integral())) << std::endl;
         }
             std::cout << "================================================" << std::endl;
         for(unsigned i=0; i<eta_bins_.size()-1; i++)
         {
             std::string s=boost::lexical_cast<std::string>(i+1);
             outFile << eta_bins_[i] << " < eta < " << eta_bins_[i+1] << " all pt: " << ((hists_->Get_Histo(label+"h_TP_"+s+"eta")->Integral())) /
                    ( (hists_->Get_Histo(label+"h_TP_"+s+"eta")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"eta")->Integral())) << std::endl;

             std::cout << eta_bins_[i] << " < eta < " << eta_bins_[i+1] << " all pt: " << ((hists_->Get_Histo(label+"h_TP_"+s+"eta")->Integral())) /
                    ( (hists_->Get_Histo(label+"h_TP_"+s+"eta")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"eta")->Integral())) << std::endl;
         }
             std::cout << "================================================" << std::endl;
         for(unsigned i=0; i<vtx_bins_.size()-1; i++)
         {
             std::string s=boost::lexical_cast<std::string>(i+1);
             outFile << vtx_bins_[i] << " < vtx < " << vtx_bins_[i+1] << " all pt,eta: " << ((hists_->Get_Histo(label+"h_TP_"+s+"vtx")->Integral())) /
                    ( (hists_->Get_Histo(label+"h_TP_"+s+"vtx")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"vtx")->Integral())) << std::endl;

             std::cout << vtx_bins_[i] << " < vtx < " << vtx_bins_[i+1] << " all pt,eta: " << ((hists_->Get_Histo(label+"h_TP_"+s+"vtx")->Integral())) /
                    ( (hists_->Get_Histo(label+"h_TP_"+s+"vtx")->Integral()) +(hists_->Get_Histo(label+"h_TF_"+s+"vtx")->Integral())) << std::endl;
         }
             std::cout << "================================================" << std::endl;
     } 
//     outFile.close();  
    return 0;
  }

  void MuonTagAndProbe::PrintInfo() {
    ;
  }
}
