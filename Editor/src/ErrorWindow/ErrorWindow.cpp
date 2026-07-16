module;
#include <ImGui/imgui.h>
module ErrorWindow;

import Error;

using namespace Umi;

void ErrorWindow::Draw()
{
	if (ImGui::Begin("Error Window"))
	{
		if (!Error::NonFatalErrors.empty())
		{
			ImGui::Text("Errors:");
			for (auto& error : Error::NonFatalErrors)
			{
				ImGui::TextWrapped("%s", error.c_str());
			}
		}
		else
		{
			ImGui::Text("No Errors.");
		}
	}
	ImGui::End();
}