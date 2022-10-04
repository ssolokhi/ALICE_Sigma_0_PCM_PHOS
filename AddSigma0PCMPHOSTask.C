#include "AliVEvent.h"

AliAnalysisTaskSigma0PCMPHOS *AddSigma0PCMPHOSTask(TString name = "name") {
	AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();

	if (!mgr || !mgr->GetInputEventHandler()) return 0x0;

	TString fileName = AliAnalysisManager::GetCommonFileName();
	fileName += ":AliAnalysisTaskSigma0PCMPHOS"; // creates a folder in the ROOT file

	AliAnalysisTaskSigma0PCMPHOS *task = new AliAnalysisTaskSigma0PCMPHOS(name.Data());
	if (!task) return 0x0;
	mgr->AddTask(task);
	task->SelectCollisionCandidates(AliVEvent::kINT7); // minimum vias trigger V0M
	mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());
	mgr->ConnectOutput(task, 1, mgr->CreateContainer("Histograms", TList::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));

	return task;
}