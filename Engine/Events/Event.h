#pragma once

#include <cstdint>


namespace Engine

{
	enum class EventType
	{
		None = 0,
		KeyPressed, KeyReleased, // KeyRepeat,
		MouseButtonPressed, MouseButtonReleased, MouseScrolled, MouseMoved,
		// TODO: add controller events
		WindowClose,
		GameMode

	};

	enum EventCategory
	{
		None = 0,
		EventCategoryInput = 1 << 0,
		EventCategoryKeyboard = 1 << 1,
		EventCategoryMouse = 1 << 2,
		EventCategoryMouseButton = 1 << 3,
		EventCategoryWindow = 1 << 4
	};

	class Event
	{
	public:

		bool isHandled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetEventName() const = 0;
		virtual int GetEventCategoryFlags() const = 0;

		bool IsinCategory(EventCategory category)
		{
			return GetEventCategoryFlags() & category;
		}
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& e) : event(e) {}
		template <typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (event.GetEventType() == T::GetStaticType())
			{
				event.isHandled = func(static_cast<T&>(event));
				return true;
			}
			return false;
		}
	private:
		Event& event;
	};
}