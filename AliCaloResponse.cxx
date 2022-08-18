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

AliCaloResponse::AliCaloResponse(): AliAnalysisTaskSE(), fAOD(0), fOutputList(0), 
hSelectedEvents(0), hEventPt(0), hVertexZ(0), hClusterEnergy(0), hReconstructedPhotons(0),
hTPCResponse(0),
fPIDResponse(0),fV0ReaderV1(0), fV0ReaderV1Name("fV0ReaderV1") {}

AliCaloResponse::AliCaloResponse(const char *name): AliAnalysisTaskSE(name), fAOD(0), fOutputList(0),
hSelectedEvents(0), hEventPt(0), hVertexZ(0), hClusterEnergy(0), hReconstructedPhotons(0),
hTPCResponse(0), 
fPIDResponse(0), fV0ReaderV1(0), fV0ReaderV1Name("fV0ReaderV1") {
	DefineInput(0, TChain::Class());
	DefineOutput(1, TList::Class());
}

AliCaloResponse::~AliCaloResponse() {
	if (fOutputList) delete fOutputList;
}

void AliCaloResponse::UserCreateOutputObjects() {
	fOutputList = new TList();
	fOutputList->SetOwner(kTRUE);

	hSelectedEvents = new TH1F("hSelectedEvents", "Number Of Selected Events", 5, 0, 5);
	fOutputList->Add(hSelectedEvents);

	hEventPt = new TH1F("hEventPt", "Event Transverse Momentum", 100, 0, 5);
	fOutputList->Add(hEventPt);

	hVertexZ = new TH1F("hVertexZ", "Vertex Z-Coordinate", 100, -20, 20);
	fOutputList->Add(hVertexZ);

	hClusterEnergy = new TH1F("hClusterEnergy", "Cluster Energy", 100, 0, 25);
	fOutputList->Add(hClusterEnergy);

	hReconstructedPhotons = new TH1F("hReconstructedPhotons", "Number Of Reconstructed Photons", 25, 0, 25);
	fOutputList->Add(hReconstructedPhotons);

	hTPCResponse = new TH2F("hTPCResponse", "TPC Response", 100, 0, 4, 250, 0, 250);
	fOutputList->Add(hTPCResponse);

	hElectronSignal = new TH2F("hElectronSignal", "Electron Signal", 100, 0, 4, 100, -10, 10);
	fOutputList->Add(hElectronSignal);

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
		hEventPt->Fill(track->Pt());
		hTPCResponse->Fill(track->P(), track->GetTPCsignal());
		hSelectedEvents->Fill(0);

		double vertexZ = fAOD->GetPrimaryVertex()->GetZ();
		if ((vertexZ < -20) || (vertexZ > 20)) continue;
		hVertexZ->Fill(vertexZ);
		hSelectedEvents->Fill(1);

		double electronSignal = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kElectron);
		if (electronSignal > 5) continue;
		hElectronSignal->Fill(track->Pt(), electronSignal);
		hSelectedEvents->Fill(2);
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
		hClusterEnergy->Fill(cluster->E());
		hSelectedEvents->Fill(3);
	}
}

void AliCaloResponse::GetPhotonConversionSignal() {
	fV0ReaderV1 = reinterpret_cast<AliV0ReaderV1*>(AliAnalysisManager::GetAnalysisManager()->GetTask(fV0ReaderV1Name.Data()));
	if (!fV0ReaderV1) {
		std::cout << "No photon candidate found!" << std::endl;
		return;
	}

}

void AliCaloResponse::Terminate(Option_t *option) {}