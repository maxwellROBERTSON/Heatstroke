#pragma once

#include "Event.hpp"

namespace Engine
{
	class GameModeEvent : public Event
	{
	public:
		static EventType GetStaticType() {
			return EventType::GameMode;
		} virtual EventType GetEventType() const override {
			return GetStaticType();
		} virtual const char* GetEventName() const override {
			return "GameModeEvent";
		}
	};
}