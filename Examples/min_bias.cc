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
  HepMC::IO_GenEvent ascii_io("min_bias.dat", std::ios::out);

  TCanvas *can = new TCanvas("can", "can", 800, 600);
  TH1D *h_cmult = new TH1D(
    "hist_cmult_over_eta",
    "Charged particle density distribution",
    501, -5, 5);

  // 
  Pythia pythia;

  //
  pythia.readFile("min_bias.cmnd");

  //
  pythia.init();

  // The event generation loop. 
  for(int i_evt = 0; i_evt < kNumEvent; ++i_evt)
  {
    if(not pythia.next()) continue;

    for(int i_part = 0; i_part < pythia.event.size(); ++i_part)
    {
      if(pythia.event[i_part].isFinal() and pythia.event[i_part].isCharged())
      {
        h_cmult->Fill(pythia.event[i_part].eta());
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
  can->SaveAs("./charged_particle.png");

  return 0;
}
