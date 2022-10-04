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
#include "AliAnalysisTaskSigma0PCMPHOS.h"
#include "AliCaloPhoton.h"
#include "AliPHOSGeometry.h"
#include "AliAODCaloCells.h"
#include "AliAODCaloCluster.h"
#include "AliV0ReaderV1.h"

AliAnalysisTaskSigma0PCMPHOS::AliAnalysisTaskSigma0PCMPHOS(): AliAnalysisTaskSE(), 
fAOD(nullptr), fOutputList(nullptr), fPIDResponse(nullptr) {}

AliAnalysisTaskSigma0PCMPHOS::AliAnalysisTaskSigma0PCMPHOS(const char *name): AliAnalysisTaskSE(name),
fAOD(nullptr), fOutputList(nullptr), fPIDResponse(nullptr) {
	DefineInput(0, TChain::Class());
	DefineOutput(1, TList::Class());
}

AliAnalysisTaskSigma0PCMPHOS::~AliAnalysisTaskSigma0PCMPHOS() {
	if (fOutputList) delete fOutputList;
}

void AliAnalysisTaskSigma0PCMPHOS::UserCreateOutputObjects() {
	fOutputList = new TList();
	fOutputList->SetOwner(kTRUE);

	fOutputList->Add(new TH1F("hEventPt", "Event Transverse Momentum", 100, 0, 5));
	fOutputList->Add(new TH1F("hVertexZ", "Vertex Z-Coordinate", 100, -20, 20));
	fOutputList->Add(new TH1F("hClusterEnergy", "Cluster Energy", 25, 0, 5));
	fOutputList->Add(new TH1F("hReconstructedPhotons", "Number Of Reconstructed Photons", 25, 0, 25));
	
	fOutputList->Add(new TH2F("hTPCResponse", "TPC Response", 100, 0, 4, 250, 0, 250));
	fOutputList->Add(new TH2F("hElectronSignal", "Electron Signal", 100, 0, 4, 100, -10, 10));
	fOutputList->Add(new TH2F("hClusterTOFvsEnergy", "Cluster TOF vs Energy", 100, 0, 1e-6, 40, 0, 20));

	PostData(1, fOutputList);
}

void AliAnalysisTaskSigma0PCMPHOS::UserExec(Option_t *option) {
	AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
	if (!mgr || !mgr->GetInputEventHandler()) return;

	fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
	if (!fAOD || !fAOD->GetHeader()) return;

	int nTracks = fAOD->GetNumberOfTracks();
	int nV0 = fAOD->GetNumberOfV0s();

	fPIDResponse = mgr->GetInputEventHandler()->GetPIDResponse();

	double primaryVertexPosition[3] = {0, 0, 0};
	fAOD->GetPrimaryVertex()->GetXYZ(primaryVertexPosition);

	for (int i = 0; i < nTracks; ++i) {
		AliAODTrack *track = dynamic_cast<AliAODTrack*>(fAOD->GetTrack(i));
		if (!track || !track->TestFilterBit(1)) continue; // 1 stands for minimum bias trigger
		FillHistogram("hEventPt", track->Pt());
		FillHistogram("hTPCResponse", track->P(), track->GetTPCsignal());

		double vertexZ = fAOD->GetPrimaryVertex()->GetZ();
		if ((vertexZ < -20) || (vertexZ > 20)) continue;
		FillHistogram("hVertexZ", vertexZ);

		double electronSignal = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kElectron);
		if (electronSignal > 5) continue;
		FillHistogram("hElectronSignal", track->Pt(), electronSignal);
	}

	GetPHOSSignal();
	GetPhotonConversionSignal();

	PostData(1, fOutputList);
}

bool AcceptTrack(const AliAODTrack *track) {
	if (!track->IsOn(AliAODTrack::kTPCrefit)) return kFALSE;
	double nCrossedPTCRows = track->GetTPCClusterInfo(2,1);
	if (nCrossedPTCRows < 70) return kFALSE;
	int locatableClusters = track->GetTPCNclsF();
	if (locatableClusters <= 0) return kFALSE;
	if (nCrossedPTCRows/locatableClusters <0.8) return kFALSE;
	return kTRUE;
}


void AliAnalysisTaskSigma0PCMPHOS::GetPHOSSignal() {
	AliAODVertex *primaryVertex = fAOD->GetPrimaryVertex();
	int nCaloClusters = fAOD->GetNumberOfCaloClusters();

	for (int i = 0; i < nCaloClusters; ++i) {
		AliAODCaloCluster *cluster = fAOD->GetCaloCluster(i);
		if (!cluster) continue;
		if (cluster->GetType() != AliVCluster::kPHOSNeutral || cluster->GetNCells() < 1) continue;
		if (cluster->E() > 1.5 || cluster->GetM02() < 0.2) continue;
		FillHistogram("hClusterEnergy", cluster->E());
		FillHistogram("hClusterTOFvsEnergy", cluster->GetTOF(), cluster->E());
	}
}

void AliAnalysisTaskSigma0PCMPHOS::GetPhotonConversionSignal() {
	AliV0ReaderV1 *fV0ReaderV1 = dynamic_cast<AliV0ReaderV1*>(AliAnalysisManager::GetAnalysisManager()->GetTask("fV0ReaderV1"));
	if (!fV0ReaderV1) return;
}

void AliAnalysisTaskSigma0PCMPHOS::FillHistogram(const TString &key, const double &value) {
	TObject *obj = fOutputList->FindObject(key);
	if (obj) static_cast<TH1F*>(obj)->Fill(value);
}

void AliAnalysisTaskSigma0PCMPHOS::FillHistogram(const TString &key, const double &value_x, const double &value_y) {
	TObject *obj = fOutputList->FindObject(key);
	if (obj) static_cast<TH2F*>(obj)->Fill(value_x, value_y);
}

void AliAnalysisTaskSigma0PCMPHOS::Terminate(Option_t *option) {}