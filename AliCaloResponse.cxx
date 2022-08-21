/**************************************************************************
 *    Copyright(c) 2022, ALICE Experiment at CERN, All rights reserved.   *
 *                                                                        *
 * Author: Sergei Solokhin                                                *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// Implementation of methods that are to needed for the ALICE Run 2 analysis script

#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAODEvent.h"
#include "AliAODInputHandler.h"
#include "TChain.h"
#include "TList.h"
#include "TString.h"
#include "TObject.h"
#include "TH1F.h"
#include "AliAODVertex.h"
#include "AliPID.h"
#include "AliPIDResponse.h"
#include "AliCaloResponse.h"
#include "AliCaloPhoton.h"
#include "AliPHOSGeometry.h"
#include "AliAODCaloCells.h"
#include "AliAODCaloCluster.h"
#include "AliV0ReaderV1.h"

AliCaloResponse::AliCaloResponse(): AliAnalysisTaskSE(), fAOD(nullptr), fOutputList(nullptr), 
fPIDResponse(nullptr) {}

AliCaloResponse::AliCaloResponse(const char *name): AliAnalysisTaskSE(name), fAOD(nullptr), fOutputList(nullptr),
fPIDResponse(nullptr) {
	DefineInput(0, TChain::Class());
	DefineOutput(1, TList::Class());
}

AliCaloResponse::~AliCaloResponse() {
	if (fOutputList) delete fOutputList;
}

void AliCaloResponse::UserCreateOutputObjects() {
	fOutputList = new TList();
	fOutputList->SetOwner(kTRUE);

	fOutputList->Add(new TH1F("hSelectedEvents", "Number Of Selected Events", 5, 0, 5));
	fOutputList->Add(new TH1F("hEventPt", "Event Transverse Momentum", 100, 0, 5));
	fOutputList->Add(new TH1F("hVertexZ", "Vertex Z-Coordinate", 100, -20, 20));
	fOutputList->Add(new TH1F("hClusterEnergy", "Cluster Energy", 100, 0, 25));
	fOutputList->Add(new TH1F("hReconstructedPhotons", "Number Of Reconstructed Photons", 25, 0, 25));
	
	fOutputList->Add(new TH2F("hTPCResponse", "TPC Response", 100, 0, 4, 250, 0, 250));
	fOutputList->Add(new TH2F("hElectronSignal", "Electron Signal", 100, 0, 4, 100, -10, 10));

	PostData(1, fOutputList);
}

void AliCaloResponse::UserExec(Option_t *option) {
	AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
	if (!mgr || !mgr->GetInputEventHandler()) return;

	fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
	if (!fAOD) return;

	int nTracks = fAOD->GetNumberOfTracks();

	fPIDResponse = mgr->GetInputEventHandler()->GetPIDResponse();

	for (int i = 0; i < nTracks; ++i) {
		AliAODTrack *track = static_cast<AliAODTrack*>(fAOD->GetTrack(i));
		if (!track || !track->TestFilterBit(1)) continue; // 1 stands for minimum bias trigger
		FillHistogram("hEventPt", track->Pt());
		FillHistogram("hTPCResponse", track->P(), track->GetTPCsignal());
		FillHistogram("hSelectedEvents", 0);

		double vertexZ = fAOD->GetPrimaryVertex()->GetZ();
		if ((vertexZ < -20) || (vertexZ > 20)) continue;
		FillHistogram("hVertexZ", vertexZ);
		FillHistogram("hSelectedEvents", 1);

		double electronSignal = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kElectron);
		if (electronSignal > 5) continue;
		FillHistogram("hElectronSignal", track->Pt(), electronSignal);
		FillHistogram("hSelectedEvents", 2);
	}

	GetPHOSSignal();
	GetPhotonConversionSignal();

	PostData(1, fOutputList);
}

void AliCaloResponse::GetPHOSSignal() {
	int nCaloClusters = fAOD->GetNumberOfCaloClusters();

	for (int i = 0; i < nCaloClusters; ++i) {
		AliAODCaloCluster *cluster = fAOD->GetCaloCluster(i);
		if (!cluster) continue;
		if (cluster->GetType() != AliVCluster::kPHOSNeutral || cluster->GetNCells() < 1) continue;
		FillHistogram("hClusterEnergy", cluster->E());
		FillHistogram("hSelectedEvents", 3);

	}
}

void AliCaloResponse::GetPhotonConversionSignal() {
	const TString cutNumberPhoton = "060000084001001500000000";
	const TString cutNumberEvent = "00000003";
	TString fV0ReaderV1Name = Form("V0ReaderV1_%s_%s", cutNumberEvent.Data(), cutNumberPhoton.Data());
	AliV0ReaderV1 *fV0ReaderV1 = reinterpret_cast<AliV0ReaderV1*>(AliAnalysisManager::GetAnalysisManager()->GetTask(fV0ReaderV1Name.Data()));

	if(fV0ReaderV1) {
		FillHistogram("hReconstructedPhotons", fV0ReaderV1->GetNumberOfPrimaryTracks());
		FillHistogram("hSelectedEvents", 5);
	}
}

void AliCaloResponse::FillHistogram(const TString &key, const double &value) {
	TObject *obj = fOutputList->FindObject(key);
	if (obj) static_cast<TH1F*>(obj)->Fill(value);
}

void AliCaloResponse::FillHistogram(const TString &key, const double &value_x, const double &value_y) {
	TObject *obj = fOutputList->FindObject(key);
	if (obj) static_cast<TH2F*>(obj)->Fill(value_x, value_y);
}

void AliCaloResponse::Terminate(Option_t *option) {}