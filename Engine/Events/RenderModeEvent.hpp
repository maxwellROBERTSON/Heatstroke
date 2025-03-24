#pragma once

#include "Event.hpp"
#include "../Core/RenderMode.hpp"

namespace Engine
{
	class RenderModeEvent : public Event
	{
	public:
		virtual int GetEventCategoryFlags() const override {
			return EventCategoryRenderMode;
		}
	protected:
		RenderModeEvent() = default;
	};

	class RenderModeToggleEvent : public RenderModeEvent
	{
	public:
		RenderModeToggleEvent(Engine::RenderMode mode, bool on) : mode(mode), on(on) {}

		Engine::RenderMode GetRenderMode() { return mode; }
		bool IsOn() { return on; }

		static EventType GetStaticType() {
			return EventType::RenderMode;
		} EventType GetEventType() const override {
			return GetStaticType();
		} const char* GetEventName() const override {
			return "RenderModeEvent";
		}
	private:
		Engine::RenderMode mode;
		bool on;
	};
}