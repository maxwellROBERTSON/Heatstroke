#pragma once

#include "Event.hpp"

namespace Engine
{
	class KeyEvent : public Event
	{
	public:
		uint16_t GetKeyCode() const { return keyCode; }
		virtual int GetEventCategoryFlags() const override {
			return EventCategoryKeyboard | EventCategoryInput;
		}
	protected:
		KeyEvent(const uint16_t key) : keyCode(key) {}
		uint16_t keyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const uint16_t key, bool isRepeated = false) :
			KeyEvent(key), repeatedKey(isRepeated) {
		}

		bool IsRepeat() const { return repeatedKey; }
		static EventType GetStaticType() {
			return EventType::KeyPressed;
		} virtual EventType GetEventType() const override {
			return GetStaticType();
		} virtual const char* GetEventName() const override {
			return "KeyPressedEvent";
		}
	private:
		bool repeatedKey;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const uint16_t key) :
			KeyEvent(key) {
		}

		static EventType GetStaticType() {
			return EventType::KeyReleased;
		} virtual EventType GetEventType() const override {
			return GetStaticType();
		} virtual const char* GetEventName() const override {
			return "KeyReleasedEvent";
		}
	};
}
