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


## Usage

See the **ALICE_Analysis_Task** repository for more info.