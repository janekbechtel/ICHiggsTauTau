import argparse
import ROOT
import os
import CombineHarvester.CombineTools.plotting as plot

ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(ROOT.kTRUE)
ROOT.RooMsgService.instance().setGlobalKillBelow(ROOT.RooFit.WARNING)

parser = argparse.ArgumentParser()
parser.add_argument('input')
parser.add_argument('--sig-model', default='DoubleVCorr')
parser.add_argument('--bkg-model', default='Exponential')
parser.add_argument('--title', default='Muon ID Efficiency')
parser.add_argument('--particle', choices=['e', 'm'], default='m')
parser.add_argument('--isMC', default = False, action="store_true")
parser.add_argument('--postfix', default='')
parser.add_argument('--plot-dir', '-p', default='./')
parser.add_argument('--bin-replace', default=None) #(100,2.3,80,2.3)
args = parser.parse_args()


plot.ModTDRStyle(width=1200, l=0.35, r=0.15)
# Apparently I don't need to do this...
# ROOT.gSystem.Load('lib/libICHiggsTauTau.so')


if args.plot_dir != '':
    os.system('mkdir -p %s' % args.plot_dir)

ROOT.RooWorkspace.imp = getattr(ROOT.RooWorkspace, 'import')
ROOT.TH1.AddDirectory(0)

filename = args.input.split(':')[0]
name = args.input.split(':')[1]

infile = ROOT.TFile(filename)
wsp = infile.Get('wsp_'+name)

pdf_args = [ ]
nparams = 1
if args.sig_model == 'DoubleVCorr':
    nparams = 15
    pdf_args.extend(
            [
                "Voigtian::signal1Pass(pt_p, mean1[90,80,100], width[2.495], sigma1[2,1,3])",
                "Voigtian::signal2Pass(pt_p, mean2[90,80,100], width,        sigma2[4,2,10])",
                "SUM::signalPass(vFrac[0.8,0,1]*signal1Pass, signal2Pass)",
                "Voigtian::signal1Fail(pt_p, mean1[90,80,100], width[2.495], sigma1[2,1,3])",
                "Voigtian::signal2Fail(pt_p, mean2[90,80,100], width,        sigma2[4,2,10])",
                "SUM::signalFail(vFrac[0.8,0,1]*signal1Fail, signal2Fail)",
            ]
        )
elif args.sig_model == 'DoubleVUncorr':
    nparams = 20
    pdf_args.extend(
            [
                "Voigtian::signal1Pass(pt_p, mean1p[90,80,100], widthp[2.495], sigma1p[2,1,3])",
                "Voigtian::signal2Pass(pt_p, mean2p[90,80,100], widthp,        sigma2p[4,2,10])",
                "SUM::signalPass(vFracp[0.8,0,1]*signal1Pass, signal2Pass)",
                "Voigtian::signal1Fail(pt_p, mean1f[90,80,100], widthf[2.495], sigma1f[2,1,3])",
                "Voigtian::signal2Fail(pt_p, mean2f[90,80,100], widthf,        sigma2f[4,2,10])",
                "SUM::signalFail(vFracf[0.8,0,1]*signal1Fail, signal2Fail)"
            ]
        )
else:
    raise RuntimeError('Chosen --sig-model %s not supported' % args.sig_model)

if args.bkg_model == 'Exponential':
    pdf_args.extend(
            [
                "Exponential::backgroundPass(pt_p, lp[-0.1,-1,0.1])",
                "Exponential::backgroundFail(pt_p, lf[-0.1,-1,0.1])"
            ]
        )
elif args.bkg_model == 'CMSShape':
    pdf_args.extend(
            [
                "RooCMSShape::backgroundPass(pt_p, alphaPass[70,60,90], betaPass[0.001,0,0.1], gammaPass[0.001,0,0.1], peak[90])",
                "RooCMSShape::backgroundFail(pt_p, alphaFail[70,60,90], betaFail[0.001,0,0.1], gammaFail[0.001,0,0.1], peak[90])",
            ]
        )
elif args.bkg_model == 'Chebychev':
    pdf_args.extend(
            [
                "RooChebychev::backgroundPass(pt_p, {a0p[0.25,0,0.5], a1p[-0.25,-1,0.1],a2p[0.,-0.25,0.25]})",
                "RooChebychev::backgroundFail(pt_p, {a0f[0.25,0,0.5], a1f[-0.25,-1,0.1],a2f[0.,-0.25,0.25]})",
            ]
        )
else:
    raise RuntimeError('Chosen --bkg-model %s not supported' % args.bkg_model)

for arg in pdf_args:
    wsp.factory(arg)

