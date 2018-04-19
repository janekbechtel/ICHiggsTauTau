import ROOT
import glob
import sys
import numpy as np
# import json
from array import array
import UserCode.ICHiggsTauTau.analysis as analysis

ROOT.RooWorkspace.imp = getattr(ROOT.RooWorkspace, 'import')
ROOT.TH1.AddDirectory(0)


bin_cfgs = [
    {
        'name': 'muon_Selection_ID',
        'var': 'm_ll(50,75,125)',
        'tag': 'muon_p && trg_t_IsoMu22',
        'probe': 'id_p',
        'binvar_x': 'pt_p',
        'bins_x': [8,15,20,25,30,40,50,100,1000],
        'binvar_y': 'eta_p',
        'bins_y': np.arange(-2.4,2.4,0.1)
    },
    {
        'name': 'muon_Selection_EmbeddedID',
        'var': 'm_ll(50,75,125)',
        'tag': 'muon_p && trg_t_IsoMu22',
        'probe': 'idEmb_p',
        'binvar_x': 'pt_p',
        'bins_x': [8,15,20,25,30,40,50,100,1000],
        'binvar_y': 'eta_p',
        'bins_y': np.arange(-2.4,2.4,0.1)
    },
    {
        'name': 'muon_Selection_VVLIso',
        'var': 'm_ll(50,75,125)',
        'tag': 'muon_p && trg_t_IsoMu22 && id_p',
        'probe': 'iso_p < 0.4',
        'binvar_x': 'pt_p',
        'bins_x': [8,15,20,25,30,40,50,100,1000],
        'binvar_y': 'eta_p',
        'bins_y': np.arange(-2.4,2.4,0.1)
    },
    {
        'name': 'muon_Selection_Iso',
        'var': 'm_ll(50,75,125)',
        'tag': 'muon_p && trg_t_IsoMu22 && id_p',
        'probe': 'iso_p < 0.15',
        'binvar_x': 'pt_p',
        'bins_x': [8,15,20,25,30,40,50,100,1000],
        'binvar_y': 'eta_p',
        'bins_y': np.arange(-2.4,2.4,0.1)
    },
    {
        'name': 'muon_Selection_Trg',
        'var': 'm_ll(50,75,125)',
        'tag': 'muon_p && trg_t_IsoMu22 && id_p && iso_p < 0.15',
        'probe': 'trg_p_IsoMu22',
        'binvar_x': 'pt_p',
        'bins_x': [8,15,20,25,30,40,50,100,1000],
        'binvar_y': 'eta_p',
        'bins_y': np.arange(-2.4,2.4,0.1)
    }
]

drawlist = []
andable = set()

for cfg in bin_cfgs:
    cfg['hist'] = ROOT.TH2D(cfg['name'], cfg['name'],
                            len(cfg['bins_x'])-1, array('d', cfg['bins_x']),
                            len(cfg['bins_y'])-1, array('d', cfg['bins_y']))
    hist = cfg['hist']
    hist.GetXaxis().SetTitle(cfg['binvar_x'])
    hist.GetYaxis().SetTitle(cfg['binvar_y'])

    cfg['bins'] = []

    for i in xrange(1, hist.GetNbinsX()+1):
        for j in xrange(1, hist.GetNbinsY()+1):
            cfg['bins'].append('%s>=%g && %s<%g && %s>=%g && %s<%g' % (
                cfg['binvar_x'], hist.GetXaxis().GetBinLowEdge(i),
                cfg['binvar_x'], hist.GetXaxis().GetBinUpEdge(i),
                cfg['binvar_y'], hist.GetYaxis().GetBinLowEdge(j),
                cfg['binvar_y'], hist.GetYaxis().GetBinUpEdge(j),
                ))
            andable.add('%s>=%g' % (cfg['binvar_x'], hist.GetXaxis().GetBinLowEdge(i)))
            andable.add('%s<%g' % (cfg['binvar_x'], hist.GetXaxis().GetBinUpEdge(i)))
            andable.add('%s>=%g' % (cfg['binvar_y'], hist.GetYaxis().GetBinLowEdge(j)))
            andable.add('%s<%g' % (cfg['binvar_y'], hist.GetYaxis().GetBinUpEdge(j)))

    for b in cfg['bins']:
        drawlist.append((cfg['var'], '((%s) && !(%s) && (%s)) * wt' % (b, cfg['probe'], cfg['tag'])))
        drawlist.append((cfg['var'], '((%s) && (%s) && (%s)) * wt' % (b, cfg['probe'], cfg['tag'])))
        andable.add(cfg['probe'])
        andable.add(cfg['tag'])


trees = {
    #~ 'EmbeddingData': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Truth1/GenMuon/EmbeddingData.root') #,
    #~ 'Embedding': analysis.TTreeEvaluator('inclusive/ZmmTP', 'tp_files/EmbeddingMuMu.root'),
    'Data': analysis.TTreeEvaluator('mm_nominal/ZmmTP', 'tp_files/DoubleMuon.root')
    #~ 'MC': analysis.TTreeEvaluator('inclusive/ZmmTP', 'tp_files/DYToLL.root')
}


# sys.exit(0)
print trees
for sample in trees:
    outfile = ROOT.TFile('output/ZmmTP_%s.root' % sample, 'RECREATE')
    hists = trees[sample].Draw(drawlist, compiled=True)

    i = 0
    for cfg in bin_cfgs:
        wsp = ROOT.RooWorkspace('wsp_'+cfg['name'], '')
        var = wsp.factory('m_ll[100,75,125]')

        outfile.cd()
        outfile.mkdir(cfg['name'])
        ROOT.gDirectory.cd(cfg['name'])

        for b in cfg['bins']:
            hists[2*i].SetName(b+':fail')
            hists[2*i+1].SetName(b+':pass')
            hists[2*i].Write()
            hists[2*i+1].Write()
            dat = wsp.imp(ROOT.RooDataHist(b, '', ROOT.RooArgList(var),
                          ROOT.RooFit.Index(wsp.factory('cat[fail,pass]')),
                          ROOT.RooFit.Import('fail', hists[2*i]),
                          ROOT.RooFit.Import('pass', hists[2*i+1]))
                          )
            i += 1
        outfile.cd()
        wsp.Write()
        cfg['hist'].Write()
        wsp.Delete()

    outfile.Close()
