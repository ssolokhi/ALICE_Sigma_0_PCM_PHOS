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
		void FillHistogram(const TString &key, const double &value);
		void FillHistogram(const TString &key, const double &value_x, const double &value_y);
		virtual void Terminate(Option_t *option); // Called at the end of an analysis task

	private:
		AliCaloResponse(const AliCaloResponse&);
		AliCaloResponse &operator=(const AliCaloResponse&);

		/// \cond CLASSDEF
		ClassDef(AliCaloResponse, 1); 
		/// \endcond

		AliAODEvent *fAOD;
		TList *fOutputList;

		AliPIDResponse *fPIDResponse;
};
#endif