model_args = [
    "expr::nSignalPass('efficiency*fSigAll*numTot',efficiency[0,1], fSigAll[0.9,0,1],numTot[1,0,1e10])",
    "expr::nSignalFail('(1-efficiency)*fSigAll*numTot',efficiency,fSigAll,numTot)",
    "expr::nBkgPass('effBkg*(1-fSigAll)*numTot',effBkg[0.9,0,1],fSigAll,numTot)",
    "expr::nBkgFail('(1-effBkg)*(1-fSigAll)*numTot',effBkg,fSigAll,numTot)",
    "SUM::passing(nSignalPass*signalPass,nBkgPass*backgroundPass)",
    "SUM::failing(nSignalFail*signalFail,nBkgFail*backgroundFail)",
    "cat[fail,pass]",
    "SIMUL::model(cat,fail=failing,pass=passing)"
]
for arg in model_args:
    wsp.factory(arg)

hist = infile.Get(name)
hist_pass=hist.Clone()
hist_pass.SetName(hist.GetName()+"_pass")
hist_tot=hist.Clone()
hist_tot.SetName(hist.GetName()+"_tot")
bin_cfg = {
    'name': hist.GetName(),
    'binvar_x': hist.GetXaxis().GetTitle(),
    'binvar_y': hist.GetYaxis().GetTitle()
}

bins = []

for i in xrange(1, hist.GetNbinsX()+1):
    for j in xrange(1, hist.GetNbinsY()+1):
        bins.append((i, j,
                     hist.GetXaxis().GetBinLowEdge(i),
                     hist.GetXaxis().GetBinUpEdge(i),
                     hist.GetYaxis().GetBinLowEdge(j),
                     hist.GetYaxis().GetBinUpEdge(j)
                    ))

res = []

