import ROOT
import glob
import sys
# import json
from array import array
import UserCode.ICHiggsTauTau.analysis as analysis

ROOT.RooWorkspace.imp = getattr(ROOT.RooWorkspace, 'import')
ROOT.TH1.AddDirectory(0)

bin_cfgs = [
    {
        'name': 'ID_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p',
        'probe': 'id_p',
        'binvar_x': 'pt_p',
        'bins_x': [10., 15., 20., 25., 30., 40., 50., 60., 80., 100., 200., 250., 300., 500., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.5, 2.1, 2.4]
    },
    {
        'name': 'IsoMedium_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && id_p',
        'probe': 'isoMedium_p > 0.5',
        'binvar_x': 'pt_p',
        'bins_x': [10., 15., 20., 25., 30., 40., 50., 60., 80., 100., 200., 250., 300., 500., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.5, 2.1, 2.4]
    },
    {
        'name': 'AIsoMedium_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && id_p',
        'probe': 'isoMedium_p < 0.5',
        'binvar_x': 'pt_p',
        'bins_x': [10., 15., 20., 25., 30., 40., 50., 60., 80., 100., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.5, 2.1, 2.4]
    },
    {
        'name': 'Trg120_IsoMedium_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && id_p && isoMedium_p > 0.5',
        'probe': 'trg_p_PFTau120',
        'binvar_x': 'pt_p',
        'bins_x': [50., 100., 110., 120., 130., 140., 150., 160., 170., 180., 190., 200., 250., 300., 500., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.5, 2.1, 2.4]
    },
    {
        'name': 'Trg140_IsoMedium_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && id_p && isoMedium_p > 0.5',
        'probe': 'trg_p_PFTau140',
        'binvar_x': 'pt_p',
        'bins_x': [50., 100., 110., 120., 130., 140., 150., 160., 170., 180., 190., 200., 250., 300., 500., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.5, 2.1, 2.4]
    },
    {
        'name': 'ID_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p',
        'probe': 'id_p',
        'binvar_x': 'pt_p',
        'bins_x': [10., 15., 20., 25., 30., 40., 50., 60., 80., 100., 200., 250., 300., 500., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.5, 2.1, 2.4]
    },
    {
        'name': 'IsoTight_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && id_p',
        'probe': 'isoTight_p > 0.5',
        'binvar_x': 'pt_p',
        'bins_x': [10., 15., 20., 25., 30., 40., 50., 60., 80., 100., 200., 250., 300., 500., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.5, 2.1, 2.4]
    },
    {
        'name': 'AIsoTight_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && id_p',
        'probe': 'isoTight_p < 0.5',
        'binvar_x': 'pt_p',
        'bins_x': [10., 15., 20., 25., 30., 40., 50., 60., 80., 100., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.5, 2.1, 2.4]
    },
    {
        'name': 'Trg120_IsoTight_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && id_p && isoTight_p > 0.5',
        'probe': 'trg_p_PFTau120',
        'binvar_x': 'pt_p',
        'bins_x': [50., 100., 110., 120., 130., 140., 150., 160., 170., 180., 190., 200., 250., 300., 500., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.5, 2.1, 2.4]
    },
    {
        'name': 'Trg140_IsoTight_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && id_p && isoTight_p > 0.5',
        'probe': 'trg_p_PFTau140',
        'binvar_x': 'pt_p',
        'bins_x': [50., 100., 110., 120., 130., 140., 150., 160., 170., 180., 190., 200., 250., 300., 500., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.5, 2.1, 2.4]
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
    'DYplusSUSY': analysis.TTreeEvaluator('inclusive/GenTau', 'output/HTT2016Studies_genTau/DYplusSUSY.root')
    #'DYJetsToLL': analysis.TTreeEvaluator('inclusive/GenTau', 'output/HTT2016Studies_genTau/DYJetsToLLM50.root')#,
    #'Data': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_v16_2/ZmmTP/SingleMuon.root') #,
    #'DataB': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonB.root'),
    #'DataC': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonC.root'),
    #'DataD': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonD.root'),
    #'DataE': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonE.root'),
    #'DataF': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonF.root'),
    #'DataG': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonG.root'),
    #'DataH': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonH.root')
}


# sys.exit(0)

for sample in trees:
    outfile = ROOT.TFile('genTau_truth/genTau_%s.root' % sample, 'RECREATE')
    hists = trees[sample].Draw(drawlist, compiled=True)

    i = 0
    for cfg in bin_cfgs:
        wsp = ROOT.RooWorkspace('wsp_'+cfg['name'], '')
        var = wsp.factory('pt_p[99,10,1000]')

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
