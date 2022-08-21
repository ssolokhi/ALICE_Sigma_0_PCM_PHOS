# ALICE Calorimeter Response Task

---

## Description

The following code is meant to extract the response of the electromagnetic detectors of ALICE (CERN). The output of the code is used to analyse ALICE data in search of photons born in the decay of $\Sigma^0$ hyperons.
    
## Repository Content
    
This repository contains several files:
    
- This **README** file;
    
- A header (**AliCaloResponse.h**) file containing class methods' prototypes required for the analysis;
    
- A methods' implementation (**AliCaloResponse.cxx**) files corresponding to the header files;
    
- A class instance (**AddCaloResponse.C**) macro;
    
- A so called *steering* (**RunCaloResponse.C**) macro for defining JAliEn instructions for running on GRID.

The full code lisence can be found in *AliCaloResponse.cxx*.

## Analysis Layout

The *UserCreateOutputObject* function is called once and it adds historgams to the output list. A noticeable reduction of code redundancy is achieved since histograms are not called as class members, as they were before. Now they are filled by their name, and it is therefore not necessary to create pointers to there objects. This makes the code less prone to errors since the pointers no longer have to be declared in the header file and initialized in both constructors in the implementation file.

The *UserExec* function that is called for each event. At first, histograms are being filled with the transverse momentum $p_T$, the PID and TPC response, the primary vertex (V0) z-coordinate if the minimum bias trigger was fired and several specific cut tests are passed (see the function body). The function then calls several separate methods:

1. *GetPHOSSignal* retrieves the neutrally-charged response of the ALICE photon spectrometer (PHOS) that has a specific cluster size;

2. *GetPhotonConversionSignal* retrieves the signal obtained with the photon conversion method (PCM). 



## Usage

See the **ALICE_Analysis_Task** repository for more info.