for b in bins:
    dat = '%s>=%g && %s<%g && %s>=%g && %s<%g' % (
            bin_cfg['binvar_x'], b[2],
            bin_cfg['binvar_x'], b[3],
            bin_cfg['binvar_y'], b[4],
            bin_cfg['binvar_y'], b[5],
            )
    label = '%s.%g_%g.%s.%g_%g' % (
            bin_cfg['binvar_x'], b[2], b[3],
            bin_cfg['binvar_y'], b[4], b[5]
            )
    label = label.replace('(', '_')
    label = label.replace(')', '_')

    # Set the initial yield and efficiency values
    yield_tot = wsp.data(dat).sumEntries()
    yield_pass = wsp.data(dat).sumEntries("cat==cat::pass")
    wsp.var("numTot").setVal(yield_tot)
    
    
    
    
    #yields=0.0
    yield_tot_err=0.0
    #print wsp.data(dat).numEntries()
    for i in range(wsp.data(dat).numEntries()):
        wsp.data(dat).get(i)
        #yields+=wsp.data(dat).weight()
        #print wsp.data(dat).weight()
        #print wsp.data(dat).weightSquared() #same as below
        yield_tot_err+=wsp.data(dat).weightError(1)*wsp.data(dat).weightError(1) #enum ErrorType 1=SumW2
    yield_tot_err=yield_tot_err**0.5
    #print yields
    #print yield_tot
    #print yield_tot_err
    
    splitData = wsp.data(dat).split(wsp.cat('cat'))
    yield_pass_err=0.0
    #print splitData.At(1).numEntries()
    for i in range(splitData.At(1).numEntries()):
        splitData.At(1).get(i)
        yield_pass_err+=splitData.At(1).weightError(1)*splitData.At(1).weightError(1)
    yield_pass_err=yield_pass_err**0.5   
    err = 0.0
    if yield_tot > 0:
        wsp.var("efficiency").setVal(yield_pass/yield_tot)
        err = ((yield_pass_err/yield_tot)**2+(yield_tot_err/yield_tot*yield_pass/yield_tot)**2)**0.5
    else:
        wsp.var("efficiency").setVal(1.5)

    # wsp.pdf("model").fitTo(wsp.data(dat),
    #                        ROOT.RooFit.Minimizer("Minuit2", "Scan"),
    #                        ROOT.RooFit.Offset(True),
    #                        ROOT.RooFit.Extended(True),
    #                        ROOT.RooFit.PrintLevel(-1))
    '''
    wsp.pdf("model").fitTo(wsp.data(dat),
                           ROOT.RooFit.Minimizer("Minuit2", "Migrad"),
                           ROOT.RooFit.Offset(True),
                           ROOT.RooFit.Extended(True),
                           ROOT.RooFit.PrintLevel(-1))

    fitres = wsp.pdf("model").fitTo(wsp.data(dat),
                                    ROOT.RooFit.Minimizer("Minuit2", "Migrad"),
                                    ROOT.RooFit.Offset(True),
                                    ROOT.RooFit.Extended(True),
                                    ROOT.RooFit.PrintLevel(-1),
                                    ROOT.RooFit.Save())
                           #ROOT.RooFit.Minos())

    fitres.Print()
    '''
    res.append((dat, wsp.var('efficiency').getVal(), err))

    
    #hist.SetBinContent(b[0], b[1], wsp.var('efficiency').getVal())
    #hist.SetBinError(b[0], b[1], err)
    hist_pass.SetBinContent(b[0], b[1], yield_pass)
    hist_pass.SetBinError(b[0], b[1], yield_pass_err)
    hist_tot.SetBinContent(b[0], b[1], yield_tot)
    hist_tot.SetBinError(b[0], b[1], yield_tot_err)
    result=0.0
    if yield_tot>0.0:
        result=yield_pass/yield_tot
    print "%i, %i: %f/%f=%f"%(b[0],b[1],yield_pass,yield_tot,result)

    canv = ROOT.TCanvas('%s' % (label), "%s" % (label))
    pad_left = ROOT.TPad('left', '', 0., 0., 0.5, 1.)
    pad_left.Draw()
    pad_right = ROOT.TPad('right', '', 0.5, 0., 1., 1.)
    pad_right.Draw()
    pads = [pad_left, pad_right]

    latex = ROOT.TLatex()
    latex.SetNDC()

    ROOT.TGaxis.SetExponentOffset(-0.08, -0.02)

    
    xframe = wsp.var("pt_p").frame(ROOT.RooFit.Title("Passing"))
    width = (wsp.var("pt_p").getMax() - wsp.var("pt_p").getMin()) / splitData.At(1).numEntries()
    splitData.At(1).plotOn(xframe, ROOT.RooFit.Name("DataPass"))
    '''
    wsp.pdf("passing").plotOn(xframe,
                              ROOT.RooFit.Slice(wsp.cat('cat'), "pass"),
                              ROOT.RooFit.LineColor(ROOT.kBlue),
                              ROOT.RooFit.Name("AllPass"))
    wsp.pdf("passing").plotOn(xframe,
                              ROOT.RooFit.Slice(wsp.cat('cat'), "pass"),
                              ROOT.RooFit.Components('backgroundPass'),
                              ROOT.RooFit.LineStyle(ROOT.kDashed),
                              ROOT.RooFit.LineColor(ROOT.kBlue),
                              ROOT.RooFit.Name("BkgPass"))
    '''
    pads[0].cd()
    xframe.Draw()
    
    axis = plot.GetAxisHist(pads[0])
    #plot.Set(axis.GetXaxis().SetTitle('m_{tag-probe} (GeV)'))
    if args.particle == 'e':
        plot.Set(axis.GetXaxis().SetTitle('p_{t}(e) (GeV)'))
    else:
        plot.Set(axis.GetXaxis().SetTitle('p_{t}(#mu) (GeV)'))
    plot.Set(axis.GetYaxis().SetTitle('Events / %g GeV' % width))
    #plot.DrawTitle(pads[0], 'Pass Region', 1)
    plot.DrawTitle(pads[0], args.title, 1)
    
    latex.SetTextSize(0.035)
    #latex.DrawLatex(0.5, 0.89, args.title)
    #latex.DrawLatex(0.5, 0.84, 'p_{T}: [%g, %g] GeV #eta: [%g, %g]' % (b[2], b[3], b[4], b[5]))
    font = latex.GetTextFont()
    latex.DrawLatex(0.2, 0.9, 'pass region')
    latex.SetTextFont(42)
    #latex.DrawLatex(0.63, 0.75, '#chi^{2} = %.2f' % (xframe.chiSquare("AllPass", "DataPass", nparams)))
    latex.DrawLatex(0.63, 0.7, '#varepsilon = %.4f #pm %.4f' % (wsp.var('efficiency').getVal(), wsp.var('efficiency').getError()))
    ROOT.gStyle.SetLegendBorderSize(1)
    legend1 = ROOT.TLegend(0.6, 0.8, 0.925, 0.939)
    legend1.AddEntry(xframe.findObject("DataPass"), "data", "ep")
    #legend1.AddEntry(xframe.findObject("AllPass"), "Z #rightarrow #mu#mu + BG", "l")
    #legend1.AddEntry(xframe.findObject("BkgPass"), "BG", "l")
    legend1.Draw()

    xframe2 = wsp.var("pt_p").frame(ROOT.RooFit.Title("Failing"))
    splitData.At(0).plotOn(xframe2, ROOT.RooFit.Name("DataFail"))
    '''
    wsp.pdf("failing").plotOn(xframe2,
                              ROOT.RooFit.Slice(wsp.cat('cat'), "fail"),
                              ROOT.RooFit.LineColor(ROOT.kRed),
                              ROOT.RooFit.Name("AllFail"))
    wsp.pdf("failing").plotOn(xframe2,
                              ROOT.RooFit.Slice(wsp.cat('cat'), "fail"),
                              ROOT.RooFit.Components('backgroundFail'),
                              ROOT.RooFit.LineStyle(ROOT.kDashed),
                              ROOT.RooFit.LineColor(ROOT.kRed),
                              ROOT.RooFit.Name("BkgFail"))
    '''
    pads[1].cd()
    xframe2.Draw()
    axis = plot.GetAxisHist(pads[1])
    #plot.Set(axis.GetXaxis().SetTitle('m_{tag-probe} (GeV)'))
    if args.particle == 'e':
        plot.Set(axis.GetXaxis().SetTitle('p_{t}(e) (GeV)'))
    else:
        plot.Set(axis.GetXaxis().SetTitle('p_{t}(#mu) (GeV)'))
    plot.Set(axis.GetYaxis().SetTitle('Events / %g GeV' % width))
    plot.DrawTitle(pads[1], 'p_{T}: [%g, %g] GeV #eta: [%g, %g]' % (b[2], b[3], b[4], b[5]), 1)
    if args.isMC:
        plot.DrawTitle(pads[1], 'MC Summer16', 3)
    else:
        plot.DrawTitle(pads[1], '36.8 fb^{-1} (13 TeV)', 3)
    #plot.DrawTitle(pads[1], 'Fail Region', 1)
    #latex.DrawLatex(0.63, 0.75, '#chi^{2} = %.2f' % (xframe2.chiSquare("AllFail", "DataFail", nparams)))
    latex.SetTextFont(font)
    latex.DrawLatex(0.2, 0.9, 'fail region')
    
    
    legend2 = ROOT.TLegend(0.6, 0.8, 0.925, 0.939)
    legend2.AddEntry(xframe2.findObject("DataFail"), "data", "ep")
    #legend2.AddEntry(xframe2.findObject("AllFail"), "Z #rightarrow #mu#mu + BG", "l")
    #legend2.AddEntry(xframe2.findObject("BkgFail"), "BG", "l")
    legend2.Draw()

    canv.Print('%s/%s.png' % (args.plot_dir, canv.GetName()))
    canv.Print('%s/%s.pdf' % (args.plot_dir, canv.GetName()))
