#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

#include "TH1D.h"
#include "TCanvas.h"
#include "TH1D.h"

using namespace Pythia8;

int main()
{
  const int kNumEvent = 10000;

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC ToHepMC;
  // Specify file where HepMC events will be stored.
  HepMC::IO_GenEvent ascii_io("ex_mb_event.dat", std::ios::out);

  TCanvas *can = new TCanvas("can", "can", 800, 600);
  TH1D *h_cmult = new TH1D(
    "hist_cmult_over_eta",
    "Charged particle density distribution",
    51, -5, 5);

  Pythia pythia;

  pythia.readFile("ex_mb_event.cmnd");

  pythia.init();
 
  for(Int_t i_evt = 0; i_evt < kNumEvent; ++i_evt)
  {
    if(not pythia.next()) continue;

    for(int i = 0; i < pythia.event.size(); ++i)
    {
      if(pythia.event[i].isFinal() and pythia.event[i].isCharged())
      {
        h_cmult->Fill(pythia.event[i].eta());
      }
    }

    HepMC::GenEvent* hepmc_evt = new HepMC::GenEvent();
    ToHepMC.fill_next_event(pythia, hepmc_evt);

    ascii_io << hepmc_evt;
    delete hepmc_evt;
  } 

  pythia.stat();

  h_cmult->Draw("hist");
  h_cmult->GetXaxis()->SetTitle("#eta");
  h_cmult->GetYaxis()->SetTitle("#frac{dN_{ch}}{d#eta}");
  can->SetLeftMargin(0.175);
  can->SaveAs("./cmult.png");

  return 0;
}
