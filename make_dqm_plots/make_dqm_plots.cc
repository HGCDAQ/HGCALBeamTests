#include <fstream>
#include <iostream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "TFile.h"
#include "TH2Poly.h"
#include "TH2I.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TKey.h"

#include "TCanvas.h"

TCanvas *c1;

// Plot any histogram to pdf
template <typename TH>
void plot_hist_to_pdf(TH *hist, TCanvas *c, std::string pathname)
{
    hist->Draw();
    c->Update();
    c->SaveAs((pathname + "/" + hist->GetName() + ".pdf").c_str());
}

// Iterate over hexaboard/layer/wire chamber
void iter_over_detector_part(TDirectoryFile *tdf_detector_part, std::string pathname)
{
    TIter next(tdf_detector_part->GetListOfKeys());

    TKey *key1, *key2;

    for (Int_t i = 0; (key1 = (TKey*)next()); ++i)
    {
        // Create directory for each hexaboard
        std::string sub_pathname(pathname + key1->GetName());
        boost::filesystem::create_directories(sub_pathname);

        TDirectoryFile *tdf = (TDirectoryFile*)key1->ReadObj();

        // loop over histograms
        TIter next_hexagon_hist(tdf->GetListOfKeys());
        for (Int_t j = 0; (key2 = (TKey*)next_hexagon_hist()); ++j)
        {
            plot_hist_to_pdf(key2->ReadObj(), c1, sub_pathname);
        }
    }
}

int main( int argc, char *argv[] )
{
    if (argc < 1 + 1)
    {
        std::cout << "This script plots all histograms found in a rootfile from EUDAC." << std::endl;
        std::cout << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "    make_dqm_plots *rootfile*" << std::endl;
        exit(0);
    }

    // Determine the name of the output directory
    std::vector<std::string> strs;
    boost::split(strs, argv[1], boost::is_any_of("/"));
    for (Int_t i = 0; i < 5; ++i) strs[strs.size() - 1].pop_back();
    std::string dirname = strs[strs.size() - 1] + "_dqm_plots";

    // Read in ROOT File
    TFile *tfile = new TFile(argv[1], "READ");

    // Canvas to draw all the plots on
    c1 = new TCanvas("c1", "c1", 500, 500);
    c1->cd();

    TIter next(tfile->GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next()))
    {
       std::string subdet_name = key->GetName();
       // Create Directory structure
       boost::filesystem::create_directories(dirname + "/" + subdet_name);
       TDirectoryFile *tdf_subdet = (TDirectoryFile*)key->ReadObj();
       iter_over_detector_part(tdf_subdet, dirname + "/" + subdet_name + "/");
    }

    delete c1;
}
