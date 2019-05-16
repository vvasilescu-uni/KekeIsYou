#include "../Includes/Statements.h"

Unit::STATE Statements::GetState(std::string unitName)
{
	for (auto& prefix : prefixes) {
		if (prefix->name != unitName) {
			continue;
		}

		for (auto& con : connectors) {
			if (!prefix->WillColide(*con, Unit::DIR_RIGHT) &&
				!prefix->WillColide(*con, Unit::DIR_BACKWARD))
			{
				continue;
			}

			for (auto& sufix : sufixes) {
				if (!con->WillColide(*sufix, Unit::DIR_RIGHT) &&
					!con->WillColide(*sufix, Unit::DIR_BACKWARD))
				{
					continue;
				}

				return sufix->specialState;
			}
		}
	}

	return Unit::NOCOLIDE;
}

void Statements::Update()
{
	for (auto& prefix : prefixes) {
		prefix->Update();
	}
	for (auto& con : connectors) {
		con->Update();
	}
	for (auto& sufix : sufixes) {
		sufix->Update();
	}
}

void Statements::Draw()
{
	for (auto& prefix : prefixes) {
		prefix->Draw();
	}
	for (auto& con : connectors) {
		con->Draw();
	}
	for (auto& sufix : sufixes) {
		sufix->Draw();
	}
}
