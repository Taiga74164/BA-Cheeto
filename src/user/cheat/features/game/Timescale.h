#pragma once
#include "user/cheat/feature_base.h"
#include "appdata/types.h"
#include "memory/hook_manager.h"
#include <algorithm>
#include "utils/config_manager.h"
#include "utils/config_field.h"

namespace cheat::features
{
	// Speeds up battle logic by scaling BattleGameTime::Tick.
	class Timescale : public FeatureBase
	{
		DECL_SINGLETON(Timescale)

	public:
		inline Timescale()
			: FeatureBase("Timescale", "Speed up battle by scaling logic time per tick", FeatureSection::Game)
		{
			HookManager::install(BattleGameTime::Tick(), hBattleGameTime_Tick);
		}

		inline void init() override
		{
			m_scaleField = Config::Field<float>("Game", getName(), "scale", 1.0f);
			m_scale = m_scaleField.get();
		}

		inline void draw() override
		{
			float prev = m_scale;
			ImGui::SliderFloat("Scale (x)", &m_scale, 0.1f, 10.0f, "%.2fx");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			if (ImGui::InputFloat("##scale_input", &m_scale, 0.0f, 0.0f, "%.2f"))
			{
				m_scale = std::clamp(m_scale, 0.1f, 10.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset")) m_scale = 1.0f;
			ImGui::TextDisabled("Affects battle logic tick; 2.0x ~ twice as fast");

			if (m_scale != prev)
			{
				m_scaleField = m_scale;
			}
		}

		float getScale() const { return m_scale; }

	private:
		inline static void hBattleGameTime_Tick(BattleGameTime* _this)
		{
			if (!s_instance->isEnabled())
			{
				CALL_ORIGINAL(hBattleGameTime_Tick, _this);
				return;
			}

			const float scale = std::clamp(s_instance->m_scale, 0.1f, 10.0f);

			const float oldLogicSecPerFrame = _this->LogicSecondPerFrame();
			const int32_t oldCurrentFrame = _this->CurrentFrame();

			_this->LogicSecondPerFrame(oldLogicSecPerFrame / scale);

			CALL_ORIGINAL(hBattleGameTime_Tick, _this);

			_this->LogicSecondPerFrame(oldLogicSecPerFrame);

			if (_this->CurrentFrame() < oldCurrentFrame)
			{
				_this->CurrentFrame(oldCurrentFrame);
			}
		}

		float m_scale = 1.0f;
		Config::Field<float> m_scaleField;
	};
}
