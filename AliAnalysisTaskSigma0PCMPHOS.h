/* Copyright(c) 2022, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

#ifndef AliAnalysisTaskSigma0PCMPHOS_H
#define AliAnalysisTaskSigma0PCMPHOS_H

#include "AliAnalysisTaskSE.h"
#include "AliPIDResponse.h"

class AliPIDResponce;

class AliAnalysisTaskSigma0PCMPHOS: public AliAnalysisTaskSE {
	public:		AliAnalysisTaskSigma0PCMPHOS();
		AliAnalysisTaskSigma0PCMPHOS(const char *name);

		virtual ~AliAnalysisTaskSigma0PCMPHOS();

		virtual void UserCreateOutputObjects(); 
		virtual void UserExec(Option_t *option);

		bool AcceptTrack(const AliAODTrack *track);
		void GetPHOSSignal();
		void GetPhotonConversionSignal();
		
		void FillHistogram(const TString &key, const double &value);
		void FillHistogram(const TString &key, const double &value_x, const double &value_y);

		virtual void Terminate(Option_t *option);

	private:
		AliAnalysisTaskSigma0PCMPHOS(const AliAnalysisTaskSigma0PCMPHOS&);
		AliAnalysisTaskSigma0PCMPHOS &operator=(const AliAnalysisTaskSigma0PCMPHOS&);

		/// \cond CLASSDEF
		ClassDef(AliAnalysisTaskSigma0PCMPHOS, 1); 
		/// \endcond

		AliAODEvent *fAOD; //!<!
		TList *fOutputList; //!<!
		AliPIDResponse *fPIDResponse; //!<!
};
#endif