#include "EventListener.h"

namespace LandingGear {
  
  EventData::EventData() {}
  EventData::EventData(EventType kind): kind(kind) {};

  EventData::EventData(EventType kind, std::string data): kind(kind), data(data) {};


  /**
   * @brief Gets the EventData data depending on what event it is.
   * 
   * @return std::string The data chunk
   */
  std::string EventData::toString() {
    switch (kind) {
      case EventType::CHUNK:
        return data;
    }

    return "";
  }

  EventListener::EventListener() {
    events = {};
  }

  /**
   * @brief Adds an event to the list of available events.
   * 
   * @param event The event to add
   */
  void EventListener::addEvent(std::string event) {
    events[event] = std::vector<EventCallback>();
  }

  /**
   * @brief Emits an event to all listeners with specified data.
   * 
   * @param event The event type
   * @param data The data to be sent
   */
  void EventListener::emit(std::string event, EventData data) {
    for (auto cb : events[event]) {
      cb(data);
    }
  }

  /**
   * @brief Turns off a listener or stops listening to an event.
   * 
   * @param event The event to stop listening to
   * @param cb The listener cb to remove
   */
  void EventListener::off(std::string event, EventCallback cb) {
    int index = -1;

    for (int i = 0; i < events[event].size(); i++) {
      EventCallback cbOld = events[event][i];

      if (cbOld.target<EventCallback>() == cb.target<EventCallback>()) {
        index = i;
        break;
      }
    }

    if (index < 0) {
      return;
    }

    events[event].erase(events[event].begin() + index);
  }

  /**
   * @brief Listens to an event only once. Similar to '.on' but calls '.off' once hit.
   * 
   * @param event The event to listen to
   * @param cb The listener cb that gets called when event is emitted.
   */
  void EventListener::once(std::string event, EventCallback cb) {
    EventCallback cb2 = [=](EventData e) {
      cb(e);

      off(event, cb2);
    };
    events[event].push_back(cb2);
  }

  /**
   * @brief Listens to an event.
   * 
   * @param event The event to listen to
   * @param cb The listener cb that gets called when event is emitted.
   */
  void EventListener::on(std::string event, EventCallback cb) {
    events[event].push_back(cb);
  }

}; // namespace LandingGear
