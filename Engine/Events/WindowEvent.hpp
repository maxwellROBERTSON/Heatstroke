#pragma once

#include "Event.hpp"

namespace Engine
{
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		static EventType GetStaticType() {
			return EventType::WindowClose;
		} virtual EventType GetEventType() const override {
			return GetStaticType();
		} virtual const char* GetEventName() const override {
			return "WindowCloseEvent";
		}
		virtual int GetEventCategoryFlags() const override {
			return EventCategoryWindow;
		}
	};
}