#include "UIAnimationLog.h"
#include "Settings.h"
#include "UICommon.h"
#include "UIManager.h"

namespace UI
{
	bool UIAnimationLog::ShouldDrawImpl() const
	{
		return Settings::bEnableAnimationLog;
	}

	void UIAnimationLog::DrawImpl()
	{
		SetWindowDimensions(Settings::fAnimationLogsOffsetX, Settings::fAnimationLogsOffsetY, Settings::fAnimationLogWidth, 0.f, WindowAlignment::kTopRight, ImVec2(Settings::fAnimationLogWidth, -1), ImVec2(Settings::fAnimationLogWidth, -1), ImGuiCond_Always);
		ForceSetWidth(Settings::fAnimationLogWidth);

		constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

		if (ImGui::Begin(_T("Animation Log"), nullptr, windowFlags)) {
			if (UIManager::GetSingleton().GetRefrToEvaluate() != nullptr) {
				auto& animationLog = AnimationLog::GetSingleton();
				if (!animationLog.IsAnimationLogEmpty()) {
					if (ImGui::BeginTable("AnimationLogTable", 1, ImGuiTableFlags_Borders)) {
						if (animationLog.tracedEntry.bIsValid) {
							DrawLogEntry(animationLog.tracedEntry);
							DrawTrace(animationLog.tracedEntry.trace);
						} else {
							animationLog.ForEachAnimationLogEntry([&](AnimationLogEntry& a_logEntry) {
								DrawLogEntry(a_logEntry);
							});
						}
						ImGui::EndTable();
					}
				} else {
					UICommon::TextUnformattedDisabled(_T("No animation log entries"));
				}
			} else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
				ImGui::TextWrapped(_T("No reference selected. Type in a FormID in the main window, or select a reference in the console."));
				ImGui::PopStyleColor();
			}
			if (IsInteractable()) {
				DrawFilterPanel();
			}
		}
		ImGui::End();
	}

	void UIAnimationLog::OnOpen()
	{
		AnimationLog::GetSingleton().SetLogAnimations(true);
	}

	void UIAnimationLog::OnClose()
	{
		AnimationLog::GetSingleton().SetLogAnimations(false);
	}

	bool UIAnimationLog::IsInteractable() const
	{
		// draw filter panel only when the main UI is open
		return UIManager::GetSingleton().bShowMain;
	}

	void UIAnimationLog::DrawFilterPanel() const
	{
		auto& animationLog = AnimationLog::GetSingleton();

		// filtering
		const auto& style = ImGui::GetStyle();
		const float helpMarkerWidth = ImGui::CalcTextSize("(?)").x + style.ItemSpacing.x * 2;
		const float filterWidth = (ImGui::GetContentRegionAvail().x - style.FramePadding.x * 2 - helpMarkerWidth * 2);

		ImGui::SetNextItemWidth(filterWidth);
		ImGui::InputTextWithHint("##filter", _T("Filter... (Affects new entries)"), &animationLog.filter);
		ImGui::SameLine();
		UICommon::HelpMarker(_T("Type a part of the log event type / animation name / path / mod name / submod name to filter the log results. You can use regex."));
	}

	void UIAnimationLog::DrawLogEntry(AnimationLogEntry& a_logEntry)
	{
		using Event = AnimationLogEntry::Event;
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		if (a_logEntry.timeDrawn < Settings::fAnimationLogEntryFadeTime) {
			const float alpha = std::lerp(0.f, 0.25f, std::fmax(Settings::fAnimationLogEntryFadeTime - a_logEntry.timeDrawn, 0.f) / Settings::fAnimationLogEntryFadeTime);
			const ImVec4 color(1.f, 1.f, 1.f, alpha);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(color));
		}

		const ImGuiIO& io = ImGui::GetIO();
		a_logEntry.timeDrawn += io.DeltaTime;

		switch (a_logEntry.event) {
		case Event::kActivate:
			UICommon::TextUnformattedColored(UICommon::LOG_ACTIVATED_COLOR, _T("Activate"));
			break;
		case Event::kActivateSynchronized:
			UICommon::TextUnformattedColored(UICommon::LOG_ACTIVATED_COLOR, _T("Activate"));
			ImGui::SameLine();
			UICommon::TextUnformattedColored(UICommon::LOG_SYNCHRONIZED_COLOR, _T("Paired"));
			break;
		case Event::kEcho:
			UICommon::TextUnformattedColored(UICommon::LOG_ECHO_COLOR, _T("Echo"));
			break;
		case Event::kLoop:
			UICommon::TextUnformattedColored(UICommon::LOG_LOOP_COLOR, _T("Loop"));
			break;
		case Event::kActivateReplace:
			UICommon::TextUnformattedColored(UICommon::LOG_ACTIVATED_COLOR, _T("Activate"));
			ImGui::SameLine();
			UICommon::TextUnformattedColored(UICommon::LOG_REPLACED_COLOR, _T("[Replaced]"));
			break;
		case Event::kActivateReplaceSynchronized:
			UICommon::TextUnformattedColored(UICommon::LOG_ACTIVATED_COLOR, _T("Activate"));
			ImGui::SameLine();
			UICommon::TextUnformattedColored(UICommon::LOG_SYNCHRONIZED_COLOR, _T("Paired"));
			ImGui::SameLine();
			UICommon::TextUnformattedColored(UICommon::LOG_REPLACED_COLOR, _T("[Replaced]"));
			break;
		case Event::kEchoReplace:
			UICommon::TextUnformattedColored(UICommon::LOG_ECHO_COLOR, _T("Echo"));
			ImGui::SameLine();
			UICommon::TextUnformattedColored(UICommon::LOG_REPLACED_COLOR, _T("[Replaced]"));
			break;
		case Event::kLoopReplace:
			UICommon::TextUnformattedColored(UICommon::LOG_LOOP_COLOR, _T("Loop"));
			ImGui::SameLine();
			UICommon::TextUnformattedColored(UICommon::LOG_REPLACED_COLOR, _T("[Replaced]"));
			break;
		case Event::kInterrupt:
			UICommon::TextUnformattedColored(UICommon::LOG_INTERRUPTED_COLOR, _T("Interrupted"));
			break;
		case Event::kPairedMismatch:
			UICommon::TextUnformattedColored(UICommon::LOG_INTERRUPTED_COLOR, _T("Interrupted"));
			ImGui::SameLine();
			UICommon::TextUnformattedColored(UICommon::LOG_SYNCHRONIZED_COLOR, _T("(Paired Mismatch)"));
			break;
		}

		if (a_logEntry.bVariant) {
			ImGui::SameLine();
			UICommon::TextUnformattedColored(UICommon::LOG_VARIANT_COLOR, _T("[Variant]"));
		}

		if (a_logEntry.event < Event::kInterrupt && a_logEntry.bInterruptible) {
			ImGui::SameLine();
			UICommon::TextUnformattedColored(UICommon::LOG_INTERRUPTED_COLOR, _T("(Interruptible)"));
		}

		if (a_logEntry.count > 1) {
			ImGui::SameLine();
			const auto text = std::format("x{} ", a_logEntry.count);
			ImGui::TextUnformatted(text.data());
		}

		const std::string projectText = std::format("{}: {} ", _T("Project"), a_logEntry.projectName);
		ImGui::SameLine(ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(projectText.data()).x);
		UICommon::TextUnformattedDisabled(projectText.data());

		const std::string clipText = std::format("{}: {} ", _T("Clip"), a_logEntry.clipName);
		const float clipTextWidth = ImGui::CalcTextSize(clipText.data()).x;

		UICommon::TextUnformattedDisabled(_T("Name:"));
		ImGui::SameLine();
		UICommon::TextUnformattedEllipsis(a_logEntry.animationName.data(), nullptr, ImGui::GetContentRegionAvail().x - clipTextWidth);

		ImGui::SameLine(ImGui::GetContentRegionMax().x - clipTextWidth);
		UICommon::TextUnformattedDisabled(clipText.data());

		if (a_logEntry.bOriginal) {
			UICommon::TextUnformattedDisabled(_T("Original animation"));
		} else {
			UICommon::TextUnformattedDisabled(_T("Mod:"));
			ImGui::SameLine();
			ImGui::TextUnformatted(a_logEntry.modName.data());
			ImGui::SameLine();
			UICommon::TextUnformattedDisabled(_T("Submod:"));
			ImGui::SameLine();
			ImGui::TextUnformatted(a_logEntry.subModName.data());
			//ImGui::Text(std::format("{} / {}", a_logEntry.modName, a_logEntry.subModName).data());
		}

		// trace button
		auto& animationLog = AnimationLog::GetSingleton();
		if (!animationLog.tracedEntry.bIsValid) {
			const auto& style = ImGui::GetStyle();
			std::string traceButtonText = _T("Show trace");
			const float traceButtonWidth = ImGui::CalcTextSize(traceButtonText.data()).x + style.FramePadding.x * 2 + style.ItemSpacing.x;
			traceButtonText += "##" + std::to_string(reinterpret_cast<std::uintptr_t>(&a_logEntry));
			ImGui::SameLine(ImGui::GetContentRegionMax().x - traceButtonWidth);
			if (ImGui::SmallButton(traceButtonText.data())) {
				animationLog.tracedEntry = a_logEntry;
			}
		}

		float variantTextWidth = 0.f;
		if (a_logEntry.bVariant) {
			variantTextWidth = ImGui::CalcTextSize(a_logEntry.variantFilename.data()).x + ImGui::GetStyle().ItemSpacing.x;
		}

		UICommon::TextUnformattedDisabled(_T("Path:"));
		ImGui::SameLine();
		UICommon::TextUnformattedEllipsis(a_logEntry.animPath.data(), nullptr, ImGui::GetContentRegionAvail().x - variantTextWidth);

		if (a_logEntry.bVariant) {
			ImGui::SameLine();
			UICommon::TextUnformattedColored(UICommon::LOG_VARIANT_COLOR, a_logEntry.variantFilename.data());
		}
	}

	void SetColors(ReplacementTrace::Step::StepResult a_result, const std::function<void()>& a_function)
	{
		bool bPushedColor = false;

		switch (a_result) {
		case ReplacementTrace::Step::StepResult::kSuccess:
		case ReplacementTrace::Step::StepResult::kNoConditions:
			ImGui::PushStyleColor(ImGuiCol_Text, UICommon::SUCCESS_COLOR);
			bPushedColor = true;
			break;
		case ReplacementTrace::Step::StepResult::kFail:
			ImGui::PushStyleColor(ImGuiCol_Text, UICommon::FAIL_COLOR);
			bPushedColor = true;
			break;
		}

		a_function();

		if (bPushedColor) {
			ImGui::PopStyleColor();
		}
	}

	void UIAnimationLog::DrawTrace(const ReplacementTrace& a_trace) const
	{
		if (ImGui::BeginChild("TraceWindow", ImVec2(0, 600), true)) {
			for (auto& step : a_trace.steps) {
				//ImGui::TableNextRow();
				//ImGui::TableSetColumnIndex(0);
				//ImGui::AlignTextToFramePadding();

				bool bNodeOpen = false;
				if (!step.conditions.empty() || !step.synchronizedConditions.empty()) {
					bNodeOpen = ImGui::TreeNodeEx(&step, ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth, "");
				} else {
					bNodeOpen = UICommon::TreeNodeCollapsedLeaf(&step, ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Bullet, "");
				}

				ImGui::SameLine();

				ImGui::BeginDisabled(step.result == ReplacementTrace::Step::StepResult::kDisabled);

				std::string modText = step.modName + " | " + step.subModName;
				ImGui::TextUnformatted(modText.data());

				UICommon::SecondColumn(0.85f);

				SetColors(step.result, [&]() {
					ImGui::TextUnformatted(GetTraceResultText(step.result).data());
				});

				ImGui::EndDisabled();

				if (bNodeOpen) {
					bool bHasSynchronizedConditions = !step.synchronizedConditions.empty();
					if (bHasSynchronizedConditions) {
						ImGui::TextUnformatted(_T("Conditions:"));
					}
					for (auto& condition : step.conditions) {
						DrawTraceCondition(condition);
					}
					if (bHasSynchronizedConditions) {
						ImGui::TextUnformatted(_T("Synchronized conditions:"));
						for (auto& condition : step.synchronizedConditions) {
							DrawTraceCondition(condition);
						}
					}

					ImGui::TreePop();
				}
			}
		}

		ImGui::EndChild();

		if (ImGui::Button(_T("Close trace"))) {
			auto& animationLog = AnimationLog::GetSingleton();
			animationLog.tracedEntry = AnimationLogEntry();
		}
	}

	std::string_view UIAnimationLog::GetTraceResultText(ReplacementTrace::Step::StepResult a_stepResult) const
	{
		switch (a_stepResult) {
		case ReplacementTrace::Step::StepResult::kSuccess:
			return _T("Success");
		case ReplacementTrace::Step::StepResult::kFail:
			return _T("Fail");
		case ReplacementTrace::Step::StepResult::kDisabled:
			return _T("Disabled");
		case ReplacementTrace::Step::StepResult::kNoConditions:
			return _T("No conditions");
		default:
			return _T("Unknown");
		}
	}

	void UIAnimationLog::DrawTraceCondition(const ReplacementTrace::Step::ConditionEntry& a_conditionEntry) const
	{
		//ImGui::TableNextRow();
		//ImGui::TableSetColumnIndex(0);

		bool bConditionOpen = false;
		if (!a_conditionEntry.childConditions.empty()) {
			bConditionOpen = ImGui::TreeNodeEx(&a_conditionEntry, ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth, "");
		} else {
			bConditionOpen = UICommon::TreeNodeCollapsedLeaf(&a_conditionEntry, ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Bullet, "");
		}

		ImGui::SameLine();

		ImGui::BeginDisabled(a_conditionEntry.evaluationResult == ReplacementTrace::Step::StepResult::kDisabled);

		ImGui::TextUnformatted(a_conditionEntry.conditionName.data());

		UICommon::SecondColumn(0.85f);

		SetColors(a_conditionEntry.evaluationResult, [&]() {
			ImGui::TextUnformatted(GetTraceResultText(a_conditionEntry.evaluationResult).data());
		});

		ImGui::EndDisabled();

		if (bConditionOpen) {
			for (auto& childCondition : a_conditionEntry.childConditions) {
				DrawTraceCondition(childCondition);
			}
			ImGui::TreePop();
		}
	}
}
