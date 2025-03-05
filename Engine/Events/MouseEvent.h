#pragma once

#include "Event.h"

namespace Engine
{
	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y) : mMouseX(x), mMouseY(y) {}

		float GetX() const { return mMouseX; }
		float GetY() const { return mMouseY; }

		static EventType GetStaticType() {
			return EventType::MouseMoved;
		} virtual EventType GetEventType() const override {
			return GetStaticType();
		} virtual const char* GetEventName() const override {
			return "MouseMovedEvent";
		}
		virtual int GetEventCategoryFlags() const override {
			return EventCategoryMouse | EventCategoryInput;
		}

	private:
		float mMouseX, mMouseY;
	};

	class MouseButtonEvent : public Event
	{
	public:
		uint16_t GetMouseButton() const { return mButton; }
		virtual int GetEventCategoryFlags() const override {
			return EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton;
		}
	protected:
		MouseButtonEvent(const uint16_t button) : mButton(button) {}
		uint16_t mButton;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const uint16_t button) : MouseButtonEvent(button) {}
		static EventType GetStaticType() {
			return EventType::MouseButtonPressed;
		} virtual EventType GetEventType() const override {
			return GetStaticType();
		} virtual const char* GetEventName() const override {
			return "MouseButtonPressedEvent";
		}
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const uint16_t button) : MouseButtonEvent(button) {}
		static EventType GetStaticType() {
			return EventType::MouseButtonReleased;
		} virtual EventType GetEventType() const override {
			return GetStaticType();
		} virtual const char* GetEventName() const override {
			return "MouseButtonReleasedEvent";
		}
	};
}