'''
if args.bin_replace is not None:
    replacements = args.bin_replace.split(':')
    for rep in replacements:
        bins = [float(x) for x in rep.split(',')]
        dest_bin_x = hist.GetXaxis().FindFixBin(bins[0])
        dest_bin_y = hist.GetYaxis().FindFixBin(bins[1])
        src_bin_x = hist.GetXaxis().FindFixBin(bins[2])
        src_bin_y = hist.GetYaxis().FindFixBin(bins[3])
        dest_val, dest_err = hist.GetBinContent(dest_bin_x, dest_bin_y), hist.GetBinError(dest_bin_x, dest_bin_y)
        src_val, src_err = hist.GetBinContent(src_bin_x, src_bin_y), hist.GetBinError(src_bin_x, src_bin_y)
        print 'Replacing content of bin %g,%g (%g +/- %g) with %g,%g (%g +/- %g)' % (dest_bin_x, dest_bin_y, dest_val, dest_err, src_bin_x, src_bin_y, src_val, src_err)
        hist.SetBinContent(dest_bin_x, dest_bin_y, src_val)
        hist.SetBinError(dest_bin_x, dest_bin_y, src_err)
'''
outfile = ROOT.TFile(filename.replace('.root', '_Fits_%s%s.root' % (name, args.postfix)), 'RECREATE')
#hist.Write()
hist_pass.Write()
hist_tot.Write()

for i in xrange(1, hist.GetNbinsY()+1):
    slice_pass = hist_pass.ProjectionX('%s_projx_%i_pass' % (hist.GetName(), i), i, i)
    slice_tot = hist_tot.ProjectionX('%s_projx_%i_tot' % (hist.GetName(), i), i, i)
    slice_pass.Write()
    slice_tot.Write()
    gr = ROOT.TGraphAsymmErrors(slice_tot)
    gr.Divide(slice_pass, slice_tot, "b(5,1)")
    gr.SetName('gr_%s_projx_%i' % (hist.GetName(), i))
    gr.Write()

outfile.Close()
wsp.Delete()
