#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

#include "TH1D.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TPaveLabel.h"
#include "TStyle.h"

#include <vector>

using namespace Pythia8;

int main()
{
  const int kNumEvent = 1000;
  

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC ToHepMC;
  // Specify file where HepMC events will be stored.
  HepMC::IO_GenEvent ascii_io("min_bias.dat", std::ios::out);

  TCanvas *can = new TCanvas("can", "can", 800, 600);

  const int kNumBin = 101;
  const int kXLow = -5;
  const int kXHigh = 5;
  const char* kTitle = "Charged particle density distribution";

  std::vector<TH1D*> h_cmult(3);
  h_cmult[0] = new TH1D("hist_default", kTitle, kNumBin, kXLow, kXHigh);
  h_cmult[1] = new TH1D("hist_common", kTitle, kNumBin, kXLow, kXHigh);
  h_cmult[2] = new TH1D("hist_cmssw", kTitle, kNumBin, kXLow, kXHigh);

  const double kBinWidth = h_cmult[0]->GetBinWidth(1);

  Pythia pythia;
  pythia.readFile("min_bias.cmnd");
  const int kNumSubRun = pythia.mode("Main:numberOfSubruns");
  for(int i_subrun = 1; i_subrun <= kNumSubRun; ++i_subrun)
  { 
    pythia.readFile("min_bias.cmnd", i_subrun);

    pythia.init();

    // The event generation loop. 
    for(Int_t i_evt = 0; i_evt < kNumEvent; ++i_evt)
    {
      if(not pythia.next()) continue;

      for(int i_part = 0; i_part < pythia.event.size(); ++i_part)
      {
        if(pythia.event[i_part].isFinal() and pythia.event[i_part].isCharged())
        {
          h_cmult[i_subrun-1]->Fill(pythia.event[i_part].eta());
        }
      }

      HepMC::GenEvent* hepmc_evt = new HepMC::GenEvent();
      ToHepMC.fill_next_event(pythia, hepmc_evt);

      ascii_io << hepmc_evt;
      delete hepmc_evt;
    } 

    pythia.stat();
  }

  h_cmult[0]->Draw("hist");
  h_cmult[1]->Draw("hist SAME");
  h_cmult[2]->Draw("hist SAME");

  h_cmult[0]->SetLineColor(1);
  h_cmult[1]->SetLineColor(38);
  h_cmult[2]->SetLineColor(46);

  for(auto h: h_cmult) {
    h->SetLineWidth(3);
    h->Scale(1.0 / (kNumEvent * kBinWidth));
   }

  h_cmult[0]->GetXaxis()->SetTitle("#eta");
  h_cmult[0]->GetYaxis()->SetTitle("#frac{dN_{ch}}{d#eta}");

  auto legend = new TLegend(0.2, 0.82, 0.48, 0.93);
  legend->AddEntry(h_cmult[0], "Pythia8 Default Settings", "l");
  legend->AddEntry(h_cmult[1], "+ Pythia Common Settings", "l");
  legend->AddEntry(h_cmult[2], "+ Additional Settings", "l");
  legend->Draw();

  can->SetLeftMargin(0.175);
  can->SaveAs("./charged_particle.png");

  return 0;
}
