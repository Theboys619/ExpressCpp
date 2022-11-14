/**
 * @file EventListener.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief An EventListener library. Contains classes and method definitions for an EventListener.
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace LandingGear {

  /**
   * @brief Event datatypes.
   * 
   */
  enum class EventType {
    CHUNK,
  };

  /**
   * @brief The EventData used in the callbacks for event listeners.
   * 
   */
  class EventData {
    private:
    std::string data;

    public:
    EventType kind;

    EventData();
    EventData(EventType kind);
    EventData(EventType kind, std::string data);

    std::string toString();
  };

  typedef std::function<void(EventData)> EventCallback; // The callback used in the EventListeners.

  /**
   * @brief Main class for an EventListener. Should be extended on another classes to allow emit events.
   * 
   */
  class EventListener {
    private:
    std::unordered_map<std::string, std::vector<EventCallback>> events;

    public:

    EventListener();

    void addEvent(std::string event);

    void emit(std::string event, EventData data);

    void off(std::string event, EventCallback cb);
    void once(std::string event, EventCallback cb);
    void on(std::string event, EventCallback cb);
  };

}; // namespace LandingGear

#endif