#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

#include "TH1D.h"
#include "TCanvas.h"
#include "TH1D.h"

using namespace Pythia8;

int main()
{
  const int kNumEvent = 2000;

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC ToHepMC;
  // Specify file where HepMC events will be stored.
  HepMC::IO_GenEvent ascii_io("min_bias_ver1.dat", std::ios::out);

  // A Canvas is an area mapped to a window directly under the control of
  // the display manager.
  // TCanvas (const char *name, const char *title, Int_t ww, Int_t wh)
  // ww = width / wh = height (in pixels)
  TCanvas *can = new TCanvas("can", "can", 800, 600);

  // Create a 1-Dim histogram with fix bins of type double.
  // TH1D (const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup)
  TH1D *h_cmult = new TH1D(
    "hist_cmult_over_eta",
    "Charged particle density distribution",
    101, -5, 5);
  const double kBinWidth = h_cmult->GetBinWidth(1);

  // 
  Pythia pythia;

  // Read the character of a run from a file.
  pythia.readFile("min_bias_ver1.cmnd");

  pythia.init();

  // The event generation loop. 
  for(int i_evt = 0; i_evt < kNumEvent; ++i_evt)
  {
    // Generate the next event
    // The method returns false if the event generation fails.
    if(not pythia.next()) continue;

    // The particle loop
    // We can ccess the properties of each particle pythia.event[i].
    for(int i_part = 0; i_part < pythia.event.size(); ++i_part)
    {
      
      if(pythia.event[i_part].isFinal() and pythia.event[i_part].isCharged())
      {
        // TH1::Fill(Double_t x)
        // Increment bin with abscissa X by 1.
        h_cmult->Fill(pythia.event[i_part].eta());
      }
    }

    HepMC::GenEvent* hepmc_evt = new HepMC::GenEvent();
    // Convert Pythia events into HepMC ones.
    ToHepMC.fill_next_event(pythia, hepmc_evt);

    ascii_io << hepmc_evt;
    delete hepmc_evt;
  } 

  // List statistics on the event generation, specifically total and partial
  // cross sections and the number of different errors. 
  pythia.stat();

  h_cmult->Scale(1 / kBinWidth); 
  h_cmult->Draw("hist");
  h_cmult->GetXaxis()->SetTitle("#eta");
  h_cmult->GetYaxis()->SetTitle("#frac{dN_{ch}}{d#eta}");
  can->SetLeftMargin(0.175);

  // Save 
  can->SaveAs("./density_dist_ver1.png");

  return 0;
}
