#include "AliVEvent.h"

AliCaloResponse *AddCaloResponseTask(TString name = "name") {
	AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();

	if (!mgr || !mgr->GetInputEventHandler()) return 0x0;

	TString fileName = AliAnalysisManager::GetCommonFileName();
	fileName += ":AliCaloResponse"; // creates a folder in the ROOT file

	AliCaloResponse *task = new AliCaloResponse(name.Data());
	if (!task) return 0x0;
	mgr->AddTask(task);
	task->SelectCollisionCandidates(AliVEvent::kINT7); // minimum vias trigger for multiplicity calculations
	mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());
	mgr->ConnectOutput(task, 1, mgr->CreateContainer("Histograms", TList::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));

	return task;
}