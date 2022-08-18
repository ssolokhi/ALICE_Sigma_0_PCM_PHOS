/* Copyright(c) 2022, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

#ifndef AliCaloResponse_H
#define AliCaloResponse_H

#include "AliAnalysisTaskSE.h"
#include "AliPIDResponse.h"
#include "AliV0ReaderV1.h"

class AliPIDResponce;
class AliMultSelectionTask;

class AliCaloResponse: public AliAnalysisTaskSE {
	/* derived from the AliAnalysisTask class for Single Events*/
	public:
		AliCaloResponse();
		AliCaloResponse(const char *name);

		virtual ~AliCaloResponse();

		virtual void UserCreateOutputObjects(); // For booking output histograms, TTrees, etc. to the .root file
		virtual void UserExec(Option_t *option); // To be called for EACH event that passed the trigger
		void GetPHOSSignal();
		void GetPhotonConversionSignal();
		virtual void Terminate(Option_t *option); // Called at the end of an analysis task

	private:
		AliCaloResponse(const AliCaloResponse&);
		AliCaloResponse &operator=(const AliCaloResponse&);

		/// \cond CLASSDEF
		ClassDef(AliCaloResponse, 1); 
		/// \endcond

		AliAODEvent *fAOD;
		TList *fOutputList;

		TH1F *hSelectedEvents;
		TH1F *hEventPt;
		TH1F *hVertexZ;
		TH1F *hClusterEnergy;
		TH1F *hReconstructedPhotons;

		TH2F *hTPCResponse;
		TH2F *hElectronSignal;

		AliPIDResponse *fPIDResponse;
		AliV0ReaderV1 *fV0ReaderV1;
		TString fV0ReaderV1Name;
};
#endif